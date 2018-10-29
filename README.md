

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

���m_bstart false, ��waitǰ�л��������̣߳���ʱthis->m_bstart = true; this->m_cdl.notify_one();ִ����ϣ� Ȼ��ִ��wait����������


��˱����������߳�wait��������ʱwait�Ὣ��ռ��������Ȼ��ȴ����߳�ִ�У�Ϊʲô�������߳���ִ�У���Ϊ���߳�ִ�е�ʱ����ȷ����

m_cdl.wait(uq_mtx, [this]()->bool { return this->m_bstart; });