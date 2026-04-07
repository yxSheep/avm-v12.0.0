#include <assert.h>
#include <string.h>
#include "av1/common/guided_adaptive_channel.h"

int qp255_quadtree_model_quantSet_intra[] = { 25, 197, 0, -8 };
int qp205_quadtree_model_quantSet_intra[] = { 643189, 690747, -17, -5 };
int qp175_quadtree_model_quantSet_intra[] = { 1838, 2153, 3, -12 };
int qp145_quadtree_model_quantSet_intra[] = { 27230, 33505, -21, 3 };
int qp120_quadtree_model_quantSet_intra[] = { 133, 199, 1, -7 };
int qp90_quadtree_model_quantSet_intra[] = { 988, 1428, 0, -11 };
int qp255_quadtree_model_quantSet_inter[] = { 56619, 15796, -9, -13 };
int qp205_quadtree_model_quantSet_inter[] = { 551342, 949167, -14, -5 };
int qp175_quadtree_model_quantSet_inter[] = { 2011, 3876, 0, -14 };
int qp145_quadtree_model_quantSet_inter[] = { 32668, 44115, -18, 2 };
int qp120_quadtree_model_quantSet_inter[] = { 20817, 19072, -12, -12 };
int qp90_quadtree_model_quantSet_inter[] = { 3455, 16494, -16, -8 };
/* guided 2
"""     5 bits qp210
        A0_scale = 0.0020
        A1_scale = 0.0014
        A0_min = -0.0349
        A1_min = -0.0135
"""
"""     5 bits qp185
        A0_scale = 0.0003
        A1_scale = 0.0004
        A0_min = -0.0094
        A1_min = -8.2010e-05
        ZP0 = 29
        ZP1 = 0
"""
"""     5 bits qp160
        A0_scale = 0.0002
        A1_scale = 0.0003
        A0_min = -0.0061
        A1_min = 0
        ZP0 = 31
        ZP1 = 0
"""
"""     5 bits qp135
        A0_scale = 8.5785e-05
        A1_scale = 0.0001
        A0_min = -0.0027
        A1_min = -3.4708e-06
        ZP0 = 31
        ZP1 = 0
"""
"""     5 bits qp110
        A0_scale = 5.9246e-05
        A1_scale = 9.2484e-05
        A0_min = -0.0018
        A1_min = -6.8486e-05
        ZP0 = 30
        ZP1 = 1
"""
"""     5 bits qp85
        A0_scale = 6.2133e-05
        A1_scale = 9.4438e-05
        A0_min = -0.0019
        A1_min = 0
        ZP0 = 31
        ZP1 = 0
"""
*/
#if CONFIG_GUIDED_COEFF_Q_BTIS == 5
QuantizationParams_t qp210_guided1_model_quantset_intra[] = { { 0.0933f, 6 } };
QuantizationParams_t qp185_guided1_model_quantset_intra[] = { { 0.0720f, 9 } };
QuantizationParams_t qp160_guided1_model_quantset_intra[] = { { 0.0461f, 0 } };
QuantizationParams_t qp135_guided1_model_quantset_intra[] = { { 0.0441f, 0 } };
QuantizationParams_t qp110_guided1_model_quantset_intra[] = { { 0.0444f, 0 } };
QuantizationParams_t qp85_guided1_model_quantset_intra[] = { { 0.0489f, 0 } };
QuantizationParams_t qp210_guided2_model_quantset_intra[] = { { 0.0020f, 17 },
                                                              { 0.0014f, 10 } };
QuantizationParams_t qp185_guided2_model_quantset_intra[] = { { 0.0003f, 29 },
                                                              { 0.0004f, 0 } };
QuantizationParams_t qp160_guided2_model_quantset_intra[] = { { 0.0002f, 31 },
                                                              { 0.0003f, 0 } };
QuantizationParams_t qp135_guided2_model_quantset_intra[] = { { 0.0001f, 31 },
                                                              { 0.0014f, 0 } };
QuantizationParams_t qp110_guided2_model_quantset_intra[] = {
  { 5.9246e-05f, 30 }, { 9.2484e-05f, 1 }
};
QuantizationParams_t qp85_guided2_model_quantset_intra[] = {
  { 6.2133e-05f, 31 }, { 9.4438e-05f, 0 }
};
QuantizationParams_t qp210_guided3_model_quantset_intra[] = {
  { 0.0010f, 17 },
  { 0.0021f, 22 },
  { 0.0014f, 11 },
};
QuantizationParams_t qp185_guided3_model_quantset_intra[] = {
  { 0.0004f, 16 },
  { 0.0007f, 26 },
  { 0.0005f, 4 },
};
QuantizationParams_t qp160_guided3_model_quantset_intra[] = {
  { 0.0001f, 26 },
  { 0.0002f, 20 },
  { 0.0004f, 0 },
};
QuantizationParams_t qp135_guided3_model_quantset_intra[] = {
  { 5.0762e-05f, 31 },
  { 4.3847e-05f, 31 },
  { 1.9194e-04f, 0 },
};
QuantizationParams_t qp110_guided3_model_quantset_intra[] = {
  { 3.4590e-05f, 30 },
  { 3.0427e-05f, 31 },
  { 1.3039e-04f, 1 },
};
QuantizationParams_t qp85_guided3_model_quantset_intra[] = {
  { 2.5558e-05f, 29 },
  { 2.2592e-05f, 31 },
  { 1.0132e-04f, 0 },
};
/*****************************1qp
 * inter*********************************************** */
QuantizationParams_t qp110_guided1_model_quantset_inter[] = { { 0.019298,
                                                                -16 } };
QuantizationParams_t qp135_guided1_model_quantset_inter[] = { { 0.021525,
                                                                -14 } };
QuantizationParams_t qp160_guided1_model_quantset_inter[] = { { 0.025305,
                                                                -7 } };
QuantizationParams_t qp185_guided1_model_quantset_inter[] = { { 0.029877,
                                                                -1 } };
QuantizationParams_t qp210_guided1_model_quantset_inter[] = { { 0.034112, 2 } };
QuantizationParams_t qp235_guided1_model_quantset_inter[] = { { 0.037564, 5 } };

QuantizationParams_t qp110_guided2_model_quantset_inter[] = {
  { 0.000194, -5 }, { 0.000103, 36 }
};
QuantizationParams_t qp135_guided2_model_quantset_inter[] = {
  { 0.000312, -3 }, { 0.000155, 34 }
};
QuantizationParams_t qp160_guided2_model_quantset_inter[] = {
  { 0.000493, 2 }, { 0.000245, 28 }
};
QuantizationParams_t qp185_guided2_model_quantset_inter[] = {
  { 0.000789, 6 }, { 0.000685, 21 }
};
QuantizationParams_t qp210_guided2_model_quantset_inter[] = {
  { 0.001192, 8 }, { 0.001013, 19 }
};
QuantizationParams_t qp235_guided2_model_quantset_inter[] = {
  { 0.001043, 9 }, { 0.000931, 18 }
};
QuantizationParams_t qp110_guided3_model_quantset_inter[] = {
  { 0.000035, -4 },
  { 0.000029, 3 },
  { 0.000341, -5 },
};
QuantizationParams_t qp135_guided3_model_quantset_inter[] = {
  { 0.000125, 8 },
  { 0.000219, 11 },
  { 0.000514, -4 },
};
QuantizationParams_t qp160_guided3_model_quantset_inter[] = {
  { 0.000314, 11 },
  { 0.000424, 15 },
  { 0.000772, -2 },
};
QuantizationParams_t qp185_guided3_model_quantset_inter[] = {
  { 0.000704, 12 },
  { 0.000773, 15 },
  { 0.001204, 3 },
};
QuantizationParams_t qp210_guided3_model_quantset_inter[] = {
  { 0.001130, 15 },
  { 0.001433, 15 },
  { 0.001801, 6 },
};
QuantizationParams_t qp235_guided3_model_quantset_inter[] = {
  { 0.001845, 17 },
  { 0.001866, 14 },
  { 0.002143, 7 },
};
// QuantizationParams_t qp110_guided2_model_quantset_inter[] = { { 0.000074,-59
// },
//                                                               { 0.000045,90 }
//                                                               };
// QuantizationParams_t qp135_guided2_model_quantset_inter[] = { { 0.000132,-42
// },
//                                                               { 0.000065,82
//                                                               } };
// QuantizationParams_t qp160_guided2_model_quantset_inter[] = { { 0.000203,-38
// },
//                                                               { 0.000096,75 }
//                                                               };
// QuantizationParams_t qp185_guided2_model_quantset_inter[] = { { 0.000391,-22
// },
//                                                               { 0.000229,36 }
//                                                               };
// QuantizationParams_t qp210_guided2_model_quantset_inter[] = {  {0.000418,-32
// },
//                                                               { 0.000351,36
//                                                               }};
// QuantizationParams_t qp235_guided2_model_quantset_inter[] = {  {0.000394,-19
// },
//                                                               { 0.000340,35
//                                                               }};
// QuantizationParams_t qp110_guided3_model_quantset_inter[] = {
// {0.000018,-59},
// {0.000013,-30},
// {0.000105,-81},
// };
// QuantizationParams_t qp135_guided3_model_quantset_inter[] = {
// {0.002876,0},
// {0.000064,5},
// {0.000185,-61},
// };
// QuantizationParams_t qp160_guided3_model_quantset_inter[] = {
//   {0.000083,-10},
// {0.000109,3},
// {0.000259,-63},
// };
// QuantizationParams_t qp185_guided3_model_quantset_inter[] = {
// {0.000441,15},
// {0.000594,12},
// {0.000631,-30},
// };
// QuantizationParams_t qp210_guided3_model_quantset_inter[] = {
// {0.000273,17},
// {0.000427,15},
// {0.000642,-45},
// };
// QuantizationParams_t qp235_guided3_model_quantset_inter[] = {
// {0.000561,22},
// {0.000752,17},
// {0.000652,-49},
// };
/*****************************3qp***********************************************
 */
QuantizationParams_t qp210_guided1_model_quantset_intra_1[] = { { 0.088662123f,
                                                                  10 } };
QuantizationParams_t qp185_guided1_model_quantset_intra_1[] = { { 0.07209694f,
                                                                  10 } };
QuantizationParams_t qp160_guided1_model_quantset_intra_1[] = { { 0.04654797f,
                                                                  0 } };
QuantizationParams_t qp135_guided1_model_quantset_intra_1[] = { { 0.04482845f,
                                                                  1 } };
QuantizationParams_t qp110_guided1_model_quantset_intra_1[] = { { 0.04720617f,
                                                                  0 } };
QuantizationParams_t qp85_guided1_model_quantset_intra_1[] = { { 0.06565027f,
                                                                 0 } };
QuantizationParams_t qp210_guided2_model_quantset_intra_1[] = {
  { 0.00134379f, 31 }, { 0.00129384f, 1 }
};
QuantizationParams_t qp185_guided2_model_quantset_intra_1[] = {
  { 0.00062740f, 29 }, { 0.00056435f, 2 }
};
QuantizationParams_t qp160_guided2_model_quantset_intra_1[] = {
  { 0.00043616f, 31 }, { 0.00037385f, 0 }
};
QuantizationParams_t qp135_guided2_model_quantset_intra_1[] = {
  { 0.00010177f, 30 }, { 0.00009708f, 2 }
};
QuantizationParams_t qp110_guided2_model_quantset_intra_1[] = {
  { 0.00008212f, 31 }, { 0.00007430f, 0 }
};
QuantizationParams_t qp85_guided2_model_quantset_intra_1[] = {
  { 0.00007428f, 30 }, { 0.00006684f, 1 }
};
QuantizationParams_t qp210_guided3_model_quantset_intra_1[] = {
  { 0.00076764f, 4 },
  { 0.00332219f, 18 },
  { 0.00192904f, 30 },
};
QuantizationParams_t qp185_guided3_model_quantset_intra_1[] = {
  { 0.00052316f, 2 },
  { 0.00119747f, 10 },
  { 0.00107029f, 26 },
};
QuantizationParams_t qp160_guided3_model_quantset_intra_1[] = {
  { 0.00029528f, 2 },
  { 0.00040223f, 14 },
  { 0.00061054f, 31 },
};
QuantizationParams_t qp135_guided3_model_quantset_intra_1[] = {
  { 0.00014492f, 31 },
  { 0.00007222f, 31 },
  { 0.00013744f, 0 },
};
QuantizationParams_t qp110_guided3_model_quantset_intra_1[] = {
  { 0.00009954f, 31 },
  { 0.00004812f, 31 },
  { 0.00009108f, 0 },
};
QuantizationParams_t qp85_guided3_model_quantset_intra_1[] = {
  { 0.00008895f, 21 },
  { 0.00003451f, 31 },
  { 0.00007697f, 0 },
};
/*****************************6qp***********************************************
 */
QuantizationParams_t qp210_guided1_model_quantset_intra_2[] = { { 0.10425594f,
                                                                  9 } };
QuantizationParams_t qp185_guided1_model_quantset_intra_2[] = { { 0.07163221f,
                                                                  7 } };
QuantizationParams_t qp160_guided1_model_quantset_intra_2[] = { { 0.05346341f,
                                                                  2 } };
QuantizationParams_t qp135_guided1_model_quantset_intra_2[] = { { 0.04582526f,
                                                                  0 } };
QuantizationParams_t qp110_guided1_model_quantset_intra_2[] = { { 0.05305009f,
                                                                  0 } };
QuantizationParams_t qp85_guided1_model_quantset_intra_2[] = { { 0.06027208f,
                                                                 0 } };
QuantizationParams_t qp210_guided2_model_quantset_intra_2[] = {
  { 0.00009285f, 31 }, { 0.00003703f, 1 }
};
QuantizationParams_t qp185_guided2_model_quantset_intra_2[] = {
  { 0.00006649f, 27 }, { 0.00002547f, 4 }
};
QuantizationParams_t qp160_guided2_model_quantset_intra_2[] = {
  { 0.00004792f, 31 }, { 0.00001967f, 3 }
};
QuantizationParams_t qp135_guided2_model_quantset_intra_2[] = {
  { 0.00003312f, 31 }, { 0.00001402f, 5 }
};
QuantizationParams_t qp110_guided2_model_quantset_intra_2[] = {
  { 0.00002816f, 31 }, { 0.00001161f, 4 }
};
QuantizationParams_t qp85_guided2_model_quantset_intra_2[] = {
  { 0.00002613f, 31 }, { 0.00000950f, 3 }
};
QuantizationParams_t qp210_guided3_model_quantset_intra_2[] = {
  { 0.00098381f, 30 },
  { 0.00099702f, 30 },
  { 0.00228723f, 20 },
};
QuantizationParams_t qp185_guided3_model_quantset_intra_2[] = {
  { 0.00061927f, 29 },
  { 0.00054930f, 29 },
  { 0.00073100f, 12 },
};
QuantizationParams_t qp160_guided3_model_quantset_intra_2[] = {
  { 0.00037785f, 31 },
  { 0.00038177f, 31 },
  { 0.00026148f, 20 },
};
QuantizationParams_t qp135_guided3_model_quantset_intra_2[] = {
  { 0.00026901f, 31 },
  { 0.00026991f, 31 },
  { 0.00010882f, 19 },
};
QuantizationParams_t qp110_guided3_model_quantset_intra_2[] = {
  { 0.00020333f, 31 },
  { 0.00020701f, 31 },
  { 0.00005894f, 28 },
};
QuantizationParams_t qp85_guided3_model_quantset_intra_2[] = {
  { 0.00017356f, 31 },
  { 0.00016957f, 31 },
  { 0.00005195f, 26 },
};
#elif CONFIG_GUIDED_COEFF_Q_BTIS == 4
QuantizationParams_t qp210_guided1_model_quantset_intra[] = { { 0.1929f, 3 } };
QuantizationParams_t qp185_guided1_model_quantset_intra[] = { { 0.1487f, 4 } };
QuantizationParams_t qp160_guided1_model_quantset_intra[] = { { 0.0954f, 0 } };
QuantizationParams_t qp135_guided1_model_quantset_intra[] = { { 0.0912f, 0 } };
QuantizationParams_t qp110_guided1_model_quantset_intra[] = { { 0.0918f, 0 } };
QuantizationParams_t qp85_guided1_model_quantset_intra[] = { { 0.1010f, 0 } };
QuantizationParams_t qp210_guided2_model_quantset_intra[] = {
  { 0.00415273f, 8 }, { 0.00281465f, 5 }
};
QuantizationParams_t qp185_guided2_model_quantset_intra[] = {
  { 0.00066201f, 14 }, { 0.00077124f, 0 }
};
QuantizationParams_t qp160_guided2_model_quantset_intra[] = {
  { 0.0004069f, 15 }, { 0.00058481f, 0 }
};
QuantizationParams_t qp135_guided2_model_quantset_intra[] = {
  { 0.00017729f, 15 }, { 0.0002735f, 0 }
};
QuantizationParams_t qp110_guided2_model_quantset_intra[] = {
  { 0.00012244f, 15 }, { 0.00019113f, 10 }
};
QuantizationParams_t qp85_guided2_model_quantset_intra[] = {
  { 0.00012841f, 15 }, { 0.00019517f, 0 }
};
QuantizationParams_t qp210_guided3_model_quantset_intra[] = {
  { 0.00216628f, 8 },
  { 0.00440598f, 11 },
  { 0.0028186f, 5 },
};
QuantizationParams_t qp185_guided3_model_quantset_intra[] = {
  { 0.00091196f, 8 },
  { 0.001496f, 13 },
  { 0.00108986f, 2 },
};
QuantizationParams_t qp160_guided3_model_quantset_intra[] = {
  { 0.00024725f, 13 },
  { 0.00048786f, 10 },
  { 0.0007344f, 0 },
};
QuantizationParams_t qp135_guided3_model_quantset_intra[] = {
  { 1.0490792e-04f, 15 },
  { 9.0616835e-05f, 15 },
  { 3.9667843e-04f, 0 },
};
QuantizationParams_t qp110_guided3_model_quantset_intra[] = {
  { 7.1485556e-05f, 14 },
  { 6.2881663e-05f, 15 },
  { 2.6947894e-04f, 0 },
};
QuantizationParams_t qp85_guided3_model_quantset_intra[] = {
  { 5.2819436e-05f, 14 },
  { 4.6690307e-05f, 15 },
  { 2.0939343e-04f, 0 },
};
#endif

QuantizationParams_t *get_q_parm_from_qindex(int qindex, int is_intra_only,
                                             int is_luma, int guided_c,
                                             int bit_depth) {
  int qindex_adjust = qindex - 24 * (bit_depth - 8);
  if (is_luma) {
    if (is_intra_only) {
      if (qindex_adjust <= 90) {
        return (guided_c == 0)   ? qp85_guided1_model_quantset_intra
               : (guided_c == 1) ? qp85_guided2_model_quantset_intra
                                 : qp85_guided3_model_quantset_intra;
      }
      if (qindex_adjust <= 124) {
        return (guided_c == 0)   ? qp110_guided1_model_quantset_intra
               : (guided_c == 1) ? qp110_guided2_model_quantset_intra
                                 : qp110_guided3_model_quantset_intra;
      }
      if (qindex_adjust <= 149) {
        return (guided_c == 0)   ? qp135_guided1_model_quantset_intra
               : (guided_c == 1) ? qp135_guided2_model_quantset_intra
                                 : qp135_guided3_model_quantset_intra;
      }
      if (qindex_adjust <= 174) {
        return (guided_c == 0)   ? qp160_guided1_model_quantset_intra
               : (guided_c == 1) ? qp160_guided2_model_quantset_intra
                                 : qp160_guided3_model_quantset_intra;
      }
      if (qindex_adjust <= 199) {
        return (guided_c == 0)   ? qp185_guided1_model_quantset_intra
               : (guided_c == 1) ? qp185_guided2_model_quantset_intra
                                 : qp185_guided3_model_quantset_intra;
      }
      return (guided_c == 0)   ? qp210_guided1_model_quantset_intra
             : (guided_c == 1) ? qp210_guided2_model_quantset_intra
                               : qp210_guided3_model_quantset_intra;
    } else {
      if (qindex_adjust <= 110) {
        return (guided_c == 0)   ? qp110_guided1_model_quantset_inter
               : (guided_c == 1) ? qp110_guided2_model_quantset_inter
                                 : qp110_guided3_model_quantset_inter;
      }
      if (qindex_adjust <= 135) {
        return (guided_c == 0)   ? qp135_guided1_model_quantset_inter
               : (guided_c == 1) ? qp135_guided2_model_quantset_inter
                                 : qp135_guided3_model_quantset_inter;
      }
      if (qindex_adjust <= 160) {
        return (guided_c == 0)   ? qp160_guided1_model_quantset_inter
               : (guided_c == 1) ? qp160_guided2_model_quantset_inter
                                 : qp160_guided3_model_quantset_inter;
      }
      if (qindex_adjust <= 185) {
        return (guided_c == 0)   ? qp185_guided1_model_quantset_inter
               : (guided_c == 1) ? qp185_guided2_model_quantset_inter
                                 : qp185_guided3_model_quantset_inter;
      }
      if (qindex_adjust <= 210) {
        return (guided_c == 0)   ? qp210_guided1_model_quantset_inter
               : (guided_c == 1) ? qp210_guided2_model_quantset_inter
                                 : qp210_guided3_model_quantset_inter;
      }
      return (guided_c == 0)   ? qp235_guided1_model_quantset_inter
             : (guided_c == 1) ? qp235_guided2_model_quantset_inter
                               : qp235_guided3_model_quantset_inter;
    }
  }
  return NULL;
}

#if CONFIG_MY_GUIDED_CNN
void quad_copy(const AdpGuidedInfo *src, AdpGuidedInfo *dst,
               struct AV1Common *cm) {
  dst->unit_index = src->unit_index;
  dst->unit_size = src->unit_size;
  dst->mode_info_length = src->mode_info_length;
  dst->unit_info_length = src->unit_info_length;
  av1_alloc_quadtree_struct(cm, dst);
  for (int i = 0; i < dst->mode_info_length; ++i) {
    dst->mode_info[i].mode = src->mode_info[i].mode;
  }
  for (int i = 0; i < dst->unit_info_length; ++i) {
    dst->use_res_cb[i] = src->use_res_cb[i]; // TODOCNN 之前没有拷贝
    dst->unit_info[i].xqd[0] = src->unit_info[i].xqd[0];
    dst->unit_info[i].xqd[1] = src->unit_info[i].xqd[1];
    dst->unit_info[i].xqd[2] = src->unit_info[i].xqd[2];
  }
  dst->signaled = src->signaled;
}

int quad_tree_get_max_unit_info_length(int width, int height, int unit_length) {
  int unit_info_length = 0;
  const int ext_size = unit_length * 3 / 2;
  for (int row = 0; row < height;) {
    const int remaining_height = height - row;
    const int this_unit_height =
        (remaining_height < ext_size) ? remaining_height : unit_length;
    for (int col = 0; col < width;) {
      const int remaining_width = width - col;
      const int this_unit_width =
          (remaining_width < ext_size) ? remaining_width : unit_length;
      const bool is_horz_partitioning_allowed =
          (this_unit_height >= unit_length);
      const bool is_vert_partitioning_allowed =
          (this_unit_width >= unit_length);
      if (!is_horz_partitioning_allowed && !is_vert_partitioning_allowed) {
        ++unit_info_length;
      } else {
        const int max_sub_units =
            is_horz_partitioning_allowed && is_vert_partitioning_allowed ? 4
                                                                         : 2;
        unit_info_length += max_sub_units;
      }
      col += this_unit_width;
    }
    row += this_unit_height;
  }
  return unit_info_length;
}

int quad_tree_get_split_info_length(int width, int height, int unit_length) {
  int split_info_len = 0;
  const int ext_size = unit_length * 3 / 2;
  for (int row = 0; row < height;) {
    const int remaining_height = height - row;
    const int this_unit_height =
        (remaining_height < ext_size) ? remaining_height : unit_length;
    for (int col = 0; col < width;) {
      const int remaining_width = width - col;
      const int this_unit_width =
          (remaining_width < ext_size) ? remaining_width : unit_length;
      const bool is_horz_partitioning_allowed =
          (this_unit_height >= unit_length);
      const bool is_vert_partitioning_allowed =
          (this_unit_width >= unit_length);
      if (is_horz_partitioning_allowed || is_vert_partitioning_allowed) {
        ++split_info_len;
      }
      col += this_unit_width;
    }
    row += this_unit_height;
  }
  return split_info_len;
}

void av1_alloc_quadtree_struct(struct AV1Common *cm, AdpGuidedInfo *quad_info) {
  if (quad_info->unit_info != NULL) {
    aom_free(quad_info->unit_info);
    quad_info->unit_info = NULL;
  }
  if (quad_info->mode_info != NULL) {
    aom_free(quad_info->mode_info);
    quad_info->mode_info = NULL;
  }
  quad_info->unit_size =
      quad_tree_get_unit_size(cm->width, cm->height, quad_info->unit_index);

  if (quad_info->mode_info_length > 0) {
    CHECK_MEM_ERROR(
        cm, quad_info->mode_info,
        (AdpModeInfo *)aom_memalign(
            16, sizeof(*quad_info->mode_info) * quad_info->mode_info_length));
  }

  assert(quad_info->unit_info_length > 0); // TODOINTER 应该可以去掉

  CHECK_MEM_ERROR(
      cm, quad_info->unit_info,
      (AdpUnitInfo *)aom_memalign(
          16, sizeof(*quad_info->unit_info) * quad_info->unit_info_length));

  CHECK_MEM_ERROR(cm, quad_info->use_res_cb,
                  (int *)aom_memalign(16, sizeof(*quad_info->use_res_cb) *
                                              quad_info->unit_info_length));
  quad_info->signaled = 0;
}

void av1_free_quadtree_struct(AdpGuidedInfo *quad_info) {
  if (quad_info->unit_info != NULL) {
    aom_free(quad_info->unit_info);
    quad_info->unit_info = NULL;
  }
  if (quad_info->mode_info != NULL) {
    aom_free(quad_info->mode_info);
    quad_info->mode_info = NULL;
  }
  memset(quad_info, 0, sizeof(*quad_info));
}

int compute_num_blocks(const int dim, const int block_size) {
  const int full_blocks = dim / block_size;
  const int rem = dim % block_size;
  if (rem < (block_size >> 1)) {
    return full_blocks;
  } else {
    return full_blocks + 1;
  }
}
#endif
