//
// Created by JinHai on 2022/8/11.
//

#pragma once

#include "bind_context.h"

#include "expression/base_expression.h"
#include "expression/subquery_expression.h"

#include <memory>

namespace infinity {
//class PlanBuilder;
struct ExprBindResult {
    std::shared_ptr<BaseExpression> bound_expression_ptr_;
    std::string err_msg_;

    inline bool IsOk() { return err_msg_.empty(); }
};


class ExpressionBinder {
public:
    explicit ExpressionBinder(std::shared_ptr<QueryContext>& query_context) : query_context_(query_context) {}

    virtual ~ExpressionBinder() = default;

    // Bind expression entry
    virtual std::shared_ptr<BaseExpression>
    BuildExpression(const hsql::Expr &expr, const std::shared_ptr<BindContext>& bind_context_ptr);

    virtual std::shared_ptr<BaseExpression>
    BuildValueExpr(const hsql::Expr &expr, const std::shared_ptr<BindContext>& bind_context_ptr);

    // Bind column reference expression also include correlated column reference.
    virtual std::shared_ptr<BaseExpression>
    BuildColExpr(const hsql::Expr &expr, const std::shared_ptr<BindContext>& bind_context_ptr);

    virtual std::shared_ptr<BaseExpression>
    BuildFuncExpr(const hsql::Expr &expr, const std::shared_ptr<BindContext>& bind_context_ptr);

    virtual std::shared_ptr<BaseExpression>
    BuildOperatorExpr(const hsql::Expr &expr, const std::shared_ptr<BindContext>& bind_context_ptr);

    virtual std::shared_ptr<BaseExpression>
    BuildCastExpr(const hsql::Expr &expr, const std::shared_ptr<BindContext>& bind_context_ptr);

    virtual std::shared_ptr<BaseExpression>
    BuildCaseExpr(const hsql::Expr &expr, const std::shared_ptr<BindContext>& bind_context_ptr);

    virtual std::shared_ptr<BaseExpression>
    BuildBinaryScalarExpr(const std::string& op,
                          const hsql::Expr* left,
                          const hsql::Expr* right,
                          const std::shared_ptr<BindContext>& bind_context_ptr);

    virtual std::shared_ptr<BaseExpression>
    BuildUnaryScalarExpr(const std::string& op,
                         const hsql::Expr* expr,
                         const std::shared_ptr<BindContext>& bind_context_ptr);

    // Bind subquery expression.
    virtual std::shared_ptr<SubqueryExpression>
    BuildSubquery(const hsql::SelectStatement& select,
                  const std::shared_ptr<BindContext>& bind_context_ptr, SubqueryType subquery_type);

//    // Bind window function.
//    virtual std::shared_ptr<BaseExpression>
//    BuildWindow(const hsql::Expr &expr, const std::shared_ptr<BindContext>& bind_context_ptr);

//    std::shared_ptr<PlanBuilder> plan_builder_ptr_;
protected:
    std::shared_ptr<QueryContext>& query_context_;

};

}