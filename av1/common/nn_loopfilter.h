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

#ifndef AOM_AV1_COMMON_NN_LOOPFILTER_H_
#define AOM_AV1_COMMON_NN_LOOPFILTER_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "aom_scale/yv12config.h"
#include "config/aom_config.h"

#if CONFIG_MSCNN
#include "av1/common/restoration.h"
#include "av1/common/av1_common_int.h"
void nn_loopfilter(YV12_BUFFER_CONFIG *buffer, YV12_BUFFER_CONFIG *residue,
                   YV12_BUFFER_CONFIG *dblk_input, aom_bit_depth_t bit_depth,
                   int qindex, int model_idx);

extern void my_nn_loopfilter_c(
    AV1_COMMON *cm, YV12_BUFFER_CONFIG *src, YV12_BUFFER_CONFIG *ccso,
    YV12_BUFFER_CONFIG *rec, YV12_BUFFER_CONFIG *res, YV12_BUFFER_CONFIG *bs,
    YV12_BUFFER_CONFIG *cnn_out, int RDMULT, int *cnn_guided_mode_costs,
    int (*norestore_costs)[2], int (*codebook_costs)[CODEBOOK_CHANNEL][256],
    AdpGuidedInfo *adp_guided_info, const aom_bit_depth_t bit_depth,
    const int q_index, int is_intra_only, int is_luma, double *rdcost);

void cnn_loopfilter(AV1_COMMON *cm, uint16_t **src_data, int *src_strides,
                    uint16_t **ccso_data, int *ccso_data_strides, int bitdepth,
                    int h, int w, uint16_t **dec_rec, int *dec_rec_strides,
                    uint16_t **dec_res, int *dec_res_strides, uint16_t **dec_bs,
                    int *dec_bs_strides, uint16_t **cnn_out,
                    int *cnn_out_strides, AdpGuidedInfo *adp_guided_info,
                    int is_intra_only, int q_index,
                    int is_luma, int RDMULT, int *cnn_guided_mode_costs,
                    int (*norestore_costs)[2],
                    int (*codebook_costs)[CODEBOOK_CHANNEL][256],
                    double *rdcost);

extern int get_qp_idx(int qindex, int is_intra_only, int bit_depth);
#endif

#if CONFIG_MSCNN && 000
void nn_loopfilter(YV12_BUFFER_CONFIG *buffer, YV12_BUFFER_CONFIG *residue,
                   YV12_BUFFER_CONFIG *dblk_input, aom_bit_depth_t bit_depth,
                   int qindex, int model_idx);
void nn_loopfilter_interpred(YV12_BUFFER_CONFIG *buffer,
                             YV12_BUFFER_CONFIG *residue,
                             YV12_BUFFER_CONFIG *dblk_input,
                             aom_bit_depth_t bit_depth, int qindex,
                             int model_idx);
#endif

#ifdef __cplusplus
}
#endif
#endif  // AOM_AV1_COMMON_NN_LOOPFILTER_H_
