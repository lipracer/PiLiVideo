//定义阻塞队列模板,音频、视频帧解析出来加入队列
//当队列达到最大值阻塞当前解析线程（每帧数据量很大，内存占用率高，可以尝试写到硬盘？？？？？）
//当队列数据消费完毕也要阻塞线程， 

#ifndef _LLQUEUE_HPP_
#define _LLQUEUE_HPP_
#include <list>
#include <mutex>
#include <condition_variable>
#include "src/LLPool.hpp"

using namespace std;

struct VideoInfo 
{
	static LLPool<3, 1700*760*3> pool;
	int width;
	int height;
	int depth;
	char *pixels;
    long long int pts;
    VideoInfo(int width_, int height_, int depth_, const char* pixels_, long long int pts_)
    {
        width = width_;
        height = height_;
        depth = depth_;
        pts = pts_;
        int data_len = width_ * height_ * (depth_ >> 3);
        //pixels = pixels_;//new char [data_len];
        pixels = VideoInfo::pool.get_block();
        memcpy(pixels, pixels_, data_len);
    }
    ~VideoInfo()
    {
        VideoInfo::pool.free_block((char*)pixels);
    }
};


template <typename T>
class LLQueue 
{
	constexpr static int Max_Size = 3;
public:
	void push_back(T t)
	{
		unique_lock<mutex> ulock(m_mtx);
        
        m_cdv.wait(ulock, [this]()->bool{return !(Max_Size == m_list.size());});

		m_list.push_back(t);
        cout << "push coutn:" << m_list.size() << endl;
		if(1 == m_list.size())
		{
			m_cdv.notify_one();
		}
	}

	void pop_front(T& t)
	{
		unique_lock<mutex> ulock(m_mtx);

        m_cdv.wait(ulock, [this]()->bool{return !(0 == m_list.size());});

		auto it = m_list.begin();
        t = *it;
		m_list.pop_front();
		cout << "pop coutn:" << m_list.size() << endl;

		if(Max_Size-1 == m_list.size())
		{
			m_cdv.notify_one();
		}
	}
    
    T front()
    {
        if(size())
        {
            return *m_list.begin();
        }
        return (T)nullptr;
    }

	size_t size() 
	{
		unique_lock<mutex> ulock(m_mtx);
		return m_list.size();
	}
private:
	std::list<T> m_list;
	mutex m_mtx;
	condition_variable m_cdv;
};

#endif
