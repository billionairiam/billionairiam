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
public:
    ~processpoll();
    void run();
    static processpoll<T>* create(int epfd,int process_num=8);
private:
    const int MAX_PROCESS=1024;
    const int MAX_EVENTS=1024;
    const int MAX_USERS=65536;
    int process_num;
    int listen_fd;
    static processpoll<T>* poll;
    
    void add_signal();
    void runchild();
    void runparent();
    int epfd;
    int m_idx;
    int user_num;
    processpoll(int listenfd,int process_num=8);
    process *m_process;
    void siginit();
    bool m_stop=false;
};
template<typename T>
processpoll<T>* processpoll<T>::poll=NULL;

template<typename T>
processpoll<T>::~processpoll()
{
    delete[] m_process;
}

static int setnonblock(int fd)
{
    int oldoperation=fcntl(fd,F_GETFL);
    int newoperation=oldoperation|O_NONBLOCK;
    fcntl(fd,F_SETFL,newoperation);
    return oldoperation;
}

static void sig_hanlder(int sig)
{
    int old=errno;
    int msg=sig;
    send(sigpipefd[1],(char *)&msg,1,0);
    errno=old;
}

static void addsig(int sig,void(*fp)(int),bool restart=true)
{
    struct sigaction act;
    memset(&act,0,sizeof(act));
    if(restart)
        act.sa_flags|=SA_RESTART;
    sigfillset(&act.sa_mask);
    assert(sigaction(sig,&act,NULL)!=-1);
}
static void addfd(int epfd,int fd)
{
    epoll_event event;
    event.data.fd=fd;
    event.events=EPOLLIN|EPOLLET;
    epoll_ctl(epfd,EPOLL_CTL_ADD,fd,&event);
    setnonblock(fd);
}

static void removefd(int epfd,int fd)
{
    epoll_ctl(epfd,EPOLL_CTL_DEL,fd,0);
    close(fd);
}

template<typename T>
void processpoll<T>::siginit()
{
    epfd=epoll_create(5);
    assert(epfd!=-1);
    int ret=socketpair(PF_UNIX,SOCK_STREAM,0,sigpipefd);
    assert(ret==0);
    
    addfd(epfd,listen_fd);
    addfd(epfd,sigpipefd[0]);

    addsig(SIGCHLD,sig_hanlder);
    addsig(SIGTERM,sig_hanlder);
    addsig(SIGINT,sig_hanlder);
    addsig(SIGPIPE,SIG_IGN);

    setnonblock(sigpipefd[1]);
}

template<typename T>
processpoll<T>::processpoll(int fd,int process_num):listen_fd(fd),m_idx(-1),process_num(process_num),m_stop(false)
{
    assert(process_num>0 && process_num<=MAX_PROCESS);
    m_process=new process[process_num];
    assert(m_process);

    for(int i=0;i<process_num;++i)
    {
        int ret=sockketpair(PF_UNIX,SOCK_STREAM,0,m_process[i].m_pipefd);
        assert(ret==0);
        m_process[i].m_pid=fork();
        assert(m_process[i].m_pid!=-1);
        if(m_process[i].m_pid==0)
        {
            m_idx=i;
            close(m_process[i].m_pipefd[0]);
            break;
        }else
        {
            close(m_pipefd[i].m_pipefd[1]);
            continue;
        }
    }
}

template<typename T>
processpoll<T>* processpoll<T>::create(int fd,int process_num=8)
{
    if(poll)return poll;
    poll=new processpoll(fd,process_num);
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
    epoll_event events[MAX_EVENTS];
    int newconn=0
    int sub_process=0;
    while(!m_stop)
    {
        int number=epoll_wait(epfd,events,MAX_EVENTS);
        if(number<0&&errno!=EINTR)
        {
            printf("EPOLL FILTURE!\n");
            break;
        }
        for(int i=0;i<number;++i)
        {
            int waitfd=events[i].data.fd;
            if((waitfd==listen_fd)&&(events[i].event*EPOLLIN))
            {
                int curprocess=sub_process;
                do
                {
                    if(m_process[curprocess].m_pid!=-1)
                        break;
                    curprocess=(curprocess+1)%MAX_PROCESS;
                } while (curprocess!=sub_process);
                if(m_process[curprocess].m_pid==-1)
                {
                    m_stop=true;
                    break;
                }
                sub_process=(curprocess+1)%MAX_PROCESS;
                
                send(m_process[PROCESS_TOWORK].m_pipefd[0],(char *)&newconn,sizeof(newconn),0);
                printf("send request to child%d\n",curprocess);
            }
            else if((waitfd==sigpipefd[0])&&(events[i].event&EPOLLIN))
            {
                char msgs[1024];
                int ret=recv(waitfd,msgs,sizeof(msgs),0);
                if(ret<=0)
                {
                    continue;
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
                                            printf("child %d join\n",i);
                                            close(m_process[i].m_pipefd[0]);
                                            m_process[i].m_pid=-1;
                                            break;
                                        }

                                    }
                                }
                                m_stop=true;
                                for(int i=0;i<process_num;++i)
                                {
                                    if(m_process[i].m_pid!=-1)
                                    {
                                        m_stop=false;
                                        break;
                                    }
                                }
                                break;
                            }
                            case SIGINT:
                            {
                                printf("kill all the child now\n");
                                for(int i=0;i<process_num;++i)
                                {
                                    int pid=m_process[i].m_pid;
                                    if(pid!=-1)
                                        kill(pid,SIGTERM);
                                }
                                m_stop=true;
                                break;
                            }
                        }
                    }
                }
            }
        }
    }
    close(epfd);
}

template<typename T>
void processpoll<T>::runchild()
{
    siginit();
    epoll_event events[MAX_EVENTS];
    T*users=new T[MAX_USERS];
    while(!m_stop)
    {
        int number=epoll_wait(epfd,events,MAX_EVENTS);
        if(number<0&&errno!=EINTR)
        {
            printf("EPOLL ERROR!\n");
            break;
        }
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
                if(connfd<0)
                {
                    printf("errno is: %d\n",errno);
                    continue;
                }
                addfd(epfd,connfd);
                users[connfd].init(epfd,connfd,clnt_addr);
                
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
                        case SIGCHLD:
                        {
                            pid_t pid;
                            int stat;
                            while((pid=waitpid(-1,&stat,WNOHANG))>0)
                            {
                                continue;
                            }
                            break;
                        }
                        case SIGTERM:
                        {

                        }
                        case SIGINT:
                        {
                            m_stop=true;
                            break;
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
    close[](users);
    users=NULL;
    close(m_process[m_idx].m_pipefd[1]);
}
#endif
