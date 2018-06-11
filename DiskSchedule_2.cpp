// FCFS.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include <iostream>
using namespace std;
int disks[100];
int count = 0;
int way;
int intway;
bool isadd = false;
int start;
int big[100];
int bit[100];
int bitcount=0;
int bigcount = 0;

void FCFS(){
	float result = 0;
	int a;
	a= start - disks[0];
	if(a<0){
	a = a*(-1);
	}
	result+=a;
	cout<<"每次移动距离："<<a<<" ";
	for(int i=0;i<count;i++){		
		if(i==count-1)
			break;
		a= disks[i] - disks[i+1];
		if(a<0){
			a = a*(-1);
				}
		cout<<a<<" ";
		result+=a;		
	}
	result = result/count;
	cout<<endl<<"平均寻道长度："<<result<<endl;
}
void SSTF(){
	int min;
	int restart = start;
	int next = 0;
	int contrast[100];
	int fresh[100];
	int recount = count;
	for(int i=0;i<count;i++){
		//得到差值数组
		for(int i=0;i<recount;i++){
			contrast[i] = disks[i] - restart;
			if(contrast[i] < 0)
				contrast[i] = contrast[i] * -1;
		}
		//得到下一个要调度的数的位置next
		for(int i=0;i<recount;i++){
			min = contrast[0];
			if(contrast[i] < min){
				min = contrast[i];
				next = i;
			}		
		}
		restart = disks[next];
		fresh[i] = disks[next];
		//移除此磁道号
		for(int i=0;i<recount;i++){
			if(next == recount - 1)
				break;
			disks[next] = disks[next+1];
			next++;
		}
		recount--;
		next = 0;
	}
	cout<<"磁道访问顺序：";
	for(int i=0;i<count;i++){
		disks[i] = fresh[i];
		cout<<disks[i]<<" ";
	}
	cout<<endl;
	FCFS();
}
void SCAN(){
	int bigi = 0;
	int biti = 0;	
	for(int i=0;i<count;i++)
	{		
		if(disks[i]>=start){
			big[bigi] = disks[i];
			bigi++;
			bigcount++;		
		}
		else
		{
			bit[biti] = disks[i];
			biti++;
			bitcount++;
	    }
	}
	int temp;
	//比start大的数，从小到大
	for(int j = 0;j<bigcount;j++)
	for(int i=0;i<bigcount;i++)
	{
		if(i == bigcount-1)
			break;
		if(big[i]>big[i+1])
		{
			 temp = big[i+1];
			 big[i+1] = big[i];
			 big[i] = temp;
		}
	}
	//比start小的数，从小到大
	for(int j = 0;j<bitcount;j++)
	for(int i=0;i<bitcount;i++)
	{
		if(i == bitcount-1)
			break;
		if(bit[i]>bit[i+1])
		{
			 temp = bit[i+1];
			 bit[i+1] = bit[i];
			 bit[i] = temp;
		}
	}
	if(isadd){
		cout<<"磁道访问顺序：";
		for(int i = 0;i<bigcount;i++){
			disks[i]=big[i];
			cout<<disks[i]<<" ";
		}
		for(int i = bitcount-1;i>=0;i--){
			disks[bigcount]=bit[i];
			cout<<disks[bigcount]<<" ";
			bigcount++;			
		}
	}else{
		int bcd = 0;
		cout<<"磁道访问顺序：";
		for(int i = bitcount-1;i>=0;i--){
			disks[bcd]=bit[i];
			cout<<disks[bcd]<<" ";
			bcd++;						
		}
		int abc = 0;
		for(int i = bitcount;i<count;i++){
			disks[bitcount]=big[abc];
			abc++;
			cout<<disks[bitcount]<<" ";
			bitcount++;
		}
	}
	cout<<endl;
	FCFS();
}
void CSCAN(){
	int bigi = 0;
	int biti = 0;	
	for(int i=0;i<count;i++)
	{		
		if(disks[i]>=start){
			big[bigi] = disks[i];
			bigi++;
			bigcount++;	
		}
		else
		{
			bit[biti] = disks[i];
			biti++;
			bitcount++;
	    }
	}
	int temp;
	//比start大的数，从小到大
	for(int j = 0;j<bigcount;j++)
	for(int i=0;i<bigcount;i++)
	{
		if(i == bigcount-1)
			break;
		if(big[i]>big[i+1])
		{
			 temp = big[i+1];
			 big[i+1] = big[i];
			 big[i] = temp;
		}
	}
	//比start小的数，从小到大
	for(int j = 0;j<bitcount;j++)
	for(int i=0;i<bitcount;i++)
	{
		if(i == bitcount-1)
			break;
		if(bit[i]>bit[i+1])
		{
			 temp = bit[i+1];
			 bit[i+1] = bit[i];
			 bit[i] = temp;
		}
	}
	if(isadd){
		cout<<"磁道访问顺序：";
		for(int i = 0;i<bigcount;i++){
			disks[i]=big[i];
			cout<<disks[i]<<" ";
		}
		//for(int i = bitcount-1;i>=0;i--){
		//	disks[bigcount]=bit[bitcount-1-i];
		//	cout<<disks[bigcount]<<" ";
		//	bigcount++;			
		//}
		int a = 0;	
		for(int i = bigcount;i<count;i++){
			disks[i]=bit[a];
			cout<<disks[i]<<" ";
			a++;			
		}
	}else{
		int bcd = 0;
		cout<<"磁道访问顺序：";
		//小数从大到小
		for(int i = bitcount-1;i>=0;i--){
			disks[bcd]=bit[i];
			cout<<disks[bcd]<<" ";
			bcd++;									
		}
		//大数从大到小
		int abc = 0;
		int i=bitcount;
		for(i;i<count;i++){
			disks[i]=big[bigcount-abc-1];
			abc++;
			cout<<disks[i]<<" ";
		}
	}
	cout<<endl;
	FCFS();
}
int _tmain(int argc, _TCHAR* argv[])
{	
	cout<<"请输入磁道号(以回车结束)"<<endl;
	for(int i=0;;i++){
		cin>>disks[i];
		count++;
		if(getchar()=='\n'){
		break;
		}	
	}
	cout<<"请输入开始磁道号：";
	cin>>start;
	cout<<"选择方法:\t1、FCFS\t2、SSTF\t3、SCAN\t4、CSCAN"<<endl;
	cin>>way;
	switch(way){	
	case 1 : FCFS();
			 break;
	case 2 :SSTF();
		break;
	case 3 :cout<<"请输入方向:1向外扫描2向内扫描："<<endl;
			cin>>intway;
			if(intway==1)
				isadd = true;
			SCAN();
			break;	
	case 4 :cout<<"请输入方向:1向外扫描2向内扫描："<<endl;
			cin>>intway;
			if(intway==1)
				isadd = true;
			CSCAN();
			break;	
	}
	return 0;
}

