#include <iostream>
#include <bitset>
#include "PUF.h"
using namespace std;
int main() {
    PUF puf;
    bitset<128> S, T, K;
    S = 0xFFFFFFFFFFFF;
    T = 0xFFFFFFFFFFFFFFF;
    bool Auth = puf.Auth(S); // Run authentication
    bool Auth1 = puf.Auth(T); // Run authentication


    return 0;
}
