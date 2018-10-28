#ifndef _LLQUEUE_HPP_
#define _LLQUEUE_HPP_
#include <list>
#include <mutex>

using namespace std;

struct VideoInfo 
{
	int width;
	int height;
	int depth;
	char *pixels;
};

template <typename T>
class LLQueue 
{
public:
	void push_back(T& t) 
	{
		lock_guard<mutex> _(m_mtx);
		m_list.push_back(t);
	}

	void pop_front(T& t)
	{
		lock_guard<mutex> _(m_mtx);
		auto it = m_list.begin();
		m_list.pop_back();
		t = it;
	}

	size_t size() 
	{
		lock_guard<mutex> _(m_mtx);
		return m_list.size();
	}
private:
	std::list<T> m_list;
	mutex m_mtx;
};

#endif