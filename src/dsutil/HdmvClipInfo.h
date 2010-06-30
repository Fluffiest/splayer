/* 
 * $Id: HdmvClipInfo.h 988 2009-02-01 21:33:05Z beliyaal $
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

enum ElementaryStreamTypes
{
    INVALID								= 0,
    VIDEO_STREAM_MPEG1					= 0x01,
    VIDEO_STREAM_MPEG2					= 0x02,
    AUDIO_STREAM_MPEG1					= 0x03, // all layers including mp3
    AUDIO_STREAM_MPEG2					= 0x04,
    VIDEO_STREAM_H264					= 0x1b,
    AUDIO_STREAM_LPCM					= 0x80,
    AUDIO_STREAM_AC3					= 0x81,
    AUDIO_STREAM_DTS					= 0x82,
    AUDIO_STREAM_AC3_TRUE_HD			= 0x83,
    AUDIO_STREAM_AC3_PLUS				= 0x84,
    AUDIO_STREAM_DTS_HD					= 0x85,
    AUDIO_STREAM_DTS_HD_MASTER_AUDIO	= 0x86,
    PRESENTATION_GRAPHICS_STREAM		= 0x90,
    INTERACTIVE_GRAPHICS_STREAM			= 0x91,
    SUBTITLE_STREAM						= 0x92,
    SECONDARY_AUDIO_AC3_PLUS			= 0xa1,
    SECONDARY_AUDIO_DTS_HD				= 0xa2,
    VIDEO_STREAM_VC1					= 0xea
};

enum BDVM_VideoFormat
{
    BDVM_VideoFormat_Unknown = 0,
    BDVM_VideoFormat_480i = 1,
    BDVM_VideoFormat_576i = 2,
    BDVM_VideoFormat_480p = 3,
    BDVM_VideoFormat_1080i = 4,
    BDVM_VideoFormat_720p = 5,
    BDVM_VideoFormat_1080p = 6,
    BDVM_VideoFormat_576p = 7,
};

enum BDVM_FrameRate
{
    BDVM_FrameRate_Unknown = 0,
    BDVM_FrameRate_23_976 = 1,
    BDVM_FrameRate_24 = 2,
    BDVM_FrameRate_25 = 3,
    BDVM_FrameRate_29_97 = 4,
    BDVM_FrameRate_50 = 6,
    BDVM_FrameRate_59_94 = 7
};

enum BDVM_AspectRatio
{
    BDVM_AspectRatio_Unknown = 0,
    BDVM_AspectRatio_4_3 = 2,
    BDVM_AspectRatio_16_9 = 3,
    BDVM_AspectRatio_2_21 = 4
};

enum BDVM_ChannelLayout
{
    BDVM_ChannelLayout_Unknown = 0,
    BDVM_ChannelLayout_MONO = 1,
    BDVM_ChannelLayout_STEREO = 3,
    BDVM_ChannelLayout_MULTI = 6,
    BDVM_ChannelLayout_COMBO = 12
};

enum BDVM_SampleRate
{
    BDVM_SampleRate_Unknown = 0,
    BDVM_SampleRate_48 = 1,
    BDVM_SampleRate_96 = 4,
    BDVM_SampleRate_192 = 5,
    BDVM_SampleRate_48_192 = 12,
    BDVM_SampleRate_48_96 = 14
};

typedef unsigned char uint8;
typedef signed char int8;

typedef unsigned short uint16;
typedef short int16;

typedef unsigned long uint32;
typedef long int32;



class CHdmvClipInfo
{
public:

	struct Stream
	{
		Stream()
		{
			memset(this, 0, sizeof(*this));
		}
		SHORT					m_PID;
		ElementaryStreamTypes	m_Type;
		char					m_LanguageCode[4];
		LCID					m_LCID;

		// Valid for video types
		BDVM_VideoFormat		m_VideoFormat;
		BDVM_FrameRate			m_FrameRate;
		BDVM_AspectRatio		m_AspectRatio;
		// Valid for audio types
		BDVM_ChannelLayout		m_ChannelLayout;
		BDVM_SampleRate			m_SampleRate;

		LPCTSTR Format();
	};


	struct PlaylistItem
	{
		CString					m_strFileName;
		REFERENCE_TIME			m_rtIn;
		REFERENCE_TIME			m_rtOut;

		REFERENCE_TIME Duration()
		{
			return m_rtOut - m_rtIn;
		}

		bool operator == (const PlaylistItem& pi) const
		{
			return pi.m_strFileName == m_strFileName;
		}
	};

	CHdmvClipInfo(void);
	~CHdmvClipInfo();

	HRESULT		ReadInfo(LPCTSTR strFile);
	Stream*		FindStream(SHORT wPID);
	bool		IsHdmv()					{ return m_bIsHdmv; };
	int			GetStreamNumber()			{ return int(m_Streams.GetCount()); };
	Stream*		GetStreamByIndex(int nIndex){ return (unsigned(nIndex) < m_Streams.GetCount()) ? &m_Streams[nIndex] : NULL; };

	HRESULT		FindMainMovie(LPCTSTR strFolder, CString& strPlaylistFile, CAtlList<PlaylistItem>& MainPlaylist);
	HRESULT		ReadPlaylist(CString strPlaylistFile, REFERENCE_TIME& rtDuration, CAtlList<PlaylistItem>& Playlist);

private :
	DWORD		SequenceInfo_start_address;
	DWORD		ProgramInfo_start_address;

	HANDLE		m_hFile;


	CAtlArray<Stream>	m_Streams;
	bool				m_bIsHdmv;

	DWORD		ReadDword();
	SHORT		ReadShort();
	BYTE		ReadByte();
	void		ReadBuffer(BYTE* pBuff, int nLen);

	HRESULT		ReadProgramInfo();
	HRESULT		ReadPlaylist(LPCTSTR strPath, LPCTSTR strFile, REFERENCE_TIME& rtDuration, CAtlList<CString>& Playlist);
	HRESULT		CloseFile(HRESULT hr);
};
