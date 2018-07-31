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

    //printf("subtract=%s\n",*line);
    e = opcodes[number+1].parse(number+1,line);
    while(is_symbol(opcodes[number].symbol, line)==1){
        (*line) += strlen(opcodes[number].symbol);
        //printf("line=%s ===%s\n",*line, opcodes[number].symbol);
        c = new_cell(2);
        //c->operator="-";
        c->operator=opcodes[number].eval;
        c->operand[0] = e; 
        c->operand[1] = opcodes[number+1].parse(number+1,line);
        e=c;
    }
    return e;
}


Cell * array(int number ,char ** line){
    Cell * c;
    Cell * e;

    //printf("subtract=%s\n",*line);
    e = opcodes[number+1].parse(number+1,line);
    while(strncmp(",",1))==0){
        (*line) += 1 
        //printf("line=%s ===%s\n",*line, opcodes[number].symbol);
        c = new_cell(2);
        //c->operator="-";
        c->operator = array()
        c->operand[0] = e; 
        c->operand[1] = opcodes[number+1].parse(number+1,line);
        e=c;
    }
    return e;
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

    //printf("number=%s\n",*line);
    line = ltrim(line);
    double value = strtod(*line,&endptr);	//try and read a number
    //printf("value=%e\n",value);

	if(*line != endptr){	
        *line = endptr;
        c = new_cell(0);
        //c->operator = 0;
        c->operator = &eval_value;
        c->value = value;
    }else if(**line=='('){
        (*line)++;
        //printf("line1=%s\n",*line);
        c = opcodes[0].parse(0,line);
        if(**line!=')'){
    		fprintf(stderr,"unmatched parenthesis\n");
        }
        (*line)++;
        //printf("line2=%s\n",*line);
    }else{
		fprintf(stderr,"failed to read value at %s\n",*line);
		exit(1);
    }
    return c;
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

//    char *line=" 1+ 1- -3";

//    Cell* c = subtract(&line);
    Cell* c = opcodes[0].parse(0,&line);

    printf("=%lf\n",evaluate(c));
    
}






