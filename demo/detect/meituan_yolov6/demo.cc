#include "nndeploy/base/glic_stl_include.h"
#include "nndeploy/base/time_profiler.h"
#include "nndeploy/device/device.h"
#include "nndeploy/model/detect/meituan_yolov6/meituan_yolov6.h"
#include "nndeploy/model/task.h"

using namespace nndeploy;

cv::Mat draw_box(cv::Mat &cv_mat, model::DetectResult &result) {
  // float w_ratio = float(cv_mat.cols) / float(640);
  // float h_ratio = float(cv_mat.rows) / float(640);
  float w_ratio = float(cv_mat.cols);
  float h_ratio = float(cv_mat.rows);
  const int CNUM = 80;
  cv::RNG rng(0xFFFFFFFF);
  cv::Scalar_<int> randColor[CNUM];
  for (int i = 0; i < CNUM; i++)
    rng.fill(randColor[i], cv::RNG::UNIFORM, 0, 256);
  int i = -1;
  for (auto bbox : result.bboxs_) {
    std::array<float, 4> box;
    box[0] = bbox.bbox_[0];  // 640.0;
    box[2] = bbox.bbox_[2];  // 640.0;
    box[1] = bbox.bbox_[1];  // 640.0;
    box[3] = bbox.bbox_[3];  // 640.0;
    NNDEPLOY_LOGE("box[0]:%f, box[2]:%f, box[1]:%f, box[3]:%f\n", box[0],
                  box[2], box[1], box[3]);
    box[0] *= w_ratio;
    box[2] *= w_ratio;
    box[1] *= h_ratio;
    box[3] *= h_ratio;
    int width = box[2] - box[0];
    int height = box[3] - box[1];
    int id = bbox.label_id_;
    cv::Point p = cv::Point(box[0], box[1]);
    cv::Rect rect = cv::Rect(box[0], box[1], width, height);
    cv::rectangle(cv_mat, rect, randColor[id]);
    std::string text = " ID:" + std::to_string(id);
    cv::putText(cv_mat, text, p, cv::FONT_HERSHEY_PLAIN, 1, randColor[id]);
  }
  return cv_mat;
}

int main(int argc, char *argv[]) {
  std::string name = "meituan_yolov6";
  base::InferenceType inference_type = base::kInferenceTypeOpenVino;
  // base::InferenceType inference_type = base::kInferenceTypeOnnxRuntime;
  // base::InferenceType inference_type = base::kInferenceTypeTensorRt;
  base::DeviceType device_type = device::getDefaultHostDeviceType();
  // base::DeviceType device_type(base::kDeviceTypeCodeCuda);
  bool is_path = true;
  std::vector<std::string> model_value;
  model_value.push_back(
      "/home/always/github/public/nndeploy/model_zoo/model/meituan_yolov6/"
      "yolov6m.onnx");
  model::Packet input("detr_in");
  model::Packet output("detr_out");
  model::Pipeline *pipeline = model::creatMeituanYolov6Pipeline(
      name, inference_type, device_type, &input, &output, true, model_value);
  if (pipeline == nullptr) {
    NNDEPLOY_LOGE("pipeline is nullptr");
    return -1;
  }

  NNDEPLOY_TIME_POINT_START("pipeline->init()");
  pipeline->init();
  NNDEPLOY_TIME_POINT_END("pipeline->init()");

  cv::Mat input_mat =
      cv::imread("/home/always/github/YOLOv6/deploy/ONNX/OpenCV/sample.jpg");
  input.set(input_mat);
  model::DetectResult result;
  output.set(result);

  NNDEPLOY_TIME_POINT_START("pipeline->run()");
  for (int i = 0; i < 1; ++i) {
    pipeline->run();
  }
  NNDEPLOY_TIME_POINT_END("pipeline->run()");

  draw_box(input_mat, result);
  cv::imwrite(
      "/home/always/github/public/nndeploy/model_zoo/model/meituan_yolov6/"
      "sample.jpg",
      input_mat);

  std::ofstream oss;
  oss.open("meituan_yolov6.dot", std::ios::out);
  pipeline->dump(oss);

  pipeline->deinit();

  delete pipeline;

  NNDEPLOY_TIME_PROFILER_PRINT();

  printf("hello world!\n");
  return 0;
}