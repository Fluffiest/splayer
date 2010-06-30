/*
 * This file is part of Media Player Classic HomeCinema.
 *
 * MPC-HC is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * MPC-HC is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with FFmpeg; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA
 */

#include "h264.h"
#include "h264data.h"
#include "h264_parser.h"
#include "golomb.h"


typedef unsigned short        USHORT;
typedef unsigned char         UCHAR;
typedef int                   INT;
typedef unsigned int          UINT;
typedef char                  CHAR;
typedef unsigned char         BYTE;
typedef unsigned short        WORD;
typedef short				  SHORT;

#define S_OK                0x00000000L
#define E_FAIL              0x80004005L


/* H.264/AVC picture entry data structure */
typedef struct _DXVA_PicEntry_H264 {
  union {
    struct {
      UCHAR  Index7Bits      : 7;
      UCHAR  AssociatedFlag  : 1;
    };
    UCHAR  bPicEntry;
  };
} DXVA_PicEntry_H264, *LPDXVA_PicEntry_H264;  /* 1 byte */
typedef struct _DXVA_Slice_H264_Long {
  UINT   BSNALunitDataLocation; /* type 1..5 */
  UINT   SliceBytesInBuffer; /* for off-host parse */
  USHORT wBadSliceChopping;  /* for off-host parse */
  USHORT first_mb_in_slice;
  USHORT NumMbsForSlice;
  USHORT BitOffsetToSliceData; /* after CABAC alignment */
  UCHAR  slice_type;
  UCHAR  luma_log2_weight_denom;
  UCHAR  chroma_log2_weight_denom;
  UCHAR  num_ref_idx_l0_active_minus1;
  UCHAR  num_ref_idx_l1_active_minus1;
  CHAR   slice_alpha_c0_offset_div2;
  CHAR   slice_beta_offset_div2;
  UCHAR  Reserved8Bits;
  DXVA_PicEntry_H264 RefPicList[2][32]; /* L0 & L1 */
  SHORT  Weights[2][32][3][2]; /* L0 & L1; Y, Cb, Cr */
  CHAR   slice_qs_delta;
                               /* rest off-host parse */
  CHAR   slice_qp_delta;
  UCHAR  redundant_pic_cnt;
  UCHAR  direct_spatial_mv_pred_flag;
  UCHAR  cabac_init_idc;
  UCHAR  disable_deblocking_filter_idc;
  USHORT slice_id;
} DXVA_Slice_H264_Long, *LPDXVA_Slice_H264_Long;
static void fill_dxva_slice_long(H264Context *h){
    MpegEncContext* const	s = &h->s;
    AVCodecContext* const	avctx= s->avctx;
	DXVA_Slice_H264_Long*	pSlice = &((DXVA_Slice_H264_Long*) h->dxva_slice_long)[h->current_slice-1];
	int						field_pic_flag;
	unsigned int			i,j,k;
	field_pic_flag = (h->s.picture_structure != PICT_FRAME);
	pSlice->slice_id						= h->current_slice-1;
	pSlice->first_mb_in_slice				= h->first_mb_in_slice;
	pSlice->NumMbsForSlice					= 0; // h->s.mb_num;				// TODO : to be checked !
	pSlice->BitOffsetToSliceData			= h->bit_offset_to_slice_data;
	pSlice->slice_type						= h->raw_slice_type; 
	pSlice->luma_log2_weight_denom			= h->luma_log2_weight_denom;
	pSlice->chroma_log2_weight_denom		= h->chroma_log2_weight_denom;
	pSlice->num_ref_idx_l0_active_minus1	= h->ref_count[0]-1;	// num_ref_idx_l0_active_minus1;
	pSlice->num_ref_idx_l1_active_minus1	= h->ref_count[1]-1;	// num_ref_idx_l1_active_minus1;
	pSlice->slice_alpha_c0_offset_div2		= h->slice_alpha_c0_offset / 2;
	pSlice->slice_beta_offset_div2			= h->slice_beta_offset / 2;
	pSlice->Reserved8Bits					= 0;
	// Fill prediction weights
	memset (pSlice->Weights, 0, sizeof(pSlice->Weights));
	for(j=0; j<2; j++){
		for(i=0; i<h->ref_count[j]; i++){
			//         L0&L1          Y,Cb,Cr  Weight,Offset
			// Weights  [2]    [32]     [3]         [2]
			pSlice->Weights[j][i][0][0] = h->luma_weight[j][i];
			pSlice->Weights[j][i][0][1] = h->luma_offset[j][i];
			for(k=0; k<2; k++){
				pSlice->Weights[j][i][k+1][0] = h->chroma_weight[j][i][k];
				pSlice->Weights[j][i][k+1][1] = h->chroma_offset[j][i][k];
			}
		}
	}
	pSlice->slice_qs_delta					= h->slice_qs_delta;
	pSlice->slice_qp_delta					= h->slice_qp_delta;
	pSlice->redundant_pic_cnt				= h->redundant_pic_count;
	pSlice->direct_spatial_mv_pred_flag		= h->direct_spatial_mv_pred;
	pSlice->cabac_init_idc					= h->cabac_init_idc;
	pSlice->disable_deblocking_filter_idc	= h->deblocking_filter;
	for(i=0; i<32; i++)
	{ pSlice->RefPicList[0][i].AssociatedFlag = 1;
	  pSlice->RefPicList[0][i].bPicEntry = 255; 
	  pSlice->RefPicList[0][i].Index7Bits = 127;
	  pSlice->RefPicList[1][i].AssociatedFlag = 1; 
	  pSlice->RefPicList[1][i].bPicEntry = 255;
	  pSlice->RefPicList[1][i].Index7Bits = 127;
	}
}
static void field_end_noexecute(H264Context *h){
    MpegEncContext * const s = &h->s;
    AVCodecContext * const avctx= s->avctx;
    s->mb_y= 0;

    s->current_picture_ptr->qscale_type= FF_QSCALE_TYPE_H264;
    s->current_picture_ptr->pict_type= s->pict_type;

    if(!s->dropable) {
        execute_ref_pic_marking(h, h->mmco, h->mmco_index);
        h->prev_poc_msb= h->poc_msb;
        h->prev_poc_lsb= h->poc_lsb;
    }
    h->prev_frame_num_offset= h->frame_num_offset;
    h->prev_frame_num= h->frame_num;

    MPV_frame_end(s);

    h->current_slice=0;
}

int decode_slice_header_noexecute (H264Context *h){
	// ==> Start patch MPC DXVA
	H264Context *h0 = h;
	// <== End patch MPC DXVA
    MpegEncContext * const s = &h->s;
    MpegEncContext * const s0 = &h0->s;
    unsigned int pps_id;
    int num_ref_idx_active_override_flag;
    unsigned int slice_type, tmp, i, j;
    int default_ref_list_done = 0;
    int last_pic_structure;

    s->dropable= h->nal_ref_idc == 0;

    if((s->avctx->flags2 & CODEC_FLAG2_FAST) && !h->nal_ref_idc){
        s->me.qpel_put= s->dsp.put_2tap_qpel_pixels_tab;
        s->me.qpel_avg= s->dsp.avg_2tap_qpel_pixels_tab;
    }else{
        s->me.qpel_put= s->dsp.put_h264_qpel_pixels_tab;
        s->me.qpel_avg= s->dsp.avg_h264_qpel_pixels_tab;
    }

	// ==> Start patch MPC DXVA
    h->first_mb_in_slice= get_ue_golomb(&s->gb);
	// <== End patch MPC DXVA

    if(h->first_mb_in_slice == 0){ //FIXME better field boundary detection
        if(h0->current_slice && FIELD_PICTURE){
            field_end(h);
        }
        h0->current_slice = 0;
        if (!s0->first_field)
            s->current_picture_ptr= NULL;
    }

    slice_type= get_ue_golomb_31(&s->gb);
    if(slice_type > 9){
        av_log(h->s.avctx, AV_LOG_ERROR, "slice type too large (%d) at %d %d\n", h->slice_type, s->mb_x, s->mb_y);
        return -1;
    }
    if(slice_type > 4){
        slice_type -= 5;
        h->slice_type_fixed=1;
    }else
        h->slice_type_fixed=0;

	// ==> Start patch MPC DXVA
	h->raw_slice_type = slice_type;
	// <== End patch MPC DXVA
    slice_type= golomb_to_pict_type[ slice_type ];
    if (slice_type == FF_I_TYPE
        || (h0->current_slice != 0 && slice_type == h0->last_slice_type) ) {
        default_ref_list_done = 1;
    }
    h->slice_type= slice_type;
    h->slice_type_nos= slice_type & 3;

    s->pict_type= h->slice_type; // to make a few old functions happy, it's wrong though
    if (s->pict_type == FF_B_TYPE && s0->last_picture_ptr == NULL) {
        av_log(h->s.avctx, AV_LOG_ERROR,
               "B picture before any references, skipping\n");
        return -1;
    }

    pps_id= get_ue_golomb(&s->gb);
    if(pps_id>=MAX_PPS_COUNT){
        av_log(h->s.avctx, AV_LOG_ERROR, "pps_id out of range\n");
        return -1;
    }
    if(!h0->pps_buffers[pps_id]) {
        av_log(h->s.avctx, AV_LOG_ERROR, "non-existing PPS %u referenced\n", pps_id);
        return -1;
    }
    h->pps= *h0->pps_buffers[pps_id];

    if(!h0->sps_buffers[h->pps.sps_id]) {
        av_log(h->s.avctx, AV_LOG_ERROR, "non-existing SPS %u referenced\n", h->pps.sps_id);
        return -1;
    }
    h->sps = *h0->sps_buffers[h->pps.sps_id];

    if(h == h0 && h->dequant_coeff_pps != pps_id){
        h->dequant_coeff_pps = pps_id;
        init_dequant_tables(h);
    }

    s->mb_width= h->sps.mb_width;
    s->mb_height= h->sps.mb_height * (2 - h->sps.frame_mbs_only_flag);

    h->b_stride=  s->mb_width*4;
    h->b8_stride= s->mb_width*2;

    s->width = 16*s->mb_width - 2*FFMIN(h->sps.crop_right, 7);
    if(h->sps.frame_mbs_only_flag)
        s->height= 16*s->mb_height - 2*FFMIN(h->sps.crop_bottom, 7);
    else
        s->height= 16*s->mb_height - 4*FFMIN(h->sps.crop_bottom, 3);

    if (s->context_initialized
        && (   s->width != s->avctx->width || s->height != s->avctx->height)) {
        if(h != h0)
            return -1;   // width / height changed during parallelized decoding
        free_tables(h);
        flush_dpb(s->avctx);
        MPV_common_end(s);
    }
    if (!s->context_initialized) {
        if(h != h0)
            return -1;  // we cant (re-)initialize context during parallel decoding
        if (MPV_common_init(s) < 0)
            return -1;
        s->first_field = 0;
        h->prev_interlaced_frame = 1;

        init_scan_tables(h);
        alloc_tables(h);

        for(i = 1; i < s->avctx->thread_count; i++) {
            H264Context *c;
            c = h->thread_context[i] = av_malloc(sizeof(H264Context));
            memcpy(c, h->s.thread_context[i], sizeof(MpegEncContext));
            memset(&c->s + 1, 0, sizeof(H264Context) - sizeof(MpegEncContext));
            c->sps = h->sps;
            c->pps = h->pps;
            init_scan_tables(c);
            clone_tables(c, h);
        }

        for(i = 0; i < s->avctx->thread_count; i++)
            if(context_init(h->thread_context[i]) < 0)
                return -1;

        s->avctx->width = s->width;
        s->avctx->height = s->height;
        s->avctx->sample_aspect_ratio= h->sps.sar;
        if(!s->avctx->sample_aspect_ratio.den)
            s->avctx->sample_aspect_ratio.den = 1;

        if(h->sps.timing_info_present_flag){
            #if __STDC_VERSION__ >= 199901L
            s->avctx->time_base= (AVRational){h->sps.num_units_in_tick * 2, h->sps.time_scale};
            #else
            s->avctx->time_base.num = h->sps.num_units_in_tick * 2;
            s->avctx->time_base.den = h->sps.time_scale;
            #endif
            if(h->x264_build > 0 && h->x264_build < 44)
                s->avctx->time_base.den *= 2;
            av_reduce(&s->avctx->time_base.num, &s->avctx->time_base.den,
                      s->avctx->time_base.num, s->avctx->time_base.den, 1<<30);
        }
    }

    h->frame_num= get_bits(&s->gb, h->sps.log2_max_frame_num);

    h->mb_mbaff = 0;
    h->mb_aff_frame = 0;
    last_pic_structure = s0->picture_structure;
    if(h->sps.frame_mbs_only_flag){
        s->picture_structure= PICT_FRAME;
    }else{
        if(get_bits1(&s->gb)) { //field_pic_flag
            s->picture_structure= PICT_TOP_FIELD + get_bits1(&s->gb); //bottom_field_flag
        } else {
            s->picture_structure= PICT_FRAME;
            h->mb_aff_frame = h->sps.mb_aff;
        }
    }
    h->mb_field_decoding_flag= s->picture_structure != PICT_FRAME;

    /* ffdshow custom code begin */
    //
    // Workaround Haali's media splitter (http://forum.doom9.org/showthread.php?p=1226434#post1226434)
    //
    // Disallow unpaired field referencing just after seeking.
    // This rule is applied only if h->has_to_drop_first_non_ref == 3.
    // This is wrong because an unpaired field is allowed to be a reference field.
    // And that's why this is optional and tried to be minimized.
    if (h->has_to_drop_first_non_ref == 1 && s->dropable){
        if (FIELD_PICTURE){
            h->has_to_drop_first_non_ref = 2;
        }else{
            h->has_to_drop_first_non_ref = 0;
        }
    } else if (h->has_to_drop_first_non_ref == 2){
        if (FIELD_PICTURE && !s->dropable)
            h->has_to_drop_first_non_ref = 3;
        else if (!FIELD_PICTURE)
            h->has_to_drop_first_non_ref = 0;
    } else if (h->has_to_drop_first_non_ref == 3){
        h->has_to_drop_first_non_ref = 0;
    }
    /* ffdshow custom code end */

    if(h0->current_slice == 0){
        while(h->frame_num !=  h->prev_frame_num &&
              h->frame_num != (h->prev_frame_num+1)%(1<<h->sps.log2_max_frame_num)){
            av_log(NULL, AV_LOG_DEBUG, "Frame num gap %d %d\n", h->frame_num, h->prev_frame_num);
            if (frame_start(h) < 0)
                return -1;
            h->prev_frame_num++;
            h->prev_frame_num %= 1<<h->sps.log2_max_frame_num;
            s->current_picture_ptr->frame_num= h->prev_frame_num;
            execute_ref_pic_marking(h, NULL, 0);
        }

        /* See if we have a decoded first field looking for a pair... */
        if (s0->first_field) {
            assert(s0->current_picture_ptr);
            assert(s0->current_picture_ptr->data[0]);
            assert(s0->current_picture_ptr->reference != DELAYED_PIC_REF);

            /* figure out if we have a complementary field pair */
            if (!FIELD_PICTURE || s->picture_structure == last_pic_structure) {
                /*
                 * Previous field is unmatched. Don't display it, but let it
                 * remain for reference if marked as such.
                 */
                s0->current_picture_ptr = NULL;
                s0->first_field = FIELD_PICTURE;

            } else {
                if ((h->nal_ref_idc &&
                        s0->current_picture_ptr->reference &&
                        s0->current_picture_ptr->frame_num != h->frame_num) ||
                        h->has_to_drop_first_non_ref == 3) { /* ffdshow custom code */
                    /*
                     * This and previous field were reference, but had
                     * different frame_nums. Consider this field first in
                     * pair. Throw away previous field except for reference
                     * purposes.
                     */
                    s0->first_field = 1;
                    s0->current_picture_ptr = NULL;

                } else {
                    /* Second field in complementary pair */
                    s0->first_field = 0;
                }
            }

        } else {
            /* Frame or first field in a potentially complementary pair */
            assert(!s0->current_picture_ptr);
            s0->first_field = FIELD_PICTURE;
        }

        if((!FIELD_PICTURE || s0->first_field) && frame_start(h) < 0) {
            s0->first_field = 0;
            return -1;
        }
    }
    //if(h != h0)
    //    clone_slice(h, h0, 0); /* ffdshow custom code */

    s->current_picture_ptr->frame_num= h->frame_num; //FIXME frame_num cleanup

    assert(s->mb_num == s->mb_width * s->mb_height);
    if(h->first_mb_in_slice << FIELD_OR_MBAFF_PICTURE >= s->mb_num ||
       h->first_mb_in_slice                    >= s->mb_num){
        av_log(h->s.avctx, AV_LOG_ERROR, "first_mb_in_slice overflow\n");
        return -1;
    }
    s->resync_mb_x = s->mb_x = h->first_mb_in_slice % s->mb_width;
    s->resync_mb_y = s->mb_y = (h->first_mb_in_slice / s->mb_width) << FIELD_OR_MBAFF_PICTURE;
    if (s->picture_structure == PICT_BOTTOM_FIELD)
        s->resync_mb_y = s->mb_y = s->mb_y + 1;
    assert(s->mb_y < s->mb_height);

    if(s->picture_structure==PICT_FRAME){
        h->curr_pic_num=   h->frame_num;
        h->max_pic_num= 1<< h->sps.log2_max_frame_num;
    }else{
        h->curr_pic_num= 2*h->frame_num + 1;
        h->max_pic_num= 1<<(h->sps.log2_max_frame_num + 1);
    }

    if(h->nal_unit_type == NAL_IDR_SLICE){
        get_ue_golomb(&s->gb); /* idr_pic_id */
    }

    if(h->sps.poc_type==0){
        h->poc_lsb= get_bits(&s->gb, h->sps.log2_max_poc_lsb);

        if(h->pps.pic_order_present==1 && s->picture_structure==PICT_FRAME){
            h->delta_poc_bottom= get_se_golomb(&s->gb);
        }
    }

    if(h->sps.poc_type==1 && !h->sps.delta_pic_order_always_zero_flag){
        h->delta_poc[0]= get_se_golomb(&s->gb);

        if(h->pps.pic_order_present==1 && s->picture_structure==PICT_FRAME)
            h->delta_poc[1]= get_se_golomb(&s->gb);
    }

    init_poc(h);

    if(h->pps.redundant_pic_cnt_present){
        h->redundant_pic_count= get_ue_golomb(&s->gb);
    }

    //set defaults, might be overridden a few lines later
    h->ref_count[0]= h->pps.ref_count[0];
    h->ref_count[1]= h->pps.ref_count[1];

    if(h->slice_type_nos != FF_I_TYPE){
        if(h->slice_type_nos == FF_B_TYPE){
            h->direct_spatial_mv_pred= get_bits1(&s->gb);
        }
        num_ref_idx_active_override_flag= get_bits1(&s->gb);

        if(num_ref_idx_active_override_flag){
            h->ref_count[0]= get_ue_golomb(&s->gb) + 1;
            if(h->slice_type_nos==FF_B_TYPE)
                h->ref_count[1]= get_ue_golomb(&s->gb) + 1;

            if(h->ref_count[0]-1 > 32-1 || h->ref_count[1]-1 > 32-1){
                av_log(h->s.avctx, AV_LOG_ERROR, "reference overflow\n");
                h->ref_count[0]= h->ref_count[1]= 1;
                return -1;
            }
        }
        if(h->slice_type_nos == FF_B_TYPE)
            h->list_count= 2;
        else
            h->list_count= 1;
    }else
        h->list_count= 0;

    if(!default_ref_list_done){
        fill_default_ref_list(h);
    }

    if(h->slice_type_nos!=FF_I_TYPE && decode_ref_pic_list_reordering(h) < 0)
        return -1;

    if(h->slice_type_nos!=FF_I_TYPE){
        s->last_picture_ptr= &h->ref_list[0][0];
        ff_copy_picture(&s->last_picture, s->last_picture_ptr);
    }
    if(h->slice_type_nos==FF_B_TYPE){
        s->next_picture_ptr= &h->ref_list[1][0];
        ff_copy_picture(&s->next_picture, s->next_picture_ptr);
    }

    if(   (h->pps.weighted_pred          && h->slice_type_nos == FF_P_TYPE )
       ||  (h->pps.weighted_bipred_idc==1 && h->slice_type_nos== FF_B_TYPE ) )
        pred_weight_table(h);
    else if(h->pps.weighted_bipred_idc==2 && h->slice_type_nos== FF_B_TYPE)
        implicit_weight_table(h);
    else {
        h->use_weight = 0;
        for (i = 0; i < 2; i++) {
            h->luma_weight_flag[i]   = 0;
            h->chroma_weight_flag[i] = 0;
        }
    }

    if(h->nal_ref_idc)
        decode_ref_pic_marking(h0, &s->gb);

    if(FRAME_MBAFF)
        fill_mbaff_ref_list(h);

    if(h->slice_type_nos==FF_B_TYPE && !h->direct_spatial_mv_pred)
        direct_dist_scale_factor(h);
    direct_ref_list_init(h);

    if( h->slice_type_nos != FF_I_TYPE && h->pps.cabac ){
        tmp = get_ue_golomb_31(&s->gb);
        if(tmp > 2){
            av_log(s->avctx, AV_LOG_ERROR, "cabac_init_idc overflow\n");
            return -1;
        }
        h->cabac_init_idc= tmp;
    }

    h->last_qscale_diff = 0;
    // ==> Start patch MPC
    h->slice_qp_delta = get_se_golomb(&s->gb);
    tmp = h->pps.init_qp + h->slice_qp_delta;
    // <== End patch MPC
    if(tmp>51){
        av_log(s->avctx, AV_LOG_ERROR, "QP %u out of range\n", tmp);
        return -1;
    }
    s->qscale= tmp;
    h->chroma_qp[0] = get_chroma_qp(h, 0, s->qscale);
    h->chroma_qp[1] = get_chroma_qp(h, 1, s->qscale);
    //FIXME qscale / qp ... stuff
    if(h->slice_type == FF_SP_TYPE){
        get_bits1(&s->gb); /* sp_for_switch_flag */
    }
    if(h->slice_type==FF_SP_TYPE || h->slice_type == FF_SI_TYPE){
    // ==> Start patch MPC
        h->slice_qs_delta = get_se_golomb(&s->gb); /* slice_qs_delta */
    // <== End patch MPC
    }

    h->deblocking_filter = 1;
    h->slice_alpha_c0_offset = 0;
    h->slice_beta_offset = 0;
    if( h->pps.deblocking_filter_parameters_present ) {
        tmp= get_ue_golomb_31(&s->gb);
        if(tmp > 2){
            av_log(s->avctx, AV_LOG_ERROR, "deblocking_filter_idc %u out of range\n", tmp);
            return -1;
        }
        h->deblocking_filter= tmp;
        if(h->deblocking_filter < 2)
            h->deblocking_filter^= 1; // 1<->0

        if( h->deblocking_filter ) {
            h->slice_alpha_c0_offset = get_se_golomb(&s->gb) << 1;
            h->slice_beta_offset = get_se_golomb(&s->gb) << 1;
        }
    }

    if(   s->avctx->skip_loop_filter >= AVDISCARD_ALL
       ||(s->avctx->skip_loop_filter >= AVDISCARD_NONKEY && h->slice_type_nos != FF_I_TYPE)
       ||(s->avctx->skip_loop_filter >= AVDISCARD_BIDIR  && h->slice_type_nos == FF_B_TYPE)
       ||(s->avctx->skip_loop_filter >= AVDISCARD_NONREF && h->nal_ref_idc == 0))
        h->deblocking_filter= 0;

    if(h->deblocking_filter == 1 && h0->max_contexts > 1) {
        if(s->avctx->flags2 & CODEC_FLAG2_FAST) {
            /* Cheat slightly for speed:
               Do not bother to deblock across slices. */
            h->deblocking_filter = 2;
        } else {
            h0->max_contexts = 1;
            if(!h0->single_decode_warning) {
                av_log(s->avctx, AV_LOG_INFO, "Cannot parallelize deblocking type 1, decoding such frames in sequential order\n");
                h0->single_decode_warning = 1;
            }
            if(h != h0)
                return 1; // deblocking switched inside frame
        }
    }

#if 0 //FMO
    if( h->pps.num_slice_groups > 1  && h->pps.mb_slice_group_map_type >= 3 && h->pps.mb_slice_group_map_type <= 5)
        slice_group_change_cycle= get_bits(&s->gb, ?);
#endif
	// ==> Start patch MPC

	// If entropy_coding_mode, align to 8 bits
	if (h->pps.cabac) align_get_bits( &s->gb );

	h->bit_offset_to_slice_data = s->gb.index;
	// <== End patch MPC

    h0->last_slice_type = slice_type;
    h->slice_num = ++h0->current_slice;
    if(h->slice_num >= MAX_SLICES){
        av_log(s->avctx, AV_LOG_ERROR, "Too many slices, increase MAX_SLICES and recompile\n");
    }

    for(j=0; j<2; j++){
        int *ref2frm= h->ref2frm[h->slice_num&(MAX_SLICES-1)][j];
        ref2frm[0]=
        ref2frm[1]= -1;
        for(i=0; i<16; i++)
            ref2frm[i+2]= 4*h->ref_list[j][i].frame_num
                          +(h->ref_list[j][i].reference&3);
        ref2frm[18+0]=
        ref2frm[18+1]= -1;
        for(i=16; i<48; i++)
            ref2frm[i+4]= 4*h->ref_list[j][i].frame_num
                          +(h->ref_list[j][i].reference&3);
    }

    h->emu_edge_width= (s->flags&CODEC_FLAG_EMU_EDGE) ? 0 : 16;
    h->emu_edge_height= (FRAME_MBAFF || FIELD_PICTURE) ? 0 : h->emu_edge_width;

    s->avctx->refs= h->sps.ref_frame_count;

	fill_dxva_slice_long(h);
    if(s->avctx->debug&FF_DEBUG_PICT_INFO){
        av_log(h->s.avctx, AV_LOG_DEBUG, "slice:%d %s mb:%d %c%s%s pps:%u frame:%d poc:%d/%d ref:%d/%d qp:%d loop:%d:%d:%d weight:%d%s %s\n",
               h->slice_num,
               (s->picture_structure==PICT_FRAME ? "F" : s->picture_structure==PICT_TOP_FIELD ? "T" : "B"),
               h->first_mb_in_slice,
               av_get_pict_type_char(h->slice_type), h->slice_type_fixed ? " fix" : "", h->nal_unit_type == NAL_IDR_SLICE ? " IDR" : "",
               pps_id, h->frame_num,
               s->current_picture_ptr->field_poc[0], s->current_picture_ptr->field_poc[1],
               h->ref_count[0], h->ref_count[1],
               s->qscale,
               h->deblocking_filter, h->slice_alpha_c0_offset/2, h->slice_beta_offset/2,
               h->use_weight,
               h->use_weight==1 && h->use_weight_chroma ? "c" : "",
               h->slice_type == FF_B_TYPE ? (h->direct_spatial_mv_pred ? "SPAT" : "TEMP") : ""
               );
    }

    return 0;
}

static int decode_nal_units_noexecute(H264Context *h, uint8_t *buf, int buf_size){
    MpegEncContext * const s = &h->s;
    AVCodecContext * const avctx= s->avctx;
    int buf_index=0;
    H264Context *hx; ///< thread context
    int context_count = 0;
	int next_avc= h->is_avc ? 0 : buf_size;

    h->max_contexts = avctx->thread_count;
#if 0
    int i;
    for(i=0; i<50; i++){
        av_log(NULL, AV_LOG_ERROR,"%02X ", buf[i]);
    }
#endif
    if(!(s->flags2 & CODEC_FLAG2_CHUNKS)){
        h->current_slice = 0;
        if (!s->first_field)
            s->current_picture_ptr= NULL;
        reset_sei(h);
    }

    for(;;){
        int consumed;
        int dst_length;
        int bit_length;
        const uint8_t *ptr;
        int i, nalsize = 0;
        int err;

       
        if(buf_index >= next_avc) {
            if(buf_index >= buf_size) break;
            nalsize = 0;
            for(i = 0; i < h->nal_length_size; i++)
                nalsize = (nalsize << 8) | buf[buf_index++];
            if(nalsize <= 1 || nalsize > buf_size - buf_index){
                if(nalsize == 1){
                    buf_index++;
                    continue;
                }else{
                    av_log(h->s.avctx, AV_LOG_ERROR, "AVC: nal size %d\n", nalsize);
                    break;
                }
            }
            next_avc= buf_index + nalsize;
        } else {
            // start code prefix search
            for(; buf_index + 3 < next_avc; buf_index++){
                // This should always succeed in the first iteration.
                if(buf[buf_index] == 0 && buf[buf_index+1] == 0 && buf[buf_index+2] == 1)
                    break;
            }

            if(buf_index+3 >= buf_size) break;

            buf_index+=3;
            if(buf_index >= next_avc) continue;
        }

        hx = h->thread_context[context_count];

        ptr= ff_h264_decode_nal(hx, buf + buf_index, &dst_length, &consumed, next_avc - buf_index);
        if (ptr==NULL || dst_length < 0){
            return -1;
        }
        while(ptr[dst_length - 1] == 0 && dst_length > 0)
            dst_length--;
        bit_length= !dst_length ? 0 : (8*dst_length - ff_h264_decode_rbsp_trailing(h, ptr + dst_length - 1));

        if(s->avctx->debug&FF_DEBUG_STARTCODE){
            av_log(h->s.avctx, AV_LOG_DEBUG, "NAL %d at %d/%d length %d\n", hx->nal_unit_type, buf_index, buf_size, dst_length);
        }

        if (h->is_avc && (nalsize != consumed) && nalsize){
            int i, debug_level = AV_LOG_DEBUG;
            for (i = consumed; i < nalsize; i++)
                if (buf[buf_index+i])
                    debug_level = AV_LOG_ERROR;
            av_log(h->s.avctx, debug_level, "AVC: Consumed only %d bytes instead of %d\n", consumed, nalsize);
        }

        buf_index += consumed;

        /* ffdshow custom code */
        if(  (s->hurry_up == 1 && hx->nal_ref_idc  == 0) //FIXME do not discard SEI id
           ||(avctx->skip_frame >= AVDISCARD_NONREF && hx->nal_ref_idc  == 0))
            continue;

      again:
        err = 0;
        switch(hx->nal_unit_type){
        case NAL_IDR_SLICE:
            if (h->nal_unit_type != NAL_IDR_SLICE) {
                av_log(h->s.avctx, AV_LOG_ERROR, "Invalid mix of idr and non-idr slices");
                return -1;
            }
            idr(h); //FIXME ensure we don't loose some frames if there is reordering
        case NAL_SLICE:
            init_get_bits(&hx->s.gb, ptr, bit_length);
            hx->intra_gb_ptr=
            hx->inter_gb_ptr= &hx->s.gb;
            hx->s.data_partitioning = 0;

			// ==> Start patch MPC DXVA
			hx->ref_pic_flag = (h->nal_ref_idc != 0);
            if((err = decode_slice_header_noexecute(hx)))
               break;
			// <== End patch MPC DXVA

            s->current_picture_ptr->key_frame |=
                    (hx->nal_unit_type == NAL_IDR_SLICE) ||
                    (h->sei_recovery_frame_cnt >= 0);
            if(hx->redundant_pic_count==0 && hx->s.hurry_up < 5
               && (avctx->skip_frame < AVDISCARD_NONREF || hx->nal_ref_idc)
               && (avctx->skip_frame < AVDISCARD_BIDIR  || hx->slice_type_nos!=FF_B_TYPE)
               && (avctx->skip_frame < AVDISCARD_NONKEY || hx->slice_type_nos==FF_I_TYPE)
               && avctx->skip_frame < AVDISCARD_ALL){
                    context_count++;
            }
            break;
        case NAL_DPA:
            init_get_bits(&hx->s.gb, ptr, bit_length);
            hx->intra_gb_ptr=
            hx->inter_gb_ptr= NULL;
			// ==> Start patch MPC DXVA
            if ((err = decode_slice_header_noexecute(hx)) < 0)
                break;
			// <== End patch MPC DXVA
            hx->s.data_partitioning = 1;

            break;
        case NAL_DPB:
            init_get_bits(&hx->intra_gb, ptr, bit_length);
            hx->intra_gb_ptr= &hx->intra_gb;
            break;
        case NAL_DPC:
            init_get_bits(&hx->inter_gb, ptr, bit_length);
            hx->inter_gb_ptr= &hx->inter_gb;

            if(hx->redundant_pic_count==0 && hx->intra_gb_ptr && hx->s.data_partitioning
               && s->context_initialized
               && s->hurry_up < 5
               && (avctx->skip_frame < AVDISCARD_NONREF || hx->nal_ref_idc)
               && (avctx->skip_frame < AVDISCARD_BIDIR  || hx->slice_type_nos!=FF_B_TYPE)
               && (avctx->skip_frame < AVDISCARD_NONKEY || hx->slice_type_nos==FF_I_TYPE)
               && avctx->skip_frame < AVDISCARD_ALL)
                context_count++;
            break;
        case NAL_SEI:
            init_get_bits(&s->gb, ptr, bit_length);
            ff_h264_decode_sei(h);
            break;
        case NAL_SPS:
            init_get_bits(&s->gb, ptr, bit_length);
            ff_h264_decode_seq_parameter_set(h);

            if(s->flags& CODEC_FLAG_LOW_DELAY)
                s->low_delay=1;

            if(avctx->has_b_frames < 2)
                avctx->has_b_frames= !s->low_delay;
            break;
        case NAL_PPS:
            init_get_bits(&s->gb, ptr, bit_length);

            ff_h264_decode_picture_parameter_set(h, bit_length);

            break;
        case NAL_AUD:
        case NAL_END_SEQUENCE:
        case NAL_END_STREAM:
        case NAL_FILLER_DATA:
        case NAL_SPS_EXT:
        case NAL_AUXILIARY_SLICE:
            break;
        default:
            av_log(avctx, AV_LOG_DEBUG, "Unknown NAL code: %d (%d bits)\n", h->nal_unit_type, bit_length);
        }

        if(context_count == h->max_contexts) {
			// ==> Start patch MPC DXVA
			// execute_decode_slices(h, context_count);
			// <== End patch MPC DXVA
            context_count = 0;
        }

        if (err < 0)
            av_log(h->s.avctx, AV_LOG_ERROR, "decode_slice_header error\n");
        else if(err == 1) {
            /* Slice could not be decoded in parallel mode, copy down
             * NAL unit stuff to context 0 and restart. Note that
             * rbsp_buffer is not transferred, but since we no longer
             * run in parallel mode this should not be an issue. */
            h->nal_unit_type = hx->nal_unit_type;
            h->nal_ref_idc   = hx->nal_ref_idc;
            hx = h;
            goto again;
        }
    }
	// ==> Start patch MPC DXVA
    //if(context_count)
    //    execute_decode_slices(h, context_count);
	// <== End patch MPC DXVA
    return buf_index;
}


int av_h264_decode_frame(struct AVCodecContext* avctx, uint8_t *buf, int buf_size){
    H264Context *h = avctx->priv_data;
    MpegEncContext *s = &h->s;
//    AVFrame *pict = data;
    int buf_index;

    s->flags= avctx->flags;
    s->flags2= avctx->flags2;

   /* end of stream, output what is still in the buffers */
    if (buf_size == 0) {
        Picture *out;
        int i, out_idx;

//FIXME factorize this with the output code below
        out = h->delayed_pic[0];
        out_idx = 0;
        for(i=1; h->delayed_pic[i] && !h->delayed_pic[i]->key_frame && !h->delayed_pic[i]->mmco_reset; i++)
            if(h->delayed_pic[i]->poc < out->poc){
                out = h->delayed_pic[i];
                out_idx = i;
            }

        for(i=out_idx; h->delayed_pic[i]; i++)
            h->delayed_pic[i] = h->delayed_pic[i+1];

		// ==> Start patch MPC DXVA
        //if(out){
        //    *data_size = sizeof(AVFrame);
        //    *pict= *(AVFrame*)out;
        //}
		// <== End patch MPC DXVA

        return 0;
    }

    /* ffdshow custom code (begin) */
    if(h->is_avc && !h->got_avcC) {
        int i, cnt, nalsize;
        unsigned char *p = avctx->extradata, *pend=p+avctx->extradata_size;

        h->nal_length_size = 2;
        cnt = 1;

        for (i = 0; i < cnt; i++) {
            nalsize = AV_RB16(p) + 2;
            if(decode_nal_units(h, p, nalsize)  < 0) {
                av_log(avctx, AV_LOG_ERROR, "Decoding sps %d from avcC failed\n", i);
                return -1;
            }
            p += nalsize;
        }
        // Decode pps from avcC
        for (i = 0; p<pend-2; i++) {
            nalsize = AV_RB16(p) + 2;
            if(decode_nal_units(h, p, nalsize)  != nalsize) {
                av_log(avctx, AV_LOG_ERROR, "Decoding pps %d from avcC failed\n", i);
                return -1;
            }
            p += nalsize;
        }
        // Now store right nal length size, that will be use to parse all other nals
        h->nal_length_size = avctx->nal_length_size?avctx->nal_length_size:4;//((*(((char*)(avctx->extradata))+4))&0x03)+1;
        // Do not reparse avcC
        h->got_avcC = 1;
    }

    if(!h->got_avcC && !h->is_avc && s->avctx->extradata_size){
        if(decode_nal_units(h, s->avctx->extradata, s->avctx->extradata_size) < 0)
            return -1;
        h->got_avcC = 1;
        s->picture_number++;
    }
    /* ffdshow custom code (end) */

	// ==> Start patch MPC DXVA
	buf_index=decode_nal_units_noexecute(h, buf, buf_size);
	// <== End patch MPC DXVA
    if(buf_index < 0)
        return -1;

    if(!(s->flags2 & CODEC_FLAG2_CHUNKS) && !s->current_picture_ptr){
        if (avctx->skip_frame >= AVDISCARD_NONREF || s->hurry_up) return 0;
        av_log(avctx, AV_LOG_ERROR, "no frame!\n");
        return -1;
    }

    if(!(s->flags2 & CODEC_FLAG2_CHUNKS) || (s->mb_y >= s->mb_height && s->mb_height)){
        Picture *out = s->current_picture_ptr;
        Picture *cur = s->current_picture_ptr;
        int i, pics, cross_idr, out_of_order, out_idx;

		// ==> Start patch MPC DXVA
		field_end_noexecute(h);
		// <== End patch MPC DXVA

        if (cur->field_poc[0]==INT_MAX || cur->field_poc[1]==INT_MAX) {
            /* Wait for second field. */
			// ==> Start patch MPC DXVA
			h->outputed_poc = -1;
            //*data_size = 0;
			// <== End patch MPC DXVA

        } else {
            cur->interlaced_frame = 0;
            cur->repeat_pict = 0;

            /* Signal interlacing information externally. */
            /* Prioritize picture timing SEI information over used decoding process if it exists. */

            if(h->sps.pic_struct_present_flag){
                switch (h->sei_pic_struct)
                {
                case SEI_PIC_STRUCT_FRAME:
                    break;
                case SEI_PIC_STRUCT_TOP_FIELD:
                case SEI_PIC_STRUCT_BOTTOM_FIELD:
                    cur->interlaced_frame = 1;
                    break;
                case SEI_PIC_STRUCT_TOP_BOTTOM:
                case SEI_PIC_STRUCT_BOTTOM_TOP:
                    if (FIELD_OR_MBAFF_PICTURE)
                        cur->interlaced_frame = 1;
                    else
                        // try to flag soft telecine progressive
                        cur->interlaced_frame = h->prev_interlaced_frame;
                    break;
                case SEI_PIC_STRUCT_TOP_BOTTOM_TOP:
                case SEI_PIC_STRUCT_BOTTOM_TOP_BOTTOM:
                    // Signal the possibility of telecined film externally (pic_struct 5,6)
                    // From these hints, let the applications decide if they apply deinterlacing.
                    cur->repeat_pict = 1;
                    break;
                case SEI_PIC_STRUCT_FRAME_DOUBLING:
                    // Force progressive here, as doubling interlaced frame is a bad idea.
                    cur->repeat_pict = 2;
                    break;
                case SEI_PIC_STRUCT_FRAME_TRIPLING:
                    cur->repeat_pict = 4;
                    break;
                }
                if ((h->sei_ct_type & 3) && h->sei_pic_struct <= SEI_PIC_STRUCT_BOTTOM_TOP)
                    cur->interlaced_frame = (h->sei_ct_type & (1<<1)) != 0;
            }else{
                /* Derive interlacing flag from used decoding process. */
                cur->interlaced_frame = FIELD_OR_MBAFF_PICTURE;
            }
            h->prev_interlaced_frame = cur->interlaced_frame;

            if (cur->field_poc[0] != cur->field_poc[1]){
                /* Derive top_field_first from field pocs. */
                cur->top_field_first = cur->field_poc[0] < cur->field_poc[1];
            }else{
                if(cur->interlaced_frame || h->sps.pic_struct_present_flag){
                    /* Use picture timing SEI information. Even if it is a information of a past frame, better than nothing. */
                    if(h->sei_pic_struct == SEI_PIC_STRUCT_TOP_BOTTOM
                      || h->sei_pic_struct == SEI_PIC_STRUCT_TOP_BOTTOM_TOP)
                        cur->top_field_first = 1;
                    else
                        cur->top_field_first = 0;
                }else{
                    /* Most likely progressive */
                    cur->top_field_first = 0;
                }
            }

            /* ffdshow custom code */
            cur->video_full_range_flag = h->sps.video_full_range_flag;
            cur->YCbCr_RGB_matrix_coefficients = h->sps.matrix_coefficients;
        //FIXME do something with unavailable reference frames

            /* Sort B-frames into display order */

            if(h->sps.bitstream_restriction_flag
               && s->avctx->has_b_frames < h->sps.num_reorder_frames){
                s->avctx->has_b_frames = h->sps.num_reorder_frames;
                s->low_delay = 0;
            }

            if(   s->avctx->strict_std_compliance >= FF_COMPLIANCE_STRICT
               && !h->sps.bitstream_restriction_flag){
                s->avctx->has_b_frames= MAX_DELAYED_PIC_COUNT;
                s->low_delay= 0;
            }

            pics = 0;
            while(h->delayed_pic[pics]) pics++;

            assert(pics <= MAX_DELAYED_PIC_COUNT);

            h->delayed_pic[pics++] = cur;
            if(cur->reference == 0)
                cur->reference = DELAYED_PIC_REF;

            out = h->delayed_pic[0];
            out_idx = 0;
            for(i=1; h->delayed_pic[i] && !h->delayed_pic[i]->key_frame && !h->delayed_pic[i]->mmco_reset; i++)
                if(h->delayed_pic[i]->poc < out->poc){
                    out = h->delayed_pic[i];
                    out_idx = i;
                }
            cross_idr = !!h->delayed_pic[i] || h->delayed_pic[0]->key_frame || h->delayed_pic[0]->mmco_reset;

            out_of_order = !cross_idr && out->poc < h->outputed_poc;

            if(h->sps.bitstream_restriction_flag && s->avctx->has_b_frames >= h->sps.num_reorder_frames)
                { }
            else if((out_of_order && pics-1 == s->avctx->has_b_frames && s->avctx->has_b_frames < MAX_DELAYED_PIC_COUNT)
               || (s->low_delay &&
                ((!cross_idr && out->poc > h->outputed_poc + 2)
                 || cur->pict_type == FF_B_TYPE)))
            {
                s->low_delay = 0;
                s->avctx->has_b_frames++;
            }

            if(out_of_order || pics > s->avctx->has_b_frames){
                out->reference &= ~DELAYED_PIC_REF;
                for(i=out_idx; h->delayed_pic[i]; i++)
                    h->delayed_pic[i] = h->delayed_pic[i+1];
            }
			// ==> Start patch MPC DXVA
            if(!out_of_order && pics > s->avctx->has_b_frames){
                //*data_size = sizeof(AVFrame);

                h->outputed_poc = out->poc;
				h->outputed_rtstart = out->reordered_opaque;
                //*pict= *(AVFrame*)out;
            }else{
				h->outputed_poc = -1;
                av_log(avctx, AV_LOG_DEBUG, "no picture\n");
            }
			// <== End patch MPC DXVA
        }
    }

	// ==> Start patch MPC DXVA
    //assert(pict->data[0] || !*data_size);
    //ff_print_debug_info(s, pict);
	// <== End patch MPC DXVA

    return get_consumed_bytes(s, buf_index, buf_size);
}
