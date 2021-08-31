#ifndef _PROCESSPOLL_HPP
#define _PROCESSPOLL_HPP

#include<stdio.h>
#include<stdlib.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<arpa/inet.h>
#include<assert.h>
#include<stdio.h>
#include<unistd.h>
#include<errno.h>
#include<string.h>
#include<fcntl.h>
#include<sys/epoll.h>
#include<signal.h>
#include<sys/wait.h>
#include<sys/stat.h>


class process{
public:
    process():m_pid(-1){};
public:
    pid_t m_pid;
    int m_pipefd[2];
};

static int sigpipefd[2];

template<typename T>
class processpoll{
    void run();
private:
    const int MAX_PROCESS=1024;
    const int MAX_EVENTS=1024;
    const int MAX_USERS=1024;
    int process_num;
    int listen_fd;
    static processpoll<T>* poll;
    static processpoll<T>* create(int epfd);
    void add_signal();
    void runchild();
    void runparent();
    int epfd;
    int m_idx;
    int user_num;
    processpoll(int listenfd,int epfd);
    process *m_process;
    void siginit();
    bool m_stop=false;
    epoll_event*events;
    int PROCESS_TOWORK=0;
};
processpoll<T>* processpoll<T>::poll=NULL;


int setnonblock(int fd)
{
    int oldoperation=fcntl(fd,F_GETFL);
    int newoperation=oldoperation|O_NONBLOCK;
    fcntl(fd,F_SETFL,newoperation);
    return oldoperation;
}

void sig_hanlder(int sig)
{
    int old=errno;
    send(sigpipefd[1],(char *)&sig,sizeof(sig),0);
    errno=old;
}

void addsig(int sig,void(*fp)(int),bool restart=true)
{
    struct sigaction act;
    memset(&act,0,sizeof(act));
    if(restart)
        act.sa_flag|=SIG_RESTART;
    sigfillset(&act.sa_mask);
    assert(sigaction(sig,&act,NULL)!=-1);
}
void addfd(int epfd,int fd)
{
    epoll_event event;
    event.data.fd=fd;
    event.events|=EPOLLIN;
    epoll_ctl(epfd,EPOLL_CTL_ADD,&event);
    setnonblock(fd,);
}

template<typename T>
void processpoll<T>::siginit()
{
    epfd=epoll_create(5);
    addfd(epfd,listen_fd);
    addfd(epfd,sigpipefd[0]);
    addsig(SIGCHILD,sig_hanlder);
    asssig(SIGTERM,sig_hanlder);
    asssig(SIGPIPE,SIG_IGN);
    setnonblock(sigpipefd[1]);
}

template<typename T>
processpoll<T>::processpoll(int fd,int process_num):listenfd(fd),m_idx(-1),process_num(process_num),m_stop(false)
{
    assert(process_num>0 && process_num<=MAX_PROCESS);
    m_process=new process[process_num];
    assert(m_process>=0);

    for(int i=0;i<process_num;++i)
    {

        m_process[i].m_pid=fork();
        assert(m_process[i].)
        sockketpair(PF_UNIX,SOCK_STREAM,0,m_process[i].m_pipefd);
        addfd()
        if(m_process[i].m_pid==0)
        {
            m_idx=i;
            close(m_process[i].m_pipefd[0]);
        }else
        {
            close(m_pipefd[i].m_pipefd[1]);
            continue;
        }
    }
}

template<typename T>
processpoll<T>* processpoll<T>::create(int fd)
{
    if(poll)return poll;
    poll=new processpoll(fd,5);
    return poll;
}

template<typename T>
void processpoll<T>::run()
{
    if(m_idx==-1)
    {
        runparent();
    }else
    {
        runchild();
    }
}

template<typename T>
void processpoll<T>::runparent()
{
    siginit();
    while(!m_stop)
    {
        int number=epoll_wait(epfd,events,MAX_EVENTS);
        for(int i=0;i<number;++i)
        {
            int waitfd=events[i].data.fd;
            if((waitfd==listen_fd)&&(events[i].event*EPOLLIN))
            {
                int curprocess=PROCESS_TOWORK;
                int flag=false;
                while(m_process[curprocess].m_pid==-1)
                {
                    flag=true;
                    curprocess=(curprocess+1)%process_num;
                    if(curprocess==PROCESS_TOWORK)
                        break;
                }
                if(flag&&curprocess==PROCESS_TOWORK)
                    return;
                PROCESS_TOWORK=curprocess;
                int sig=0
                send(m_process[PROCESS_TOWORK].m_pipefd[0],(char *)&sig,sizeof(sig),0);
            }
            else if((waitfd==sigpipefd[0])&&(events[i].event&EPOLLIN))
            {
                char msgs[1024];
                int ret=recv(waitfd,msgs,sizeof(msgs),0);
                if(ret<=0&&errno!=EAGAIN)
                {
                    m_stop=true;
                }else
                {
                    for(int i=0;i<ret;++i)
                    {
                        switch(msgs[i])
                        {
                            case SIG_TERM:
                            {
                            }
                            case SIGCHILD:
                            {
                                pid_t pid;
                                int state;
                                if((pid=waitpid(-1,&stat,WNOHANG))!=0)
                                {
                                    for(int i=0;i<process_num;++i)
                                    {
                                        if(m_process[i].m_pid==pid)
                                        {
                                            m_process[i].m_pid=-1;
                                            break;
                                        }

                                    }
                                }
                            }
                            case SIGINT:
                            {
                                for(int i=0;i<process_num;++i)
                                {
                                    m_process[i].m_pid=-1;
                                }
                                m_stop=true;
                            }
                        }
                    }
                }
            }
        }
    }
}

template<typename T>
void runchild()
{
    siginit();
    T*users=new T[MAX_USERS];
    while(!m_stop)
    {
        int number=epoll_wait(epfd,events,MAX_EVENTS);
        for(int i=0;i<number;++i)
        {
            int fd=events[i].data.fd;
            if(fd==m_process[m_idx].m_pipefd[1])
            {
                int client
                int ret=recv(fd,(char *)&client,sizeof(client),0);
                if(ret<=0&&errno!=EAGAIN)
                {
                    continue;
                }
                struct sockaddr_in clnt_addr;
                sockaddr_len adr_sz;
                int connfd=accept(listen_fd,(sockaddr*)&clnt_addr,&adr_sz);
                addfd(epfd,connfd);
                assert(user_num<MAX_USERS-1);
                users[user_num].init(connfd);
                user_num++;
            }
            else if((fd==sigpipefd[1])&&(events[i].events&EPOLLIN))
            {
                char msg[1024];
                int ret=recv(sigpipefd[1],msg,sizeof(msg),0);
                if(ret<=0&&errno!=EAGAIN)
                    continue;
                for(int i=0;i<ret;++i)
                {
                    switch(msg[i])
                    {
                        case SIGCHILD:
                        {

                        }
                        case SIGTERM:
                        {

                        }
                        case SIGINT:
                        {

                        }
                    }
                }
            }
            else if(events[i].event&EPOLLIN)
            {
                users[fd].process();
            }
        }
    }
}
#endif
