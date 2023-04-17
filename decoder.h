//
// Created by mo on 4/13/23.
//

#include<string>
#include </home/mo/IT/openMS/simde-master/simde/x86/sse.h>
#include </home/mo/IT/openMS/simde-master/simde/x86/sse2.h>
#include </home/mo/IT/openMS/simde-master/simde/x86/ssse3.h>
#ifndef SSEPLAYGROUND_DECODER_H
#define SSEPLAYGROUND_DECODER_H


class Decoder {

public:

    void decodeString(const std::string &in, std::string &out);

private:

    const simde__m128i mask1 = simde_mm_set1_epi32(0xFF000000);//00111111 00000000 00000000 00000000
    const simde__m128i mask2 = simde_mm_set1_epi32(0x00FF0000);//00000000 00111111 00000000 00000000
    const simde__m128i mask3 = simde_mm_set1_epi32(0x0000FF00);//00000000 00000000 00111111 00000000
    const simde__m128i mask4 = simde_mm_set1_epi32(0x000000FF);//00000000 00000000 00000000 00111111

    const simde__m128i shuffleMask = simde_mm_setr_epi8(3,2,1,0,7,6,5,4,11,10,9,8,15,14,13,12);

    const simde__m128i shuffleMask2 = simde_mm_setr_epi8(3,2,1,7,6,5,11,10,9,15,14,13,0,4,8,12);

};


#endif //SSEPLAYGROUND_DECODER_H
