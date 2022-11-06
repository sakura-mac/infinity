//
// Created by JinHai on 2022/7/23.
//

#include "logical_create_table.h"

#include <sstream>

namespace infinity {

std::string LogicalCreateTable::ToString(int64_t& space) {
    std::stringstream ss;
    std::string arrow_str;
    if(space > 3) {
        space -= 4;
        arrow_str = "->  ";
    }
    ss << std::string(space, ' ') << arrow_str << "Create Table: " << *schema_name_ << "." << table_definitions_->name();
    space += arrow_str.size();
    return ss.str();
}

}

