using namespace std;
#include "hmm.hpp"
#include <iostream>
#include <string.h>
#include <stdlib.h>
#include <fstream>


int main(int argc, char *argv[])
{
  Model *m;
  char modFile[300];
  char seqFile[300];
  strcpy(modFile,"");
  strcpy(seqFile,"");
  int N =2;  
  for (int i=1; i<argc; i++) {
    if (!strncmp("-m",argv[i],2)) {
      if (i+1 >= argc) {
	cerr << " a file name must follow option -m \n";
	exit(1);
      } else {
	strcpy(modFile, argv[i+1]);
	i++;
      }
    } else if (!strncmp("-s",argv[i],2)) {
      if (i+1 >= argc) {
	cerr << " a file name must follow option -s \n";
	exit(1);
      } else {
	strcpy(seqFile, argv[i+1]);
	i++;
      }
    }
  }
  m = new Model(modFile);
  m->Decode(seqFile);
  return 0;
}
