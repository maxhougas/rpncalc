/*Max's RPN Calculator*/

#include<math.h>
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<unistd.h>

#define ARG1HELP (!strcmp(argv[1], "--help") || !strcmp(argv[1], "-h"))
#define ARG1VERB (!strcmp(argv[1], "--verbose") || !strcmp(argv[1],"-v"))
#define CAT "cat"
#define HELPTXT "./help.txt" /*this file is not included with the source code*/

#define M_PI (2*acos(0))
#define M_E (exp(1))
#define TAU (2*M_PI)
#define DEE (M_PI/180)

#define BOTTOM_UP 0
#define TOP_DOWN 1

#define LINE_LENGTH 150
#define STACK_SIZE 100

#define BADTOKEN 4
#define EMPTYSTACK 2
#define FULLSTACK 3
#define NOERRORS 0
#define NAN 5
#define UNSPECIFIED 6
#define QUITFLAG 1

#define WHITE " \t\n\r"

#define NUMMATCH1(a) (47<a[0] && a[0]<58) /*0-9*/
#define NUMMATCH2(a) ((a[0]==43 || a[0]==45 || a[0]==46) && (47<a[1] && a[1]<58)) /*+, -, ., 0-9*/
#define NUMMATCH3(a) ((a[0]==43 || a[0]==45) && a[1]==46 && (47<a[2] && a[2]<58)) /*+, -, ., 0-9*/

#define TOKISCHM (!strcmp(tokens[i],"chm"))
#define TOKISPEK (!strcmp(tokens[i],"pek"))

typedef struct
{
 double* data;
 unsigned int p;
 unsigned int max;
} stack;

stack* stack_inst_ui(unsigned int max)
{
 stack* new_stack = malloc(sizeof(stack));
 double* data = malloc(sizeof(double)*(max-1));

 if(data==NULL)
  new_stack = NULL;
 else
 {
  new_stack->max = max;
  new_stack->data = data;
  new_stack->p = 0;
 }
 return new_stack;
}

stack* stack_inst_puiui(double* data, unsigned int max, unsigned int p)
{
 stack* new_stack = malloc(sizeof(stack));
 double* stack_data = malloc(sizeof(double)*(max-1));

 if(data==NULL)
  new_stack = NULL;
 else
 {
  memcpy(stack_data, data, p-1);

  new_stack->max = max;
  new_stack->data = data;
  new_stack->p = p;
 }
 return new_stack;
}

int stack_clear(stack* this)
{
 this->p = 0;
 return NOERRORS;
}

int stack_get_empty(stack* this)
{
 return this->p;
}

int stack_get_full(stack* this)
{
 return (this->p==this->max);
}

int stack_get_size(stack* this)
{
 return this->max;
}

int stack_kill(stack* this)
{
 free(this->data);
 free(this);
 return NOERRORS;
}

double stack_peek(stack* this)
{
 double ret;
 if(!this->p)
 {
  printf("Error empty stack\n");
  ret = NAN;
 }
 else
 {
  ret = this->data[this->p-1];
 }
 return ret;
}

double stack_pop(stack* this)
{
 double ret;
 if(!this->p)
 {
  printf("Error empty stack\n");
  ret == NAN;
 }
 else
 {
  this->p--;
  ret = this->data[this->p];
 }
 return ret;
}

double stack_push(stack* this, double data)
{
 double ret;
 if(this->p==this->max)
 {
  printf("Error full stack\n");
  ret = NAN;
 }
 else
 {
  this->data[this->p] = data;
  this->p++;
  ret = data;
 }
 return ret;
}

int stack_swap(stack* this) /*swap top two on this*/
{
 int ret;
 if(this==NULL | this->data==NULL)
  ret = UNSPECIFIED;
 else if(this->p<2)
  ret = EMPTYSTACK;
 else
 {
  double tmp = this->data[this->p-1];
  this->data[this->p-1] = this->data[this->p-2];
  this->data[this->p-2] = tmp;

  ret = NOERRORS;
 }
 return ret;
}

int stack_view(stack* this, int direction) /*use BOTTOM_UP and TOP_DOWN*/
{
 unsigned int i;
 double ret;
 if(direction == BOTTOM_UP)
 {
  for(i=0;i<this->p;i++)
   printf("% f\n",this->data[i]);
  ret = NOERRORS;
 }
 else if(direction == TOP_DOWN)
 {
  for(i=this->p-1;i<-1;i--)
   printf("% f\n",this->data[i]);
  ret = NOERRORS;
 }
 else
  ret = UNSPECIFIED;
 return ret;
}

int execute_op(stack* numstack, double* mem, char* op)
{
 int ret;

 if(!strcmp(op, "e"))
 {
  if(stack_get_full(numstack))
   ret = FULLSTACK;
  else
  {
   stack_push(numstack, M_E);
   ret = NOERRORS;
  }
 }
 else if(!strcmp(op, "t")) /*tau = 2*pi*/
 {
  if(stack_get_full(numstack))
   ret = FULLSTACK;
  else
  {
   stack_push(numstack, TAU);
   ret = NOERRORS;
  }
 }
 else if(!strcmp(op, "+"))
 {
  if(stack_get_empty(numstack)<2)
   ret = EMPTYSTACK;
  else
  {
   double b = stack_pop(numstack);
   double a = stack_pop(numstack);

   stack_push(numstack, a+b);
   ret = NOERRORS;
  }
 }
 else if(!strcmp(op, "-"))
 {
  if(stack_get_empty(numstack)<2)
   ret = EMPTYSTACK;
  else
  {
   double b = stack_pop(numstack);
   double a = stack_pop(numstack);

   stack_push(numstack, a-b);
   ret = NOERRORS;
  }
 }
 else if(!strcmp(op, "*"))
 {
  if(stack_get_empty(numstack)<2)
   ret = EMPTYSTACK;
  else
  {
   double b = stack_pop(numstack);
   double a = stack_pop(numstack);

   stack_push(numstack, a*b);
   ret = NOERRORS;
  }
 }
 else if(!strcmp(op, "/"))
 {
  if(stack_get_empty(numstack)<2)
   ret = EMPTYSTACK;
  else
  {
   double b = stack_pop(numstack);
   double a = stack_pop(numstack);

   stack_push(numstack, a/b);
   ret = NOERRORS;
  }
 }
 else if(!strcmp(op, "^"))
 {
  if(stack_get_empty(numstack)<2)
   ret = EMPTYSTACK;
  else
  {
   double b = stack_pop(numstack);
   double a = stack_pop(numstack);

   stack_push(numstack, pow(a,b));
   ret = NOERRORS;
  }
 }
 else if(!strcmp(op, "exp"))
 {
  if(stack_get_empty(numstack)<1)
   ret = EMPTYSTACK;
  else
  {
   double a = stack_pop(numstack);

   stack_push(numstack, exp(a));
   ret = NOERRORS;
  }
 }
 else if(!strcmp(op, "ln"))
 {
  if(stack_get_empty(numstack)<1)
   ret = EMPTYSTACK;
  else
  {
   double a = stack_pop(numstack);

   stack_push(numstack, log(a));
   ret = NOERRORS;
  }
 }
 else if(!strcmp(op, "d"))
 {
  if(stack_get_full(numstack))
   ret = FULLSTACK;
  else
  {
   stack_push(numstack, DEE);
   ret = NOERRORS;
  }
 }
 else if(!strcmp(op, "d*"))
 {
  double a = stack_pop(numstack);

  stack_push(numstack, a*DEE);
  ret = NOERRORS;
 }
 else if(!strcmp(op, "d/"))
 {
  double a = stack_pop(numstack);

  stack_push(numstack, a/DEE);
  ret = NOERRORS;
 }
 else if(!strcmp(op, "cos"))
 {
  if(stack_get_empty(numstack)<1)
   ret = EMPTYSTACK;
  else
  {
   double a = stack_pop(numstack);

   stack_push(numstack, cos(a));
   ret = NOERRORS;
  }
 }
 else if(!strcmp(op, "aco")) /*arccosine*/
 {
  if(stack_get_empty(numstack)<1)
   ret = EMPTYSTACK;
  else
  {
   double a = stack_pop(numstack);

   stack_push(numstack, acos(a));
   ret = NOERRORS;
  }
 }
 else if(!strcmp(op, "sin"))
 {
  if(stack_get_empty(numstack)<1)
   ret = EMPTYSTACK;
  else
  {
   double a = stack_pop(numstack);

   stack_push(numstack, sin(a));
   ret = NOERRORS;
  }
 }
 else if(!strcmp(op, "asi")) /*arcsine*/
 {
  if(stack_get_empty(numstack)<1)
   ret = EMPTYSTACK;
  else
  {
   double a = stack_pop(numstack);

   stack_push(numstack, asin(a));
   ret = NOERRORS;
  }
 }
 else if(!strcmp(op, "tan"))
 {
  if(stack_get_empty(numstack)<1)
   ret = EMPTYSTACK;
  else
  {
   double a = stack_pop(numstack);

   stack_push(numstack, tan(a));
   ret = NOERRORS;
  }
 }
 else if(!strcmp(op, "ata")) /*arctangent*/
 {
  if(stack_get_empty(numstack)<1)
   ret = EMPTYSTACK;
  else
  {
   double a = stack_pop(numstack);

   stack_push(numstack, atan(a));
   ret = NOERRORS;
  }
 }
 else if(!strcmp(op, "at2")) /*2 input arctanget*/
 {
  if(stack_get_empty(numstack)<2)
   ret = EMPTYSTACK;
  else
  {
   double b = stack_pop(numstack);
   double a = stack_pop(numstack);

   stack_push(numstack, atan2(a, b));
   ret = NOERRORS;
  }
 }
 else if(!strcmp(op, "coh")) /*hyperbolic cosine*/
 {
  if(stack_get_empty(numstack)<1)
   ret = EMPTYSTACK;
  else
  {
   double a = stack_pop(numstack);

   stack_push(numstack, cosh(a));
   ret = NOERRORS;
  }
 }
 else if(!strcmp(op, "ach")) /*hyperbolic arccosine*/
 {
  if(stack_get_empty(numstack)<1)
   ret = EMPTYSTACK;
  else
  {
   double a = stack_pop(numstack);

   stack_push(numstack, acosh(a));
   ret = NOERRORS;
  }
 }
 else if(!strcmp(op, "sih")) /*hyperbolic sine*/
 {
  if(stack_get_empty(numstack)<1)
   ret = EMPTYSTACK;
  else
  {
   double a = stack_pop(numstack);

   stack_push(numstack, sinh(a));
   ret = NOERRORS;
  }
 }
 else if(!strcmp(op, "ash")) /*hyperbolic arcsine*/
 {
  if(stack_get_empty(numstack)<1)
   ret = EMPTYSTACK;
  else
  {
   double a = stack_pop(numstack);

   stack_push(numstack, asinh(a));
   ret = NOERRORS;
  }
 }
 else if(!strcmp(op, "tah")) /*hyperbolic tangent*/
 {
  if(stack_get_empty(numstack)<1)
   ret = EMPTYSTACK;
  else
  {
   double a = stack_pop(numstack);

   stack_push(numstack, tanh(a));
   ret = NOERRORS;
  }
 }
 else if(!strcmp(op, "ath")) /*hyperbolic arctangent*/
 {
  if(stack_get_empty(numstack)<1)
   ret = EMPTYSTACK;
  else
  {
   double a = stack_pop(numstack);

   stack_push(numstack, atanh(a));
   ret = NOERRORS;
  }
 }
 else if(!strcmp(op, "abs"))
 {
  if(stack_get_empty(numstack)<1)
   ret = EMPTYSTACK;
  else
  {
   double a = stack_pop(numstack);

   stack_push(numstack, fabs(a));
   ret = NOERRORS;
  }
 }
 else if(!strcmp(op, "hyp"))
 {
  if(stack_get_empty(numstack)<2)
   ret = EMPTYSTACK;
  else
  {
   double b = stack_pop(numstack);
   double a = stack_pop(numstack);

   stack_push(numstack, hypot(a, b));
   ret = NOERRORS;
  }
 }
 else if(!strcmp(op, "cei"))
 {
  if(stack_get_empty(numstack)<1)
   ret = EMPTYSTACK;
  else
  {
   double a = stack_pop(numstack);

   stack_push(numstack, ceil(a));
   ret = NOERRORS;
  }
 }
 else if(!strcmp(op, "flo"))
 {
  if(stack_get_empty(numstack)<1)
   ret = EMPTYSTACK;
  else
  {
   double a = stack_pop(numstack);

   stack_push(numstack, floor(a));
   ret = NOERRORS;
  }
 }
 else if(!strcmp(op, "\\"))
 {
  if(stack_get_empty(numstack)<2)
   ret = EMPTYSTACK;
  else
  {
   double b = stack_pop(numstack);
   double a = stack_pop(numstack);

   stack_push(numstack, floor(a/b));
   ret = NOERRORS;
  }
 }
 else if(!strcmp(op, "%"))
 {
  if(stack_get_empty(numstack)<2)
   ret = EMPTYSTACK;
  else
  {
   double b = stack_pop(numstack);
   double a = stack_pop(numstack);

   stack_push(numstack, fmod(a, b));
   ret = NOERRORS;
  }
 }
 else if(!strcmp(op, "clr"))
 {
  stack_clear(numstack);
  ret = NOERRORS;
 }
 else if(!strcmp(op, "len")) /*returns the current size of the stack*/
 {
  double a = (double)stack_get_empty(numstack);

  stack_push(numstack, a);
  ret = NOERRORS;
 }
 else if(!strcmp(op, "pek"))
 {
  if(stack_get_empty(numstack)<1)
   ret = EMPTYSTACK;
  else
  {
   double a = stack_peek(numstack);

   printf("Peeking %f\n", a);
   ret = NOERRORS;
  }
 }
 else if(!strcmp(op, "pop"))
 {
  if(stack_get_empty(numstack)<1)
   ret = EMPTYSTACK;
  else
  {
   double a = stack_pop(numstack);

   ret = NOERRORS;
  }
 }
 else if(!strcmp(op, "swp"))
 {
  if(stack_get_empty(numstack)<2)
   ret = EMPTYSTACK;
  else
  {
   stack_swap(numstack);
   ret = NOERRORS;
  }
 }
 else if(!strcmp(op, "chm"))
 {
  printf("Memory %f\n", *mem);
 }
 else if(!strcmp(op, "pem"))
 {
  if(stack_get_empty(numstack)<1)
   ret = EMPTYSTACK;
  else
  {
   *mem = stack_peek(numstack);
   ret = NOERRORS;
  }
 }
 else if(!strcmp(op, "pom"))
 {
  if(stack_get_empty(numstack)<1)
   ret = EMPTYSTACK;
  else
  {
   *mem = stack_pop(numstack);
   ret = NOERRORS;
  }
 }
 else if(!strcmp(op, "pum"))
 {
  if(stack_get_full(numstack))
   ret = FULLSTACK;
  else
  {
   stack_push(numstack, *mem);
   ret = NOERRORS;
  }
 }
 else if(!strcmp(op, "q"))
 {
  printf("Quitting\n");
  ret = QUITFLAG;
 }
 else
  ret = BADTOKEN;
 return ret;
}

int is_number(char* num)
{
 if(NUMMATCH1(num) || NUMMATCH2(num) || NUMMATCH3(num))
  return 1;
 else
  return 0;
}

int get_tokens(stack* numstack, double* mem, int verbose)
{
 int ret;
 char buffer[LINE_LENGTH];

 if(fgets(buffer, LINE_LENGTH, stdin)==NULL)
  ret = QUITFLAG;
 else
 {
  char* p;
  int i;
  char* tokens[LINE_LENGTH];

  ret = NOERRORS;
  if(verbose)
   printf(" %s", buffer);

  tokens[0] = strtok_r(buffer, WHITE, &p);
  for(i = 1; (i<LINE_LENGTH) && (tokens[i-1]!=NULL); i++)
  {
   tokens[i] = strtok_r(NULL, WHITE, &p);
  }

  for(i = 0; tokens[i]!=NULL; i++)
  {
   if(is_number(tokens[i]))
   {
    double tmp;
    tmp = stack_push(numstack, atof(tokens[i]));
    if(tmp==NAN)
     printf("\nFull stack");
    if(verbose)
     printf("%f ", stack_peek(numstack));
   }
   else
   {
    int err;

    if(0<i && verbose && is_number(tokens[i-1]))
     printf("\n");
    err = execute_op(numstack, mem, tokens[i]);
    if(verbose && !TOKISPEK && !TOKISCHM)
    {
     printf("%3s:", tokens[i]);
     if(stack_get_empty(numstack))
      printf("%f\n", stack_peek(numstack));
     else if(err==NOERRORS || err==QUITFLAG)
      printf("NULL\n");
    }
    if(err==BADTOKEN)
     printf("Unexpected token\n");
    else if(err==EMPTYSTACK)
     printf("Empty stack\n");
    else if(err==FULLSTACK)
     printf("Full stack\n");
    else if(err==QUITFLAG)
     ret = QUITFLAG;
   }
  }
  if(0<i && verbose && is_number(tokens[i-1]))
   printf("\n");
  if(!verbose && stack_get_empty(numstack)) /*if not verbose print result of each line*/
   printf("End of line yields %f\n", stack_peek(numstack));
  else if(!verbose && !stack_get_empty(numstack))
   printf("End of line yields empty stack\n");
 }
 return ret;
}


int main(int argc, char** argv)
{
 int ret;
 int verbose;
 int quit;
 double mem;
 stack* numstack;

 quit = 0;
 verbose = 0;

 if(argc>1)
 {
  if(ARG1HELP)
  {
   execlp(CAT, CAT, HELPTXT, NULL);
   quit = 1;
   ret = 0;
  }
  else if(ARG1VERB)
   verbose = 1;
 }

 if(!quit)
  numstack = stack_inst_ui(STACK_SIZE);
 while(!quit)
 {
  printf(":");
  if(get_tokens(numstack, &mem, verbose)==QUITFLAG)
   quit = 1;
 }
ret = 0;
return ret;
}
