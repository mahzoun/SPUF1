#include <iostream>
#include <bitset>
#include <time.h>
#include "PUF.h"
using namespace std;



template<typename T>
void Print(const T& t)
{
	std::cout << hex << t << " ";
}
template<typename T>
void Print(const std::vector<T>& v)
{
	std::cout << endl << '[';
	for (auto it = v.begin(); it != v.end(); ++it) {
		if (it != v.begin()) std::cout << ',';
		Print(*it);
	}
	std::cout << ']';
}




int main() {
	
	srand(time(NULL));
    PUF puf;
	bitset<128> S, S1,T, K;
	//bool Auth = puf.Auth(S); // Run authentication
	//puf.Collision_State1();
    T = puf.Random(T);
	for (int i = 0; i < 16; i++)
	{
		bitset<4> ii = bitset<4>(i);// take all possible 4bit combination, ii is the bitset of the int i
//		cout << "II is " << ii << endl;
		for (int i = 0; i < 4; i++)
		{
			T[124 + i] = ii[i];// the last 4 bits corresponding to ES63,
		}
//        cout << "Input is " << T << endl;

		puf.Stage_1(T);
//		puf.Auth(T);
	}//*/
    return 0;
}
