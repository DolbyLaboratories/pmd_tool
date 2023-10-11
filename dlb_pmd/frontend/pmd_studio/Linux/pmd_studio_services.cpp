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

#include <filesystem>
#include <unistd.h>
#include <sys/types.h>
#include <pwd.h>
#include <string.h>

#include "pmd_studio_error.h"
#include "pmd_studio.h"
#include "pmd_studio_services.h"

#define APPLICATION_NAME "pmd_studio"
#define COMPANY_NAME "Dolby"

void pmd_studio_services_get_application_data_path(std::filesystem::path &path);

void pmd_studio_create_application_data_path(void)
{
	std::error_code ec;
	std::filesystem::path path;

	pmd_studio_services_get_application_data_path(path);
	std::filesystem::create_directories(path, ec);
	if (ec)
	{
		pmd_studio_error(PMD_STUDIO_ERR_FILE, "Unable to create application data path");
	}
}

void pmd_studio_services_get_application_data_path(std::filesystem::path &path)
{
	if ((getenv("HOME")) != NULL)
	{
		path = std::string(getenv("HOME"));
	}
	else
	{
		path = std::string(getpwuid(getuid())->pw_dir);
	}
	path /= ".config";
	path /= COMPANY_NAME;
	path /= APPLICATION_NAME;
}

void pmd_studio_services_get_application_data_path(char *filename)
{
	std::filesystem::path path;
	pmd_studio_services_get_application_data_path(path);
	strncpy(filename, path.c_str(), PMD_STUDIO_MAX_FILENAME_LENGTH);
}
