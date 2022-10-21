#ifndef ExtThreads_hpp
#define ExtThreads_hpp
#include <thread>
#include <map>

namespace ExtThreads
{
	bool shall_stop;
	
	std::vector<std::thread*> ext_threads;
	
	
	void init()
	{
		shall_stop = false;
	}
	
	template<class F>
	void launch_thread(F f)
	{
		std::thread* t = new std::thread(f);
		ext_threads.push_back(t);
	}
	
	void request_all_to_stop()
	{
	    shall_stop = true;
	}

	void request_all_to_join()
	{
		for(auto &t : ext_threads)
		{
			t->join();
		} 

		for(auto &t : ext_threads)
		{
			delete(t);
		}
		ext_threads.clear(); 
	}
	
	bool stop_requested()
	{
		return shall_stop;
	}
	
}
#endif
