#include <cstdio>
#include <iostream>

#include "src/DecodeVideo.h"

extern "C"
{
#include "ffmpeg/include/libavcodec/avcodec.h"
#include "ffmpeg/include/libavformat/avformat.h"
#include "ffmpeg/include/libswscale/swscale.h"
}

#define SRC_FILE "./wangzhe.mp4"

int decode_video()
{
	AVFormatContext *pFormatCtx;
	int i, videoStream;
	AVCodecContext *pCodecCtx;
	AVCodec *pCodec;
	AVFrame *pFrame;
	AVFrame *pFrameRGB;
	AVPacket packet;
	int frameFinished;
	int numBytes;
	uint8_t *buffer;


	//register all formats and codes
	av_register_all();
	//support network stream input
	avformat_network_init();
	pFormatCtx = avformat_alloc_context();

	if (0 != avformat_open_input(&pFormatCtx, SRC_FILE, NULL, NULL))
	{
		printf("couldn't open file\n");
		return -1;
	}

	//retrieve stream information
	if (avformat_find_stream_info(pFormatCtx, NULL) < 0)
	{
		return -1;
	}

	av_dump_format(pFormatCtx, -1, SRC_FILE, 0);

	videoStream = -1;

	for (int i = 0; i < pFormatCtx->nb_streams; ++i)
	{
		if (AVMEDIA_TYPE_VIDEO == pFormatCtx->streams[i]->codec->codec_type)
		{
			videoStream = i;
			break;
		}
	}

	if (-1 == videoStream)
	{
		return -1;
	}

	AVStream *pStream = pFormatCtx->streams[videoStream];
	pCodecCtx = pFormatCtx->streams[videoStream]->codec;
	pCodec = avcodec_find_decoder(pCodecCtx->codec_id);

	if (NULL == pCodec)
	{

		return -1;
	}

	if (avcodec_open2(pCodecCtx, pCodec, NULL) < 0)
	{
		return -1;
	}

	pFrame = av_frame_alloc();
	if (NULL == pFrame)
	{
		return -1;
	}

	pFrameRGB = av_frame_alloc();
	if (NULL == pFrameRGB)
	{
		return -1;
	}

	numBytes = avpicture_get_size(AV_PIX_FMT_RGB24, pCodecCtx->width,
		pCodecCtx->height);

	buffer = (uint8_t*)av_malloc(numBytes * sizeof(uint8_t));
	avpicture_fill((AVPicture*)pFrameRGB, buffer, AV_PIX_FMT_RGB24,
		pCodecCtx->width, pCodecCtx->height);


	while (av_read_frame(pFormatCtx, &packet) >= 0)
	{
		if (videoStream == packet.stream_index)
		{
			avcodec_decode_video2(pCodecCtx, pFrame, &frameFinished,
				&packet);
			if (frameFinished)
			{
				struct SwsContext *img_convert_ctx = NULL;
				img_convert_ctx = sws_getCachedContext(img_convert_ctx,
					pCodecCtx->width, pCodecCtx->height,
					pCodecCtx->pix_fmt, pCodecCtx->width, pCodecCtx->height,
					AV_PIX_FMT_BGR24, SWS_BICUBIC, NULL, NULL, NULL);

				if (!img_convert_ctx)
				{
					exit(1);
				}

				sws_scale(img_convert_ctx, pFrame->data, pFrame->linesize, 0, pCodecCtx->height,
					pFrameRGB->data, pFrameRGB->linesize);

				//player_win.wait_times(pFrame->pts * av_q2d(pStream->time_base) * 1000);
				//player_win.draw_img((char*)pFrameRGB->data[0]);

			}
		}
		av_free_packet(&packet);
	}

	av_free(buffer);
	av_free(pFrameRGB);
	av_free(pFrame);
	avcodec_close(pCodecCtx);
	avformat_close_input(&pFormatCtx);
}