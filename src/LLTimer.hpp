

#include <thread>
#include <condition_variable>
#include <mutex>
#include <chrono>
#include <iostream>

using namespace std;

class LLTimer 
{
public:
	LLTimer(chrono::milliseconds duration, function<void(void)> callback) : m_bquit(false), m_bstart(false)
	{
        m_duration = duration;
        m_callback = callback;
		unique_lock<mutex> uq_mtx(m_mtx);
		thread th([this](){

			unique_lock<mutex> uq_mtx(m_mtx);
			cout << "thread start!" << endl;
			this->m_bstart = true;
			this->m_cdl.notify_one();
			uq_mtx.unlock();

			while (!this->m_bquit)
			{
                this_thread::sleep_for(m_duration);
                m_callback();
			}
		});
		th.detach();
		//this_thread::sleep_for(chrono::milliseconds(10000));
		cout << "wait!" << endl;
		m_cdl.wait(uq_mtx, [this]()->bool { return this->m_bstart; });
	};
	~LLTimer() {};
    
    static LLTimer* create_timer(chrono::milliseconds duration, function<void(void)> callback)
    {
        return new LLTimer(duration, callback);
    }
    
private:
	//thread th;
public:
private:
	condition_variable m_cdl;
	mutex m_mtx;
	bool m_bquit;
	bool m_bstart;
    chrono::milliseconds m_duration;
    function<void(void)> m_callback;
};
