/*
������instruction set�Ĳ��ִ����ݵ� �м俪ʼ
*/

#include<iostream>
#include<stdio.h>
#include <string> 
#include<vliw.h>
#include<fstream>
#include<base_math.h>
using namespace std;

// ---------------------------------------------------
int main()
{
	// 新模型参数 - 768x768x1 输入 (通道补齐到32处理), 共14层卷积 (0~13)
	// Conv11拆分为Conv11a(处理Conv10输出128ch)和Conv11b(处理Conv4输出256ch)
	// Layer: 0:768->384, 1:384->192, 2:192->96, 3:96->48, 4:48->24, 5:24->24(stride1), 
	//        6:24, 7:24(1x1), 8:24, 9:24(yolo), 10:24(1x1), 11a:48(route1), 11b:48(route2), 12:48(yolo)
	int kstorge[14] = { 768, 384, 192, 96, 48, 24, 24, 24, 24, 24, 24, 48, 48, 48 };
	int original[14] = { 1, 16, 32, 64, 128, 256, 512, 1024, 256, 512, 256, 128, 256, 256 };  // 输入通道
	//                   C0  C1  C2  C3  C4   C5   C6   C7    C8   C9  C10 C11a C11b C12
	int filters[14] = { 16, 32, 64, 128, 256, 512, 1024, 256, 512, 21, 128, 256, 256, 21 };    // 输出通道
	int filters_size[14] = { 1, 3, 3, 3, 3, 3, 3, 1, 3, 1, 1, 3, 3, 1 };  // 卷积核大小
    //                       0，1，2，3，4， 5，6，7，8， 9，10，11a，11b，12
	// ofstream file("VLIW_768x768_504_0305_len1024_test_address_0x00_v1.coe");

	// ----------------------
	//ofstream file("VLIW_768x768_504_0305_len1024_test_address_0x80_inst1_v2.coe");
	//ofstream file("VLIW_768x768_504_0305_len1024_test_address_0x80_inst2_v1.coe");
	//ofstream file("VLIW_768x768_504_0305_len1024_test_address_0x80_inst_1and2_v1.coe");
	//ofstream file("VLIW_768x768_504_0305_len1024_test_address_0x80_inst_123_v1.coe");
	//ofstream file("VLIW_768x768_504_0305_len1024_test_address_0x80_inst_1234_v1.coe");

	//ofstream file("VLIW_768x768_504_0305_len1024_test_address_0x80_inst_12_v2.coe");
	//ofstream file("VLIW_768x768_504_0305_len1024_test_address_0x80_inst_12_v4.coe");
	// ofstream file("VLIW_768x768_504_0305_len1024_test_address_0x80_inst_12_count_1_v5.coe");
	//ofstream file("VLIW_768x768_504_0305_len1024_test_address_0x80_inst_123_v1.coe");


	// ofstream file("VLIW_768x768_504_0305_len1024_test_address_0x80_inst_only3_v1.coe");
	// ofstream file("VLIW_768x768_504_0305_len1024_test_address_0x80_inst_only3_v2.coe");

	// ofstream file("VLIW_768x768_504_0305_len1024_test_address_0x80_inst_only3_v3.coe");

	// ofstream file("VLIW_768x768_504_0305_len1024_test_address_0x80_inst_123_0318_v2.coe");
	// ofstream file("VLIW_768x768_504_0305_len1024_test_address_0x80_inst_123_0318_v7_guo.coe");

	// ofstream file("VLIW_768x768_504_0305_len1024_test_address_0x80_inst_123_0319_v8_guo.coe");
	// ofstream file("VLIW_768x768_504_0305_len1024_test_address_0x80_inst_only3_0319.coe");

	// ofstream file("VLIW_768x768_504_0305_len1024_test_address_0x80_inst_v123_0320_v2.coe");
	// ofstream file("VLIW_768x768_504_0305_len1024_test_address_0x80_inst_v123_0320_v3.coe");
	// ofstream file("VLIW_768x768_504_0305_len1024_test_address_0x80_inst_only3_0320_v2.coe");
	
	// ofstream file("VLIW_768x768_504_0305_len1024_test_address_0x80_inst_v1234_0321_v3.coe");
	// ofstream file("VLIW_768x768_504_0305_len1024_test_address_0x80_inst_only4_0320_v1.coe");

	//ofstream file("VLIW_768x768_504_0305_len1024_test_address_0x80_inst_v1234_0321_v3.coe");

	// ofstream file("VLIW_768x768_504_0305_len1024_test_address_0x80_inst_v1234_0321_v4_guo_sim.coe");


	// ofstream file("VLIW_768x768_504_0305_len1024_test_address_0x80_inst_only4_0323_v1.coe");
	// ofstream file("VLIW_768x768_504_0305_len1024_test_address_0x80_inst_v1234_0325_v7_guo_sim.coe");
	// ofstream file("VLIW_768x768_504_0305_len1024_test_address_0x80_inst_v12345_0326_v1.coe");

	// ofstream file("VLIW_768x768_504_0305_len1024_test_address_0x80_inst_v123456_0326_v6.coe");
	// ofstream file("VLIW_768x768_504_0305_len1024_test_address_0x80_inst_v123456_0326_v10.coe");


	// ofstream file("VLIW_768x768_504_0305_len1024_test_address_0x80_inst_v1234567_0328_v1.coe");
	// ofstream file("VLIW_768x768_504_0305_len1024_test_address_0x80_inst_v12345678_0328_v1.coe");

	// ofstream file("VLIW_768x768_504_0305_len1024_test_address_0x80_inst_v123456789_0328_v1.coe");

	// ofstream file("VLIW_768x768_504_0305_len1024_test_address_0x80_inst_only7_0327_v2.coe");
	// ofstream file("VLIW_768x768_504_len1024_test_address_0x80_inst_only7_0328_v3.coe");

	ofstream file("VLIW_768x768_504_0305_len1024_test_address_0x80_inst_v12345678910_0328_v1.coe");
	// ----------------------only

	// 768x768模型各层指令数 (14层)
	int count;
	std::string VLIW;

	int add = 0;
	file << "memory_initialization_radix = 2;" << endl;
	file << "memory_initialization_vector=" << endl;
	//for (count = 0; count < 6; count++)
	// for (count = 0; count < 1; count++)  // Layer 0
	// for (count = 0; count < 2; count++)     // Layer 1
	// for (count = 1; count < 2; count++)     // Layer 1
	// for (count = 0; count < 4; count++)

	// for (count = 3; count < 4; count++)
	// for (count = 2; count < 3; count++)     // Layer 1
	// for (count = 0; count < 4; count++)     // Layer 1
	// for (count = 0; count < 5; count++)        // Layer 1
	// for (count = 0; count < 7; count++)        // Layer 1
	// for (count = 0; count < 8; count++)        // Layer 1

	for (count = 0; count < 10; count++)        // Layer 1
	// for (count = 6; count < 7; count++)        // Layer 1
	{
		int k = kstorge[count];
		int m = (original[count] % 32 == 0 ? original[count] / 32 : original[count] / 32+1);
		int n = (filters[count] % 32 == 0 ? filters[count] / 32 : filters[count] / 32+1);
		//cout << k <<" "<< m <<" "<< n << endl;
		cout <<"conv"<<count << endl;

		if (count == 0) // layer 0
		{
			for (int i = 1; i <= 48; i++)
			{
				cout << i << " ";
				VLIW = VLIW_1024bit(i, m, n, k, count);
				file << VLIW << ",";
				add++;
			}
		}
		else if (count == 1)  // layer 1
		{
			for (int i = 1; i <= 13; i++)
			// for (int i = 1; i < 2; i++)
			{
				cout << i << " ";
				VLIW = VLIW_1024bit(i, m, n, k, count);
				file << VLIW << ",";
				add++;
			}
		}
		else if (count == 2)  // layer 2
		{
			for (int i = 1; i <= 8; i++)  
			{
				cout << i << " ";
				VLIW = VLIW_1024bit(i, m, n, k, count);
				file << VLIW << ",";
				add++;
			}
		}
		else if (count == 3)  // layer 3 ：  输入：96x96x64，输出：48x48x128（3x3 + pooling）
		{
			for (int i = 1; i <= 16; i++)  //  16
			// for (int i = 1; i <= 2; i++)  //  16
			{
				cout << i << " ";
				VLIW = VLIW_1024bit(i, m, n, k, count);
				file << VLIW << ",";
				add++;
			}
		}
		else if (count == 4)  // layer : 输入：48x48x128，输出：48x48x256（3x3）
		{
			for (int i = 1; i <= m * n + 8; i++)  
			{
				// m=4, n=8, 32 convolution + 8 pooling = 40 instructions
				cout << i << " ";
				VLIW = VLIW_1024bit(i, m, n, k, count);
				file << VLIW << ",";
				add++;
			}
		}
		else if (count == 5)  // layer 5 : 输入：24x24x256，输出：24x24x512（3x3）
		{
			for (int i = 1; i <= m * n; i++)  
			{
				// m=8, n=16, 128 instructions
				cout << i << " ";
				VLIW = VLIW_1024bit(i, m, n, k, count);
				file << VLIW << ",";
				add++;
			}
		}
		// -------------------------------------
		// layer 7 : 输入：24x24x512，输出：24x24x1024（3x3）
		else if (count == 6)  // layer 6 : 输入：24x24x512，输出：24x24x1024（3x3）
		{
			for (int i = 1; i <= m * n; i++)  
			{
				// m=8, n=16, 128 instructions
				cout << i << " ";
				VLIW = VLIW_1024bit(i, m, n, k, count);
				file << VLIW << ",";
				add++;
			}
		}
		else if (count == 7)  // layer 8 : 输入：24x24x1024，输出：24x24x256（1x1）
		{
			for (int i = 1; i <= m * n; i++)  
			{
				// m=8, n=16, 128 instructions
				cout << i << " ";
				VLIW = VLIW_1024bit(i, m, n, k, count);
				file << VLIW << ",";
				add++;
			}
		}
		else if (count == 8)  // layer 9 : 输入：24x24x256，输出：24x24x512（3x3）
		{
			for (int i = 1; i <= m * n; i++)  
			{
				// m=8, n=16, 128 convolution
				cout << i << " ";
				VLIW = VLIW_1024bit(i, m, n, k, count);
				file << VLIW << ",";
				add++;
			}
		}

		// -------------------------------------
		else if (count == 9)  // Yolo 层，layer 10 : 输入：24x24x512，输出：24x24x32（1x1）
		{
			for (int i = 1; i <= m * n; i++)  
			{
				// m=16, n=1, 128 convolution
				cout << i << " ";
				VLIW = VLIW_1024bit(i, m, n, k, count);
				file << VLIW << ",";
				add++;
			}
		}

		cout << endl << endl;
	}
}


// // ---------------------------------------------------

// int main()
// {
// 	int kstorge[14] = { 256, 128, 64, 32, 16, 8, 8, 8, 8, 8, 8, 16, 16, 16 };
// 	int original[14] = { 1, 16, 32, 64, 128, 256, 512, 1024, 256, 512, 256, 128, 256, 256 };
// 	int filters[14] = { 16, 32, 64, 128, 256, 512, 1024, 256, 512, 255, 128, 256, 256, 255 };
// 	int filters_size[14] = { 1, 3, 3, 3, 3, 3, 3, 1, 3, 1, 1, 3, 3, 1 };
// 	//ofstream file("F:\\sjc\\690t\\DNN_690t_230224\\VLIW_256x256\\VLIW_1127_504.coe");

// 	//ofstream file("VLIW_1127_504_0305_len1024_test.coe");

// 	//ofstream file("VLIW_1127_504_0305_len1024_test_address_0x00_v2.coe");
// 	ofstream file("VLIW_768x768_504_0305_len1024_test_address_0x00_v1.coe");
// 	//file << "memory_initialization_radix = 2;" << endl;
// 	//file << "memory_initialization_vector=" << endl;
// 	int count;
// 	std::string VLIW;
// 	/*
// 	int i = 0;
// 	int m = 0;
// 	int n = 0;
// 	int k = 0;
// 	count = 0;
// 	VLIW = VLIW_1024bit(i, m, n, k, count);
// 	cout << VLIW << endl;
// 	file << VLIW << ",";
// 	*/
// /*
// 	int add = 0; 
	
// 	int k = kstorge[count];
// 	int m = (original[count] % 32 == 0 ? original[count] / 32 : original[count] / 32 + 1);
// 	int n = (filters[count] % 32 == 0 ? filters[count] / 32 : filters[count] / 32 + 1);
// 	//cout << m << n << endl;
// 	for (int i = 1; i <= m*n; i++)
// 	{
// 		//cout <<"VLIW"<< i << endl;
// 		//cout << i << endl;
// 		VLIW = VLIW_1024bit(i, m, n, k, count);
// 		//cout << count;
// 		//cout << VLIW << endl;
// 		file << VLIW << ",";
// 	}
// */
	
// 	int add = 0;
// 	file << "memory_initialization_radix = 2;" << endl;
// 	file << "memory_initialization_vector=" << endl;
// 	for (count = 0; count < 14; count++)
// 	{
// 		int k = kstorge[count];
// 		int m = (original[count] % 32 == 0 ? original[count] / 32 : original[count] / 32+1);
// 		int n = (filters[count] % 32 == 0 ? filters[count] / 32 : filters[count] / 32+1);
// 		//cout << k <<" "<< m <<" "<< n << endl;
// 		cout <<"conv"<<count << endl;
// 		if (count == 0)
// 		{
// 			for (int i = 1; i <= 4; i++)
// 			{
// 				cout << i << " ";
// 				VLIW = VLIW_1024bit(i, m, n, k, count);
// 				file << VLIW << ",";
// 				add++;
// 			}
// 		}
// 		else if (count == 4)
// 		{
// 			for (int i = 1; i <= m*n+8; i++)
// 			{
// 				cout << i << " ";
// 				VLIW = VLIW_1024bit(i, m, n, k, count);
// 				file << VLIW << ",";
// 				add++;
// 			}
// 		}
// 		else
// 		{
// 			for (int i = 1; i <= m*n; i++)
// 			{
// 				cout << i << " ";
// 				VLIW = VLIW_1024bit(i, m, n, k, count);
// 				//cout << count;
// 				//cout << VLIW << endl;
// 				file << VLIW <<',';
// 				add++;
// 			}	
// 		}
// 		cout << endl << endl;
// 	}
	
// 	//cout << add << endl;


// 	system("pause");
// }

