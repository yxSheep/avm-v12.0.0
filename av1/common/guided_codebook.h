#ifndef AOM_AV1_COMMON_GUIDED_CODEBOOK_H_
#define AOM_AV1_COMMON_GUIDED_CODEBOOK_H_

#ifdef __cplusplus
#include <vector>
#include <array>
#include "enums.h"
#include "av1/common/enums.h"

using CodebookSet = std::vector<std::vector<float>>;
using CodebookTable = const std::array<CodebookSet, CODEBOOK_CHANNEL>;
using QPLevelCodeBookTable = const std::array<CodebookTable, QP_NUM>;

extern const QPLevelCodeBookTable intra_codebooks;
extern const QPLevelCodeBookTable inter_codebooks;
extern const QPLevelCodeBookTable res_intra_codebooks;
extern const QPLevelCodeBookTable res_inter_codebooks;
extern const int codebook_nsymbs_intra[QP_NUM][CODEBOOK_CHANNEL];
extern const int codebook_nsymbs_inter[QP_NUM][CODEBOOK_CHANNEL];
extern const int res_codebook_nsymbs_intra[QP_NUM][CODEBOOK_CHANNEL];
extern const int res_codebook_nsymbs_inter[QP_NUM][CODEBOOK_CHANNEL];

int get_codebook_index(int is_intra_only, int qp_idx, int block_size_idx,
                       const std::vector<float> &coef, bool is_residual_cb);

std::vector<float> get_codebook_val(int is_intra_only, int qp_idx,
                                    int block_size_idx, int c, int idx,
                                    bool is_residual_cb);
                                    
std::pair<int, std::vector<float>> get_codebook_idx_and_val(
    int is_intra_only, int qp_idx, int block_size_idx,
    const std::vector<float> &coef, bool is_residual_cb);
#endif  // __cplusplus

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
uint8_t get_codebook_nsymbs(int qp_idx, int channel_num, int is_intra_only,
                            int is_residual_cb);

int get_codebook_index_c(int qp_idx, int block_size_idx, const int *coef,
                         int len);

#ifdef __cplusplus
}
#endif

#endif  // AOM_AV1_COMMON_GUIDED_CODEBOOK_H_
