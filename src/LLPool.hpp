#ifndef _LLPOOL_H_
#define _LLPOOL_H_

#include <list>
#include <mutex>

using namespace std;
template <int count, int size>
class LLPool
{
    union BufBlock
    {
        BufBlock *next;
        char block[size];
    };
public:
    LLPool()
    {
        _head = (BufBlock*)new char[sizeof(BufBlock) * count];
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
    
    char* get_block()
    {
        lock_guard<mutex> _(m_mtx);
        if(nullptr == _head)
        {
            printf("WARN new buf!!!!!!!!!!!!!!!!!!\n");
            BufBlock *new_block = (BufBlock*)new char[sizeof(BufBlock) * count];
            m_buf_list.push_back(new_block);
            init_block(new_block);
            _head = new_block;
        }
		BufBlock *tmp = _head;
		_head = tmp->next;
        return (char*)tmp;
    }
    void free_block(char *block)
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
            head->next = head + 1;
            printf("%d ", (long long)head->next - (long long)head);
			head = head->next;
        }
        
		head->next = nullptr;
    }
public:

private:
    BufBlock *_head;
    std::list<BufBlock*> m_buf_list;    
    mutex m_mtx;
};


#endif
