/************************************************************************
 * dlb_bitbuf
 * Copyright (c) 2018, Dolby Laboratories Inc.
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
#ifdef SYMBOL_PREFIX

    #define PREFIX_JOIN(a,b)          a ## _ ## b
    #define EVALUATE_PREFIX_JOIN(a,b) PREFIX_JOIN(a,b)
    #define APPLY_SYMBOL_PREFIX(sid)  EVALUATE_PREFIX_JOIN(SYMBOL_PREFIX,sid)

    #define dlb_bitbuf_align                  APPLY_SYMBOL_PREFIX(dlb_bitbuf_align)
    #define dlb_bitbuf_get_abs_pos            APPLY_SYMBOL_PREFIX(dlb_bitbuf_get_abs_pos)
    #define dlb_bitbuf_get_alignment_bits     APPLY_SYMBOL_PREFIX(dlb_bitbuf_get_alignment_bits)
    #define dlb_bitbuf_get_bits_left          APPLY_SYMBOL_PREFIX(dlb_bitbuf_get_bits_left)
    #define dlb_bitbuf_init                   APPLY_SYMBOL_PREFIX(dlb_bitbuf_init)
    #define dlb_bitbuf_set_abs_pos            APPLY_SYMBOL_PREFIX(dlb_bitbuf_set_abs_pos)
    #define dlb_bitbuf_skip                   APPLY_SYMBOL_PREFIX(dlb_bitbuf_skip)
    #define skip_bits_forward                 APPLY_SYMBOL_PREFIX(skip_bits_forward)
    #define dlb_bitbuf_fast_peek              APPLY_SYMBOL_PREFIX(dlb_bitbuf_fast_peek)
    #define dlb_bitbuf_fast_peek_long         APPLY_SYMBOL_PREFIX(lb_bitbuf_fast_peek_long)
    #define dlb_bitbuf_fast_read              APPLY_SYMBOL_PREFIX(dlb_bitbuf_fast_read)
    #define dlb_bitbuf_fast_read_long         APPLY_SYMBOL_PREFIX(dlb_bitbuf_fast_read_long)
    #define dlb_bitbuf_get_version            APPLY_SYMBOL_PREFIX(dlb_bitbuf_get_version)
    #define dlb_bitbuf_peek                   APPLY_SYMBOL_PREFIX(dlb_bitbuf_peek)
    #define dlb_bitbuf_peek_long              APPLY_SYMBOL_PREFIX(dlb_bitbuf_peek_long)
    #define dlb_bitbuf_read                   APPLY_SYMBOL_PREFIX(dlb_bitbuf_read)
    #define dlb_bitbuf_read_long              APPLY_SYMBOL_PREFIX(dlb_bitbuf_read_long)
    #define dlb_bitbuf_safe_peek              APPLY_SYMBOL_PREFIX(dlb_bitbuf_safe_peek)
    #define dlb_bitbuf_safe_peek_long         APPLY_SYMBOL_PREFIX(dlb_bitbuf_safe_peek_long)
    #define dlb_bitbuf_safe_read              APPLY_SYMBOL_PREFIX(dlb_bitbuf_safe_read)
    #define dlb_bitbuf_safe_read_long         APPLY_SYMBOL_PREFIX(dlb_bitbuf_safe_read_long)
    #define dlb_bitbuf_write                  APPLY_SYMBOL_PREFIX(dlb_bitbuf_write)
    #define dlb_bitbuf_write_long             APPLY_SYMBOL_PREFIX(dlb_bitbuf_write_long)

#endif
