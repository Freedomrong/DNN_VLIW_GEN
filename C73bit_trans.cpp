#include<base_math.h>
#include <string>
std::string C73bit_trans(int enable, long long address, int length) {
	// 1 36 36  
	std::string enable_b = decToBinS(enable, 1);
	std::string address_b = decToBinL(address, 36);
	std::string length_b = decToBinL(length, 36);

	std::string end_string = length_b + address_b + enable_b;
	//std::string end_string = enable_b + "_" + address_b + "_" + length_b + "_";
	return end_string;
}