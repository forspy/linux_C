#pragma once
//链表的商业化头文件写法
//数据域结构
#define BUFFER_SIZE 1024
struct NodeData//结构体数据域
{
	char send_buffer[BUFFER_SIZE];//命令所带的参数
	int(*Init)(void);//函数指针
	int(*SendFunction)(char* str, int lenght);//函数指针，用于在链表节点中调用函数，数据是这个节点的数据，类似于回调函数
};

typedef NodeData* P_NodeData;

struct Node//节点对象
{
	NodeData data;//数据域
	struct Node* p_next;//指针域
};
typedef Node* P_Node;

struct List//链表对象
{
	P_Node pfront;//头指针
	P_Node prear;//尾指针
	int length;//链表长度
};
typedef List* P_list;

//向链表中注册一个节点
void RegisterNode(P_list plist, P_Node pnode);
bool IsEmpty(P_list plist);//链表判空
//链表遍历
void TraverseList(P_list plist, void(*func)(NodeData data));