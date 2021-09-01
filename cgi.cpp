#include <unistd.h>
#include"processpoll.hpp"
#include<sys/socket.h>
#include<netinet/in.h>
class conn_cgi{
public:
    conn_cgi();
    ~conn_cgi();
    void init(int epfd,int connfd,,const sockaddr_in&addr)
    {
        epfd=epfd;
        connfd==connfd;
        clnt_addr=addr;
        recv_idx=0;
    }
    void process()
    {
        int ret=-1;
        while (1)
        {
            ret=recv(connfd,buf+recv_idx,BUF_SIZE-1-recv_idx,0);
            if((ret<0)&&(errno!=EAGAIN))
            {
                removefd(efpd,connfd);
                break;
            }
            else if(ret==0)
            {
                removefd(efpd,connfd);
                break;
            }
            else
            {
                
                int i=recv_idx;
                recv_idx+=ret;
                for(;i<recv_idx;++i)
                {
                    if((i>=1)&&(buf[i-1]=='\r')&&(buf[i]=='\n'))
                    {
                        break;
                    }
                }
                if(i==recv_idx)
                {
                    continue;
                }
                buf[i-1]='\0';
                char*file_name=buf;
                if(access(file_name,F_OK)==-1)
                {
                    removefd(epfd,connfd);
                    break;
                }
                pid_t pid=fork();
                if(pid<0)
                {
                    removefd(epfd,connfd);
                    continue;
                }else if(pid>0)
                {
                    removefd(epfd,connfd);
                    break;
                }
                else
                {
                    close(STDOUT_FILENO);
                    dup(connfd);
                    execl(buf,buf,0);
                    exit(0);
                }
                
                
            }
            
            
        }
        
    }
    
private:
    static const  int BUF_SIZE=1024;
    char buf[BUF_SIZE];
    static int efpd;
    int connfd;
    sockaddr_in clnt_addr;
    int recv_idx;
};

int conn_cgi::epfd=-1;

int main(int argc,char*argv[])
{
    if(argc!=2)
    {
        printf("usage error!\n");
        return 1;
    }
    struct sockaddr_in addr;
    memset(&addr,0,sizeof(addr));
    addr.sin_addr.s_addr=htonl(INADDR_ANY);
    addr.sin_family=AF_INET;
    addr.sin_port=htons(atoi(argv[1]));

    int sock=socket(PF_INET,SOCK_STREAM,0);
    assert(sock1=-1);

    int ret=bind(sock,(sockaddr*)&addr,sizeof(addr));
    assert(ret!=-1);

    ret=listen(sock,5);
    assert(ret!=-1);
    processpoll<conn_cgi>* propoll=processpoll<conn_cgi>::create(sock);
    if(propoll)
    {
        propoll->run();
        delete propoll;
    }
    close(sock);
    return 0;

    
}
