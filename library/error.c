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
//	@File				error.c
//	@Description		
//			
///////////////////////////////////////////////////////////////////////////////////


/*
*	@function ErrorException
*	@param    e      - Error Flag
*	@param    format - Fprint format
*   @param    ...    - Arguments
*/
void ErrorException (int e, char* format, ...) {

    va_list arg;
    va_start(arg, format);
    vfprintf(stderr, format, arg);
    va_end(arg);
    
    if (e) {
       exit(e);
    }
}
