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

#ifndef PMD_STUDIO_FILE_MENU_H_
#define PMD_STUDIO_FILE_MENU_H_


#include "ui.h"
#include "dlb_pmd_api.h"
#include "pmd_studio.h"

typedef struct
{
    uiMenu *menu;
    uiMenuItem *new_model;
    uiMenuItem *open_model;
    uiMenuItem *save_pmd;
    uiMenuItem *save_sadm;
    uiMenuItem *quit;
} pmd_studio_file_menu;
    

static
void
new_model
    (uiMenuItem *item
    ,uiWindow *window
    ,void *data
    )
{
    pmd_studio *s = (pmd_studio*)data;

    (void)item;
    (void)window;
    pmd_studio_reset(s);
}

    
static
void
open_model
    (uiMenuItem *item
    ,uiWindow *window
    ,void *data
    )
{
    pmd_studio *s = (pmd_studio*)data;
    char *filename;

    (void)item;

    filename = uiOpenFile(window);
    if (filename)
    {
        pmd_studio_open_file(s, filename);
    }
}

static
void
save_model_pmd
    (uiMenuItem *item
    ,uiWindow *window
    ,void *data
    )
{
    pmd_studio *s = (pmd_studio*)data;
    const char *filename;
    (void)item;

    filename = uiSaveFile(window);
    pmd_studio_save_file(s, filename, 0);
}


static
void
save_model_sadm
    (uiMenuItem *item
    ,uiWindow *window
    ,void *data
    )
{
    pmd_studio *s = (pmd_studio*)data;
    const char *filename;
    (void)item;

    filename = uiSaveFile(window);
    pmd_studio_save_file(s, filename, 1);
}


static
void
do_quit
    (uiMenuItem *item
    ,uiWindow *w
    ,void *data
    )
{
    (void)item;
    (void)w;
    (void)data;
    
    uiQuit();
}


static
dlb_pmd_success
pmd_studio_file_menu_init
    (pmd_studio_file_menu *fm
    ,pmd_studio *s
    )
{
    fm->menu       = uiNewMenu("File");
    fm->new_model  = uiMenuAppendItem(fm->menu, "New");
    fm->open_model = uiMenuAppendItem(fm->menu, "Open");
    fm->save_pmd   = uiMenuAppendItem(fm->menu, "Save PMD");
    fm->save_sadm  = uiMenuAppendItem(fm->menu, "Save sADM");
    uiMenuAppendSeparator(fm->menu);
    fm->quit       = uiMenuAppendItem(fm->menu, "Quit");

    uiMenuItemOnClicked(fm->new_model,  new_model,       s);
    uiMenuItemOnClicked(fm->open_model, open_model,      s);
    uiMenuItemOnClicked(fm->save_pmd,   save_model_pmd,  s);
    uiMenuItemOnClicked(fm->save_sadm,  save_model_sadm, s);
    uiMenuItemOnClicked(fm->quit,       do_quit,    NULL);
    return PMD_SUCCESS;
}


#endif /* PMD_STUDIO_FILE_MENU_H_ */
