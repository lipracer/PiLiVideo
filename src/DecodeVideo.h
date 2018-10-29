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
using namespace std;

class FormatCtx
{
public:
    FormatCtx(string filename): m_video_strem(-1)
    {
        m_filename = filename;
        
    }
    ~FormatCtx()
    {
        avcodec_close(m_pcodec_ctx);
        avformat_close_input(&m_pformat_ctx);
    }
    int init_info()
    {
        av_register_all();
        avformat_network_init();
        
        m_pformat_ctx = avformat_alloc_context();
        
        if (0 != avformat_open_input(&m_pformat_ctx, m_filename.c_str(), NULL, NULL))
        {
            printf("couldn't open file\n");
            return -1;
        }
        
        //retrieve stream information
        if (avformat_find_stream_info(m_pformat_ctx, NULL) < 0)
        {
            return -1;
        }
        
        av_dump_format(m_pformat_ctx, -1, m_filename.c_str(), 0);
        

        for (int i = 0; i < m_pformat_ctx->nb_streams; ++i)
        {
            if (AVMEDIA_TYPE_VIDEO == m_pformat_ctx->streams[i]->codec->codec_type)
            {
                m_video_strem = i;
                break;
            }
        }
        
        if (-1 == m_video_strem)
        {
            return -1;
        }
        
        m_pstream = m_pformat_ctx->streams[m_video_strem];
        m_pcodec_ctx = m_pformat_ctx->streams[m_video_strem]->codec;
        m_pcodec = avcodec_find_decoder(m_pcodec_ctx->codec_id);
        
        if (NULL == m_pcodec)
        {
            
            return -1;
        }
        
        if (avcodec_open2(m_pcodec_ctx, m_pcodec, NULL) < 0)
        {
            return -1;
        }
        m_width = m_pcodec_ctx->width;
        m_height = m_pcodec_ctx->height;
        return 0;
    }
    AVFormatContext* get_format_ctx()
    {
        return m_pformat_ctx;
    }
    int width(){return m_width;}
    int height(){return m_height;}
private:
public:
    AVFormatContext *m_pformat_ctx;
    int m_video_strem;
    string m_filename;
    AVStream *m_pstream;
    AVCodecContext *m_pcodec_ctx;
    
    AVCodec *m_pcodec;
    int m_width;
    int m_height;
private:

};

class DecodeVideo
{
public:
    DecodeVideo(FormatCtx& fmt_ctx);
    ~DecodeVideo();
    int decode_video(LLWindow* window);
private:
    void find_stream_index();
public:
private:
    FormatCtx& m_fmt_ctx;
};

#endif
