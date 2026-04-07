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

#include <stdlib.h>

#include "config/aom_config.h"
#include "config/aom_dsp_rtcd.h"

#include "aom_dsp/aom_dsp_common.h"
#include "aom_dsp/loopfilter.h"
#include "aom_ports/mem.h"

// #if CONFIG_MSCNN
// #define CNN_MAX_DBL_FLT_LEN 8
// static const int bs_q_map_8bit[CNN_MAX_DBL_FLT_LEN] = { 85, 170, 255, 85, 0, 170,
//                                                     0, 255 };
// static const int bs_q_map_10bit[CNN_MAX_DBL_FLT_LEN] = { 341, 682, 1023, 341, 0, 682,
//                                                      0, 1023 };
// #endif

#if CONFIG_MSCNN
#define CNN_MAX_DBL_FLT_LEN 8
static const int bs_q_map_8bit[CNN_MAX_DBL_FLT_LEN] = { 64, 128, 191, 255, 0, 128,
                                                    0, 255 };
static const int bs_q_map_10bit[CNN_MAX_DBL_FLT_LEN] = { 256, 512, 767, 1023, 0, 512,
                                                     0, 1023 };
#endif

#if CONFIG_MSCNN
static INLINE void nn_filt_generic_asym_highbd(int q_threshold, int width_neg,
                                               int width_pos, uint16_t *s,
                                               uint16_t *bs_s, const int pitch,
                                               int bs_pitch, int bd
#if CONFIG_DISABLE_LOOP_FILTERS_LOSSLESS
                                               ,
                                               int is_lossless_neg,
                                               int is_lossless_pos
#endif  // CONFIG_DISABLE_LOOP_FILTERS_LOSSLESS

) {
  if (width_neg < 1) return; // val: 1 2 3 4 6 8
  if (width_pos < 1) return; // idx: 0 1 2 3 5 7

  int width = AOMMAX(width_neg, width_pos);
  int delta_m2 = (3 * (s[0] - s[-1 * pitch]) - (s[pitch] - s[-2 * pitch])) * 4;

  int q_thresh_clamp = q_threshold * q_thresh_mults[width - 1];
  delta_m2 = clamp(delta_m2, -q_thresh_clamp, q_thresh_clamp);

#if CONFIG_MSCNN
  const int *bs_q_map = bd == 8 ? bs_q_map_8bit : bs_q_map_10bit;
#endif

#if CONFIG_DISABLE_LOOP_FILTERS_LOSSLESS
  if (!is_lossless_neg) {
#endif  // CONFIG_DISABLE_LOOP_FILTERS_LOSSLESS
    int delta_m2_neg = delta_m2 * w_mult[width_neg - 1];
    for (int i = 0; i < width_neg; i++) {
      s[(-i - 1) * pitch] = clip_pixel_highbd(
          s[(-i - 1) * pitch] +
              ROUND_POWER_OF_TWO(delta_m2_neg * (width_neg - i), 3 + DF_SHIFT),
          bd);
#if CONFIG_MSCNN
      if (i == 0) {
        bs_s[(-i - 1) * pitch] =
            AOMMAX(bs_q_map[width_neg - 1], bs_s[(-i - 1) * pitch]);
      }
#endif
    }
#if CONFIG_DISABLE_LOOP_FILTERS_LOSSLESS
  }
  if (!is_lossless_pos) {
#endif  // CONFIG_DISABLE_LOOP_FILTERS_LOSSLESS
    int delta_m2_pos = delta_m2 * w_mult[width_pos - 1];
    for (int i = 0; i < width_pos; i++) {
      s[i * pitch] = clip_pixel_highbd(
          s[i * pitch] -
              ROUND_POWER_OF_TWO(delta_m2_pos * (width_pos - i), 3 + DF_SHIFT),
          bd);
#if CONFIG_MSCNN
      if (i == 0) {
        bs_s[i * pitch] = AOMMAX(bs_q_map[width_pos - 1], bs_s[i * pitch]);
      }
#endif
    }
#if CONFIG_DISABLE_LOOP_FILTERS_LOSSLESS
  }
#endif  // CONFIG_DISABLE_LOOP_FILTERS_LOSSLESS
}
#endif

static INLINE void filt_generic_asym_highbd(int q_threshold, int width_neg,
                                            int width_pos, uint16_t *s,
                                            const int pitch, int bd
#if CONFIG_DISABLE_LOOP_FILTERS_LOSSLESS
                                            ,
                                            int is_lossless_neg,
                                            int is_lossless_pos
#endif  // CONFIG_DISABLE_LOOP_FILTERS_LOSSLESS

) {
  if (width_neg < 1) return;
  if (width_pos < 1) return;

  int width = AOMMAX(width_neg, width_pos);
  int delta_m2 = (3 * (s[0] - s[-1 * pitch]) - (s[pitch] - s[-2 * pitch])) * 4;

  int q_thresh_clamp = q_threshold * q_thresh_mults[width - 1];
  delta_m2 = clamp(delta_m2, -q_thresh_clamp, q_thresh_clamp);

#if CONFIG_DISABLE_LOOP_FILTERS_LOSSLESS
  if (!is_lossless_neg) {
#endif  // CONFIG_DISABLE_LOOP_FILTERS_LOSSLESS
    int delta_m2_neg = delta_m2 * w_mult[width_neg - 1];
    for (int i = 0; i < width_neg; i++) {
      s[(-i - 1) * pitch] = clip_pixel_highbd(
          s[(-i - 1) * pitch] +
              ROUND_POWER_OF_TWO(delta_m2_neg * (width_neg - i), 3 + DF_SHIFT),
          bd);
    }
#if CONFIG_DISABLE_LOOP_FILTERS_LOSSLESS
  }
  if (!is_lossless_pos) {
#endif  // CONFIG_DISABLE_LOOP_FILTERS_LOSSLESS
    int delta_m2_pos = delta_m2 * w_mult[width_pos - 1];
    for (int i = 0; i < width_pos; i++) {
      s[i * pitch] = clip_pixel_highbd(
          s[i * pitch] -
              ROUND_POWER_OF_TWO(delta_m2_pos * (width_pos - i), 3 + DF_SHIFT),
          bd);
    }
#if CONFIG_DISABLE_LOOP_FILTERS_LOSSLESS
  }
#endif  // CONFIG_DISABLE_LOOP_FILTERS_LOSSLESS
}

#if CONFIG_MSCNN
void nn_aom_highbd_lpf_horizontal_generic_c(uint16_t *s, uint16_t *bs_s, int pitch,
                                         int bs_pitch, int filt_width_neg,
                                         int filt_width_pos,
                                         const uint16_t *q_thresh,
                                         const uint16_t *side_thresh, int bd
#if CONFIG_DISABLE_LOOP_FILTERS_LOSSLESS
                                         ,
                                         int is_lossless_neg,
                                         int is_lossless_pos
#endif  // CONFIG_DISABLE_LOOP_FILTERS_LOSSLESS
) {
  int i;

  int count = 4;

  int filt_neg = (filt_width_neg >> 1) - 1;
  int filter = filt_choice_highbd(s, pitch, filt_width_neg, filt_width_pos,
                                  *q_thresh, *side_thresh, s + count - 1);

  for (i = 0; i < count; ++i) {
#if CONFIG_MSCNN
    nn_filt_generic_asym_highbd(*q_thresh, AOMMIN(filter, filt_neg), filter, s,
                             bs_s, pitch, bs_pitch, bd
#if CONFIG_DISABLE_LOOP_FILTERS_LOSSLESS
                             ,
                             is_lossless_neg, is_lossless_pos
#endif  // CONFIG_DISABLE_LOOP_FILTERS_LOSSLESS
    );
    ++s;
    ++bs_s;
#else
    filt_generic_asym_highbd(*q_thresh, AOMMIN(filter, filt_neg), filter, s,
                             pitch, bd
#if CONFIG_DISABLE_LOOP_FILTERS_LOSSLESS
                             ,
                             is_lossless_neg, is_lossless_pos
#endif  // CONFIG_DISABLE_LOOP_FILTERS_LOSSLESS
    );
    ++s;
#endif
  }
}
#endif

void aom_highbd_lpf_horizontal_generic_c(uint16_t *s, int pitch,
                                         int filt_width_neg, int filt_width_pos,
                                         const uint16_t *q_thresh,
                                         const uint16_t *side_thresh, int bd
#if CONFIG_DISABLE_LOOP_FILTERS_LOSSLESS
                                         ,
                                         int is_lossless_neg,
                                         int is_lossless_pos
#endif  // CONFIG_DISABLE_LOOP_FILTERS_LOSSLESS
) {
  int i;

  int count = 4;

  int filt_neg = (filt_width_neg >> 1) - 1;
  int filter = filt_choice_highbd(s, pitch, filt_width_neg, filt_width_pos,
                                  *q_thresh, *side_thresh, s + count - 1);

  for (i = 0; i < count; ++i) {
    filt_generic_asym_highbd(*q_thresh, AOMMIN(filter, filt_neg), filter, s,
                             pitch, bd
#if CONFIG_DISABLE_LOOP_FILTERS_LOSSLESS
                             ,
                             is_lossless_neg, is_lossless_pos
#endif  // CONFIG_DISABLE_LOOP_FILTERS_LOSSLESS
    );
    ++s;
  }
}

#if CONFIG_MSCNN
void nn_aom_highbd_lpf_vertical_generic_c(uint16_t *s, uint16_t *bs_s, int pitch,
                                       int bs_pitch, int filt_width_neg,
                                       int filt_width_pos,
                                       const uint16_t *q_thresh,
                                       const uint16_t *side_thresh, int bd
#if CONFIG_DISABLE_LOOP_FILTERS_LOSSLESS
                                       ,
                                       int is_lossless_neg, int is_lossless_pos
#endif  // CONFIG_DISABLE_LOOP_FILTERS_LOSSLESS
) {
  int i;
  int count = 4;

  int filt_neg = (filt_width_neg >> 1) - 1;
  int filter =
      filt_choice_highbd(s, 1, filt_width_neg, filt_width_pos, *q_thresh,
                         *side_thresh, s + (count - 1) * pitch);

  // loop filter designed to work using chars so that we can make maximum use
  // of 8 bit simd instructions.
  for (i = 0; i < count; ++i) {
#if CONFIG_MSCNN
    nn_filt_generic_asym_highbd(*q_thresh, AOMMIN(filter, filt_neg), filter, s,
                             bs_s, 1, 1, bd
#if CONFIG_DISABLE_LOOP_FILTERS_LOSSLESS
                             ,
                             is_lossless_neg, is_lossless_pos
#endif  // CONFIG_DISABLE_LOOP_FILTERS_LOSSLESS
    );
    s += pitch;
    bs_s += bs_pitch;
#else
    filt_generic_asym_highbd(*q_thresh, AOMMIN(filter, filt_neg), filter, s, 1,
                             bd
#if CONFIG_DISABLE_LOOP_FILTERS_LOSSLESS
                             ,
                             is_lossless_neg, is_lossless_pos
#endif  // CONFIG_DISABLE_LOOP_FILTERS_LOSSLESS
    );
    s += pitch;
#endif
  }
}
#endif

void aom_highbd_lpf_vertical_generic_c(uint16_t *s, int pitch,
                                       int filt_width_neg, int filt_width_pos,
                                       const uint16_t *q_thresh,
                                       const uint16_t *side_thresh, int bd
#if CONFIG_DISABLE_LOOP_FILTERS_LOSSLESS
                                       ,
                                       int is_lossless_neg, int is_lossless_pos
#endif  // CONFIG_DISABLE_LOOP_FILTERS_LOSSLESS
) {
  int i;
  int count = 4;

  int filt_neg = (filt_width_neg >> 1) - 1;
  int filter =
      filt_choice_highbd(s, 1, filt_width_neg, filt_width_pos, *q_thresh,
                         *side_thresh, s + (count - 1) * pitch);

  // loop filter designed to work using chars so that we can make maximum use
  // of 8 bit simd instructions.
  for (i = 0; i < count; ++i) {
    filt_generic_asym_highbd(*q_thresh, AOMMIN(filter, filt_neg), filter, s, 1,
                             bd
#if CONFIG_DISABLE_LOOP_FILTERS_LOSSLESS
                             ,
                             is_lossless_neg, is_lossless_pos
#endif  // CONFIG_DISABLE_LOOP_FILTERS_LOSSLESS
    );
    s += pitch;
  }
}