//
// Created by sauron on 12/1/20.
//

#ifndef SPUF1_PUF_H
#define SPUF1_PUF_H


#include <iostream>
#include <cstdint>
#include <random>
#include <bitset>
#include <chrono>
#define CH_SIZE 128
static const uint32_t aes_sbox[256] = {
        /* 0     1     2     3     4     5     6     7     8     9     A     B     C     D     E     F  */
        0x63, 0x7c, 0x77, 0x7b, 0xf2, 0x6b, 0x6f, 0xc5, 0x30, 0x01, 0x67, 0x2b, 0xfe, 0xd7, 0xab, 0x76,
        0xca, 0x82, 0xc9, 0x7d, 0xfa, 0x59, 0x47, 0xf0, 0xad, 0xd4, 0xa2, 0xaf, 0x9c, 0xa4, 0x72, 0xc0,
        0xb7, 0xfd, 0x93, 0x26, 0x36, 0x3f, 0xf7, 0xcc, 0x34, 0xa5, 0xe5, 0xf1, 0x71, 0xd8, 0x31, 0x15,
        0x04, 0xc7, 0x23, 0xc3, 0x18, 0x96, 0x05, 0x9a, 0x07, 0x12, 0x80, 0xe2, 0xeb, 0x27, 0xb2, 0x75,
        0x09, 0x83, 0x2c, 0x1a, 0x1b, 0x6e, 0x5a, 0xa0, 0x52, 0x3b, 0xd6, 0xb3, 0x29, 0xe3, 0x2f, 0x84,
        0x53, 0xd1, 0x00, 0xed, 0x20, 0xfc, 0xb1, 0x5b, 0x6a, 0xcb, 0xbe, 0x39, 0x4a, 0x4c, 0x58, 0xcf,
        0xd0, 0xef, 0xaa, 0xfb, 0x43, 0x4d, 0x33, 0x85, 0x45, 0xf9, 0x02, 0x7f, 0x50, 0x3c, 0x9f, 0xa8,
        0x51, 0xa3, 0x40, 0x8f, 0x92, 0x9d, 0x38, 0xf5, 0xbc, 0xb6, 0xda, 0x21, 0x10, 0xff, 0xf3, 0xd2,
        0xcd, 0x0c, 0x13, 0xec, 0x5f, 0x97, 0x44, 0x17, 0xc4, 0xa7, 0x7e, 0x3d, 0x64, 0x5d, 0x19, 0x73,
        0x60, 0x81, 0x4f, 0xdc, 0x22, 0x2a, 0x90, 0x88, 0x46, 0xee, 0xb8, 0x14, 0xde, 0x5e, 0x0b, 0xdb,
        0xe0, 0x32, 0x3a, 0x0a, 0x49, 0x06, 0x24, 0x5c, 0xc2, 0xd3, 0xac, 0x62, 0x91, 0x95, 0xe4, 0x79,
        0xe7, 0xc8, 0x37, 0x6d, 0x8d, 0xd5, 0x4e, 0xa9, 0x6c, 0x56, 0xf4, 0xea, 0x65, 0x7a, 0xae, 0x08,
        0xba, 0x78, 0x25, 0x2e, 0x1c, 0xa6, 0xb4, 0xc6, 0xe8, 0xdd, 0x74, 0x1f, 0x4b, 0xbd, 0x8b, 0x8a,
        0x70, 0x3e, 0xb5, 0x66, 0x48, 0x03, 0xf6, 0x0e, 0x61, 0x35, 0x57, 0xb9, 0x86, 0xc1, 0x1d, 0x9e,
        0xe1, 0xf8, 0x98, 0x11, 0x69, 0xd9, 0x8e, 0x94, 0x9b, 0x1e, 0x87, 0xe9, 0xce, 0x55, 0x28, 0xdf,
        0x8c, 0xa1, 0x89, 0x0d, 0xbf, 0xe6, 0x42, 0x68, 0x41, 0x99, 0x2d, 0x0f, 0xb0, 0x54, 0xbb, 0x16
};


static uint32_t RAND11[
	CH_SIZE / 2] = {
			0x29b9eeb1, 0xa30aa7c6, 0x6364c68a, 0xb4dac955, 0x7d9f40c7, 0x85208bf0, 0x45571e19,
			0x689a8495, 0xa02d1461, 0xf58a50af, 0xbd500599, 0x5bab9480, 0x31fea2a9, 0x7566ea2,
			0x2f620923, 0x3c7cfbb5, 0xb77db7a7, 0xea4fc2a9, 0xbf42737b, 0x3e283008, 0xdaa861e8,
			0xcb1d089e, 0xe3d0839a, 0x306b7748, 0xdfff9bb8, 0xc541bf5f, 0xc7b01821, 0xcf0705e8,
			0xf11e87c9, 0xa10e2f86, 0xe9d12613, 0xe521e74b, 0x3683875, 0x40a82884, 0x26b51af6,
			0x3ecc3757, 0xf38412ed, 0x6b7b6bbf, 0xf9521658, 0x7256f4f2, 0x54a95f41, 0xece1d015,
			0xd71b078a, 0x8a9b6dc5, 0x26ebee72, 0x4aea529c, 0xc1707669, 0x2b0a3ce8, 0x38973e75,
			0xe5e7a675, 0x40f8f1a6, 0x22a00704, 0x380a91b1, 0x99c7091d, 0x9181c941, 0xe8019e8,
			0xe91cd774, 0x808edae5, 0xce9d7a79, 0x72ecfe1d, 0x7a768054, 0x15707c08, 0xeedabf38,
			0xbd29e4a5
};

static uint32_t RAND22[CH_SIZE / 2] = {
		0x15da1dce, 0x582978f6, 0xa0f68232, 0xb0de0f47, 0x8f8488b0, 0x7fc08313, 0x633b560,
		0x5077ffe3, 0xc9b25516, 0x97c7d6ef, 0xdf577144, 0x7cc24ac8, 0x3a2f8263, 0xa8267a3f,
		0x1dcf336a, 0xf39be0c3, 0x4db92105, 0x6e4e3810, 0xedeba02d, 0x48c3de7d, 0xeee96ab6,
		0xd6492615, 0x183a220b, 0xa6e1be5, 0x8564117e, 0x4831b590, 0xe3fcb607, 0x74fdca0a,
		0x139566c, 0x14c1fe3d, 0x18fd2a96, 0xac09cab4, 0x51a57f6f, 0x720a5a7e, 0x856bfa8,
		0x1d813bc7, 0xb25705f8, 0xa98370b7, 0xa4cb33f6, 0x17ee1645, 0xbce63040, 0xd7425b54,
		0x5044863c, 0x3ddec493, 0x69cd2ab7, 0x44f19fe4, 0x5bee6c6c, 0xab98dcfd, 0xf0e90945,
		0x8d4e4133, 0x9061dff8, 0x5a5945af, 0xeb0044e2, 0x3d87aeee, 0x255b5632, 0x34ce651b,
		0xc50aae18, 0xdb033495, 0x5fe1156c, 0x34a3c394, 0x278abf28, 0xe232ffc6, 0x796d174b,
		0x30ca0e72
};


class PUF {
public:
    uint32_t RAND1[CH_SIZE/2], RAND2[CH_SIZE/2];
    PUF(){
        auto seed = std::chrono::high_resolution_clock::now().time_since_epoch().count();
        std::mt19937 mt_rand(seed);
        for(int i = 0; i < CH_SIZE/2; i++)
            RAND1[i] = mt_rand();
        for(int i = 0; i < CH_SIZE/2; i++)
            RAND2[i] = mt_rand();
    }
    std::bitset<CH_SIZE/2> Stage_1(std::bitset<CH_SIZE>);
    std::bitset<CH_SIZE> Sbox(std::bitset<CH_SIZE>);
    std::bitset<CH_SIZE/2> Stage_2(std::bitset<CH_SIZE>);
	std::bitset<CH_SIZE> Random(std::bitset<128>);
    bool Sboxes();
    void searchCollisions();
    //void Intersect(std::vector<bitset<4> >, std::vector<bitset<4> >);
	std::vector<unsigned long> Intersect(std::vector<unsigned long>, std::vector<unsigned long>);
	void Collision_State1();
    bool Auth(std::bitset<CH_SIZE>);
};


#endif //SPUF1_PUF_H
