#include <stdio.h>
#include "xorencryption.h"


//define prime constants (every 4th prime from the first 25)
int PRIMES[] = {2, 11, 23, 41, 59, 73, 97};
int NUM_PRIMES = 7; //external constant

//XOR encrypt with prime mask
void xor_with_prime(unsigned char *data, size_t length, int prime) {
        for (size_t i = 0; i < length; i++){
        unsigned char mask = (unsigned char)((i * prime) & 0xFF);
        data[i] = data[i] ^ mask; //XOR OPERATION with MASK
        }
}
