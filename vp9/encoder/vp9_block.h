/*
 *  Copyright (c) 2010 The WebM project authors. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */

#ifndef VP9_ENCODER_VP9_BLOCK_H_
#define VP9_ENCODER_VP9_BLOCK_H_

#include "vp9/common/vp9_entropymv.h"
#include "vp9/common/vp9_entropy.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
  unsigned int sse;
  int sum;
  unsigned int var;
} diff;

struct macroblock_plane {
  DECLARE_ALIGNED(16, int16_t, src_diff[64 * 64]);
  tran_low_t *qcoeff;
  tran_low_t *coeff;
  uint16_t *eobs;
  struct buf_2d src;

  // Quantizer setings
  int16_t *quant_fp;
  int16_t *round_fp;
  int16_t *quant;
  int16_t *quant_shift;
  int16_t *zbin;
  int16_t *round;
#if CONFIG_NEW_QUANT
  cumbins_type_nuq *cumbins_nuq;
#endif  // CONFIG_NEW_QUANT
#if CONFIG_TX_SKIP
  int16_t *quant_pxd_fp;
  int16_t *round_pxd_fp;
  int16_t *quant_pxd;
  int16_t *quant_shift_pxd;
  int16_t *zbin_pxd;
  int16_t *round_pxd;
#if CONFIG_NEW_QUANT
  cumbins_type_nuq *cumbins_nuq_pxd;
#endif  // CONFIG_NEW_QUANT
#endif  // CONFIG_TX_SKIP
#if CONFIG_TWO_STAGE
  DECLARE_ALIGNED(16, int16_t, src_diff_stg2[64 * 64]);
  tran_low_t *coeff_stg2;
  tran_low_t *qcoeff_stg2;
  uint16_t *eobs_stg2;
  int16_t *quant_fp_stg1[TWO_STAGE_MAX_QINDEX_PLUS];
  int16_t *round_fp_stg1[TWO_STAGE_MAX_QINDEX_PLUS];
  int16_t *quant_stg1[TWO_STAGE_MAX_QINDEX_PLUS];
  int16_t *quant_shift_stg1[TWO_STAGE_MAX_QINDEX_PLUS];
  int16_t *zbin_stg1[TWO_STAGE_MAX_QINDEX_PLUS];
  int16_t *round_stg1[TWO_STAGE_MAX_QINDEX_PLUS];
#endif  // CONFIG_TWO_STAGE

  int64_t quant_thred[2];
};

/* The [2] dimension is for whether we skip the EOB node (i.e. if previous
 * coefficient in this block was zero) or not. */
typedef unsigned int vp9_coeff_cost[PLANE_TYPES][REF_TYPES][COEF_BANDS][2]
                                   [COEFF_CONTEXTS][ENTROPY_TOKENS];

#if CONFIG_TX_SKIP
typedef unsigned int vp9_coeff_cost_pxd[PLANE_TYPES][REF_TYPES][2]
                                       [COEFF_CONTEXTS][ENTROPY_TOKENS];
#endif  // CONFIG_TX_SKIP

typedef struct macroblock MACROBLOCK;
struct macroblock {
  struct macroblock_plane plane[MAX_MB_PLANE];

  MACROBLOCKD e_mbd;
  int skip_block;
  int select_tx_size;
  int skip_recode;
  int skip_optimize;
  int q_index;

  int errorperbit;
  int sadperbit16;
  int sadperbit4;
  int rddiv;
  int rdmult;
  int mb_energy;

  int mv_best_ref_index[MAX_REF_FRAMES];
  unsigned int max_mv_context[MAX_REF_FRAMES];
  unsigned int source_variance;
  unsigned int pred_sse[MAX_REF_FRAMES];
  int pred_mv_sad[MAX_REF_FRAMES];

  int nmvjointcost[MV_JOINTS];
  int *ndvcost[2];
  int *nmvcost[2];
  int *nmvcost_hp[2];
  int **mvcost;

  int nmvjointsadcost[MV_JOINTS];
  int *nmvsadcost[2];
  int *nmvsadcost_hp[2];
  int **mvsadcost;

  // These define limits to motion vector components to prevent them
  // from extending outside the UMV borders
  int mv_col_min;
  int mv_col_max;
  int mv_row_min;
  int mv_row_max;

  uint8_t zcoeff_blk[TX_SIZES][256];
  int skip;

  int encode_breakout;

  // note that token_costs is the cost when eob node is skipped
  vp9_coeff_cost token_costs[TX_SIZES];

#if CONFIG_TX_SKIP
  vp9_coeff_cost_pxd token_costs_pxd[TX_SIZES];
#endif  // CONFIG_TX_SKIP

  int in_static_area;

  int optimize;

  // indicate if it is in the rd search loop or encoding process
  int use_lp32x32fdct;
  int skip_encode;

  // use fast quantization process
  int quant_fp;

  // skip forward transform and quantization
  uint8_t skip_txfm[MAX_MB_PLANE << 2];

  int64_t bsse[MAX_MB_PLANE << 2];

  // Used to store sub partition's choices.
  MV pred_mv[MAX_REF_FRAMES];

  void (*fwd_txm4x4)(const int16_t *input, tran_low_t *output, int stride);
  void (*itxm_add)(const tran_low_t *input, uint8_t *dest, int stride, int eob);
#if CONFIG_VP9_HIGHBITDEPTH
  void (*highbd_itxm_add)(const tran_low_t *input, uint8_t *dest, int stride,
                          int eob, int bd);
#endif  // CONFIG_VP9_HIGHBITDEPTH
#if CONFIG_PALETTE
  DECLARE_ALIGNED(16, double, kmeans_data_buffer[MAX_MB_PLANE * 64 * 64]);
  DECLARE_ALIGNED(16, int, kmeans_indices_buffer[64 * 64]);
#endif  // CONFIG_PALETTE
};

#ifdef __cplusplus
}  // extern "C"
#endif

#endif  // VP9_ENCODER_VP9_BLOCK_H_
