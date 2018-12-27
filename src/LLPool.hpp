#ifndef _LLPOOL_H_
#define _LLPOOL_H_

#include <list>
#include <mutex>

using namespace std;
using u64 = unsigned long long;

template <int count>
class LLPool
{
    union BufBlock
    {
        BufBlock *next;
        char block[0];
    };
public:
    LLPool(int buf_size) : _buf_size(buf_size)
    {
        _head = (BufBlock*)new char[buf_size * count];
        m_buf_list.push_back(_head);
        init_block(_head);
    }
    
    ~LLPool()
    {
        lock_guard<mutex> _(m_mtx);
        for (auto it : m_buf_list)
        {
            delete it;
        }
    }
    
    void* get_block()
    {
        lock_guard<mutex> _(m_mtx);
        if(nullptr == _head)
        {
            printf("WARN new buf!!!!!!!!!!!!!!!!!!\n");
            BufBlock *new_block = (BufBlock*)new char[_buf_size * count];
            m_buf_list.push_back(new_block);
            init_block(new_block);
            _head = new_block;
        }
		BufBlock *tmp = _head;
		_head = tmp->next;
        return tmp;
    }
    void free_block(void *block)
    {
        lock_guard<mutex> _(m_mtx);
		BufBlock* block_ = (BufBlock*)block;
		block_->next = _head;
        _head = block_;
    }
private:
    void init_block(BufBlock* head)
    {
        for (int i = 0; i < count - 1; ++i)
        {
            head->next = (BufBlock*)((char*)head + _buf_size);
            printf("%d ", (long long)head->next - (long long)head);
			head = head->next;
        }
        
		head->next = nullptr;
    }
public:

private:
    BufBlock *_head;
    u64 _buf_size;
    std::list<BufBlock*> m_buf_list;    
    mutex m_mtx;
};


#endif
