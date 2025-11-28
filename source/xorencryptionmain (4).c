#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "xorencryption.h" //must be in the same directory
#include <errno.h>
#define BUFFERSIZE 1001 //block size from encrypt 1.0 --- can be changed at anytime 



int main (int argc, char *argv[]){
if (argc != 4){
for (int i = 0; i < NUM_PRIMES; i++){
printf(" %d. Prime %d\n", i + 1, PRIMES[i]);
}
return 1;
}

//input and output files
const char *input_file = argv[1];
const char *output_file = argv[2];

int myprime = atoi(argv[3]); // conver ASCII to integer
int prime = PRIMES[myprime -1]; //select a prime and subtract one bc array

//check for valid input -- should not be 0 or less than 1
if (myprime < 1 || myprime > NUM_PRIMES) {
return 1;
}


//read input file 
FILE *fileinput = fopen(input_file, "rb");
if (!fileinput) {
      printf("ERROR: Cannot open file!\n");
      return 1;
}
  
FILE *fileoutput = fopen(output_file, "wb");
if (!fileoutput) {fclose(fileinput);
return 1;
}


unsigned char buffer[BUFFERSIZE];
size_t bytesread; 
size_t total_processed = 0; //to make sure the whole file was processed
printf("processing file in...%d byte blocks \n", BUFFERSIZE);

while ((bytesread = fread(buffer, 1, BUFFERSIZE, fileinput)) > 0) {
xor_with_prime(buffer, bytesread, prime);
fwrite(buffer, 1, bytesread, fileoutput);
total_processed = total_processed + bytesread;
}

fclose(fileinput);
fclose(fileoutput);
return 0;
}
