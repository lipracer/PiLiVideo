

#include <thread>
#include <condition_variable>
#include <mutex>
#include <chrono>
#include <iostream>

using namespace std;

class LLTimer 
{
public:
	LLTimer() : m_bquit(false), m_bstart(false)
	{
		unique_lock<mutex> uq_mtx(m_mtx);
		thread th([this](){

			unique_lock<mutex> uq_mtx(m_mtx);
			cout << "thread start!" << endl;
			this->m_bstart = true;
			this->m_cdl.notify_one();
			uq_mtx.unlock();


			while (!this->m_bquit)
			{
				this_thread::sleep_for(chrono::microseconds(1));
			}
		});
		th.detach();
		this_thread::sleep_for(chrono::milliseconds(10000));
		cout << "wait!" << endl;
		m_cdl.wait(uq_mtx, [this]()->bool { return this->m_bstart; });
	};
	~LLTimer() {};
private:
	//thread th;
public:
private:
	condition_variable m_cdl;
	mutex m_mtx;
	bool m_bquit;
	bool m_bstart;
};