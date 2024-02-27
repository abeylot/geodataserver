#ifndef NonGrowableQueue_hpp
#define NonGrowableQueue_hpp
#endif
#include <mutex>
template<class T, int SIZE>
class NonGrowableQueue
{
    private:

    std::mutex _token;

    int next_in_key;
    int next_out_key;
    T items[SIZE]{};
    bool itemIsUsed[SIZE]{};


    public:

    NonGrowableQueue():next_in_key(0),next_out_key(0)
    {
        for(int i = 0; i < SIZE; i++) itemIsUsed[i] = false;
    }



    bool empty()
    {
        _token.lock();
        bool res = !itemIsUsed[next_out_key];
        _token.unlock();
        return res;
    }

    bool push(T item)
    {

        _token.lock();

        int key = next_in_key;
        if(itemIsUsed[key])
        {
            _token.unlock();
            return false;
        } else {
            next_in_key = ( next_in_key + 1 ) % SIZE;;
            items[key] = item;
            itemIsUsed[key] = true;
            _token.unlock();
            return true;
        }
    }

    bool pop(T& item)
    {
        _token.lock();
        int key = next_out_key;
        if(!itemIsUsed[key])
        {
            _token.unlock();
            return false;
        } else {
            item = items[key];
            itemIsUsed[key] = false;
            next_out_key = (next_out_key + 1) % SIZE;
            _token.unlock();
            return true;
        }
    }

};
