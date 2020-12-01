#include <iostream>
#include <bitset>
#include "PUF.h"
using namespace std;
int main() {
    PUF puf;
    bitset<128> S, T, K;
    bool Auth = puf.Auth(S); // Run authentication


    return 0;
}
