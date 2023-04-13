//Author: Moritz Berger Date: 13.04.2023

//Includes:
#include </buffer/ag_bsc/pmsb_23/morib70/simdplayground/encoder.h>
//#include <encoder.h>
#include <iostream>



int main()
{
const std::string data = "Test123";
std::string out = "";

Encoder base;

base.encodeString(data,out);


std::cout << out << "\n";
//encodeString(&data,&out[0]);

return 0;
}