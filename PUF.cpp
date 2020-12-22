//
// Created by sauron on 12/1/20.
//

#include "PUF.h"
#include <iostream>
#include <algorithm>
#include <bitset>
using namespace std;

std::bitset<CH_SIZE / 2> PUF::Stage_1(std::bitset<CH_SIZE> S) {
    std::bitset<CH_SIZE/2> R;
    static long holdrand[64];
    for(int j = 0; j < 64; j++)
        holdrand[j] = 0;
    for (int i = 0; i < 64; i++) { // generate random output of ES1_i
        if (i < 32) {
            int idx = i * 4;
            for (int j = 0; j < 4; j++) {
                holdrand[i] *= 2;
                holdrand[i] += S[idx + 3 - j];
            }
        }
        if (i >= 32) {
            int idx = (i - 32) * 4;
            for (int j = 0; j < 4; j++) {
                holdrand[i] *= 2;
                holdrand[i] += S[idx + 3 - j];
            }
        }
        long temp = holdrand[i];
        temp *= RAND1[i];
        temp = (((temp * 214013L + 2531011L) >> 16) & 0x7fff);
        R[i] = temp % 2;
    }
//    std::cout << "holdrands are " << holdrand[63]  << " " << holdrand[31]<< std::endl;
//	std::cout << "Output of ES63 and ES31:  " << R[63]  << " " << R[31]<< std::endl;
    cout << R << endl;
	/*std::cout << "Output of ES0 and ES32:  " << R[0] << R[32] << std::endl;
	std::cout << "Output of ES1 and ES33:  " << R[1] << R[33]<< std::endl;
	std::cout << "Output of ES2 and ES34:  " << R[2] << R[34] << std::endl;
	std::cout << "Output of ES3 and ES35:  " << R[3] << R[35] << std::endl;
	std::cout << "Output of ES4 and ES36:  " << R[4] << R[36] << std::endl;
	*/
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
            temp[j] = S[i + j];
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
    std::cout << "Output of Sbox is: " << S << endl;
    bitset<CH_SIZE / 2> S_after_Stage2;
    S_after_Stage2 = Stage_2(S);
    std::cout << "Output of Stage 2 is: " << S_after_Stage2 << endl;
    bool Auth = S_after_Stage2.count() % 2;
    std::cout << "Auth is: " << Auth << endl;
    return Auth;
}

////%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%////

bitset<CH_SIZE> PUF::Random(bitset<CH_SIZE> S)// Random 128 bits state
{
	for (size_t i = 0; i < CH_SIZE; i++)
	{
		S[i] = rand() % 2;
	}
//	std::cout << S << endl;
	return S;
}
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


//void PUF::Intersect(vector<bitset<4> > vector1, vector<bitset<4> > vector2)
//void PUF::Intersect(vector<int> vector1, vector<int> vector2)
vector<unsigned long> PUF::Intersect(vector<unsigned long> vector1, vector<unsigned long> vector2)
//found in google, didn't invent this function myself. I can't work with iterators :D
{

	sort(vector1.begin(), vector1.end());
	sort(vector2.begin(), vector2.end());
	//vector<bitset<4>> v(vector1.size() + vector2.size());
	//vector<bitset<4>>::iterator it, st;

	vector<unsigned long> v(vector1.size() + vector2.size());
	vector<unsigned long> v_inters;// if return v; then v has the intersected elements and all zeroes after that.
	//I want only the intersection
	vector<unsigned long>::iterator it, st;

	it = set_intersection(vector1.begin(), vector1.end(), vector2.begin(), vector2.end(), v.begin());

	std::cout << "\nCommon elements:\n";
	for (st = v.begin(); st != it; ++st)
	{
		std::cout << *st << ", ";
		v_inters.push_back(*st);
	}
	std::cout << '\n';
	return v_inters;
}


void PUF::Collision_State1()
// 1. take all comb. for the 4 bits going to sb63 and sb31
// 2. Create two subsets of the inpits- leading to responce 0 and responce 1
// 3. Repeat with other fixed 124bits
// 4 intersect the subsets
{
	PUF puf;
	bitset<128> S, S1;
	vector<int> Intercep1, Intercept0, Challange;
	vector<vector<unsigned long>> OutputSets1, OutputSets0;// store the subsets from each experiment
	int numbOfExp = 5;
	for (int i = 0; i < numbOfExp; i++)//repeat the experiment 5 times
	{
		vector<unsigned long> Out1, Out0;//the subsets leading to responce 1 and 0
		for (size_t i = 0; i < 16; i++)// 16 possible 4bit comb
		{
			bitset<4> ii = bitset<4>(i);// take all possible 4bit combination, ii is the bitset of the int i
			S = Random(S);
			for (int i = 0; i < 3; i++)
			{
				S[124 + i] = ii[i];// the last 4 bits corresponding to ES63,
			}

			bool Auth = puf.Auth(S); // Run authentication
			if ((Auth == 1)) Out1.push_back(i);
			if ((Auth == 0)) Out0.push_back(i);
		}
		OutputSets1.push_back(Out1);
		OutputSets0.push_back(Out0);
		//cout << "responce 1 (out): "<<endl; Print(Out1);
		//cout << "responce 0 (out): "<<endl; Print(Out0);
	}

	cout << "responce 0 (OutputSets): "; Print(OutputSets0);
	cout << "responce 1 (OutputSets): "; Print(OutputSets1);
	//find the intersections:
	vector<unsigned long> Intersect0, Intersect1;
	Intersect0 = (Intersect(OutputSets0[0], OutputSets0[1]));
	Intersect1 = (Intersect(OutputSets1[0], OutputSets1[1]));
	for (int i = 2; i < numbOfExp; i++)
	{
		Intersect0 = (Intersect(Intersect0, OutputSets0[i]));
		Intersect1 = (Intersect(Intersect1, OutputSets1[i]));
	}
	cout <<endl<< "print the int set of responces 0" << endl; Print(Intersect0);
	cout <<endl<< "print the int set of responces 1" << endl; Print(Intersect1);
}