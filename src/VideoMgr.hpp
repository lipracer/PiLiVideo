#ifndef _VIDEOMGR_HPP_
#define _VIDEOMGR_HPP_

#include "src/LLQueue.hpp"

class VideoMgr 
{
public:
	static VideoMgr& get_instance()
	{
		static VideoMgr mgr;
		return mgr;
	}
private:
	LLQueue<VideoInfo> m_video_queue;
};

#endif