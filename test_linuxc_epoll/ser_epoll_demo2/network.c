#include "network.h"


ssize_t readn(int fd, void *usrbuf, size_t n)
{
    size_t nleft = n; //��ʾ����Ҫ��ȡ���ֽ���
    ssize_t nread;
    char *bufp = usrbuf; //����read������ŵ�λ��

    while(nleft > 0)
    {
        if((nread = read(fd, bufp, nleft)) == -1)
        {
            if(errno == EINTR)  //interupt
                nread = 0;  //continue;  �ж���Ҫ�ٴζ�ȡ
            else
                return -1;  // ERROR
        }else if(nread == 0)  // EOF
            break;
        
        nleft -= nread;
        bufp += nread;
    }
    return (n - nleft);
}

ssize_t writen(int fd, void *usrbuf, size_t n)
{
    size_t nleft = n;
    ssize_t nwrite;

    char *bufp = usrbuf;

    while(nleft > 0)
    {
        //nwrite == 0Ҳ���ڴ���
        if((nwrite = write(fd, bufp, nleft)) <= 0)
        {
            if(errno == EINTR)
                nwrite = 0;
            else
                return -1; // -1 �� 0
        }

        nleft -= nwrite;
        bufp += nwrite;
    }
    return n;  //���ﲻ�� n- nleft ������n
}

//recv_peekѡ�����һ����ȷ�Ķ�ȡ���̡�
ssize_t recv_peek(int sockfd, void *buf, size_t len) {
    int nread;
    while (1) {
        //�������ֻ�ɹ�����һ��
        nread = recv(sockfd, buf, len, MSG_PEEK);
        if (nread < 0 && errno == EINTR) {  //���ж��������ȡ
            continue;
        }
        if (nread < 0) {
            return -1;
        }
        break;
    }
    return nread;
}


ssize_t readline(int sockfd, void *buf, size_t maxline) {
    int nread;  //һ��IO��ȡ������
    int nleft;  //��ʣ����ֽ���
    char *ptr;  //������ݵ�ָ���λ��
    int ret;    //readn�ķ���ֵ
    int total = 0;  //Ŀǰ�ܹ���ȡ���ֽ���

    nleft = maxline-1;
    ptr = buf;

    while (nleft > 0) {
        //��һ�ε��ý�����Ԥ������
        //��û����İ����ݴӻ�������ȡ��
        ret = recv_peek(sockfd, ptr, nleft);
        //ע�������ȡ���ֽڲ��������Բ��Ǵ��󣡣���
        if (ret <= 0) {
            return ret;
        }

        nread = ret;
        int i;
        for (i = 0; i < nread; ++i) {
            if (ptr[i] == '\n') {
                //������������Ķ�ȡ����
                ret = readn(sockfd, ptr, i + 1);
                if (ret != i + 1) {
                    return -1;
                }
                total += ret;
                ptr += ret;
                *ptr = 0;
                return total;   //���ش��еĳ��� '\n'����������
            }
        }
        //���û�з���\n����Щ����Ӧȫ������
        ret = readn(sockfd, ptr, nread);
        if (ret != nread) {
            return -1;
        }
        nleft -= nread;
        total += nread;
        ptr += nread;
    }
    *ptr = 0;
    return maxline-1;
}


