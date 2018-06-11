// DiskSchedule.cpp : 定义控制台应用程序的入口点。

#include "stdafx.h"
#include <iostream>
#include <string>
#include <vector>
#include <map>
using namespace std;

//初始访问磁道号
static int NUM_FIRST;
//磁盘访问方向
static string VISIT_ORIENTATION = "in2out";//默认从里向外
//平均距离
static int AVEDIS=0;

//磁道集合
static vector<int> disks;
static vector<int> newList;
static map<int,int> distances;


void selectSort(vector<int> &list);
void show(vector<int> list);
void scan();
void cscan();
void selectScan();
void getScanList(vector<int>  &list);
void getCScanList(vector<int>  &list);

void init()
{
	cout<<"请输入当前初始磁道号：";
	cin>>NUM_FIRST;
	cout<<endl;
	cout<<"请输入被访问的磁道号(结束请按回车键)：";
	int temp_num;
	for(int i=0;;i++)
	{
		cin>>temp_num;
		char ch =  getchar();
		disks.push_back(temp_num);	
		if(ch=='\n')
		{
			break;	
		}
	}
	cout<<"结果:"<<disks.size()<<endl;
	cout<<"请选择磁道访问方向( in2out 或者 out2in )： ";
	string orientation= "in2out";
	for(int i=0;;i++)
	{
		cin>>orientation;
		if( orientation=="out2in"  )
		{
			VISIT_ORIENTATION = orientation;
			selectScan();
			break;
		}else
		{
			if( orientation=="in2out"  )
			{
				selectScan();
				break;
			}else
			{
				cout<<"请正确输入方向：( in2out 或者 out2in )"<<endl;
			}
		}
	}

}
void selectScan()
{
	cout<<endl;
	cout<<"********************请选择磁盘调度算法********************"<<endl;
	cout<<"************           1.电梯调度             ************"<<endl;
	cout<<"************           2.循环扫描             ************"<<endl;
	cout<<"**********************************************************"<<endl;
	int cho = 1;
	cin>>cho;
	if(cho==1)
	{
		scan();
	}else
	{
		cscan();
	}
}

//电梯算法
void scan()
{
	int size = disks.size();
	if(size == 0)
	{
		cout<<"被访问磁道号为空，请输入后再试！"<<endl;
	}else
	{
		vector<int> temp;
		for(int i=0;i<size;i++)
		{
			temp.push_back( disks.at(i) );
		}
		//1.排序
		selectSort(temp);
		show(temp);
		//2.获取序列
		getScanList(temp);
		    
	}
}
//获取扫描序列
void getScanList(vector<int>  &list)
{
	int size = list.size();
	int sumDis = 0;
	int tempDis = NUM_FIRST;
	bool isRight = false;
	for(int i=0;i<size;i++ )
	{
		if( list.at(i) >= NUM_FIRST )
		{
			
			int flag = i;//标志位
			for(int j=flag;j<size;j++)
			{
				int num_disk = list.at(j);
				int distance =num_disk - tempDis;
				newList.push_back(num_disk);
				distances.insert(pair<int,int> (num_disk,distance));
				sumDis+=distance;
				tempDis = num_disk;
			}
			for(int j = flag-1;j>=0;j--)
			{
				int num_disk = list.at(j);
				int distance = tempDis - num_disk ;
				newList.push_back(num_disk);
				distances.insert(pair<int,int> (num_disk,distance));
				sumDis+=distance;
				tempDis = num_disk;
			}
			isRight = true;
			break;
		}else
		{
			//所有的磁道号都是小于当前的磁道号
			isRight = false;
		}
	}
	if(isRight!=true)
	{
		int last = list.size()-1;
		for(int j = last ;j>=0;j--)
		{
			int num_disk = list.at(j);
			int distance = tempDis - num_disk ;
			newList.push_back(num_disk);
			distances.insert(pair<int,int> (num_disk,distance));
			sumDis+=distance;
			tempDis = num_disk;
		}

	}
	AVEDIS = sumDis/distances.size();
	cout<<"平均寻道长度："<<AVEDIS<<endl;
	show(newList);
	list.clear();
	
}


//简单选排(可用于链式存储结构)
void selectSort(vector<int> &list)
{
	int size = list.size();
	for(int i=0;i<size;i++ )
	{
		int k = i;
		for(int j=i+1;j<size;j++ )
		{
			if( list.at(j) < list.at(k) )
			{
				k = j;
			}
		}
		if(k!=i)
		{
			int temp = list.at(i);
			list.at(i) = list.at(k);
			list.at(k) = temp;
		}
	}

}
//快速排序(适合顺序结构)
vector<int> ::iterator quickSort(vector<int> &list)
{
	vector<int> ::iterator it_low = list.begin();
	vector<int> ::iterator it_high = list.end();
	int flag = list.at(0);//基准数
	while( it_low < it_high )
	{
		//在末端方向找一个比基准数小的数值
		while( it_low < it_high && *it_high > flag )
		{
			it_high--;
		}
		*it_low = *it_high;//将右端的小于基准数的值填到首端
		//在首端方向找一个比基准数大的数值
		while( it_low < it_high && *it_low <= flag )
		{
			it_low++;
		}
		//(末端产生了一个坑)
		//1.填坑：(it_low++ 了)   将找到的左端的小于基准数的值填到末端
		//2.填坑：(it_low没有++)  将上次的结果填到末端
		*it_high = *it_low;
	}
	*it_low = flag;
	return it_low;
}


//循环扫描算法
void cscan()
{
	int size = disks.size();
	if(size == 0)
	{
		cout<<"被访问磁道号为空，请输入后再试！"<<endl;
	}else
	{
		vector<int> temp;
		for(int i=0;i<size;i++)
		{
			temp.push_back( disks.at(i) );
		}
		//1.排序
		selectSort(temp);
		show(temp);
		//2.获取序列
		getCScanList(temp);
		    
	}
}
//获取扫描序列
void getCScanList(vector<int>  &list)
{
	int size = list.size();
	int sumDis = 0;
	int tempDis = NUM_FIRST;
	bool isRight = false;
	for(int i=0;i<size;i++ )
	{
		if( list.at(i) >= NUM_FIRST )
		{
			
			int flag = i;//标志位
			for(int j=flag;j<size;j++)
			{
				int num_disk = list.at(j);
				int distance =num_disk - tempDis;
				newList.push_back(num_disk);
				distances.insert(pair<int,int> (num_disk,distance));
				sumDis+=distance;
				tempDis = num_disk;
			}
			for(int j = 0;j<=flag-1;j++)
			{
				int num_disk = list.at(j);
				int distance = abs( tempDis - num_disk );			
				newList.push_back(num_disk);
				distances.insert(pair<int,int> (num_disk,distance));
				sumDis+=distance;
				tempDis = num_disk;
			}
			isRight = true;
			break;
		}else
		{
			//所有的磁道号都是小于当前的磁道号
			isRight = false;
		}
	}
	if(isRight!=true)
	{
		for(int j = 0 ;j<size;j++)
		{
			int num_disk = list.at(j);
			int distance = tempDis - num_disk ;
			newList.push_back(num_disk);
			distances.insert(pair<int,int> (num_disk,distance));
			sumDis+=distance;
			tempDis = num_disk;
		}

	}
	AVEDIS = sumDis/distances.size();
	cout<<"平均寻道长度："<<AVEDIS<<endl;
	show(newList);
	list.clear();
	
}

//输出
void show(vector<int> list)
{
	cout<<"磁道序列为："<<endl;
	int size = list.size();
	for( int i=0;i<size;i++ )
	{
		if(i!=size-1)
		{
			cout<<list.at(i)<<" - ";					
		}else
		{
			cout<<list.at(i);		
		}
	}
	cout<<endl;
}
void show(map<int,int> maps)
{
	map<int,int> ::iterator it ;
	for(it = maps.begin();it!=maps.end();it++ )
	{
		cout<<"磁道号："<<it->first<<" 移动距离："<<it->second<<endl;
	}
}


