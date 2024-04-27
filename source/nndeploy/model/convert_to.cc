
#include "nndeploy/model/convert_to.h"

namespace nndeploy {
namespace model {

base::Status ConvertTo::run() {
  base::Status status = base::kStatusCodeOk;

  device::Tensor *src = inputs_[0]->getTensor(this);
  device::TensorDesc src_desc = src->getDesc();
  int index = inputs_[0]->getIndex(this);
  if (src_desc.data_type_ == param_->dst_data_type_) {
    if (parallel_type_ != base::kParallelTypePipeline) {
      device::Tensor *dst = device::getShallowCopyTensor(src);
      outputs_[0]->set(dst, index);
    } else {
      device::Tensor *dst = device::getDeepCopyTensor(src);
      outputs_[0]->set(dst, index);
    }
  } else {
    device::TensorDesc dst_desc = src_desc;
    dst_desc.data_type_ = param_->dst_data_type_;
    device::Tensor *dst =
        outputs_[0]->create(src->getDevice(), dst_desc, index);
    status = src->convertTo(dst);
    NNDEPLOY_RETURN_ON_NEQ(status, base::kStatusCodeOk, "convertTo failed.");
  }

  return status;
}

}  // namespace model
}  // namespace nndeploy
