// fix the executable to run on win95
#include <stdio.h>
int main(int argc,char**argv) {
  FILE*f=fopen(argv[1],"r+b");
  char version=4;
  fseek(f,0x110,SEEK_SET);
  fwrite(&version,1,1,f);
  fseek(f,0x118,SEEK_SET);
  fwrite(&version,1,1,f);
  return 0;
}
