///////////////////////////////////////////////////////////////////////////////////
//
//	@Project 			IFJ 2017
//
//  @Authors
//  Jandov� Krisn�na 	xjando04
//  Vil�m Faigel		xfaige00
//  Nikola Timkov�		xtimko01
//	Bc. V�slav Dole�al	xdolez76
//
//	@File				main.c
//	@Description		
//			
///////////////////////////////////////////////////////////////////////////////////

#include "./library/init.h"

//string word;

int main() {
    //int token = 0;
    
    FILE * f;
    
    f = loadFile("./tests/scanner_test.txt");
    
    /*
    strInit(&word);
    scanner_init(f);
	
	int i;	
	
	for (i = 0; i < 22; i++) {
		int token = scanner_next_token(&word);
		printf("%d\n",token);
	}
	*/
 
    return 0;
}
