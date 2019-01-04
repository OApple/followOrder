#pragma once
#include <sys/types.h>
#include <sys/time.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <linux/types.h>
#include <netinet/in.h>
#include <netinet/udp.h>
#include <netinet/ip.h>
#include <netpacket/packet.h>
#include <net/ethernet.h>
#include <arpa/inet.h>
#include <string.h>
#include <signal.h>
#include <iconv.h>
#include <iostream>
#include <net/if.h>
#include <stdio.h>
#include <sys/uio.h>
#include <fcntl.h>
#include <unistd.h>
#include <linux/filter.h>
#pragma once
#include <stdlib.h>
#define ETH_HDR_LEN        14
#define IP_HDR_LEN         20
#define UDP_HDR_LEN         8
#define TCP_HDR_LEN        20

static int m_sock;
using namespace std;
void PrintData (unsigned char* data , int Size)
{

    for(int i=0 ; i < Size ; i++)
    {
        if( i!=0 && i%16==0)   //if one line of hex printing is complete...
        {
            printf("         ");
            for(int j=i-16 ; j<i ; j++)
            {
                if(data[j]>=32 && data[j]<=128)
                    printf("%c",(unsigned char)data[j]); //if its a number or alphabet

                else printf("."); //otherwise print a dot
            }
            printf("\n");
        }

        if(i%16==0) printf("   ");
            printf(" %02X",(unsigned int)data[i]);

        if( i==Size-1)  //print the last spaces
        {
            for(int j=0;j<15-i%16;j++) printf("   "); //extra spaces

            printf("         ");

            for(int j=i-i%16 ; j<=i ; j++)
            {
                if(data[j]>=32 && data[j]<=128) printf("%c",(unsigned char)data[j]);
                else printf(".");
            }
            printf("\n");
        }
    }
}
void sig_handler(int sig){
    struct ifreq ethreq;
    if (sig == SIGTERM)
    {
          printf("SIGTERM recieved, exiting...n");
    }
    else if (sig == SIGINT)
    {
          printf("SIGINT recieved, exiting...n");
    }
    else if (sig == SIGQUIT)
    {
          printf("SIGQUIT recieved, exiting...n");
    }

    // turn off the PROMISCOUS mode
    strncpy(ethreq.ifr_name, "eth1", IFNAMSIZ);
    if (ioctl(m_sock, SIOCGIFFLAGS, &ethreq) != -1)
    {
          ethreq.ifr_flags &= ~IFF_PROMISC;
          ioctl(m_sock, SIOCSIFFLAGS, &ethreq);
    }

    close(m_sock);
    exit(0);
}
typedef struct {
 unsigned char Version;
 unsigned char Type;
 char UnEncLen;
 unsigned short SequenceSeries;
 unsigned int SequenceNumber;
 unsigned int unknown;
 unsigned short field;
 unsigned short TDECCounter;
 unsigned int RequestID;
} datasend;
typedef struct {
 uint8_t version;
 uint8_t type;
 uint8_t len;
 uint16_t number;
 char data[255];
} datasend2;
static const char hex_chars[] = "0123456789ABCDEF";
//int main(int argc, char ** argv){
int netTest(){
    int n;
    unsigned char buf[2048]="";
    unsigned char *ethhead;
    unsigned char *iphead;
    struct ifreq ethreq;
    struct sigaction sighandle;
#if 0
    $tcpdump ip -s 2048 -d host 192.168.1.2
    (000) ldh      [12]
    (001) jeq      #0x800           jt 2 jf 7
    (002) ld       [26]
    (003) jeq      #0xc0a8019c      jt 6 jf 4
    (004) ld       [30]
    (005) jeq      #0xc0a8019c      jt 6 jf 7
    (006) ret      #2048
    (007) ret      #0
#endif

    /*
    struct sock_filter  // Filter block
海姹网（网址:http://www.seacha.com），标签：Linux C++自己动手写网络抓包工具， Socket,抓包,tcpdump
    {
        __u16 code;     // Actual filter code
        __u8 jt;        // Jump true
        __u8 jf;        // Jump false
        __u32 k;        // Generic multiuse field
    };
    code对应命令代码；
    jt是jump if true后面的操作数，注意这里用的是相对行偏移，如2就表示向前跳转2行，而不像伪代码中使用绝对行号；
    jf为jump if false后面的操作数；
    k对应伪代码中第3列的操作数。
    */
    struct sock_filter bpf_code[] =
    {
        { 0x28, 0, 0, 0x0000000c },
        { 0x15, 0, 5, 0x00000800 },
        { 0x20, 0, 0, 0x0000001a },
        //0xc0a8019c 192.168.1.156 即IP地址的16进制形式
        { 0x15, 2, 0, 0xc0a8b189 },
        { 0x20, 0, 0, 0x0000001e },
        //0xc0a8019c 192.168.1.156 即IP地址的16进制形式
        { 0x15, 0, 1, 0xc0a8b189 },
        { 0x6,  0, 0, 0x00000800 },
        { 0x6,  0, 0, 0x00000000 }
    };

    struct sock_fprog filter;
    filter.len = sizeof(bpf_code) / sizeof(bpf_code[0]);
    filter.filter = bpf_code;

    sighandle.sa_flags = 0;
    sighandle.sa_handler = sig_handler;
    sigemptyset(&sighandle.sa_mask);
    sigaddset(&sighandle.sa_mask, SIGTERM);
    sigaddset(&sighandle.sa_mask, SIGINT);
    sigaddset(&sighandle.sa_mask, SIGQUIT);
    sigaction(SIGTERM, &sighandle, NULL);
    sigaction(SIGINT, &sighandle, NULL);
    sigaction(SIGQUIT, &sighandle, NULL);

    // AF_PACKET allows application to read pecket from and write packet to network device
    // SOCK_DGRAM the packet exclude ethernet header
    // SOCK_RAW raw data from the device including ethernet header
    // ETH_P_IP all IP packets
    if ((m_sock = socket(AF_PACKET, SOCK_RAW, htons(ETH_P_IP))) < 0)
    {
          perror("socket");
          return 1;
    }

    // set NIC to promiscous mode, so we can recieve all packets of the network
    strncpy(ethreq.ifr_name, "eth1", IFNAMSIZ);
    if(ioctl(m_sock, SIOCGIFFLAGS, &ethreq) == -1){
          perror("ioctl");
          close(m_sock);
          return 1;
    }
    /*
    ethreq.ifr_flags |= IFF_PROMISC;
    //ethreq.ifr_flags &= ~IFF_PROMISC;
    if(ioctl(m_sock, SIOCSIFFLAGS, &ethreq) == -1)
    {
          perror("ioctl");
          close(m_sock);
          return 1;
    }
    */
    // attach the bpf filter
    /*
    if(setsockopt(m_sock, SOL_SOCKET, SO_ATTACH_FILTER, &filter, sizeof(filter)) == -1)
    {
          perror("setsockopt");
          close(m_sock);
          return 1;
    }*/
    //datasend msg_recv;
    //memset(&msg_recv,0,sizeof(datasend));
    while(1)
    {
        n = recvfrom(m_sock, buf, sizeof(buf), 0, NULL, NULL);
        //n = recvfrom(m_sock, &msg_recv, sizeof(datasend), 0, NULL, NULL);
        if(n < (ETH_HDR_LEN + IP_HDR_LEN + UDP_HDR_LEN))
        {
            printf("invalid packetn");
            continue;
        }/*
        printf("version:%3d\n",msg_recv.version);
        printf("type:%d\n",msg_recv.type);
        printf("len:%d\n",msg_recv.len);
        printf("number:%d\n",msg_recv.number);
        printf("data:%s\n",msg_recv.data);*/

        printf("%d bytes recieved\n", n);
        ethhead = (unsigned char *)buf;
        struct ethhdr *eth = (struct ethhdr *)(buf);

        //////IP header
        unsigned short iphdrlen;
        struct iphdr *ip = (struct iphdr*)(buf + sizeof(struct ethhdr));
        // Source and destination addresses: IP and port
        struct sockaddr_in source, dest;
        memset(&source, 0, sizeof(source));
        source.sin_addr.s_addr = ip->saddr;
        memset(&dest, 0, sizeof(dest));
        dest.sin_addr.s_addr = ip->daddr;
        //struct iphdr *ip = (struct iphdr *)( buffer + sizeof(struct ethhdr) );
        /* getting actual size of IP header*/
        iphdrlen = ip->ihl*4;
        /* getting pointer to udp header*/
        struct udphdr *udp=(struct udphdr*)(buf + iphdrlen + sizeof(struct ethhdr));
        //data type

        //Extracting data
        unsigned char * data = (buf + iphdrlen + sizeof(struct ethhdr) + sizeof(struct udphdr));
        int remaining_data = n - (iphdrlen + sizeof(struct ethhdr) + sizeof(struct udphdr));
        unsigned int c = 0;
        printf("iphdrlen %d bytes,ethhdr %d bytes,udphdr %d bytes,receive %d bytes", iphdrlen,sizeof(struct ethhdr),sizeof(struct udphdr),n);
        if(remaining_data<=0){
            cout<<"no data"<<endl;
            continue;
        }
        //char d[4096]="\0";
        //memcpy(d,data,remaining_data);
        //PrintData(data,remaining_data);
        //continue;
        printf("Ethernet Header\n");
        printf("\t|-Source Address : %.2X-%.2X-%.2X-%.2X-%.2X-%.2X\n",eth->h_source[0],eth->h_source[1],eth->h_source[2],eth->h_source[3],eth->h_source[4],eth->h_source[5]);
        printf("\t|-Destination Address : %.2X-%.2X-%.2X-%.2X-%.2X-%.2X\n",eth->h_dest[0],eth->h_dest[1],eth->h_dest[2],eth->h_dest[3],eth->h_dest[4],eth->h_dest[5]);
        printf("\t|-Protocol : %d\n",eth->h_proto);
        printf("\t|-Version : %d\n",(unsigned int)ip->version);

        printf("\t|-Internet Header Length : %d DWORDS or %d Bytes\n",(unsigned int)ip->ihl,((unsigned int)(ip->ihl))*4);

        printf("\t|-Type Of Service : %d\n",(unsigned int)ip->tos);

        printf("\t|-Total Length : %d Bytes\n",ntohs(ip->tot_len));

        printf("\t|-Identification : %d\n",ntohs(ip->id));

        printf("\t|-Time To Live : %d\n",(unsigned int)ip->ttl);

        printf("\t|-Protocol : %d\n",(unsigned int)ip->protocol);

        printf("\t|-Header Checksum : %d\n",ntohs(ip->check));

        printf("\t|-Source IP : %s\n", inet_ntoa(source.sin_addr));

        printf("\t|-Destination IP : %s\n",inet_ntoa(dest.sin_addr));
        printf("\t|-Source Port : %d\n" , ntohs(udp->source));
        printf("\t|-Destination Port : %d\n" , ntohs(udp->dest));
        printf("\t|-UDP Length : %d\n", ntohs(udp->len));
        printf("\t|-UDP Checksum : %d\n", ntohs(udp->check));
        /*
        string strSha1;
        for(int i=0;i<remaining_data;i++)
        {
            if(i!=0 && i%16==0)
                printf("\n");
            printf(" %.2X ",data[i]);
        }

        char errMsg[1024];
        //codeCC->convert((char*)data,remaining_data,errMsg,1024);
        //cerr << "--->>>ErrorMsg=" << errMsg << endl;
        cout<<endl;
        cout<<strSha1<<endl;*/
        /*
         char str[remaining_data];
         char dst[remaining_data];
        for(int i = 0; i<remaining_data;i++)
        {
            str[2*i] = data[i]>>4;
            str[2*i+1] = data[i]&0xf;
        }
            for(int i = 0; i<remaining_data;i++)
        {
            sprintf(&dst[i],"%X/n",str[i]);
        }
        for(int i = 0; i<remaining_data;i++)
        {
             printf("%c\n",dst[i]);
        }*/
        /*
        printf("Ethernet: MAC[%02X:%02X:%02X:%02X:%02X:%02X]",
            ethhead[0], ethhead[1], ethhead[2],
            ethhead[3], ethhead[4], ethhead[5]);
        printf("->[%02X:%02X:%02X:%02X:%02X:%02X]",
            ethhead[6], ethhead[7], ethhead[8],
            ethhead[9], ethhead[10], ethhead[11]);
        printf(" type[%04x]\n", (ntohs(ethhead[12]|ethhead[13]<<8)));

        iphead = ethhead + ETH_HDR_LEN;
        // header length as 32-bit
        printf("IP: Version: %d HeaderLen: %d[%d]", (*iphead>>4), (*iphead & 0x0f), (*iphead & 0x0f)*4);
        printf(" TotalLen %d", (iphead[2]<<8|iphead[3]));
        printf(" IP [%d.%d.%d.%d]", iphead[12], iphead[13], iphead[14], iphead[15]);
        printf("->[%d.%d.%d.%d]", iphead[16], iphead[17], iphead[18], iphead[19]);
        printf(" %d", iphead[9]);

        if (iphead[9] == IPPROTO_TCP)
        {
            printf("[TCP]");
        }
        else if (iphead[9] == IPPROTO_UDP)
        {
            printf("[UDP]");
        }
        else if (iphead[9] == IPPROTO_ICMP)
        {
            printf("[ICMP]");
        }
        else if (iphead[9] == IPPROTO_IGMP)
        {
            printf("[IGMP]");
        }
        else if (iphead[9] == IPPROTO_IGMP)
        {
            printf("[IGMP]");
        }
        else
        {
            printf("[OTHERS]");
        }

        printf(" PORT [%d]->[%d]\n", (iphead[20] << 8 | iphead[21]), (iphead[22] << 8 | iphead[23]));
        */
    }

    close(m_sock);

    return 0;
}

