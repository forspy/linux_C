#include "function_global.h"

int make_ans(
            unsigned char* buf,                 //��������
            unsigned short messageID,           //�ظ���messageID
            unsigned char  status,              //�ظ���״̬
            unsigned short recvMessageNum)      //�ظ����ǵڼ�����Ϣ
{
    unsigned short *tmp;
    tmp = (unsigned short*)buf;
    *tmp = htons(recvMessageNum);
    tmp = (unsigned short*)(&buf[2]);
    *tmp = htons(messageID);
    buf[4] = status;
    return 5;
}