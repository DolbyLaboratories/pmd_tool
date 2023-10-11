/************************************************************************
 * dlb_pmd
 * Copyright (c) 2023, Dolby Laboratories Inc.
 * All rights reserved.
 * 
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above
 *    copyright notice, this list of conditions and the following
 *    disclaimer in the documentation and/or other materials provided
 *    with the distribution.
 *
 * 3. Neither the name of the copyright holder nor the names of its
 *    contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 * FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
 * COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT,
 * INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
 * STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED
 * OF THE POSSIBILITY OF SUCH DAMAGE.
 **********************************************************************/

/**
 * @file parser_tagstack.h
 * @brief datastructure for maintaining parser stack (for error reporting)
 */

#ifndef PARSER_TAGSTACK_H_
#define PARSER_TAGSTACK_H_

#define XML_TAG_STACK_SIZE (32)

typedef struct parser parser;

typedef enum
{
    TAG_ENCODING_DEFAULT,
    TAG_ENCODING_BASE16,
} tag_encoding;


typedef dlb_pmd_success (*on_pop_tag)(parser *p);


typedef struct
{
    const char *tag;
    unsigned int lineno;
    tag_encoding encoding;
    on_pop_tag pop_action;
} tagloc;


typedef struct
{
    tagloc stack[XML_TAG_STACK_SIZE];
    unsigned int top;
} tag_stack;


static inline
void
tag_stack_init
    (tag_stack *stack
    )
{
    stack->top = 0;
}


static inline
void
tag_stack_push
    (tag_stack *stack
    ,const char *tag
    ,unsigned int lineno
    ,on_pop_tag pop_action
    )
{
    tagloc *loc;

    if (stack->top < XML_TAG_STACK_SIZE-1)
    {
        loc = &stack->stack[stack->top];
        loc->tag = tag;
        loc->lineno = lineno;
        loc->encoding = TAG_ENCODING_DEFAULT;
        loc->pop_action = pop_action;
        stack->top += 1;
    }
}


static inline
int
tag_stack_pop
    (tag_stack *stack
    ,parser *p
    )
{
    if (stack->top > 0)
    {
        stack->top -= 1;
        if (stack->stack[stack->top].pop_action)
        {
            if (stack->stack[stack->top].pop_action(p))
            {
                return 1;
            }
        }
    }
    return 0;
}


static inline
tagloc *
tag_stack_top
    (tag_stack *stack
    )
{
    if (stack->top < XML_TAG_STACK_SIZE-1 && stack->top > 0)
    {
        return &stack->stack[stack->top-1];
    }
    return NULL;
}


#endif /* PARSER_TAGSTACK_H_ */
