#include <iostream>
#include <bitset>
#include "PUF.h"
using namespace std;
int main() {
    PUF puf;
    bitset<128> S;
    bool Auth = puf.Auth(S);
    return 0;
}
