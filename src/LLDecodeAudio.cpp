#include <mutex>
#include "LLDecodeAudio.h"
extern "C" 
{
#include "ffmpeg/include/libswresample/swresample.h"
}

using namespace std;

#define MAX_AUDIO_FRAME_SIZE 192000

static  Uint8  *audio_chunk;
static  Uint32  audio_len;
static  Uint8  *audio_pos;

static mutex g_mtx;

static void  fill_audio(void *udata, Uint8 *stream, int len) {
	//SDL 2.0
	//g_mtx.lock();
	printf("fill_audio--------------------------\n");
	SDL_memset(stream, 0, len);
	if (audio_len == 0)
		return;

	len = (len > audio_len ? audio_len : len);	/*  Mix  as  much  data  as  possible  */

	SDL_MixAudio(stream, audio_pos, len, SDL_MIX_MAXVOLUME);
	audio_pos += len;
	audio_len -= len;
	//g_mtx.unlock();
}

LLDecodeAudio::LLDecodeAudio(LLFormatCtx& fmt_ctx) : m_fmt_ctx(fmt_ctx)
{
}

int LLDecodeAudio::decode_audio()
{
	AVPacket *packet = (AVPacket *)av_malloc(sizeof(AVPacket));
	av_init_packet(packet);

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
	AVFrame *pFrame = av_frame_alloc();
	//SDL------------------

	//Init
	//if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_TIMER)) {
	//	printf("Could not initialize SDL - %s\n", SDL_GetError());
	//	return -1;
	//}
	//SDL_AudioSpec
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

	while (av_read_frame(m_fmt_ctx.m_pformat_ctx, packet) >= 0) {
		if (packet->stream_index == m_fmt_ctx.m_audio_strem_index)
		{
			ret = avcodec_decode_audio4(m_fmt_ctx.m_paudio_codec_ctx, pFrame, &got_picture, packet);
			if (ret < 0) {
				printf("Error in decoding audio frame.\n");
				return -1;
			}
			if (got_picture > 0) {
				swr_convert(au_convert_ctx, &out_buffer, MAX_AUDIO_FRAME_SIZE, (const uint8_t **)pFrame->data, pFrame->nb_samples);

				printf("index:%5d\t pts:%lld\t packet size:%d\n", index, packet->pts, packet->size);
				index++;
			}

			g_mtx.lock();
			while (audio_len > 0)//Wait until finish
				SDL_Delay(1);

			//Set audio buffer (PCM data)
			audio_chunk = (Uint8 *)out_buffer;
			//Audio buffer length
			audio_len = out_buffer_size;
			audio_pos = audio_chunk;
			g_mtx.unlock();
			//return 0;
		}
		av_free_packet(packet);
	}

	swr_free(&au_convert_ctx);

	SDL_CloseAudio();//Close SDL
	//SDL_Quit();
	return 0;
}