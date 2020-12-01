//
// Created by sauron on 12/1/20.
//

#include "PUF.h"
#include <iostream>
#include <bitset>

std::bitset<CH_SIZE / 2> PUF::Stage_1(std::bitset<CH_SIZE> S) {
    std::bitset<CH_SIZE/2> R;
    for (int i = 0; i < 64; i++) { // generate random output of ES1_i
        static long holdrand = 0L;
        if (i < 32) {
            int idx = i * 4;
            for (int j = 0; j < 4; j++) {
                holdrand *= 2;
                holdrand += S[idx + j];
            }
        }
        if (i >= 32) {
            int idx = (i - 32) * 4;
            for (int j = 0; j < 4; j++) {
                holdrand *= 2;
                holdrand += S[idx + j];
            }
        }
        holdrand += RAND1[i];
        holdrand = (((holdrand * 214013L + 2531011L) >> 16) & 0x7fff) % 2;
        R[i] = holdrand;
    }
    return R;
}

std::bitset<CH_SIZE/2> PUF::Stage_2(std::bitset<CH_SIZE> S) {
    std::bitset<CH_SIZE/2> R;
    for (int i = 0; i < 64; i++) { // generate random output of ES1_i
        static long holdrand = 0L;
        if (i < 32) {
            int idx = i * 4;
            for (int j = 0; j < 4; j++) {
                holdrand *= 2;
                holdrand += S[idx + j];
            }
        }
        if (i >= 32) {
            int idx = (i - 32) * 4;
            for (int j = 0; j < 4; j++) {
                holdrand *= 2;
                holdrand += S[idx + j];
            }
        }
        holdrand += RAND2[i];
        holdrand = (((holdrand * 214013L + 2531011L) >> 16) & 0x7fff) % 2;
        R[i] = holdrand;
    }
    return R;
}

std::bitset<CH_SIZE> PUF::Sbox(std::bitset<CH_SIZE> S) {
    std::bitset<CH_SIZE> R;
    for (int i = 0; i < CH_SIZE; i += 8) {
        std::bitset<8> temp;
        for (int j = 0; j < 8; j++)
            temp[j] = R[i + j];
        uint32_t sbox_input = temp.to_ulong();
        uint32_t sbox_output = aes_sbox[sbox_input];
        for (int j = 0; j < 8; j++) {
            R[i + 7 - j] = sbox_output % 2;
            sbox_output /= 2;
        }
    }
    return R;
}

bool PUF::Auth(std::bitset<CH_SIZE> S) {
    using namespace std;
    cout << "Initial state is: " << S << endl;
    bitset<CH_SIZE / 2> S_after_Stage1;
    S_after_Stage1 = Stage_1(S);
    cout << "Output of Stage 1 is: " << S_after_Stage1 << endl;
    bitset<CH_SIZE> Sbox_input;
    for(int i = 0 ; i < 16; i++){
        for(int j = 0; j < 8; j++){
            Sbox_input[i * 8 + j] = S_after_Stage1[(i * 4 + j) % (1<<6)]; // i'th sbox input is 8 consecuetive outputs of stage 1 i * 4 , ... , i * 4 + 7
        }
    }
    S = Sbox(Sbox_input);
    cout << "Output of Sbox is: " << S << endl;
    bitset<CH_SIZE / 2> S_after_Stage2;
    S_after_Stage2 = Stage_2(S);
    cout << "Output of Stage 2 is: " << S_after_Stage2 << endl;
    bool Auth = S_after_Stage2.count() % 2;
    cout << "Auth is: " << Auth << endl;
    return Auth;
}