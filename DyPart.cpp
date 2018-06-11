#include "stdafx.h"
#include <iostream>
#include <vector>
#include <string>
#include <iomanip>
#include <map>
using namespace std;

static int SIZE=1;//空闲分区默认大小为 1

//分区
typedef struct Partition{
	int pno;//分区ID
	int capacity;//分区大小 
	bool isAllocate;//分配状态( true:已分配；false：未分配 )
	long addr;//分区地址 
}Partition;

//分区节点 
typedef struct PartNode{
	Partition partition;//分区节点
	struct PartNode *qian;//前继
	struct PartNode *hou;//后继 
}PartNode,*PartLinkList; 

//进程/作业 
typedef struct Process
{
	int pno;//进程名
	int need_capacity;//进程大小
	int partition;//进程所在分区编号
	PartNode *part;//进程所在分区地址
}Process;

//进程集
static map<int,Process> proMap;

string status(bool isAllocation);
void initPartition(PartLinkList &par,int size);
void showInfo(PartLinkList p);
void showInfo(map<int,Process> pros);
PartNode *getHead(PartLinkList pList);
PartNode *getLast(PartLinkList pList);
PartNode *getNewPart(int pno,int capacity);
vector<Partition> BubbleSort( vector<Partition> &parts );
string status(bool isAllocation);
void firstFit(PartLinkList &part,vector<Process> pros);
void bestFit(PartLinkList &part,vector<Process> pros);
void nextFit(PartLinkList &part,Process pro);
void worstFit(PartLinkList &part,vector<Process> pros);
int reqAlloc( PartLinkList &part, int ch );
vector<Partition> getFreePart(PartLinkList partList);
PartNode *getFreeNode( PartLinkList partList,PartNode p  );
PartNode *getFreeNode( PartLinkList partList,Partition p  );
void RecycleAlg( PartLinkList &partList,int partno  );


//初始化分区
void initPartition(PartLinkList &par,int size)
{
	cout<<"----------------------------初始化分区资源----------------------------"<<endl;
	par = new PartNode;//头节点
	par->qian = NULL;
	par->hou = NULL;
	PartNode *temp = par;
	for(int i=0;i<size;i++)
	{
		cout<<endl;
		PartNode *p = new PartNode;
		//cout<<"请输入该分区编号: ";
		//cin>>p->partition.pno;
		p->partition.pno = i+1; 
		cout<<"当前分区："<<p->partition.pno<<endl;
		cout<<"请输入该分区空间大小: ";
		cin>>p->partition.capacity;
		p->partition.isAllocate = false;
		p->hou=NULL;
		p->qian = temp->hou;
		temp->hou = p;
		temp = p;
	}
	cout<<endl;
}

//申请分配空闲分区
int reqAlloc( PartLinkList &part, int ch ){
	vector<Process> pros;
	cout<<"结束请按 y ！"<<endl;
	for(int i=0;;i++)
	{
		int pno;
		cout<<"请输入作业编号(整数)：";
		cin>>pno;
		int  request;
		cout<<"请输入作业所需空间大小(单位:KB)：" ;
		cin>>request;
		cout<<endl;
		//申请分区
		if (request<0 || request == 0)
		{
			cout << "分配大小不合适，请重试！" << endl;
			continue;
		}else{

			Process p;
			p.pno = pno;
			p.need_capacity = request;
			pros.push_back(p);
			cout<<"结束？ y/n ";
			string s;
			cin>>s;
			if(s=="y")
			{
				break;	
			}
		}
		
	}
	if(pros.size()>0){
		//选择分配算法
		switch(ch){
			//首次适应算法
		case 1:
			firstFit(part,pros);
			break;
			//循环首次适应算法
		case 2:
			nextFit(part,pros);
			break;
			//最佳适应算法
		case 3:
			bestFit(part,pros);
			break;
			//最坏适应算法
		case 4:
			worstFit(part,pros);
			break;
		default:break; 
		} 
		return 1;
	}
	return 0;

}
//首次适应算法(First Fit) 
void firstFit(PartLinkList &part,vector<Process> pros){
	PartLinkList first = getHead(part);
	int len = pros.size();
	for( int i=0;i<len;i++ )
	{ 
		Process pro = pros.at(i);
		PartNode *pnode = first;
		while(pnode)
		{
			int node_cap = pnode->partition.capacity;
			int need_cap = pro.need_capacity;
			bool isAlloc = false;
			if( need_cap<= node_cap && pnode->partition.isAllocate==false )
			{
				cout<<"当前符合条件节点编号: "<<pnode->partition.pno<<" 空间大小："<<node_cap<<" 申请空间："<<need_cap<<endl;
				//进程存入进程集合
				pro.partition = pnode->partition.pno;
				pro.part = pnode;
				proMap.insert(pair<int,Process>(pro.pno,pro));
				//空闲分区分配空间
				int surplus = pnode->partition.capacity - need_cap;
				pnode->partition.capacity=need_cap;
				pnode->partition.isAllocate = true;
				if( surplus!=0 )
				{
					//新建空闲分区
					SIZE++;
					PartNode *newPart = getNewPart(SIZE,surplus);
					PartNode *pnext = pnode->hou;
					if(pnext!=NULL)
					{
						pnext->qian = newPart;	
						newPart->hou = pnext;
					}else
					{
						newPart->hou = NULL;
					}
					pnode->hou = newPart;
					newPart->qian = pnode;				
				}
				isAlloc = true;
				break;
			}
			pnode = pnode->hou;
			if( pnode==NULL && !isAlloc )
			{
				cout<<"没有可分配空闲分区！未执行分配请求！"<<endl;

			}
		}
	}
	showInfo(part);
	showInfo(proMap);

} 

//循环首次适应算法(Next Fit)
void nextFit(PartLinkList &part,vector<Process> pros){
	PartLinkList first = getHead(part);
	int len = pros.size();
	for( int i=0;i<len;i++ )
	{ 
		Process pro = pros.at(i);
		//检索上次作业分配所在分区
		map<int,Process> ::iterator it = --proMap.end();
		int prePartNo = it->second.partition;
		PartNode *previousPart = it->second.part;//以上次分配的分区点作为分界点
		PartNode *pnode = it->second.part;
		pnode = pnode->hou;
		while( pnode )
		{
			int node_cap = pnode->partition.capacity;
			int need_cap = pro.need_capacity;
			bool isAlloc = false;
			if( need_cap<= node_cap && pnode->partition.isAllocate==false )
			{
				cout<<"当前符合条件节点编号: "<<pnode->partition.pno<<" 空间大小："<<node_cap<<" 申请空间："<<need_cap<<endl;
				//进程存入进程集合
				pro.partition = pnode->partition.pno;
				pro.part = pnode;
				proMap.insert(pair<int,Process>(pro.pno,pro));
				//空闲分区分配空间
				int surplus = pnode->partition.capacity - need_cap;
				pnode->partition.capacity=need_cap;
				pnode->partition.isAllocate = true;
				if( surplus!=0 )
				{
					//新建空闲分区
					SIZE++;
					PartNode *newPart = getNewPart(SIZE,surplus);
					PartNode *pnext = pnode->hou;
					if(pnext!=NULL)
					{
						pnext->qian = newPart;	
						newPart->hou = pnext;
					}else
					{
						newPart->hou = NULL;
					}
					pnode->hou = newPart;
					newPart->qian = pnode;				
				}
				isAlloc = true;
				break;

			}
			pnode = pnode->hou;
			if( pnode == previousPart && !isAlloc )
			{
				//从头重新遍历并已到达分界点，并且也没有分配
				cout<<"没有可分配空闲分区！未执行分配请求！"<<endl;
				break;
			}
			if( pnode==NULL && !isAlloc )
			{
				//遍历到最后一个节点且还没有分配时：
				pnode = first;//从头开始遍历
			}
		}
	}
	showInfo(part);
	showInfo(proMap);
}

//最佳适应算法(Best Fit)
void bestFit(PartLinkList &part,vector<Process> pros){
	
	cout<<"最佳适应算法--"<<endl;
	int size_pros = pros.size();
	//遍历提出请求的进程作业
	for(int k=0;k<size_pros;k++ )
	{
		vector<Partition> freePart = getFreePart(part);//申请前检索空闲分区 
		vector<Partition> sortPart = BubbleSort(freePart);//对空闲分区排序
		cout<<"排序后: "<<sortPart.size()<<endl;
		bool isAlloc = false;
		cout<<endl;
		int size_sp = sortPart.size();//排序后的空闲分区
		for( int i=0;i<size_sp;i++ )
		{
			Process pro = pros.at(k);
			int need_cap = pro.need_capacity;
			int cap = sortPart.at(i).capacity;
			if( cap>= need_cap )
			{
				
				//空闲区空间大小大于申请空间的大小--分配
				PartNode *pnode = getFreeNode(part,sortPart.at(i));
				cout<<"当前符合条件节点编号: "<<pnode->partition.pno<<" 空间大小："<<pnode->partition.capacity <<" 申请空间："<<need_cap<<endl;
				//申请进程存入进程集合
				pro.partition = pnode->partition.pno;
				pro.part = pnode;
				proMap.insert(pair<int,Process>(pro.pno,pro));
				//空闲分区分配空间
				int surplus = pnode->partition.capacity - need_cap;
				pnode->partition.capacity=need_cap;
				pnode->partition.isAllocate = true;
				if( surplus!=0 )
				{
					//新建空闲分区
					SIZE++;
					PartNode *newPart = getNewPart(SIZE,surplus);
					PartNode *pnext = pnode->hou;
					if(pnext!=NULL)
					{
						pnext->qian = newPart;	
						newPart->hou = pnext;
					}else
					{
						newPart->hou = NULL;
					}
					pnode->hou = newPart;
					newPart->qian = pnode;				
				}
				isAlloc = true;
				break;

			}
			if( i==size_sp-1 && !isAlloc )
			{
				cout<<"没有可分配空闲分区！未执行分配请求！"<<endl;
			}
		}
	}

	cout<<endl;
	showInfo(part);
	showInfo(proMap);
} 

//最坏适应算法(Worst Fit) 
void worstFit(PartLinkList &part,vector<Process> pros)
{
	cout<<"最坏适应算法--"<<endl;
	int size_pros = pros.size();
	//遍历提出请求的进程作业
	for(int k=0;k<size_pros;k++ )
	{
		vector<Partition> freePart = getFreePart(part);//申请前检索空闲分区 
		vector<Partition> sortPart = BubbleSort(freePart);//对空闲分区排序
		cout<<"排序后: "<<sortPart.size()<<endl;
		bool isAlloc = false;
		cout<<endl;
		int size_sp = sortPart.size();//排序后的空闲分区
		if( size_sp > 0 )
		{
			Process pro = pros.at(k);
			int i = size_sp-1;//选取最大的空闲分区进行分配操作
			int need_cap = pro.need_capacity;
			int cap = sortPart.at(i).capacity;
			//请求的空间大小是否符合当前空闲区的空间大小
			if( cap>= need_cap )
			{
				//空闲区空间大小大于申请空间的大小--分配
				PartNode *pnode = getFreeNode(part,sortPart.at(i));
				cout<<"当前符合条件节点编号: "<<pnode->partition.pno<<" 空间大小："<<pnode->partition.capacity <<" 申请空间："<<need_cap<<endl;
				//申请进程存入进程集合
				pro.partition = pnode->partition.pno;
				pro.part = pnode;
				proMap.insert(pair<int,Process>(pro.pno,pro));
				//空闲分区分配空间
				int surplus = pnode->partition.capacity - need_cap;
				pnode->partition.capacity=need_cap;
				pnode->partition.isAllocate = true;
				if( surplus!=0 )
				{
					//新建空闲分区
					SIZE++;
					PartNode *newPart = getNewPart(SIZE,surplus);
					PartNode *pnext = pnode->hou;
					if(pnext!=NULL)
					{
						pnext->qian = newPart;	
						newPart->hou = pnext;
					}else
					{
						newPart->hou = NULL;
					}
					pnode->hou = newPart;
					newPart->qian = pnode;				
				}
				isAlloc = true;
			}
		}
		if(!isAlloc )
		{
			cout<<"没有可分配空闲分区！未执行分配请求！"<<endl;
		}

	}
	cout<<endl;
	showInfo(part);
	showInfo(proMap);
}

//分区回收
void RecycleAlg( PartLinkList &partList,int partno  )   // 回收算法    
{   
	PartNode *pPre = NULL;
	PartNode *pNext = NULL;
    //定义前一个分区空闲标志
	bool previous = false;
	//定义后一个分区空闲标志
	bool next = false;
	//获取头节点
	PartNode *p = getHead(partList);
	while( p )
	{
		if( p->partition.isAllocate == true )
		{
			//寻找作业号所在内存分区
			Partition part = p->partition;
			if( partno == part.pno  )
			{
				int proNo = 0;
				map<int,Process> ::iterator it;
				for( it = proMap.begin();it!=proMap.end();it++ )
				{
					if( it->second.partition == partno )
					{
						proNo = it->first;
						proMap.erase(proNo);
						break;
					}
				}

				//释放分区前一个分区是否空闲
				if( p->qian )
				{
					pPre = p->qian;
					if( p->qian->partition.isAllocate == false )
					{
						previous = true;
					}
				}
				//释放分区后一个分区是否空闲
				if( p->hou )
				{
					pNext = p->hou;
					if( p->hou->partition.isAllocate == false)
					{
						next = true;
					}
				}
				//与前后分区合并
				if( previous==true && next==true )
				{
					PartNode *pCurrently = p;
					int totalSize = p->partition.capacity+pPre->partition.capacity+pNext->partition.capacity;//三者相加求和
					pPre->partition.capacity = totalSize;
					//去掉当前节点及当前节点的后继节点			
					if( pNext->hou )
					{
						pPre->hou = pNext->hou;
						pNext->hou->qian = pPre;
					}else
					{
						pPre->hou = NULL;
					}
					delete pNext;
					delete pCurrently;
					pNext = NULL;
					pCurrently = NULL;
				}
				//与前分区合并
				if( previous==true && next!=true )
				{
					PartNode *pCurrently = p;
					int totalSize = pCurrently->partition.capacity+pPre->partition.capacity;//两者求和
					pPre->partition.capacity = totalSize;
					//去掉当前节点
					if(pCurrently->hou)
					{
						pPre->hou = pCurrently->hou;
						pCurrently->hou->qian = pPre;
					}else
					{
						pPre->hou =NULL;
					}
					delete pCurrently;
					pCurrently = NULL;

				}
				//与后分区合并
				if(!previous && next)
				{
					PartNode *pCurrently = p;
					int totalSize = pCurrently->partition.capacity+pNext->partition.capacity;//两者求和
					pCurrently->partition.capacity = totalSize;
					pCurrently->partition.isAllocate = false;
					//去掉后继节点
					if( pNext->hou )
					{
						pCurrently->hou = pNext->hou;
						pNext->qian = pCurrently;
					}else
					{
						pCurrently->hou = NULL;
					}
					delete pNext;
					pNext = NULL;
				}
				//不合并--调整当前分区状态
				if(!previous && !next)
				{
					PartNode *pCurrently = p;
					p->partition.isAllocate = false;
				}
				showInfo(partList);
				showInfo(proMap);
				break;

			}	
		}
		p = p->hou;
		if( p==NULL )
		{
			cout<<"没有该分区！"<<endl;
		}

	}
    return ;    
}    


//输出分区明细
void showInfo(PartLinkList p)
{
	cout<<endl;
	cout<<"-----------------------------分区资源明细-----------------------------"<<endl;
	cout<<endl;
	PartNode *pnode = getHead(p);
	cout<<setw(12)<<"编号";
	while(pnode!=NULL)
	{
		cout<<setw(8)<<pnode->partition.pno;
		pnode = pnode->hou;
	}
	cout<<endl;
	cout<<setw(12)<<"大小";
	pnode = getHead(p);
	while(pnode!=NULL)
	{
		cout<<setw(8)<<pnode->partition.capacity;
		pnode = pnode->hou;
	}
	cout<<endl;
	cout<<setw(12)<<"状态";
	pnode = getHead(p);
	while(pnode!=NULL)
	{
		cout<<setw(8)<<status( pnode->partition.isAllocate );
		pnode = pnode->hou;
	}
	cout<<endl;
	cout<<endl;
}
void showInfo(map<int,Process> pros)
{
	cout<<endl;
	cout<<"-----------------------------当前进程明细-----------------------------"<<endl;
	cout<<endl;
	map<int,Process> ::iterator it;
	cout<<setw(12)<<"编号";
	for( it=pros.begin();it!=pros.end();it++ )
	{
		cout<<setw(8)<<it->first;
	}
	cout<<endl;
	cout<<setw(12)<<"大小";
	for( it=pros.begin();it!=pros.end();it++ )
	{
		cout<<setw(8)<<it->second.need_capacity;
	}
	cout<<endl;
	cout<<setw(12)<<"分区";
	for( it=pros.begin();it!=pros.end();it++ )
	{
		cout<<setw(8)<<it->second.partition;
	}
	cout<<endl;
	cout<<endl;
	
}
//状态(是否分配)
string status(bool isAllocation)
{
	if(isAllocation)
	{
		return "已分配";
	}else
	{
		return "未分配";
	}
}

//新建分区(编号,大小)
PartNode *getNewPart(int pno,int capacity)
{
	PartNode *newPart = new PartNode;
	newPart->partition.capacity = capacity;
	newPart->partition.pno = pno;
	newPart->partition.isAllocate = false;
	return newPart;
}
//获取首节点
PartNode *getHead(PartLinkList pList)
{
	return pList->hou;
}
//获取尾节点
PartNode *getLast(PartLinkList pList )
{
	PartNode *pnode = getHead(pList);
	while(pnode){
		pnode = pnode->hou;
	}
	return pnode;
}
//排序(冒泡)
vector<Partition> BubbleSort( vector<Partition> &parts )
{
	int length = parts.size()-1;//分区列表的大小
	bool isSwap = true;//默认进行了交换
	int size = 0;
	while( length>0 && isSwap )
	{
		isSwap = false;//是否发生了交换
		for( int i=0;i< length;i++ )
		{
			if( parts.at(i).capacity > parts.at(i+1).capacity )
			{
				isSwap = true;
				size++;
				int temp_cap = parts.at(i).capacity;
				parts.at(i).capacity= parts.at(i+1).capacity;
				parts.at(i+1).capacity = temp_cap;
				int temp_addr = parts.at(i).addr;
				parts.at(i).addr= parts.at(i+1).addr;
				parts.at(i+1).addr = temp_addr;
				bool temp_allo = parts.at(i).isAllocate;
				parts.at(i).isAllocate= parts.at(i+1).isAllocate;
				parts.at(i+1).isAllocate = temp_allo;
				int temp_pno =  parts.at(i).pno ;
				parts.at(i).pno= parts.at(i+1).pno;
				parts.at(i+1).pno = temp_pno;
			}

		}
		length--;

	}
	return parts;

}
//检出空闲分区
vector<Partition> getFreePart(PartLinkList partList)
{
	PartNode *p = getHead(partList);
	vector<Partition> arrFreePart;
	while( p )
	{
		if(  p->partition.isAllocate ==false )
		{	
			Partition part = p->partition;
			arrFreePart.push_back(part);
		}
		p=p->hou;
	}
	return arrFreePart;
}
//查找
PartNode *getFreeNode( PartLinkList partList,PartNode p  )
{
	PartNode *part = getHead(partList);
	while( part )
	{
		if( part->partition.isAllocate == false )
		{
			if( p.partition.pno == part->partition.pno )
			{
				return part;//返回符合条件的此节点
			}
		}
		part = part->hou;

	}
	return NULL;

}
PartNode *getFreeNode( PartLinkList partList,Partition p  )
{
	PartNode *part = getHead(partList);
	while( part )
	{
		if( part->partition.isAllocate == false )
		{
			if( p.pno == part->partition.pno )
			{
				return part;//返回符合条件的此节点
			}
		}
		part = part->hou;

	}
	return NULL;
}



int main(int argc, char** argv) {

	PartLinkList partList;

	initPartition(partList,SIZE);

	while (1)
	{
		int ch;//算法选择标记
		cout<<endl;
		cout << "             动态分区分配方式             \n";
		cout << "******************************************\n";
		cout << "** 1) 首次适应算法  2) 循环首次适应算法 **\n";
		cout << "** 3) 最佳适应算法  4) 最坏适应算法     **\n";
		cout << "******************************************\n";
		cout << "请选择分配算法：";
		cin >> ch;
		int choice;  //操作选择标记

		cout << "******************************************\n";
		cout << "**    1) 分配内存        2) 回收内存    **\n";
		cout << "**    3) 查看分配        0) 退    出    **\n";
		cout << "******************************************\n";
		cout << "请输入您的操作 ：";
		cin >> choice;
		if (choice == 1){
			reqAlloc(partList,ch); // 分配内存
		}else if (choice == 2)  // 内存回收
		{
			vector<int> freePart;
			cout<<"结束请按 y "<<endl;
			for(int i=0;;i++)
			{
				int ID;
				cout << "请输入您要释放的分区号：";
				cin >> ID;
				freePart.push_back(ID);
				string str = "y";
				cout<<"结束? y/n " ;
				cin>>str;
				if(str=="y")
				{
					break;
				}
			}
			int len = freePart.size();
			for( int i=0;i<len;i++ )
			{
				RecycleAlg(partList,freePart.at(i) );
			}
		}
		else if (choice == 3){ showInfo(partList); showInfo(proMap);}//显示主存
		else if (choice == 0) break; //退出
		else //输入操作有误
		{
			cout << "输入有误，请重试！" << endl;
			continue;
		}
	}
	//showInfo(partList);

	return 0;
}