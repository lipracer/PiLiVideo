#include <cstdio>
#include <iostream>
#include <thread>
#include <chrono>
#include "src/LLDecodeVideo.h"
#include "src/LLVideoMgr.hpp"
using namespace std;

#define MAX_AUDIO_FRAME_SIZE 192000

static  Uint8  *audio_chunk;
static  Uint32  audio_len;
static  Uint8  *audio_pos;

LLPool<3> VideoInfo::pool(1700 * 960);

static void  fill_audio(void *udata, Uint8 *stream, int len) {
	//SDL 2.0
	//g_mtx.lock();
	printf("fill_audio--------------------------\n");
	SDL_memset(stream, 0, len);
	if (audio_len == 0)
		return;

	len = (len > audio_len ? audio_len : len);	/*  Mix  as  much  data  as  possible  */

	SDL_MixAudio(stream, audio_pos, len, SDL_MIX_MAXVOLUME);
	//audio_pos += len;
	audio_len -= len;
	//g_mtx.unlock();
}

LLDecodeVideo::LLDecodeVideo(LLFormatCtx& fmt_ctx) : m_fmt_ctx(fmt_ctx)
{
    
}

int LLDecodeVideo::decode_video(LLWindow* window)
{
    
    AVFrame *pFrame = av_frame_alloc();
    if (NULL == pFrame)
    {
        return -1;
    }
    
    AVFrame *pFrameRGB = av_frame_alloc();
    if (NULL == pFrameRGB)
    {
        return -1;
    }
    
    int numBytes = avpicture_get_size(AV_PIX_FMT_RGB24, m_fmt_ctx.width(),
                                  m_fmt_ctx.height());
    
    unsigned char *buffer = (uint8_t*)av_malloc(numBytes * sizeof(uint8_t));
    avpicture_fill((AVPicture*)pFrameRGB, buffer, AV_PIX_FMT_RGB24, m_fmt_ctx.width(), m_fmt_ctx.height());
    
    AVPacket packet;
    int frameFinished;
    
    window->m_video_start_tp = chrono::steady_clock::now();

	//Out Audio Param
	uint64_t out_channel_layout = AV_CH_LAYOUT_STEREO;
	//nb_samples: AAC-1024 MP3-1152
	int out_nb_samples = m_fmt_ctx.m_paudio_codec_ctx->frame_size;
	AVSampleFormat out_sample_fmt = AV_SAMPLE_FMT_S16;
	int out_sample_rate = 44100;
	int out_channels = av_get_channel_layout_nb_channels(out_channel_layout);
	//Out Buffer Size
	int out_buffer_size = av_samples_get_buffer_size(NULL, out_channels, out_nb_samples, out_sample_fmt, 1);

	uint8_t *out_buffer = (uint8_t *)av_malloc(MAX_AUDIO_FRAME_SIZE * 2);

	SDL_AudioSpec wanted_spec;
	wanted_spec.freq = out_sample_rate;
	wanted_spec.format = AUDIO_S16SYS;
	wanted_spec.channels = out_channels;
	wanted_spec.silence = 0;
	wanted_spec.samples = out_nb_samples;
	wanted_spec.callback = fill_audio;
	wanted_spec.userdata = m_fmt_ctx.m_paudio_codec_ctx;

	if (SDL_OpenAudio(&wanted_spec, NULL) < 0) {
		string error_msg = SDL_GetError();
		printf("can't open audio.\n");
		return -1;
	}


	//FIX:Some Codec's Context Information is missing
	long long int in_channel_layout = av_get_default_channel_layout(m_fmt_ctx.m_paudio_codec_ctx->channels);
	//Swr

	SwrContext* au_convert_ctx = swr_alloc();
	au_convert_ctx = swr_alloc_set_opts(au_convert_ctx, out_channel_layout, out_sample_fmt, out_sample_rate,
		in_channel_layout, m_fmt_ctx.m_paudio_codec_ctx->sample_fmt, m_fmt_ctx.m_paudio_codec_ctx->sample_rate, 0, NULL);
	swr_init(au_convert_ctx);

	int ret = 0;
	int got_picture = 0;
	int index = 0;
	//Play
	SDL_PauseAudio(0);


    while (av_read_frame(m_fmt_ctx.m_pformat_ctx, &packet) >= 0)
    {
        if (m_fmt_ctx.m_video_strem_index == packet.stream_index)
        {
            avcodec_decode_video2(m_fmt_ctx.m_pvideo_codec_ctx, pFrame, &frameFinished, &packet);
            if (frameFinished)
            {
                struct SwsContext *img_convert_ctx = NULL;
                img_convert_ctx = sws_getCachedContext(img_convert_ctx,
                                                       m_fmt_ctx.width(), m_fmt_ctx.height(),
                                                       m_fmt_ctx.m_pvideo_codec_ctx->pix_fmt, m_fmt_ctx.width(), m_fmt_ctx.height(),
                                                       AV_PIX_FMT_BGR24, SWS_BICUBIC, NULL, NULL, NULL);
                
                if (!img_convert_ctx)
                {
                    exit(1);
                }
                
                sws_scale(img_convert_ctx, pFrame->data, pFrame->linesize, 0, m_fmt_ctx.height(),
                          pFrameRGB->data, pFrameRGB->linesize);
                
                //window->wait_times(pFrame->pts * av_q2d(m_fmt_ctx.m_pstream->time_base) * 1000);
//                window->test((char*)pFrameRGB->data[0]);
                VideoInfo *info = new VideoInfo(m_fmt_ctx.width(), m_fmt_ctx.height(), 24, (char*)pFrameRGB->data[0], pFrame->pts * av_q2d(m_fmt_ctx.m_pvideo_stream->time_base) * 1000);
                LLVideoMgr::get_instance().m_video_queue.push_back(info);
                sws_freeContext(img_convert_ctx);
                
            }
        }
		else if (packet.stream_index == m_fmt_ctx.m_audio_strem_index)
		{
			int ret = avcodec_decode_audio4(m_fmt_ctx.m_paudio_codec_ctx, pFrame, &got_picture, &packet);
			if (ret < 0) {
				printf("Error in decoding audio frame.\n");
				return -1;
			}
			if (got_picture > 0) {
				swr_convert(au_convert_ctx, &out_buffer, MAX_AUDIO_FRAME_SIZE, (const uint8_t **)pFrame->data, pFrame->nb_samples);

				printf("index:%5d\t pts:%lld\t packet size:%d\n", index, packet.pts, packet.size);
				index++;
			}

			while (audio_len > 0)//Wait until finish
			{
				cout << "SDL_Delay-------------\n";
				SDL_Delay(1);
			}

			//Set audio buffer (PCM data)
			audio_chunk = (Uint8 *)out_buffer;
			//Audio buffer length
			audio_len = out_buffer_size;
			audio_pos = audio_chunk;

			//return 0;
		}
        av_free_packet(&packet);
    }
    
    av_free(buffer);
    av_free(pFrameRGB);
    av_free(pFrame);
    return 0;

}

LLDecodeVideo::~LLDecodeVideo()
{
    
}

