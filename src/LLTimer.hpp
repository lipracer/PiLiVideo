

#include <thread>
#include <condition_variable>
#include <mutex>
#include <chrono>
#include <iostream>

using namespace std;

class LLTimer 
{
public:
	LLTimer(chrono::milliseconds duration, function<void(long long int)> callback) : m_bquit(false), m_bstart(false)
	{
        m_duration = duration;
        m_callback = callback;
		unique_lock<mutex> uq_mtx(m_mtx);
		thread th([this](){

			unique_lock<mutex> uq_mtx(m_mtx);
			cout << "timer thread start!" << endl;
			this->m_bstart = true;
			this->m_cdl.notify_one();
			uq_mtx.unlock();
            m_star_tp = chrono::steady_clock::now();
			while (!this->m_bquit)
			{
                m_cur_tp = chrono::steady_clock::now();				
                this_thread::sleep_for(m_duration);
                long long int tick = (chrono::duration_cast<chrono::milliseconds>(m_cur_tp - m_star_tp)).count();
				cout << "current time point:" << tick << endl;
                m_callback(tick);
			}
		});
		th.detach();
		//this_thread::sleep_for(chrono::milliseconds(10000));
		cout << "wait!" << endl;
		m_cdl.wait(uq_mtx, [this]()->bool { return this->m_bstart; });
	};
	~LLTimer() {};
    
    static LLTimer* create_timer(chrono::milliseconds duration, function<void(long long int)> callback)
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
    chrono::time_point<chrono::steady_clock> m_star_tp;
    chrono::time_point<chrono::steady_clock> m_cur_tp;
    chrono::milliseconds m_duration;
    function<void(long long int tickcount)> m_callback;
};
