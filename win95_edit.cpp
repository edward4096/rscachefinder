// fix the executable to run on win95
#include <stdio.h>
int main(int argc,char**argv) {
  FILE*f=fopen(argv[1],"r+b");
  int off;
  fseek(f,0x03C,SEEK_SET);
  fread(&off,1,4,f);
  printf("off=%d\n", off);
  char version=4;
  fseek(f,0x40+off,SEEK_SET);
  fwrite(&version,1,1,f);
  fseek(f,0x48+off,SEEK_SET);
  fwrite(&version,1,1,f);
  return 0;
}
