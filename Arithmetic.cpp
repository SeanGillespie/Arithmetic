/*
Arithmetic.cpp
@author Sean Gillespie
contact: gillespi20@up.edu

Contents:  main function and Three classes: Token, ExpParser, and Calculator
Function main displays usage, prompts user for input and creates instances of
ExpParser and Calculator which are to get the result of the expression and display it. 
Program loops, continuously prompting, solving, and displaying unless:
a) enter is hit at prompt with no input expression (feature stated in usage)
b) there is an error with the expression, in which case a message will be
   displayed as to why and the program will terminate.

ExpParser takes user input string and checks for many types of input errors 
as well as creating a vector of Tokens. Errors that are not checked with
this class are found in Calculator as this reduces amount of code needed

Each "Token" is a number, operator, or parenthesis from the input string.
contains information such as type, numerical value (if type is number), 
precedence in order of operations, and operator (if type is operator).

Calculator uses a vector of Tokens to calculate the result value of the 
expression.  The algorithm in brief uses two stacks of Token objects, one 
stack for operators (+*-/ and parenthesis) and one for numbers. The 
algorithm loops over each token in the vector, pushing each operator 
or number to its respective stack until the end is reached, or performing 
calculations when precedence rules or perenthesees dictate a binary operation.
If an operation is allowed, the r-value and l-value are popped from the 
value stack and used with the operator on top of the operator stack. The 
resulting value us then pushed to the value stack and we iterate further. This
continues until we find that there is an error in the expression, in which
case we will terminate, but if not, the result of the calculation is the
value left at the top of the vale stack at the end.
*/

#include <iostream>
#include <iterator>
#include <cstdlib>
#include <algorithm>
#include <string>
#include <stack>
#include <vector>
#include <ctype.h>
using namespace std;


/*************************************************************************************
 * @class Token
 * @description: Contains all attributes of a Token, which is one number or symbol
 * @author Sean Gillespie
 *************************************************************************************/
class Token {

	int type;//type of the character in the Token
	double value;//double of the value of the Token if it is a digit
	char op;//if the token is an operator, this is its operator (+, -, * or /)
	int precedence;//numerical value representing order of operations

public:

	//These constant values represent the type of the character
	const int UNKNOWN = -1;
	const int NUMBER = 0;
	const int OPERATOR = 1;
	const int LEFT_PARENTHESIS = 2;
	const int RIGHT_PARENTHESIS = 3;

	/* 
	* Default Constructor.
	* @param values Container whose values are summed.
	* @return sum of `values`, or 0.0 if `values` is empty.
	*/
	Token() { type = UNKNOWN; }//set type to unknown since we do not know what it is with no params

	/*
	* Parameterized Constructor for Token
	* Used when a calculation results in a new Token whose value is a double
	* @param val Double value of the Token if it is a number.
	*/
	Token(double val) {
		type = NUMBER;
		value = val;
	}

	/*
	* Parameterized Constructor for char
	* @param ch Char of the Token.
	*/
	Token(char ch) {//combine this with checking valid chars
		switch (ch) {//Here we are setting the type, operation, and precedence of the Token based on what the character is
		case '+':
			type = OPERATOR;
			op = ch;
			precedence = 1;//Addition and subtraction are higher priority than parenthesees but lower than division and multiplicaiton
			break;
		case '-':
			type = OPERATOR;
			op = ch;
			precedence = 1;
			break;
		case '*':
			type = OPERATOR;
			op = ch;
			precedence = 2;//Multiplicaton and division have highest priority in order of operations
			break;
		case '/':
			type = OPERATOR;
			op = ch;
			precedence = 2;
			break;
		case '(':
			type = LEFT_PARENTHESIS;
			precedence = 0;//Precedence of parenthesees is lowest so we do not try to perform e.g. "2 ( 4" as a calculation
			break;
		case ')':
			type = RIGHT_PARENTHESIS;
			precedence = 0;
			break;
		default://If the char is not an operator or parenthesee, it is by default a digit)
			type = NUMBER;
			value = toNumber(ch);  //numbers have no precedence as they are on their own stack in calculating
		}
	}

	//Getters for private variables
	int getType() { return type; }
	double getValue() { return value; }
	char getOp() { return op; }
	int getPrecedence() { return precedence; }

	/*
	* toNumber()
	* @param i Integer representing ASCII value of the character.
	* @return a digit 0 thru 9 that is the integer value of the char
	*/
	int toNumber(int i) { return i - 48; }
};


/*****************************************************************************************
 * @class ExpParser
 * @description: Perfoms operations on the user input string to check if it is valid
 * @author Sean Gillespie
 *****************************************************************************************/
class ExpParser {//try to use scanf instead of all this stuff

	string expStr;//the string that represents the expression the user enters
	vector<Token> parsedTokens;//vector of Token objects that will be used in the calculation

public:

	/*
	* Parameterized Constructor for ExpParser
	* @param input String that the user entered in.
	*/
	ExpParser(string input) {
		expStr = input;
		removeSpaces();
		checkValidChars();
		checkParenthesees();
		checkNearParens();
		parsedTokens = createTokenVector();
	}

	//getters for private variables
	string getExpStr() { return expStr; }
	vector<Token> getParsedTokens() { return parsedTokens; }

	/*
	* createTokenVector
	* Creates token objects from the characters in the string and adds them to a vector of tokens
	* @return vector created from the string
	*/
	vector<Token> createTokenVector() {
		vector<Token> tokens;
		for (int i = 0; i < expStr.length(); ++i) {
			tokens.push_back(Token(expStr.at(i)));
		}
		return tokens;
	}

	/*
	* removeSpaces
	* removes all whitespace from the expression string
	*/
	void removeSpaces() { expStr.erase(remove_if(expStr.begin(), expStr.end(), isspace), expStr.end()); }

	/*
	* checkValidChars
	* Looks at each char in the string to make sure only allowed chars are there, throws error if that is not the case.
	*/
	void checkValidChars() {
		for (int i = 0; i < expStr.length(); ++i) {
			if (!isdigit(expStr.at(i)) && expStr.at(i) != ')' && expStr.at(i) != '(' && expStr.at(i) != '+' && expStr.at(i) != '*' && expStr.at(i) != '/' && expStr.at(i) != '-') {
				cout << "character " << expStr.at(i) << " at index " << i << " is not allowed. Try again." << endl;
				system("pause");
				exit(EXIT_FAILURE);
			}
		}
	}

	/*
	* checkParenthesees
	* Uses a simple FIFO stack to check if parentheses match and are formatted correctly
	*/
	void checkParenthesees() {
		stack<char> parenthesees;
		for (int i = 0; i < expStr.length(); ++i) {
			if (expStr.at(i) == '(') {
				parenthesees.push(expStr.at(i));  //if we see a left parenthese, push it to the stack
			}
			else if (expStr.at(i) == ')') {
				if (!parenthesees.empty()) {
					parenthesees.pop();  //pop because we found a matching pair
				}
				else {
					cout << "Parenthesees are not balanced. Try again." << endl;  //Throw error because we cannot have a ) as the first parenthese
					system("pause");
					exit(EXIT_FAILURE);
				}
			}
		}
		if (!parenthesees.empty()) {
			cout << "Parenthesees are not balanced. Try again." << endl;  //If the stack is not empty at the end, not balanced, throw error
			system("pause");
			exit(EXIT_FAILURE);
		}
	}

	/*
	* checkNearParens
	* Throws errors where there is no operator between a number and parenthese, e.g (3+2)4 and 1+3(4/2) as we do not support multiplication in that way
	*/
	void checkNearParens() {
		for (int i = 0; i < expStr.length(); ++i) {
			if (expStr.at(i) == ')' && i + 1 < expStr.length() && isdigit(expStr.at(i+1))) {
				cout << "Missing operator between right parenthese and number. Try again." << endl;  //cannot do something like (3+2)4
				system("pause");
				exit(EXIT_FAILURE);
			}
			else if (expStr.at(i) == '(' && i > 0 && isdigit(expStr.at(i - 1))) {
				cout << "Missing operator between left parenthesee and number. Try again." << endl;  //cannot do something like 3(4+2)
				system("pause");
				exit(EXIT_FAILURE);
			}
		}
	}
};


/*********************************************************************************************************************************************
 * @class Calculator
 * @description: Uses vector of Tokens to calculate the result of the expression, or throws error if the expression is not formed correctly
 * @author Sean Gillespie
 **********************************************************************************************************************************************/
class Calculator {

public:

	/*
	* applyOperation
	* @params double a The l-value of this binary sub-expression
	* @params double b The r-value of this binary sub-expression
	* @params char op The binary operation to be performed
	* @return t New Token object with the value of the binary expression result
	*/
	Token applyOperation(double a, double b, char op) {
		double result = 0;
		switch (op) {
		case '+':
			result = a + b;
			break;
		case '-':
			result = a - b;
			break;
		case '*':
			result = a * b;
			break;
		case '/':
			result = a / b;
			break;
		default:
			cout << "Operator " << op << " is not an allowed binary operation. Try again." << endl;//throw error, the operator is not a +, -, * or /
			system("pause");
			exit(EXIT_FAILURE);
		}
		Token t(result);//create new token with result as its value using the parameterized constructor to handle non-int values
		return t;
	}
	/*
	* calculate
	* @params vector<Token> tokens A vector of all Tokens in the expression
	* @return value at top of valueStack which is the result
	*/
	double calculate(vector<Token> tokens) {
		unsigned int i = 0;  //for iterating
		stack<Token> valueStack;  //stack to keep track of all numeric Tokens
		stack<Token> operatorStack;  //stack to keep track of non-numeric tokens (operators, parenthesees)
		Token * val1;  //val1, val2, and op are pointers to items in the stacks that will be passed into applyOperation()
		Token * val2;
		Token * op;
		Token t;  //dummy token used for getting the type values

		while (i < tokens.size()) {
			if (tokens.at(i).getType() == t.LEFT_PARENTHESIS) {  //push left parenthese to stack and keep going
				operatorStack.push(tokens.at(i));
				++i;
			}
			else if (tokens.at(i).getType() == t.NUMBER) { //for more than 1 digit we keep "appending" them until the next char is not a number
				double val = 0;  //value of this number in the expression
				while (i < tokens.size() && tokens.at(i).getType() == t.NUMBER) {
					val = (val * 10) + ((tokens.at(i).getValue()));  //appending using some quick maths
					++i;
				}
				valueStack.push(Token(val));
			}
			else if (tokens.at(i).getType() == t.RIGHT_PARENTHESIS) {  //evaulate all that is enclosed between ) and its matching (
				while (!operatorStack.empty() && operatorStack.top().getType() != t.LEFT_PARENTHESIS) {
					if (!valueStack.empty()) {
						val2 = &valueStack.top(); //getting r-value
						valueStack.pop();
					}
					else {
						cout << "Error: expression not formed correctly. Try again" << endl; //if the stack is empty at this point, something is wrong
						system("pause");
						exit(EXIT_FAILURE);
					}
					if (!valueStack.empty()) {
						val1 = &valueStack.top(); //getting r-value
						valueStack.pop();
					}
					else {
						cout << "Error: expression not formed correctly. Try again" << endl; //if the stack is empty at this point, something is wrong
						system("pause");
						exit(EXIT_FAILURE);
					}
					op = &operatorStack.top();  //retrieve the top operator which will be used with the l-value and r-value
					operatorStack.pop();
					valueStack.push(applyOperation(val1->getValue(), val2->getValue(), op->getOp()));  //evaluate result and push to valueStack
				} 
				if (!operatorStack.empty()) {
					operatorStack.pop();  //if operator stack is not empty, there is a parenthese we need to pop
				}
				++i;
			}	
			else {   //if the current Token is an operator 
				while (!operatorStack.empty() && operatorStack.top().getPrecedence() >= (tokens.at(i).getPrecedence())) {  //determine which operation to do first, this one or a different one from the operatorStack
					if (!valueStack.empty()) {
						val2 = &valueStack.top();  
						valueStack.pop();
					}
					else {
						cout << "Error: expression not formed correctly. Try again" << endl;  //if the stack is empty at this point, something is wrong
						system("pause");
						exit(EXIT_FAILURE);
					}
					if (!valueStack.empty()) {
						val1 = &valueStack.top();
						valueStack.pop();
					}
					else {
						cout << "Error: expression not formed correctly. Try again" << endl;  //if the stack is empty at this point, something is wrong
						system("pause");
						exit(EXIT_FAILURE);
					}
					op = &operatorStack.top();  //get operator for this binary calculation and pop it
					operatorStack.pop();
					valueStack.push(applyOperation(val1->getValue(), val2->getValue(), op->getOp()));  //result of binary sub-expression is pushed to valueStack
				}
				operatorStack.push(tokens.at(i)); 
				++i;
			}
		}
		//we should only get here adter all the iterating
		while (!operatorStack.empty()) {  //All of the expressions tokens have been accounted for, so now we loop and evaluate what is on the stacks
			if (!valueStack.empty()) {
				val2 = &valueStack.top();  //set r-value from top of stack
				valueStack.pop();
			}
			else {
				cout << "Error: expression not formed correctly. Try again." << endl;  //if the stack is empty at this point, something is wrong
				system("pause");
				exit(EXIT_FAILURE);
			}
			if (!valueStack.empty()) {
				val1 = &valueStack.top();  //l-value is next on the stack
				valueStack.pop();
			}
			else {
				cout << "Error: expression not formed correctly. Try again." << endl;  //if the stack is empty at this point, something is wrong
				system("pause");
				exit(EXIT_FAILURE);
			}
			op = &operatorStack.top();  //get operator used for this next calc
			operatorStack.pop();
			valueStack.push(applyOperation(val1->getValue(), val2->getValue(), op->getOp()));  //push result of the operator sub-expression to the valueStack
		}
		if (!operatorStack.empty()) {
			cout << "Issues with operators. Terminating." << endl;//if there are more operators, the expression was not formed correctly
			system("pause");
			exit(EXIT_FAILURE);
		}
		//add a null check here
		if (!valueStack.empty()) {
			return valueStack.top().getValue();  //top of valueStack is the result if there was nothing wrong 
		}
		else {
			cout << "Issues with operators. Terminating." << endl;//if there are more operators, the expression was not formed correctly
			system("pause");
			exit(EXIT_FAILURE);
		}
	}
};


/*
* function Main
* Creates ExpParser to parse string expression, and Calculator to get its result
* Provides usage directions, prompts for input, displays result.
*/
int main() {
	//Banner to display at program beginning to help user
	cout << "******************************************************************************" << endl;
	cout << "WELCOME TO THE ARITHMETIC EXPRESSION CALCLATOR. THIS PROGRAM PARSES EXPRESSIONS" << endl;
	cout << "CONTAINING ANY COMBINATION OF POSITIVE INTEGERS AND PARENTHESEES WITH" << endl;
	cout << "BINARY ADDITION, MULTIPLICATION, SUBTRACTION, AND DIVISION AND RETURNS" << endl;
	cout << "THE CORRECT VALUE OF THE EXPRESSION.  IF THERE IS A PROBLEM WITH THE." << endl;
	cout << "ENTERED EXPRESSION, AN ERROR WILL BE DISPLAYED AND PROGRAM WILL TERMINATE." << endl << endl;
	cout << "ALLOWED CHARACTERS: ()+-/*0123456789 SPACES ARE OPTIONAL." << endl << endl;
	cout << "EXAMPLE VALID INPUT: (54 * (4 + 3*2 ) + 876) or 1000/4 + 3/5+(3+(7*2)) " << endl;
	cout << "******************************************************************************" << endl << endl;

	do {
		string line;
		cout << "Enter an Arithmetic Expression or hit enter with no input to exit the program: " << endl;
		getline(cin, line);
		if (line.empty()) { break; }  //exit the loop as nothing has been entered.
		ExpParser parser(line);  //new ExpParser instance to parse the entered string
		Calculator c; //new Calculator instance to perform the operations on the vector of Tokens
		cout << "RESULT IS: " << c.calculate(parser.getParsedTokens()) << endl;
	} 
	while (1);
	return 0;
}