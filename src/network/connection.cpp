//
// Created by JinHai on 2022/7/20.
//

#include "connection.h"
#include "main/query_context.h"
#include "main/logger.h"

#include <iostream>

namespace infinity {

Connection::Connection(boost::asio::io_service& io_service)
    : socket_(std::make_shared<boost::asio::ip::tcp::socket>(io_service)),
      pg_handler_(std::make_shared<PGProtocolHandler>(socket())),
      session_ptr_(std::make_shared<Session>()){}

void
Connection::Run() {
    // Disable Nagle's algorithm to reduce TCP latency, but will reduce the throughput.
    socket_->set_option(boost::asio::ip::tcp::no_delay(true));
    HandleConnection();
    while(!terminate_connection_) {
        try {
            HandleRequest();
        } catch (const std::exception& e) {
            std::map<PGMessageType, std::string> error_message_map;
            error_message_map[PGMessageType::kHumanReadableError] = e.what();
            LOG_ERROR(e.what());
            pg_handler_->send_error_response(error_message_map);
            pg_handler_->send_ready_for_query();
        }

    }
}

void
Connection::HandleConnection() {
    const auto body_length = pg_handler_->read_startup_header();

    pg_handler_->read_startup_body(body_length);
    pg_handler_->send_authentication();
    pg_handler_->send_parameter("server_version", "14");
    pg_handler_->send_parameter("server_encoding", "UTF8");
    pg_handler_->send_parameter("client_encoding", "UTF8");
    pg_handler_->send_parameter("DateStyle", "IOS, DMY");
    pg_handler_->send_ready_for_query();
}

void
Connection::HandleRequest() {
    const auto cmd_type = pg_handler_->read_command_type();

    std::shared_ptr<QueryContext> query_context_ptr
        = std::make_shared<QueryContext>(session_ptr_, session_ptr_->transaction());
    query_context_ptr->set_current_schema(session_ptr_->current_schema());

    switch (cmd_type) {
        case PGMessageType::kBindCommand: {
            LOG_DEBUG("BindCommand");
            break;
        }
        case PGMessageType::kDescribeCommand: {
            LOG_DEBUG("DescribeCommand");
            break;
        }
        case PGMessageType::kExecuteCommand: {
            LOG_DEBUG("ExecuteCommand");
            break;
        }
        case PGMessageType::kParseCommand: {
            LOG_DEBUG("ParseCommand");
            break;
        }
        case PGMessageType::kSimpleQueryCommand: {
            HandlerSimpleQuery(query_context_ptr);
            break;
        }
        case PGMessageType::kSyncCommand: {
            LOG_DEBUG("SyncCommand");
            break;
        }
        case PGMessageType::kTerminateCommand: {
            terminate_connection_ = true;
            break;
        }
        default: {
            NetworkAssert(false, "Unknown command type");
        }
    }
}

void
Connection::HandlerSimpleQuery(std::shared_ptr<QueryContext>& query_context) {
    const std::string& query = pg_handler_->read_command_body();
    LOG_DEBUG("Query: {}", query);

    // Start to execute the query.
    QueryResult result = query_context->Query(query);

    // Response to the result message to client
    if(result.result_ == nullptr) {
        std::map<PGMessageType, std::string> error_message_map;
        std::string response_message = "Error: " + query;
        error_message_map[PGMessageType::kHumanReadableError] = response_message;
        pg_handler_->send_error_response(error_message_map);
    } else {
        // Have result
        SendTableDescription(result.result_);
        SendQueryResponse(result.result_);
        uint64_t row_count = result.result_->row_count();
        SendComplete(result.root_operator_type_, row_count);
    }

    pg_handler_->send_ready_for_query();

}

void
Connection::SendTableDescription(const std::shared_ptr<Table>& result_table) {
    uint32_t column_name_length_sum = 0;
    for(auto& column: result_table->table_def()->columns()) {
        column_name_length_sum += column.name().size();
    }

    // No output columns, no need to send table description, just return.
    if (column_name_length_sum == 0) return ;

    pg_handler_->SendDescriptionHeader(column_name_length_sum, result_table->table_def()->columns().size());

    for(auto& column: result_table->table_def()->columns()) {
        uint32_t object_id = 0;
        int16_t  object_width = 0;

        switch(column.logical_type().GetTypeId()) {
            case LogicalTypeId::kBoolean:
                object_id = 16;
                object_width = 1;
                break;
            case LogicalTypeId::kSmallInt:
                object_id = 21;
                object_width = 2;
                break;
            case LogicalTypeId::kTinyInt:
                object_id = 18; // char
                object_width = 1;
                break;
            case LogicalTypeId::kInteger:
                object_id = 23;
                object_width = 4;
                break;
            case LogicalTypeId::kBigInt:
                object_id = 20;
                object_width = 8;
                break;
            case LogicalTypeId::kFloat:
                object_id = 700;
                object_width = 4;
                break;
            case LogicalTypeId::kDouble:
                object_id = 701;
                object_width = 8;
                break;
            case LogicalTypeId::kText:
                object_id = 25;
                object_width = -1;
                break;
            case LogicalTypeId::kVarchar:
                object_id = 25;
                object_width = -1;
                break;
            case LogicalTypeId::kDate:
                object_id = 1082;
                object_width = 8;
                break;
            case LogicalTypeId::kTime:
                object_id = 1266;
                object_width = 12;
                break;
            case LogicalTypeId::kDecimal:
                object_id = 1700;
                object_width = -1;
                break;
            case LogicalTypeId::kInterval:
                object_id = 1186;
                object_width = 16;
                break;
            default:
                NetworkError("Not supported type.");
        }

        pg_handler_->SendDescription(column.name(), object_id, object_width);
    }
}

void
Connection::SendQueryResponse(const std::shared_ptr<Table>& result_table) {
    int64_t column_count = result_table->table_def()->column_count();
    auto values_as_strings = std::vector<std::optional<std::string>>(column_count);
    uint64_t block_count = result_table->block_count();
    for(uint64_t idx = 0; idx < block_count; ++ idx) {
        auto block = result_table->blocks()[idx];
        int64_t row_count = block->row_count();

        for(int64_t row_id = 0; row_id < row_count; ++ row_id) {
            int64_t string_length_sum = 0;

            // iterate each chunk of the block
            for(int64_t chunk_id = 0; chunk_id < column_count; ++ chunk_id) {
                auto& chunk = block->columns()[chunk_id];
                const std::string string_value = chunk.RowToString(row_id);
                values_as_strings[chunk_id] = string_value;
                string_length_sum += string_value.size();
            }
            pg_handler_->SendData(values_as_strings, string_length_sum);
        }
    }
}

void
Connection::SendComplete(LogicalNodeType root_operator_type, uint64_t row_count) {
    std::string message;
    switch (root_operator_type) {
        case LogicalNodeType::kInsert: {
            message = "INSERT 0 1";
            break;
        }
        case LogicalNodeType::kUpdate: {
            message = "UPDATE -1";
            break;
        }
        case LogicalNodeType::kDelete: {
            message = "DELETE -1";
            break;
        }
        default: {
            message = "SELECT " + std::to_string(row_count);
        }
    }

    pg_handler_->SendComplete(message);
}

}