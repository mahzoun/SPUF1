#include <iostream>
#include <bitset>
#include <time.h>
#include <chrono>
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

int matrix[16][16][16][16];

struct collision_class{
    pair<int, int> p[2];
};

vector<collision_class> collisions;
int main() {
    ios_base::sync_with_stdio(false);
    srand(time(NULL));
    PUF puf;
    bitset<128> S2, R;
    bitset<64> S, T;
    //bool Auth = puf.Auth(S); // Run authentication
    //puf.Collision_State1();
    for(int I = 0; I < 16; I++) {
        for (int J = 0; J < 16; J++) {
            for (int t = 0; t < 100; t++) {
                S2 = puf.Random(S2);
                for (int i = 0; i < 64; i++)
                    S[i] = S2[i];

                for (int i = 0; i < 16; i++) {
                    for (int j = 0; j < 8; j++) {
                        S2[i * 8 + j] = S[(i * 4 + j) % (1
                                << 6)]; // i'th sbox input is 8 consecuetive outputs of stage 1 i * 4 , ... , i * 4 + 7
                    }
                }
                vector<int> list[2];
//                cout << "----------------------\n";
                bitset<4> first = I;
                bitset<4> second = J;

                for (int i = 0; i < (1 << 4); i++) {
                    bitset<4> temp = i;
                    for (int j = 0; j < 4; j++) {
                        S2[j] = first[j];
                        S2[4 + j] = temp[j]; //second half of the first sbox
                        S2[8 + j] = temp[j]; //first half of the second sbox
                        S2[12 + j] = second[j];
                    }
                    R = puf.Sbox(S2);
                    T = puf.Stage_2(R);
                    list[T.count() % 2].push_back(i);
                }

                for (int j = 0; j < 2; j++) {
//                    for (int i = 0; i < list[j].size(); i++)
//                        cout << list[j][i] << " ";
//                    cout << endl;
                    for (int i = 0; i < list[j].size(); i++)
                        for (int ii = i; ii < list[j].size(); ii++) {
                            matrix[I][J][list[j][i]][list[j][ii]]++;
                        }
                }
            }
        }
    }
    int counter = 0;
    for(int i = 0; i < 16; i++) {
        for (int j = 0; j < 16; j++) {
            for (int ii = 0; ii < 16; ii++) {
                for (int jj = ii + 1; jj < 16; jj++) {
                    if(matrix[i][j][ii][jj] == 100){
                        counter++;
                        collision_class temp;
                        int a[4];
                        a[0] = aes_sbox[(i << 4) + ii];
                        a[1] = aes_sbox[(ii << 4) + j];
                        a[2] = aes_sbox[(i << 4) + jj];
                        a[3] = aes_sbox[(jj << 4) + j];
                        temp.p[0] = make_pair(a[0], a[1]);
                        temp.p[1] = make_pair(a[2], a[3]);
                        collisions.push_back(temp);
//                        cout << i << "\t" << j << "\t" << ii << "\t" << jj <<  "\t" << aes_sbox[(i << 4) + ii] << "\t"
//                            << aes_sbox[(ii << 4) + j] << "\t" << aes_sbox[(i << 4) + jj] << "\t" << aes_sbox[(jj << 4) + j] << endl;
                    }
                }
            }
        }
    }

    cout << counter << endl;
    vector<vector<collision_class>> list;
//    vector<collision_class> null;
//    list.push_back(null);
    for(int i = 0; i < collisions.size(); i++){ // iterating elements
        bool founded_collisions = false;
        for(int j = 0; j < list.size(); j++){ // iterating lists of collisions
            for(int k = 0; k < list[j].size(); k++){ //trying to find a collisions
                cout << i << "\t" << j << "\t" << k << "\t" << list[j].size() << endl;
                if(collisions[i].p[0] == list[j][k].p[0] or collisions[i].p[0] == list[j][k].p[1] or collisions[i].p[1] == list[j][k].p[0] or collisions[i].p[1] == list[j][k].p[1]){
                    list[j].push_back(collisions[i]);
                    founded_collisions = true;
                    break;
                }
            }
        }
        if(founded_collisions == false){ // create new class
            vector<collision_class> null;
            null.push_back(collisions[i]);
            list.push_back(null);
        }
    }

    for(int i = 0; i < list.size(); i++){
        cout << "\n ------------------------ \n";
        for(int j = 0; j < list[i].size(); j++){
            cout << list[i][j].p[0].first << "\t" << list[i][j].p[0].second << "\t" << list[i][j].p[1].first << "\t" << list[i][j].p[1].second << "\t" << endl;
        }
    }
    int count_classes = 0;
    for(int i = 0 ; i < list.size();i++){
        if(list[i].size() > 1)
            count_classes++;
    }
    cout << count_classes << endl;
    return 0;
}
