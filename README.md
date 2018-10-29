

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

如果m_bstart false, 在wait前切换到上面线程，此时this->m_bstart = true; this->m_cdl.notify_one();执行完毕， 然后执行wait，将会死锁


因此必须先让主线程wait起来（此时wait会将独占锁解锁，然后等待子线程执行，为什么不让子线程先执行，因为子线程执行的时机不确定）

m_cdl.wait(uq_mtx, [this]()->bool { return this->m_bstart; });