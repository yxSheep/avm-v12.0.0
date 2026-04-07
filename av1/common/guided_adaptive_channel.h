#ifndef AOM_AV1_COMMON_GUIDED_ADAPTIVE_CHANNEL_H_
#define AOM_AV1_COMMON_GUIDED_ADAPTIVE_CHANNEL_H_

#include <float.h>
#include "config/aom_config.h"
#include "av1/common/av1_common_int.h"

#ifdef __cplusplus
extern "C" {
#endif

#if CONFIG_MY_GUIDED_CNN
#define GUIDED_A_BITS CONFIG_GUIDED_COEFF_Q_BTIS
#define GUIDED_A_NUM_VALUES (1 << GUIDED_A_BITS)
#define GUIDED_A_MID (GUIDED_A_NUM_VALUES >> 1)
#define GUIDED_A_RANGE (GUIDED_A_NUM_VALUES - 1)
#define GUIDED_A_PAIR_BITS (GUIDED_A_BITS * 2 - 1)
#define GUIDED_QT_UNIT_SIZES_LOG2 2
#define GUIDED_QT_UNIT_SIZES (1 << GUIDED_QT_UNIT_SIZES_LOG2)

typedef enum {
  GUIDED_NONE = 0, 
  GUIDED_C1,
  GUIDED_C2,
  GUIDED_C3,
  GUIDED_C_TYPES,
  GUIDED_C_INVALID = -1,
} GuidedAdaptiveChannelType;

typedef struct {
  float scale;
  int zero_point;
} QuantizationParams_t;

QuantizationParams_t *get_q_parm_from_qindex(int qindex, int is_intra_only,
                                             int is_luma, int guided_c,
                                             int bit_depth);

void quad_copy(const AdpGuidedInfo *src, AdpGuidedInfo *dst,
               struct AV1Common *cm);

int quad_tree_get_max_unit_info_length(int width, int height, int unit_length);

int quad_tree_get_split_info_length(int width, int height, int unit_length);

static INLINE int get_guided_norestore_ctx(int qindex, int is_intra_only) {
  (void)qindex;
  (void)is_intra_only;
  if (is_intra_only) return 1;
  return 0;
}

static INLINE int quad_tree_get_unit_size(int width, int height,
                                          int unit_index) {
  const int max_dim = AOMMAX(width, height);
  const int max_dim_pow_2_bits = 1 + get_msb(max_dim);
  const int max_dim_pow_2 = 1 << max_dim_pow_2_bits;
  const int max_unit_size = AOMMAX(AOMMIN(max_dim_pow_2, 2048), 256);
  assert(unit_index >= 0 && unit_index < GUIDED_QT_UNIT_SIZES);
  return max_unit_size >> unit_index;
}

void av1_alloc_quadtree_struct(struct AV1Common *cm, AdpGuidedInfo *quad_info);

void av1_free_quadtree_struct(AdpGuidedInfo *quad_info);

int compute_num_blocks(int dim, int block_size);
#endif

#ifdef __cplusplus
}
#endif

#endif  // AOM_AV1_COMMON_GUIDED_ADAPTIVE_CHANNEL_H_
