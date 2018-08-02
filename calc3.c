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



Cell *plus(char ** line);
Cell *subtract(char ** line);
//Cell *number(char ** line);
Cell *number(int number, char ** line);
double eval_plus(Cell *c);
double eval_subtract(Cell *c);
double eval_value(Cell *c);
double eval_multiply(Cell *c);
double eval_divide(Cell *c);
double eval_power(Cell *c);
double evaluate(Cell* c);

double eval_avg(Cell *c);
double eval_max(Cell *c);

Cell * expression(int number ,char ** line);


struct ops {
    char * symbol;
    double (* eval)(Cell *);
    Cell* (* parse)(int num,char **line);
};


struct ops opcodes[]={
    {"-",&eval_subtract,&expression},
    {"+",&eval_plus,&expression},
    {"*",&eval_multiply,&expression},
    {"/",&eval_divide,&expression},
    {"^",eval_power,&expression},
    {"",&eval_value,&number}
};

struct ops symboltable[]={
    {"avg",&eval_avg,&expression},
    {"max",&eval_max,&expression},
};

char ** ltrim(char ** line){
    while((**line) == ' ' && (**line) != 0){
        (*line)++;
    }
    return line;
}


char is_symbol(char* token,char **line){
    line = ltrim(line);
    int len=strlen(token);
    if(strncmp(*line,token,len)==0){
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


Cell * expression(int number ,char ** line){
    Cell * c;
    Cell * e;

    e = opcodes[number+1].parse(number+1,line);
    while(is_symbol(opcodes[number].symbol, line)==1){
        (*line) += strlen(opcodes[number].symbol);
        c = new_cell(2);
        c->operator=opcodes[number].eval;
        c->operand[0] = e; 
        c->operand[1] = opcodes[number+1].parse(number+1,line);
        e=c;
    }
    return e;
}


Cell * array(char ** line){
    Cell * c;

    //printf("subtract=%s\n",*line);
    //e = opcodes[number+1].parse(number+1,line);
    c = new_cell(1);
    c->operand[0] = opcodes[0].parse(0,line);
        //c->operator="-";
    //c->operator=opcodes[number].eval;
//    c->operand[0] = e; 
    printf("array=%s\n",*line);
    while(strncmp(*line,",",1)==0){
        printf("array=%s\n",*line);
        (*line) += 1;
        //c->operand = reallocarray(c->value, c->length+1, sizeof(Cell*));
        c->length++;
        c->operand = (Cell **) realloc(c->operand, (c->length)*sizeof(Cell*));
        c->operand[(c->length)-1] = opcodes[0].parse(0,line);
    }
    return c;
}


/*
Cell * subtract(char ** line){
    Cell * c;
    Cell * e;

    //printf("subtract=%s\n",*line);
    e = plus(line);
    while(is_symbol("-",line)==1){
        (*line)++;
        c = new_cell(2);
        //c->operator="-";
        c->operator=opcodes[0].eval;
        c->operand[0] = e; 
        c->operand[1] = plus(line); 
        e=c;
    }
    return e;
}

Cell * plus(char ** line){
    Cell * c;
    Cell * e;

    //printf("plus=%s\n",*line);
    e = number(line);
    while(is_symbol("+",line)==1){
        (*line)++;
        c = new_cell(2);
        //c->operator="+";
        c->operator = opcodes[1].eval;
        c->operand[0] = e; 
        c->operand[1] = number(line); 
        e=c;
    }
    return e;
}
*/

//Cell * number(char ** line){
Cell * number(int num, char ** line){
    char * endptr;
    Cell * c;
    size_t len;

    //printf("number=%s\n",*line);
    line = ltrim(line);
    double value = strtod(*line,&endptr);	//try and read a number
    //printf("value=%e\n",value);

	if(*line != endptr){	
        *line = endptr;
        c = new_cell(0);
        //c->operator = 0;
        c->operator = &eval_value;
        printf("value=%e\n", value);
        c->value = value;
        c->length = 1;
    }else if(**line=='('){
        (*line)++;
        //printf("line1=%s\n",*line);
        c = opcodes[0].parse(0,line);
        if(**line!=')'){
    		fprintf(stderr,"unmatched parenthesis\n");
        }
        (*line)++;
        //printf("line2=%s\n",*line);
    }else if((len=strspn(*line,"abcdefghijklmnopqrstuvwxyz"))!=0){
        printf("len=%ld line=%s\n",len,*line);
        if(*((*line)+len)=='('){
            //run function
            int i=0;
            while((symboltable[i].symbol!=NULL)&&(strncmp(symboltable[i].symbol,*line,len)!=0)){
                i++;
            }
            (*line)+=len+1;
            c = array(line);
            c->operator = symboltable[i].eval;
            if(**line!=')'){
    		    fprintf(stderr,"unmatched parenthesis\n");
            }
            (*line)++;
        }else if(*((*line)+len)=='='){
            printf("= line=%s\n",*line);
            //assign to symbol
        }else{
		fprintf(stderr,"failed to comprehend at %s\n",*line);

        }
        printf("line2=%s\n",*line);
    }else{
		fprintf(stderr,"failed to read value at %s\n",*line);
		exit(1);
    }
    return c;
}

double eval_avg(Cell *c){
    double total=0;
    printf("c->length=%d\n",c->length);
    for(int i=0; i < c->length; i++){
        total += evaluate(c->operand[i]);
        printf("total=%e\n",total);
    }
    printf("avg=%e\n",total/c->length);
    return total/c->length;
}

double eval_max(Cell *c){
    double max=0;
    double cur=0;
    for(int i=0;i<c->length;i++){
        cur+=evaluate(c->operand[i]);
        if((i==0) || (cur>max)){
            max=cur;
        }
    }

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
    return c->operator(c);
}



int main(int argc, char* argv[]){

/*
    if(argc==1){
      printf("usage: %s [expression]\n",argv[0]);
      exit(1);
    }
    int arglen=0;
    for(int i=1;i<argc;i++){
        arglen+=strlen(argv[i]);
    }
    char * line = calloc(sizeof(char),arglen);
    for(int i=1;i<argc;i++){
        strcat(line,argv[i]); 
    }
*/
    char *line=" 1+ 1- -3+avg(1,2,3)";
    printf("%s\n",line);
//    Cell* c = subtract(&line);
    Cell* c = opcodes[0].parse(0,&line);

    printf("=%lf\n",evaluate(c));
    
}






