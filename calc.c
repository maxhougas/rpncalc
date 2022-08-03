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

#define ISNUM(a) (NUMMATCH1(a) || NUMMATCH2(a) || NUMMATCH3(a))

#define HASH(op,l) ((l>0?op[0]:0) + (l>1?256*op[1]:0) + (l>2?256*256*op[2]:0))

#define LN 28268
#define EXP 7370853
#define COS 7565155
#define ACO 7299937
#define SIN 7235955
#define ASI 6910817
#define TAN 7233908
#define ATA 6386785
#define AT2 3306593
#define COH 6844259
#define ACH 6841185
#define SIH 6842739
#define ASH 6845281
#define TAH 6840692
#define ATH 6845537
#define ABS 7561825
#define HYP 7371112
#define CEI 6907235
#define FLO 7302246
#define CLR 7498851
#define LEN 7234924
#define PEK 7038320
#define POP 7368560
#define SWP 7370611
#define CHM 7170147
#define PEM 7169392
#define POM 7171952
#define PUM 7173488
#define QUI 6911345

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

double stack_peek(stack* this, double* out)
{
 if(!this->p)
 {
  printf("Error empty stack\n");

  return NAN;
 }
 else
 {
  *out = this->data[this->p-1];

  return NOERRORS;
 }
}

int stack_pop(stack* this, double* out)
{
 if(!this->p)
 {
  printf("Error empty stack\n");

  return NAN;
 }
 else
 {
  this->p--;
  *out = this->data[this->p];

  return NOERRORS;
 }
}

int stack_push(stack* this, double data)
{
 if(this->p==this->max)
 {
  printf("Error full stack\n");

  return FULLSTACK;
 }
 else
 {
  this->data[this->p] = data;
  this->p++;
  
  return NOERRORS;
 }
}

int stack_swap(stack* this) /*swap top two on this*/
{
 if(this==NULL || this->data==NULL)
  return UNSPECIFIED;
 else if(this->p<2)
  return EMPTYSTACK;
 else
 {
  double tmp = this->data[this->p-1];
  this->data[this->p-1] = this->data[this->p-2];
  this->data[this->p-2] = tmp;

  return NOERRORS;
 }
}

int stack_view(stack* this, int direction) /*use BOTTOM_UP and TOP_DOWN*/
{
 int i;

 if(direction == BOTTOM_UP)
 {
  for(i=0;i<this->p;i++)
   printf("% f\n",this->data[i]);
  return NOERRORS;
 }
 else if(direction == TOP_DOWN)
 {
  for(i=this->p-1;i<-1;i--)
   printf("% f\n",this->data[i]);
  return NOERRORS;
 }
 else
  return UNSPECIFIED;
}

int execute_op(stack* numstack, double* mem, char* op)
{

 switch(HASH(op,strlen(op)))
 {

  case 'q':
   printf("Quitting\n");
   return QUITFLAG;
  break;

  case 'e':
   if(stack_get_full(numstack))
    return FULLSTACK;
   else
    stack_push(numstack, M_E);
  break;

  case 't':
   if(stack_get_full(numstack))
    return FULLSTACK;
   else
    stack_push(numstack, TAU);
  break;

  case 'd':
   if(stack_get_full(numstack))
    return FULLSTACK;
   else
    stack_push(numstack, DEE);
  break;

  case '+':
   if(stack_get_empty(numstack)<2)
    return EMPTYSTACK;
   else
   {
    double a,b;
    stack_pop(numstack,&b);
    stack_pop(numstack,&a);
    stack_push(numstack, a+b);
   }
  break;

  case '-':
   if(stack_get_empty(numstack)<2)
    return EMPTYSTACK;
   else
   {
    double a,b;
    stack_pop(numstack,&b);
    stack_pop(numstack,&a);
    stack_push(numstack, a-b);
   }
  break;


  case '*':
   if(stack_get_empty(numstack)<2)
    return EMPTYSTACK;
   else
   {
    double a,b;
    stack_pop(numstack,&b);
    stack_pop(numstack,&a);
    stack_push(numstack, a*b);
   }
  break;

  case '/':
   if(stack_get_empty(numstack)<2)
    return EMPTYSTACK;
   else
   {
    double a,b;
    stack_pop(numstack,&b);
    stack_pop(numstack,&a);
    stack_push(numstack, a/b);
   }
  break;

  case '^':
   if(stack_get_empty(numstack)<2)
    return EMPTYSTACK;
   else
   {
    double a,b;
    stack_pop(numstack,&a);
    stack_pop(numstack,&b);
    stack_push(numstack,pow(a,b));
   }
  break;

  case '\\':
   if(stack_get_empty(numstack)<2)
    return EMPTYSTACK;
   else
   {
    double a,b;
    stack_pop(numstack,&a);
    stack_pop(numstack,&b);
    stack_push(numstack,floor(a/b));
   }
  break;

  case '%':
   if(!stack_get_empty(numstack))
    return EMPTYSTACK;
   else
   {
    double a,b;
    stack_pop(numstack,&a);
    stack_pop(numstack,&b);
    stack_push(numstack,fmod(a,b));
   }
  break;

  case LN:
   if(!stack_get_empty(numstack))
    return EMPTYSTACK;
   else
   {
    double a;
    stack_pop(numstack,&a);
    stack_push(numstack,log(a));
   }
  break;

  case EXP:
   if(!stack_get_empty(numstack))
    return EMPTYSTACK;
   else
   {
    double a;
    stack_pop(numstack,&a);
    stack_push(numstack,exp(a));
   }
  break;

  case COS:
   if(!stack_get_empty(numstack))
    return EMPTYSTACK;
   else
   {
    double a;
    stack_pop(numstack,&a);
    stack_push(numstack,cos(a));
   }
  break;

  case ACO:
   if(!stack_get_empty(numstack))
    return EMPTYSTACK;
   else
   {
    double a;
    stack_pop(numstack,&a);
    stack_push(numstack,acos(a));
   }
  break;

  case SIN:
   if(!stack_get_empty(numstack))
    return EMPTYSTACK;
   else
   {
    double a;
    stack_pop(numstack,&a);
    stack_push(numstack,sin(a));
   }
  break;

  case ASI:
   if(!stack_get_empty(numstack))
    return EMPTYSTACK;
   else
   {
    double a;
    stack_pop(numstack,&a);
    stack_push(numstack,sin(a));
   }
  break;

  case TAN:
   if(!stack_get_empty(numstack))
    return EMPTYSTACK;
   else
   {
    double a;
    stack_pop(numstack,&a);
    stack_push(numstack,tan(a));
   }
  break;

  case ATA:
   if(!stack_get_empty(numstack))
    return EMPTYSTACK;
   else
   {
    double a;
    stack_pop(numstack,&a);
    stack_push(numstack,atan(a));
   }
  break;

  case AT2:
   if(stack_get_empty(numstack)<2)
    return EMPTYSTACK;
   else
   {
    double a,b;
    stack_pop(numstack,&b);
    stack_pop(numstack,&a);
    stack_push(numstack,atan2(a,b));
   }
  break;

  case COH:
   if(!stack_get_empty(numstack))
    return EMPTYSTACK;
   else
   {
    double a;
    stack_pop(numstack,&a);
    stack_push(numstack,cosh(a));
   }
  break;

/* inverse hyperbolic functions not ANSI compliant
  case ACH:
   if(!stack_get_empty(numstack))
    return EMPTYSTACK;
   else
   {
    double a;
    stack_pop(numstack,&a);
    stack_push(numstack,acosh(a));
   }
  break;
*/

  case SIH:
   if(!stack_get_empty(numstack))
    return EMPTYSTACK;
   else
   {
    double a;
    stack_pop(numstack,&a);
    stack_push(numstack,sinh(a));
   }
  break;

/*
  case ASH:
   if(!stack_get_empty(numstack))
    return EMPTYSTACK;
   else
   {
    double a;
    stack_pop(numstack,&a);
    stack_push(numstack,asinh(a));
   }
  break;
*/

  case TAH:
   if(!stack_get_empty(numstack))
    return EMPTYSTACK;
   else
   {
    double a;
    stack_pop(numstack,&a);
    stack_push(numstack,tanh(a));
   }
  break;

/*
  case ATH:
   if(!stack_get_empty(numstack))
    return EMPTYSTACK;
   else
   {
    double a;
    stack_pop(numstack,&a);
    stack_push(numstack,atanh(a));
   }
  break;
*/

  case ABS:
   if(!stack_get_empty(numstack))
    return EMPTYSTACK;
   else
   {
    double a;
    stack_pop(numstack,&a);
    stack_push(numstack,fabs(a));
   }
  break;

/*
  case HYP:
   if(stack_get_empty(numstack)<2)
    return EMPTYSTACK;
   else
   {
    double a,b;
    stack_pop(numstack,&a);
    stack_pop(numstack,&b);
    stack_push(numstack,hypot(a,b));
   }
  break;
*/

  case CEI:
   if(!stack_get_empty(numstack))
    return EMPTYSTACK;
   else
   {
    double a;
    stack_pop(numstack,&a);
    stack_push(numstack,ceil(a));
   }
  break;

  case FLO:
   if(!stack_get_empty(numstack))
    return EMPTYSTACK;
   else
   {
    double a;
    stack_pop(numstack,&a);
    stack_push(numstack,floor(a));
   }
  break;

  case CLR:
   stack_clear(numstack);
  break;

  case LEN:
   stack_push(numstack,stack_get_empty(numstack));
  break;

  case PEK:
   if(!stack_get_empty(numstack))
    return EMPTYSTACK;
   else
   {
    double a;
    stack_peek(numstack,&a);
    printf("Peeking %f\n",a);
   }
  break;

  case POP:
   if(!stack_get_empty(numstack))
    return EMPTYSTACK;
   else
    numstack->p--;
  break;

  case SWP:
   if(stack_get_empty(numstack)<2)
    return EMPTYSTACK;
   else
    stack_swap(numstack);
  break;

  case CHM:
   printf("Memory %f\n", *mem);
  break;

  case PEM:
   if(!stack_get_empty(numstack))
    return EMPTYSTACK;
   else
   {
    double a;
    stack_peek(numstack,&a);
    *mem = a;
   }
  break;

  case POM:
   if(!stack_get_empty(numstack))
    return EMPTYSTACK;
   else
   {
    double a;
    stack_pop(numstack,&a);
    *mem = a;
   }
  break;

  case PUM:
   if(stack_get_full(numstack))
    return FULLSTACK;
   else
    stack_push(numstack,*mem);
  break;

  case QUI:
   printf("Quitting\n");
   return QUITFLAG;
  break;

  default:
   return BADTOKEN;
 }

 return NOERRORS;
}

int is_number(char* num)
{
 return (NUMMATCH1(num) || NUMMATCH2(num) || NUMMATCH3(num));
}

int get_tokens(stack* numstack, double* mem, int verbose)
{
 int ret;
 char buffer[LINE_LENGTH];

 if(fgets(buffer, LINE_LENGTH, stdin)==NULL)
  ret = QUITFLAG;
 else
 {
  /*char* p = NULL;*/
  int i;
  char* tokens[LINE_LENGTH];

  ret = NOERRORS;
  if(verbose)
   printf("%s\n", buffer);

  tokens[0] = strtok(buffer, WHITE);
  for(i = 1; (i<LINE_LENGTH) && (tokens[i-1]!=NULL); i++)
  {
   tokens[i] = strtok(NULL, WHITE);
   /*printf(" %s",tokens[i-1]);*/
  }
  /*printf("\n");*/

  for(i = 0; tokens[i]!=NULL; i++)
  {
   if(is_number(tokens[i]))
   {
    int err;

    err = stack_push(numstack, atof(tokens[i]));
    if(err==FULLSTACK)
     printf("Full stack\n");
    if(verbose)
    {
     double a;
     stack_peek(numstack,&a);
     printf("%f\n",a);
    }
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
     {
      double a;
      stack_peek(numstack,&a);
      printf("%f\n",a);
     }
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
  {
   double a;
   stack_peek(numstack,&a);
   printf("End of line yields %f\n",a);
  }
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
