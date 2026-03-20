#include<base_math.h>
#include <string> //40bit-;e
std::string Dotherbit_trans(int Quantization_LeftOrRight, int Quantization_length,int total, int pooling,int pooling_length, int upsampling, int pad, int pad_type, int width, int high, int kernel, int stride, int fmapCache_cmd_valid, int weight_rd_addr_cmd_base,int psumenable,int biasenable,int anotherpad,int firstlayer,int fl,int fl_length,int pcie,int relu_not) {
	//1 8 1 1 1 2 4 10 10 3 3 1 6 
	std::string Quantization_LeftOrRight_b = decToBinS(Quantization_LeftOrRight, 1);
	std::string Quantization_length_b = decToBinS(Quantization_length, 8);
	std::string total_b = decToBinS(total, 1);
	std::string pooling_b = decToBinS(pooling, 1);
	std::string pooling_length_b = decToBinS(pooling_length, 1);
	std::string upsampling_b = decToBinS(upsampling,1);

	std::string pad_b = decToBinS(pad, 2);
	std::string pad_type_b = decToBinS(pad_type, 4);

	std::string width_b = decToBinS(width, 10);
	std::string high_b = decToBinS(high, 10);

	std::string kernel_b = decToBinS(kernel, 3);
	std::string stride_b = decToBinS(stride, 3);
	std::string fmapCache_cmd_valid_b = decToBinS(fmapCache_cmd_valid, 1);

	std::string weight_rd_addr_cmd_base_b = decToBinS(weight_rd_addr_cmd_base, 5);

	std::string psumenable_b = decToBinS(psumenable, 1);
	std::string biasenable_b = decToBinS(biasenable, 1);

	std::string anotherpad_b = decToBinS(anotherpad, 1);
	std::string firstlayer_b = decToBinS(firstlayer, 1);

	std::string fl_b = decToBinS(fl, 2);
	std::string fl_length_b = decToBinS(fl_length, 2);
	std::string pcie_b = decToBinS(pcie, 1);

	std::string relu_not_b = decToBinS(relu_not, 1);

	//std::string end_string = Quantization_LeftOrRight_b + "_" + Quantization_length_b + "_" + total_b + "_" + pooling_b + "_" + upsampling_b + "_" + pad_b + "_" + pad_type_b + "_" + width_b + "_" + high_b + "_" + kernel_b + "_" + stride_b + "_" + fmapCache_cmd_valid_b + "_" + weight_rd_addr_cmd_base_b + "_" + psumenable_b + "_" + biasenable_b;
	std::string end_string = relu_not_b + pcie_b + fl_length_b + fl_b + firstlayer_b + anotherpad_b + biasenable_b + psumenable_b + weight_rd_addr_cmd_base_b + fmapCache_cmd_valid_b + stride_b + kernel_b + high_b + width_b + pad_type_b + pad_b + upsampling_b + pooling_length_b + pooling_b + total_b + Quantization_length_b + Quantization_LeftOrRight_b;
	return end_string;
}