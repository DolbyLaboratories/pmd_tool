# pmd_tool (dlb_pmd_lib)
# version 1.7.3

This project provides applications and libraries to assist with conversion between
various professional audio metadata formats and containers.

pmd_tool is a command line utility that converts between the following
representations of professional audio metadata:

- PMD as defined in SMPTE RDD49 metadata
- ADM/Serial ADM Metadata as defined in ITU Rec. BS. 2076/2125 and SMPTE ST 2116

PMD Studio is an application that provides a user interface for authoring
professional audio metadata in either file or streaming formats.

For more information see the [release notes](ReleaseNotes.md).

## Getting Started

These instructions will help you get a copy of the project up and
running on your local machine for development and testing purposes.

### Folder Structure

- **README.md** This file.

- **ReleaseNotes.md** Release notes.

- **PmdStudioQsg.md** Quick Start Guide for PMD Studio.

- **LICENSE** Terms of use.

- **dlb_buffer/** Buffer management component.

- **dlb_octfile/** This component defines a wrapper around the stdio
  FILE type which allows file operations to work on octets, rather than chars.
  On platforms where CHAR_BIT is >8, the top bits in each char will be zero padded.

- **dlb_pmd/** Main front-end applications and core conversion modules.

- **dlb_socket/** Cross-platform socket component.

- **dlb_wave/** Component providing read/write to Microsoft (broadcast) audio WAV format.

- **dlb_xmllib/** XML parser.

- **googletest/** C++ test framework from Google.

- **Lawo/** Ember+ console integration library from Lawo.

- **libui/** Cross-platform GUI library.

- **portaudio/** Portable real-time audio library.

- **xerces/** Another XML parser used by the test framework.

- **zlib/** General purpose compression library.


### Prerequisites

For Linux and OSX, the library and tool can be built using GNU makefiles.
For Windows, Visual Studio 2015 and 2017 projects and solutions are provided.
For all platforms, 64-bit targets are supported.
For Linux, 32-bit platforms are supported.

The following packages are required when building under Ubuntu/Debian
sudo apt-get install build-essential libgtk-3-dev libasound2-dev libjack-dev

### Build instructions

#### Using the GNU makefiles

Use the makefiles located in dlb_pmd/make. Go to the appropriate directory
and run GNU make. Release and debug executables are created in the same
directory as the makefile.

#### Using Microsoft Visual Studio (on Windows)

Go to the 64 bit Windows MSVS directory under dlb_pmd/make/pmd_tool.
In Visual Studio 2015 or 2017, open the corresponding solution file (.sln).
Select build solution in MSVS.

Alternatively, run msbuild from the Windows command line:

```
>call "C:\Program Files (x86)\Microsoft Visual Studio 14.0\Common7\Tools\vsvars32.bat"
>msbuild pmd_tool_2015.sln /property:Configuration=debug
>msbuild pmd_tool_2015.sln /property:Configuration=release
```

## Running the applications

Several applications are available. However, we think the following two
are the most useful.

### PMD Tool
pmd_tool is a command line utility and detailed usage is provided by
running the tool with no options. This should be used for file conversion
operations. This application does not provide any real-time or streaming
funcionality other than writing wav files that may be streamed by another
application.

### PMD Studio
PMD Studio is an application for authoring professional metadata.
It provides a simple user interface for configuring audio beds, objects
and presentations. The authored metadata may be saved as an XML file in
either sADM or PMD formats or streamed using a professional sound card.  

Please see the [PMD Studio Quick Start Guide](PmdStudioQsg.md)

Basic usage of the command line when launching PMD Studio is obtained using '-h'.

![Screenshot of PMD Studio](pmd_studio.png)

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

## Known Limitations

The ADM XML output does not support or use common definitions as defined
in ITU.R BS 2094.

## Release Notes

See the [Release Notes](ReleaseNotes.md) file for details, including
information on additional applications and features.

## License

This project is licensed under the BSD-3 License - see the
[LICENSE](LICENSE) file for details