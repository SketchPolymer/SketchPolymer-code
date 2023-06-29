#include "RngFast.hpp"

__m256i m1; // multiplier used in fast division

__m256i d1;

uint32_t shf1;

uint32_t shf2;


__m256i m11; // multiplier used in fast division

__m256i d11;

uint32_t shf11;

uint32_t shf12;


union U256 {

int32_t i[8];

__m256i ymm;

};


extern U256 eight_samples; // The array that'll contain the samples


rng::rng128 gen_arr;  



void 
initFastMod(uint32_t d) {
// Set or change divisor, calculate parameters

uint32_t L, L2, sh1, sh2, m;

switch (d) {

case 
0:

m = sh1 = sh2 = 1 / d;
// provoke error for d = 0

break;

case 
1:

m = 1; sh1 = sh2 =
0; 
// parameters for d = 1

break;

case 
2:

m = 1; sh1 = 
1; sh2 = 0;
// parameters for d = 2

break;

default: 
// general case for d > 2

L = ceil(log2(d));//bit_scan_reverse(d- 1) + 1; // ceil(log2(d))

L2 = L < 32 ? 
1 << L : 0;
// 2^L, overflow to 0 if L = 32

m = 1 + 
uint32_t((uint64_t(L2 - d) <<
32) / d); 
// multiplier

sh1 = 1; sh2 = L - 1; 
// shift counts

}

m1 = _mm256_set1_epi32(m);

d1 = _mm256_set1_epi32(d);

shf1 = sh1;

shf2 = sh2;

}



void get8samples() {

int indx = 
0;

eight_samples.ymm =
_mm256_set_epi64x(gen_arr(),
gen_arr(), 
gen_arr(), gen_arr());

__m256i t1 = _mm256_mul_epu32(eight_samples.ymm, m1);
// 32x32->64 bit unsigned multiplication of even ele/*ment*/s of a

__m256i t2 = _mm256_srli_epi64(t1,
32); 
// high dword of even numbered results

__m256i t3 = _mm256_srli_epi64(eight_samples.ymm,
32); 
// get odd elements of a into position for multiplication

__m256i t4 = _mm256_mul_epu32(t3, m1);
// 32x32->64 bit unsigned multiplication of odd elements

__m256i t5 = _mm256_set_epi32(-1,
0, -1,
0, -1,
0, -1,
0); 
// mask for odd elements

__m256i t7 = _mm256_blendv_epi8(t2, t4, t5);
// blend two results

__m256i t8 = _mm256_sub_epi32(eight_samples.ymm, t7);
// subtract

__m256i t9 = _mm256_srli_epi32(t8, shf1);
// shift right logical

__m256i t10 = _mm256_add_epi32(t7, t9);
// add

__m256i t11 = _mm256_srli_epi32(t10, shf2);
// shift right logical

__m256i t12 = _mm256_mullo_epi32(t11, d1);
// multiply quotient with divisor

eight_samples.ymm =
_mm256_sub_epi32(eight_samples.ymm, t12);
// subtract

}



/* EXAMPLE OF USE 
int main(){
  //initFastMod(500);
  //get8samples();
  //::cout << "0: " << eight_samples.i[0] << " 1: " << eight_samples.i[1] << std::endl;
  return result;
}*/