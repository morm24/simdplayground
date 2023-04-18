//
// Created by mo on 4/13/23.
//

#include "decoder.h"


void Decoder::decodeString(const std::string &in, std::string &out) {
    out.clear();
    out.resize((in.size() / 4) * 3);
    const char* inPtr = &in[0];

    //padding count:
    uint8_t g = 0;
    if( in[in.size() -1] == '=')
        g++;
    if( in[in.size() -2] == '=')
        g++;

    unsigned outsize = (in.size() / 16) * 12 + 16;
    //not final size (final rezize later to cutoff unwanted characters)
    out.resize(outsize);
    char* outPtr = &out[0];
    size_t loop= in.size()  / 16;





    for(int i = 0; i < loop; i++){

        simde__m128i data = simde_mm_lddqu_si128((simde__m128i*) (inPtr + i*16) );

        registerDecoder(data);

        simde_mm_storeu_si128((simde__m128 *) (outPtr + i * 12), data);

    }

    size_t read = loop * 16;
    size_t written = loop *12 ;
    std::string appendix(16 - (in.size()%16) , 'x');
    std::string rest = in.substr(read) + appendix;

    simde__m128i data = simde_mm_lddqu_si128((simde__m128i*) &rest[0] );
    registerDecoder(data);
    simde_mm_storeu_si128((simde__m128 *) (outPtr + written), data);

    //cutting off decoding of appendix
    outsize = (in.size() / 4) * 3 - g;
    out.resize(outsize);

}

void Decoder::registerDecoder(simde__m128i &data) {

    //ASCII letters must be translated over to base64. This cannot be achieved by just adding/substracting a single value
    //from each letter since in ASCII Alphabet capital letters aren't followed up by small Letters, and small Letters not by numbers (..).

    //Therefore certain kinds of characters must be masked out for further processing:

    //plusMask equals 0xFF for each corresponding plus, otherwise its 0
    simde__m128i plusMask = simde_mm_cmpeq_epi8(data, allPlus);
    simde__m128i allMask = plusMask;
    //slashMask similar to plusMask
    simde__m128i slashMask = simde_mm_cmpeq_epi8(data, allSlash);
    allMask |= slashMask;
    //for number mask: all characters less than '9' plus 1 must be numbers, '+' or '/' because input is Base64
    //therefore "not allMask and less than '9' + 1 (see allNumbers in header) " applied on data sets all bytes corresponding to numbers in the mask to 0xFF
    simde__m128i numberMask = simde_mm_andnot_si128(allMask, simde_mm_cmplt_epi8(data, allNumbers));
    allMask |= numberMask;
    simde__m128i bigLetterMask = simde_mm_andnot_si128(allMask, simde_mm_cmplt_epi8(data, allBigLetters));
    allMask |= bigLetterMask;
    simde__m128i smallLetterMask = simde_mm_andnot_si128(allMask, simde_mm_cmplt_epi8(data, allSmallLetters));

    //match ASCII characters with coresponding Base64 codes:
    data =  plusMask    & all62|
            slashMask   & all63|
            numberMask  & simde_mm_add_epi8( data, all4)|
            bigLetterMask & simde_mm_sub_epi8( data, all65)| //ASCII 'A' is 65, Base64 'A' is 0
            smallLetterMask & simde_mm_sub_epi8(data, all71);

    //convert little endian to big endian:
    data = simde_mm_shuffle_epi8(data, shuffleMask);

    //the actual magic (conversion base64 to ASCII) happens here
    data =  simde_mm_slli_epi32((data & mask1),2)   |
            simde_mm_slli_epi32((data & mask2),4)   |
            simde_mm_slli_epi32((data & mask3),6)   |
            simde_mm_slli_epi32((data & mask4),8);

    //convert big endian to little endian
    data= simde_mm_shuffle_epi8(data, shuffleMask2);
}
