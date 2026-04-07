/*
 * Copyright (c) 2021, Alliance for Open Media. All rights reserved
 *
 * This source code is subject to the terms of the BSD 3-Clause Clear License
 * and the Alliance for Open Media Patent License 1.0. If the BSD 3-Clause Clear
 * License was not distributed with this source code in the LICENSE file, you
 * can obtain it at aomedia.org/license/software-license/bsd-3-c-c/.  If the
 * Alliance for Open Media Patent License 1.0 was not distributed with this
 * source code in the PATENTS file, you can obtain it at
 * aomedia.org/license/patent-license/.
 */
#include <iostream>
#include <memory>
#include <stdint.h>
#include "config/aom_config.h"
#if CONFIG_MSCNN
#include <assert.h>
#include <vector>
#include <string>
#include <iomanip>
#include <sstream>
#include <fstream>
#include <filesystem>
#include "av1/encoder/encoder.h"
#include "av1/common/av1_common_int.h"
#include "av1/common/enums.h"
#include "av1/common/guided_codebook.h"
#include "av1/common/nn_loopfilter.h"
#include "guided_adaptive_channel.h"
#include "av1/encoder/rd.h"
// #include <torch/script.h> // torch
#include <any>
#include <climits>
#include <float.h>
#include "aom_dsp/binary_codes_writer.h"
#include <chrono>
#include <cmath>
#include <cassert>
#include <algorithm>
#include <limits>
#include "tensorflow/lite/interpreter.h"
#include "tensorflow/lite/kernels/register.h"
#include "tensorflow/lite/model.h"
// #include "tensorflow/lite/optional_debug_tools.h"
// #include "tensorflow/lite/delegates/flex/delegate.h"
//  移除 Flex 委托的头文件（核心：彻底脱离 Flex 依赖）
// #include "Eigen/Dense"
#include <map>
#include <cstring>
#include <cstdlib>

using namespace std;
// //
// delete--------------------------------------------------------------------------
// class PredBuffer {
//  public:
//   static PredBuffer &getInstance(int y_width, int y_height, int
//   subsampling_x,
//                                  int subsampling_y, int border) {
//     static PredBuffer instance(y_width, y_height, subsampling_x,
//     subsampling_y,
//                                border);  // 单例
//     return instance;
//   }

//   // Get Set 方法
//   YV12_BUFFER_CONFIG *getPred() { return &_pred; }
//   struct buf_2d *getDstPred() { return &_dst_pred; }

//  private:
//   YV12_BUFFER_CONFIG _pred;
//   struct buf_2d _dst_pred;

//   PredBuffer(int y_width, int y_height, int subsampling_x, int subsampling_y,
//              int border) {
//     memset(&_pred, 0, sizeof(YV12_BUFFER_CONFIG));
//     aom_alloc_frame_buffer(&_pred, y_width, y_height, subsampling_x,
//                            subsampling_y, border, 0, false);

//     memset(&_dst_pred, 0, sizeof(struct buf_2d));
//   }

//   ~PredBuffer() { aom_free_frame_buffer(&_pred); }

//   PredBuffer(const PredBuffer &) = delete;
//   PredBuffer &operator=(const PredBuffer &) = delete;
// };

// struct buf_2d *dst_pred_ptr = nullptr;
// YV12_BUFFER_CONFIG *pred_ptr = nullptr;
// extern "C" YV12_BUFFER_CONFIG *getPred(int y_width, int y_height,
//                                        int subsampling_x, int subsampling_y,
//                                        int border) {
//   pred_ptr = PredBuffer::getInstance(y_width, y_height, subsampling_x,
//                                      subsampling_y, border)
//                  .getPred();
//   return pred_ptr;
// }

// extern "C" struct buf_2d *getDstPred(int y_width, int y_height,
//                                      int subsampling_x, int subsampling_y,
//                                      int border) {
//   dst_pred_ptr = PredBuffer::getInstance(y_width, y_height, subsampling_x,
//                                          subsampling_y, border)
//                      .getDstPred();
//   return dst_pred_ptr;
// }
// //
// delete--------------------------------------------------------------------------

// std::map<std::string, torch::jit::script::Module> module_map;
// std::map<std::string, torch::jit::script::Module>::iterator it;

// void generate_interm_guided_restoration(
//     uint16_t **ccso_data, int *ccso_data_strides, int bitdepth, int h, int w,
//     int c, uint16_t **dec_rec, int *dec_rec_strides, uint16_t **dec_res,
//     int *dec_res_strides, uint16_t **dec_bs, int *dec_bs_strides, int
//     q_index, const char *model_location, int is_intra_only,
//     std::vector<std::vector<std::vector<float>>> &interm) {
//   // const auto max_val = static_cast<float>((1 << bitdepth) - 1); // 应该 *
//   // 1020
//   const auto max_val = (255 << (bitdepth - 8));
//   // const auto max_val = 1020;
//   int qindex_adjust = q_index - 24 * (bitdepth - 8);

//   // load model
//   torch::Device device(torch::kCPU);
//   torch::jit::script::Module module;
//   try {
//     it = module_map.find(model_location);
//     if (it != module_map.end())
//       module = module_map[std::string(model_location)];
//     else {
//       module = torch::jit::load(model_location, device);
//       module_map[std::string(model_location)] = module;
//     }
//   } catch (const c10::Error &e) {
//     std::cerr << "error loading the model: " << model_location << "\n";
//     std::cerr << "Exception message: " << e.what() << "\n";
//     exit(0);
//   }
//   c10::InferenceMode guard;  // ?

//   // init dec_tensor base intra or inter
//   at::Tensor dec_tensor;

//   uint8_t input_channels = is_intra_only ? 4 : 5;
//   dec_tensor = torch::zeros({ 1, input_channels, h, w },
//                             torch::TensorOptions().dtype(torch::kInt16));

//   if (input_channels == 5) {
//     auto qp_tensor = torch::full({ 1, 1, h, w }, qindex_adjust,
//                                  torch::TensorOptions().dtype(torch::kInt16));
//     dec_tensor.slice(1, 4, 5) = qp_tensor;
//   }

//   // fill dec_tensor
//   uint16_t *plane_src = ccso_data[0];
//   uint16_t *plane_dblk_input = dec_rec[0];
//   uint16_t *plane_residue = dec_res[0];
//   uint16_t *plane_bs = dec_bs[0];
//   for (int h_idx = 0; h_idx < h; h_idx++) {
//     auto ccso_tensor = torch::from_blob(
//         plane_src, { w }, torch::TensorOptions().dtype(torch::kInt16));
//     auto rec_tensor = torch::from_blob(plane_dblk_input, { w },
//     torch::kInt16); auto res_tensor = torch::from_blob(
//         plane_residue, { w }, torch::kInt32);  // res = 反变换后 - 反变换前
//     auto pred_tensor =
//         rec_tensor.to(torch::kInt32) - res_tensor;  //
//         可能不是预测帧prediction
//     auto bs_tensor = torch::from_blob(
//         plane_bs, { w }, torch::TensorOptions().dtype(torch::kInt16));

//     dec_tensor.slice(1, 0, 1).slice(2, h_idx, h_idx + 1).slice(3, 0, w) =
//         ccso_tensor;
//     dec_tensor.slice(1, 1, 2).slice(2, h_idx, h_idx + 1).slice(3, 0, w) =
//         rec_tensor;
//     dec_tensor.slice(1, 2, 3).slice(2, h_idx, h_idx + 1).slice(3, 0, w) =
//         pred_tensor.to(torch::kInt16);
//     dec_tensor.slice(1, 3, 4).slice(2, h_idx, h_idx + 1).slice(3, 0, w) =
//         bs_tensor;

//     plane_src += ccso_data_strides[0];
//     plane_residue += dec_res_strides[0];
//     plane_dblk_input += dec_rec_strides[0];
//     plane_bs += dec_bs_strides[0];
//   }

//   at::Tensor normalized_tensor;
//   if (input_channels == 5) {
//     at::Tensor slice1 = dec_tensor.slice(1, 0, 4).to(torch::kFloat32) /
//     max_val; at::Tensor slice2 = dec_tensor.slice(1, 4,
//     5).to(torch::kFloat32); normalized_tensor = torch::cat({ slice1, slice2
//     }, 1).to(torch::kFloat32);
//   } else {
//     at::Tensor slice1 = dec_tensor.slice(1, 0, 4).to(torch::kFloat32) /
//     max_val; normalized_tensor = slice1;
//   }

//   module.eval();
//   torch::NoGradGuard no_grad;
//   torch::Tensor output;
//   try {
//     output = module.forward({ normalized_tensor }).toTensor();
//   } catch (const c10::Error &e) {
//     std::cerr << "Inference error: " << e.what() << std::endl;
//   }

//   for (int h_idx = 0; h_idx < h; h_idx++) {
//     for (int w_idx = 0; w_idx < w; w_idx++) {
//       switch (c) {
//         case 3:
//           interm[h_idx][w_idx][2] =
//               output[0][2][h_idx][w_idx].item<float>() * max_val;
//         case 2:
//           interm[h_idx][w_idx][1] =
//               output[0][1][h_idx][w_idx].item<float>() * max_val;
//         case 1:
//           interm[h_idx][w_idx][0] =
//               output[0][0][h_idx][w_idx].item<float>() * max_val;
//         default:;
//       }
//     }
//   }
// }

// TFLite 模型缓存（替代原来的 torch::jit::script::Module）
std::map<std::string, std::unique_ptr<tflite::Interpreter>>
    tflite_interpreter_map;
std::map<std::string, std::unique_ptr<tflite::FlatBufferModel>>
    tflite_model_map;

// 辅助函数：NCHW 转 NHWC
void nchw_to_nhwc(const float *nchw_data, float *nhwc_data, int batch,
                  int channels, int height, int width) {
  for (int b = 0; b < batch; ++b) {
    for (int h = 0; h < height; ++h) {
      for (int w = 0; w < width; ++w) {
        for (int c = 0; c < channels; ++c) {
          nhwc_data[b * height * width * channels + h * width * channels +
                    w * channels + c] =
              nchw_data[b * channels * height * width + c * height * width +
                        h * width + w];
        }
      }
    }
  }
}

// 辅助函数：NHWC 转 NCHW
void nhwc_to_nchw(const float *nhwc_data, float *nchw_data, int batch,
                  int channels, int height, int width) {
  for (int b = 0; b < batch; ++b) {
    for (int c = 0; c < channels; ++c) {
      for (int h = 0; h < height; ++h) {
        for (int w = 0; w < width; ++w) {
          nchw_data[b * channels * height * width + c * height * width +
                    h * width + w] =
              nhwc_data[b * height * width * channels + h * width * channels +
                        w * channels + c];
        }
      }
    }
  }
}

void generate_interm_guided_restoration_tflite(
    uint16_t **ccso_data, int *ccso_data_strides, int bitdepth, int h, int w,
    int c, uint16_t **dec_rec, int *dec_rec_strides, uint16_t **dec_res,
    int *dec_res_strides, uint16_t **dec_bs, int *dec_bs_strides, int q_index,
    const char *model_location, int is_intra_only,
    std::vector<std::vector<std::vector<float>>> &interm, int base_qindex) {
  // const auto max_val = static_cast<float>((1 << bitdepth) - 1);
  const auto max_val = static_cast<float>(255 << (bitdepth - 8));
  int qindex_adjust = q_index - 24 * (bitdepth - 8);
  (void)qindex_adjust;

  // 加载或获取 TFLite 模型
  tflite::Interpreter *interpreter = nullptr;
  std::string model_path_str(model_location);

  auto it = tflite_interpreter_map.find(model_path_str);
  if (it != tflite_interpreter_map.end()) {
    interpreter = it->second.get();
  } else {
    // 加载模型
    std::unique_ptr<tflite::FlatBufferModel> model =
        tflite::FlatBufferModel::BuildFromFile(model_location);

    if (!model) {
      std::cerr << "Error loading TFLite model: " << model_location << "\n";
      exit(1);
    }

    // 创建解释器
    tflite::ops::builtin::BuiltinOpResolver resolver;
    std::unique_ptr<tflite::Interpreter> new_interpreter;
    tflite::InterpreterBuilder builder(*model, resolver);

    if (builder(&new_interpreter) != kTfLiteOk) {
      std::cerr << "Error building TFLite interpreter: " << model_location
                << "\n";
      exit(1);
    }

    // 分配张量
    if (new_interpreter->AllocateTensors() != kTfLiteOk) {
      std::cerr << "Error allocating tensors: " << model_location << "\n";
      exit(1);
    }

    // 缓存模型和解释器
    tflite_model_map[model_path_str] = std::move(model);
    interpreter = new_interpreter.get();
    tflite_interpreter_map[model_path_str] = std::move(new_interpreter);
  }

  // 准备输入数据（NCHW 格式）
  uint8_t input_channels = is_intra_only ? 4 : 5;
  std::vector<int16_t> dec_tensor_data(1 * input_channels * h * w, 0);

  if (input_channels == 5) {
    // 填充 QP 通道
    for (int i = 0; i < h * w; ++i) {
      // dec_tensor_data[4 * h * w + i] = static_cast<int16_t>(qindex_adjust);
      dec_tensor_data[4 * h * w + i] = static_cast<int16_t>(base_qindex);
    }
  }

  // 填充其他通道
  uint16_t *plane_src = ccso_data[0];
  uint16_t *plane_dblk_input = dec_rec[0];
  uint16_t *plane_residue = dec_res[0];
  uint16_t *plane_bs = dec_bs[0];

  for (int h_idx = 0; h_idx < h; h_idx++) {
    for (int w_idx = 0; w_idx < w; w_idx++) {
      int idx = h_idx * w + w_idx;

      // CCSO
      dec_tensor_data[0 * h * w + idx] =
          static_cast<int16_t>(plane_src[h_idx * ccso_data_strides[0] + w_idx]);

      // REC
      dec_tensor_data[1 * h * w + idx] = static_cast<int16_t>(
          plane_dblk_input[h_idx * dec_rec_strides[0] + w_idx]);

      // PRED = REC - RES
      int32_t rec_val = static_cast<int32_t>(
          plane_dblk_input[h_idx * dec_rec_strides[0] + w_idx]);
      int32_t res_val = static_cast<int32_t>(
          plane_residue[h_idx * dec_res_strides[0] + w_idx]);
      dec_tensor_data[2 * h * w + idx] =
          static_cast<int16_t>(rec_val - res_val);

      // BS
      dec_tensor_data[3 * h * w + idx] =
          static_cast<int16_t>(plane_bs[h_idx * dec_bs_strides[0] + w_idx]);
    }
  }

  // 归一化输入（转换为 float32）
  // fprintf(stdout, "max_val: %f\n", max_val);
  std::vector<float> normalized_input(1 * input_channels * h * w);
  for (int ch = 0; ch < 4; ++ch) {
    for (int i = 0; i < h * w; ++i) {
      normalized_input[ch * h * w + i] =
          static_cast<float>(dec_tensor_data[ch * h * w + i]) / max_val;
    }
  }

  if (input_channels == 5) {
    for (int i = 0; i < h * w; ++i) {
      normalized_input[4 * h * w + i] =
          static_cast<float>(dec_tensor_data[4 * h * w + i]);
    }
  }

  // 获取输入输出张量信息
  auto *input_tensor = interpreter->input_tensor(0);
  auto *output_tensor = interpreter->output_tensor(0);

  // 检查并调整输入形状（如果需要）
  std::vector<int> current_input_shape = { input_tensor->dims->data[0],
                                           input_tensor->dims->data[1],
                                           input_tensor->dims->data[2],
                                           input_tensor->dims->data[3] };

  std::vector<int> target_input_shape = { 1, h, w, input_channels };  // NHWC

  bool need_resize = (current_input_shape != target_input_shape);

  if (need_resize) {
    interpreter->ResizeInputTensor(0, target_input_shape);
    interpreter->AllocateTensors();
    input_tensor = interpreter->input_tensor(0);
    output_tensor = interpreter->output_tensor(0);
  }

  // 将 NCHW 格式的输入转换为 NHWC 格式
  std::vector<float> nhwc_input(1 * h * w * input_channels);
  nchw_to_nhwc(normalized_input.data(), nhwc_input.data(), 1, input_channels, h,
               w);

  // 设置输入数据
  std::memcpy(input_tensor->data.f, nhwc_input.data(),
              sizeof(float) * 1 * h * w * input_channels);

  // 运行推理
  if (interpreter->Invoke() != kTfLiteOk) {
    std::cerr << "TFLite inference error\n";
    exit(1);
  }

  // 获取输出（NHWC 格式）
  float *output_data = output_tensor->data.f;
  int output_channels = output_tensor->dims->data[3];  // NHWC 格式，通道在最后

  // 将输出从 NHWC 转换为 NCHW
  std::vector<float> nchw_output(1 * output_channels * h * w);
  nhwc_to_nchw(output_data, nchw_output.data(), 1, output_channels, h, w);

  // 将输出写入 interm（反归一化）
  for (int h_idx = 0; h_idx < h; h_idx++) {
    for (int w_idx = 0; w_idx < w; w_idx++) {
      switch (c) {
        case 3:
          interm[h_idx][w_idx][2] =
              nchw_output[2 * h * w + h_idx * w + w_idx] * max_val;
        case 2:
          interm[h_idx][w_idx][1] =
              nchw_output[1 * h * w + h_idx * w + w_idx] * max_val;
        case 1:
          interm[h_idx][w_idx][0] =
              nchw_output[0 * h * w + h_idx * w + w_idx] * max_val;
        default:;
      }
    }
  }
}

static void apply_linear_combination(
    const uint16_t *src, int src_stride,
    const std::vector<std::vector<std::vector<float>>> &interm, int start_row,
    int end_row, int start_col, int end_col, int bit_depth,
    const std::vector<std::vector<int>> &A, size_t &A_index, uint16_t *dgd,
    int dgd_stride, uint16_t *cnn_out, int cnn_out_stride,
    const QuantizationParams_t *quantset, GuidedAdaptiveChannelType mode_type,
    int is_intra_only, int q_index) {
  // 打印 is_intra_only q_index mode_type TODOINTER
  // fprintf(stdout, "is_intra_only: %d, q_index: %d, mode_type: %d\n",
  //         is_intra_only, q_index, mode_type);
  if (mode_type == GUIDED_NONE) {
    for (int r = start_row; r < end_row; ++r) {
      for (int c = start_col; c < end_col; ++c) {
        const int dgd_unclipped = static_cast<int>(round(
            static_cast<float>(dgd[r * dgd_stride + c]) + interm[r][c][0]));
        cnn_out[r * cnn_out_stride + c] =
            clip_pixel_highbd(dgd_unclipped, bit_depth);
      }
    }
    return;
  } else if (mode_type == GUIDED_C_INVALID) {
    for (int r = start_row; r < end_row; ++r) {
      for (int c = start_col; c < end_col; ++c) {
        cnn_out[r * cnn_out_stride + c] = dgd[r * dgd_stride + c];
      }
    }
    return;
  }

  // level 1 2
  const auto &this_A = A[A_index++];
  const auto use_residual_cb = this_A.size() > 1;
  auto idx = this_A[0];
  auto res_idx = use_residual_cb ? this_A[1] : -1;
  auto qp_idx = get_qp_idx(q_index, is_intra_only, bit_depth);
  std::vector<float> qA =
      get_codebook_val(is_intra_only, qp_idx, 0, mode_type, idx, false);

  if (use_residual_cb) {
    const std::vector<float> res_qA =
        get_codebook_val(is_intra_only, qp_idx, 0, mode_type, res_idx, true);
    for (int i = 0; i < (int)qA.size(); i++) {
      qA[i] += res_qA[i];
    }
  }

  for (int r = start_row; r < end_row; r++) {
    for (int c = start_col; c < end_col; c++) {
      int dgd_unclipped = dgd[r * dgd_stride + c];
      float interm_val = 0;
      for (int ch = 0; ch < mode_type; ++ch) {
        interm_val += qA[ch] * interm[r][c][ch];
      }
      dgd_unclipped += static_cast<int>(roundf(interm_val));
      cnn_out[r * cnn_out_stride + c] =
          clip_pixel_highbd(dgd_unclipped, bit_depth);
    }
  }
}

static void apply_quadtree_partitioning(
    const uint16_t *src, int src_stride,
    const std::vector<std::vector<std::vector<std::vector<float>>>> &interms,
    int start_row, int start_col, int width, int height, int quadtree_max_size,
    int max_unit_width, int max_unit_height, int bit_depth,
    const std::vector<int> &split, size_t &split_index,
    const std::vector<std::vector<int>> &A, size_t &A_index, uint16_t *dgd,
    int dgd_stride, uint16_t *cnn_out, int cnn_out_stride, int q_index,
    int is_intra_only, int is_luma) {
  const int end_row = AOMMIN(start_row + max_unit_height, height);
  const int end_col = AOMMIN(start_col + max_unit_width, width);

  GuidedAdaptiveChannelType mode_type = GUIDED_NONE;
  mode_type = static_cast<GuidedAdaptiveChannelType>(split[split_index++]);
  auto guided_c = AOMMAX(0, mode_type - 1);
  assert(mode_type >= 0 && mode_type < GUIDED_C_TYPES);
  auto &interm = interms[guided_c];

  apply_linear_combination(src, src_stride, interm, start_row, end_row,
                           start_col, end_col, bit_depth, A, A_index, dgd,
                           dgd_stride, cnn_out, cnn_out_stride, nullptr,
                           mode_type, is_intra_only, q_index);
}

// 简单的矩阵乘法
void matrix_multiply(const float *A, int rows_a, int cols_a, const float *B,
                     int rows_b, int cols_b, float *C) {
  // C = A * B
  for (int i = 0; i < rows_a; ++i) {
    for (int j = 0; j < cols_b; ++j) {
      float sum = 0.0f;
      for (int k = 0; k < cols_a; ++k) {
        sum += A[i * cols_a + k] * B[k * cols_b + j];
      }
      C[i * cols_b + j] = sum;
    }
  }
}

// 矩阵转置
void matrix_transpose(const float *A, int rows, int cols, float *AT) {
  for (int i = 0; i < rows; ++i) {
    for (int j = 0; j < cols; ++j) {
      AT[j * rows + i] = A[i * cols + j];
    }
  }
}

// Cholesky 分解求解线性方程组 Ax = b
// 返回 true 表示成功，false 表示矩阵不是正定的
bool cholesky_solve(const float *A, int n, const float *b, float *x) {
  // 分配临时内存用于 Cholesky 分解
  std::vector<float> L(n * n, 0.0f);

  // Cholesky 分解：A = L * L^T
  for (int i = 0; i < n; ++i) {
    for (int j = 0; j <= i; ++j) {
      float sum = A[i * n + j];
      for (int k = 0; k < j; ++k) {
        sum -= L[i * n + k] * L[j * n + k];
      }

      if (i == j) {
        if (sum <= 0.0f) {
          return false;  // 矩阵不是正定的
        }
        L[i * n + j] = std::sqrt(sum);
      } else {
        L[i * n + j] = sum / L[j * n + j];
      }
    }
  }

  // 前向替换：L * y = b
  std::vector<float> y(n);
  for (int i = 0; i < n; ++i) {
    float sum = b[i];
    for (int j = 0; j < i; ++j) {
      sum -= L[i * n + j] * y[j];
    }
    y[i] = sum / L[i * n + i];
  }

  // 后向替换：L^T * x = y
  for (int i = n - 1; i >= 0; --i) {
    float sum = y[i];
    for (int j = i + 1; j < n; ++j) {
      sum -= L[j * n + i] * x[j];
    }
    x[i] = sum / L[i * n + i];
  }

  return true;
}

// // 修改后的 generate_linear_combination_torch（不使用 Eigen）
static bool generate_linear_combination(
    const int qp_idx, const int unit_index,
    const std::vector<std::vector<std::vector<float>>> &interm, int c,
    const uint16_t *src, int src_stride, const uint16_t *dgd, int dgd_stride,
    int start_row, int end_row, int start_col, int end_col,
    const QuantizationParams_t *quadtset, int rdmult, const int *norestorecost,
    const int (*this_blk_codebook_cost)[CODEBOOK_CHANNEL][256], int bit_depth,
    std::vector<std::vector<uint16_t>> &out, std::vector<std::vector<int>> &A,
    int is_intra_only, bool &use_res_cb) {
  assert(c >= 0 && c <= 4);

  int64_t err_no_filter = 0;
  for (int i = start_row; i < end_row; ++i) {
    for (int j = start_col; j < end_col; ++j) {
      int diff = src[i * src_stride + j] - dgd[i * dgd_stride + j];
      err_no_filter += diff * diff;
    }
  }

  if (c == 0) {
    int64_t cnn_no_guided_err = 0;
    for (int i = start_row; i < end_row; ++i) {
      for (int j = start_col; j < end_col; ++j) {
        int pred_val = dgd[i * dgd_stride + j];
        pred_val += static_cast<int>(std::round(interm[i][j][0]));
        pred_val = clip_pixel_highbd(pred_val, bit_depth);
        out[i - start_row][j - start_col] = pred_val;
        int diff = src[i * src_stride + j] - pred_val;
        cnn_no_guided_err += diff * diff;
      }
    }
    if (err_no_filter < cnn_no_guided_err) {
      for (int i = start_row; i < end_row; ++i) {
        for (int j = start_col; j < end_col; ++j) {
          out[i - start_row][j - start_col] = dgd[i * dgd_stride + j];
        }
      }
    }
    return cnn_no_guided_err < err_no_filter;
  }

  const int num_pixels = (end_row - start_row) * (end_col - start_col);

  // 准备数据：GT residual [num_pixels, 1]
  std::vector<float> r_gt(num_pixels);

  // 准备数据：predicted residuals [num_pixels, c]
  std::vector<float> R(num_pixels * c);

  int idx = 0;
  for (int i = start_row; i < end_row; ++i) {
    for (int j = start_col; j < end_col; ++j) {
      auto res_gt =
          static_cast<float>(src[i * src_stride + j] - dgd[i * dgd_stride + j]);
      r_gt[idx] = res_gt;

      for (int ch = 0; ch < c; ++ch) {
        R[idx * c + ch] = interm[i][j][ch];
      }
      ++idx;
    }
  }

  // 求解最小二乘：A = (R^T R)^-1 R^T r_gt
  // 计算 R^T [c, num_pixels]
  std::vector<float> RT(c * num_pixels);
  matrix_transpose(R.data(), num_pixels, c, RT.data());

  // 计算 R^T * R [c, c]
  std::vector<float> RTR(c * c);
  matrix_multiply(RT.data(), c, num_pixels, R.data(), num_pixels, c,
                  RTR.data());

  // 添加正则化项
  for (int i = 0; i < c; ++i) {
    RTR[i * c + i] += 1e-4f;
  }

  // 计算 R^T * r_gt [c, 1]
  std::vector<float> RTy(c);
  for (int i = 0; i < c; ++i) {
    float sum = 0.0f;
    for (int j = 0; j < num_pixels; ++j) {
      sum += RT[i * num_pixels + j] * r_gt[j];
    }
    RTy[i] = sum;
  }

  // 使用 Cholesky 分解求解 (R^T R) * A = R^T r_gt
  std::vector<float> A_tensor(c);
  bool success = cholesky_solve(RTR.data(), c, RTy.data(), A_tensor.data());

  if (!success) {
    // 如果 Cholesky 分解失败，使用伪逆（SVD 或 LU 分解）
    // 这里简化处理，使用高斯消元法
    // 注意：对于小矩阵（c <= 4），可以直接使用高斯消元
    std::cerr << "Warning: Cholesky decomposition failed, using Gaussian "
                 "elimination\n";

    // 简单的高斯消元法求解线性方程组
    std::vector<std::vector<float>> aug_matrix(c, std::vector<float>(c + 1));
    for (int i = 0; i < c; ++i) {
      for (int j = 0; j < c; ++j) {
        aug_matrix[i][j] = RTR[i * c + j];
      }
      aug_matrix[i][c] = RTy[i];
    }

    // 高斯消元
    for (int i = 0; i < c; ++i) {
      // 找到主元
      int max_row = i;
      for (int k = i + 1; k < c; ++k) {
        if (std::abs(aug_matrix[k][i]) > std::abs(aug_matrix[max_row][i])) {
          max_row = k;
        }
      }
      std::swap(aug_matrix[i], aug_matrix[max_row]);

      // 消元
      for (int k = i + 1; k < c; ++k) {
        float factor = aug_matrix[k][i] / aug_matrix[i][i];
        for (int j = i; j <= c; ++j) {
          aug_matrix[k][j] -= factor * aug_matrix[i][j];
        }
      }
    }

    // 回代
    for (int i = c - 1; i >= 0; --i) {
      A_tensor[i] = aug_matrix[i][c];
      for (int j = i + 1; j < c; ++j) {
        A_tensor[i] -= aug_matrix[i][j] * A_tensor[j];
      }
      A_tensor[i] /= aug_matrix[i][i];
    }
  }

  // 转换为 std::vector
  std::vector<float> floatA(c);
  for (int ch = 0; ch < c; ++ch) {
    floatA[ch] = A_tensor[ch];
  }

  // 后续代码保持不变...
  double bestcost = RDCOST_DBL_WITH_NATIVE_BD_DIST(
      rdmult, norestorecost[1] >> 4, err_no_filter, bit_depth);

  int64_t err = 0;

  std::vector<int> bestA;
  bool is_any_candidate_hit = false;
  auto codebook_ret =
      get_codebook_idx_and_val(is_intra_only, qp_idx, 0, floatA, false);
  auto codebook_idx = codebook_ret.first;
  auto qA = codebook_ret.second;

  vector<float> actul_resA;
  for (size_t i = 0; i < qA.size(); i++) {
    actul_resA.push_back(floatA[i] - qA[i]);
  }

  auto res_codebook_ret =
      get_codebook_idx_and_val(is_intra_only, qp_idx, 0, actul_resA, true);
  auto res_codebook_idx = res_codebook_ret.first;
  auto res_qA = res_codebook_ret.second;

  vector<float> final_qA;
  for (size_t i = 0; i < res_qA.size(); i++) {
    final_qA.push_back(qA[i] + res_qA[i]);
  }

  auto codebook_cost = this_blk_codebook_cost[0][c - 1][codebook_idx];
  auto res_codebook_cost = this_blk_codebook_cost[1][c - 1][res_codebook_idx];

  for (int i = start_row; i < end_row; ++i) {
    for (int j = start_col; j < end_col; ++j) {
      int pred_val = dgd[i * dgd_stride + j];
      for (int ch = 0; ch < c; ++ch) {
        pred_val += static_cast<int>(
            std::round(static_cast<float>(qA[ch] * interm[i][j][ch])));
      }
      pred_val = clip_pixel_highbd(pred_val, bit_depth);
      int diff = static_cast<int>(src[i * src_stride + j]) - pred_val;
      err += diff * diff;
    }
  }

  double cb_cost = RDCOST_DBL_WITH_NATIVE_BD_DIST(
      rdmult, (norestorecost[0] + codebook_cost) >> 4, err, bit_depth);

  err = 0;
  for (int i = start_row; i < end_row; ++i) {
    for (int j = start_col; j < end_col; ++j) {
      int pred_val = dgd[i * dgd_stride + j];
      for (int ch = 0; ch < c; ++ch) {
        pred_val += static_cast<int>(
            std::round(static_cast<float>(final_qA[ch] * interm[i][j][ch])));
      }
      pred_val = clip_pixel_highbd(pred_val, bit_depth);
      int diff = static_cast<int>(src[i * src_stride + j]) - pred_val;
      err += diff * diff;
    }
  }

  double res_cb_cost = RDCOST_DBL_WITH_NATIVE_BD_DIST(
      rdmult, (norestorecost[0] + codebook_cost + res_codebook_cost) >> 4, err,
      bit_depth);

  use_res_cb = res_cb_cost < cb_cost;
  auto cost = use_res_cb ? res_cb_cost : cb_cost;

  // cout << "res_cb_cost: " << res_cb_cost << " cb_cost: " << cb_cost << "
  // cost: " << cost << endl; cout << "residual:" << (use_res_cb ? "true" :
  // "false") << endl;

  if (cost < bestcost) {
    bestcost = cost;
    bestA.push_back(codebook_idx);
    // fprintf(stderr, "codebook_idx: %d\n", codebook_idx);
    if (use_res_cb) {
      bestA.push_back(res_codebook_idx);
      // fprintf(stderr, "res_codebook_idx: %d\n", res_codebook_idx);
    }
    is_any_candidate_hit = true;
  }

  if (!is_any_candidate_hit) {
    return false;
  }

  const auto &bestA_float =
      get_codebook_val(is_intra_only, qp_idx, 0, c, bestA[0], false);

  vector<float> bestA_float_vec(bestA_float);
  if (use_res_cb) {
    const auto &res_bestA_float =
        get_codebook_val(is_intra_only, qp_idx, 0, c, bestA[1], true);
    for (size_t i = 0; i < res_bestA_float.size(); i++) {
      bestA_float_vec[i] += res_bestA_float[i];
    }
  }

  for (int i = start_row; i < end_row; i++) {
    for (int j = start_col; j < end_col; j++) {
      int out_unclipped = dgd[i * dgd_stride + j];
      for (int ch = 0; ch < c; ++ch) {
        out_unclipped += static_cast<int>(
            std::round((bestA_float_vec[ch]) * interm[i][j][ch]));
      }
      out[i - start_row][j - start_col] =
          clip_pixel_highbd(out_unclipped, bit_depth);
    }
  }
  A.push_back(bestA);
  return true;
}

// static bool generate_linear_combination_torch(
//     const int qp_idx, const int unit_index,
//     const std::vector<std::vector<std::vector<float>>> &interm, int c,
//     const uint16_t *src, int src_stride, const uint16_t *dgd, int dgd_stride,
//     int start_row, int end_row, int start_col, int end_col,
//     const QuantizationParams_t *quadtset, int rdmult, const int
//     *norestorecost, const int
//     (*this_blk_codebook_cost)[CODEBOOK_CHANNEL][256], int bit_depth,
//     std::vector<std::vector<uint16_t>> &out, std::vector<std::vector<int>>
//     &A, int is_intra_only, bool &use_res_cb) {
//   assert(c >= 0 && c <= 4);

//   int64_t err_no_filter = 0;  // 不进行滤波
//   for (int i = start_row; i < end_row; ++i) {
//     for (int j = start_col; j < end_col; ++j) {
//       int diff = src[i * src_stride + j] - dgd[i * dgd_stride + j];
//       err_no_filter += diff * diff;
//     }
//   }

//   if (c == 0) {  // 只使用CNN滤波
//     int64_t cnn_no_guided_err = 0;
//     for (int i = start_row; i < end_row; ++i) {
//       for (int j = start_col; j < end_col; ++j) {
//         int pred_val = dgd[i * dgd_stride + j];
//         pred_val += static_cast<int>(std::round(interm[i][j][0]));
//         pred_val = clip_pixel_highbd(pred_val, bit_depth);
//         out[i - start_row][j - start_col] = pred_val;
//         int diff = src[i * src_stride + j] - pred_val;
//         cnn_no_guided_err += diff * diff;
//       }
//     }
//     if (err_no_filter < cnn_no_guided_err) {  //
//     // 如果使用CNN滤波err大，则使用不滤波的结果
//       for (int i = start_row; i < end_row; ++i) {
//         for (int j = start_col; j < end_col; ++j) {
//           out[i - start_row][j - start_col] = dgd[i * dgd_stride + j];
//         }
//       }
//     }
//     return cnn_no_guided_err < err_no_filter;  //
//     // 如果使用CNN滤波err更小,则返回true
//   }

//   const int num_pixels = (end_row - start_row) * (end_col - start_col);
//   // [num_pixels, 1] GT residual
//   torch::Tensor r_gt = torch::empty({ num_pixels, 1 }, torch::kFloat32);
//   // [num_pixels, c] predicted residuals
//   torch::Tensor R = torch::empty({ num_pixels, c }, torch::kFloat32);
//   int idx = 0;
//   for (int i = start_row; i < end_row; ++i) {
//     for (int j = start_col; j < end_col; ++j) {
//       auto res_gt = static_cast<float>(src[i * src_stride + j] - dgd[i *
//       dgd_stride + j]); r_gt[idx][0] = res_gt; for (int ch = 0; ch < c; ++ch)
//       {
//         R[idx][ch] = interm[i][j][ch];
//       }
//       ++idx;
//     }
//   }
//   // Solve A = (R^T R)^-1 R^T r_gt
//   torch::Tensor RT = R.transpose(0, 1);           // [c, N]
//   torch::Tensor RTR = RT.matmul(R);               // [c, c]
//   RTR += 1e-4 * torch::eye(c, R.options());       // 稳定正则
//   auto RTy = RT.matmul(r_gt);                     // [c, 1]
//   auto A_tensor = torch::linalg_solve(RTR, RTy);  // 替代 lstsq，更

//   // Apply scale
//   std::vector<float> floatA(c);

//   for (int ch = 0; ch < c; ++ch) {
//     floatA[ch] = A_tensor[ch].item<float>();
//   }

//   double bestcost = RDCOST_DBL_WITH_NATIVE_BD_DIST(
//       rdmult, norestorecost[1] >> 4, err_no_filter, bit_depth);

//   int64_t err = 0;

//   std::vector<int> bestA;
//   bool is_any_candidate_hit = false;
//   auto codebook_ret = get_codebook_idx_and_val(is_intra_only, qp_idx, 0,
//   floatA, false); auto codebook_idx = codebook_ret.first; auto qA =
//   codebook_ret.second;

//   vector<float> actul_resA;
//   for (size_t i = 0; i < qA.size(); i++) {
//     actul_resA.push_back(floatA[i] - qA[i]);
//   }  // 得到真实残差

//   auto res_codebook_ret = get_codebook_idx_and_val(is_intra_only, qp_idx, 0,
//   actul_resA, true); auto res_codebook_idx = res_codebook_ret.first; auto
//   res_qA = res_codebook_ret.second;

//   vector<float> final_qA;
//   for (size_t i = 0; i < res_qA.size(); i++) {
//     final_qA.push_back(qA[i] + res_qA[i]);
//   }

//   auto codebook_cost = this_blk_codebook_cost[0][c - 1][codebook_idx];
//   auto res_codebook_cost = this_blk_codebook_cost[1][c -
//   1][res_codebook_idx];

//   for (int i = start_row; i < end_row; ++i) {
//     for (int j = start_col; j < end_col; ++j) {
//       int pred_val = dgd[i * dgd_stride + j];
//       for (int ch = 0; ch < c; ++ch) {
//         pred_val += static_cast<int>(
//             std::round(static_cast<float>(qA[ch] * interm[i][j][ch])));
//       }
//       pred_val = clip_pixel_highbd(pred_val, bit_depth);
//       int diff = static_cast<int>(src[i * src_stride + j]) - pred_val;
//       err += diff * diff;
//     }
//   }

//   double cb_cost = RDCOST_DBL_WITH_NATIVE_BD_DIST(
//       rdmult, (norestorecost[0] + codebook_cost) >> 4, err, bit_depth);

//   err = 0;
//   for (int i = start_row; i < end_row; ++i) {
//     for (int j = start_col; j < end_col; ++j) {
//       int pred_val = dgd[i * dgd_stride + j];
//       for (int ch = 0; ch < c; ++ch) {
//         pred_val += static_cast<int>(
//             std::round(static_cast<float>(final_qA[ch] * interm[i][j][ch])));
//       }
//       pred_val = clip_pixel_highbd(pred_val, bit_depth);
//       int diff = static_cast<int>(src[i * src_stride + j]) - pred_val;
//       err += diff * diff;
//     }
//   }

//   double res_cb_cost = RDCOST_DBL_WITH_NATIVE_BD_DIST(
//       rdmult, (norestorecost[0] + codebook_cost + res_codebook_cost) >> 4,
//       err, bit_depth);

//   use_res_cb = res_cb_cost < cb_cost;
//   auto cost = use_res_cb ? res_cb_cost : cb_cost;

//   // cout << "res_cb_cost: " << res_cb_cost << " cb_cost: " << cb_cost << "
//   // cost: " << cost << endl; cout << "residual:" << (use_res_cb ? "true" :
//   // "false") << endl;

//   if (cost < bestcost) {
//     bestcost = cost;
//     bestA.push_back(codebook_idx);
//     if (use_res_cb) {
//       bestA.push_back(res_codebook_idx);
//     }
//     is_any_candidate_hit = true;
//   }

//   if (!is_any_candidate_hit) {
//     return false;
//   }

//   const auto &bestA_float = get_codebook_val(is_intra_only, qp_idx, 0, c,
//   bestA[0], false);

//   // for (auto &e : bestA_float) {
//   //   std::cout << e << "<--->";
//   // }
//   // std::cout << std::endl;

//   vector<float> bestA_float_vec(bestA_float);
//   if (use_res_cb) {
//     const auto &res_bestA_float =
//         get_codebook_val(is_intra_only, qp_idx, 0, c, bestA[1], true);
//     for (size_t i = 0; i < res_bestA_float.size(); i++) {
//       bestA_float_vec[i] += res_bestA_float[i];
//     }
//   }

//   for (int i = start_row; i < end_row; i++) {
//     for (int j = start_col; j < end_col; j++) {
//       int out_unclipped = dgd[i * dgd_stride + j];
//       for (int ch = 0; ch < c; ++ch) {
//         out_unclipped += static_cast<int>(
//             std::round((bestA_float_vec[ch]) * interm[i][j][ch]));
//       }
//       out[i - start_row][j - start_col] =
//           clip_pixel_highbd(out_unclipped, bit_depth);
//     }
//   }
//   A.push_back(bestA);
//   return true;
// }

// computes SSE between 'rst' and 'src'
static int64_t compute_sse(const std::vector<std::vector<uint16_t>> &rst,
                           const uint16_t *src, int src_stride, int start_row,
                           int end_row, int start_col, int end_col) {
  int64_t sse = 0;
  for (int r = start_row; r < end_row; ++r) {
    for (int c = start_col; c < end_col; ++c) {
      const uint16_t this_rst = rst[r - start_row][c - start_col];
      const uint16_t this_src = src[r * src_stride + c];
      const int64_t diff = (int64_t)(this_rst - this_src);
      sse += diff * diff;
    }
  }
  return sse;
}

// computes bitrate for the given weight parameters
static int compute_rate(
    const int qp_idx, const int unit_index,
    const std::vector<std::vector<int>> &A, const std::vector<int> &prev_A,
    const QuantizationParams_t *quadtset, const int *norestorecosts,
    const int (*this_blk_codebook_cost)[CODEBOOK_CHANNEL][256], int mode_type,
    int is_filtered, int is_intra_only,
    const std::vector<std::vector<int>> &A_list, const int &use_res_cb) {
  if (A.empty()) return 0;
  const std::vector<int> &this_A = A[0];
  int total_costs = 0;
  if (is_filtered) {
    total_costs += norestorecosts[0];
    int idx = this_A[0];
    total_costs += this_blk_codebook_cost[0][mode_type - 1][idx];

    if (use_res_cb) {
      int res_idx = this_A[1];
      // cout << total_costs << "bef" << endl;
      total_costs += this_blk_codebook_cost[1][mode_type - 1][res_idx];
      // cout << total_costs << "af" << endl;

      // cout << "res_idx: " << res_idx << "sec" <<
      // this_blk_codebook_cost[1][mode_type - 1][res_idx] << endl;
    }
  } else {
    total_costs += norestorecosts[1];
  }
  return total_costs;
}

static bool try_one_model(
    int qp_idx, int unit_index,
    const std::vector<std::vector<std::vector<float>>> &interm,
    GuidedAdaptiveChannelType model_type, const uint16_t *src,
    const int src_stride, const uint16_t *dgd, const int dgd_stride,
    int start_row, int end_row, int start_col, int end_col,
    const QuantizationParams_t *quantset, int rdmult,
    const std::vector<int> &prev_A, const int *cnn_guided_mode_costs,
    const int *norestorecosts,
    const int (*codebook_cost)[CODEBOOK_CHANNEL][256], int bit_depth,
    double *this_rdcost, std::vector<std::vector<uint16_t>> &out,
    std::vector<std::vector<int>> &this_A,
    std::vector<std::vector<int>> &A_list, int num_rows, int num_cols,
    int is_intra_only) {
  bool use_res_cb = false;
  auto is_filtered = false;
  is_filtered = generate_linear_combination(
      qp_idx, unit_index, interm, model_type, src, src_stride, dgd, dgd_stride,
      start_row, end_row, start_col, end_col, quantset, rdmult, norestorecosts,
      codebook_cost, bit_depth, out, this_A, is_intra_only, use_res_cb);

  // assert(IMPLIES(model_type == GUIDED_NONE, this_A.size() == 0));
  // assert(IMPLIES(model_type == GUIDED_C1, this_A[0].size() == 1));
  // assert(IMPLIES(model_type == GUIDED_C2, this_A[0].size() == 2));
  // assert(IMPLIES(model_type == GUIDED_C3, this_A[0].size() == 3));

  // compute SSE
  int64_t sse = INT64_MAX;
  sse =
      compute_sse(out, src, src_stride, start_row, end_row, start_col, end_col);

  const int a_signaling_cost =
      compute_rate(qp_idx, unit_index, this_A, prev_A, quantset, norestorecosts,
                   codebook_cost, model_type, is_filtered, is_intra_only,
                   A_list, use_res_cb);

  const int mode_signaling_cost = cnn_guided_mode_costs[model_type];
  const int bitrate =
      a_signaling_cost + (is_filtered ? mode_signaling_cost : 0);

  *this_rdcost =
      RDCOST_DBL_WITH_NATIVE_BD_DIST(rdmult, bitrate >> 4, sse, bit_depth);
  return is_filtered;
}

static void select_model(
    int unit_index,
    const std::vector<std::vector<std::vector<std::vector<float>>>> &interms,
    const uint16_t *src, int src_stride, int start_row, int start_col,
    int width, int height, int quadtree_max_size, int max_unit_width,
    int max_unit_height, int rdmult, const int *cnn_guided_mode_costs,
    const int norestorecosts[2],
    const int (*codebook_cost)[CODEBOOK_CHANNEL][256], int bit_depth,
    const uint16_t *dgd, int dgd_stride, int is_intra_only, int q_index,
    int is_luma, std::vector<int> &mode, std::vector<std::vector<int>> &A,
    double *rdcost, int num_rows, int num_cols) {
  const int end_row = AOMMIN(start_row + max_unit_height, height);
  const int end_col = AOMMIN(start_col + max_unit_width, width);
  auto qp_idx = get_qp_idx(q_index, is_intra_only, bit_depth);
  auto best_rdcost = DBL_MAX;
  std::vector<std::vector<int>> best_A;
  std::vector best_out(max_unit_height, std::vector<uint16_t>(max_unit_width));
  GuidedAdaptiveChannelType best_mode_type = GUIDED_C_INVALID;
  vector<bool> filtered_flags(GUIDED_C_TYPES);

  for (int type = 0; type < GUIDED_C_TYPES; ++type) {
    const auto this_mode_type = static_cast<GuidedAdaptiveChannelType>(type);
    const auto guided_c = AOMMAX(0, type - 1);
    auto &interm = interms[guided_c];

    double this_rdcost;
    std::vector<std::vector<int>> this_A;
    std::vector this_out(max_unit_height,
                         std::vector<uint16_t>(max_unit_width));

    std::vector<int> prev_A;

    for (int i = 0; i < type; ++i) {
      prev_A.push_back(GUIDED_A_MID);
    }

    filtered_flags[type] = try_one_model(
        qp_idx, unit_index, interm, this_mode_type, src, src_stride, dgd,
        dgd_stride, start_row, end_row, start_col, end_col, nullptr, rdmult,
        prev_A, cnn_guided_mode_costs, norestorecosts, codebook_cost, bit_depth,
        &this_rdcost, this_out, this_A, A, num_rows, num_cols, is_intra_only);

    if (this_rdcost < best_rdcost) {
      best_rdcost = this_rdcost;
      best_A = this_A;
      best_out = this_out;
      best_mode_type = this_mode_type;
    }
  }

  bool is_filtered = false;
  for (auto filtered_flag : filtered_flags) {
    if (filtered_flag) {
      is_filtered = true;
      break;
    }
  }
  if (!is_filtered) {
    best_mode_type = GUIDED_C_INVALID;
  } else {
    // Save a0, a1 pairs.
    for (auto &a0a1 : best_A) {
      A.push_back(a0a1);
    }
  }
  // Save RDCost.
  *rdcost = best_rdcost;
  mode.push_back(best_mode_type);
}

#define MY_TFLITE_MODEL_PATH "../av1/tflite_models/"
int select_model_path(int cnn_index, int qindex, int is_intra_only, int is_luma,
                      int bit_depth, std::string &model_path,
                      std::string type_size = "float32") {
  int qindex_adjust = qindex - 24 * (bit_depth - 8);
  type_size += ".tflite";
  std::string model_name;
  char buf[256];

  if (is_intra_only) {  // intra
    if (qindex_adjust <= 90) {
      model_name = (cnn_index == 0)   ? ("qp85_g1_" + type_size)
                   : (cnn_index == 1) ? ("qp85_g2_" + type_size)
                                      : ("qp85_g3_" + type_size);
    } else if (qindex_adjust <= 120) {
      model_name = (cnn_index == 0)   ? ("qp110_g1_" + type_size)
                   : (cnn_index == 1) ? ("qp110_g2_" + type_size)
                                      : ("qp110_g3_" + type_size);
    } else if (qindex_adjust <= 145) {
      model_name = (cnn_index == 0)   ? "qp135_g1_" + type_size
                   : (cnn_index == 1) ? "qp135_g2_" + type_size
                                      : "qp135_g3_" + type_size;
    } else if (qindex_adjust <= 175) {
      model_name = (cnn_index == 0)   ? "qp160_g1_" + type_size
                   : (cnn_index == 1) ? "qp160_g2_" + type_size
                                      : "qp160_g3_" + type_size;
    } else if (qindex_adjust <= 205) {
      model_name = (cnn_index == 0)   ? "qp185_g1_" + type_size
                   : (cnn_index == 1) ? "qp185_g2_" + type_size
                                      : "qp185_g3_" + type_size;
    } else {
      model_name = (cnn_index == 0)   ? "qp210_g1_" + type_size
                   : (cnn_index == 1) ? "qp210_g2_" + type_size
                                      : "qp210_g3_" + type_size;
    }
    snprintf(buf, sizeof(buf), "intra/%d_%s", 0, model_name.c_str());
    model_path = std::string(MY_TFLITE_MODEL_PATH) + buf;
    return 0;
  } else {  // inter
    if (qindex_adjust <= 110) {
      model_name = (cnn_index == 0)   ? "qp110_g1_" + type_size
                   : (cnn_index == 1) ? "qp110_g2_" + type_size
                                      : "qp110_g3_" + type_size;
    } else if (qindex_adjust <= 135) {
      model_name = (cnn_index == 0)   ? "qp135_g1_" + type_size
                   : (cnn_index == 1) ? "qp135_g2_" + type_size
                                      : "qp135_g3_" + type_size;
    } else if (qindex_adjust <= 160) {
      model_name = (cnn_index == 0)   ? "qp160_g1_" + type_size
                   : (cnn_index == 1) ? "qp160_g2_" + type_size
                                      : "qp160_g3_" + type_size;
    } else if (qindex_adjust <= 185) {
      model_name = (cnn_index == 0)   ? "qp185_g1_" + type_size
                   : (cnn_index == 1) ? "qp185_g2_" + type_size
                                      : "qp185_g3_" + type_size;
    } else if (qindex_adjust <= 210) {
      model_name = (cnn_index == 0)   ? "qp210_g1_" + type_size
                   : (cnn_index == 1) ? "qp210_g2_" + type_size
                                      : "qp210_g3_" + type_size;
    } else {
      model_name = (cnn_index == 0)   ? "qp235_g1_" + type_size
                   : (cnn_index == 1) ? "qp235_g2_" + type_size
                                      : "qp235_g3_" + type_size;
    }
    snprintf(buf, sizeof(buf), "inter/%d_%s", (is_luma ? 0 : -1),
             model_name.c_str());
    model_path = std::string(MY_TFLITE_MODEL_PATH) + buf;
    return 0;
  }
  return -1;
}

// #define MY_TORCH_MODEL_PATH "../av1/torch_models/"
// int select_model_path(int cnn_index, int qindex, int is_intra_only, int
// is_luma,
//                       int bit_depth, std::string &model_path) {
//   int qindex_adjust = qindex - 24 * (bit_depth - 8);

//   std::string model_name;
//   char buf[256];

//   if (is_intra_only) {  // intra
//     if (qindex_adjust <= 90) {
//       model_name = (cnn_index == 0)   ? "qp85_g1.pt"
//                    : (cnn_index == 1) ? "qp85_g2.pt"
//                                       : "qp85_g3.pt";
//     } else if (qindex_adjust <= 120) {
//       model_name = (cnn_index == 0)   ? "qp110_g1.pt"
//                    : (cnn_index == 1) ? "qp110_g2.pt"
//                                       : "qp110_g3.pt";
//     } else if (qindex_adjust <= 145) {
//       model_name = (cnn_index == 0)   ? "qp135_g1.pt"
//                    : (cnn_index == 1) ? "qp135_g2.pt"
//                                       : "qp135_g3.pt";
//     } else if (qindex_adjust <= 175) {
//       model_name = (cnn_index == 0)   ? "qp160_g1.pt"
//                    : (cnn_index == 1) ? "qp160_g2.pt"
//                                       : "qp160_g3.pt";
//     } else if (qindex_adjust <= 205) {
//       model_name = (cnn_index == 0)   ? "qp185_g1.pt"
//                    : (cnn_index == 1) ? "qp185_g2.pt"
//                                       : "qp185_g3.pt";
//     } else {
//       model_name = (cnn_index == 0)   ? "qp210_g1.pt"
//                    : (cnn_index == 1) ? "qp210_g2.pt"
//                                       : "qp210_g3.pt";
//     }
//     snprintf(buf, sizeof(buf), "my_intra_models/%d_%s", 0,
//     model_name.c_str()); model_path = std::string(MY_TORCH_MODEL_PATH) + buf;
//     return 0;
//   } else {  // inter
//     if (qindex_adjust <= 110) {
//       model_name = (cnn_index == 0)   ? "qp110_g1.pt"
//                    : (cnn_index == 1) ? "qp110_g2.pt"
//                                       : "qp110_g3.pt";
//     } else if (qindex_adjust <= 135) {
//       model_name = (cnn_index == 0)   ? "qp135_g1.pt"
//                    : (cnn_index == 1) ? "qp135_g2.pt"
//                                       : "qp135_g3.pt";
//     } else if (qindex_adjust <= 160) {
//       model_name = (cnn_index == 0)   ? "qp160_g1.pt"
//                    : (cnn_index == 1) ? "qp160_g2.pt"
//                                       : "qp160_g3.pt";
//     } else if (qindex_adjust <= 185) {
//       model_name = (cnn_index == 0)   ? "qp185_g1.pt"
//                    : (cnn_index == 1) ? "qp185_g2.pt"
//                                       : "qp185_g3.pt";
//     } else if (qindex_adjust <= 210) {
//       model_name = (cnn_index == 0)   ? "qp210_g1.pt"
//                    : (cnn_index == 1) ? "qp210_g2.pt"
//                                       : "qp210_g3.pt";
//     } else {
//       model_name = (cnn_index == 0)   ? "qp235_g1.pt"
//                    : (cnn_index == 1) ? "qp235_g2.pt"
//                                       : "qp235_g3.pt";
//     }
//     snprintf(buf, sizeof(buf), "my_inter_models/%d_%s", (is_luma ? 0 : -1),
//              model_name.c_str());
//     model_path = std::string(MY_TORCH_MODEL_PATH) + buf;
//     return 0;
//   }
//   return -1;
// }

void cnn_loopfilter(AV1_COMMON *cm, uint16_t **src_data, int *src_strides,
                    uint16_t **ccso_data, int *ccso_data_strides, int bitdepth,
                    int h, int w, uint16_t **dec_rec, int *dec_rec_strides,
                    uint16_t **dec_res, int *dec_res_strides, uint16_t **dec_bs,
                    int *dec_bs_strides, uint16_t **cnn_out,
                    int *cnn_out_strides, AdpGuidedInfo *adp_guided_info,
                    int is_intra_only, int q_index, int is_luma, int RDMULT,
                    int *cnn_guided_mode_costs, int (*norestore_costs)[2],
                    int (*codebook_costs)[CODEBOOK_CHANNEL][256],
                    double *rdcost) {
  // char model_location[1024] = { 0 };
  // init interms[i][h][w][i] (i = 0, 1, 2)
  std::vector<std::vector<std::vector<std::vector<float>>>> interms;
  for (int i = 1; i <= 3; i++) {
    interms.emplace_back(
        h, std::vector<std::vector<float>>(w, std::vector<float>(i)));
  }
  // src ccso cnn Y plane
  uint16_t *plane_src = src_data[0];
  uint16_t *plane_ccso = ccso_data[0];
  uint16_t *plane_cnn_out = cnn_out[0];
  // src ccso cnn Y stride
  int src_stride = src_strides[!is_luma];
  int ccso_stride = ccso_data_strides[!is_luma];
  int cnn_out_stride = cnn_out_strides[!is_luma];

  // get c1 c2 c3 res
  std::string model_path;
  for (int i = 0; i < static_cast<int>(interms.size()); ++i) {
    // get model path
    select_model_path(i, q_index, is_intra_only, is_luma, bitdepth, model_path);
    // std::cout << "model_path" << model_path << "\n";
    // build res
    // generate_interm_guided_restoration(
    //     ccso_data, ccso_data_strides, bitdepth, h, w, i + 1, dec_rec,
    //     dec_rec_strides, dec_res, dec_res_strides, dec_bs, dec_bs_strides,
    //     q_index, model_path.c_str(), is_intra_only, interms[i]);
    generate_interm_guided_restoration_tflite(
        ccso_data, ccso_data_strides, bitdepth, h, w, i + 1, dec_rec,
        dec_rec_strides, dec_res, dec_res_strides, dec_bs, dec_bs_strides,
        q_index, model_path.c_str(), is_intra_only, interms[i],
        cm->quant_params.base_qindex);
  }

  // inter 0 intra 1
  const int norestore_ctx = is_intra_only ? 1 : 0;
  const int null_norestore_costs[2] = { 0, 0 };
  const int *this_norestorecosts = norestore_ctx == -1
                                       ? null_norestore_costs
                                       : norestore_costs[norestore_ctx];

  int best_unit_index = -1;    // best split unit index
  std::vector<int> best_mode;  // mode of each unit
  std::vector<std::vector<int>>
      best_A;  // vector coefficients index of each unit
  double best_rdcost_total = DBL_MAX;
  for (int this_unit_index = 0; this_unit_index < GUIDED_QT_UNIT_SIZES;
       ++this_unit_index) {
    // basic segmentation size
    const int quadtree_max_size =
        quad_tree_get_unit_size(w, h, this_unit_index);
    const int rows = compute_num_blocks(h, quadtree_max_size);
    const int cols = compute_num_blocks(w, quadtree_max_size);

    // For each unit, compute the best mode for this unit.
    std::vector<int> this_mode;            // selected partitioning options.
    std::vector<std::vector<int>> this_A;  // selected a0, a1 weight pairs.
    double this_rdcost_total = 0.0;
    const int ext_size = quadtree_max_size * 3 / 2;

    for (int row = 0; row < h;) {
      const int remaining_height = h - row;
      const int this_unit_height =
          (remaining_height < ext_size) ? remaining_height : quadtree_max_size;

      for (int col = 0; col < w;) {
        const int remaining_width = w - col;
        const int this_unit_width =
            (remaining_width < ext_size) ? remaining_width : quadtree_max_size;

        double this_rdcost;  // 每一种分割方式下的每一个小块运用四种方式 四选一
        select_model(this_unit_index, interms, plane_src, src_stride, row, col,
                     w, h, quadtree_max_size, this_unit_width, this_unit_height,
                     RDMULT, cnn_guided_mode_costs, this_norestorecosts,
                     codebook_costs, bitdepth, plane_ccso, ccso_stride,
                     is_intra_only, q_index, is_luma, this_mode, this_A,
                     &this_rdcost, rows, cols);
        this_rdcost_total += this_rdcost;  // 累积到整帧的cost

        col += this_unit_width;
      }
      row += this_unit_height;
    }

    // Update best options.
    printf("[this_unit_index：%d][%lf：this_rdcost_total]\n", this_unit_index,
           this_rdcost_total);
    if (this_rdcost_total < best_rdcost_total) {
      best_unit_index = this_unit_index;
      best_mode = this_mode;
      best_A = this_A;
      best_rdcost_total = this_rdcost_total;
    }
  }

  // Fill in the best options.
  adp_guided_info->unit_index = best_unit_index;
  adp_guided_info->mode_info_length = static_cast<int>(best_mode.size());
  adp_guided_info->unit_info_length = static_cast<int>(best_A.size());

  av1_alloc_quadtree_struct(cm, adp_guided_info);
  for (unsigned int i = 0; i < best_mode.size(); ++i) {
    adp_guided_info->mode_info[i].mode = best_mode[i];
  }
  for (unsigned int i = 0; i < best_A.size(); ++i) {
    for (unsigned int j = 0; j < best_A[i].size(); ++j) {
      adp_guided_info->use_res_cb[i] = best_A[i].size() > 1 ? 1 : 0;
      // printf("best_A[i].size()： %ld ，adp_guided_info->use_res_cb[i]： %d \n",
      //        best_A[i].size(), adp_guided_info->use_res_cb[i]);
      adp_guided_info->unit_info[i].xqd[j] = best_A[i][j];
    }
  }
  *rdcost = best_rdcost_total;

  // printf("\n");
  // for (int i = 0; i < adp_guided_info->unit_info_length; i++) {
  //   printf("%d ", adp_guided_info->use_res_cb[i]);
  // }
  // printf("\n");

  size_t split_index = 0;
  size_t A_index = 0;
  const int quadtree_max_size = adp_guided_info->unit_size;
  const int ext_size = quadtree_max_size * 3 / 2;
  for (int row = 0; row < h;) {
    const int remaining_height = h - row;
    const int this_unit_height =
        (remaining_height < ext_size) ? remaining_height : quadtree_max_size;

    for (int col = 0; col < w;) {
      const int remaining_width = w - col;
      const int this_unit_width =
          (remaining_width < ext_size) ? remaining_width : quadtree_max_size;

      apply_quadtree_partitioning(
          plane_src, src_stride, interms, row, col, w, h, quadtree_max_size,
          this_unit_width, this_unit_height, bitdepth, best_mode, split_index,
          best_A, A_index, plane_ccso, ccso_stride, plane_cnn_out,
          cnn_out_stride, q_index, is_intra_only, is_luma);

      col += this_unit_width;
    }
    row += this_unit_height;
  }
  cm->is_use_cnn = true;  // TODOINTER
}
#endif

#if CONFIG_MAKE_DATASETS
extern const char *dataset_file_path;
extern int dataset_qp;

extern "C" void nn_create_dir(const char *path) {
  std::error_code ec;  // 可选：用于捕获错误，避免抛异常
  bool created = std::filesystem::create_directories(std::string(path), ec);

  if (!ec) {
    if (created) {
      std::cout << "目录已创建: " << path << "\n";
    } else {
      std::cout << "目录已存在: " << path << "\n";
    }
  } else {
    std::cerr << "创建目录失败: " << ec.message() << "\n";
    exit(-1);
  }
}

extern "C" void nn_lf_make_datasets(AV1_COMP *cpi, AV1_COMMON *cm);

const std::string datasets_output_path = "../datasets_inter_limit65";

enum class OUTPUT_DATA_TYPE {
  REC,
  BS,
  POST_DBLK,
  POST_CDEF,
  POST_CCSO,
  POST_LR,
  POST_GDF,
  CNN_OUT,
  // PRED,
  // RES,
  // QP,
  OUTPUT_DATA_TYPE_COUNT
};

std::string nn_get_dataset_file_name(const std::string &file_path, int h, int w,
                                     int poc, int is_intra_only, int qindex,
                                     aom_bit_depth_t bit_depth) {
  // 1. 路径的最后一层
  std::size_t pos = file_path.find_last_of(R"(\/)");
  if (pos == std::string::npos) {
    return "";
  }
  // 2. 文件名
  std::string file_name = file_path.substr(pos + 1);
  if (file_name.empty()) {
    return "";
  }
  // 3. 后缀名
  pos = file_name.rfind(".");
  if (pos == std::string::npos) {
    return "";
  }
  // 4. 格式化 poc h w
  std::ostringstream oss;
  oss << "_" << w << "*" << h;
  oss << "_" << std::setw(4) << std::setfill('0') << poc;
  oss << "_" << is_intra_only;
  oss << "_" << "qp" << qindex;
  oss << "_" << bit_depth << "bit";
  return file_name.substr(0, pos) + oss.str() + file_name.substr(pos);
}

void nn_write_frame(const std::string &filename, uint16_t **data,
                    const int *strides, int h, int w,
                    aom_bit_depth_t bit_depth) {
  assert(data != NULL && strides != NULL);

  uint16_t *plane_y = data[0];
  uint16_t *plane_u = data[1];
  uint16_t *plane_v = data[2];

  int stride_y = strides[0];
  int stride_uv = strides[1];

  std::vector<uint8_t> row_8bit(w);
  std::vector<uint16_t> row_16bit(w);

  std::ofstream ofs(filename, std::ios::binary);
  if (!ofs) {
    std::cerr << "Error: Cannot open file: " << filename << "\n";
    exit(-1);
  }
  for (int i = 0; i < h; ++i) {
    uint16_t *row_ptr = plane_y + i * stride_y;
    for (int j = 0; j < w; ++j) {
      if (bit_depth == AOM_BITS_8 && row_ptr[j] > 255) {
        std::cerr << "Overflow: " << filename << " Value: " << row_ptr[j]
                  << "\n";
      } else if (bit_depth == AOM_BITS_10 && row_ptr[j] > 1023) {
        std::cerr << "Overflow: " << filename << " Value: " << row_ptr[j]
                  << "\n";
      } else if (bit_depth == AOM_BITS_12 && row_ptr[j] > 4095) {
        std::cerr << "Overflow: " << filename << " Value: " << row_ptr[j]
                  << "\n";
      }
      row_8bit[j] = static_cast<uint8_t>(row_ptr[j] & 0xFF);
      row_16bit[j] = row_ptr[j];
    }
    if (bit_depth == AOM_BITS_8) {
      ofs.write(reinterpret_cast<char *>(row_8bit.data()), w * sizeof(uint8_t));
    } else {
      ofs.write(reinterpret_cast<char *>(row_16bit.data()),
                w * sizeof(uint16_t));
    }
  }
  ofs.flush();
  // h >>= 1;
  // w >>= 1;
  // for (int uv = 0; uv < 2; ++uv) {
  //   uint16_t *plane_uv = NULL;
  //   if (uv == 0) {
  //     plane_uv = plane_u;
  //   } else {
  //     plane_uv = plane_v;
  //   }
  //   assert(plane_uv != NULL);

  //   for (int i = 0; i < h; ++i) {
  //     uint16_t *row_ptr = plane_uv + i * stride_uv;
  //     for (int j = 0; j < w; ++j) {
  //       if (bit_depth == AOM_BITS_8 && row_ptr[j] > 255) {
  //         std::cerr << "Overflow: " << filename << " Value: " << row_ptr[j]
  //                   << "\n";
  //       } else if (bit_depth == AOM_BITS_10 && row_ptr[j] > 1023) {
  //         std::cerr << "Overflow: " << filename << " Value: " << row_ptr[j]
  //                   << "\n";
  //       } else if (bit_depth == AOM_BITS_12 && row_ptr[j] > 4095) {
  //         std::cerr << "Overflow: " << filename << " Value: " << row_ptr[j]
  //                   << "\n";
  //       }
  //       row_8bit[j] = static_cast<uint8_t>(row_ptr[j] & 0xFF);
  //       row_16bit[j] = row_ptr[j];
  //     }
  //     if (bit_depth == AOM_BITS_8) {
  //       ofs.write(reinterpret_cast<char *>(row_8bit.data()),
  //                 w * sizeof(uint8_t));
  //     } else {
  //       ofs.write(reinterpret_cast<char *>(row_16bit.data()),
  //                 w * sizeof(uint16_t));
  //     }
  //   }
  // }
  // ofs.flush();
  ofs.close();
}

void nn_write_pred(const std::string &filename, uint16_t **rec,
                   const int *rec_strides, uint16_t **res,
                   const int *res_strides, int h, int w,
                   aom_bit_depth_t bit_depth) {
  assert(rec != NULL && res != NULL);
  assert(rec_strides != NULL && res_strides != NULL);

  uint16_t *plane_rec_y = rec[0];
  uint16_t *plane_rec_u = rec[1];
  uint16_t *plane_rec_v = rec[2];

  uint16_t *plane_res_y = res[0];
  uint16_t *plane_res_u = res[1];
  uint16_t *plane_res_v = res[2];

  std::vector<uint8_t> matrix_8bit(w * h);
  std::vector<uint16_t> matrix_16bit(w * h);

  std::ofstream ofs(filename, std::ios::binary);
  if (!ofs) {
    std::cerr << "Error: Cannot open file: " << filename << "\n";
    exit(-1);
  }
  // int cntpos = 0;
  // int cntneg = 0;
  // int cntzero = 0;
  for (int r = 0; r < h; ++r) {
    for (int c = 0; c < w; ++c) {
      // int value = plane_rec_y[r * rec_strides[0] + c] -
      //             plane_res_y[r * res_strides[0] + c];
      // if (value > 255) {
      //   std::cout<< "value_y: "<< value << "\n";
      // }
      // fprintf(stdout, "PCNN[Y -> %d = %d - %d]\n", value,
      //         plane_rec_y[r * rec_strides[0] + c],
      //         plane_res_y[r * res_strides[0] + c]);
      // if (plane_res_y[r * res_strides[0] + c] > 0) {
      //   cntpos++;
      // } else if (plane_res_y[r * res_strides[0] + c] < 0) {
      //   cntneg++;
      // } else {
      //   cntzero++;
      // }
      matrix_8bit[r * w + c] =
          static_cast<uint8_t>((plane_rec_y[r * rec_strides[0] + c] -
                                plane_res_y[r * res_strides[0] + c]) &
                               0xFF);
      matrix_16bit[r * w + c] =
          static_cast<uint16_t>((plane_rec_y[r * rec_strides[0] + c] -
                                 plane_res_y[r * res_strides[0] + c]) &
                                0xFFFF);
    }
  }
  if (bit_depth == AOM_BITS_8) {
    ofs.write(reinterpret_cast<char *>(matrix_8bit.data()),
              w * h * sizeof(uint8_t));
  } else {
    ofs.write(reinterpret_cast<char *>(matrix_16bit.data()),
              w * h * sizeof(uint16_t));
  }
  ofs.flush();
  h >>= 1;
  w >>= 1;
  for (int uv = 0; uv < 2; ++uv) {
    uint16_t *plane_rec_uv = NULL;
    uint16_t *plane_res_uv = NULL;
    if (uv == 0) {
      plane_rec_uv = plane_rec_u;
      plane_res_uv = plane_res_u;
    } else {
      plane_rec_uv = plane_rec_v;
      plane_res_uv = plane_res_v;
    }
    assert(plane_rec_uv != NULL && plane_res_uv != NULL);
    for (int r = 0; r < h; ++r) {
      for (int c = 0; c < w; ++c) {
        // int value = plane_rec_uv[r * rec_strides[1] + c] -
        //             plane_res_uv[r * res_strides[1] + c];
        // if (value > 255) {
        //   std::cout << "value_uv: " << value << "\n";
        // }
        // fprintf(stdout, "PCNN[%c -> %d = %d - %d]\n", uv == 0 ? 'U' : 'V',
        //         value, plane_rec_uv[r * rec_strides[1] + c],
        //         plane_res_uv[r * res_strides[1] + c]);
        // if (plane_res_uv[r * res_strides[1] + c] > 0) {
        //   cntpos++;
        // } else if (plane_res_uv[r * res_strides[1] + c] < 0) {
        //   cntneg++;
        // } else {
        //   cntzero++;
        // }
        matrix_8bit[r * w + c] =
            static_cast<uint8_t>((plane_rec_uv[r * rec_strides[1] + c] -
                                  plane_res_uv[r * res_strides[1] + c]) &
                                 0xFF);
        matrix_16bit[r * w + c] =
            static_cast<uint16_t>((plane_rec_uv[r * rec_strides[1] + c] -
                                   plane_res_uv[r * res_strides[1] + c]) &
                                  0xFFFF);
      }
    }
    if (bit_depth == AOM_BITS_8) {
      ofs.write(reinterpret_cast<char *>(matrix_8bit.data()),
                w * h * sizeof(uint8_t));
    } else {
      ofs.write(reinterpret_cast<char *>(matrix_16bit.data()),
                w * h * sizeof(uint16_t));
    }
    ofs.flush();
  }
  // fprintf(stdout, "PCNN pos: %d, neg: %d, zero: %d\n", cntpos, cntneg,
  // cntzero);
  ofs.close();
}

void nn_write_pred_v2(const std::string &filename, uint16_t **data,
                      const int *strides, int h, int w,
                      aom_bit_depth_t bit_depth) {
  uint16_t *plane_pred_y = data[0];
  uint16_t *plane_pred_u = data[1];
  uint16_t *plane_pred_v = data[2];

  std::ofstream ofs(filename, std::ios::binary);
  if (!ofs) {
    std::cerr << "Error: Cannot open file: " << filename << "\n";
    exit(-1);
  }
  std::vector<uint8_t> row_8bit(w);
  std::vector<uint16_t> row_16bit(w);
  for (int i = 0; i < h; ++i) {
    uint16_t *row_ptr = plane_pred_y + i * strides[0];
    for (int j = 0; j < w; ++j) {
      // if (row_ptr[j] < INT16_MIN || row_ptr[j] > INT16_MAX) {
      //   std::cerr << "Overflow: " << filename << " Value: " << row_ptr[j]
      //             << "\n";
      // }
      row_8bit[j] = static_cast<uint8_t>(row_ptr[j] & 0xFF);
      row_16bit[j] = static_cast<uint16_t>(row_ptr[j] & 0xFFFF);
    }
    if (bit_depth == AOM_BITS_8) {
      ofs.write(reinterpret_cast<char *>(row_8bit.data()), w * sizeof(uint8_t));
    } else {
      ofs.write(reinterpret_cast<char *>(row_16bit.data()),
                w * sizeof(uint16_t));
    }
  }

  h >>= 1;
  w >>= 1;
  ofs.flush();
  for (int uv = 0; uv < 2; ++uv) {
    uint16_t *plane_pred_uv = NULL;
    if (uv == 0) {
      plane_pred_uv = plane_pred_u;
    } else {
      plane_pred_uv = plane_pred_v;
    }
    for (int i = 0; i < h; ++i) {
      uint16_t *row_ptr = plane_pred_uv + i * strides[1];
      for (int j = 0; j < w; ++j) {
        // if (row_ptr[j] < INT16_MIN || row_ptr[j] > INT16_MAX) {
        //   std::cerr << "Overflow: " << filename << " Value: " << row_ptr[j]
        //             << "\n";
        // }
        row_8bit[j] = static_cast<uint8_t>(row_ptr[j] & 0xFF);
        row_16bit[j] = static_cast<uint16_t>(row_ptr[j] & 0xFFFF);
      }
      if (bit_depth == AOM_BITS_8) {
        ofs.write(reinterpret_cast<char *>(row_8bit.data()),
                  w * sizeof(uint8_t));
      } else {
        ofs.write(reinterpret_cast<char *>(row_16bit.data()),
                  w * sizeof(uint16_t));
      }
    }
  }
  ofs.flush();
  ofs.close();
}

void nn_write_res(const std::string &filename, uint16_t **data,
                  const int *strides, int h, int w) {
  uint16_t *plane_res_y = data[0];
  uint16_t *plane_res_u = data[1];
  uint16_t *plane_res_v = data[2];

  std::ofstream ofs(filename, std::ios::binary);
  if (!ofs) {
    std::cerr << "Error: Cannot open file: " << filename << "\n";
    exit(-1);
  }
  std::vector<uint16_t> row_16bit(w);
  for (int i = 0; i < h; ++i) {
    uint16_t *row_ptr = plane_res_y + i * strides[0];
    for (int j = 0; j < w; ++j) {
      // if (row_ptr[j] < INT16_MIN || row_ptr[j] > INT16_MAX) {
      //   std::cerr << "Overflow: " << filename << " Value: " << row_ptr[j]
      //             << "\n";
      // }
      row_16bit[j] = static_cast<uint16_t>(row_ptr[j] & 0xFFFF);
    }
    ofs.write(reinterpret_cast<char *>(row_16bit.data()), w * sizeof(uint16_t));
  }

  h >>= 1;
  w >>= 1;
  ofs.flush();
  // for (int uv = 0; uv < 2; ++uv) {
  //   uint16_t *plane_res_uv = NULL;
  //   if (uv == 0) {
  //     plane_res_uv = plane_res_u;
  //   } else {
  //     plane_res_uv = plane_res_v;
  //   }
  //   for (int i = 0; i < h; ++i) {
  //     uint16_t *row_ptr = plane_res_uv + i * strides[1];
  //     for (int j = 0; j < w; ++j) {
  //       // if (row_ptr[j] < INT16_MIN || row_ptr[j] > INT16_MAX) {
  //       //   std::cerr << "Overflow: " << filename << " Value: " <<
  //       row_ptr[j]
  //       //             << "\n";
  //       // }
  //       row_16bit[j] = static_cast<uint16_t>(row_ptr[j] & 0xFFFF);
  //     }
  //     ofs.write(reinterpret_cast<char *>(row_16bit.data()),
  //               w * sizeof(uint16_t));
  //   }
  // }
  // ofs.flush();
  ofs.close();
}

void nn_write_res_v2(const std::string &filename, uint16_t **rec,
                     const int *rec_strides, uint16_t **pred,
                     const int *pred_strides, int h, int w,
                     aom_bit_depth_t bit_depth) {
  assert(rec != NULL && pred != NULL);
  assert(rec_strides != NULL && pred_strides != NULL);

  uint16_t *plane_rec_y = rec[0];
  uint16_t *plane_rec_u = rec[1];
  uint16_t *plane_rec_v = rec[2];

  uint16_t *plane_pred_y = pred[0];
  uint16_t *plane_pred_u = pred[1];
  uint16_t *plane_pred_v = pred[2];

  std::vector<uint16_t> matrix_16bit(w * h);

  std::ofstream ofs(filename, std::ios::binary);
  if (!ofs) {
    std::cerr << "Error: Cannot open file: " << filename << "\n";
    exit(-1);
  }

  for (int r = 0; r < h; ++r) {
    for (int c = 0; c < w; ++c) {
      // int value = static_cast<int>(plane_rec_y[r * rec_strides[0] + c]) -
      //             plane_pred_y[r * pred_strides[0] + c];
      // fprintf(stdout, "PCNN[Y -> %d = %d - %d]\n", value,
      //         plane_rec_y[r * rec_strides[0] + c],
      //         plane_pred_y[r * pred_strides[0] + c]);
      matrix_16bit[r * w + c] = static_cast<uint16_t>(
          (static_cast<int>(plane_rec_y[r * rec_strides[0] + c]) -
           plane_pred_y[r * pred_strides[0] + c]) &
          0xFFFF);
    }
  }

  ofs.write(reinterpret_cast<char *>(matrix_16bit.data()),
            w * h * sizeof(uint16_t));

  ofs.flush();
  h >>= 1;
  w >>= 1;
  for (int uv = 0; uv < 2; ++uv) {
    uint16_t *plane_rec_uv = NULL;
    uint16_t *plane_pred_uv = NULL;
    if (uv == 0) {
      plane_rec_uv = plane_rec_u;
      plane_pred_uv = plane_pred_u;
    } else {
      plane_rec_uv = plane_rec_v;
      plane_pred_uv = plane_pred_v;
    }
    assert(plane_rec_uv != NULL && plane_pred_uv != NULL);
    for (int r = 0; r < h; ++r) {
      for (int c = 0; c < w; ++c) {
        // int value = static_cast<int>(plane_rec_uv[r * rec_strides[1] + c]) -
        //             plane_pred_uv[r * pred_strides[1] + c];
        // fprintf(stdout, "PCNN[%c -> %d = %d - %d]\n", uv == 0 ? 'U' : 'V',
        //         value, plane_rec_uv[r * rec_strides[1] + c],
        //         plane_pred_uv[r * pred_strides[1] + c]);
        matrix_16bit[r * w + c] = static_cast<uint16_t>(
            ((static_cast<int>(plane_rec_uv[r * rec_strides[1] + c])) -
             plane_pred_uv[r * pred_strides[1] + c]) &
            0xFFFF);
      }
    }
    ofs.write(reinterpret_cast<char *>(matrix_16bit.data()),
              w * h * sizeof(uint16_t));
    ofs.flush();
  }
  // fprintf(stdout, "PCNN pos: %d, neg: %d, zero: %d\n", cntpos, cntneg,
  // cntzero);
  ofs.close();
}

void nn_write_qp(const std::string &filename, int qp,
                 aom_bit_depth_t bit_depth) {
  std::ofstream ofs(filename, std::ios::binary | std::ios::app);
  if (!ofs) {
    std::cerr << "Error: Cannot open file: " << filename << "\n";
    exit(-1);
  }
  ofs.write(reinterpret_cast<const char *>(&qp), sizeof(uint8_t));

  // std::ofstream ofs1(filename + std::to_string(bit_depth) + "bit_depth",
  //                    std::ios::binary);
  // if (ofs1) {
  //   std::cout << filename + "[bit_depth: " << bit_depth << "]\n";
  // }
}

void nn_lf_make_datasets(AV1_COMP *cpi, AV1_COMMON *cm) {
  assert(cpi != NULL && cm != NULL);

  if (frame_is_intra_only(cm)) return;  // inter 不需要 key frame

  std::string dataset_file_name = nn_get_dataset_file_name(
      dataset_file_path, cm->cur_frame->height, cm->cur_frame->width,
      cm->cur_frame->absolute_poc, frame_is_intra_only(cm),
      cm->quant_params.base_qindex, cm->seq_params.bit_depth);
  if (dataset_file_name.empty() || dataset_file_name.size() == 0) {
    std::cerr << "Error: Invalid dataset file path: " << dataset_file_path
              << "\n";
    exit(-1);
  }

  std::string dataset_output_path =
      datasets_output_path + "/qp" + std::to_string(dataset_qp);
  std::vector<std::string> dataset_output_paths{
    dataset_output_path + "/src/",  // 0
    dataset_output_path + "/rec/",       dataset_output_path + "/bs/",
    dataset_output_path + "/post_dblk/", dataset_output_path + "/post_cdef/",
    dataset_output_path + "/post_ccso/", dataset_output_path + "/post_lr/",
    dataset_output_path + "/post_gdf/",  dataset_output_path + "/cnn_out/",
    dataset_output_path + "/pred/",      dataset_output_path + "/res/",
    dataset_output_path + "/qp/"
  };

  for (auto &str : dataset_output_paths) {
    std::error_code ec;  // 可选：用于捕获错误，避免抛异常
    bool created = std::filesystem::create_directories(str, ec);

    if (!ec) {
      if (created) {
        std::cout << "目录已创建: " << str << "\n";
      } else {
        std::cout << "目录已存在: " << str << "\n";
      }
    } else {
      std::cerr << "创建目录失败: " << ec.message() << "\n";
      exit(-1);
    }
    str += dataset_file_name;
  }

  std::vector<YV12_BUFFER_CONFIG *> dataset_output_buffers{
    cpi->source,        &cpi->nn_dblk_input, &cpi->bs_buffer,
    &cpi->nn_post_dblk, &cpi->nn_post_cdef,  &cpi->nn_post_ccso,
    &cpi->nn_post_lr,   &cpi->nn_post_gdf,   &cpi->cnn_out
  };

  // output frames
  for (std::size_t i = 0; i < dataset_output_buffers.size(); ++i) {
    if (i == 1 || i == 2 || i == 5) {
      nn_write_frame(
          dataset_output_paths[i], dataset_output_buffers[i]->buffers,
          dataset_output_buffers[i]->strides,
          dataset_output_buffers[i]->y_crop_height,
          dataset_output_buffers[i]->y_crop_width, cm->seq_params.bit_depth);
    }
    if (i == 0 && cm->quant_params.base_qindex == 235) {
      nn_write_frame(
          dataset_output_paths[i], dataset_output_buffers[i]->buffers,
          dataset_output_buffers[i]->strides,
          dataset_output_buffers[i]->y_crop_height,
          dataset_output_buffers[i]->y_crop_width, cm->seq_params.bit_depth);
    }
  }

  // output pred
  // nn_write_pred_v2(
  //     dataset_output_path + "/pred/" + dataset_file_name,
  //     cm->cur_frame_residue->buf.buffers, cm->cur_frame_residue->buf.strides,
  //     cm->cur_frame_residue->buf.y_crop_height,
  //     cm->cur_frame_residue->buf.y_crop_width, cm->seq_params.bit_depth);
  nn_write_pred(dataset_output_path + "/pred/" + dataset_file_name,
                cpi->nn_dblk_input.buffers, cpi->nn_dblk_input.strides,
                cm->cur_frame_residue->buf.buffers,
                cm->cur_frame_residue->buf.strides,
                cpi->nn_dblk_input.y_crop_height,
                cpi->nn_dblk_input.y_crop_width, cm->seq_params.bit_depth);

  // nn_create_dir((dataset_output_path + "/predv2/").c_str());
  // nn_write_pred_v2(dataset_output_path + "/predv2/" + dataset_file_name,
  //                  pred_ptr->buffers, pred_ptr->strides,
  //                  pred_ptr->y_crop_height, pred_ptr->y_crop_width,
  //                  cm->seq_params.bit_depth);

  // output res
  // nn_write_res_v2(dataset_output_path + "/res/" + dataset_file_name,
  //                 cpi->nn_dblk_input.buffers, cpi->nn_dblk_input.strides,
  //                 cm->cur_frame_residue->buf.buffers,
  //                 cm->cur_frame_residue->buf.strides,
  //                 cpi->nn_dblk_input.y_crop_height,
  //                 cpi->nn_dblk_input.y_crop_width, cm->seq_params.bit_depth);
  nn_write_res(dataset_output_path + "/res/" + dataset_file_name,
               cm->cur_frame_residue->buf.buffers,
               cm->cur_frame_residue->buf.strides,
               cm->cur_frame_residue->buf.y_crop_height,
               cm->cur_frame_residue->buf.y_crop_width);

  // output qp and bit depth
  nn_write_qp(dataset_output_path + "/qp/" + dataset_file_name,
              cm->quant_params.base_qindex, cm->seq_params.bit_depth);
}
#endif

#if CONFIG_MSCNN && 000
#include <torch/script.h>
std::map<std::string, torch::jit::script::Module> module_map;
std::map<std::string, torch::jit::script::Module>::iterator it;

extern "C" int nn_loopfilter_jit(const char *model_location,
                                 uint16_t **dec_data, int *dec_data_strides,
                                 int dec_byte_count, int h, int w,
                                 uint16_t **dec_residue,
                                 int *dec_residue_strides,
                                 uint16_t **dblk_input);

int nn_loopfilter_jit(const char *model_location, uint16_t **dec_data,
                      int *dec_data_strides, int dec_byte_count, int h, int w,
                      uint16_t **dec_residue, int *dec_residue_strides,
                      uint16_t **dblk_input) {
  torch::Device device(torch::kCPU);
  torch::jit::script::Module module;
  try {
    // Deserialize the ScriptModule from a file using torch::jit::load().
    it = module_map.find(model_location);
    if (it != module_map.end())
      module = module_map[std::string(model_location)];
    else {
      module = torch::jit::load(model_location, device);
      module_map[std::string(model_location)] = module;
    }
  } catch (const c10::Error &e) {
    std::cerr << "error loading the model: " << model_location << "\n";
    exit(0);
  }
  c10::InferenceMode guard;

  // copy decoded data
  at::Tensor dec_tensor;
  dec_tensor =
      torch::zeros({ 1, 3, h, w }, torch::TensorOptions().dtype(torch::kInt16));
  uint16_t *plane_src = dec_data[0];
  int32_t *plane_residue = (int32_t *)dec_residue[0];
  uint16_t *plane_dblk_input = dblk_input[0];
  for (int h_idx = 0; h_idx < h; h_idx++) {
    // seems like we always get 16-bit buffers
    dec_tensor.slice(1, 0, 1).slice(2, h_idx, h_idx + 1).slice(3, 0, w) =
        torch::from_blob(plane_src, { w },
                         torch::TensorOptions().dtype(torch::kInt16));
    dec_tensor.slice(1, 1, 2).slice(2, h_idx, h_idx + 1).slice(3, 0, w) =
        torch::from_blob(plane_residue, { w },
                         torch::TensorOptions().dtype(torch::kInt32));
    dec_tensor.slice(1, 2, 3).slice(2, h_idx, h_idx + 1).slice(3, 0, w) =
        torch::from_blob(plane_dblk_input, { w },
                         torch::TensorOptions().dtype(torch::kInt16));
    plane_src += dec_data_strides[0];
    plane_residue += dec_residue_strides[0];
    plane_dblk_input += dec_data_strides[0];
  }
  dec_tensor = dec_tensor.toType(torch::kFloat32).to(device);

  if (dec_byte_count ==
      1) {  // current support is for 8-bit and 10-bit data only
    dec_tensor = dec_tensor * 4.0;  // TODO: Overflow prevention
  }

  // forward pass
  std::vector<torch::jit::IValue> inputs;
  inputs.push_back(dec_tensor);
  at::Tensor pred_tensor = module.forward(inputs).toTensor();

  pred_tensor = pred_tensor *
                16.0;  // to counter loss in rounding. TODO: Overflow prevention

  if (dec_byte_count == 1) {  // TODO: current support is for 8-bit and 10-bit
                              // data only, extend for other bitdepths
    pred_tensor = pred_tensor.round();
  } else {
    pred_tensor = pred_tensor.round();
  }

  // copy filtered data
  uint16_t *plane_dst = dec_data[0];
  for (int h_idx = 0; h_idx < h; h_idx++) {
    for (int w_idx = 0; w_idx < w; w_idx++) {
      plane_dst[w_idx] =
          (uint16_t)(pred_tensor[0][0][h_idx][w_idx].item<float>());
    }
    plane_dst += dec_data_strides[0];
  }

  return 0;
}
#endif
