#include <stdio.h>
#include <string.h>
#include <stdlib.h>

char *dec2bin(int dec){
    char *out = (char *)malloc(41), bin[40];

    int pos = 0;
    while(dec > 0){
        if((pos+1) % 5 == 0){
            bin[pos] = ' ';
        }
        else if(dec % 2 == 1){
            bin[pos] = '1';
            dec /= 2;
        }
        else{
            bin[pos] = '0';
            dec /= 2;             
        }
        pos++;
    }
    while((pos+1) % 5 != 0){ //Padding
        bin[pos] = '0';
        pos++;
    }

    for(int i = 0; i < (int)strlen(bin); i++){
        out[i] = bin[strlen(bin)-i-1];
    }

    return out;
}

int main(int argc, char* argv[]){
    if(argc != 2){
        printf("Usage: %s <decimal number>\n", argv[0]);
        return 1;
    }
    int dec = atoi(argv[1]);
    printf("Decimal: %d\n", dec);
    printf("Hexadecimal: %x\n", dec);
    printf("Octal: %o\n", dec);

    printf("Binary: %s\n", dec2bin(dec));
    return 0;
}