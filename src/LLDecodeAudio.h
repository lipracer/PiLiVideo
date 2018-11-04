#ifndef _LLDECODEAUDIO_H_
#define _LLDECODEAUDIO_H_

//template decode_video && decode_audio 
#include "LLDecodeVideo.h"

//const int MAX_AUDIO_FRAME_SIZE = 192000;

class LLDecodeAudio
{
public:
	LLDecodeAudio(LLFormatCtx& fmt_ctx);
	int decode_audio();
private:
public:
private:
	LLFormatCtx& m_fmt_ctx;
};

#endif 
