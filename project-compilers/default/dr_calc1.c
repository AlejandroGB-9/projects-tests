#include <stdio.h>
#include <stdlib.h>

#define T_NUMBER 1001
#define T_OP     1002		

int token ;			// Here we store the current token/literal 
int number ;		// and the value of the number 

int line_counter = 1 ;

int rd_lex ()
{
	int c ;
	
	do {
		c = getchar () ;
	} while (c == ' ' || c == '\t') ;
	
	if (isdigit (c)){
		ungetc (c, stdin) ;
		scanf ("%d", &number) ;
		token = T_NUMBER ;
		return (token) ;	// returns the Token for Number
	}

	if (c == '\n')
		line_counter++ ;	// info for rd_syntax_error()
	
	token = c ;
	return (token) ;	// returns a literal
}


void rd_syntax_error (int expected, int token, char *output) 
{
	fprintf (stderr, "ERROR in line %d ", line_counter) ;
	fprintf (stderr, output, token, expected) ;
	
	exit (0) ;
}


void MatchSymbol (int expected_token)
{
	if (token != expected_token){
		rd_syntax_error (expected_token, token, "token %d expected, but %d was read") ;
	}
}


void ParseNumber () 			
{					        
	MatchSymbol (T_NUMBER) ;
	rd_lex () ;
}


void ParseTerm (){		// T ::= N   
	ParseNumber () ;	
}


void ParseExpression () ; 	// required prototype for forward reference for mutual recursion

void ParseExpressionRest ()	// E' ::= lambda | OpE 
{							// ExpressionRest is a nullable Non Terminal
	if (token == '\n'){	// Therefore, we check FOLLOW(ExpressionRest)
		return ;			// This means that lambda has been derived
	}

	switch (token){			// Expression_Rest derives in alternatives
		case '+' :  			// requires checking FIRST(ExpressionRest))
		case '-' :
		case '*' :
		case '/' :  
			    	break ;
		default :   rd_syntax_error (token, 0, "Token %d was read, but an Operator was expected");
			     	break ;
	}
	rd_lex () ;				

	ParseExpression () ;	// Forward reference in mutual recursion requires a previous prototye
}


void ParseExpression (){		// E ::= TE'
	ParseTerm () ;
	ParseExpressionRest () ;
}


int main (void){

	while (1){
		rd_lex () ;
		ParseExpression () ;
		printf ("OK\n") ;
	}	
	
	system ("PAUSE") ;
}

