//
// Created by mo on 4/13/23.
//

#include "decoder.h"

void Decoder::decodeString(const std::string &in, std::string &out) {
    out.clear();
    out.resize((in.size() / 4) * 3);
    const char* inPtr = &in[0];

    int testsize = out.size();
    char* outPtr = &out[0];

    const size_t loop= in.size() / 16;


    for(int i = 0; i < loop; i++){

        simde__m128i data = simde_mm_lddqu_si128((simde__m128i*) (inPtr + i*16) );

        //match ASCII characters with coresponding Base64 codes:

        simde__m128i plusMask = simde_mm_cmpeq_epi8(data, simde_mm_set1_epi8('+' ));
        simde__m128i allMask = plusMask;
        simde__m128i slashMask = simde_mm_cmpeq_epi8(data, simde_mm_set1_epi8('/'));
        allMask |= slashMask;
        simde__m128i numberMask = simde_mm_andnot_si128(allMask, simde_mm_cmplt_epi8(data, simde_mm_set1_epi8('9' +1)));
        allMask |= numberMask;
        simde__m128i bigLetterMask = simde_mm_andnot_si128(allMask, simde_mm_cmplt_epi8(data, simde_mm_set1_epi8('Z' +1)));
        allMask |= bigLetterMask;
        simde__m128i smallLetterMask = simde_mm_andnot_si128(allMask, simde_mm_cmplt_epi8(data, simde_mm_set1_epi8('z' +1)));

        data =  plusMask    & simde_mm_set1_epi8(62)|
                slashMask   & simde_mm_set1_epi8(63)|
                numberMask  & simde_mm_add_epi8( data, simde_mm_set1_epi8(4))|
                bigLetterMask & simde_mm_sub_epi8( data, simde_mm_set1_epi8(65))| //ASCII 'A' is 65, Base64 'A' is 0
                smallLetterMask & simde_mm_sub_epi8(data, simde_mm_set1_epi8(71));

        data = simde_mm_shuffle_epi8(data, shuffleMask);

        data =  simde_mm_slli_epi32((data & mask1),2)   |
                simde_mm_slli_epi32((data & mask2),4)   |
                simde_mm_slli_epi32((data & mask3),6)   |
                simde_mm_slli_epi32((data & mask4),8);

        data= simde_mm_shuffle_epi8(data, shuffleMask2);

        //

        simde_mm_storeu_si128((simde__m128*) (outPtr + i * 12), data);
        testsize = out.size();

        int ik = 3;
    }




}