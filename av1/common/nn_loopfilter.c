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

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <math.h>
#include "aom/aom_integer.h"
#include "av1/common/av1_common_int.h"
#include "av1/common/nn_loopfilter.h"
#include "config/aom_config.h"

#if CONFIG_MSCNN
const int blk_sizes[BLK_SIZE_COUNT] = { 16, 32, 64,
                                        128 };  // max is superblock size

int get_qp_idx(int qindex, int is_intra_only, int bit_depth) {
  // 调整 10 bit 和 12 bit 的 qindex
  int qindex_adjust = qindex - 24 * (bit_depth - 8);
  if (is_intra_only) {
    if (qindex_adjust <= 90) {  // 85
      return 5;
    } else if (qindex_adjust <= 120) {  // 110
      return 4;
    } else if (qindex_adjust <= 145) {  // 135
      return 3;
    } else if (qindex_adjust <= 175) {  // 160
      return 2;
    } else if (qindex_adjust <= 205) {  // 185
      return 1;
    } else {
      return 0;
    }
  } else {
    if (qindex_adjust <= 110) {
      return 5;
    } else if (qindex_adjust <= 135) {
      return 4;
    } else if (qindex_adjust <= 160) {
      return 3;
    } else if (qindex_adjust <= 185) {
      return 2;
    } else if (qindex_adjust <= 210) {
      return 1;
    } else {
      return 0;
    }
  }
  return -1;
}

void nn_loopfilter_interpred(YV12_BUFFER_CONFIG *buffer,
                             YV12_BUFFER_CONFIG *residue,
                             YV12_BUFFER_CONFIG *dblk_input,
                             aom_bit_depth_t bit_depth, int qindex,
                             int model_idx) {}

void my_nn_loopfilter_c(AV1_COMMON *cm, YV12_BUFFER_CONFIG *src,
                        YV12_BUFFER_CONFIG *ccso, YV12_BUFFER_CONFIG *rec,
                        YV12_BUFFER_CONFIG *res, YV12_BUFFER_CONFIG *bs,
                        YV12_BUFFER_CONFIG *cnn_out, int RDMULT,
                        int *cnn_guided_mode_costs, int (*norestore_costs)[2],
                        int (*codebook_costs)[CODEBOOK_CHANNEL][256],
                        AdpGuidedInfo *adp_guided_info,
                        const aom_bit_depth_t bit_depth, const int q_index,
                        int is_intra_only, int is_luma, double *rdcost) {
  cnn_loopfilter(
      cm, src->buffers, src->strides, ccso->buffers, ccso->strides, bit_depth,
      ccso->y_height, ccso->y_width, rec->buffers, rec->strides, res->buffers,
      res->strides, bs->buffers, bs->strides, cnn_out->buffers,
      cnn_out->strides, adp_guided_info, is_intra_only, q_index, is_luma,
      RDMULT, cnn_guided_mode_costs, norestore_costs, codebook_costs, rdcost);
}
#endif

#if CONFIG_MSCNN && 000
const int blk_sizes[BLK_SIZE_COUNT] = { 16, 32, 64,
                                        128 };  // max is superblock size

int nn_loopfilter_jit(const char *model_location, uint16_t **dec_data,
                      int *dec_data_strides, int dec_byte_count, int h, int w,
                      uint16_t **dec_residue, int *dec_residue_strides,
                      uint16_t **dblk_input);

void nn_loopfilter(YV12_BUFFER_CONFIG *buffer, YV12_BUFFER_CONFIG *residue,
                   YV12_BUFFER_CONFIG *dblk_input, aom_bit_depth_t bit_depth,
                   int qindex, int model_idx) {
  char key[1024];
  int qindex_adjust = qindex - 24 * (bit_depth - 8);

  sprintf(key, "%s/intra_frame_models/qplb_000_qpub_099/checkpoint_%d.pth.jit",
          TORCH_MODELS_PATH, model_idx);
  if (qindex_adjust <= 99) {
    sprintf(key,
            "%s/intra_frame_models/qplb_000_qpub_099/checkpoint_%d.pth.jit",
            TORCH_MODELS_PATH, model_idx);
  } else if (qindex_adjust <= 124) {
    sprintf(key,
            "%s/intra_frame_models/qplb_100_qpub_124/checkpoint_%d.pth.jit",
            TORCH_MODELS_PATH, model_idx);
  } else if (qindex_adjust <= 149) {
    sprintf(key,
            "%s/intra_frame_models/qplb_125_qpub_149/checkpoint_%d.pth.jit",
            TORCH_MODELS_PATH, model_idx);
  } else if (qindex_adjust <= 174) {
    sprintf(key,
            "%s/intra_frame_models/qplb_150_qpub_174/checkpoint_%d.pth.jit",
            TORCH_MODELS_PATH, model_idx);
  } else if (qindex_adjust <= 199) {
    sprintf(key,
            "%s/intra_frame_models/qplb_175_qpub_199/checkpoint_%d.pth.jit",
            TORCH_MODELS_PATH, model_idx);
  } else {
    sprintf(key,
            "%s/intra_frame_models/qplb_200_qpub_500/checkpoint_%d.pth.jit",
            TORCH_MODELS_PATH, model_idx);
  }

  nn_loopfilter_jit(key, buffer->buffers, buffer->strides,
                    (bit_depth > 8) ? 2 : 1, buffer->y_height, buffer->y_width,
                    residue->buffers, residue->strides, dblk_input->buffers);
}

void nn_loopfilter_interpred(YV12_BUFFER_CONFIG *buffer,
                             YV12_BUFFER_CONFIG *residue,
                             YV12_BUFFER_CONFIG *dblk_input,
                             aom_bit_depth_t bit_depth, int qindex,
                             int model_idx) {
  char key[1024];
  int qindex_adjust = qindex - 24 * (bit_depth - 8);

  sprintf(key, "%s/inter_frame_models/qplb_000_qpub_110/checkpoint_%d.pth.jit",
          TORCH_MODELS_PATH, model_idx);
  if (qindex_adjust <= 110) {
    sprintf(key,
            "%s/inter_frame_models/qplb_000_qpub_110/checkpoint_%d.pth.jit",
            TORCH_MODELS_PATH, model_idx);
  } else if (qindex_adjust <= 135) {
    sprintf(key,
            "%s/inter_frame_models/qplb_111_qpub_135/checkpoint_%d.pth.jit",
            TORCH_MODELS_PATH, model_idx);
  } else if (qindex_adjust <= 160) {
    sprintf(key,
            "%s/inter_frame_models/qplb_136_qpub_160/checkpoint_%d.pth.jit",
            TORCH_MODELS_PATH, model_idx);
  } else if (qindex_adjust <= 185) {
    sprintf(key,
            "%s/inter_frame_models/qplb_161_qpub_185/checkpoint_%d.pth.jit",
            TORCH_MODELS_PATH, model_idx);
  } else if (qindex_adjust <= 210) {
    sprintf(key,
            "%s/inter_frame_models/qplb_186_qpub_210/checkpoint_%d.pth.jit",
            TORCH_MODELS_PATH, model_idx);
  } else {
    sprintf(key,
            "%s/inter_frame_models/qplb_211_qpub_500/checkpoint_%d.pth.jit",
            TORCH_MODELS_PATH, model_idx);
  }

  nn_loopfilter_jit(key, buffer->buffers, buffer->strides,
                    (bit_depth > 8) ? 2 : 1, buffer->y_height, buffer->y_width,
                    residue->buffers, residue->strides, dblk_input->buffers);
}
#endif
