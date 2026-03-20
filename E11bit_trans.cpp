#include<base_math.h>
#include <string>
std::string E11bit_trans(int Penable, int Paddress, int Benable ,int Baddress) {
	// 1 36 36  
	std::string Penable_b = decToBinS(Penable, 1);
	std::string Paddress_b = decToBinS(Paddress, 1);
	std::string Benable_b = decToBinS(Benable, 1);
	std::string Baddress_b = decToBinS(Baddress, 8);

	std::string end_string = Baddress_b + Benable_b + Paddress_b + Penable_b;
	//std::string end_string = Penable_b+"_" + Paddress_b+"_" + Benable_b+"_" + Baddress_b+"_";
	return end_string;
}