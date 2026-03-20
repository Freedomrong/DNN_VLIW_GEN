
#include<base_math.h>
#include <string>
std::string B74bit_trans(int enable, int WaitMMU, long long address, int length) {
	// 1 1 36 36 
	std::string enable_b = decToBinS(enable, 1);
	std::string WaitMMU_b = decToBinS(WaitMMU, 1);
	std::string address_b = decToBinL(address, 36);
	std::string length_b = decToBinL(length, 36);

	//std::string end_string = enable_b + "_" + WaitMMU_b + "_" + address_b + "_" + length_b + "_";
	std::string end_string =  length_b + address_b  + WaitMMU_b + enable_b;
	return end_string;
}