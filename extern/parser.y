// prefix for functions name in generated parser
%name yajp_parser

// prefix will be added to the front of each terminal
%token_prefix YAJP_PARSER_TOKEN_

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
%extra_argument { yajp_parser_recognized_entity_t *entity }

%start_symbol start

start 	    ::= obj.

obj         ::= OBEGIN obj_content OEND.
obj         ::= OBEGIN OEND.

obj_content ::= pair.
obj_content ::= obj_content COMMA.
obj_content ::= obj_content pair.

pair_key    ::= STRING(A).			{ entity->token = A; entity->type = YAJP_PARSER_RECOGNIZED_ENTITY_TYPE_KEY; }

pair_value  ::= COLON value.

pair        ::= pair_key pair_value.		{ entity->type = YAJP_PARSER_RECOGNIZED_ENTITY_TYPE_PAIR; }

arr_begin   ::= ABEGIN.
arr_end     ::= AEND.

arr         ::= arr_begin arr_content arr_end.
arr         ::= arr_begin arr_end.

arr_content ::= value.
arr_content ::= arr_content COMMA.
arr_content ::= arr_content value.

value       ::= obj.
value       ::= arr.
value       ::= STRING(A).			{ entity->token = A; entity->type = YAJP_PARSER_RECOGNIZED_ENTITY_TYPE_VALUE; }
value       ::= BOOLEAN(A).			{ entity->token = A; entity->type = YAJP_PARSER_RECOGNIZED_ENTITY_TYPE_VALUE; }
value       ::= NUMBER(A).			{ entity->token = A; entity->type = YAJP_PARSER_RECOGNIZED_ENTITY_TYPE_VALUE; }
value       ::= NULL(A).			{ entity->token = A; entity->type = YAJP_PARSER_RECOGNIZED_ENTITY_TYPE_VALUE; }


