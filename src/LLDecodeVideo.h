#ifndef _DECODEVIDEO_H_
#define _DECODEVIDEO_H_

extern "C"
{
#include "ffmpeg/include/libavcodec/avcodec.h"
#include "ffmpeg/include/libavformat/avformat.h"
#include "ffmpeg/include/libswscale/swscale.h"
}

#include <string>
#include "LLWindow.h"
#include "LLPool.hpp"
using namespace std;

enum class LLError : int
{
	Ok = 0,
	Fail = -1,
	ErrorOpenFile = -1,
	NoFindStreamInfo = 1,

	NoFindVedio = 2 << AVMEDIA_TYPE_VIDEO,
	NoFindVideoCodec = 3 << AVMEDIA_TYPE_VIDEO,
	NoOpenVideoCodec = 4 << AVMEDIA_TYPE_VIDEO,

	NoFindAudio = 2 << AVMEDIA_TYPE_AUDIO,
	NoFindAudioCodec = 3 << AVMEDIA_TYPE_AUDIO,
	NoOpenAudioCodec = 4 << AVMEDIA_TYPE_AUDIO,

};

class LLFormatCtx
{
public:
    LLFormatCtx(string filename): m_video_strem_index(-1), m_audio_strem_index(-1)
    {
        m_filename = filename;
		m_pvideo_codec_ctx = nullptr;
		m_pvideo_codec = nullptr;

		m_paudio_codec_ctx = nullptr;
		m_paudio_codec = nullptr;
        
    }
    ~LLFormatCtx()
    {
        avcodec_close(m_pvideo_codec_ctx);
        avformat_close_input(&m_pformat_ctx);
    }
	LLError init_info()
    {
        av_register_all();
        avformat_network_init();
        
        m_pformat_ctx = avformat_alloc_context();
        
        if (0 != avformat_open_input(&m_pformat_ctx, m_filename.c_str(), NULL, NULL))
        {
            printf("couldn't open file\n");
            return LLError::ErrorOpenFile;
        }
        
        //retrieve stream information
        if (avformat_find_stream_info(m_pformat_ctx, NULL) < 0)
        {
            return LLError::NoFindStreamInfo;
        }        
        av_dump_format(m_pformat_ctx, -1, m_filename.c_str(), 0);
        
		m_video_strem_index = set_codec_ctx(AVMEDIA_TYPE_VIDEO, m_pvideo_codec_ctx, m_pvideo_codec);

		m_audio_strem_index = set_codec_ctx(AVMEDIA_TYPE_AUDIO, m_paudio_codec_ctx, m_paudio_codec);
   
		if (-1 != m_video_strem_index)
		{
			m_pvideo_stream = m_pformat_ctx->streams[m_video_strem_index];
			m_width = m_pvideo_codec_ctx->width;
			m_height = m_pvideo_codec_ctx->height;
		}

        return LLError::Ok;
    }
    AVFormatContext* get_format_ctx()
    {
        return m_pformat_ctx;
    }
    int width(){return m_width;}
    int height(){return m_height;}
private:
	int set_codec_ctx(AVMediaType type, AVCodecContext* &codec_ctx, AVCodec* &codec)
	{
		int stream_index = -1;
		for (int i = 0; i < m_pformat_ctx->nb_streams; ++i)
		{
			if (type == m_pformat_ctx->streams[i]->codec->codec_type)
			{
				stream_index = i;
				break;
			}
		}

		if (-1 == stream_index)
		{
			return stream_index;
		}

		codec_ctx = m_pformat_ctx->streams[stream_index]->codec;
		codec = avcodec_find_decoder(codec_ctx->codec_id);

		if (NULL == codec)
		{

			return -1;
		}

		if (avcodec_open2(codec_ctx, codec, NULL) < 0)
		{
			return -1;
		}
		return stream_index;
	}
public:
    AVFormatContext *m_pformat_ctx;
    int m_video_strem_index;
	int m_audio_strem_index;
    string m_filename;
    AVStream *m_pvideo_stream;

    AVCodecContext *m_pvideo_codec_ctx;    
    AVCodec *m_pvideo_codec;

	AVCodecContext *m_paudio_codec_ctx;
	AVCodec *m_paudio_codec;

    int m_width;
    int m_height;
private:

};

class LLDecodeVideo
{
public:
    LLDecodeVideo(LLFormatCtx& fmt_ctx);
    ~LLDecodeVideo();
    int decode_video(LLWindow* window);
private:
    void find_stream_index();
public:
private:
    LLFormatCtx& m_fmt_ctx;
};

#endif
