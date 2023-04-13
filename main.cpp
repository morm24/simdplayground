#include <iostream>
#include <string>
//#include </home/mo/IT/openMS/simde-master/simde/x86/sse.h>
//#include </home/mo/IT/openMS/simde-master/simde/x86/sse2.h>
//#include </home/mo/IT/openMS/simde-master/simde/x86/ssse3.h>
#include <x86/sse2.h>
#include <x86/ssse3.h>
#include <emmintrin.h>

int main()
{

  // einlesen Daten
  std::string eingabe = "ABCDEFGHIJKLMNOPQRSTUVWXYZA";
  std::string ausgabe;
  ausgabe.resize(eingabe.size() / 3 * 4);
  // simde__m128i testdata = simde_mm_loadu_si128((simde__m128i *)"ABCDEFGHIJKLMNOPAB");
  const char *ausgabePtr = &ausgabe[0];

  size_t outsize;

  // sichere die ersten drei b its der eingabe in der variable data
  simde__m128i data = simde_mm_load_si128(reinterpret_cast<const simde__m128i *>(eingabe.data()));
  // sortiere die bytes der eingabe entsprechend des little / big endian standarts
  data = simde_mm_shuffle_epi8(data, simde_mm_setr_epi8(2, 2, 1, 0, 5, 5, 4, 3, 8, 8, 7, 6, 11, 11, 10, 9));
  // auto shifted_data = simde_mm_srli_epi32(data,8);

  // erstelle die Masken für die bitoperationen auf den daten
  // Masken
  const simde__m128i mask1 = simde_mm_set1_epi32(0x3F000000); // 00111111 00000000 00000000 00000000
  const simde__m128i mask2 = simde_mm_set1_epi32(0x003F0000); // 00000000 00111111 00000000 00000000
  const simde__m128i mask3 = simde_mm_set1_epi32(0x00003F00); // 00000000 00000000 00111111 00000000
  const simde__m128i mask4 = simde_mm_set1_epi32(0x0000003F); // 00000000 00000000 00000000 00111111

  const simde__m128i allA = _mm_set1_epi8('A');
  const simde__m128i alla = _mm_set1_epi8('a' - 26);
  const simde__m128i all0 = _mm_set1_epi8('0' - 52);
  const simde__m128i allPlus = _mm_set1_epi8('+');
  const simde__m128i allSlash = _mm_set1_epi8('/');

  // schiebe immer 6 bits nach hinten und füge zwei nullen an.
  //  From XXXXXXXX YYYYYYYY ZZZZZZZZ to: 00XXXXXX 00XXYYYY 00YYYYZZ 00ZZZZZZ
  simde__m128i masked_data = simde_mm_srli_epi32(data, 2) & mask1 |
                             simde_mm_srli_epi32(data, 4) & mask2 |
                             simde_mm_srli_epi32(data, 6) & mask3 |
                             data & mask4;

  auto encodedBinaries = simde_mm_shuffle_epi8(masked_data, simde_mm_setr_epi8(3, 2, 1, 0, 7, 6, 5, 4, 11, 10, 9, 8, 15, 14, 13, 12));

  //check for "categories" of chars (A..Z; a..z; 0..9; +; /) and mask, and filter the data, so only those characters will be in a bitstring. 
  simde__m128i bigLetterMask = simde_mm_cmplt_epi8(encodedBinaries, _mm_set1_epi8(26)); // (a < b) ? 0xFF : 0x00
  simde__m128i allMask = bigLetterMask;
  simde__m128i smallLetterMask = simde_mm_andnot_si128(allMask, simde_mm_cmplt_epi8(encodedBinaries, simde_mm_set1_epi8(52))); // not allMask and  b where b is 0xFF if binaries are smaller than 52
  allMask |= smallLetterMask;
  simde__m128i numberMask = simde_mm_andnot_si128(allMask, simde_mm_cmplt_epi8(encodedBinaries, simde_mm_set1_epi8(62)));
  allMask |= numberMask;
  simde__m128i plusMask = simde_mm_andnot_si128(allMask, simde_mm_cmplt_epi8(encodedBinaries, simde_mm_set1_epi8(63)));
  allMask |= numberMask;
  simde__m128i &slashAntiMask = allMask;

  //change Base64 value to its correspondent ASCII value, by adding the character category mask to the filtered data,
  simde__m128i finalData = bigLetterMask & simde_mm_add_epi8(encodedBinaries, allA) |
                           smallLetterMask & simde_mm_add_epi8(encodedBinaries, alla) |
                           numberMask & simde_mm_add_epi8(encodedBinaries, all0) |
                           plusMask & allPlus |
                           simde_mm_andnot_si128(slashAntiMask, allSlash);

  //load the final data in the out_ptr
  simde_mm_storeu_si128((simde__m128 *)ausgabePtr, finalData);

  return 0;
}