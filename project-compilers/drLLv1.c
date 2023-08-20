#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>

#define T_NUMBER 	1001
#define T_OPERATOR	1002		

int ParseExpression () ;		// Prototype for forward reference

int token ;		// Here we store the current token/literal 
int number ;		// the value of the number 
int token_val ;		// or the arithmetic operator
			// TO DO: Pack these variables in a struct

int line_counter = 1 ;


int rd_lex ()
{
	int c ;
	
	do {
		c = getchar () ;
		if (c == '\n')
			line_counter++ ;	// info for rd_syntax_error()
	} while (c == ' ' || c == '\t' || c == '\r') ;

	if (isdigit (c)) {
		ungetc (c, stdin) ;
		scanf ("%d", &number) ;
		token = T_NUMBER ;
		return (token) ;	// returns the Token for Number
	}

	if (c == '+' || c == '-' || c == '*' || c == '/') {
		token_val = c ;
		token = T_OPERATOR ;
		return (token) ;
	}				// returns the Token for Arithmetic Operators
	
	token = c ;
	return (token) ;		// returns a literal
}


void rd_syntax_error (int expected, int token, char *output) 
{
	fprintf (stderr, "ERROR in line %d ", line_counter) ;
	fprintf (stderr, output, expected, token) ;

	exit (0) ;
}


void MatchSymbol (int expected_token)
{
	if (token != expected_token) {
		rd_syntax_error (expected_token, token, "token %d expected, but %d was read") ;
	}
	rd_lex () ;
}


#define ParseLParen() 	MatchSymbol ('(') ; // More concise and efficient definitions
#define ParseRParen() 	MatchSymbol (')') ; // rather than using functions
					// This is only useful for matching Literals


int ParseNumber () 			// Parsing Non Terminals and some Tokens require more
{							// complex functions
	int val = number ;	    // store number value to avoid losing it when reading
	MatchSymbol (T_NUMBER) ; // a second number
	return val ;
}

int ParseOperator () 
{
	int val = token_val ;
	MatchSymbol (T_OPERATOR) ;
	return val ;
}

int ParseOpExpression () // TO DO: make | (E) & check that the right one is returned
{
	int val ;
	int val2 ;
	int operator ;

	if (token == T_NUMBER) {	
		
		val = ParseNumber () ;
		val2 = ParseNumber () ;	


	} else {
		
		ParseLParen () ;
		val = ParseExpression () ;
		ParseRParen () ;

		if (token == T_NUMBER){

			val2 = ParseNumber () ;

		} else {

			ParseLParen () ;
			val2 = ParseExpression () ;
			ParseRParen () ;

		}

	}

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
	return val;

}

int ParseExpression () 		
{							
	int val ;
	int operator ;
	operator = ParseOperator () ;
	val=ParseOpExpression () ;
	return val;
}

int ParseAxiom () 		
{
	int val ;
	ParseLParen () ;
	val = ParseExpression () ;
	ParseRParen () ;
	return val ;
}


int main (void)
{

	while (1) {
		rd_lex () ;
        printf ("Valor %d\n", ParseAxiom ()) ;
	}	
	
	system ("PAUSE") ;
}