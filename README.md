# SPUF1
Implementation of so called Strong PUF

### Run

```
x@y:~/SPUF1$ cmake . #code directory
x@y:~/SPUF1$ make
x@y:~/SPUF1$ ./SPUF1
```

### Notes on the Implementation
All interfaces required for authentication are in `PUF.h`. Each instance of `PUF` class is a stable circuit. Each stable circuit 
consists of random ES boxes. randomness to generate functions.  
See `main.cpp` for an example. Each step of the authentication process is also available publicly. 

### Usage
```
Puf puf; // Initialize the circuits with fresh randomness
puf.Auth(bitset<128> S); // Authenticate S. S is a std::bitset of size 128
puf.Stage_1(bitset<128> S); // Output 64 bits as the output of stage 1
puf.Stage_2(bitset<128> S); // Output 64 bits as the output of stage 2
puf.Sbox(bitset<128> S); // Performs 16 AES sboxes on 16 bytes.
```
Generating the required input for the Sbox layer and computing the final result is implemented in the
`Pum::Auth()` function.

### Resources
[1] Vikram B. Suresh, Raghavan Kumar, Mark Anders, Himanshu Kaul, Vivek De, Sanu Mathew:
A 0.26% BER, 1028 Challenge-Response Machine-Learning Resistant Strong-PUF in 14nm CMOS Featuring Stability-Aware Adversarial Challenge Selection. VLSI Circuits 2020: 1-2    

[2] Vikram B. Suresh, Raghavan Kumar, Sanu Mathew:
INVITED: A 0.26% BER, Machine-Learning Resistant 1028 Challenge-Response PUF in 14nm CMOS Featuring Stability-Aware Adversarial Challenge Selection. DAC 2020: 1-3
