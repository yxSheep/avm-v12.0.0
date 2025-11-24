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

#include <torch/script.h>
#include <iostream>
#include <memory>
#include <stdint.h>
#include "config/aom_config.h"

#if CONFIG_MSCNN
std::map<std::string, torch::jit::script::Module> module_map;
std::map<std::string,torch::jit::script::Module>::iterator it;

extern "C" int nn_loopfilter_jit(const char *model_location, uint16_t **dec_data,
                                 int *dec_data_strides, int dec_byte_count,
                                 int h, int w, uint16_t **dec_residue,
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
  }
  catch (const c10::Error& e) {
    std::cerr << "error loading the model: " << model_location << "\n";
    exit(0);
  }
  c10::InferenceMode guard;

  // copy decoded data 
  at::Tensor dec_tensor;
  dec_tensor = torch::zeros({ 1, 3, h, w }, torch::TensorOptions().dtype(torch::kInt16));
  uint16_t *plane_src = dec_data[0];
  int32_t  *plane_residue = (int32_t *)dec_residue[0];
  uint16_t *plane_dblk_input = dblk_input[0];
  for (int h_idx = 0; h_idx < h; h_idx++) {
    // seems like we always get 16-bit buffers
    dec_tensor.slice(1, 0, 1).slice(2, h_idx, h_idx+1).slice(3, 0, w) = torch::from_blob(plane_src, {w}, torch::TensorOptions().dtype(torch::kInt16));
    dec_tensor.slice(1, 1, 2).slice(2, h_idx, h_idx+1).slice(3, 0, w) = torch::from_blob(plane_residue, {w}, torch::TensorOptions().dtype(torch::kInt32));
    dec_tensor.slice(1, 2, 3).slice(2, h_idx, h_idx+1).slice(3, 0, w) = torch::from_blob(plane_dblk_input, {w}, torch::TensorOptions().dtype(torch::kInt16));
    plane_src += dec_data_strides[0];
    plane_residue += dec_residue_strides[0];
    plane_dblk_input += dec_data_strides[0];
  }
  dec_tensor = dec_tensor.toType(torch::kFloat32).to(device);

  if (dec_byte_count==1) { // current support is for 8-bit and 10-bit data only
    dec_tensor = dec_tensor * 4.0; // TODO: Overflow prevention
  }

  // forward pass
  std::vector<torch::jit::IValue> inputs;
  inputs.push_back(dec_tensor);
  at::Tensor pred_tensor = module.forward(inputs).toTensor();

  pred_tensor = pred_tensor * 16.0; // to counter loss in rounding. TODO: Overflow prevention

  if (dec_byte_count==1) { // TODO: current support is for 8-bit and 10-bit data only, extend for other bitdepths
    pred_tensor = pred_tensor.round();
  } else {
    pred_tensor = pred_tensor.round();
  }

  // copy filtered data
  uint16_t *plane_dst = dec_data[0];
  for (int h_idx = 0; h_idx < h; h_idx++) {
    for (int w_idx = 0; w_idx < w; w_idx++) {
      plane_dst[w_idx] = (uint16_t) (pred_tensor[0][0][h_idx][w_idx].item<float>());
    }
    plane_dst += dec_data_strides[0];
  }
  
  return 0;
}
#endif
