//
// Created by jinhai on 23-1-5.
//

#include <gtest/gtest.h>
#include "base_test.h"
#include "common/column_vector/column_vector.h"
#include "common/types/value.h"
#include "main/logger.h"
#include "main/stats/global_resource_usage.h"
#include "main/infinity.h"
#include "common/types/info/varchar_info.h"
#include "storage/catalog.h"
#include "function/aggregate/avg.h"
#include "function/aggregate_function_set.h"
#include "expression/column_expression.h"

class AvgFunctionTest : public BaseTest {
    void
    SetUp() override {
        infinity::GlobalResourceUsage::Init();
        infinity::Infinity::instance().Init();
    }

    void
    TearDown() override {
        infinity::Infinity::instance().UnInit();
        EXPECT_EQ(infinity::GlobalResourceUsage::GetObjectCount(), 0);
        EXPECT_EQ(infinity::GlobalResourceUsage::GetRawMemoryCount(), 0);
        infinity::GlobalResourceUsage::UnInit();
    }
};

TEST_F(AvgFunctionTest, avg_func) {
    using namespace infinity;

    UniquePtr<Catalog> catalog_ptr = MakeUnique<Catalog>();

    RegisterAvgFunction(catalog_ptr);

    String op = "avg";
    SharedPtr<FunctionSet> function_set = catalog_ptr->GetFunctionSetByName(op);
    EXPECT_EQ(function_set->type_, FunctionType::kAggregate);
    SharedPtr<AggregateFunctionSet> aggregate_function_set = std::static_pointer_cast<AggregateFunctionSet>(function_set);
    {
        DataType data_type(LogicalType::kTinyInt);
        SharedPtr<ColumnExpression> col_expr_ptr = MakeShared<ColumnExpression>(data_type,
                                                                                "t1",
                                                                                "c1",
                                                                                0,
                                                                                0);

        AggregateFunction func = aggregate_function_set->GetMostMatchFunction(col_expr_ptr);
        EXPECT_STREQ("AVG(TinyInt)->Double", func.ToString().c_str());

        Vector<DataType> column_types;
        column_types.emplace_back(data_type);

        SizeT row_count = DEFAULT_VECTOR_SIZE;

        DataBlock data_block;
        data_block.Init(column_types);

        double sum = 0;
        for (SizeT i = 0; i < row_count; ++i) {
            if(i % 2 == 0) {
                data_block.AppendValue(0, Value::MakeTinyInt(50));
                sum += 50;
            } else {
                data_block.AppendValue(0, Value::MakeTinyInt(100));
                sum += 100;
            }
        }
        data_block.Finalize();

        for (SizeT i = 0; i < row_count; ++i) {
            Value v = data_block.GetValue(0, i);
            EXPECT_EQ(v.type_.type(), LogicalType::kTinyInt);
            if(i % 2 == 0) {
                EXPECT_EQ(v.value_.tiny_int, static_cast<i8>(50));
            } else {
                EXPECT_EQ(v.value_.tiny_int, static_cast<i8>(100));
            }
        }


        func.init_func_(func.GetState());
        func.update_func_(func.GetState(), data_block.column_vectors[0]);
        DoubleT result;
        result = *(DoubleT*)func.finalize_func_(func.GetState());

        EXPECT_FLOAT_EQ(result, sum / row_count);
    }

    {
        DataType data_type(LogicalType::kSmallInt);
        SharedPtr<ColumnExpression> col_expr_ptr = MakeShared<ColumnExpression>(data_type,
                                                                                "t1",
                                                                                "c1",
                                                                                0,
                                                                                0);

        AggregateFunction func = aggregate_function_set->GetMostMatchFunction(col_expr_ptr);
        EXPECT_STREQ("AVG(SmallInt)->Double", func.ToString().c_str());

        Vector<DataType> column_types;
        column_types.emplace_back(data_type);

        SizeT row_count = DEFAULT_VECTOR_SIZE;

        DataBlock data_block;
        data_block.Init(column_types);

        double sum = 0;
        for (SizeT i = 0; i < row_count; ++i) {
            data_block.AppendValue(0, Value::MakeSmallInt(static_cast<SmallIntT>(i)));
            sum += static_cast<double>(i);
        }
        data_block.Finalize();

        for (SizeT i = 0; i < row_count; ++i) {
            Value v = data_block.GetValue(0, i);
            EXPECT_EQ(v.type_.type(), LogicalType::kSmallInt);
            EXPECT_EQ(v.value_.small_int, static_cast<SmallIntT>(i));
        }


        func.init_func_(func.GetState());
        func.update_func_(func.GetState(), data_block.column_vectors[0]);
        DoubleT result;
        result = *(DoubleT*)func.finalize_func_(func.GetState());

        EXPECT_FLOAT_EQ(result, sum / row_count);
    }

    {
        DataType data_type(LogicalType::kInteger);
        SharedPtr<ColumnExpression> col_expr_ptr = MakeShared<ColumnExpression>(data_type,
                                                                                "t1",
                                                                                "c1",
                                                                                0,
                                                                                0);

        AggregateFunction func = aggregate_function_set->GetMostMatchFunction(col_expr_ptr);
        EXPECT_STREQ("AVG(Integer)->Double", func.ToString().c_str());

        Vector<DataType> column_types;
        column_types.emplace_back(data_type);

        SizeT row_count = DEFAULT_VECTOR_SIZE;

        DataBlock data_block;
        data_block.Init(column_types);

        double sum = 0;
        for (SizeT i = 0; i < row_count; ++i) {
            data_block.AppendValue(0, Value::MakeInt(static_cast<IntegerT>(2 * i)));
            sum += static_cast<double>(2 * i);
        }
        data_block.Finalize();

        for (SizeT i = 0; i < row_count; ++i) {
            Value v = data_block.GetValue(0, i);
            EXPECT_EQ(v.type_.type(), LogicalType::kInteger);
            EXPECT_EQ(v.value_.integer, static_cast<IntegerT>(2 * i));
        }


        func.init_func_(func.GetState());
        func.update_func_(func.GetState(), data_block.column_vectors[0]);
        DoubleT result;
        result = *(DoubleT*)func.finalize_func_(func.GetState());

        EXPECT_FLOAT_EQ(result, sum / row_count);
    }

    {
        DataType data_type(LogicalType::kBigInt);
        SharedPtr<ColumnExpression> col_expr_ptr = MakeShared<ColumnExpression>(data_type,
                                                                                "t1",
                                                                                "c1",
                                                                                0,
                                                                                0);

        AggregateFunction func = aggregate_function_set->GetMostMatchFunction(col_expr_ptr);
        EXPECT_STREQ("AVG(BigInt)->Double", func.ToString().c_str());

        Vector<DataType> column_types;
        column_types.emplace_back(data_type);

        SizeT row_count = DEFAULT_VECTOR_SIZE;

        DataBlock data_block;
        data_block.Init(column_types);

        double sum = 0;
        for (SizeT i = 0; i < row_count; ++i) {
            data_block.AppendValue(0, Value::MakeBigInt(static_cast<BigIntT>(2 * i)));
            sum += static_cast<double>(2 * i);
        }
        data_block.Finalize();

        for (SizeT i = 0; i < row_count; ++i) {
            Value v = data_block.GetValue(0, i);
            EXPECT_EQ(v.type_.type(), LogicalType::kBigInt);
            EXPECT_EQ(v.value_.big_int, static_cast<BigIntT>(2 * i));
        }


        func.init_func_(func.GetState());
        func.update_func_(func.GetState(), data_block.column_vectors[0]);
        DoubleT result;
        result = *(DoubleT*)func.finalize_func_(func.GetState());

        EXPECT_FLOAT_EQ(result, sum / row_count);
    }

    {
        DataType data_type(LogicalType::kFloat);
        SharedPtr<ColumnExpression> col_expr_ptr = MakeShared<ColumnExpression>(data_type,
                                                                                "t1",
                                                                                "c1",
                                                                                0,
                                                                                0);

        AggregateFunction func = aggregate_function_set->GetMostMatchFunction(col_expr_ptr);
        EXPECT_STREQ("AVG(Float)->Double", func.ToString().c_str());

        Vector<DataType> column_types;
        column_types.emplace_back(data_type);

        SizeT row_count = DEFAULT_VECTOR_SIZE;

        DataBlock data_block;
        data_block.Init(column_types);

        double sum = 0;
        for (SizeT i = 0; i < row_count; ++i) {
            data_block.AppendValue(0, Value::MakeFloat(static_cast<FloatT>(2 * i)));
            sum += static_cast<double>(2 * i);
        }
        data_block.Finalize();

        for (SizeT i = 0; i < row_count; ++i) {
            Value v = data_block.GetValue(0, i);
            EXPECT_EQ(v.type_.type(), LogicalType::kFloat);
            EXPECT_FLOAT_EQ(v.value_.float32, static_cast<FloatT>(2 * i));
        }


        func.init_func_(func.GetState());
        func.update_func_(func.GetState(), data_block.column_vectors[0]);
        DoubleT result;
        result = *(DoubleT*)func.finalize_func_(func.GetState());

        EXPECT_FLOAT_EQ(result, sum / row_count);
    }

    {
        DataType data_type(LogicalType::kDouble);
        SharedPtr<ColumnExpression> col_expr_ptr = MakeShared<ColumnExpression>(data_type,
                                                                                "t1",
                                                                                "c1",
                                                                                0,
                                                                                0);

        AggregateFunction func = aggregate_function_set->GetMostMatchFunction(col_expr_ptr);
        EXPECT_STREQ("AVG(Double)->Double", func.ToString().c_str());

        Vector<DataType> column_types;
        column_types.emplace_back(data_type);

        SizeT row_count = DEFAULT_VECTOR_SIZE;

        DataBlock data_block;
        data_block.Init(column_types);

        double sum = 0;
        for (SizeT i = 0; i < row_count; ++i) {
            data_block.AppendValue(0, Value::MakeDouble(static_cast<DoubleT>(2 * i)));
            sum += static_cast<double>(2 * i);
        }
        data_block.Finalize();

        for (SizeT i = 0; i < row_count; ++i) {
            Value v = data_block.GetValue(0, i);
            EXPECT_EQ(v.type_.type(), LogicalType::kDouble);
            EXPECT_FLOAT_EQ(v.value_.float64, static_cast<DoubleT>(2 * i));
        }

        func.init_func_(func.GetState());
        func.update_func_(func.GetState(), data_block.column_vectors[0]);
        DoubleT result;
        result = *(DoubleT*)func.finalize_func_(func.GetState());

        EXPECT_FLOAT_EQ(result, sum / row_count);
    }

    {
        DataType data_type(LogicalType::kHugeInt);
        SharedPtr<ColumnExpression> col_expr_ptr = MakeShared<ColumnExpression>(data_type,
                                                                                "t1",
                                                                                "c1",
                                                                                0,
                                                                                0);

        AggregateFunction func = aggregate_function_set->GetMostMatchFunction(col_expr_ptr);

        // Need to recheck how to cast the type.
        EXPECT_STRNE("AVG(HugeInt)->Double", func.ToString().c_str());
    }

    {
        DataType data_type(LogicalType::kBoolean);
        SharedPtr<ColumnExpression> col_expr_ptr = MakeShared<ColumnExpression>(data_type,
                                                                                "t1",
                                                                                "c1",
                                                                                0,
                                                                                0);

        EXPECT_THROW(aggregate_function_set->GetMostMatchFunction(col_expr_ptr), PlannerException);
    }
}