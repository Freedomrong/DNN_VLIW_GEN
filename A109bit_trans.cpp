
#include<base_math.h>
#include <string>
std::string A109bit_trans(int enable, long long source_address, long long aim_address, int length){
	// 1 36 36 36 
	std::string enable_b = decToBinS(enable,1);
	std::string source_address_b = decToBinL(source_address,36);
	std::string aim_address_b= decToBinL(aim_address,36);
	std::string length_b= decToBinL(length,36);

	//std::string end_string=enable_b +"_" +source_address_b+ "_"+ aim_address_b+"_" + length_b+"_";
	std::string end_string =  length_b + aim_address_b + source_address_b +enable_b;

	return end_string;
}