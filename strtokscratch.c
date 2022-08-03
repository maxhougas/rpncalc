#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../headers/hougasconsts.h"

int tokenize(char** tokens, int tokensWidth, char* buffer, int bufferWidth)
{
 tokens[0] = strtok(buffer," ");

 int i;
 for(i=1;i<tokensWidth && tokens[i-1];i++)
  tokens[i] = strtok(NULL," ");

 return SUCCESS;
}

int main(int argc, char** argv)
{
 char words[] = "hello world and stuff";
 char* tokens[4] = {NULL};

 tokenize(tokens,sizeof(tokens)/sizeof(char*),words,sizeof(words)/sizeof(char));

 int i;
 for(i=0;i<4;i++)
  printf("%s\n",tokens[i]);

 return SUCCESS;
}
