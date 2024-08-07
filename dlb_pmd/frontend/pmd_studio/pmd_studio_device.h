/************************************************************************
 * dlb_pmd
 * Copyright (c) 2020, Dolby Laboratories Inc.
 * Copyright (c) 2020, Dolby International AB.
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

#include <mutex>
#include <ifaddrs.h>
#include <net/if.h>

#include "pmd_studio.h"
#include "pmd_studio_audio_outputs.h"
#include "pmd_studio_device_consts.h"
#include "pmd_studio_settings.h"
#include "pmd_studio_device_settings.h"
#include "ring_buffer.h"

struct pmd_studio_device;

#define MIN_FRAMES_PER_BUFFER (128)
#define MAX_FRAMES_PER_BUFFER (4096)
#define AUTO_FRAMES_PER_BUFFER (0)
#define MAX_LATENCY_MS (1000000)

typedef void* pmd_studio_device_ring_buffer_handle;

const char
*pmd_studio_device_get_settings_menu_name(void);

void
pmd_studio_device_init_settings(
    pmd_studio_device_settings *settings
    );

dlb_pmd_success
pmd_studio_device_init(
    pmd_studio_device **retdevice,
    pmd_studio_common_device_settings *common_settings,
    pmd_studio_device_settings *settings,
    uiWindow *win,
    pmd_studio *studio
    );

void
pmd_studio_device_edit_settings
(
    pmd_studio_device_settings *device_settings,
    uiWindow *win,
    pmd_studio *studio
    );

dlb_pmd_success
pmd_studio_device_reset(
    pmd_studio_device *device
    );

PMDStudioRingBufferList *pmd_studio_device_get_ring_buffer_list(pmd_studio *studio);

dlb_pmd_success
pmd_studio_device_update_mix_matrix(
	pmd_studio *studio
	);

void pmd_studio_device_option(pmd_studio *studio, const char *option);

bool pmd_studio_device_channel_requires_smpte337(unsigned int channel, pmd_studio *studio);

dlb_pmd_success
pmd_studio_device_add_ring_buffer(
    unsigned int startchannel,         // output channel index starting at 0
    unsigned int num_channels,         // number of channels (1/2)
    pmd_studio_video_frame_rate frame_rate,
    pmd_studio *studio
    );


dlb_pmd_success
pmd_studio_device_delete_ring_buffer(
    pmd_studio_device_ring_buffer_handle handle,
    pmd_studio *studio
    );

void
*pmd_studio_device_get_ring_buffer(
    pmd_studio_device_ring_buffer_handle handle,
    unsigned int &size_bytes
    );

void
pmd_studio_device_commit_ring_buffer(
    pmd_studio_device_ring_buffer_handle handle
    );

void
pmd_studio_device_commit_ring_buffer(
    pmd_studio_device_ring_buffer_handle handle
    , unsigned int size_bytes);

dlb_pmd_success
pmd_studio_get_input_device_name(
    char **name,
    pmd_studio *studio,
    int device_index
    );

dlb_pmd_success
pmd_studio_get_output_device_name(
    char **name,
    pmd_studio *studio,
    int device_index
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
