#include <stdio.h>
#include <stdlib.h>

#define T_NUMBER 1001
#define T_OP     1002		

int token ;			// Here we store the current token/literal 
int number ;			// and the value of the number 

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
	return (token) ;	// returns a Literal
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


int ParseNumber () 			// Parsing Non Terminals and some Tokens require more
{							// complex functions
	MatchSymbol (T_NUMBER) ;
	return number ;
}


int ParseTerm (){		// T ::= N       returns the numeric value of the Term
	int val ;

	rd_lex () ;
	val = ParseNumber () ;

	return val ;
}


int ParseExpression () 			// E ::= TE' + E' ::= lambda | E 
{								// returns the numeric value of the Expression
	int val ;
	int val2 ;
	int operator ;

	val = ParseTerm () ;

//      ParseExpressionRest () ;			// we wxpand this function into ParseExpression()

	rd_lex () ;				// ExpressionRest is a nullable Non Terminal
	if (token == '\n'){	// Therefore, we check FOLLOW(ExpressionRest)
		return val ;		// This means that lambda has been derived
	}

	switch (token){		// Expression_Rest derives in alternatives
		case '+' :  		// requires checking FIRST(ExpressionRest))
		case '-' :
		case '*' :
		case '/' :  operator = token ;	// remember the operator for later
			    	break ;
		default :   rd_syntax_error (token, 0, "Token %d was read, but an Operator was expected");
			     	break ;
	}

	val2 = ParseExpression () ;
								// At this point the input has been parsed correctly
	switch (operator){			// This part is for the Semantic actions
		case '+' :  val += val2 ;
			    	break ;
		case '-' :  val -= val2 ;
			    	break ;
		case '*' :  val *= val2 ;
			    	break ;
		case '/' :  val /= val2 ;
			    	break ;
		default :   rd_syntax_error (operator, 0, "Unexpected error in ParseExpressionRest for operator %c\n") ;
			    	break ;
	}
	
	return val ;
}


int main (void){

	while (1){
		printf ("Valor %d OK\n", ParseExpression ()) ;
	}	
	
	system ("PAUSE") ;
}

