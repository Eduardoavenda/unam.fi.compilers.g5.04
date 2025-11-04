#include <stdio.h>
#include <string.h>

typedef enum {KW, ID, OP, NUM, LIT, PUNC,  
              TIPO, VAR, ASIGN, DECL, SSYM
} Sym;

const char* sym_name[] = {
    "KW","ID","OP","NUM","LIT","PUNC","TIPO","VAR","ASIGN","DECL","S"
};

#define MAXTOK 4096


typedef struct { Sym v[MAXTOK]; int top; } Stack;
void s_init(Stack* s){ s->top = -1; }
int  s_size(Stack* s){ return s->top + 1; }
void s_push(Stack* s, Sym x){ s->v[++s->top] = x; }
Sym  s_peek(Stack* s, int i){ return s->v[s->top - i]; } 
void s_popn(Stack* s, int n){ s->top -= n; }


int assign_from_literal = 0;


int try_reduce(Stack* st){

    if (s_size(st) >= 3 &&
        s_peek(st,0)==PUNC && s_peek(st,1)==NUM && s_peek(st,2)==OP){
        s_popn(st,3);
        s_push(st,ASIGN);
        return 1;
    }

    if (s_size(st) >= 3 &&
        s_peek(st,0)==PUNC && s_peek(st,1)==LIT && s_peek(st,2)==OP){
        s_popn(st,3);
        s_push(st,ASIGN);
        assign_from_literal = 1; 
        return 1;
    }

    if (s_size(st) >= 1 && s_peek(st,0)==KW){
        s_popn(st,1);
        s_push(st,TIPO);
        return 1;
    }

    if (s_size(st) >= 1 && s_peek(st,0)==ID){
        s_popn(st,1);
        s_push(st,VAR);
        return 1;
    }

    if (s_size(st) >= 3 &&
        s_peek(st,0)==ASIGN && s_peek(st,1)==VAR && s_peek(st,2)==TIPO){
        s_popn(st,3);
        s_push(st,DECL);
        return 1;
    }

    if (s_size(st) >= 1 && s_peek(st,0)==DECL){
        s_popn(st,1);
        s_push(st,SSYM);
        return 1;
    }
    return 0;
}


int map_token(const char* w, Sym* out){
    if      (strcmp(w,"Keyword")==0)     { *out = KW;   return 1; }
    else if (strcmp(w,"Identifier")==0)  { *out = ID;   return 1; }
    else if (strcmp(w,"Operator")==0)    { *out = OP;   return 1; }
    else if (strcmp(w,"Constant")==0)    { *out = NUM;  return 1; }
    else if (strcmp(w,"Literal")==0)     { *out = LIT;  return 1; }
    else if (strcmp(w,"Punctuation")==0) { *out = PUNC; return 1; }

    else if (!strcmp(w,"Total") || !strcmp(w,"of") || !strncmp(w,"tokens",6) )
        return 0;
    return -1; 
    
}

int main(void){
    Stack st; s_init(&st);
    char w[128];

    while (scanf("%127s", w)==1){
        Sym t;
        int m = map_token(w, &t);
        if (m == 0) continue;      
        if (m < 0)  continue;      


        s_push(&st, t);

        while (try_reduce(&st)) {}
    }

    
    while (try_reduce(&st)) {}

    if (s_size(&st)==1 && s_peek(&st,0)==SSYM){
        printf("---------\nParsing Success! :) \n");
        if (assign_from_literal){
            printf("\nSDT error---------");
        } else {
            printf("SDT Verified!\n -------- ");
        }
        return 0;
    } else {
        printf("Parsing error... :( \n");
        return 1;
    }
}
