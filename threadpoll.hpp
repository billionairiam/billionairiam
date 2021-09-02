#include<pthread.h>
#include<exception>
#include"locker.hpp"
#include<list>
template<typename T>
class threadpoll
{
public:
  
  threadpoll(int threadnum,int requestnum):m_threadnum(threadnum),m_requestnum(requestnum)
  {
    
    if(m_threadnum<0||MAX_WORKER<0)
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
      if(pthread_detach(pool[i))
      {
        delete[] pool;
        throw std::exception();
      }
    }
    
  }
  
  ~threadpoll()
  {
    delete[] poll;
  }
  void push(T* worker)
 {
   
   m_lock.lock();
   m_workers.push_back(worker);
   
 }
  
private:
  void worker(void *args)
  {
    threadpoll* sub_process=(pthread_t*)args;
    sub_process->run();
    return sub_process;
    
  }
  void run()
  {
    while(!m_stop)
    {
      
    }
  }
  
private:
  
  const int MAX_REQUEST;
  const int MAX_WORKER;
  int m_threadnum;
  int m_requestnum;
  pthread_t* poll;
  list<T*>m_workers;
  locker m_lock;
  sem m_sem;
  bool m_stop;
};
