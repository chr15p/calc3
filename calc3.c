#define _GNU_SOURCE

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>




typedef struct _cell{
   struct _cell ** operand;
   double value;
   unsigned int length;
   //char * operator;
   double (* operator)(struct _cell *);
} Cell;



Cell *plus(char ** statement);
Cell *subtract(char ** statement);
//Cell *number(char ** statement);
Cell *number(int number, char ** statement);
double eval_plus(Cell *c);
double eval_subtract(Cell *c);
double eval_value(Cell *c);
double eval_multiply(Cell *c);
double eval_divide(Cell *c);
double eval_power(Cell *c);
double evaluate(Cell* c);

double eval_avg(Cell *c);
double eval_max(Cell *c);

Cell * expression(int number ,char ** statement);


struct operator {
    char * symbol;
    double (* eval)(Cell *);
};


struct operator addition[]={
    {"-", &eval_subtract},
    {"+", &eval_plus},
    {NULL,NULL}
};

struct operator multiply[]={
    {"*", &eval_multiply},
    {"/", &eval_divide},
    {NULL,NULL}
};

struct operator power[]={
    {"^", eval_power},
    {NULL,NULL}
};

struct operator value[]={
    {"", &eval_value},
    {NULL,NULL}
};

struct operator symboltable[]={
    {"avg",&eval_avg},
    {"max",&eval_max}
};

struct precedencelevel {
    Cell* (* parse)(int num,char **statement);
    struct operator *value;
};

struct precedencelevel precedence[] = {
    { &expression, addition},
    { &expression, multiply}, 
    { &expression, power}, 
    { &number, value} 
};




char ** ltrim(char ** statement){
    while((**statement) == ' ' && (**statement) != 0){
        (*statement)++;
    }
    return statement;
}


char is_symbol(char* token,char **statement){
    statement = ltrim(statement);
    int len=strlen(token);
    if((len > 0) && (strncmp(*statement,token,len)==0)){
        return 1;
    }
    return 0;
}


Cell * new_cell(int len){
    Cell * c = (Cell *) malloc(sizeof(Cell));
    if (c == NULL){
		fprintf(stderr,"failed to cell allocate memory\n");
		exit(1);
    }

    if (len >0){
        c->operand = (Cell **) calloc(len + 1, sizeof(Cell*));
        if (c == NULL){
		    fprintf(stderr,"failed to operand allocate memory\n");
    		exit(1);
        }
    }
    c->length=len;
    return c;
}


//double (*(Cell *)) getevalfuction(statement,struct operator *value){
//typedef double (*evalfunc)(Cell *);
//evalfunc getevalfuction(statement,struct operator *value){
//double (*evalfunc)(Cell *) getevalfuction(statement,struct operator *value){
int getevalfuction(char **statement,struct operator *value){
    int x=0;

    while(value[x].symbol != NULL){
        if(is_symbol(value[x].symbol, statement)){
            return x;
        }
        x++;
    }
    return -1;
}


Cell * expression(int level,char **statement){
    Cell * c;
    Cell * e;
    int x=0;

    //printf("expression level=%d statement=%s\n",level,*statement);

    e = precedence[level].parse(level+1,statement);
    while((x=getevalfuction(statement,precedence[level].value )) != -1){
        (*statement) += 1;
        c = new_cell(2);
        c->operator = precedence[level].value[x].eval;
        c->operand[0] = e; 
        c->operand[1] = precedence[level].parse(level+1,statement);
        //printf("returned to level=%d statement=%s\n",level,*statement);
        e=c;
    }
    //printf("exit expression level=%d statement=%s\n",level,*statement);
    return e;
}


Cell * array(char ** statement){
    Cell * c;

    //printf("subtract=%s\n",*statement);
    //e = opprecedence[number+1].parse(number+1,statement);
    c = new_cell(1);
    c->operand[0] = precedence[0].parse(0,statement);
        //c->operator="-";
    //c->operator=opprecedence[number].eval;
//    c->operand[0] = e; 
    //printf("array=%s\n",*statement);
    while(strncmp(*statement,",",1)==0){
        //printf("array=%s\n",*statement);
        (*statement) += 1;
        //c->operand = reallocarray(c->value, c->length+1, sizeof(Cell*));
        c->length++;
        c->operand = (Cell **) realloc(c->operand, (c->length)*sizeof(Cell*));
        c->operand[(c->length)-1] = precedence[0].parse(0,statement);
    }
    return c;
}


//Cell * number(char ** statement){
Cell * number(int num, char ** statement){
    char * endptr;
    Cell * c;
    size_t len;

    //printf("number=%s\n",*statement);
    statement = ltrim(statement);
    double value = strtod(*statement,&endptr);	//try and read a number
    //printf("value=%e\n",value);

	if(*statement != endptr){	
        //printf("a\n");
        *statement = endptr;
        c = new_cell(0);
        //c->operator = 0;
        c->operator = &eval_value;
        //printf("value=%e\n", value);
        c->value = value;
        c->length = 1;
    }else if(**statement=='('){
        //printf("b\n");
        (*statement)++;
        //printf("statement1=%s\n",*statement);
        c = precedence[0].parse(0,statement);
        if(**statement!=')'){
    		fprintf(stderr,"unmatched parenthesis\n");
        }
        (*statement)++;
        //printf("statement2=%s\n",*statement);
    }else if((len=strspn(*statement,"abcdefghijklmnopqrstuvwxyz"))!=0){
        //printf("c\n");
        //printf("len=%ld statement=%s\n",len,*statement);
        if(*((*statement)+len)=='('){
            //run function
            int i=0;
            while((symboltable[i].symbol!=NULL)&&(strncmp(symboltable[i].symbol,*statement,len)!=0)){
                i++;
            }
            (*statement)+=len+1;
            c = array(statement);
            c->operator = symboltable[i].eval;
            if(**statement!=')'){
    		    fprintf(stderr,"unmatched parenthesis\n");
            }
            (*statement)++;
        }else if(*((*statement)+len)=='='){
            printf("= statement=%s\n",*statement);
            //assign to symbol
        }else{
		fprintf(stderr,"failed to comprehend at %s\n",*statement);

        }
        //printf("statement2=%s\n",*statement);
    }else{
		fprintf(stderr,"failed to read value at %s\n",*statement);
		exit(1);
    }
    //printf("return \n");
    return c;
}

double eval_avg(Cell *c){
    double total=0;
    //printf("c->length=%d\n",c->length);
    for(int i=0; i < c->length; i++){
        total += evaluate(c->operand[i]);
        //printf("total=%e\n",total);
    }
    //printf("avg=%e\n",total/c->length);
    return total/c->length;
}

double eval_max(Cell *c){
    double max=0;
    double cur=0;
    for(int i=0; i < c->length; i++){
        cur = evaluate(c->operand[i]);
        //printf("cur=%e max=%e\n",cur,max);
        if((i==0) || (cur > max)){
            max=cur;
        }
    }
    //printf("max=%e\n",max);

    return max;
}

double eval_plus(Cell *c){
    return evaluate(c->operand[0]) + evaluate(c->operand[1]);
}

double eval_subtract(Cell *c){
    return evaluate(c->operand[0]) - evaluate(c->operand[1]);
}

double eval_multiply(Cell *c){
    return evaluate(c->operand[0]) * evaluate(c->operand[1]);
}

double eval_divide(Cell *c){
    return evaluate(c->operand[0]) / evaluate(c->operand[1]);
}

double eval_power(Cell *c){
    return pow(evaluate(c->operand[0]),evaluate(c->operand[1]));
}

double eval_value(Cell *c){
    return c->value;
}

double evaluate(Cell* c){
    //printf("evaluate");
    return c->operator(c);
}



int main(int argc, char* argv[]){

    if(argc==1){
      printf("usage: %s [expression]\n",argv[0]);
      exit(1);
    }
    int arglen=0;
    for(int i=1;i<argc;i++){
        arglen+=strlen(argv[i]);
    }
    char * statement = calloc(sizeof(char),arglen);
    for(int i=1;i<argc;i++){
        strcat(statement,argv[i]); 
    }

//    char *statement=" 1+1-3+max(1,5,3)";
    //char *statement=" 1-2+3";
    printf("%s\n",statement);
//    Cell* c = subtract(&statement);
    Cell* c = precedence[0].parse(0,&statement);

    printf("=%lf\n",evaluate(c));
    
}






