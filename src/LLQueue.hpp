//定义阻塞队列模板,音频、视频帧解析出来加入队列
//当队列达到最大值阻塞当前解析线程（每帧数据量很大，内存占用率高，可以尝试写到硬盘？？？？？）
//当队列数据消费完毕也要阻塞线程， 

#ifndef _LLQUEUE_HPP_
#define _LLQUEUE_HPP_
#include <list>
#include <mutex>
#include <condition_variable>

using namespace std;

struct VideoInfo 
{
	int width;
	int height;
	int depth;
	char *pixels;
    VideoInfo(int width_, int height_, int depth_, const char* pixels_)
    {
        width = width_;
        height = height_;
        depth = depth_;
        int data_len = width_ * height_ * (depth_ >> 3);
        pixels = new char [data_len];
        memcpy(pixels, pixels_, data_len);
    }
};

template <typename T>
class LLQueue 
{
	constexpr static int Max_Size = 10;
public:
	void push_back(T t)
	{
		unique_lock<mutex> ulock(m_mtx);
        
        cout << "wait...\n";
        m_cdv.wait(ulock, [this]()->bool{return !(Max_Size == m_list.size());});

		m_list.push_back(t);

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
		m_list.pop_back();
		

		if(Max_Size-1 == m_list.size())
		{
			m_cdv.notify_one();
		}
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
