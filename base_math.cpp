#include <string>
#include <iostream>
using namespace std;
std::string decToBinL(long long dec,int bit_width) {
	//cout << dec<<endl;
	long long temp = dec;
	std::string result = std::to_string(temp % 2);

	while (temp) {
		temp = temp / 2;
		result = std::to_string(temp % 2).append(result);
	}

	//cout << result << endl;
	//看是否有一定的位数
	std::string mid_string = result;
	while (mid_string.size() < bit_width)
	{
		mid_string = "0"+mid_string;
	}
	//cout << mid_string << endl;
	return mid_string;
}


std::string decToBinS(int dec, int bit_width) {
	//cout << dec<<endl;
	int temp = dec;
	std::string result = std::to_string(temp % 2);
	temp = temp / 2;

	while (temp) {
		result = std::to_string(temp % 2).append(result);
		temp = temp / 2;
	}

	//看是否有一定的位数
	std::string mid_string = result;
	while (mid_string.size() < bit_width)
	{
		mid_string = "0" + mid_string;
	}
	//cout << mid_string << endl;
	return mid_string;
}



std::string zero(int length) {
	std::string zero_string = "0";
	while (zero_string.size() < length)
	{
		zero_string = "0" + zero_string;
	}
	return zero_string;
}