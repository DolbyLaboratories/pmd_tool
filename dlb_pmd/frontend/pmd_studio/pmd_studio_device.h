/************************************************************************
 * dlb_pmd
 * Copyright (c) 2020, Dolby Laboratories Inc.
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

#ifndef __PMD_STUDIO_DEVICE_H__
#define __PMD_STUDIO_DEVICE_H__

#include "pmd_studio.h"

typedef struct pmd_studio_device pmd_studio_device;

#define DEFAULT_FRAMES_PER_BUFFER (128)
#define MIN_FRAMES_PER_BUFFER (128)
#define MAX_FRAMES_PER_BUFFER (4096)


dlb_pmd_success
pmd_studio_device_init(
	pmd_studio_device **s,
	int input_device,
	int output_device,
	unsigned int num_channels,
	float latency,
	unsigned int frames_per_buffer,
	dlb_pmd_bool am824_mode
	);

dlb_pmd_success
pmd_studio_device_update_mix_matrix(
	pmd_studio *studio
	);

void
pmd_studio_device_list(
	void
	);

dlb_pmd_success
pmd_studio_device_add_ring_buffer(
        unsigned int startchannel,         // output channel index starting at 0
        unsigned int num_channels,         // number of channels (1/2)
        uint32_t pcmbuf[],                 // base pointer for ring buffer 
        unsigned int pcmbufsize,           // size is in 32 bit words/samples
        unsigned int *ring_buffer_handle,  // returned handle that can be used to disable/delete later
        pmd_studio *studio
    );

dlb_pmd_success
pmd_studio_device_delete_ring_buffer(
        unsigned int ring_buffer_handle,
        pmd_studio *studio
    );

void
pmd_studio_device_print_debug(
    pmd_studio *studio
    );

void
pmd_studio_device_close(
	pmd_studio_device *s
	);

#endif /* __PMD_STUDIO_DEVICE_H__ */
