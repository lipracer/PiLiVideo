#ifndef _LLPOOL_H_
#define _LLPOOL_H_


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
        init_block(_head);
    }
    
    ~LLPool()
    {
        while (_head->next)
        {
            BufBlock *tmp = _head->next;
            delete tmp;
            _head = tmp;
        }
    }
    
    char* get_block()
    {
        if(nullptr == _head->next)
        {
            BufBlock *new_block = (BufBlock*)new char[sizeof(BufBlock) * count];
            init_block(new_block);
            _head->next = new_block;
        }
        return _head->next;
    }
    char* free_block(BufBlock *block)
    {
        block->next = _head;
        _head = block;
    }
private:
    void init_block(BufBlock* head)
    {
        for (int i = 0; i < count - 1; ++i)
        {
            head->next = head + 1;
        }
        (head + cout)->next = nullptr;
    }
public:
private:
    BufBlock *_head;
    BufBlock *_cur_head;
    int _index;
    //template <int size>

    
};


#endif
