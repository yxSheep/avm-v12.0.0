#include "guided_codebook.h"
#include <cstdint>
#include <iostream>
#include <vector>
#include <array>
#include <unordered_set>
#include <cstdio>
#include "av1/common/enums.h"
#include <algorithm>
#include <cmath>

extern "C" uint8_t get_codebook_nsymbs(int qp_idx,
                                         int channel_num, int is_intra_only,int is_residual_cb) {
    auto &codebook_nsymbols =
        (is_residual_cb ? 
            (is_intra_only ? res_codebook_nsymbs_intra : res_codebook_nsymbs_inter) :
            (is_intra_only ? codebook_nsymbs_intra : codebook_nsymbs_inter));
    return codebook_nsymbols[qp_idx][channel_num];    
}
std::vector<float> cdist(
    const std::vector<float>& x,
    const std::vector<std::vector<float>>& y,
    float eps = 1e-8f
) {
    size_t m = y.size();
    size_t d = x.size();
    std::vector<float> dist(m, 0.0f);

    for (size_t j = 0; j < m; ++j) {
        float y2 = 0.0f;
        float xy = 0.0f;
        for (size_t k = 0; k < d; ++k) {
            y2 += y[j][k] * y[j][k];
            xy += x[k] * y[j][k];
        }
        float x2 = 0.0f;
        for (size_t k = 0; k < d; ++k)
            x2 += x[k] * x[k];

        dist[j] = std::sqrt(std::max(x2 + y2 - 2.0f * xy, eps));
    }
    return dist;
}

int get_codebook_index(int is_intra_only,int qp_idx, int block_size_idx,
                       const std::vector<float> &coef,bool is_residual_cb) {
  int c = (int)coef.size() - 1;
  if (qp_idx < 0 || qp_idx >= QP_NUM) return -1;
  

  const auto& codebook = (is_residual_cb ? 
                (is_intra_only ? res_intra_codebooks[qp_idx][c] : res_inter_codebooks[qp_idx][c]) : 
                (is_intra_only ? intra_codebooks[qp_idx][c] : inter_codebooks[qp_idx][c]));

  const auto& dist = cdist(coef, codebook);
  auto min_it = std::min_element(dist.begin(), dist.end());
  return min_it != dist.end() ? static_cast<int>(min_it - dist.begin()) : -1;
}

 std::vector <float> get_codebook_val(int is_intra_only,int qp_idx, int block_size_idx,int c,int idx,bool is_residual_cb) {
    c-=1;
    // assert(c>0);
  const auto& codebook = (is_residual_cb ? 
                (is_intra_only ? res_intra_codebooks[qp_idx][c] : res_inter_codebooks[qp_idx][c]) : 
                (is_intra_only ? intra_codebooks[qp_idx][c] : inter_codebooks[qp_idx][c]));
    return codebook[idx];
}

std::pair<int, std::vector<float>> get_codebook_idx_and_val(int is_intra_only,int qp_idx, int block_size_idx, const std::vector<float> &coef,bool is_residual_cb){
    auto idx = get_codebook_index(is_intra_only, qp_idx, block_size_idx, coef,is_residual_cb);
    // std::cout << is_intra_only << " " << qp_idx << " " << block_size_idx << " " << coef.size() << " " << idx << std::endl;
    return std::make_pair(idx,get_codebook_val(is_intra_only,qp_idx,block_size_idx,coef.size(),idx,is_residual_cb));
}
