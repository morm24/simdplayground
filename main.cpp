#include <iostream>
#include <string>
#include </home/mo/IT/openMS/simde-master/simde/x86/sse.h>
#include </home/mo/IT/openMS/simde-master/simde/x86/sse2.h>
#include </home/mo/IT/openMS/simde-master/simde/x86/ssse3.h>
#include <emmintrin.h>
#include <omp.h>
#include "encoder.h"
#include "decoder.h"
int main() {



    std::string eingabe = "ABCDEFGHIJKLXXXXXXXXXXXXABCDEFGHIJKLABCDEFGHIJKL";
    std::string ausgabe{};
    std::string rekonstruktion{};
    Encoder a;
    a.encodeString(eingabe, ausgabe);

    std::cout << ausgabe << std::endl;

    Decoder b;
    b.decodeString(ausgabe, rekonstruktion);

    std::cout << rekonstruktion << std::endl;

    return 0;
}