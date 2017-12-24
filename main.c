#include <stdio.h>
#include "ctype.h"
#include <stdlib.h>
#include <math.h>

typedef enum fn_id {
    bad_id,
    sin_id,
    cos_id
    } fn_id;

double eval_factor(char const *exp, char const** end);
double eval_term(char const* exp, char const** end);
double eval_exp(char const* exp, char const** end);

int error_occured = 0;

void error(char const* msg,char const* remainder){
    if(!error_occured){
        printf("%s at: %s\n", msg, remainder);
        error_occured = 1;
    }
}

//skip white space : delete white space in expression
char const* skipws(char const* exp) {
    while(isspace((int)(unsigned char)*exp)) {
        exp++;
    }
    return exp;
}

fn_id getfn(char const** x) {
    if((*x)[0] == 's' && (*x)[1] == 'i' && (*x)[2] == 'n' && !isalpha((*x)[3])) {
        (*x) += 3;
        return sin_id;
    }
    else if((*x)[0] == 'c' && (*x)[1] == 'o' && (*x)[2] == 's' && !isalpha((*x)[3])) {
        (*x) += 3;
        return cos_id;
    }
    else {
        error("Bad function name", *x);
        while(isalpha(**x)) {
            ++(*x);
        }
        return bad_id;
    }
}

double eval_factor(char const* exp, char const** end)
{
    double result;
    exp = skipws(exp);
    if(*exp == '(') {
        result = eval_exp(exp+1, end);
        if(**end != ')') {
            error("missing closing parenthesis", *end);
        } else {
            ++*end;
        }
    } else if(*exp == '+') {
        result = eval_factor(exp+1,end);
    } else if(*exp == '-') {
        result = -eval_factor(exp+1,end);
    } else if(isalpha(*exp)) {
        switch(getfn(&exp)) {
    case sin_id:
        result = sin(eval_factor(exp, end));
        break;
    case cos_id:
        result = cos(eval_factor(exp, end));
        break;
    case bad_id:
        break;
        }
    } else {
        char *endptr;
        result = strtod(exp, &endptr);
        *end = endptr;
        if(*end == exp) {
            error("invalid number", *end);
        }
    }
    return result;
}

double eval_term(char const* exp, char const** end){
    double result = eval_factor(exp, end);
    *end = skipws(*end);
    while(**end == '*' || **end == '/') {
        char const* op = *end;
        double factor = eval_factor(*end+1, end);
        if(*op == '*') {
            result *= factor;
        } else if(factor != 0.0) {
            result /= factor;
        } else {
            error("divide by 0.0", op);
        }
        *end = skipws(*end);
    }
    return result;
}

double eval_exp(char const* exp, char const** end) {
    double result = eval_term(exp, end);
    *end = skipws(*end);
    while(**end == '+' || **end == '-') {
        char const* op = *end;
        double term = eval_term(*end+1, end);
        if(*op == '+') {
            result += term;
        } else {
            result -= term;
        }
        *end = skipws(*end);
    }
    return result;
}

double eval(char const* exp) {
    char const* end;
    double result = eval_exp(exp, &end);
    if(*end != '\0') {
        error("trailing data after expression", end);
    }
    return result;
}
int main(int argc, char *argv[])
{
    while(*++argv != NULL) {
        double result;
        error_occured = 0;
        printf("evaluating %s\n -> ", *argv);
        result = eval(*argv);
        if(!error_occured) {
            printf("%.10g\n", result);
        }
    }

    return 0;
}
