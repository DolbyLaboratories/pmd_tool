/******************************************************************************
 * This program is protected under international and U.S. copyright laws as
 * an unpublished work. This program is confidential and proprietary to the
 * copyright owners. Reproduction or disclosure, in whole or in part, or the
 * production of derivative works therefrom without the express permission of
 * the copyright owners is prohibited.
 *
 *                Copyright (C) 2016-2019 by Dolby Laboratories,
 *                Copyright (C) 2016-2019 by Dolby International AB.
 *                            All rights reserved.
 ******************************************************************************/

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
