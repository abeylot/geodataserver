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
            return true;
        }
    }

};
