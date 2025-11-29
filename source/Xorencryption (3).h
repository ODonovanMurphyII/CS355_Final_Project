#ifndef XORENCRYPTION_H
#define XORENCRYPTION_H

#include <stddef.h> 

//constants for prime number selection 
extern int PRIMES[];
extern int NUM_PRIMES;

//function prototype
void xor_with_prime(unsigned char *data, size_t length, int prime);

#endif //preprocessor director
