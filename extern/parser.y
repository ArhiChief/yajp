// prefix for functions name in generated parser
%name yajp_parser

// prefix will be added to the front of each terminal
%token_prefix YAJP_TOKEN_

// additional include directives
%include {
    #include <assert.h>
    #include <stdlib.h>
    #include "parser.h"
}

// type of token
%token_type { const yajp_lexer_token_t * }

// fourth argument for yajp_parser_parse function
%extra_argument { yajp_deser_ctx_t *ctx }

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

pair        ::= key value.

key         ::= STRING(A) COLON.    { /*yajp_parser_set_action(ctx, A->attrs.val, A->attrs.val_size);*/ }

arr         ::= ABEGIN arr_content AEND.
arr         ::= ABEGIN AEND.

arr_content ::= value.
arr_content ::= arr_content COMMA.
arr_content ::= arr_content value.


value       ::= obj.
value       ::= arr.
value       ::= STRING(A).          { /*yajp_parser_try_set_value(ctx, A);*/ }
value       ::= BOOLEAN(A).         { /*yajp_parser_try_set_value(ctx, A);*/ }
value       ::= NUMBER(A).          { /*yajp_parser_try_set_value(ctx, A);*/ }
value       ::= NULL(A).            { /*yajp_parser_try_set_value(ctx, A);*/ }