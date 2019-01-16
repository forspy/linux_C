//向链表中添加一个节点(利用尾指针的尾插法)
#include "teach_linuxc_list typedef.h"

void RegisterNode(P_list plist, P_Node pnode)
{
	if (isEmpty(plist) != 1)//如果链表里面有节点
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
//链表判空
bool IsEmpty(P_list plist)
{
	return (plist->prear == nullptr&&plist->pfront == nullptr);
}
//链表遍历
void TraverseList(P_list plist, void(*func)(NodeData data))
{
	P_Node ptempnode = plist->pfront;//使用一个ptempnode去遍历
	while (ptempnode)
	{
		func(ptempnode->data);
		ptempnode = ptempnode->p_next;
	}
}