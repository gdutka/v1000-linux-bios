#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <iostream>
using std::cerr;
using std::endl;
#include <fstream>
using std::ofstream;
#include <cstdlib> // for exit function

int savefile(char* av,uint32_t crc)
{
   strcat(av,"_crc"); 
   ofstream outdata(av, std::ios::binary ); // outdata is like cin
   int i; // loop index
   uint8_t num[4] = {0xFF, 0xFF, 0xFF, 0xFF};

   if( !outdata ) { // file couldn't be opened
      cerr << "Error: file could not be opened" << endl;
      exit(1);
   }
   outdata.put(crc);
   outdata.put(crc>>8);
   outdata.put(crc>>16);
   outdata.put(crc>>24);
   outdata.close();
 
   return 0;
}

uint32_t crc32_for_byte(uint32_t r) {
  for(int j = 0; j < 8; ++j)
    r = (r & 1? 0: (uint32_t)0xEDB88320L) ^ r >> 1;
  return r ^ (uint32_t)0xFF000000L;
}

void crc32(const void *data, size_t n_bytes, uint32_t* crc) {
  static uint32_t table[0x100];
  if(!*table)
    for(size_t i = 0; i < 0x100; ++i)
      table[i] = crc32_for_byte(i);
  for(size_t i = 0; i < n_bytes; ++i)
    *crc = table[(uint8_t)*crc ^ ((uint8_t*)data)[i]] ^ *crc >> 8;
}

int main(int ac, char** av) {
  FILE *fp;
  char buf[1L << 15];
  for(int i = ac > 1; i < ac; ++i)
    if((fp = i? fopen(av[i], "rb"): stdin)) { 
      uint32_t crc = 0;
      while(!feof(fp) && !ferror(fp))
        crc32(buf, fread(buf, 1, sizeof(buf), fp), &crc);
      if(!ferror(fp))
        printf("%08x%s%s\n", crc, ac > 2? "\t": "", ac > 2? av[i]: "");
        savefile(av[i],crc);
      if(i)
        fclose(fp);
    }
  return 0;
}
