#ifndef _VIDEOMGR_HPP_
#define _VIDEOMGR_HPP_

#include "src/LLQueue.hpp"

class LLVideoMgr
{
public:
	static LLVideoMgr& get_instance()
	{
		static LLVideoMgr mgr;
		return mgr;
	}
public:
	LLQueue<VideoInfo*> m_video_queue;
};

#endif
