#include <iostream>
#include <bitset>
#include <time.h>
#include <chrono>
#include <algorithm>
#include <fstream>
#include "PUF.h"

using namespace std;

template<typename T>
void Print(const T &t) {
    std::cout << hex << t << " ";
}

template<typename T>
void Print(const std::vector<T> &v) {
    std::cout << endl << '[';
    for (auto it = v.begin(); it != v.end(); ++it) {
        if (it != v.begin()) std::cout << ',';
        Print(*it);
    }
    std::cout << ']';
}

int matrix[16][16][16][16];

struct collision_class {
    pair<int, int> p[2];
};

vector<collision_class> collisions; //[16][16]; // i, j
vector<collision_class> C[16][16]; // i, j

ofstream fout("output.txt");

int collision_classes_stage2() {
    ios_base::sync_with_stdio(false);
    srand(time(NULL));
    PUF puf;
    bitset<128> S2, Sbox_out;
    bitset<64> S, T;
    //bool Auth = puf.Auth(S); // Run authentication
    //puf.Collision_State1();
    // iterating x0x1x2
    for (int i = 0; i < 16; i++) { // x0
        for (int j = 0; j < 16; j++) { //x2
            for (int x = 0; x < 16; x++) {
                bitset<4> I = i, J = j, X = x;
                for (int k = 0; k < 4; k++) { // SBOX 1, 0 input = I||X, X||J
                    S2[k] = J[k];
                    S2[4 + k] = X[k];
                    S2[8 + k] = X[k];
                    S2[12 + k] = I[k];
                }
                Sbox_out = puf.Sbox(S2);
                S = puf.Stage_2(Sbox_out);
                bitset<4> first, second;
                for (int k = 0; k < 4; k++) {
                    first[k] = S[k];
                    second[k] = S[k + 32];
                }
                collision_class temp;
                temp.p[0] = make_pair(i, x);
                temp.p[1] = make_pair(x, j);
                C[(int) first.to_ulong()][(int) second.to_ulong()].push_back(temp);
            }
        }
    }
    int counter = 0;
    for (int i = 0; i < 16; i++) {
        for (int j = 0; j < 16; j++) {
            fout << hex << i << " " << j << endl;
            if (C[i][j].size() == 0)
                counter++;
            for (int k = 0; k < C[i][j].size(); k++) {
                fout << hex << C[i][j][k].p[0].first << "\t" << C[i][j][k].p[0].second << "\t";
                fout << hex << C[i][j][k].p[1].first << "\t" << C[i][j][k].p[1].second << "\n";
            }
            fout << dec << "_________________________________\n";
        }
    }
    cout << counter << endl;
}

long long choose(int n, int k) {
    if (k == 0)
        return 1;
    return n * choose(n - 1, k - 1) / k;
}

int compute_number_of_solutions() {

    int count = 0;
    for (int i = 0; i <= 16; i++)
        for (int j = i; j <= 16 - i; j++)
            for (int k = j; k <= 16 - i - j; k++)
                for (int t = k; t <= 16 - i - j - k; t++) {
                    if (i + j + k + t == 16) {
                        cout << i << "\t" << j << "\t" << k << "\t" << t << endl;
                        count++;
                    }
                }
    cout << count << endl;
    long double div = 1 << 32;
    vector<int> v1, v2;
    v1.push_back(1);
    v1.push_back(2);
    v1.push_back(5);
    v1.push_back(8);
    v2.push_back(3);
    v2.push_back(4);
    v2.push_back(4);
    v2.push_back(5);
    do {
        long long p = choose(16, v2[0] + v2[1]);
        p *= choose(16, v2[0] + v2[2]);
        //p /= div;
        for (int i = 0; i < 4; i++)
            cout << v2[i] << "\t";
        cout << p << endl;
    } while (next_permutation(v2.begin(), v2.end()));
}

struct collision {
    vector<int> C[4];
} Collisions[16];

struct database {
    bitset<32> Class; // representing 32 bit collision class
    bool response;
};

vector<database> db;
PUF puf;

void find_collision(int position) { // find collision classes of ES1_position
    bitset<128> S, S1;
    vector<int> Intercep1, Intercept0, Challange;
    vector<vector<unsigned long>> OutputSets1, OutputSets0;// store the subsets from each experiment
    int numbOfExp = 7;
    for (int t = 0; t < numbOfExp; t++)//repeat the experiment numbOfExp times
    {
        S = puf.Random(S);
        vector<unsigned long> Out1, Out0;//the subsets leading to responce 1 and 0
        for (int i = 0; i < 16; i++)// 16 possible 4bit comb
        {
            bitset<8> ii = bitset<8>(i);// take all possible 4bit combination, ii is the bitset of the int i

            for (int j = 0; j < 4; j++) {
                S[position +
                  j] = ii[j];// the 4 bits corresponding to ESbox starting at bit bossition(ES63 starts at 124)
            }

            bool Auth = puf.Auth(S); // Run authentication
            if ((Auth == 1)) Out1.push_back(i);
            if ((Auth == 0)) Out0.push_back(i);
        }
        OutputSets1.push_back(Out1);
        OutputSets0.push_back(Out0);

    }
    //find the intersections:
    vector<unsigned long> intersect;

    vector<vector<unsigned long>> take;
    vector<vector<unsigned long>> EquivClases;
    for (unsigned long i = 0; i < pow(2, numbOfExp); i++)// all comb for intersecting the results
    {
        take.clear();
        //cout << endl << "i= " << i << endl;
        bitset<7> ii = bitset<7>(i);// how to make it work for any bitset size ?
        // CHANGE  bitset size WHEN NumbOfExp changes!
        for (int j = 0;
             j < ii.size(); j++)// ii is a combination of the 0 and 1 response subsets for each exp to intersect
        {
            if (ii[j] == 0) take.push_back(OutputSets0[j]); //in 'take' store the combinations to intersect
            else if (ii[j] == 1) take.push_back(OutputSets1[j]);
        }
        //cout << "Take is: " << endl;
        //Print(take);
        if (take[0].empty() || take[1].empty() || take[2].empty())
            continue;// if one of these is empty, discard this combination

        intersect = puf.Intersect(take[0], take[1]);

        for (int z = 2; z < numbOfExp; z++) {
            intersect = puf.Intersect(intersect, take[z]);
        }
        //cout<<"intersection is:"<<endl;
        //Print(intersect);
        if (intersect.empty() != 1) EquivClases.push_back(intersect);
    }
    for (int i = 0; i < EquivClases.size(); i++) {
        for (int j = 0; j < EquivClases[i].size(); j++) {
            int index = (124 - position) / 4;
            Collisions[index].C[i].push_back(EquivClases[i][j]);
        }
    }
//    cout << dec << "\n Equivalence classes:\t" << position << endl;
//    Print(EquivClases);
}

int main() {
    ios_base::sync_with_stdio(false);
    srand(time(NULL));
//    PUF puf;
    for (int T = 124; T >= 64; T -= 4) { // find collision class for class 63-T, 31 - T
        find_collision(T);
    }
    bool collision_founded = false;

    while (collision_founded == false) {
        //Check if the collision has not yet found after expected number of rounds.
        if (db.size() % 1000 == 0 & db.size() > (1 << 16)) {
            cout << "Warning, db size is too big: \t" << db.size() << endl;
        }
        //Random input generated by server
        bitset<128> input;
        input = puf.Random(input);
        for (int i = 0; i < 64; i++)
            input[i] = 0;

        // Check the db for possible collisions
        bitset<32> plaintext_collision; // we tested that this works correctly :D
        for (int idx = 124; idx >= 64; idx -= 4) {
            bitset<4> temp;
            for (int j = 0; j < 4; j++)
                temp[j] = input[idx + j];
            unsigned long temp2 = temp.to_ulong();
            int index = (124 - idx) / 4;
            // current_collision is either 0, 1, 2, 3 (the index of its corresponding collision class) which is a 2 bit value.
            bitset<2> current_collision = 0;
            for (int j = 0; j < 4; j++) {
                for (int k = 0; k < Collisions[index].C[j].size(); k++) {
                    if (temp2 == Collisions[index].C[j][k])
                        current_collision = j;
                }
            }
            // plaintext_collision contains all indices of collision classes
            plaintext_collision[(idx - 64) / 2 + 1] = current_collision[1];
            plaintext_collision[(idx - 64) / 2] = current_collision[0];
        }

        //Searching in db for plaintext collision
        bool collision_found_in_db = false;
        for (int I = 0; I < db.size(); I++) {
            if (db[I].Class == plaintext_collision) {
                // The attacker responds to challenge that already knows the answer.
                collision_founded = true;
                cout << "Collision found after " << db.size() << endl;
                cout << plaintext_collision << "\t" << db[I].response << endl;
                collision_found_in_db = true;
                break;
            }
        }
        if (collision_found_in_db == false) {
            database new_entry;
            new_entry.Class = plaintext_collision;
            new_entry.response = puf.Auth(input);
            db.push_back(new_entry);
        }
    }
            cout << "_____________________\n";
    //generate new plaintexts that we already know the answer
    for (int I = 0; I < 1024; I++) {
        bitset<128> input;
//        cout << db[I].Class << "\t" << db[I].response << endl;
        for (int i = 31; i >= 1; i -= 2) { // i and i - 1 represents the collision class for pair ES1 boxes 63 - i
            int temp = db[I].Class[i] * 2 + db[I].Class[i - 1];
//            Print(Collisions[i/2].C[temp]);
//            cout << "Class elements -- > ";
//            for(int j = 0; j < Collisions[15 - (i/2)].C[temp].size(); j++) {
//                cout << Collisions[15 - (i / 2)].C[temp][j] << "\t";
//            }
//            cout << endl;
            for(int j = 0; j < Collisions[15 - (i/2)].C[temp].size(); j++) {
                bitset<128> test_input;
                bitset<4> temper = Collisions[15 - (i / 2)].C[temp][j];
                for(int k = 0; k < 4; k++){
                    test_input[64 + (i-1)*2 + k] = temper[k];
                }
//                cout << test_input << "\t" << puf.Auth(test_input) << endl;
            }
            bitset<4> cur = Collisions[15 - (i / 2)].C[temp][0];
//            cout << 15 - (i/2) << "\t" << cur << "\t" << Collisions[15 - (i / 2)].C[temp][0] << endl;
            for (int j = 0; j < 4; j++)
                input[64 + (i - 1) * 2 + j] = cur[j];
        }
        fout << input << "\t" << db[I].response  << "\t" << puf.Auth(input) << endl;
    }
    return 0;
}