/*
A lexical analyzer system for simple arithmetic expressions.

<ifstmt> -> if (<boolexpr>) '{'<assign>'}' [else '{'<assign>'}']
<boolexpr> -> <boolterm> {|| <boolterm>}
<boolterm> -> <boolfactor> {&& <boolfactor>}
<boolfactor> -> TRUE | FALSE | ! <boolfactor> | (<boolexpr>)
<assign> -> id = <expr>; {<assign>}
<expr> -> <term> {( +|- ) <term>}
<term> -> <factor> {( *|/|% ) <factor>}
<factor>-> id | int_constant | (<expr>)
*/

#include <iostream>
#include <fstream>
#include <cctype>
using namespace std;

/* Global declarations */
/* Variables */
int charClass;
char lexeme[100];
char nextChar;
int lexLen;
int token;
int nextToken;
int error = 0;
ifstream in_fp("syntax.txt");

/* Function declarations */
void getChar();
void addChar();
void getNonBlank();
int lex(); // This will get the nextToken 
void expr();
void term();
void factor();
void assign();
void booleanFactor(); 
void booleanTerm();
void booleanExpr();
void ifStatement();


/* Character classes */
#define LETTER 0
#define DIGIT 1
#define UNKNOWN 99

/* Token codes */
#define INT_LIT 10
#define IDENT 11
#define ADD_OP 21
#define SUB_OP 22
#define MULT_OP 23
#define DIV_OP 24
#define MOD_OP 25
#define ASSIGN_OP 26
#define SEMICOLON 27
#define LEFT_PAREN 28
#define RIGHT_PAREN 29
#define TRUE_STMT 30
#define FALSE_STMT 31
#define NOT_STMT 32
#define AND_STMT 33
#define OR_STMT 34
#define IF_STMT 35
#define ELSE_STMT 36
#define OPEN_BRACKET 37
#define CLOSE_BRACKET 38

/******************************************************/
/* main driver */
int main()
{
	/* Open the input data file and process its contents */

	if (in_fp.fail())
	{
		cout << "File could not be opened\n";
		cin.get();
		exit(1);
	}
	else {
		getChar();
		lex();
		do {
			ifStatement(); // Checking if statement
			lex();	// Getting the nextToken
		}
		while (nextToken != EOF);
	}
	in_fp.close();
	if (error == 0)
		cout << "----- 1 succeeded, 0 failed -----\n";
	else
		cout << "----- 0 succeeded " << error << " failed ----- \n";

	system("PAUSE");

	return 0;
}

/*****************************************************/
/* lookup - a function to lookup operators and parentheses
and return the token */
int lookup(char ch) {
	switch (ch) {
	case '(':
		addChar();
		nextToken = LEFT_PAREN;
		break;
	case ')':
		addChar();
		nextToken = RIGHT_PAREN;
		break;
	case '+':
		addChar();
		nextToken = ADD_OP;
		break;
	case '-':
		addChar();
		nextToken = SUB_OP;
		break;
	case '*':
		addChar();
		nextToken = MULT_OP;
		break;
	case '/':
		addChar();
		nextToken = DIV_OP;
		break;
	case '%':
		addChar();
		nextToken = MOD_OP;
		break;
	case '=':
		addChar();
		nextToken = ASSIGN_OP;
		break;
	case ';':
		addChar();
		nextToken = SEMICOLON;
		break;
	case '{':
		addChar();
		nextToken = OPEN_BRACKET;
		break;
	case '}':
		addChar();
		nextToken = CLOSE_BRACKET;
		break;
	default:
		addChar();
		nextToken = EOF;
		break;
	}
	return nextToken;
}

/*****************************************************/
/* addChar - a function to add nextChar to lexeme */
void addChar() {
	if (lexLen <= 98) {
		lexeme[lexLen++] = nextChar;
		lexeme[lexLen] = 0;
	}
	else
		cout << " Error - lexeme is too long \n";
}

/*****************************************************/
/* getChar - a function to get the next character of
input and determine its character class */
void getChar() {
	in_fp.get(nextChar);
	if (in_fp.eof())   // if no more character in the file
		nextChar = EOF;

	if (nextChar != EOF) {
		if (isalpha(nextChar))
			charClass = LETTER;
		else if (isdigit(nextChar))
			charClass = DIGIT;
		else if (nextChar == '&' || nextChar == '|' || nextChar == '!')
			charClass = LETTER;
		else charClass = UNKNOWN;
	}
	else
		charClass = EOF;
}

/*****************************************************/
/* getNonBlank - a function to call getChar until it
returns a non-whitespace character */
void getNonBlank() {
	while (isspace(nextChar))
		getChar();
}

/***************************************************** /
// lex - a simple lexical analyzer for arithmetic
expressions */
int lex() {
	lexLen = 0;
	getNonBlank();
	switch (charClass) {
	/* Parse identifiers */
	case LETTER:
		addChar();
		getChar();
		while (charClass == LETTER || charClass == DIGIT) {
			addChar();
			getChar();
		}
		nextToken = IDENT;

		if (lexeme[0] == 'T' && lexeme[1] == 'R' && lexeme[2] == 'U' && lexeme[3] == 'E') {
			nextToken = TRUE_STMT;
		}
		else {
			if (lexeme[0] == 'F' && lexeme[1] == 'A' && lexeme[2] == 'L' && lexeme[3] == 'S' && lexeme[4] == 'E') {
				nextToken = FALSE_STMT;
			}
			else if (lexeme[0] == '!') {
			nextToken = NOT_STMT;
			}
			else if (lexeme[0] == '&' && lexeme[1] == '&') {
			nextToken = AND_STMT;
			}
			else if (lexeme[0] == '|' && lexeme[1] == '|') {
			nextToken = OR_STMT;
			}
			else if (lexeme[0] == 'i' && lexeme[1] == 'f') {
			nextToken = IF_STMT;
			}
			else if (lexeme[0] == 'e' && lexeme[1] == 'l' && lexeme[2] == 's' && lexeme[3] == 'e') {
			nextToken = ELSE_STMT;
			}
		}
		break;


	/* Parse integer literals */
	case DIGIT:
		addChar();
		getChar();
		while (charClass == DIGIT) {
			addChar();
			getChar();
		}
		nextToken = INT_LIT;
		break;


	/* Parentheses and operators */
	case UNKNOWN:
		lookup(nextChar);
		getChar();
		if (nextToken == EOF)
			cout << "Error -  undefined operator/symbol \n";
		break;

	/* EOF */
	case EOF:
		nextToken = EOF;
		lexeme[0] = 'E';
		lexeme[1] = 'O';
		lexeme[2] = 'F';
		lexeme[3] = 0;
		break;

	} /* End of switch */
	cout << "Next token is: " << nextToken
	<< "       Next lexeme is " << lexeme << "\n";
	
	return nextToken;
} /* End of function lex */

/*******************************************************************
Operator/keyword: +, -, *, /, %, (, )
Identifer: (a+b+...+z+A+B+...Z)(a+b+...+z+A+B+...Z+0+1+2+...+9)*
Integer: (0+1+2+...+9)(0+1+2+...+9)*

The Extended Backus-Naur Form (EBNF) is:
<ifstmt> -> if(<boolexpr> '{'<assign>'}'[else'{'<assign>'}']
<bollexpr> -> <boolterm> {||<boolterm}
<boolterm> -> <boolfactor> {&& <boolfactor>}
<boolfactor> -> TRUE | FALSE | !<boolfactor> | (<boolexpr>)
<assign> -> id = <expr>; {<assign>}
<expr> -> <term> {(+| -) <term>}
<term> -> <factor> {(*| / | %) <factor>}
<factor>-> id | int_constant | (<expr>)
*******************************************************************/


/******************************************************************/
/* This function will check if statement, it will check for boolean expr and open brackets*/
/* <ifstmt> -> if (<boolexpr>) '{'<assign>'}' [else '{'<assign>'}'] */
void ifStatement() {
	if (nextToken == IF_STMT) {
		lex();
		if (nextToken == LEFT_PAREN) {
			lex();
			booleanExpr();
			if (nextToken == RIGHT_PAREN) {
				lex();
				if (nextToken == OPEN_BRACKET) {
					lex();
					assign();
				}
			}
			else {
				 cout << "Error - missing right parenthesis \n"; 
				 error++;
			}
		}
		else {
			 cout << "Error - missing left parenthesis \n"; 
			 error++;
		}
	}
	else if (nextToken == ELSE_STMT) {
		lex();
		if (nextToken == OPEN_BRACKET) {
			lex();
			assign();
		}
		else {
			cout << "Error - else statement \n";
			error++;
		}
	}
	else {
		cout << "Error  - If statement \n";
		error++;
	}
}

/*******************************************************************/
/* This function will check OR statment*/
/* <boolexpr> -> <boolterm> {|| <boolterm>} */
void booleanExpr() {
	booleanTerm();
	if (nextToken == OR_STMT) {
		lex();
		booleanExpr();
	}
}

/*******************************************************************/
/* This function will check AND statement*/
/* <boolterm> -> <boolfactor> {&& <boolfactor>} */
void booleanTerm() {
	booleanFactor();
	if (nextToken == AND_STMT) {
		lex();
		booleanTerm();
	}
}

/******************************************************************/
/*This fuction will check logical operators*/
/* <boolfactor> -> TRUE | FALSE | ! <boolfactor> | (<boolexpr>) */
void booleanFactor() {
	if (nextToken == TRUE_STMT || nextToken == FALSE_STMT) {
		lex();
	}
	else if (nextToken == NOT_STMT) {
		lex();
		booleanFactor();
	}
	else if (nextToken == LEFT_PAREN) {
		lex();
		booleanExpr();
		if (nextToken == RIGHT_PAREN)
			lex();
		else {
			cout << "Error - Missing right parenthesis \n";
			error++;
		}
	}
	else {
		cout << "Error -  Boolean Factor \n"; 
		error++;
	}
}

/********************************************************************/
/* This function will assign an id to expression */
/* <assign> -> id = <expr>; {<assign>} */
void assign() {
	if (nextToken == IDENT)
	{
		lex(); // gets on token ahead 
		if (nextToken == ASSIGN_OP)
		{
			lex();  // gets one token ahead
			expr();
			if (nextToken == SEMICOLON) {
				lex();
				if (nextToken == IDENT) {
					assign();
				}
			}
			else {
				cout << "Error - missing a semicolon \n"; 
				error++;
			}
		}
		else {
			cout << "Error - must be an assignment \n"; 
			error++;
		}
	}
	else {
		if (nextToken != EOF) {
			cout << "Error  - Missing an ID \n";
			error++;
		}
	}
}


/* Function expr
Parses strings in the language
generated by the rule:
<expr> -> <term> {(+ | -) <term>}
*/
void expr()
{
	/* Parse the first term */
	term();
	/* As long as the next token is + or -, call
	lex to get the next token and parse the
	next term */

	while (nextToken == ADD_OP ||
		nextToken == SUB_OP) {
		lex(); // to get the nextToken
		term();
	}
}


/* term
Parses strings in the language generated by
the rule:
<term> -> <factor> {(* | / | %) <factor>}
*/
void term()
{
	/* Parse the first factor */
	factor();
	/* As long as the next token is * or /,
	next token and parse the next factor */

	while (nextToken == MULT_OP ||
		nextToken == DIV_OP || nextToken == MOD_OP)
	{
		lex();
		factor();
	}
} /* End of function term */


/* Function factor
Parses strings in the language generated by the
rule: <factor> -> id | int_constant |( <expr>) */
void factor()
{
	/* Determine which RHS */
	if (nextToken == IDENT || nextToken == INT_LIT)
		lex();
	/* If the RHS is (<expr>) â€“call lex to pass over
	the left parenthesis, call expr, and check for
	the right parenthesis */
	else if (nextToken == LEFT_PAREN)
	{
		lex();
		expr();
		if (nextToken == RIGHT_PAREN)
			lex();
		else {
			cout << "Error - missing right parenthesis \n"; 
			error++;
		}
			
	}  /* End of else if (nextToken == ...  */
	else {
		cout << "Error - missing left parenthesis  \n"; /* Neither RHS matches */ 
		error++;
	}
}
