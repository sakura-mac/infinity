//
// Created by JinHai on 2022/7/28.
//

#pragma once

#include "executor/physical_operator.h"

namespace infinity {

class PhysicalPreparedPlan : public PhysicalOperator {
public:
    explicit PhysicalPreparedPlan(uint64_t id)
        : PhysicalOperator(PhysicalOperatorType::kPreparedPlan, nullptr, nullptr, id) {}
    ~PhysicalPreparedPlan() override = default;

    void
    Init() override;

    void
    Execute(std::shared_ptr<QueryContext>& query_context) override;
};

}
