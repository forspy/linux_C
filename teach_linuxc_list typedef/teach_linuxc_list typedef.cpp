//�����������һ���ڵ�(����βָ���β�巨)
#include "teach_linuxc_list typedef.h"

void RegisterNode(P_list plist, P_Node pnode)
{
	if (isEmpty(plist) != 1)//������������нڵ�
	{
		plist->prear->p_next = pnode;
	}
	else
	{
		plist->pfront = pnode;
	}
	plist->prear = pnode;
	plist->length++;
}
//�����п�
bool IsEmpty(P_list plist)
{
	return (plist->prear == nullptr&&plist->pfront == nullptr);
}
//�������
void TraverseList(P_list plist, void(*func)(NodeData data))
{
	P_Node ptempnode = plist->pfront;//ʹ��һ��ptempnodeȥ����
	while (ptempnode)
	{
		func(ptempnode->data);
		ptempnode = ptempnode->p_next;
	}
}