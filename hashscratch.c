#include <stdio.h>
#include <string.h>

#define HASH(op,l) ((l>0?op[0]:0) + (l>1?256*op[1]:0) + (l>2?256*256*op[2]:0))

int main(int argc, char** argv)
{
 int magicnumber = HASH("cos",3);

 char buffer[10] = {0};

 fgets(buffer,10,stdin);

 strtok(buffer," \t\n\r");
 magicnumber = HASH(buffer,strlen(buffer));

 printf("%d\n%d\n",(int)strlen(buffer),magicnumber);

 return 0;
}