# pmd_tool (dlb_pmd_lib)
# version 2.3.0

This project provides applications and libraries to assist with conversion between
various professional audio metadata formats and containers.

pmd_tool is a command line utility that converts between the following
representations of professional audio metadata:

- ADM/Serial ADM Metadata as defined in ITU Rec. BS. 2076/2125 and SMPTE ST 2116
- PMD as defined in SMPTE RDD49 metadata

PMD Studio is an application that provides a user interface for authoring
professional audio metadata in either file or streaming formats.

For more information see the [release notes](ReleaseNotes.md).



## Getting Started

These instructions will help you get a copy of the project up and
running on your local machine for development and testing purposes.

### Folder Structure

- **README.md** This file.

- [**PmdStudioFAQ.md**](PmdStudioFAQ.md) Frequently asked questions when getting started with PMD Studio

- **ReleaseNotes.md** Release notes.

- **PmdStudioQsg.md** Quick Start Guide for PMD Studio.

- **LICENSE** Terms of use.

- **cmake/** Additional cmake files

- **dlb_buffer/** Buffer management component.

- **dlb_octfile/** This component defines a wrapper around the stdio
  FILE type which allows file operations to work on octets, rather than chars.
  On platforms where CHAR_BIT is >8, the top bits in each char will be zero padded.

- **dlb_pmd/** Main front-end applications and core conversion modules.

- **dlb_socket/** Cross-platform socket component.

- **dlb_st2110/** IP stream management layer

- **dlb_wave/** Component providing read/write to Microsoft (broadcast) audio WAV format.

- **dlb_xmllib/** XML parser.

- **googletest/** C++ test framework from Google.

- **Lawo/** Ember+ console integration library from Lawo.

- **libui/** Cross-platform GUI library.

- **portaudio/** Portable real-time audio library.

- **xerces/** Another XML parser used by the test framework.

- **zlib/** General purpose compression library.


### Prerequisites

For all platforms, 64-bit targets are supported. (tested on Linux)
For Linux, 32-bit platforms are supported.

The following tools are required for building the libraries and tools:

- CMake >= 3.18
- Conan 1.XX (tested on 1.58)
- Git LFS


```
sudo apt-get install python3 python3-pip
```
```
pip3 install cmake conan==1.58
```

#### Professional Sound Card

The following packages are required when building under Ubuntu/Debian and
using a professional sound card:

```
sudo apt-get install build-essential libgtk-3-dev libasound2-dev libjack-dev
```

#### NVIDIA ConnectX

When using an NVidia ConnectX SMARTNIC ethernet card the appropriate NVIDIA
ConnectX ethernet driver and Rivermax SDK must be installed. Not all SDK versions 
are supported, if in doubt of which versions to install, consult the 
[PMD Studio Frequently Asked Questions](PmdStudioFAQ.md). Please contact NVIDIA 
regarding the Rivermax SDK and installation instructions. In addition the following 
packages are required when building under Ubuntu/Debian

```
sudo apt-get install build-essential linuxptp libgtk-3-dev python python3 python3-pip libavahi-client-dev libavahi-compat-libdnssd-dev libnss-mdns avahi-utils gcc-9 libcurl4-openssl-dev libasound2-dev libjack-dev
```

Before running the application, the linux machine must be configured to use PTP
(Precision Time Protocol). This will vary depending on the distribution used with
the insructions here being for Ubuntu.
1. Disable NTP. This is found in Settings->Details->Date&Time. Set automatic setting
of time and date to off.

2. Edit the system service unit file for ptp4l using sudo systemctl edit ptp4l using
the following or similar. Edit the ptp4l.conf file with the correct PTP settings for
your network.
```
[Service]
ExecStart=
ExecStart=/usr/sbin/ptp4l -f /etc/linuxptp/ptp4l.conf -i enp3s0f0
```

3. Repeat for the phc2sys service with the following for the unit file:
```
[Service]
ExecStart=
ExecStart=/usr/sbin/phc2sys -s enp3s0f0 -w -m -n 0
```

4. Apply the settings and restart the services:
```
sudo systemctl daemon-reload
sudo systemctl enable ptp4l phc2sys
sudo systemctl restart ptp4l phc2sys
systemctl status ptp4l phc2sys
```
Verify that the last status command shows the services running with stable clock updates.

### Build instructions

Building dlb_pmd follows a similar process as most CMake-ready projects:
1. Create a build directory and enter it (e.g. `mkdir build; cd build`)
2. Generate makefiles using CMake with optional variables specified: <br>

| Variable name             | Possible values      | Description | Default value |
|---------------------------|----------------------|-------------|---------------|
| BUILD_PMD_STUDIO_RIVERMAX | TRUE / FALSE         | Builds dlb_pmd_studio_rivermax (requires Rivermax SDK, use when using NVidia ConnectX SMARTNIC ethernet card) | FALSE |
| RIVERMAX_API_INCLUDE_DIR  | Path                 | Specify the Rivermax API directory | `/usr/include/mellanox/` |
| CMAKE_BUILD_TYPE          | "Release" / "Debug"  | Project build type | "Release" |

  Example: `cmake .. -DCMAKE_BUILD_TYPE="Release" -DBUILD_PMD_STUDIO_RIVERMAX=TRUE`

3. Build the project with `cmake --build .`

If issues arise, consult the [Frequently Asked Questions](PmdStudioFAQ.md) document.

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
either S-ADM or PMD formats or streamed using a professional sound card.  

Please see the [PMD Studio Quick Start Guide](PmdStudioQsg.md)

Basic usage of the command line when launching PMD Studio is obtained using '-h'.

![Screenshot of PMD Studio](pmd_studio.png)

## Testing the library

Along the project, two testing tools are built - `pmd_test` and `pmd_unit_tests`. 

Both are built on top of the googletest framework, so the complete
suite can be run simply by running the executable without arguments.
Note that the tests take a long time to start up, and take several
hours to run.

These tools are also added as CTests, so you can launch them using the `ctest` command.

## Known Limitations

The ADM XML output does not support or use common definitions as defined
in ITU.R BS 2094.

## Release Notes

See the [Release Notes](ReleaseNotes.md) file for details, including
information on additional applications and features.

## License

This project is licensed under the BSD-3 License - see the
[LICENSE](LICENSE) file for details