# pmd_tool (dlb_pmd_lib)
# version 1.2.0

pmd_tool is a command line utility that converts between
the following representations of SMPTE RDD49 metadata

 - a human-readable XML file matching Schema in
   dlb_pmd/include/dlb_pmd_3_2_0.xsd

 - a serialized binary payload

 - SMPTE-337m encoded binary payload carried on a PCM pair (or
   channel) in a .WAV file

The tool is built around the dlb_pmd_lib library, whose API can be
found in dlb_pmd/include. The purpose of this code is to help speed
development of software and products that use SMPTE RDD49 for audio
metadata carriage in professional applications. For more information
about see the PMD application guide included with this code.

## Getting Started

These instructions will help you get a copy of the project up and
running on your local machine for development and testing purposes.

### Folder Structure

- README.md         This file.

- dlb_bitbuf/       A component which provides sequential access
                    to single or multiple bits in a bitstream.

- dlb_buffer/       Buffer management component

- dlb_octfile/      This component defines a wrapper around the
                    stdio FILE type which allows file operations
                    to work on octets, rather than chars.
                    On platforms where CHAR_BIT is >8, the top
                    bits in each char will be zero padded.

- dlb_pmd/          Main front-end application and core conversion
                    modules

- dlb_wave/         Component providing read/write to Microsoft
                    audio WAV format

- dlb_xmllib/       XML parser

- googletest/       C++ test framework from Google

- xerces/    	    Another XML parser used by the test framework


### Prerequisites

For Linux and OSX, the library and tool can be built using GNU
makefiles For windows, both GNU makefiles and visual studio projects
are provided. For all platforms, both 32- and 64-bit targets are
supported.

### Build instructions

#### Using the GNU makefiles

Use the makefiles located in dlb_pmd/make/pmd_tool. Go to the
appropriate directory and run GNU make. Note that the Windows GNU
makefiles are written to use gcc style arguments not the Microsoft
compiler. These require Cygwin, MinGW or similar.

Release and debug executables are created in the same directory as the
makefile.

#### Using the Microsoft Visual Studio (on Windows)

Go to either the 32 bit or 64 bit the Windows MSVS directories under
dlb_pmd/make/pmd_tool. In MSVS, open the solution file (.sln) that
matches the version of MSVS that you are using. Select build solution
in MSVS.

## Running the tool

pmd_tool is a command line utility and detailed usage is provided by
running the tool with no options.

## Testing the library

To test the basic functionality of the library, there is an additional
pmd_test tool. The build makefiles and projects are found under
dlb_pmd/make/pmd_test, and the test suite can be built in the same
manner as the pmd_tool.

pmd_test is built on top of the googletest framework, so the complete
suite can be run simply by running the executable without arguments.
Note that the tests take a long time to start up, and take several
hours to run.

pmd_fuzz is an experimental model-based fuzzer that generates random
models and tests that serialization/deserialization works correctly.


## Release Notes

See the [Release Notes](ReleaseNotes.md) file for details

## License

This project is licensed under the BSD-3 License - see the
[LICENSE](LICENSE) file for details

