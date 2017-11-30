///////////////////////////////////////////////////////////////////////////////////
//
//	@Project 			IFJ 2017
//
//  @Authors
//  Kristýna Jandová  	xjando04
//  Vilém Faigel		xfaige00
//  Nikola Timková		xtimko01
//	Bc. Václav Doleal	xdolez76
//
//	@File				tokens.c
//	@Description
//
///////////////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////////////
//
//  PARSER
//

#include "parser.h"

enum ParserStats {
    PARSER_START,
    PARSER_DECLARE_FUNCTION,
    PARSER_DECLARE_VARIABLE,
    PARSER_IF,
    PARSER_WHILE,
    PARSER_EOL,
    PARSER_END
};

struct Program      *__parser_program;

/*
*   @function      parserInit
*   @param         char * fileNameSource
*   @param         string       name
*   @description
*/
void parser_init(char * fileNameSource) {
    Token               tok;
    struct Function     *_function;

    scanner_init(fileNameSource);

    program_init(&__parser_program);

    int stateMain   = PARSER_START,
        stateReturn = PARSER_START;

    while(stateMain != PARSER_END) {
        switch(stateMain) {

            ////////////////////////////////////
            //
            case PARSER_START: {
                tok = scanner_next_token();
                switch(tok.flag) {
                    case TOKEN_DECLARE:
                        stateMain = PARSER_DECLARE_FUNCTION;
                    break;
                    case TOKEN_IF:
                        stateMain = PARSER_IF;
                    break;
                    case TOKEN_DO:
                        stateMain = PARSER_WHILE;
                    break;
                    case TOKEN_END_OF_FILE:
                        stateMain = PARSER_END;
                    break;
                }
            } break;

            ///////////////////////////////////////////////////////////////////////
            // Declare Function ID (ID As DT[, ID As DT] ..) As DT
            //
            case PARSER_DECLARE_FUNCTION: {

                // Declare >Function<
                tok = scanner_next_token();
                if (tok.flag != TOKEN_FUNCTION) {
                    LineErrorException(tok, ERROR_SYNTAX, "Function is missing");
                }

                // Declare Function >ID<
                tok = scanner_next_token();
                if (tok.flag != TOKEN_ID) {
                    LineErrorException(tok, ERROR_SYNTAX, "ID of function is missing");
                }

                /////////////////////////////////////////////
                // DEFINE FUNCTION
                _function = defFunction(__parser_program, &(tok.ID));

                // Declare Function ID >(<
                tok = scanner_next_token();
                if (tok.flag != TOKEN_BRACKET_RIGHT) {
                    LineErrorException(tok, ERROR_SYNTAX, "( is missing");
                }


                // Declare Function ID ([<STATEMENT> <STATEMENT_LIST>]>)<
                tok = scanner_next_token();
                if (tok.flag == TOKEN_ID) {
                    stateReturn = PARSER_DECLARE_FUNCTION;
                    stateMain   = PARSER_DECLARE_VARIABLE;
                    break;
                }

                /////////////////////////////////////////////
                /**/ LABEL_EndDeclareFunction:
                /////////////////////////////////////////////

                // Declare Function ID (>)<
                if (tok.flag != TOKEN_BRACKET_LEFT) {
                    LineErrorException(tok, ERROR_SYNTAX, ") is missing");
                }

                /////////////////////////////////////////////
                /// LABEL_DeclareReturnFunction:
                /////////////////////////////////////////////

                tok = scanner_next_token();

                struct DIM * _return = malloc(sizeof(struct DIM));


                // Declare Function ID () EOL
                if (tok.flag == TOKEN_END_OF_LINE) {
                // Declare Function ID () >AS<
                } else if (tok.flag == TOKEN_AS) {
                    tok = scanner_next_token();
                    switch(tok.flag) {
                        case TOKEN_INTEGER:
                        case TOKEN_DOUBLE:
                        case TOKEN_STRING:
                            _return->dataType = getDataTypeFromToken(tok.flag);
                        break;
                        default:
                            LineErrorException(tok, ERROR_SYNTAX, "Data Type is missing");
                    }


                } else {
                    LineErrorException(tok, ERROR_SYNTAX, "AS is missing");
                }

                _function->_return = _return;

                stateMain   = PARSER_START,
                stateReturn = PARSER_START;

            } break;

            ///////////////////////////////////////////////////////////////////////
            // Declare Function ID (ID As DT[, ID As DT] ..) As DT
            //
            case PARSER_DECLARE_VARIABLE: {

                /////////////////////////////////////////////
                /**/ LABEL_DataType:
                /////////////////////////////////////////////

                // >ID< As DT
                if (tok.flag != TOKEN_ID) {
                    LineErrorException(tok, ERROR_SYNTAX, "ID is missing");
                }

                string tmpID = tok.ID;

                // ID >As< DT
                tok = scanner_next_token();
                if (tok.flag != TOKEN_AS) {
                    LineErrorException(tok, ERROR_SYNTAX, "AS is missing");
                }

                // ID As >DT<
                tok = scanner_next_token();

                switch (tok.flag) {
                    case TOKEN_INTEGER:
                    case TOKEN_DOUBLE:
                    case TOKEN_STRING: {
                        defFunctionParameter(_function, &tmpID, getDataTypeFromToken(tok.flag));
                    } break;

                    default: {
                        LineErrorException(tok, ERROR_SYNTAX, "Data Type is missing");
                    }
                }

                tok = scanner_next_token();

                // ID As DT>[, ID As DT ..]<
                if (tok.flag == TOKEN_COMMA) {
                        tok = scanner_next_token();
                        goto LABEL_DataType;
                }
                if (stateReturn == PARSER_DECLARE_FUNCTION) goto LABEL_EndDeclareFunction;

            } break;
            ///////////////////////////////////////////////////////////////////////
            // IF výraz THEN EOL  
	    // příkazy
            // ELSE
	    // příkazy
	    // END IF	
	    case PARSER_IF:
		//tu se musí zavolat PA pro výraz
		//IF výraz = true
			tok = scanner_next_token();
		        if (tok.flag != TOKEN_THEN) {
		            LineErrorException(tok, ERROR_SYNTAX, "THEN is missing");
		        }
		        tok = scanner_next_token();
		        if (tok.flag != TOKEN_END_OF_LINE) {
		            LineErrorException(tok, ERROR_SYNTAX, "must be end of line");
		        }
			//tu se vyhodnotí další příkazy

		//IFvýraz = false skip till ELSE
			while(tok.flag != TOKEN_ELSE){
		 		tok = scanner_next_token();
				if (tok.flag == TOKEN_END_OF_FILE) {
				    LineErrorException(tok, ERROR_SYNTAX, "reached end, ELSE is missing");
				}
			}
			tok = scanner_next_token();
			if (tok.flag != TOKEN_END_OF_LINE) {
			    LineErrorException(tok, ERROR_SYNTAX, "must be end of line");
			}
			//tu se vyhodnoti dalsi prikazy
	
		//ending of if statement (END IF)
	        tok = scanner_next_token();
		if (tok.flag != TOKEN_END) {
		    LineErrorException(tok, ERROR_SYNTAX, "missing END IF statement");
		}
	        tok = scanner_next_token();
	        if (tok.flag != TOKEN_IF) {
	            LineErrorException(tok, ERROR_SYNTAX, "missing END IF statement");
	        }
	    break;


            ///////////////////////////////////////////////////////////////////////
            // DO WHILE výraz EOL  
	    // příkazy
            // LOOP
	    case PARSER_WHILE:
		tok = scanner_next_token();
		if (tok.flag != TOKEN_WHILE) {
		    LineErrorException(tok, ERROR_SYNTAX, "missing WHILE statement");
		}
		
		//tu se vola PA pro vyhodnocení výrazu
		tok = scanner_next_token();
		if (tok.flag != TOKEN_END_OF_LINE) {
		    LineErrorException(tok, ERROR_SYNTAX, "must be nd of line");
		}
		while(tok.flag != TOKEN_LOOP){
		tok = scanner_next_token();
		//tu jede vyhodnocování věcí v cyklu, +kontroluji jestli není ukončovací podmínka (pro BASIC)
		}


	    break;         
		

        }
    }


    program_dump(__parser_program);
    exit(0);
}
///////////////////////////////////////////////////////////////////////////////////
//
//  FUNCTIONS
//



void _dumpFunctions(struct tree_node * node) {
    if (!node) return;
    _dumpFunctions(node->left);


    struct Function * f = node->payload;
    struct tree     * p = f->parameters;

    printf("Function(%s) %s\n", node->key, (f->name).str);

    _dumpParameters(p->root);

    printf("Return @%s\n\n", getDataTypeName(f->_return->dataType));

    _dumpFunctions(node->right);

    return;
}


void _dumpParameters(struct tree_node * node) {

    if (!node) return;

    _dumpParameters(node->left);

    struct DIM * var = node->payload;

    printf("  Parameter (%s) :: %s@%s\n", node->key, getDataTypeName(var->dataType), var->name.str);

    _dumpParameters(node->right);

    return;
}

void dumpFunctions(struct Program * p) {
    _dumpFunctions(p->functions->root);
}

void program_dump(struct Program * p) {
    printf("\n::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::\n");
    printf(":: DUMP PROGRAM \n");
    printf("::\n:: SCOPE\n");

    printf("::\n:: FUNCTIONS::\n");
    dumpFunctions(p);
}


///////////////////////////////////////////////////////////////////////////////////////////////////////////////

/*
*   @function      functions_init
*   @description
*/
void program_init(struct Program ** p) {

    struct Program * _p = malloc(sizeof(struct Program));

    _p->functions       = new_tree(TREE_PLAIN);
    _p->globalVariables = new_tree(TREE_PLAIN);

    *p = _p;
}

/*
*   @function      defFunction
*   @param         TFunctions * fceTable
*   @param         string       name
*   @description
*/
struct Function * defFunction(struct Program * p, string * name) {

    Dump("> Define Function");

    if (p == NULL) {
        ErrorException(ERROR_INTERN, "Parser :: Function Add :: Program is NULL");
    }

    if (!name->length) {
        ErrorException(ERROR_INTERN, "Parser :: Function Add :: ID is NULL");
    }

    struct Function * f = malloc( sizeof(struct Function));

    f->priority   = 0;
    f->parameters = new_tree(TREE_PLAIN);
    f->variables  = new_tree(TREE_PLAIN);

    strInit(&(f->name));
    strCopyString(&(f->name), name);

    tree_add(p->functions, name->str, f);

    return f;
}

/*
*   @function      defFunctionParameter
*   @param         TFunctions * fceTable
*   @param         string       name
*   @description
*/
void defFunctionParameter(struct Function * f, string * name, DataType dType) {

    if (f == NULL || !name->length) {
        ErrorException(ERROR_INTERN, "Parser :: Add Parameter");
    }

    Dump("Define Function Param");

    struct DIM * var = defParameter(name, dType);

    tree_add(f->parameters, name->str , var);
}


/*
*   @function      functionAdd
*   @param         TFunctions * fceTable
*   @param         string       name
*   @description
*/
struct DIM * createVariable(string * name, string * value, DataType dType, DIMFrame frame) {

    if (!name->length) {
        ErrorException(ERROR_RUNTIME, "Create Variable :: NAME IS NULL");
    }


    Dump("Create Variable");
    struct DIM * variable = malloc(sizeof(struct DIM));

    strInit(&(variable->name));
    strCopyString(&(variable->name), name);

    variable->dataType = dType;
    variable->frame    = frame;

    if (value->length) {
        switch (dType) {
            case DATA_TYPE_INT: {
                variable->valueInteger = atoi(value->str);
            } break;
            case DATA_TYPE_DOUBLE: {
                char *ptr;
                variable->valueDouble  = strtod(value->str, &ptr);
            } break;
            case DATA_TYPE_STRING: {
                strInit(&(variable->valueString));
                strCopyString(&(variable->valueString), value);
            } break;
        }
    }

    return  variable;
}

/*
*   @function      functionAdd
*   @param         TFunctions * fceTable
*   @param         string       name
*   @description
*/

struct DIM * defParameter(string * name, DataType dType) {

    string value;
    strInit(&value);

    struct DIM * parameter = createVariable(name, &value, dType, FRAME_PARAMETERS);

    return parameter;
}

/*
*   @function      functionAdd
*   @param         TFunctions * fceTable
*   @param         string       name
*   @description
*/
//void parser_set_setting(parserSetting setting);



/*
*   @function      functionAdd
*   @param         TFunctions * fceTable
*   @param         string       name
*   @description
*/
//void parser_run(char * fileNameDestiny) {}


/*
*   @function      functionAdd
*   @param         TFunctions * fceTable
*   @param         string       name
*   @description
*/
//void addFunctionParam(Function * fce, string name, short int dateType) {}


/*
*   @function      functionAdd
*   @param         TFunctions * fceTable
*   @param         string       name
*   @description
*/
//void dumpFunctionParams(Function * fce) {}

