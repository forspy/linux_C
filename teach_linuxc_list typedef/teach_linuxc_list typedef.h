#pragma once
//�������ҵ��ͷ�ļ�д��
//������ṹ
#define BUFFER_SIZE 1024
struct NodeData//�ṹ��������
{
	char send_buffer[BUFFER_SIZE];//���������Ĳ���
	int(*Init)(void);//����ָ��
	int(*SendFunction)(char* str, int lenght);//����ָ�룬����������ڵ��е��ú���������������ڵ�����ݣ������ڻص�����
};

typedef NodeData* P_NodeData;

struct Node//�ڵ����
{
	NodeData data;//������
	struct Node* p_next;//ָ����
};
typedef Node* P_Node;

struct List//�������
{
	P_Node pfront;//ͷָ��
	P_Node prear;//βָ��
	int length;//������
};
typedef List* P_list;

//��������ע��һ���ڵ�
void RegisterNode(P_list plist, P_Node pnode);
bool IsEmpty(P_list plist);//�����п�
//�������
void TraverseList(P_list plist, void(*func)(NodeData data));