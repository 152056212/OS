// bank.cpp : 定义控制台应用程序的入口点。
#include "stdafx.h"
#include <vector>
#include <iostream>
#include <string>
#include <map>
#include <iomanip>
using namespace std;

//资源集
map<string,int> rm;
map<string,int> ::iterator itRm;
map<string,int> avaMap;
map<string,int> ::iterator itAva;

typedef struct
{
	//进程名称
	string pro_name;
	//最大需求
	map<string,int> maxMap;
	//资源分配
	map<string,int> allocationMap;
	//资源需求
	map<string,int> needMap;
}Process;
//进程集
map<string,Process> processMap;
map<string,Process> ::iterator itProMap;
map<string,Process> getCopy();
map<string,bool> finishs;
vector<string> secProArr;
//资源函数
map<string,int> setWork(map<string,int> work,map<string,int> allocation);
map<string,int> wkAndAva;

void showInfo(map<string,Process> processMap);
void falseFinish();
void secDistribution(map<string,Process> temp);

//初始化总资源数量
void initRes()
{
	cout<<"----------------------------初始化资源总量----------------------------"<<endl;
	int num_avi;
	cout<<"请输入资源信息...";
	cout<<"资源类数：";
	cin>>num_avi;
	cout<<endl;
	for(int i=0;i<num_avi;i++)
	{
		string re_name;
		int num_re;
		cout<<"请输入资源名称:";
		cin>>re_name;
		cout<<"资源数:";
		cin>>num_re;
		cout<<endl;
		pair<string,int>temp(re_name,num_re);
		rm.insert(temp);
	}
}
//可利用资源向量
void setAva()
{
	cout<<"初始化当前的资源可利用数......."<<endl;
	avaMap.clear();
	string rename="";
	int ava=0;
	map<string,int> ::iterator it;
	for(it=rm.begin();it!=rm.end();it++)
	{
		int allocation=0;
		for(itProMap=processMap.begin();itProMap!=processMap.end(); itProMap++)
		{
			allocation+=itProMap->second.allocationMap.at(it->first);

		}
		ava = it->second - allocation;
		cout<<setw(16)<<"资源名"<<setw(16)<<it->first<<setw(16)<<" 可用资源数:"<<setw(16)<<ava<<endl;
		avaMap.insert(pair<string,int>(it->first,ava));
		ava=0;
	}
	
}

//创建进程
void createPro()
{
	cout<<"-------------------------------创建进程-------------------------------"<<endl;
	cout<<"当前可用资源明细"<<endl;
	//遍历资源
	for(itRm = rm.begin(); itRm!=rm.end();itRm++ )
	{
		cout<<itRm->first<<" "<<itRm->second<<endl;
	}
	//1.初始化最大需求向量
	int numOfPro;
	cout<<"请输入进程数:";
	cin>>numOfPro;
	cout<<"请设置进程信息："<<endl;
	cout<<endl;
	for(int i=0;i<numOfPro;i++)
	{
		Process pro;
		//1.进程名
		string proName;
		cout<<"请输入进程名:";cin>>proName;cout<<endl;
		pro.pro_name = proName;
		//2.进程最大需求
		map<string,int>  ::iterator it;
		for(it=rm.begin();it!=rm.end();it++)
		{
			int max;
			cout<<it->first<<"的资源数:"<<it->second<<" "<<"请输入对于"<<it->first<<"资源的最大资源需求:";
			cin>>max;
			cout<<endl;
			pair<string,int> temp(it->first,max);				
			pro.maxMap.insert(temp);
		}
				it=rm.begin();
		//3.进程资源分配
		map<string,int> maxMap = pro.maxMap;
		for(it=maxMap.begin();it!=maxMap.end();it++)
		{
				int allocation=0;//默认资源分配数：0
				//cout<<proName<<"进程"<<"---"<<it->first<<"最大需求:"<<it->second<<" 分配数:";
				//cin>>allocation;
				//cout<<endl;
				pair<string,int> temp(it->first,0);
				pro.allocationMap.insert(temp);
		}
		it = rm.begin();
		//4.进程资源分配
		map<string,int> nMap=pro.needMap;
		for(it=maxMap.begin();it!=maxMap.end();it++)
		{
			string reName=it->first;
			int num_needs = it->second - pro.allocationMap.at(it->first);
			//cout<<proName<<"进程"<<"中资源"<<it->first<<"的需求为："<<num_needs;
			pair<string,int> tempNeed(reName,num_needs);
			pro.needMap.insert(tempNeed);
		}
		//cout<<endl;
		pair<string,Process> tempPro(pro.pro_name,pro);
		processMap.insert(tempPro);
		finishs.insert(pair<string,bool>(pro.pro_name,false) );
		showInfo(processMap);
		
	}
	
}
//输出进程信息
void showInfo(map<string,Process> processMap)
{
	cout<<"-----------------------------当前进程信息-----------------------------"<<endl;
	for(itProMap = processMap.begin();itProMap!= processMap.end();itProMap++)
		{
			if(itProMap == processMap.begin())
			{
				cout<<setw(16)<<"资源"<<setw(16/2)<<"|"<<setw(16)<<"已分配"<<setw(16/2)<<"|"<<setw(16)<<"需求"<<endl;
			}
			cout<<setw(16)<<itProMap->first<<setw(16/2)<<"|";
			int size = rm.size();
			for(itRm = rm.begin();itRm!=rm.end();itRm++)
			{
				int i = itProMap->second.allocationMap.at(itRm->first);
				cout<<setw(16/size)<<i;
			}
			cout<<setw(16/2)<<"|";
			for(itRm = rm.begin();itRm!=rm.end();itRm++)
			{
				int i = itProMap->second.needMap.at(itRm->first);
				cout<<setw(16/size)<<i;
			}
			cout<<endl;
		}
	cout<<endl;
}

//假设性分配
void hypTesting(bool isFirst)
{
	cout<<"假设性分配......"<<endl;
	map<string,Process> tempPro = getCopy();
	map<string,int> reqMap;
	bool isReq = true;
	while(isReq)
	{
	//////////////////////////////////////////////////////////////////////////////	
	//1.输入请求
	
	map<string,int> ::iterator itRes;
	string pro_name;
	reqMap.clear();
	if(isFirst==true)
	{
		reqMap.clear();
		itProMap = processMap.begin();
		pro_name = itProMap->first;
		for(itRm=rm.begin();itRm!=rm.end();itRm++)
		{
			reqMap.insert(pair<string,int>(itRm->first,itProMap->second.needMap.at(itRm->first)));
		}
	}else
	{
		reqMap.clear();
	
		cout<<"请输入提出请求的进程名：";		
		cin>>pro_name;
		cout<<"请输入该进程的资源请求情况:"<<endl;
		for(itRm=rm.begin();itRm!=rm.end();itRm++)
		{
		int req;
		cout<<"资源"<<itRm->first<<" ";
		cin>>req;
		cout<<endl;
		reqMap.insert(pair<string,int>(itRm->first,req));
		}
	}
	
	//2.检查合理性(是否符合进程当前分配资源的资源数)
	bool isAva = false;//默认非法
	setAva();
	for(itRm=rm.begin();itRm!=rm.end();itRm++)
	{
		cout<<endl;
		cout<<"资源："<<itRm->first<<" 总量:"<<rm.at(itRm->first)<<" 当前可用:"<<avaMap.at(itRm->first)<<" ";
		int ava = avaMap.at(itRm->first);
		int req = reqMap.at(itRm->first);
		map<string,int> ::iterator itEnd = rm.end();
		//判断请求资源量是否小于该资源的可用数
		if(ava>=req)
		{	
			if(itRm==(--itEnd))
			{
				isAva=true;
			}
			continue;
		}else
		{
			cout<<"资源"<<itRm->first<<"请求数大于该资源的可用数";
			cout<<"资源请求非法！";
			break;
		}
	}
	//3.检查合法性(是否符合Need)
	bool isNeed = false;//默认非法
	for(itRes=reqMap.begin();itRes!=reqMap.end();itRes++ )
	{
		int need = tempPro.at(pro_name).needMap.at(itRes->first);
		map<string,int> ::iterator itEnd = reqMap.end();
		if( itRes->second<=need )
		{
			if(itRes==(--itEnd))
			{
				//所有请求合法
				isNeed=true;
			}
			continue;
		}else
		{
			cout<<endl;
			cout<<itRes->first<<"请求数:"<<itRes->second<<"实际需求数:"<<need;
			cout<<"资源请求非法！"<<endl;
			break;
		}
		
	}

	bool reqIsOk = isAva&&isNeed;
	if(reqIsOk)
	{
		//请求合法，再判断合法性
		cout<<"请求合法！"<<endl;
		//假设性分配
		//p[i]进程的资源分配数减少
		map<string,int> work;//新的ava
		for(itRm=rm.begin();itRm!=rm.end();itRm++)
		{
			int reqNum = reqMap.at(itRm->first);
			int newAva = avaMap.at(itRm->first)-reqNum;
			//原可利用资源向量没有更新，过安全检查后更新
			work.insert(pair<string,int>(itRm->first,newAva));
			tempPro.at(pro_name).allocationMap.at(itRm->first) = tempPro.at(pro_name).allocationMap.at(itRm->first)+reqNum;
			tempPro.at(pro_name).needMap.at(itRm->first) =  tempPro.at(pro_name).needMap.at(itRm->first)-reqNum;
			
		}
		finishs.at(pro_name) = true;
		secProArr.push_back(pro_name);
		wkAndAva = setWork(work,tempPro.at(pro_name).allocationMap);
		map<string,Process> ::iterator itTempPro;
		itTempPro=tempPro.begin();
		while(itTempPro!=tempPro.end())
		{
			//遍历每个进程还是否安全
			if(finishs.at(itTempPro->first)==true)
			{
				itTempPro++;
				continue;
			}else
			{
				map<string,int> ::iterator itEnd = rm.end();
				//与进程的Need值比较
				map<string,int> tm;
				itEnd--;
				for(itRm=rm.begin();itRm!=rm.end();itRm++)
				{
					//计算释放后的资源
					int newRes = wkAndAva.at(itRm->first);
					if( itTempPro->second.needMap.at(itRm->first) <= newRes )
					{
						if(itRm==itEnd)
						{
							//合法
							finishs.at(itTempPro->first) = true;
							secProArr.push_back(itTempPro->first);
							
						}
						continue;
					}else
					{
						cout<<"出问题的进程:"<<itTempPro->first<<" 对应的资源:"<<itRm->first<<" 资源可用数:"<<newRes<<" 进程所需:"<<itTempPro->second.needMap.at(itRm->first);
						break;
					}

				}
				if( finishs.at(itTempPro->first) == true )
				{
					//cout<<"进程"<<itTempPro->first<<"合法"<<endl;
					//cout<<wkAndAva.begin()->first<<" "<<wkAndAva.begin()->second<<endl;
					wkAndAva = setWork(wkAndAva,tempPro.at(itTempPro->first).allocationMap);
					//重新开始
					itTempPro=tempPro.begin();
					//cout<<"重新检查"<<endl;

				}else
				{
					cout<<endl;
					//cout<<"检查下一个！"<<endl;
					itTempPro++;
				}
			}
		}
		bool isSec = true;
		for(itTempPro = processMap.begin();itTempPro!=processMap.end();itTempPro++)
		{
			if(finishs.at(itTempPro->first)==false)
			{
				isSec = false;
				break;
			}else
				continue;
		}
		if(isSec)
		{
			cout<<"可安全分配！"<<endl;
			cout<<"安全序列之一为:.........."<<endl;
			int size = secProArr.size();
			for(int i=0;i<size;i++)
			{
				cout<<setw(8)<<secProArr.at(i);
				if(i<(size-1)){
					cout<<setw(8)<<" --  ";
				}
			}
			cout<<endl;
			if(isFirst!=true)
			{
				showInfo(tempPro);				
			}
			cout<<endl;
			//执行安全性分配
			if(isFirst!=true)
			{
				secDistribution(tempPro);				
			}
		}else
		{
			cout<<"请求不合法！系统无法安全执行！"<<endl;
		}
		
		  
	}
	//////////////////////////////////////////////////////////////////////////////
	secProArr.clear();
	falseFinish();
	if(isFirst=true){
		isFirst = false;
		tempPro = getCopy();
	}
		string isContinue;
		cout<<"继续: y/n ?"<<endl;
		cin>>isContinue;
		if( isContinue=="y" )
		{
			isReq = true;
		}else
		{
			isReq = false;	
		}
	}

	
}
//当前资源值(work)+当前进程分配值(allocation)
map<string,int> setWork(map<string,int> work,map<string,int> allocation)
{
	map<string,int> temp;
	for(itRm=rm.begin();itRm!=rm.end();itRm++)
	{
		string rname = itRm->first;
		int num = work.at(rname)+allocation.at(rname);	
		//cout<<num<<" "<<allocation.at(rname)<<endl;
		pair<string,int> p(rname,num);
		temp.insert(p);
	}
	return temp;
}
//撤销操作
void falseFinish()
{
	for(itProMap = processMap.begin(); itProMap != processMap.end(); itProMap++ )
	{
		finishs.at(itProMap->first ) = false;
	}
}

//安全分配
void secDistribution(map<string,Process> temp)
{
	map<string,Process> ::iterator it;
	for(it = temp.begin();it!=temp.end();it++)
	{
		for(itRm = rm.begin();itRm!=rm.end();itRm++)
		{
			processMap.at(it->first).allocationMap.at(itRm->first) = temp.at(it->first).allocationMap.at(itRm->first);
			processMap.at(it->first).needMap.at(itRm->first) = temp.at(it->first).needMap.at(itRm->first); 
		}
	}
}
//获取进程副本
map<string,Process> getCopy()
{
	//获取当前进程副本
	map<string,Process> tempPro;
	map<string,Process> ::iterator it;
	for(it=processMap.begin();it!=processMap.end();it++)
	{
		pair<string,Process> tempItem(it->first,it->second);
		tempPro.insert(tempItem);
	}
	return tempPro;
}

int _tmain(int argc, _TCHAR* argv[])
{
	vector<int> v;
	//map<string,int> m;
	//m.clear();
	//m.insert( pair<string,int>("aaaa",561) );
	initRes();
	createPro();
	hypTesting(true);
	return 0;
}

