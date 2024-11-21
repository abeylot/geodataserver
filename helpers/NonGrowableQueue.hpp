#ifndef NonGrowableQueue_hpp
#define NonGrowableQueue_hpp
#endif
#include <mutex>
using namespace std::chrono_literals;
template<class T, int SIZE>
class NonGrowableQueue
{
    private:

    std::mutex _token;
    std::timed_mutex _empty;

    int next_in_key;
    int next_out_key;
    std::atomic<unsigned int> _used_size;
    T items[SIZE]{};
    bool itemIsUsed[SIZE]{};


    public:

    NonGrowableQueue():next_in_key(0),next_out_key(0),_used_size()
    {
        for(int i = 0; i < SIZE; i++) itemIsUsed[i] = false;
    }



    bool empty()
    {
        std::unique_lock lk(_token);
        bool res = !itemIsUsed[next_out_key];
        lk.unlock();
        return res;
    }

    bool push(T item)
    {

        std::unique_lock lk(_token);

        int key = next_in_key;
        if(itemIsUsed[key])
        {
            return false;
        } else {
            next_in_key = ( next_in_key + 1 ) % SIZE;;
            items[key] = item;
            itemIsUsed[key] = true;
            _used_size++;
            _empty.unlock();
            return true;
        }
    }

    bool pop(T& item)
    {
        std::unique_lock lk(_token);
        int key = next_out_key;
        if(!itemIsUsed[key])
        {
            return false;
        } else {
            item = items[key];
            itemIsUsed[key] = false;
            next_out_key = (next_out_key + 1) % SIZE;
            _used_size --;
            return true;
        }
    }

    bool pop_timed(T& item, std::chrono::seconds t = 1s)
    {
        bool locked = _empty.try_lock_for(t);
        if(locked)
        {
            bool result = pop(item);
            if(_used_size != 0 )
            {
                _empty.unlock();
            }
            return result;
        }
        else return false;
    }

};
