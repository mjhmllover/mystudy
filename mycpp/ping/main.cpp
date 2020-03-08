#include <stdio.h> 
#include <stdlib.h>
#include <string.h> 
#include <signal.h> 
#include <arpa/inet.h> 
#include <sys/types.h> 
#include <sys/socket.h> 
#include <sys/time.h>
#include <unistd.h> 
#include <netinet/in.h> 
#include <netinet/ip.h> 
#include <netinet/ip_icmp.h> 
#include <netdb.h> 
#include <setjmp.h> 
#include <errno.h> 

#define PACKET_SIZE 4096 
#define MAX_WAIT_TIME 5   //接收超时5秒
#define MAX_NO_PACKETS 3 

char sendpacket[PACKET_SIZE]; 
char recvpacket[PACKET_SIZE]; 
int sockfd,datalen=56; 
int nsend=0,nreceived=0; 
struct sockaddr_in dest_addr; //定义网络地址结构
pid_t pid; 
struct sockaddr_in from; 
struct timeval tvrecv; 

void statistics(int signo);  //统计
unsigned short cal_chksum(unsigned short *addr,int len);  //对icmp包进行校验
int pack(int pack_no);  //填充icmp报文
void send_packet(void);  //发送icmp包
void recv_packet(void);  //接收icmp包
int unpack(char *buf,int len);  //解析收到的icmp包
void tv_sub(struct timeval *out,struct timeval *in);  //计算时间差

void statistics(int signo) 
{ 
   printf("\n--------------------PING statistics-------------------\n"); 
   printf("%d packets transmitted, %d received , %%%.2f lost\n",nsend,nreceived,(float)(nsend-nreceived)/nsend*100); 
   close(sockfd); 
   exit(1); 
} 
/*校验和算法*/ 
unsigned short cal_chksum(unsigned short *addr,int len) 
{ 
   int nleft=len; 
   int sum=0; 
   unsigned short *w=addr; 
   unsigned short answer=0; 

   /*把ICMP报头二进制数据以2字节为单位累加起来*/ 
   while(nleft>1) 
   { 
     sum+=*w++; 
     nleft-=2; 
   } 
   /*若ICMP报头为奇数个字节，会剩下最后一字节。把最后一个字节视为一个2字节数据的高字节， 
   这个2字节数据的低字节为0，继续累加*/ 
   if( nleft==1) 
   { 
       *(unsigned char *)(&answer)=*(unsigned char *)w; 
       sum+=answer; 
   } 
   sum=(sum>>16)+(sum&0xffff); 
   sum+=(sum>>16); 
   answer=~sum; 
   return answer; 
} 
/*设置ICMP报头*/ 
int pack(int pack_no) 
{ 
    int i,packsize; 
    struct icmp *icmp; 
    struct timeval *tval; 
    icmp=(struct icmp*)sendpacket; 
    icmp->icmp_type=ICMP_ECHO; //ping时，icmp的协议类型是ICMP_ECHO
    icmp->icmp_code=0; 
    icmp->icmp_seq=pack_no;  //报文编号
    icmp->icmp_id=pid;  //发送报文的进程号
    packsize=8+datalen;   //8为icmp报文头的长度
    tval= (struct timeval *)icmp->icmp_data; //tval指向icmp报文的数据区
    gettimeofday(tval,NULL); /*记录发送时间*/ //在数据区填写报文的发送时间
    icmp->icmp_cksum=cal_chksum( (unsigned short *)icmp,packsize); /*校验算法*/ 
    return packsize; 
} 

/*发送1个ICMP报文*/ 
void send_packet() 
{ 
    int packetsize; 
    nsend++; 
    packetsize = pack(nsend); /*设置ICMP报头*/ 
    if(sendto(sockfd,sendpacket,packetsize,0,(struct sockaddr *)&dest_addr,sizeof(dest_addr))<0 ) 
    { 
        perror("sendto error"); 
        exit(-1);
    } 

    return;
} 

/*接收所有ICMP报文*/ 
void recv_packet() 
{ 
     int n,fromlen; 
     extern int errno; 
     signal(SIGALRM,statistics); 
     fromlen=sizeof(from); 
     while(1)
     { 
       alarm(MAX_WAIT_TIME); //设置闹钟5s
       if( (n=recvfrom(sockfd,recvpacket,sizeof(recvpacket),0,(struct sockaddr *)&from,(socklen_t *)&fromlen)) <0) 
       { 
           if(errno==EINTR)
           {
              perror("recvfrom error"); 
              continue; 
           }
       } 
       gettimeofday(&tvrecv,NULL);   /*记录接收时间*/ 
       if(unpack(recvpacket,n)==-1)   /*收到的包不是ICMP包，继续接收*/
       {
           continue; 
       }
       else                          /*收到回应包，结束循环*/
       {
           nreceived++; 
           break;
       }
    }

} 
/*剥去ICMP报头*/ 
int unpack(char *buf,int len) 
{ 
    int i,iphdrlen; 
    struct ip *ip; 
    struct icmp *icmp; 
    struct timeval *tvsend; 
    double rtt; 
    ip=(struct ip *)buf; 

    iphdrlen=ip->ip_hl<<2;              /*求ip报头长度,即ip报头的长度标志乘4*/ 
    icmp=(struct icmp *)(buf+iphdrlen); /*越过ip报头,指向ICMP报头*/ 
    len-=iphdrlen;                      /*ICMP报头及ICMP数据报的总长度*/ 

    if(len<8)                           /*小于ICMP报头长度则不合理*/ 
    { 
      printf("ICMP packets\'s length is less than 8\n"); 
      return -1; 
    } 

    /*确保所接收的是我所发的的ICMP的回应*/ 
    if( (icmp->icmp_type==ICMP_ECHOREPLY) && (icmp->icmp_id==pid) ) 
    { 
        tvsend=(struct timeval *)icmp->icmp_data; 
        tv_sub(&tvrecv,tvsend); /*接收和发送的时间差*/ 
        rtt=tvrecv.tv_sec*1000+tvrecv.tv_usec/1000.0; /*以毫秒为单位计算rtt*/ 
        /*显示相关信息*/ 
        printf("%d byte from %s: icmp_seq=%u ttl=%d rtt=%.3f ms\n", 
        len,inet_ntoa(from.sin_addr),icmp->icmp_seq,ip->ip_ttl,rtt); 
        return 0;
    } 
    else 
    {
        return -1; 

    }

}

int main(int argc,char *argv[]) 
{ 
    struct hostent *host; 
    struct protoent *protocol; 
    unsigned long inaddr=0l; 
    int waittime=MAX_WAIT_TIME;   //接收超时
    int size=50*1024; 

    if(argc<2)   //参数检查
    { 
       printf("usage:%s hostname/IP address\n",argv[0]); 
       exit(1); 
    } 

    if( (protocol=getprotobyname("icmp") )==NULL)   //获得icmp协议的属性，如果出错，说明不支持icmp协议
    { 
        perror("getprotobyname"); 
        exit(1); 
    } 
    /*生成使用ICMP的原始套接字,这种套接字只有root才能生成*/ 
    if( (sockfd=socket(AF_INET,SOCK_RAW,protocol->p_proto) )<0) 
    { 
        perror("socket error"); 
        exit(1); 
    } 
    /* 回收root权限,设置当前用户权限*/ 
    setuid(getuid()); //getuid获取当前进程的实际用户，setuid设置当前进程的有效用户

    /*扩大套接字接收缓冲区到50K这样做主要为了减小接收缓冲区溢出的 
    的可能性,若无意中ping一个广播地址或多播地址,将会引来大量应答*/ 
    setsockopt(sockfd,SOL_SOCKET,SO_RCVBUF,&size,sizeof(size) ); //修改套接字属性
    bzero(&dest_addr,sizeof(dest_addr)); 
    dest_addr.sin_family=AF_INET; //ping采用的是网络协议


    /*判断是主机名还是ip地址*/ 
    if((inaddr=inet_addr(argv[1]))==INADDR_NONE) 
    { 
        if((host=gethostbyname(argv[1]) )==NULL) /*是主机名*/ 
        { 
           perror("gethostbyname error"); 
           exit(1); 
        } 
        memcpy( (char *)&dest_addr.sin_addr.s_addr,host->h_addr,host->h_length); 
        printf("PING %s (%s): %d bytes data in ICMP packets.\n",host->h_name, 
            inet_ntoa(dest_addr.sin_addr),datalen); 
    } 
    else /*是ip地址*/ 
    {
        memcpy( (char *)&dest_addr.sin_addr.s_addr,(char *)&inaddr,sizeof(inaddr)); 

        printf("PING %s (%s): %d bytes data in ICMP packets.\n",argv[1], 
            inet_ntoa(dest_addr.sin_addr),datalen); 
    }
    /*获取main的进程id,用于设置ICMP的标志符*/ 
    pid=getpid(); 
    signal(SIGINT, statistics);//捕获SIGINT信号，交由statistics函数处理
    while(1)
    {
        send_packet(); /*发送所有ICMP报文*/ 
        recv_packet(); /*接收所有ICMP报文*/ 
        sleep(1);
    }

    return 0; 

} 
/*两个timeval结构相减*/ 
void tv_sub(struct timeval *out,struct timeval *in) 
{ 
     if( (out->tv_usec-=in->tv_usec)<0) 
     { 
         --out->tv_sec; 
         out->tv_usec+=1000000; 
     } 
     out->tv_sec-=in->tv_sec; 
} 
