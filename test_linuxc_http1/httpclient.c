#include <stdio.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <time.h>
#include <errno.h>
#include <signal.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/time.h>
#include <netinet/in.h>
#include <arpa/inet.h>
 
#define IPSTR "61.147.124.120"
#define PORT 80
#define BUFSIZE 1024
 
int main(int argc, char **argv)
{
        int sockfd, ret, i, h;
        struct sockaddr_in servaddr;
        char str1[4096], str2[4096], buf[BUFSIZE], *str;
        socklen_t len;
        fd_set   t_set1;
        struct timeval  tv;
		/*�������� �ͻ���*/
        if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0 ) {
                printf("������������ʧ��,���̼߳�����ֹ---socket error!\n");
                exit(0);
        };
 
        bzero(&servaddr, sizeof(servaddr));
        servaddr.sin_family = AF_INET;
        servaddr.sin_port = htons(PORT);
        if (inet_pton(AF_INET, IPSTR, &servaddr.sin_addr) <= 0 ){
                printf("������������ʧ��,���̼߳�����ֹ--inet_pton error!\n");
                exit(0);
        };
 
        if (connect(sockfd, (struct sockaddr *)&servaddr, sizeof(servaddr)) < 0){
                printf("���ӵ�������ʧ��,connect error!\n");
                exit(0);
        }
        printf("��Զ�˽���������\n");
 
        //��������
        memset(str2, 0, 4096);
        strcat(str2, "qqCode=474497857");
        str=(char *)malloc(128);
        len = strlen(str2);
        sprintf(str, "%d", len);
 
        memset(str1, 0, 4096);
        strcat(str1, "POST /webservices/qqOnlineWebService.asmx/qqCheckOnline HTTP/1.1\n");
        strcat(str1, "Host: www.webxml.com.cn\n");
        strcat(str1, "Content-Type: application/x-www-form-urlencoded\n");
        strcat(str1, "Content-Length: ");
        strcat(str1, str);
        strcat(str1, "\n\n");
 
        strcat(str1, str2);
        strcat(str1, "\r\n\r\n");
        printf("%s\n",str1);
 
        ret = write(sockfd,str1,strlen(str1));
        if (ret < 0) {
                printf("����ʧ�ܣ����������%d��������Ϣ��'%s'\n",errno, strerror(errno));
                exit(0);
        }else{
                printf("��Ϣ���ͳɹ�����������%d���ֽڣ�\n\n", ret);
        }
 
        FD_ZERO(&t_set1);
        FD_SET(sockfd, &t_set1);
 
        while(1){
                sleep(2);
                tv.tv_sec= 0;
                tv.tv_usec= 0;
                h= 0;
                printf("--------------->1\n");
                h= select(sockfd +1, &t_set1, NULL, NULL, &tv);
                printf("--------------->2\n");
 
                //if (h == 0) continue;
                if (h < 0) {
                        close(sockfd);
                        printf("�ڶ�ȡ���ݱ���ʱSELECT��⵽�쳣�����쳣�����߳���ֹ��\n");
                        return -1;
                };
 
                if (h > 0){
                        memset(buf, 0, 4096);
                        i= read(sockfd, buf, 4095);
                        if (i==0){
                                close(sockfd);
                                printf("��ȡ���ݱ���ʱ����Զ�˹رգ����߳���ֹ��\n");
                                return -1;
                        }
 
                        printf("%s\n", buf);
                }
        }
        close(sockfd);
 
 
        return 0;
}