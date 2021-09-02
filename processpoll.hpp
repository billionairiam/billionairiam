#include<pthread.h>
#include<exception>

#include<list>
#include<cstdio>
#include"locker.hpp"
template<typename T>
class threadpoll{
public:  
  threadpoll(int threadnum,int max_requestnum):m_threadnum(threadnum),max_request(max_requestnum),m_stop(false),pool(NULL)
  {
    
    if(m_threadnum<0||max_request<0)
      throw std::exception();
    
    pool=new pthread_t[threadnum];
    
    if(!pool)
      throw std::exception();
    
    for(int i=0;i<threadnum;++i)
    {
      if(!pthread_create(pool+i,NULL,worker,this))
      {
        delete[] pool;
        throw std::exception();
      }
      if(pthread_detach(pool[i]))
      {
        delete[] pool;
        throw std::exception();
      }
    }
    
  }
  
  ~threadpoll()
  {
    delete[] pool;
    m_stop=true;
  }

  bool push(T* work)
  {
    
    m_lock.lock();
    if(m_workers.size()>max_request)
    {
      m_lock.unlock();
      return false;
    }
    m_workers.push_back(work);
    m_lock.unlock();
    m_sem.post();
    return true;

  }


private:
  static void* worker(void *args)
  {
    threadpoll* sub_process=(pthread_t*)args;
    sub_process->run();
    return sub_process;
    
  }

  void run()
  {
    while(!m_stop)
    {
      m_sem.wait();
      m_lock.lock();
      if(m_workers.empty())
      {
        m_lock.lock();
        continue;
      }
      T* first=m_workers.front();
      m_workers.pop_front();
      m_lock.unlock();
      if(!first)continue;
      first.process();
      
    }
  }

private:
  int m_threadnum;
  int max_request;
  pthread_t* pool;
  std::list<T*>m_workers;
  Locker m_lock;
  SEM m_sem;
  bool m_stop;
}; 
