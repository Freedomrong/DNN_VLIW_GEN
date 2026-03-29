

#include<stdio.h>
#include<iostream>

using namespace std;
// int kstorge[14] = { 256, 128, 64, 32, 16, 8, 8, 8, 8, 8, 8, 16, 16, 16 };
// int original[14] = { 32, 32, 32, 64, 128, 256, 512, 1024, 256, 512, 256, 128, 256, 256 };
// int filters[14] = { 32, 32, 64, 128, 256, 512, 1024, 256, 512, 256, 128, 256, 256, 256 };
// int filters_size[14] = { 1, 3, 3, 3, 3, 3, 3, 1, 3, 1, 1, 3, 3, 1 };
// int fl[14] = {0,1,0,1,1,0,0,1,0,0,0,1,1,1};
// int fl_length[14] = {0,1,0,1,1,0,0,1,0,0,0,2,1,2};


// 新模型 768x768 参数 (14层: Conv11拆分为Conv11a和Conv11b)
// Conv0~Conv10不变, Conv11a处理Conv10输出(128ch), Conv11b处理Conv4输出(256ch), Conv12(原Conv12)改为Conv13
int kstorge[14] = { 768, 384, 192, 96, 48, 24, 24, 24, 24, 24, 24, 48, 48, 48 };
int original[14] = { 32, 32, 32, 64, 128, 256, 512, 1024, 256, 512, 32, 128, 256, 32 };
//                   C0  C1  C2  C3  C4   C5   C6   C7    C8   C9  C10 C11a C11b C12
int filters[14] = { 32, 32, 64, 128, 256, 512, 1024, 256, 512, 32, 128, 256, 256, 32 };
int filters_size[14] = { 1, 3, 3, 3, 3, 3, 3, 1, 3, 1, 1, 3, 3, 1 };
// fl: 是否需要移位 (fl_length>0则为1)
int fl[14] =           { 0, 1, 1, 1, 1, 0, 1, 1, 0, 0, 1, 0, 0, 0 };
// fl_length: 当前层数据1 - 前一层数据3
int fl_length[14] =    { 0, 1, 2, 2, 1, 0, 1, 1, 0, 0, 3, 0, 0, 0 };

long long  Count_DDR_Globalbuffer_first(int count)
{
	long long GlobalBuffer_DDR_source_address;
	if (count == 0)
	{
		GlobalBuffer_DDR_source_address = 0x0A0000000;
	}
	else if (count == 1)
	{
		GlobalBuffer_DDR_source_address = 0x0A3000000;
		//GlobalBuffer_DDR_source_address = 0x0FF000000;
	}
	else if (count == 2)
	{
		GlobalBuffer_DDR_source_address = 0x0A4000000;
	}
	else if (count == 3)
	{
		GlobalBuffer_DDR_source_address = 0x0A5000000;
	}
    else if (count == 4)
	{
		GlobalBuffer_DDR_source_address = 0x0A6000000;
	}
	else if (count == 5)
	{
		GlobalBuffer_DDR_source_address = 0x0A9000000;
	}
	else if (count == 6)
	{
		GlobalBuffer_DDR_source_address = 0x0A8000000;
	}
	else if (count == 7)
	{
		GlobalBuffer_DDR_source_address = 0x0A3000000;
	}
	else if (count == 8)
	{
		GlobalBuffer_DDR_source_address = 0x0A4000000;
	}
	else if (count == 9)
	{
		GlobalBuffer_DDR_source_address = 0x0A5000000;
	}
	else
	{
		GlobalBuffer_DDR_source_address = 0x0A0000000;
	}
	
	return GlobalBuffer_DDR_source_address;
}

long long Count_GlobalBuffer_first(int count)
{
	long long GlobalBuffer_first = 0;
	if (count == 0 || count == 1 || count == 2 || count == 3)
	{
		GlobalBuffer_first = 0x0c0000000;
	}
	else
	{
		GlobalBuffer_first = 0x0c0000000;
	}

	return GlobalBuffer_first;
}

long long Count_DDR_WeightCacheBuffer_first(int count)
{
	long long DDR_WeightCacheBuffer_first = 0x091000000;
	for (int i = 0; i < count; i++)
	{
		DDR_WeightCacheBuffer_first += filters_size[i]*filters_size[i]*2*original[i]*filters[i];
	}

	return DDR_WeightCacheBuffer_first;
}

int Count_Bias_addr_first(int count)
{
	int Bias_addr_first = 0;
	for (int i = 0; i < count; i++)
	{
		if (i != 11)
		{
			Bias_addr_first += (filters[i]%32 == 0?filters[i]/32:filters[i]/32+1);//��ʮһ���������Ҫbias����˲�����bias��ַ����
		}
	}
	return Bias_addr_first;

}
/*
int Count_Bias_addr_first(int count)
{
	int Bias_addr_first = 0;
	if (count == 3)
	{
		Bias_addr_first = 0;
	}
	else
	{
		Bias_addr_first = 4;
	}
	return Bias_addr_first;

}*/
long long Count_compute_result_first(int count)
{
	long long compute_rseult_first = 0;
	// if ( count == 2 || count ==6 || count == 8 || count == 10)
	// {
	// 	compute_rseult_first = 0x0c0000000;
	// }
	// else if (count ==3||count==5||count ==12)
	// {
	// 	compute_rseult_first = 0x0c0050000;
	// }
	// else if (count == 4)
	// {
	// 	compute_rseult_first = 0x0c00A0000;
	// }
	// else if (count == 7)
	// {
	// 	compute_rseult_first = 0x0c00D0000;
	// }
	// else if (count == 9)
	// {
	// 	compute_rseult_first = 0x085000000;
	// }
	// else if (count == 13)
	// {
	// 	compute_rseult_first = 0x086000000;
	// }
	if (count == 0)
	{
		compute_rseult_first = 0x0A3000000;
	}
	else if (count == 1)
	{
		compute_rseult_first = 0x0A4000000;
	}
	else if (count == 2)
	{
		compute_rseult_first = 0x0A5000000;
	}
	else if (count == 3)
	{
		compute_rseult_first = 0x0A6000000;
	}
	else if (count == 4)
	{
		compute_rseult_first = 0x0A7000000;
	}
	else if (count == 5)
	{
		compute_rseult_first = 0x0A8000000;
	}
	else if (count == 6)
	{
		compute_rseult_first = 0x0A3000000;
	}
	else if (count == 7)
	{
		compute_rseult_first = 0x0A4000000;  // result save
	}
	else if (count == 8)
	{
		compute_rseult_first = 0x0A5000000;
	}
	else if (count == 9)
	{
		compute_rseult_first = 0x0A6000000;
	}
	return compute_rseult_first;
}
/*8bit*/
/*
int compute_fl(int count)
{
	int compute_fl = 0;
	if (count == 11 || count == 12)//����
	{
		compute_fl = 2;
	}
	else if ( count == 2 || count == 3||count ==4 ||count ==6||count ==10)//����
	{
		compute_fl = 1;
	}
	else 
	{
		compute_fl = 0;
	}
	return compute_fl;
}

int compute_fl_length(int count)
{
	int compute_fl_length = 0;
	if (count == 2)//����
	{
		compute_fl_length = 2;
	}
	else if (count == 3 || count == 4 || count == 6 ||count ==10|| count ==11 || count ==12)
	{
		compute_fl_length = 1;
	}
	else
	{
		compute_fl_length = 0;
	}
	return compute_fl_length;
}*/

/*16bit*/

int compute_fl(int count)
{
	int compute_fl = 0;
	/*if (count == 11)//����
	{
		compute_fl = 2;
	}
	else if (count == 2 || count == 3 || count == 4 || count == 6)//����
	{
		compute_fl = 1;
	}
	else
	{
		compute_fl = 0;
	}*/
	compute_fl = fl[count];
	return compute_fl;
}

int compute_fl_length(int count)
{
	int compute_fl_length = 0;
	/*if (count == 2)//����
	{
		compute_fl_length = 2;
	}
	else if (count == 3 || count == 4 || count == 6 || count == 11)
	{
		compute_fl_length = 1;
	}
	else
	{
		compute_fl_length = 0;
	}*/
	compute_fl_length = fl_length[count];
	return compute_fl_length;
}