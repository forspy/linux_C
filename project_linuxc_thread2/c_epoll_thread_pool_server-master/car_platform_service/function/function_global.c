#include "function_global.h"

int make_ans(
            unsigned char* buf,                 //传出数据
            unsigned short messageID,           //回复的messageID
            unsigned char  status,              //回复的状态
            unsigned short recvMessageNum)      //回复的是第几个消息
{
    unsigned short *tmp;
    tmp = (unsigned short*)buf;
    *tmp = htons(recvMessageNum);
    tmp = (unsigned short*)(&buf[2]);
    *tmp = htons(messageID);
    buf[4] = status;
    return 5;
}