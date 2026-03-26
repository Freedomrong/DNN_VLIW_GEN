#include<iostream>
#include<vliw.h>
#include <string>
#include<base_math.h>
#include<stdio.h>
#include<connect.h>
using namespace std;
//std::string VLIW_1024bit(

std::string VLIW_1024bit(int count, int m, int n, int k, int conv_num)
{
	int GlobalBuffer_DDR_enable;
	long long GlobalBuffer_DDR_source_address;
	long long GlobalBuffer_DDR_aim_address;
	int GlobalBuffer_DDR_length;

	int GlobalBuffer_enable;
	int GlobalBuffer_WaitMMU;
	long long GlobalBuffer_source_address;
	int GlobalBuffer_length;

	int WeightCache_DDR_enable;
	long long WeightCache_DDR_source_address;
	long long WeightCache_DDR_aim_address;
	int WeightCache_DDR_length;

	int WeightCache_enable;
	int WeightCache_WaitMMU;
	long long WeightCache_source_address;
	int WeightCache_length;

	int Psum_enable;
	int Psum_next;

	int Bias_enable;
	int Bias_source_address;

	int Compute_Result_enable;
	long long Compute_Result_source_address;
	int Compute_Result_length;

	int Global_CMD_Quantization_LeftOrRight;
	int Global_CMD_Quantization_length;
	int Global_CMD_total;
	int Global_CMD_pooling;
	int Global_CMD_pooling_length;
	int Global_CMD_upsampling;
	int Global_CMD_pad;
	int Global_CMD_pad_type;
	int Global_CMD_width;
	int Global_CMD_high;
	int Global_CMD_kernel;
	int Global_CMD_stride;
	int Global_CMD_fmapCache_cmd_valid;
	int Global_CMD_weight_rd_addr_cmd_base;

	int Psum_module_enable;
	int Bias_module_enable;

	int another_pad;
	int First_layer;

	int fl;
	int fl_length;

	int PCIE_START;

	int RELU_NOT;

	int Bias_addr_first = Count_Bias_addr_first(conv_num);///////������ȡ
	long long compute_result_first = Count_compute_result_first(conv_num); ////////������ȡ
	long long DDR_Globalbuffer_first = Count_DDR_Globalbuffer_first(conv_num);//////������ȡ
	//long long DDR_Globalbuffer_first = 0x0FF000000;
	long long DDR_WeightCacheBuffer_first = Count_DDR_WeightCacheBuffer_first(conv_num);////////������ȡ
	long long GlobalBuffer_first = Count_GlobalBuffer_first(conv_num);
	// long long WeightCache_first = 0x100100000;
	// long long PrePsumBuffer_first = 0x102000000;
	// long long BiasBuffer_first = 0x106000000;
	// long long quiteBuffer_first = 0x004000000;

	long long WeightCache_first = 0x0c2000000;
	// long long PrePsumBuffer_first = 0x0c4000000;
	// long long BiasBuffer_first = 0x0c6000000;
	// long long quiteBuffer_first = 0x0c8000000;

	int fl_data = compute_fl(conv_num);
	int fl_length_data = compute_fl_length(conv_num);
	// int filters_size[14] = { 1, 3, 3, 3, 3, 3, 3, 1, 3, 1, 1, 3, 3, 1 };
	// int original[14] = { 1, 16, 32, 64, 128, 256, 512, 1024, 256, 512, 256, 128, 256, 256 };
	// int kstorge[14] = { 256, 128, 64, 32, 16, 8, 8, 8, 8, 8, 8, 16, 16, 16 };
	// int result_back[14] = { 128, 64, 32, 16, 8, 8, 8, 8, 8, 8, 16, 16, 16, 16 };
	// int quantization[14] = { 16,16,15,16,15,15,15,16,16,16,16,18,18,18 };


	// 新模型参数 - 768x768x1 输入, 共14层卷积 (0~13)
	// Conv11拆分为Conv11a(处理Conv10输出128ch)和Conv11b(处理Conv4输出256ch)
	// 【重要】使用输入优先模式 (input_tile = count/n)，和256x256模型一致
	int filters_size[14] = { 1, 3, 3, 3, 3, 3, 3, 1, 3, 1, 1, 3, 3, 1 };
	int original[14] = { 1, 16, 32, 64, 128, 256, 512, 1024, 256, 512, 256, 128, 256, 256 };
	//                   C0  C1  C2  C3  C4   C5   C6   C7    C8   C9  C10 C11a C11b C12
	int kstorge[14] = { 768, 384, 192, 96, 48, 24, 24, 24, 24, 24, 24, 48, 48, 48 };
	int result_back[14] = { 384, 192, 96, 48, 24, 24, 24, 24, 24, 24, 48, 48, 48, 48 };
	// quantization = 数据1 + 数据2 - 数据3
	//int quantization[14] = { 15, 16, 17, 16, 15, 15, 16, 15, 16, 17, 16, 16, 15, 18 };

	int quantization[14] = { 15, 16, 17, 16, 
		                     15, 15, 16, 15, 
							 16, 17, 16, 16, 
							 15, 18 };   // 倒数第三个16，不确定是怎么来的

	// ////////////////////////////////////////////////////////////////////////////////////////////
	// // Layer1 (conv_num=1) slice helper
	// int l1_tile_h = 0;
	// int l1_start_row = 0;

	// if (conv_num == 1) {
	// 	// Layer1 (conv_num=1) slice helper
	// 	l1_tile_h = (count == 1 || count == 12) ? 33 : 34;
	// 	l1_start_row = (count == 1) ? 0 : ((count == 12) ? 351 : ((count - 1) * 32 - 1));
	// } else {
	// 	l1_tile_h = 0;
	// 	l1_start_row = 0;
	// }

	//地址类型：
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////
	//1.从DDR中读fmap进入GlobalBuffer
	// 【重要】使用输入优先模式: input_tile = (count-1) / n, output_tile = (count-1) % n
	// 和256x256模型保持一致
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////
	if (conv_num == 0) {
		GlobalBuffer_DDR_enable = 1; 
		GlobalBuffer_DDR_source_address = DDR_Globalbuffer_first + (long long)(count - 1) * 16 * 768 * 64;
		// GlobalBuffer_DDR_source_address = DDR_Globalbuffer_first; // BUG FIX: Retain offset calculation
		GlobalBuffer_DDR_aim_address = GlobalBuffer_first;
		GlobalBuffer_DDR_length = 16 * 768; // Fits in SRAM
	}
	else if (conv_num == 1) {
		// GlobalBuffer_DDR_enable = 1;
		// GlobalBuffer_DDR_source_address = DDR_Globalbuffer_first + (long long)l1_start_row * 384 * 64;
		// GlobalBuffer_DDR_aim_address = GlobalBuffer_first;
		// GlobalBuffer_DDR_length = l1_tile_h * 384;

		if (count == 1) {
			GlobalBuffer_DDR_enable = 1;
			GlobalBuffer_DDR_source_address = DDR_Globalbuffer_first;							//3x3的卷积核必须一次读入
			GlobalBuffer_DDR_aim_address = GlobalBuffer_first;
			GlobalBuffer_DDR_length = 384*31;  //per 64Bytes 384*31   //需要pad=1, 31+pad， pad只补充在最开始一层
		}
		else if (count == 2)
		{
			GlobalBuffer_DDR_enable = 1;
			GlobalBuffer_DDR_source_address = DDR_Globalbuffer_first + 29 * 384 * 64;   // 这个29是怎么计算的，0-29，刚好3x3，这次从30开始取
			GlobalBuffer_DDR_aim_address = GlobalBuffer_first;
			GlobalBuffer_DDR_length = 384*32;  //per 64Bytes 128*32 
		}
		else if (count == 3)
		{
			GlobalBuffer_DDR_enable = 1;
			GlobalBuffer_DDR_source_address = DDR_Globalbuffer_first + 59 * 384 * 64;   // 这个29是怎么计算的，0-29，刚好3x3，这次从30开始取
			GlobalBuffer_DDR_aim_address = GlobalBuffer_first;
			GlobalBuffer_DDR_length = 384*32;  //per 64Bytes 128*32 
		}
		// else if (count == 4 || count == 5 || count == 6 || count == 7 || count == 8 || count == 9 || count == 10)
		else if (count >= 4 && count <= 12)
		{
			GlobalBuffer_DDR_enable = 1;
			GlobalBuffer_DDR_source_address = DDR_Globalbuffer_first + (29 + (count - 2) * 30) * 384 * 64;   // 这个29是怎么计算的，0-29，刚好3x3，这次从30开始取
			GlobalBuffer_DDR_aim_address = GlobalBuffer_first;
			GlobalBuffer_DDR_length = 384*32;  //per 64Bytes 128*32 
			// 89, 119, 149
		}
		else if (count == 13) // 
		{
			GlobalBuffer_DDR_enable = 1;
			GlobalBuffer_DDR_source_address = DDR_Globalbuffer_first + (29 + (count - 2) * 30) * 384 * 64;   // 这个29是怎么计算的，0-29，刚好3x3，这次从30开始取
			GlobalBuffer_DDR_aim_address = GlobalBuffer_first;
			GlobalBuffer_DDR_length = 384*(384 - 359);  //per 64Bytes 128*32 
			// 89, 119, 149
		}
		
	}
	else if (conv_num == 2)
	{
		// // 先加载图像切片，A/B两组输出通道复用同一片输入数据。
		// int channel_step = (count - 1) % 2 + 1; // 1, 2
		
		// if (channel_step == 1) { // 仅在处理第1组通道时搬运新图像切片
		// 	GlobalBuffer_DDR_enable = 1; 
		// 	GlobalBuffer_DDR_source_address = DDR_Globalbuffer_first + (long long)l2_start_row * 192 * 64;
		// 	GlobalBuffer_DDR_aim_address = GlobalBuffer_first;
		// 	GlobalBuffer_DDR_length = l2_tile_h * 192;
		// } else {
		// 	GlobalBuffer_DDR_enable = 0;
		// 	GlobalBuffer_DDR_source_address = 0;
		// 	GlobalBuffer_DDR_aim_address = 0;
		// 	GlobalBuffer_DDR_length = 0;
		// }
		if (count == 1) {
			GlobalBuffer_DDR_enable = 1;
			GlobalBuffer_DDR_source_address = DDR_Globalbuffer_first;							//3x3的卷积核必须一次读入
			GlobalBuffer_DDR_aim_address = GlobalBuffer_first;
			GlobalBuffer_DDR_length = 192*63;  //per 64Bytes 384*31   //需要pad=1, 31+pad， pad只补充在最开始一层
		}
		else if (count == 2 || count == 4 || count == 6 || count == 8)
		{
			GlobalBuffer_DDR_enable = 0;
			GlobalBuffer_DDR_source_address = 0;   // 这个29是怎么计算的，0-29，刚好3x3，这次从30开始取
			GlobalBuffer_DDR_aim_address = GlobalBuffer_first;
			GlobalBuffer_DDR_length = 0;  //per 64Bytes 128*32 
		}
		else if (count == 3)
		{
			GlobalBuffer_DDR_enable = 1;
			GlobalBuffer_DDR_source_address = DDR_Globalbuffer_first + 61 * 192 * 64;   // 这个29是怎么计算的，0-29，刚好3x3，这次从30开始取
			GlobalBuffer_DDR_aim_address = GlobalBuffer_first;
			GlobalBuffer_DDR_length = 192*64;  //per 64Bytes 128*32 
		}
		else if (count == 5)
		{
			GlobalBuffer_DDR_enable = 1;
			GlobalBuffer_DDR_source_address = DDR_Globalbuffer_first + (61 + 62) * 192 * 64;   // 这个29是怎么计算的，0-29，刚好3x3，这次从30开始取
			GlobalBuffer_DDR_aim_address = GlobalBuffer_first;
			GlobalBuffer_DDR_length = 192*64;  //per 64Bytes 128*32 
		}
		else if (count == 7)
		{
			GlobalBuffer_DDR_enable = 1;
			GlobalBuffer_DDR_source_address = DDR_Globalbuffer_first + (61 + 62 + 62) * 192 * 64;   // 这个29是怎么计算的，0-29，刚好3x3，这次从30开始取
			GlobalBuffer_DDR_aim_address = GlobalBuffer_first;
			GlobalBuffer_DDR_length = 192*(192 - 61 - 62 - 62);  //per 64Bytes 128*32 
		}
		
	}
	else if (conv_num == 3) // 96x96x64 -> 48x48x128
	{
		// 首先psum buffer，把96行拆分成64 + 32两个输出条带
		// 每个条带分别跑 m*n（即每条带 8 条，总共 16 条）

		// // block 1 : 64x 96 
		// if (count == 1) {
		// 	GlobalBuffer_DDR_enable = 1;
		// 	GlobalBuffer_DDR_source_address = DDR_Globalbuffer_first;							//3x3的卷积核必须一次读入
		// 	GlobalBuffer_DDR_aim_address = GlobalBuffer_first;
		// 	GlobalBuffer_DDR_length = 96*63;  //per 64Bytes 384*31   //需要pad=1, 31+pad， pad只补充在最开始一层
		// }
		// else if (count == 2)
		// {
		// 	GlobalBuffer_DDR_enable = 1;
		// 	GlobalBuffer_DDR_source_address = DDR_Globalbuffer_first + 96 * 96 * 64;   // 这个29是怎么计算的，0-29，刚好3x3，这次从30开始取
		// 	// ddr ： 所有像素的前32 ，所有像素的后32通道存； 这一条，后32通道，所有要加像素的前32作为偏移
		// 	GlobalBuffer_DDR_aim_address = GlobalBuffer_first;
		// 	GlobalBuffer_DDR_length = 96 * 63;  
		// }  // 1 + 2
		
		// // block 1， 第二个卷积核， ...
		// if (count == 3 || count == 5 || count == 7) {
		// 	GlobalBuffer_DDR_enable = 1;
		// 	GlobalBuffer_DDR_source_address = DDR_Globalbuffer_first;							//3x3的卷积核必须一次读入
		// 	GlobalBuffer_DDR_aim_address = GlobalBuffer_first;
		// 	GlobalBuffer_DDR_length = 96*63;  //per 64Bytes 384*31   //需要pad=1, 31+pad， pad只补充在最开始一层
		// }
		// else if (count == 4 || count == 6 || count == 8)
		// {
		// 	GlobalBuffer_DDR_enable = 1;
		// 	GlobalBuffer_DDR_source_address = DDR_Globalbuffer_first + 96 * 96 * 64;   // 这个29是怎么计算的，0-29，刚好3x3，这次从30开始取
		// 	// ddr ： 所有像素的前32 ，所有像素的后32通道存； 这一条，后32通道，所有要加像素的前32作为偏移
		// 	GlobalBuffer_DDR_aim_address = GlobalBuffer_first;
		// 	GlobalBuffer_DDR_length = 96 * 63;  
		// }  // 1 + 2

		// block 1 : 64x 96 
		if (count == 1) {
			GlobalBuffer_DDR_enable = 1;
			GlobalBuffer_DDR_source_address = DDR_Globalbuffer_first;							//3x3的卷积核必须一次读入
			GlobalBuffer_DDR_aim_address = GlobalBuffer_first;
			GlobalBuffer_DDR_length = 96*63;  //per 64Bytes 384*31   //需要pad=1, 31+pad， pad只补充在最开始一层
		}
		else if (count == 2)
		{
			GlobalBuffer_DDR_enable = 1;
			GlobalBuffer_DDR_source_address = DDR_Globalbuffer_first + 96 * 96 * 64;   // 这个29是怎么计算的，0-29，刚好3x3，这次从30开始取
			// ddr ： 所有像素的前32 ，所有像素的后32通道存； 这一条，后32通道，所有要加像素的前32作为偏移
			// GlobalBuffer_DDR_aim_address = GlobalBuffer_first + 96 * 63 * 64;
			GlobalBuffer_DDR_aim_address = GlobalBuffer_first + 0x80000;
			GlobalBuffer_DDR_length = 96 * 63;  
		}  // 1 + 2
		
		if (count >= 3 && count <=8) {
			GlobalBuffer_DDR_enable = 0;
			GlobalBuffer_DDR_source_address = 0;
			GlobalBuffer_DDR_aim_address = 0;
			GlobalBuffer_DDR_length = 0;  
		}
		

		// ------------------------------------------------
		// block 2
		if (count == 9) {
			GlobalBuffer_DDR_enable = 1;
			GlobalBuffer_DDR_source_address = DDR_Globalbuffer_first + 61 * 96 * 64;							//3x3的卷积核必须一次读入
			GlobalBuffer_DDR_aim_address = GlobalBuffer_first;
			GlobalBuffer_DDR_length = 96*(96 - 61);  //per 64Bytes 384*31   //需要pad=1, 31+pad， pad只补充在最开始一层
		}
		else if (count == 10)
		{
			GlobalBuffer_DDR_enable = 1;
			GlobalBuffer_DDR_source_address = DDR_Globalbuffer_first + (96 * 96 * 64) + 61 * 96 * 64;   // 这个29是怎么计算的，0-29，刚好3x3，这次从30开始取
			// ddr ： 所有像素的前32 ，所有像素的后32通道存； 这一条，后32通道，所有要加像素的前32作为偏移
			// GlobalBuffer_DDR_aim_address = GlobalBuffer_first + + 96 * (96 - 61) * 64;
			GlobalBuffer_DDR_aim_address = GlobalBuffer_first + 0x80000;
			GlobalBuffer_DDR_length = 96 * (96 - 61);  
		}  // 1 + 2

		if (count >= 11 && count <=16) {
			GlobalBuffer_DDR_enable = 0;
			GlobalBuffer_DDR_source_address = 0;
			GlobalBuffer_DDR_aim_address = 0;
			GlobalBuffer_DDR_length = 0;  
		}
	}
	else if (conv_num == 4)
	{
		// block 1 : 48 * 48 * 128 
		if (count == 1) {
			GlobalBuffer_DDR_enable = 1;
			GlobalBuffer_DDR_source_address = DDR_Globalbuffer_first;							//3x3的卷积核必须一次读入
			GlobalBuffer_DDR_aim_address = GlobalBuffer_first;
			GlobalBuffer_DDR_length = 48 * 48 * 4;  //per 64Bytes 384*31   //需要pad=1, 31+pad， pad只补充在最开始一层
		}
		else if (count >= 33) { // 每次只读需要的数据
			GlobalBuffer_DDR_enable = 1;
			GlobalBuffer_DDR_source_address = 0xa7000000 + (long long)(count - 33) * 48 * 48 * 64;							//3x3的卷积核必须一次读入
			GlobalBuffer_DDR_aim_address = GlobalBuffer_first;
			GlobalBuffer_DDR_length = 48 * 48;  //per 64Bytes 384*31 
		}
		else {
			GlobalBuffer_DDR_enable = 0;
			GlobalBuffer_DDR_source_address = 0;							//3x3的卷积核必须一次读入
			GlobalBuffer_DDR_aim_address = 0;
			GlobalBuffer_DDR_length = 0;
		}
	}

	else if (conv_num == 5)
	{
		int channel_in_step = (count - 1) % m + 1; 
		
		if (conv_num == 4 && count > 32) // 池化阶段：需要开启DDR从显存重新加载中间结果到SRAM
		{
			GlobalBuffer_DDR_enable = 1;
			GlobalBuffer_DDR_source_address = 0x003000000 + (long long)(count - 33) * 48 * 48 * 64;
			GlobalBuffer_DDR_length = 48 * 48;
		}
		else
		{
			GlobalBuffer_DDR_enable = 1; 
			if (conv_num == 3) {
				GlobalBuffer_DDR_source_address = DDR_Globalbuffer_first + (long long)(channel_in_step - 1) * 96 * 96 * 64;
				GlobalBuffer_DDR_length = 96 * 96; 
			} else if (conv_num == 4) {
				GlobalBuffer_DDR_source_address = DDR_Globalbuffer_first + (long long)(channel_in_step - 1) * 48 * 48 * 64;
				GlobalBuffer_DDR_length = 48 * 48; 
			}
		}
		GlobalBuffer_DDR_aim_address = GlobalBuffer_first;
	}
	else if (conv_num == 5)
	{
		GlobalBuffer_DDR_enable = 0; // 第6层特征图全在片上，无需DDR搬运
		GlobalBuffer_DDR_source_address = 0;
		GlobalBuffer_DDR_aim_address = 0;
		GlobalBuffer_DDR_length = 0;
	}
	

	///////////////////////////////////////////////////////////////////////////////////////////////////////////////
	//2.从GlobalBuffer中读fmap进入Compute
	// 如果你在同一条指令中，既要从 DDR 搬数据到 SRAM，又要用 SRAM 的数据去计算（处理同一个 Tile），那么计算引擎（Compute）必须等待数据从 DDR 彻底搬完才能开始算，否则就会读到错误/陈旧的数据（导致计算结果全错）。 这就引出了 GlobalBuffer_WaitMMU 的作用：它是一个硬件同步控制标志位（Sync Flag）。
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////
	GlobalBuffer_enable = 1;
	if (conv_num == 0)
	{
		GlobalBuffer_WaitMMU = 1;   // 是否需要等待 DDR 数据搬运完成（再进行下一步计算）。当数据已经在片内缓存时：(WaitMMU = 0)
		GlobalBuffer_source_address = GlobalBuffer_first;
		GlobalBuffer_length = 16 * 768;
	}
	else if (conv_num == 1)
	{
		// GlobalBuffer_WaitMMU = 1;   // 是否需要等待 DDR 数据搬运完成（再进行下一步计算）。当数据已经在片内缓存时：(WaitMMU = 0)
		// GlobalBuffer_source_address = GlobalBuffer_first;
		// GlobalBuffer_length = l1_tile_h * 384;

		if (count == 1) {
			GlobalBuffer_WaitMMU = 1;
			GlobalBuffer_source_address = GlobalBuffer_first;
			GlobalBuffer_length = 384*31;
		}
		else if (count >= 2 && count <= 12) {
			GlobalBuffer_WaitMMU = 1;
			GlobalBuffer_source_address = GlobalBuffer_first;
			GlobalBuffer_length = 384*32;
		}
		else if (count == 13) {
			GlobalBuffer_WaitMMU = 1;
			GlobalBuffer_source_address = GlobalBuffer_first;
			GlobalBuffer_length = 384*(384 - 359);
		}
	}
	else if (conv_num == 2)
	{
		// {
		// 	int channel_step = (count - 1) % 2 + 1;
		// 	if (channel_step == 1) {
		// 		GlobalBuffer_WaitMMU = 1; // 等待图像切片搬运完成
		// 	} else {
		// 		GlobalBuffer_WaitMMU = 0; // 图像已经在SRAM中，直接算另外一半通道
		// 	}
		// 	GlobalBuffer_source_address = GlobalBuffer_first;
		// 	GlobalBuffer_length = l2_tile_h * 192;
		// }
		if (count == 1) {
			GlobalBuffer_WaitMMU = 1;
			GlobalBuffer_source_address = GlobalBuffer_first;
			GlobalBuffer_length = 192*63;
		}
		else if (count == 2) {
			GlobalBuffer_WaitMMU = 0;
			GlobalBuffer_source_address = GlobalBuffer_first;
			GlobalBuffer_length = 192*63;
		}
		else if (count == 3 || count == 5) {
			GlobalBuffer_WaitMMU = 1;
			GlobalBuffer_source_address = GlobalBuffer_first;
			GlobalBuffer_length = 192*64;
		}
		else if (count == 4 || count == 6) {
			GlobalBuffer_WaitMMU = 0;
			GlobalBuffer_source_address = GlobalBuffer_first;
			GlobalBuffer_length = 192*64;
		}
		else if (count == 7) {
			GlobalBuffer_WaitMMU = 1;
			GlobalBuffer_source_address = GlobalBuffer_first;
			GlobalBuffer_length = 192*(192 - 61 - 62 - 62);
		}
		else if (count == 8) {
			GlobalBuffer_WaitMMU = 0;
			GlobalBuffer_source_address = GlobalBuffer_first;
			GlobalBuffer_length = 192*(192 - 61 - 62 - 62);
		}
	}
	else if (conv_num == 3)
	{
		// GlobalBuffer_WaitMMU = 1; // 每次读取都有新的输出图像，必须等
		// GlobalBuffer_source_address = GlobalBuffer_first;
		// GlobalBuffer_length = 96 * 96;

		// block 1
		if (count == 1) {
			GlobalBuffer_WaitMMU = 1;
			GlobalBuffer_source_address = GlobalBuffer_first;
			GlobalBuffer_length = 96*63;
		}
		else if (count == 2) {
			GlobalBuffer_WaitMMU = 1;
			// GlobalBuffer_source_address = GlobalBuffer_first + 96 * 63 * 64;  // GlobalBuffer_DDR_aim_address
			GlobalBuffer_source_address = GlobalBuffer_first + 0x80000;

			GlobalBuffer_length = 96 * 63;
		}
		else if (count == 3 || count == 5 || count == 7) {
			GlobalBuffer_WaitMMU = 0;
			GlobalBuffer_source_address = GlobalBuffer_first;
			GlobalBuffer_length = 96*63;
		}
		else if (count == 4 || count == 6 || count == 8) {
			GlobalBuffer_WaitMMU = 0;
			// GlobalBuffer_source_address = GlobalBuffer_first + 96 * 63 * 64;
			GlobalBuffer_source_address = GlobalBuffer_first + 0x80000;
			GlobalBuffer_length = 96 * 63;
		}

		// block 2
		if (count == 9) {
			GlobalBuffer_WaitMMU = 1;
			GlobalBuffer_source_address = GlobalBuffer_first;
			GlobalBuffer_length = 96*(96 - 61);
		}
		else if (count == 10) {
			GlobalBuffer_WaitMMU = 1;
			// GlobalBuffer_source_address = GlobalBuffer_first + 96 * (96 - 61) * 64;
			GlobalBuffer_source_address = GlobalBuffer_first + 0x80000;
			GlobalBuffer_length = 96 * (96 - 61);
		}
		else if (count == 11 || count == 13 || count == 15) {
			GlobalBuffer_WaitMMU = 0;
			GlobalBuffer_source_address = GlobalBuffer_first;
			GlobalBuffer_length = 96*(96 - 61);
		}
		else if (count == 12 || count == 14 || count == 16) {
			GlobalBuffer_WaitMMU = 0;
			// GlobalBuffer_source_address = GlobalBuffer_first + 96 * (96 - 61) * 64;
			GlobalBuffer_source_address = GlobalBuffer_first + 0x80000;
			GlobalBuffer_length = 96 * (96 - 61);
		}
	}
	else if (conv_num == 4)
	{
		// if (count > 32)
		// {
		// 	GlobalBuffer_WaitMMU = 1; // 必须等待DDR加载完成
		// 	// 从SRAM中刚才DDR载入的位置读取
		// 	GlobalBuffer_source_address = GlobalBuffer_first;
		// 	GlobalBuffer_length = 2304; // 48x48
		// }
		// else
		// {
		// 	GlobalBuffer_WaitMMU = 1; 
		// 	GlobalBuffer_source_address = GlobalBuffer_first;
		// 	GlobalBuffer_length = 48 * 48;
		// }

		// if (count == 1 || count == 5) {
		if (count <= 32) {
			if (count == 1) {
				GlobalBuffer_WaitMMU = 1;
			}
			else {
				GlobalBuffer_WaitMMU = 0; // 前32条指令，输入图像在SRAM中，直接算下一条
			}
			if (count % 4 == 1) {
				GlobalBuffer_source_address = GlobalBuffer_first;
				GlobalBuffer_length = 48*48;
			}
			else if (count % 4 == 2) {
				GlobalBuffer_source_address = GlobalBuffer_first + 48*48*64;
				GlobalBuffer_length = 48*48;
			}
			else if (count % 4 == 3) {
				GlobalBuffer_source_address = GlobalBuffer_first + 48*48*64 * 2;
				GlobalBuffer_length = 48*48;
			}
			else if (count % 4 == 0) {
				GlobalBuffer_source_address = GlobalBuffer_first + 48*48*64 * 3;
				GlobalBuffer_length = 48*48;
			}
		}
		else { // > 33条指令
			GlobalBuffer_WaitMMU = 1;
			GlobalBuffer_source_address = GlobalBuffer_first;
			GlobalBuffer_length = 48*48;
		}
		
	}
	else if (conv_num == 5)
	{
		GlobalBuffer_WaitMMU = 0;
		GlobalBuffer_source_address = GlobalBuffer_first;
		GlobalBuffer_length = 576;
	}

	///////////////////////////////////////////////////////////////////////////////////////////////////////////////
	//3.从WeightCache中读fmap进入Compute
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////
	if (conv_num == 0) // Layer 0: 1x1卷积，每条指令都搬权重（与256_ok一致）
	{
		WeightCache_DDR_enable = 1;
		WeightCache_DDR_source_address = DDR_WeightCacheBuffer_first;
		WeightCache_DDR_aim_address = WeightCache_first;
		WeightCache_DDR_length = 32; // 1x1x32 = 32 (单个tile的权重切面)
	}
	else if (conv_num == 1)  // Layer 1: 3x3卷积核
	{
		// if (count == 1) {  // 第一条指令，从DDR读权重
		// 	WeightCache_DDR_enable = 1;
		// 	WeightCache_DDR_source_address = DDR_WeightCacheBuffer_first;
		// 	WeightCache_DDR_aim_address = WeightCache_first;
		// 	WeightCache_DDR_length = 288; // 3*3*32
		// } else {
		// 	WeightCache_DDR_enable = 0;
		// 	WeightCache_DDR_source_address = 0;
		// 	WeightCache_DDR_aim_address = 0;
		// 	WeightCache_DDR_length = 0;
		// }

		WeightCache_DDR_enable = 1;
		WeightCache_DDR_source_address = DDR_WeightCacheBuffer_first;
		WeightCache_DDR_aim_address = WeightCache_first;
		WeightCache_DDR_length = 288; // 3*3*32
	}
	else if (conv_num == 2) // 3x3卷积核, n=2
	{
		if (count == 1) {  // 第一条指令，从DDR读权重
			WeightCache_DDR_enable = 1;
			WeightCache_DDR_source_address = DDR_WeightCacheBuffer_first;
			WeightCache_DDR_aim_address = WeightCache_first;
			WeightCache_DDR_length = 576; // 3*3*64
		} else {
			WeightCache_DDR_enable = 0;
			WeightCache_DDR_source_address = 0;
			WeightCache_DDR_aim_address = 0;
			WeightCache_DDR_length = 0;
		}
	}
	else if (conv_num == 3) // 3x3卷积核, n=4
	{
		// if (count == 1) {  // 第一条指令，从DDR读权重,全部读上来
		// 	WeightCache_DDR_enable = 1;
		// 	WeightCache_DDR_source_address = DDR_WeightCacheBuffer_first;
		// 	WeightCache_DDR_aim_address = WeightCache_first;
		// 	WeightCache_DDR_length = 1152; // 3*3*128
		// } else {
		// 	WeightCache_DDR_enable = 0;
		// 	WeightCache_DDR_source_address = 0;
		// 	WeightCache_DDR_aim_address = 0;
		// 	WeightCache_DDR_length = 0;
		// }

		// block 1 & 
		// block 2
		// if (count == 1 || count == 9) {  // 第一条指令，从DDR读权重,全部读上来
		// 	WeightCache_DDR_enable = 1;
		// 	WeightCache_DDR_source_address = DDR_WeightCacheBuffer_first;
		// 	WeightCache_DDR_aim_address = WeightCache_first;
		// 	WeightCache_DDR_length = 288; // 3*3*128*2
		// } 
		// else if (count == 2 || count == 10) {
		// 	WeightCache_DDR_enable = 1;
		// 	WeightCache_DDR_source_address = DDR_WeightCacheBuffer_first + (3*3*64) * 128;
		// 	WeightCache_DDR_aim_address = WeightCache_first;
		// 	WeightCache_DDR_length = 288; // 3*3*128*2
		// }
		// else if (count == 3 || count == 5 || count == 7 || count == 11 || count == 13 || count == 15) {
		// 	WeightCache_DDR_enable = 1;
		// 	WeightCache_DDR_source_address = DDR_WeightCacheBuffer_first + (3*3*64) * 32 * (count / 2);
		// 	WeightCache_DDR_aim_address = WeightCache_first;
		// 	WeightCache_DDR_length = 288; // 3*3*128*2
		// }
		// else if (count == 4 || count == 6 || count == 8 || count == 12 || count == 14 || count == 16) {
		// 	WeightCache_DDR_enable = 1;
		// 	WeightCache_DDR_source_address = (DDR_WeightCacheBuffer_first + (3*3*64) * 128) + (3*3*64) * 32 * ((count-1) / 2);
		// 	WeightCache_DDR_aim_address = WeightCache_first;
		// 	WeightCache_DDR_length = 288; // 3*3*128*2
		// }
		// 为了避免出错，我可以考虑，读需要的权重上来。不用一次性读那么多；
		// 根据以往的推断，权重在DDR的存储是，
		// filter 1 : 前32， filte 2: 前32， filter 3: 前32， filter 4: 前32
		// filter 1 : 后32， filte 2: 后32， filter 3: 后32， filter 4: 后32
		// 我们在第三层的执行流程是：
		// count 1: fmap @ filter 1(前32); count 2: fmap @ filter 1(后32) ；两者的结果累加输出, 结果写回。
		// count 3: fmap @ filter 2(前32); count 4: fmap @ filter 2(后32) ；两者的结果累加输出, 结果写回。
		// count 5: fmap @ filter 3(前32); count 6: fmap @ filter 3(后32) ；两者的结果累加输出, 结果写回。
		// count 7: fmap @ filter 4(前32); count 8: fmap @ filter 4(后32) ；两者的结果累加输出, 结果写回。

		// if (count == 1 || count == 9) {  // 第一条指令，从DDR读权重,全部读上来
		// 	WeightCache_DDR_enable = 1;
		// 	WeightCache_DDR_source_address = DDR_WeightCacheBuffer_first;
		// 	WeightCache_DDR_aim_address = WeightCache_first;
		// 	WeightCache_DDR_length = 288; // 3*3*128*2
		// } 
		// else if (count == 2 || count == 10) {
		// 	WeightCache_DDR_enable = 1;
		// 	WeightCache_DDR_source_address = DDR_WeightCacheBuffer_first + (3*3*64) * 128;
		// 	WeightCache_DDR_aim_address = WeightCache_first;
		// 	WeightCache_DDR_length = 288; // 3*3*128*2
		// }
		// else if (count == 3 || count == 5 || count == 7 || count == 11 || count == 13 || count == 15) {
		// 	WeightCache_DDR_enable = 1;
		// 	WeightCache_DDR_source_address = DDR_WeightCacheBuffer_first + (3*3*64) * 32 * ((count%8) / 2);
		// 	WeightCache_DDR_aim_address = WeightCache_first;
		// 	WeightCache_DDR_length = 288; // 3*3*128*2
		// }
		// else if (count == 4 || count == 6 || count == 8 || count == 12 || count == 14 || count == 16) {
		// 	WeightCache_DDR_enable = 1;
		// 	WeightCache_DDR_source_address = (DDR_WeightCacheBuffer_first + (3*3*64) * 128) + (3*3*64) * 32 * (((count%8)-1) / 2);
		// 	WeightCache_DDR_aim_address = WeightCache_first;
		// 	WeightCache_DDR_length = 288; // 3*3*128*2
		// }


		// -------------
		if (count == 1 || count == 9) {  // 第一条指令，从DDR读权重,全部读上来
			WeightCache_DDR_enable = 1;
			WeightCache_DDR_source_address = DDR_WeightCacheBuffer_first;
			WeightCache_DDR_aim_address = WeightCache_first;
			WeightCache_DDR_length = 288; // 3*3*128*2
		} 
		else if (count == 2 || count == 10) {
			WeightCache_DDR_enable = 1;
			WeightCache_DDR_source_address = DDR_WeightCacheBuffer_first + (3*3*64) * 128;
			WeightCache_DDR_aim_address = WeightCache_first;
			WeightCache_DDR_length = 288; // 3*3*128*2
		}
		else if (count == 3 || count == 5 || count == 7 || count == 11 || count == 13 || count == 15) {
			WeightCache_DDR_enable = 1;
			WeightCache_DDR_source_address = DDR_WeightCacheBuffer_first + (3*3*64) * 32 * ((count%8) / 2);
			WeightCache_DDR_aim_address = WeightCache_first;
			WeightCache_DDR_length = 288; // 3*3*128*2
		}
		else if (count == 4 || count == 6 || count == 12 || count == 14) {
			WeightCache_DDR_enable = 1;
			WeightCache_DDR_source_address = (DDR_WeightCacheBuffer_first + (3*3*64) * 128) + (3*3*64) * 32 * (((count%8)-1) / 2);
			WeightCache_DDR_aim_address = WeightCache_first;
			WeightCache_DDR_length = 288; // 3*3*128*2
		}
		else if (count == 8) {
			WeightCache_DDR_enable = 1;
			WeightCache_DDR_source_address = (DDR_WeightCacheBuffer_first + (3*3*64) * 128) + (3*3*64) * 32 * (3);
			WeightCache_DDR_aim_address = WeightCache_first;
			WeightCache_DDR_length = 288; // 3*3*128*2
		}
		else if (count == 16) {
			WeightCache_DDR_enable = 1;
			WeightCache_DDR_source_address = (DDR_WeightCacheBuffer_first + (3*3*64) * 128) + (3*3*64) * 32 * (3);
			WeightCache_DDR_aim_address = WeightCache_first;
			WeightCache_DDR_length = 288; // 3*3*128*2
		}


	}
	else if (conv_num == 4) // n=4 or n=8
	{
		if (count <= 32) {
			// if (count == 1) {  // 第一条指令，从DDR读权重,全部读上来
			// 	WeightCache_DDR_enable = 1;
			// 	WeightCache_DDR_source_address = DDR_WeightCacheBuffer_first;
			// 	WeightCache_DDR_aim_address = WeightCache_first;
			// 	WeightCache_DDR_length = 3*3*4*128; // 3*3*256
			// 	// 256 filter, 8次， 1个filter是3x3x128； 先读四个filter， 4x4= 16； 前16，读一次。后16读一次。
			// } 
			// else if (count == 17) {
			// 	WeightCache_DDR_enable = 1;
			// 	WeightCache_DDR_source_address = DDR_WeightCacheBuffer_first + 3*3*64*4*128;
			// 	WeightCache_DDR_aim_address = WeightCache_first;
			// 	WeightCache_DDR_length = 3*3*4*128; // 3*3*256
			// }
			// else {
			// 	WeightCache_DDR_enable = 0;
			// 	WeightCache_DDR_source_address = 0;
			// 	WeightCache_DDR_aim_address = 0;
			// 	WeightCache_DDR_length = 0;
			// }

			// if (count >= 1 && count <= 32) {
			const long long base_stride = 3 * 3 * 64;
			const long long block_stride = base_stride * 256; // 4路轮转步长
			const long long group_stride = base_stride * 32;  // 每4个count后向后平移

			WeightCache_DDR_enable = 1;
			WeightCache_DDR_source_address = DDR_WeightCacheBuffer_first
				+ block_stride * ((count - 1) % 4)
				+ group_stride * ((count - 1) / 4);
			WeightCache_DDR_aim_address = WeightCache_first;
			WeightCache_DDR_length = 3 * 3 * 32; // 3*3*256
			// }


		}
		else {
			WeightCache_DDR_enable = 0;
			WeightCache_DDR_source_address = 0;
			WeightCache_DDR_aim_address = 0;
			WeightCache_DDR_length = 0;
		}
	}
	else if (conv_num == 5)
	{
		int channel_in_step = (count - 1) % m + 1;
		if (channel_in_step == 1) {
			WeightCache_DDR_enable = 1;
			WeightCache_DDR_source_address = DDR_WeightCacheBuffer_first + (long long)((count - 1) / m) * 2304 * 64;
			WeightCache_DDR_aim_address = WeightCache_first;
			WeightCache_DDR_length = 2304; 
		} else {
			WeightCache_DDR_enable = 0;
			WeightCache_DDR_source_address = 0;
			WeightCache_DDR_aim_address = 0;
			WeightCache_DDR_length = 0;
		}
	}

	///////////////////////////////////////////////////////////////////////////////////////////////////////////////
	//4. Weight Cache Control
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// 待定
	if (filters_size[conv_num] == 3) // 3x3
	{
		if (WeightCache_DDR_enable == 1)
		{
			WeightCache_enable = 1;
			WeightCache_WaitMMU = 1;
			WeightCache_source_address = WeightCache_first;
			WeightCache_length = WeightCache_DDR_length;
		}
		else
		{
			WeightCache_enable = 0;
			WeightCache_WaitMMU = 0;
			WeightCache_source_address = 0;
			WeightCache_length = 0;
		}
	}
	else if (filters_size[conv_num] == 1)
	{
		if (conv_num == 0)
		{
			if (count == 1) {
				WeightCache_enable = 1;
				WeightCache_WaitMMU = 1;
				WeightCache_source_address = WeightCache_first;
				WeightCache_length = WeightCache_DDR_length;
			}
			else {
				WeightCache_enable = 0;
				WeightCache_WaitMMU = 0;
				WeightCache_source_address = 0;
				WeightCache_length = 0;
			}
		}
		else
		{
			if (count % n == 1)
			{
				WeightCache_enable = 1;
				WeightCache_WaitMMU = 1;
				WeightCache_source_address = WeightCache_first;
				WeightCache_length = WeightCache_DDR_length;
			}
			else
			{
				WeightCache_enable = 0;
				WeightCache_WaitMMU = 0;
				WeightCache_source_address = 0;
				WeightCache_length = 0;
			}
		}

	}

	///////////////////////////////////////////////////////////////////////////////////////////////////////////////
	//5. Psum Buffer ： 先算一半的通道，得到一个未完成的结果，这个结果就叫部分和（Partial Sum）。硬件把这个部分和存到专门的片上 Psum Buffer 里。
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// 待定
	if (conv_num == 0 || conv_num == 1 || conv_num == 2)
	{
		Psum_enable = 0;
	}
	else if (conv_num == 3)
	{
		// m = 2
		if (count % m == 0) { Psum_enable = 1; } // 当 count 为 2说明一个输出组（32个通道）的计算已经完成
		else { Psum_enable = 0; }  
	}
	else if (conv_num == 4)
	{
		if (conv_num == 4 && count > 32) { Psum_enable = 0; } // 池化阶段不累加
		else if (count % m == 1) { Psum_enable = 0; } //当 count 为 1, 5, 9... 时，说明正在计算每一个输出组的第一个输入块。新输入，不累加
		else { Psum_enable = 1; }

		// 1： 0， 2： 1， 3， 1，4， 1 
	}
	else
	{
		Psum_enable = 0;
	}

	if (conv_num == 0 || conv_num == 1 || conv_num == 2)
	{
		Psum_next = 1;
	} 
	else if (conv_num == 3)
	{
		if (count % m == 0) { Psum_next = 1; } // 结果写回DDR
		else { Psum_next = 0; }               
	}
	else if (conv_num == 4)  // 待定
	{
		if (count > 32) { Psum_next = 1; } // 池化结果独立推送
		else if (count % m == 0) { Psum_next = 1; } // 当 count 为 4, 8, 12... 时，说明一个输出组（32个通道）的计算已经完成
		else { Psum_next = 0; }
	}
	else if (conv_num == 5)
	{
		if (count % m == 0) { Psum_next = 1; } 
		else { Psum_next = 0; }
	}
	else
	{
		Psum_next = 0;
	}

	///////////////////////////////////////////////////////////////////////////////////////////////////////////////
	//6. Bias
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////
	if (conv_num == 0 || conv_num == 1)
	{
		Bias_enable = 1;
		Bias_source_address = Bias_addr_first;
	}
	else if (conv_num == 2)
	{
		// int channel_step = (count - 1) % 2 + 1; // 1, 2
		// Bias_enable = 1;
		// Bias_source_address = Bias_addr_first + channel_step - 1; 
		Bias_enable = 1;
		if (count == 1 || count == 3 || count == 5 || count == 7) {
			Bias_source_address = Bias_addr_first;
		}
		else if (count == 2 || count == 4 || count == 6 || count == 8) {
			Bias_source_address = Bias_addr_first + 1;
		}
		else {
			Bias_source_address = 0;
		}	
	}
	else if (conv_num == 3)
	{
		// if (count % m == 0) { // 最后一次算完才加Bias
		// 	Bias_enable = 1;
		// 	int channel_out_step = (count - 1) / m + 1; 
		// 	Bias_source_address = Bias_addr_first + channel_out_step - 1; 
		// } else {
		// 	Bias_enable = 0;
		// 	Bias_source_address = 0;
		// }

		// block 1
		if (count == 1 || count == 3 || count == 5 || count == 7) {
			Bias_enable = 0;
			Bias_source_address = 0;
		}
		else if (count == 2) {
			Bias_enable = 1;
			Bias_source_address = Bias_addr_first;
		}
		else if (count == 4) {
			Bias_enable = 1;
			Bias_source_address = Bias_addr_first + 1;
		}
		else if (count == 6) {
			Bias_enable = 1;
			Bias_source_address = Bias_addr_first + 2;
		}
		else if (count == 8) {
			Bias_enable = 1;
			Bias_source_address = Bias_addr_first + 3;
		}

		// block 2
		if (count == 9 || count == 11 || count == 13 || count == 15) {
			Bias_enable = 0;
			Bias_source_address = 0;
		}
		else if (count == 10) {
			Bias_enable = 1;
			Bias_source_address = Bias_addr_first;
		}
		else if (count == 12) {
			Bias_enable = 1;
			Bias_source_address = Bias_addr_first + 1;
		}
		else if (count == 14) {
			Bias_enable = 1;
			Bias_source_address = Bias_addr_first + 2;
		}
		else if (count == 16) {
			Bias_enable = 1;
			Bias_source_address = Bias_addr_first + 3;
		}
	}
	else if (conv_num == 4)
	{
		if (count > 32) { 
			Bias_enable = 0; 
			Bias_source_address = 0;
		}
		else {
			// if (count % 4 == 0) {
			// 	Bias_enable = 1;
			// 	int channel_out_step = (count - 1) / 4 + 1; 
			// 	Bias_source_address = Bias_addr_first + channel_out_step - 1;
			// }
			// else {
			// 	Bias_enable = 0;
			// 	Bias_source_address = 0;
			// }

			if (count % 4 == 0) {
				Bias_enable = 1;
				if (count == 4) {
					Bias_source_address = Bias_addr_first;
				}
				else if (count == 8) {
					Bias_source_address = Bias_addr_first + 1;
				}
				else if (count == 12) {
					Bias_source_address = Bias_addr_first + 2;
				}
				else if (count == 16) {
					Bias_source_address = Bias_addr_first + 3;
				}
				else if (count == 20) {
					Bias_source_address = Bias_addr_first + 4;
				}
				else if (count == 24) {
					Bias_source_address = Bias_addr_first + 5;
				}
				else if (count == 28) {
					Bias_source_address = Bias_addr_first + 6;
				}
				else if (count == 32) {
					Bias_source_address = Bias_addr_first + 7;
				}
			}
			else {
				Bias_enable = 0;
				Bias_source_address = 0;
			}


		}
	}
	else if (conv_num == 5)
	{
		if (count % m == 0) {
			Bias_enable = 1;
			int channel_out_step = (count - 1) / m + 1; 
			Bias_source_address = Bias_addr_first + channel_out_step - 1; 
		} else {
			Bias_enable = 0;
			Bias_source_address = 0;
		}
	}
	else
	{
		Bias_enable = 0;
		Bias_source_address = 0;
	}

	///////////////////////////////////////////////////////////////////////////////////////////////////////////////
	//7.Result Writeback to DDR
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////
	if (conv_num == 0)
	{
		Compute_Result_enable = 1;
		// Output is 384 wide, pool-height 9 (effective 8 with step 8)
		Compute_Result_source_address = compute_result_first + (long long)(count-1) * 8 * 384 * 64;
		Compute_Result_length = 8 * 384; 
	}
	else if (conv_num == 1)
	{
		// Compute_Result_enable = 1;
		// // 输入宽 384，每次扫描算 32 行步长。经过 2x2 pooling 后输出宽高减半。
		// // 所以单条指令输出的是: 宽 192，高 16 的特征块。
		// // Output is 384 wide, pool-height 9 (effective 8 with step 8)
		// Compute_Result_source_address = compute_result_first + (long long)(count-1) * 16 * 192 * 64;
		// Compute_Result_length = 16 * 192; 

		if (count >= 1 && count <= 12) 
		{
			Compute_Result_enable = 1;
			Compute_Result_source_address = compute_result_first + (count - 1) * 15 * 192 * 64;
			Compute_Result_length = 15 * 192;
		}
		else
		{
			Compute_Result_enable = 1;
			Compute_Result_source_address = compute_result_first + 12 * 15 * 192 * 64;
			Compute_Result_length = 12 * 192;
		}
	}
	else if (conv_num == 2)
	{
		//指令1 (切片1，组A)：group=0, slice=0 -> 写在 DDR 起点 [0 ~ 32行]。
		//指令2 (切片1，组B)：group=满图体积, slice=0 -> 跨越万水千山，从很远的地方（后一平面）开始写 [96*96的偏移起点 ~ 32行]。
		//指令3 (切片2，组A)：group=0, slice=32行体积 -> 完美地、无缝地接在了指令1写完的地方！继续写 [32行 ~ 64行]！
		//指令4 (切片2，组B)：group=满图体积, slice=32行体积 -> 完美地、无缝地接在了指令2写完的地方！
		//指令5 (切片3，组A)：group=0, slice=64行体积 -> 补齐了最下面的三十多行，A组全图拼装完毕！
		//指令6 (切片3，组B)：group=满图体积, slice=64行体积 -> 补齐最下面三十多行，B组全图拼装完毕！

		// Compute_Result_enable = 1;
		// int spatial_step = (count - 1) / 2 + 1; // 1, 2, 3
		// int channel_step = (count - 1) % 2 + 1; // 1, 2
		// // 步长 64 pooling 后高 32
		// long long slice_offset = (long long)(spatial_step - 1) * 32 * 96 * 64;
		// // B组通道存放点跨越 A 组全尺寸 (总高96 x 总宽96)
		// long long group_offset = (long long)(channel_step - 1) * 96 * 96 * 64;
		
		// Compute_Result_source_address = compute_result_first + group_offset + slice_offset;
		// Compute_Result_length = 32 * 96;

		// if (count >= 1 && count <= 6) 
		// {
		// 	Compute_Result_enable = 1;
		// 	Compute_Result_source_address = compute_result_first + (count - 1) * 31 * 96 * 64;
		// 	Compute_Result_length = 31 * 96;
		// }
		// else if (count == 7)
		// {
		// 	Compute_Result_enable = 1;
		// 	Compute_Result_source_address = compute_result_first + 6 * 31 * 96 * 64;
		// 	Compute_Result_length = 3 * 96;
		// }
		// else if (count == 8)
		// {
		// 	Compute_Result_enable = 1;
		// 	Compute_Result_source_address = compute_result_first + (6 * 31 * 96 * 64) + (3 * 96 * 64);
		// 	Compute_Result_length = 3 * 96;
		// }

		if (count == 1 || count == 3 || count == 5) {
			Compute_Result_enable = 1;
			Compute_Result_source_address = compute_result_first + (count / 2) * 31 * 96 * 64;
			Compute_Result_length = 31 * 96;
		}
		else if (count == 2 || count == 4 || count == 6) {
			Compute_Result_enable = 1;
			Compute_Result_source_address = compute_result_first + 96*96*64 + ((count - 1)/2) * 31 * 96 * 64;
			Compute_Result_length = 31 * 96;
		}
		else if (count == 7)
		{
			Compute_Result_enable = 1;
			Compute_Result_source_address = compute_result_first + 3 * 31 * 96 * 64;
			Compute_Result_length = 3 * 96;
		}
		else if (count == 8)
		{
			Compute_Result_enable = 1;
			Compute_Result_source_address = compute_result_first + 96*96*64 + (3 * 31 * 96 * 64);
			Compute_Result_length = 3 * 96;
		}
		
	}
	else if (conv_num == 3)
	{
		// block 1
		if (count == 1 || count == 3 || count == 5 || count == 7) 
		{
			Compute_Result_enable = 0;
			Compute_Result_source_address = 0;
			Compute_Result_length = 0;
		}
		else if (count == 2) {
			Compute_Result_enable = 1;
			Compute_Result_source_address = compute_result_first;
			Compute_Result_length = 31 * 48;
		} 
		else if (count == 4) 
		{
			Compute_Result_enable = 1;
			Compute_Result_source_address = compute_result_first + 48 * 48 * 64;
			Compute_Result_length = 31 * 48;
		}
		else if (count == 6) {
			Compute_Result_enable = 1;
			Compute_Result_source_address = compute_result_first + 48 * 48 * 64 * 2;
			Compute_Result_length = 31 * 48;
		} 
		else if (count == 8) {
			Compute_Result_enable = 1;
			Compute_Result_source_address = compute_result_first + 48 * 48 * 64 * 3;
			Compute_Result_length = 31 * 48;
		} 

		// block 2
		if (count == 9 || count == 11 || count == 13 || count == 15) 
		{
			Compute_Result_enable = 0;
			Compute_Result_source_address = 0;
			Compute_Result_length = 0;
		}
		else if (count == 10) {
			Compute_Result_enable = 1;
			Compute_Result_source_address = compute_result_first + 31 * 48 * 64;
			Compute_Result_length = (48 - 31) * 48;
		} 
		else if (count == 12) 
		{
			Compute_Result_enable = 1;
			Compute_Result_source_address = (compute_result_first + 48 * 48 * 64) + (31 * 48 * 64);
			Compute_Result_length = (48 - 31) * 48;
		}
		else if (count == 14) {
			Compute_Result_enable = 1;
			Compute_Result_source_address = (compute_result_first + 48 * 48 * 64 * 2) + (31 * 48 * 64);
			Compute_Result_length = (48 - 31) * 48;
		} 
		else if (count == 16) {
			Compute_Result_enable = 1;
			Compute_Result_source_address = (compute_result_first + 48 * 48 * 64 * 3) + (31 * 48 * 64);
			Compute_Result_length = (48 - 31) * 48;
		} 




		// if (count % m == 0) { // 最后一次算完才写回
		// 	Compute_Result_enable = 1;
		// 	int channel_out_step = (count - 1) / m + 1; // 1, 2, 3, 4
		// 	long long group_offset = (long long)(channel_out_step - 1) * 48 * 48 * 64;
		// 	Compute_Result_source_address = compute_result_first + group_offset;
		// 	Compute_Result_length = 48 * 48; // 经过 pooling 变为 48x48
		// } else {
		// 	Compute_Result_enable = 0;
		// 	Compute_Result_source_address = 0;
		// 	Compute_Result_length = 0;
		// }
	}
	else if (conv_num == 4)
	{
		if (count > 32) // 池化阶段：写回最终 24x24 结果到片上BRAM;
		{
			Compute_Result_enable = 1;
			Compute_Result_source_address = 0xc0000000 + 0x80000 + (count - 33) * 24 * 24 * 64; 
			Compute_Result_length = 24*24; // 48*48
			
			// int pool_out_step = count - 32; 
			// long long group_offset = (long long)(pool_out_step - 1) * 24 * 24 * 64;
			// Compute_Result_source_address = 0xc0000000 + group_offset; 
			// Compute_Result_length = 576; // 24*24
		}
		else if (count % 4 == 0) // 卷积阶段：结果暂存到 DDR 0x003000000
		{
			// Compute_Result_enable = 1;
			// int channel_out_step = (count - 1) / m + 1;  //  m = 4
			// long long group_offset = (long long)(channel_out_step - 1) * 48 * 48 * 64;
			// Compute_Result_source_address = compute_result_first + group_offset; 
			// Compute_Result_length = 2304; // 48*48

			if (count == 4) {
				Compute_Result_enable = 1;
				Compute_Result_source_address = compute_result_first; 
				Compute_Result_length = 48*48; // 48*48
			}
			else if (count == 8) {
				Compute_Result_enable = 1;
				Compute_Result_source_address = compute_result_first + 48 * 48 * 64; 
				Compute_Result_length = 48*48; // 48*48
			}
			else if (count == 12) {
				Compute_Result_enable = 1;
				Compute_Result_source_address = compute_result_first + 48 * 48 * 64 * 2; 
				Compute_Result_length = 48*48; // 48*48
			}
			else if (count == 16) {
				Compute_Result_enable = 1;
				Compute_Result_source_address = compute_result_first + 48 * 48 * 64 * 3; 
				Compute_Result_length = 48*48; // 48*48
			}
			else if (count == 20) {
				Compute_Result_enable = 1;
				Compute_Result_source_address = compute_result_first + 48 * 48 * 64 * 4; 
				Compute_Result_length = 48*48; // 48*48
			}
			else if (count == 24) {
				Compute_Result_enable = 1;
				Compute_Result_source_address = compute_result_first + 48 * 48 * 64 * 5; 
				Compute_Result_length = 48*48; // 48*48
			}
			else if (count == 28) {
				Compute_Result_enable = 1;
				Compute_Result_source_address = compute_result_first + 48 * 48 * 64 * 6; 
				Compute_Result_length = 48*48; // 48*48
			}
			else if (count == 32) {
				Compute_Result_enable = 1;
				Compute_Result_source_address = compute_result_first + 48 * 48 * 64 * 7; 
				Compute_Result_length = 48*48; // 48*48
			}

		}
		else
		{
			Compute_Result_enable = 0;
			Compute_Result_source_address = 0;
			Compute_Result_length = 0;
		}
	}
	else if (conv_num == 5)
	{
		if (count % m == 0)
		{
			Compute_Result_enable = 1;
			int channel_out_step = (count - 1) / m + 1; 
			long long group_offset = (long long)(channel_out_step - 1) * 24 * 24 * 64;
			Compute_Result_source_address = compute_result_first + group_offset; 
			Compute_Result_length = 576; 
		}
		else
		{
			Compute_Result_enable = 0;
		}
	}
	else
	{
		Compute_Result_enable = 0;
		Compute_Result_source_address = 0;
		Compute_Result_length = 0;
	}

	///////////////////////////////////////////////////////////////////////////////////////////////////////////////
	//8.Quantization
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////
	Global_CMD_Quantization_LeftOrRight = 1;
	Global_CMD_Quantization_length = quantization[conv_num];
	
	Global_CMD_total = 1;


	///////////////////////////////////////////////////////////////////////////////////////////////////////////////
	//9.pooling
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////
	Global_CMD_pooling = 0;
	if (conv_num == 0 || conv_num == 1 || conv_num == 2)
	{
		Global_CMD_pooling = 1;
		Global_CMD_pooling_length = 1; // 2x2 pooling
	}
	else if (conv_num == 3)
	{
		// if (count % m == 0) {
		// 	Global_CMD_pooling = 1;
		// 	Global_CMD_pooling_length = 1;
		// } else {
		// 	Global_CMD_pooling = 0;
		// 	Global_CMD_pooling_length = 0;
		// }
		// block 1
		if (count == 1 || count == 3 || count == 5 || count == 7) 
		{
			Global_CMD_pooling = 0;
			Global_CMD_pooling_length = 0; // 2x2 pooling
		}
		else if (count == 2 || count == 4 || count == 6 || count == 8) {
			Global_CMD_pooling = 1;
			Global_CMD_pooling_length = 1; // 2x2 pooling
		} 
		

		// block 2
		if (count == 9 || count == 11 || count == 13 || count == 15) 
		{
			Global_CMD_pooling = 0;
			Global_CMD_pooling_length = 0; // 2x2 pooling
		}
		else if (count == 10 || count == 12 || count == 14 || count == 16) {
			Global_CMD_pooling = 1;
			Global_CMD_pooling_length = 1; // 2x2 pooling
		} 
	}
	else if (conv_num == 4)
	{
		if (count > 32) {
			Global_CMD_pooling = 1;
			Global_CMD_pooling_length = 1;
		} else {
			Global_CMD_pooling = 0;
			Global_CMD_pooling_length = 0;
		}
	}
	else {
		Global_CMD_pooling = 0;
		Global_CMD_pooling_length = 0;
	}
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////
	//10.upsampling
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////
	Global_CMD_upsampling = 0;
	
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////
	//12.PAD： [上，左，右，下]
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////
	if (filters_size[conv_num] == 3)
	{
		if (conv_num == 0)
		{
			Global_CMD_pad = 0;
			Global_CMD_pad_type = 0;
		}
		else if (conv_num == 1)
		{
			if (count == 1) 
			{
				Global_CMD_pad = 1;
				Global_CMD_pad_type = 14;
			}
			else if (count >= 2 &&  count <= 12) 
			{
				Global_CMD_pad = 1;
				Global_CMD_pad_type = 6;
			}
			else if (count == 13)
			{
				Global_CMD_pad = 1;
				Global_CMD_pad_type = 7;
			}
		}
		else if (conv_num == 2)
		{
			if (count == 1) 
			{
				Global_CMD_pad = 1;
				Global_CMD_pad_type = 14;
			}
			else if (count == 2) 
			{
				Global_CMD_pad = 1;
				Global_CMD_pad_type = 14;
			}
			else if (count >= 3 && count <= 6) 
			{
				Global_CMD_pad = 1;
				Global_CMD_pad_type = 6;
			}
			else if (count == 7 || count == 8)
			{
				Global_CMD_pad = 1;
				Global_CMD_pad_type = 7;
			}
		}

		else if (conv_num == 3)
		{
			// block 1
			if (count >= 1 && count <= 8) 
			{
				Global_CMD_pad = 1;
				Global_CMD_pad_type = 14;
			}
			else if (count >= 9 && count <= 16) 
			{
				Global_CMD_pad = 1;
				Global_CMD_pad_type = 7;
			}
		}

		else if (conv_num == 4)
		{
			if (count > 32)
			{
				Global_CMD_pad = 0;
				Global_CMD_pad_type = 0;
			}
			else
			{
				Global_CMD_pad = 1;
				Global_CMD_pad_type = 15;
			}
		}
	}
	else
	{
		Global_CMD_pad = 0;
		Global_CMD_pad_type = 0;
	}




	// if (conv_num == 0 || conv_num == 1 || conv_num == 2 || conv_num == 3 || conv_num == 4 || conv_num == 5)
	// {
	// 	if (conv_num == 4 && count > 32)
	// 	{
	// 		Global_CMD_pad = 0;
	// 		Global_CMD_pad_type = 0;
	// 	}
	// 	else if (conv_num == 0 && filters_size[conv_num] == 1) // Includes Layer 0 (1x1 conv)
	// 	{
	// 		Global_CMD_pad = 0;
	// 		Global_CMD_pad_type = 0;
	// 	}
	// 	else
	// 	{
	// 		Global_CMD_pad = 1;
	// 		if (conv_num == 3 || conv_num == 4 || conv_num == 5) {
	// 			Global_CMD_pad_type = 15; // 无论此时是哪个批次，整图都在手里，四面全补
	// 		} else if (conv_num == 2) {
	// 			int spatial_step = (count - 1) / 2 + 1; // 1, 2, 3
	// 			if (spatial_step == 1) {
	// 				Global_CMD_pad_type = 11; // Top Strip 
	// 			} else if (spatial_step == 3) {
	// 				Global_CMD_pad_type = 7; // Bottom Strip
	// 			} else {
	// 				Global_CMD_pad_type = 3; // Middle Strips
	// 			}
	// 		} else if (conv_num == 1){
	// 			int last_count_val = 12;  // 总共12条指令
	// 			if (count == 1) {
	// 				Global_CMD_pad_type = 11; // Top Strip 
	// 			} else if (count == last_count_val) {
	// 				Global_CMD_pad_type = 7; // Bottom Strip
	// 			} else {
	// 				Global_CMD_pad_type = 3; // Middle Strips
	// 			}
	// 		}
	// 	}
	// }
	// else
	// {
	// 	Global_CMD_pad = 0;
	// 	if (conv_num == 5) {
	// 		Global_CMD_pad = 1;
	// 		Global_CMD_pad_type = 15;
	// 	}
	// }

	///////////////////////////////////////////////////////////////////////////////////////////////////////////////
	//13.Global_CMD_width and Global_CMD_high
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////
	if (conv_num == 0)
	{
		Global_CMD_width = 768;
		Global_CMD_high = 16;
	}
	else if (conv_num == 1)
	{
		// Global_CMD_width = 384;
		// // Global_CMD_high = 34;
		// Global_CMD_high = l1_tile_h;

		if (count == 1)
		{
			Global_CMD_width = 384;
			Global_CMD_high = 31;
		}
		else if (count >= 2 && count <= 12)
		{
			Global_CMD_width = 384;
			Global_CMD_high = 32;
		}
		else
		{
			Global_CMD_width = 384;
			Global_CMD_high = (384 - 359);
		}
	}
	else if (conv_num == 2)
	{
		if (count == 1)
		{
			Global_CMD_width = 192;
			Global_CMD_high = 63;
		}
		else if (count == 2)
		{
			Global_CMD_width = 192;
			Global_CMD_high = 63;
		}
		else if (count == 3 || count == 4 || count == 5 || count == 6)
		{
			Global_CMD_width = 192;
			Global_CMD_high = 64;
		}
		else if (count == 7 || count == 8)
		{
			Global_CMD_width = 192;
			Global_CMD_high = 7;
		}
	}
	else if (conv_num == 3)
	{
		if (count >= 1 && count <= 8)
		{
			Global_CMD_width = 96;
			Global_CMD_high = 63;
		}
		else if (count >= 9 && count <= 16)
		{
			Global_CMD_width = 96;
			Global_CMD_high = 35;
		}
		
	}
	else if (conv_num == 4)
	{
		Global_CMD_width = 48;
		Global_CMD_high = 48;
	}
	else if (conv_num == 5)
	{
		Global_CMD_width = 24;
		Global_CMD_high = 24;
	}
	else {
		Global_CMD_width = 0;
		Global_CMD_high = 0;
	}


	///////////////////////////////////////////////////////////////////////////////////////////////////////////////
	//14.Global_CMD_kernel and Global_CMD_stride
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////
	Global_CMD_kernel = filters_size[conv_num];
	Global_CMD_stride = 1;

	if ((conv_num == 4 && count > 32)) {
		Global_CMD_fmapCache_cmd_valid = 0;  // 待定
	} else {
		Global_CMD_fmapCache_cmd_valid = 1;
	}

	// important
	if (conv_num == 0 || conv_num == 1)
	{
		Global_CMD_weight_rd_addr_cmd_base = 0;
	}
	else if (conv_num == 2)
	{
		if (count == 1 || count == 3 || count == 5 || count == 7) 
		{
			Global_CMD_weight_rd_addr_cmd_base = 0;
		}
		else if (count == 2 || count == 4 || count == 6 || count == 8)
		{
			Global_CMD_weight_rd_addr_cmd_base = 9;
		}
	}
	else if (conv_num == 3)
	{
		// if (count == 1) 
		// {
		// 	Global_CMD_weight_rd_addr_cmd_base = 0;
		// }
		// else if (count == 2)
		// {
		// 	Global_CMD_weight_rd_addr_cmd_base = 9;  // 待定
		// }

		Global_CMD_weight_rd_addr_cmd_base = 0;
	}

	else if (conv_num == 4)
	{
		if (conv_num == 4 && count > 32) {
			Global_CMD_weight_rd_addr_cmd_base = 0;
		}
		else {
			// int channel_in_step = (count - 1) % m + 1; 
			// if (channel_in_step == 1 || channel_in_step == 3 || channel_in_step == 5 || channel_in_step == 7) {
			// 	Global_CMD_weight_rd_addr_cmd_base = 0;
			// } else {
			// 	Global_CMD_weight_rd_addr_cmd_base = 9; 
			// }

			Global_CMD_weight_rd_addr_cmd_base = 0;
		}
	}

	///////////////////////////////////////////////////////////////////////////////////////////////////////////////
	//15.Psum_module_enable and Bias_module_enable
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////
	if (conv_num == 4 && count > 32)
	{
		Psum_module_enable = 0;
	}
	else
	{
		Psum_module_enable = 1;
	}
	

	// if (conv_num == 0 || conv_num == 1 || conv_num == 2 || conv_num == 3 || conv_num == 4 || conv_num == 5) {
	// 	if (conv_num == 4 && count > 32) {
	// 		Bias_module_enable = 0;
	// 	} else {
	// 		Bias_module_enable = 1;
	// 	}
	// } else {
	// 	Bias_module_enable = 0;
	// }

	if (conv_num == 0 || conv_num == 1 || conv_num == 2)
	{
		Bias_module_enable = 1;
	}
	else if (conv_num == 3) {
		// if (count % 2 == 1) {
		// 	Bias_module_enable = 0;
		// }
		// else {
		// 	Bias_module_enable = 1;
		// }

		if (count % 2 == 0) {
			Bias_module_enable = 1;
		}
		else {
			Bias_module_enable = 0;
		}
	}
	else if (conv_num == 4)
	{
		if (count > 32)
		{
			Bias_module_enable = 0;
		}
		else
		{
			if (count % 4 == 0) {
				Bias_module_enable = 1;
			}
			else {
				Bias_module_enable = 0;
			}
			
		}
	}
	
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////
	//16.another_pad and 
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////
	another_pad = 0;

	///////////////////////////////////////////////////////////////////////////////////////////////////////////////
	//17.First_layer : conv4需要
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////
	if (conv_num == 4 && count > 32)
	{
		First_layer = 1;
	}
	else
	{
		First_layer = 0;
	}
	
	if (conv_num == 4 && count > 32)
	{
		fl = 0;
		fl_length = 0;
	}
	else
	{
		fl = fl_data;
		fl_length = fl_length_data;
	}
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////
	//18.PCIE_START
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// PCIE_START = 0;

	// luis add
	// Layer 0 only asserts PCIE_START on the last instruction.
	// if (conv_num == 0 && count == 48) {
	// 	PCIE_START = 1;
	// }
	// else 
	
	// if (conv_num == 2 && count == 8) {
	// 	PCIE_START = 1;
	// }
	// else {	
	// 	PCIE_START = 0;
	// }

	// if (conv_num == 1 && count == 13) {
	// if (conv_num == 3 && count == 2) {
	// 	PCIE_START = 1;
	// }
	// else {	
	// 	PCIE_START = 0;
	// }
	PCIE_START = 1;

	// PCIE_START = 1;

	///////////////////////////////////////////////////////////////////////////////////////////////////////////////
	//19.RELU_NOT:conv9和conv13需要
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////
	RELU_NOT = 0;

	///////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// Full instruction debug dump (all major fields per instruction)
	cout << "\n[INST DBG] conv_num=" << conv_num
		 << " count=" << count
		 << " m=" << m
		 << " n=" << n
		 << " k=" << k
		 << endl;
	cout << "  GlobalBuffer_DDR_enable=" << GlobalBuffer_DDR_enable
		 << " GlobalBuffer_DDR_source_address=0x" << hex << GlobalBuffer_DDR_source_address
		 << " GlobalBuffer_DDR_aim_address=0x" << GlobalBuffer_DDR_aim_address
		 << dec << " GlobalBuffer_DDR_length=" << GlobalBuffer_DDR_length
		 << endl;
	cout << "  GlobalBuffer_enable=" << GlobalBuffer_enable
		 << " GlobalBuffer_WaitMMU=" << GlobalBuffer_WaitMMU
		 << " GlobalBuffer_source_address=0x" << hex << GlobalBuffer_source_address
		 << dec << " GlobalBuffer_length=" << GlobalBuffer_length
		 << endl;
	cout << "  WeightCache_DDR_enable=" << WeightCache_DDR_enable
		 << " WeightCache_DDR_source_address=0x" << hex << WeightCache_DDR_source_address
		 << " WeightCache_DDR_aim_address=0x" << WeightCache_DDR_aim_address
		 << dec << " WeightCache_DDR_length=" << WeightCache_DDR_length
		 << endl;
	cout << "  WeightCache_enable=" << WeightCache_enable
		 << " WeightCache_WaitMMU=" << WeightCache_WaitMMU
		 << " WeightCache_source_address=0x" << hex << WeightCache_source_address
		 << dec << " WeightCache_length=" << WeightCache_length
		 << endl;
	cout << "  Psum_enable=" << Psum_enable
		 << " Psum_next=" << Psum_next
		 << " Bias_enable=" << Bias_enable
		 << " Bias_source_address=" << Bias_source_address
		 << endl;
	cout << "  Compute_Result_enable=" << Compute_Result_enable
		 << " Compute_Result_source_address=0x" << hex << Compute_Result_source_address
		 << dec << " Compute_Result_length=" << Compute_Result_length
		 << endl;
	cout << "  Global_CMD_Quantization_LeftOrRight=" << Global_CMD_Quantization_LeftOrRight
		 << " Global_CMD_Quantization_length=" << Global_CMD_Quantization_length
		 << " Global_CMD_total=" << Global_CMD_total
		 << " Global_CMD_pooling=" << Global_CMD_pooling
		 << " Global_CMD_pooling_length=" << Global_CMD_pooling_length
		 << " Global_CMD_upsampling=" << Global_CMD_upsampling
		 << " Global_CMD_pad=" << Global_CMD_pad
		 << " Global_CMD_pad_type=" << Global_CMD_pad_type
		 << " Global_CMD_width=" << Global_CMD_width
		 << " Global_CMD_high=" << Global_CMD_high
		 << " Global_CMD_kernel=" << Global_CMD_kernel
		 << " Global_CMD_stride=" << Global_CMD_stride
		 << " Global_CMD_fmapCache_cmd_valid=" << Global_CMD_fmapCache_cmd_valid
		 << " Global_CMD_weight_rd_addr_cmd_base=" << Global_CMD_weight_rd_addr_cmd_base
		 << endl;
	cout << "  Psum_module_enable=" << Psum_module_enable
		 << " Bias_module_enable=" << Bias_module_enable
		 << " another_pad=" << another_pad
		 << " First_layer=" << First_layer
		 << " fl=" << fl
		 << " fl_length=" << fl_length
		 << " PCIE_START=" << PCIE_START
		 << " RELU_NOT=" << RELU_NOT
		 << endl;
	
	std::string GlobalBuffer_DDR = A109bit_trans(GlobalBuffer_DDR_enable, GlobalBuffer_DDR_source_address, GlobalBuffer_DDR_aim_address, GlobalBuffer_DDR_length);
	std::string GlobalBuffer = B74bit_trans(GlobalBuffer_enable, GlobalBuffer_WaitMMU, GlobalBuffer_source_address, GlobalBuffer_length);

	std::string WeightCache_DDR = A109bit_trans(WeightCache_DDR_enable, WeightCache_DDR_source_address, WeightCache_DDR_aim_address, WeightCache_DDR_length);
	std::string WeightCache = B74bit_trans(WeightCache_enable, WeightCache_WaitMMU, WeightCache_source_address, WeightCache_length);

	std::string PsumANDBias = E11bit_trans(Psum_enable,Psum_next,Bias_enable, Bias_source_address);
	std::string Compute_Result = C73bit_trans(Compute_Result_enable, Compute_Result_source_address, Compute_Result_length);

	std::string Other_cmd = Dotherbit_trans(Global_CMD_Quantization_LeftOrRight, Global_CMD_Quantization_length, Global_CMD_total, Global_CMD_pooling, Global_CMD_pooling_length, Global_CMD_upsampling, Global_CMD_pad, Global_CMD_pad_type, Global_CMD_width, Global_CMD_high,
		Global_CMD_kernel, Global_CMD_stride, Global_CMD_fmapCache_cmd_valid, Global_CMD_weight_rd_addr_cmd_base, Psum_module_enable, Bias_module_enable, another_pad, First_layer, fl,fl_length, PCIE_START,RELU_NOT);  //40bit

	//1024-511=513bit
	std::string zero_string = zero(513);

	std::string VLIM = zero_string + Other_cmd + Compute_Result + PsumANDBias + WeightCache + WeightCache_DDR + GlobalBuffer + GlobalBuffer_DDR;
	//std::string VLIM = GlobalBuffer_DDR + GlobalBuffer;

	return VLIM;
}
