/******************************************************************************
 * This program is protected under international and U.S. copyright laws as
 * an unpublished work. This program is confidential and proprietary to the
 * copyright owners. Reproduction or disclosure, in whole or in part, or the
 * production of derivative works therefrom without the express permission of
 * the copyright owners is prohibited.
 *
 *                Copyright (C) 2019-2020 by Dolby Laboratories,
 *                Copyright (C) 2019-2020 by Dolby International AB.
 *                            All rights reserved.
 ******************************************************************************/

#ifndef PMD_STUDIO_FILE_MENU_H_
#define PMD_STUDIO_FILE_MENU_H_

#include <stdio.h>

#include "ui.h"
#include "dlb_pmd_api.h"
#include "pmd_studio.h"
#include "pmd_studio_device.h"

typedef struct
{
    uiMenu *menu;
    uiMenuItem *new_model;
    uiMenuItem *open_model;
    uiMenuItem *save_pmd;
    uiMenuItem *save_sadm;
    uiMenuItem *update;
    uiMenuItem *settings;
    uiMenuItem *device_settings;
    uiMenuItem *quit;
#ifndef NDEBUG
    uiMenuItem *debug;
#endif
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
        pmd_studio_reset(s);
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
void
do_update
    (uiMenuItem *item
    ,uiWindow *w
    ,void *data
    )
{
	int status;
	pmd_studio *s = (pmd_studio*)data;
	FILE *file;


    (void)item;
    (void)w;
    
    

    pmd_studio_save_file(s, "pmd.xml", 0);
    pmd_studio_save_file(s, "pmd.klv", 0);
    pmd_studio_save_file(s, "sadm.xml", 1);
    /* Check to see if update script exists before running it */
    file = fopen("./update", "r");
    if (file != NULL)
    {
        fclose(file);
        status = system("./update");
    }

    (void)status;
}

#ifndef NDEBUG
static
void
print_debug
    (uiMenuItem *item
    ,uiWindow *w
    ,void *data
    )
{
	pmd_studio *s = (pmd_studio*)data;

    (void)item;
    (void)w;
    pmd_studio_debug(s);
}
#endif

MAY_BE_UNUSED
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
    fm->update     = uiMenuAppendItem(fm->menu, "Update");
    fm->settings   = uiMenuAppendItem(fm->menu, "Settings");
    fm->device_settings = uiMenuAppendItem(fm->menu, pmd_studio_device_get_settings_menu_name());
    fm->quit       = uiMenuAppendItem(fm->menu, "Quit");
#ifndef NDEBUG
    fm->debug      = uiMenuAppendItem(fm->menu, "Debug");
#endif
    uiMenuItemOnClicked(fm->new_model,  new_model,       s);
    uiMenuItemOnClicked(fm->open_model, open_model,      s);
    uiMenuItemOnClicked(fm->save_pmd,   save_model_pmd,  s);
    uiMenuItemOnClicked(fm->save_sadm,  save_model_sadm, s);
    uiMenuItemOnClicked(fm->update,     do_update,  s);
    uiMenuItemOnClicked(fm->settings,   edit_settings, s);
    uiMenuItemOnClicked(fm->device_settings,    pmd_studio_settings_edit_device_settings, s);
    uiMenuItemOnClicked(fm->quit,       do_quit,    NULL);
#ifndef NDEBUG
    uiMenuItemOnClicked(fm->debug,       print_debug,    s);
#endif
    return PMD_SUCCESS;
}


#endif /* PMD_STUDIO_FILE_MENU_H_ */
