# pmd_tool (dlb_pmd_lib)
# version 1.7.2

pmd_tool is a command line utility that converts between
the following representations of SMPTE RDD49 metadata

 - a human-readable XML file matching the schema in
   dlb_pmd/include/dlb_pmd_3_2_1.xsd

 - a serialized binary payload

 - SMPTE-337m encoded binary payload carried on a PCM pair (or
   channel) in a .WAV file

The tool is built around the dlb_pmd_lib library, whose API can be
found in dlb_pmd/include. The purpose of this code is to help speed
development of software and products that use SMPTE RDD49 for audio
metadata carriage in professional applications. 

For more information see the release notes.

## Getting Started

These instructions will help you get a copy of the project up and
running on your local machine for development and testing purposes.

### Folder Structure

- **README.md** This file.

- **ReleaseNotes.md** Release notes.

- **LICENSE** Terms of use.

- **dlb_bitbuf/** A component which provides sequential access
  to single or multiple bits in a bitstream.

- **dlb_buffer/** Buffer management component.

- **dlb_octfile/** This component defines a wrapper around the stdio
  FILE type which allows file operations to work on octets, rather than chars.
  On platforms where CHAR_BIT is >8, the top bits in each char will be zero padded.

- **dlb_pmd/** Main front-end applications and core conversion modules.

- **dlb_socket/** Cross-platform socket component.

- **dlb_wave/** Component providing read/write to Microsoft (broadcast) audio WAV format.

- **dlb_xmllib/** XML parser.

- **googletest/** C++ test framework from Google.

- **libui/** Cross-platform GUI library.

- **portaudio/** Portable real-time audio library.

- **xerces/** Another XML parser used by the test framework.

- **zlib/** General purpose compression library.


### Prerequisites

For Linux and OSX, the library and tool can be built using GNU
makefiles. For windows, Visual Studio 2015 projects and solutions
are provided. For all platforms, 64-bit targets are supported.
For Linux, 32-bit platforms are supported.

### Build instructions

#### Using the GNU makefiles

Use the makefiles located in dlb_pmd/make/pmd_tool. Go to the
appropriate directory and run GNU make. Release and debug executables
are created in the same directory as the makefile.

#### Using Microsoft Visual Studio (on Windows)

Go to the 64 bit Windows MSVS directory under dlb_pmd/make/pmd_tool.
In Visual Studio 2015, open the solution file (.sln).  Select build solution
in MSVS.

Alternatively, run msbuild from the Windows command line:

```
>call "C:\Program Files (x86)\Microsoft Visual Studio 14.0\Common7\Tools\vsvars32.bat"
>msbuild pmd_tool_2015.sln /property:Configuration=debug
>msbuild pmd_tool_2015.sln /property:Configuration=release
```

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

See the [Release Notes](ReleaseNotes.md) file for details, including
information on additional applications and features.

## License

This project is licensed under the BSD-3 License - see the
[LICENSE](LICENSE) file for details

