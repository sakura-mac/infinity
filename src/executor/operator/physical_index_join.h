//
// Created by JinHai on 2022/7/28.
//

#pragma once

#include "executor/physical_operator.h"

namespace infinity {

class PhysicalIndexJoin : public PhysicalOperator {
public:
    explicit PhysicalIndexJoin(uint64_t id)
        : PhysicalOperator(PhysicalOperatorType::kJoinIndex, nullptr, nullptr, id) {}
    ~PhysicalIndexJoin() override = default;

    void
    Init() override;

    void
    Execute(std::shared_ptr<QueryContext>& query_context) override;
};

}