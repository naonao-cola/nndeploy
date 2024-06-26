#include "nndeploy/op/model_desc.h"

namespace nndeploy {

namespace op {

// TODO:
// 变量命名为：小写 + 下划线，类或者结构体重成员命名为 小写 + 下划线
// + 并且下划线结尾
//
std::shared_ptr<Expr> ModelDesc::MakeConv2d(
    std::shared_ptr<Expr> input,
    std::shared_ptr<Expr> weight,  // 卷积还有bias呀
    std::shared_ptr<Conv2dParam> param, std::string name) {
  auto descAndParam = std::make_shared<OpDescAndParam>();
  descAndParam->op_desc_ = OpDesc(name, kOpTypeConv2d);
  descAndParam->op_desc_.inputs_ = {input->getOutputName()[0],
                                    weight->getOutputName()[0]};
  descAndParam->op_desc_.outputs_ = {
      name +
      ".output"};  // TODO：假如想让输出保持模型结构一致，是不是需要新增一个参数呢？
  descAndParam->op_param_ = param;

  auto convExpr = std::make_shared<Expr>(descAndParam);

  op_desc_params_.push_back(descAndParam);

  return convExpr;
}

std::shared_ptr<Expr> ModelDesc::MakeRelu(std::shared_ptr<Expr> input,
                                          std::string name) {
  auto descAndParam = std::make_shared<OpDescAndParam>();
  descAndParam->op_desc_ = OpDesc(name, kOpTypeRelu);
  descAndParam->op_desc_.inputs_ = {input->getOutputName()};
  descAndParam->op_desc_.outputs_ = {name + ".output"};

  auto reluExpr = std::make_shared<Expr>(descAndParam);

  op_desc_params_.push_back(descAndParam);

  return reluExpr;
}

}  // namespace op
}  // namespace nndeploy