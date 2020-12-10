// prefix for functions name in generated parser
%name yajp_parser

// prefix will be added to the front of each terminal
%token_prefix YAJP_TOKEN_

// additional include directives
%include {
    #include <assert.h>
    #include <stdlib.h>
    #include "parser.h"
    #include "lexer.h"
}

// type of token
%token_type { const yajp_lexer_token_t * }

// fourth argument for yajp_parser_parse function
%extra_argument { yajp_parser_recognized_action_t *action }

/*
 * Grammar based on https://web.cs.dal.ca/~sjackson/bitJson/JSON.html with
 * additional changes which are necessary for our needs.
 */


// starting symbol of grammar
%start_symbol root

root        ::= obj.
root        ::= arr.

obj         ::= OBEGIN obj_content OEND.
obj         ::= OBEGIN OEND.

obj_content ::= pair.
obj_content ::= obj_content COMMA.
obj_content ::= obj_content pair.

pair        ::= STRING(A) COLON value(B).	{ action->field = A; action->value = B; action->is_recognized = true; }

arr         ::= ABEGIN arr_content AEND.
arr         ::= ABEGIN AEND.

arr_content ::= value.
arr_content ::= arr_content COMMA.
arr_content ::= arr_content value.


value       ::= obj.
value       ::= arr.
value       ::= STRING.          {  }
value       ::= BOOLEAN.         {  }
value       ::= NUMBER.          {  }
value       ::= NULL.            {  }