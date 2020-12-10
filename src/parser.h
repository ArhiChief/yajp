/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/*
 * parser.h
 * Copyright (C) 2020 Sergei Kosivchenko <arhichief@gmail.com> 
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated
 * documentation files (the "Software"), to deal in the Software without restriction, including without limitation the
 * rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to
 * permit persons to whom the Software is furnished to do so, subject to the following conditions:
 * 
 * The above copyright notice and this permission notice shall be included in all copies or substantial portions of
 * the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO
 * THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
 * TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */


#ifndef YAJP_PARSER_H
#define YAJP_PARSER_H

#include "lexer.h"

#ifndef DEBUG
#define NDEBUG
#endif

#ifndef NDEBUG
/**
 * \brief   Turn parser tracing on
 *
 * \details Turn parser tracing on by giving a stream to which to write the trace and a prompt to preface each
 * trace message.
 *
 * \note Tracing is turned off by making either argument NULL
 *
 * \param[in]   TraceFILE
 * \param[in]   zTracePrompt
 */
void yajp_parser_trace(FILE *TraceFILE, char *zTracePrompt);
#endif

/**
 * Recognized parser actions. Retur
 */
typedef struct {
    bool is_recognized;
    const yajp_lexer_token_t *field;
    const yajp_lexer_token_t *value;
} yajp_parser_recognized_action_t;

/**
 * \brief   Initialize a new parser that has already been allocated
 * \param[in]   yyp      The parser to be initialized
 */
void yajp_parser_init(void *yyp);

/**
 * \brief   This function allocates a new parser.
 *
 * \param[in]   mallocProc  Pointer to memory allocation function
 *
 * \return  Return pointer to allocated parser structure
 */
void *yajp_parser_allocate(void *(*mallocProc)(size_t));

/**
 * \brief   Clear all secondary memory allocations from the parser
 *
 * \param[in] yyp   The parser to be finalized
 */
void yajp_parser_finalize(void *yyp);

/**
 *
 * \brief Deallocate and destroy a parser
 *
 * \param[in]   yyp         The parser to be released
 * \param[in]   freeProc    Function used to reclaim memory
 */
void yajp_parser_release(void *yyp, void (*freeProc)(void *));

/**
 * \brief
 *
 * \param[in]   yyp         The parser
 * \param[in]   yymajor     The major token code number
 * \param       yyminor     The value for the token.
 * \param[out]  action      Recognized action. If value is YAJP_PARSER_RECOGNIZED_ACTION_NONE more tokens required
 */
void yajp_parser_parse(void *yyp, int yymajor, const yajp_lexer_token_t *yyminor, yajp_parser_recognized_action_t *action);

/**
 * \brief   Return the fallback token corresponding to canonical token
 *
 * \param[in]   token   Token
 * \return  Fallback token corresponding to canonical token or 0 if iToken has no fallback.
 */
int yajp_parser_fallback(int token);

#endif //YAJP_PARSER_H
