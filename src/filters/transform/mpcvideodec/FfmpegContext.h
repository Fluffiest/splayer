/* 
 * $Id: FfmpegContext.h 1207 2009-08-02 15:35:14Z casimir666 $
 *
 * (C) 2006-2007 see AUTHORS
 *
 * This file is part of mplayerc.
 *
 * Mplayerc is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 *
 * Mplayerc is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#pragma once

#include <dxva.h>

struct AVCodecContext;


enum PCI_Vendors
{
    PCIV_ATI				= 0x1002,
    PCIV_nVidia				= 0x10DE,
    PCIV_Intel				= 0x8086,
    PCIV_S3_Graphics		= 0x5333
};

// Bitmasks for DXVA compatibility check
#define DXVA_UNSUPPORTED_LEVEL   1
#define DXVA_TOO_MUCH_REF_FRAMES 2
#define DXVA_INCOMPATIBLE_SAR    4

// === H264 functions
void			FFH264DecodeBuffer (struct AVCodecContext* pAVCtx, BYTE* pBuffer, UINT nSize, int* pFramePOC, int* pOutPOC, REFERENCE_TIME* pOutrtStart);
HRESULT			FFH264BuildPicParams (DXVA_PicParams_H264* pDXVAPicParams, DXVA_Qmatrix_H264* pDXVAScalingMatrix, int* nFieldType, int* nSliceType, struct AVCodecContext* pAVCtx, int nPCIVendor);
int				FFH264CheckCompatibility(int nWidth, int nHeight, struct AVCodecContext* pAVCtx, BYTE* pBuffer, UINT nSize, int nPCIVendor, int m_nPCIDevice, LARGE_INTEGER VideoDriverVersion, int* refFrameCount);
void			FFH264SetCurrentPicture (int nIndex, DXVA_PicParams_H264* pDXVAPicParams, struct AVCodecContext* pAVCtx);
void			FFH264UpdateRefFramesList (DXVA_PicParams_H264* pDXVAPicParams, struct AVCodecContext* pAVCtx);
BOOL			FFH264IsRefFrameInUse (int nFrameNum, struct AVCodecContext* pAVCtx);
void			FF264UpdateRefFrameSliceLong(DXVA_PicParams_H264* pDXVAPicParams, DXVA_Slice_H264_Long* pSlice, struct AVCodecContext* pAVCtx);
void			FFH264SetDxvaSliceLong (struct AVCodecContext* pAVCtx, void* pSliceLong);

// === VC1 functions
HRESULT			FFVC1UpdatePictureParam (DXVA_PictureParameters* pPicParams, struct AVCodecContext* pAVCtx, int* nFieldType, int* nSliceType, BYTE* pBuffer, UINT nSize);
int				FFIsSkipped(struct AVCodecContext* pAVCtx);

// === Mpeg2 functions
HRESULT			FFMpeg2DecodeFrame (DXVA_PictureParameters* pPicParams, DXVA_QmatrixData* m_QMatrixData, DXVA_SliceInfo* pSliceInfo, int* nSliceCount,
struct AVCodecContext* pAVCtx, struct AVFrame* pFrame, int* nNextCodecIndex, int* nFieldType, int* nSliceType, BYTE* pBuffer, UINT nSize);

// === Common functions
int				IsVista();
char*			GetFFMpegPictureType(int nType);
int				FFIsInterlaced(struct AVCodecContext* pAVCtx, int nHeight);
unsigned long	FFGetMBNumber(struct AVCodecContext* pAVCtx);
//void			FFSetThreadNumber(struct AVCodecContext* pAVCtx, int nThreadCount);
//BOOL			FFSoftwareCheckCompatibility(struct AVCodecContext* pAVCtx);
int				FFGetCodedPicture(struct AVCodecContext* pAVCtx);
BOOL			FFGetAlternateScan(struct AVCodecContext* pAVCtx);
