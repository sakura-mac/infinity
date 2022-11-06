//
// Created by JinHai on 2022/7/28.
//

#pragma once

//#include <utility>

#include "expression/base_expression.h"
#include "executor/physical_operator.h"
#include "executor/expression/expression_executor.h"

namespace infinity {

class PhysicalProject : public PhysicalOperator {
public:
    explicit PhysicalProject(uint64_t id, std::shared_ptr<PhysicalOperator> left, std::vector<std::shared_ptr<BaseExpression>> expressions)
        : PhysicalOperator(PhysicalOperatorType::kProjection, std::move(left), nullptr, id),
          expressions_(std::move(expressions))
          {}

    ~PhysicalProject() override = default;

    void
    Init() override;

    void
    Execute(std::shared_ptr<QueryContext>& query_context) override;

    std::vector<std::shared_ptr<BaseExpression>> expressions_;

    ExpressionExecutor executor;
};

}

