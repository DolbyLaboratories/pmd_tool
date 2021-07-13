# PMD Studio Quick Start Guide
# Version 1.7.4

PMD Studio is an application for authoring professional metadata.
It provides a simple user interface for configuring audio beds, objects
and presentations. The authored metadata may be saved as an XML file in
either Serial ADM or PMD formats or streamed using a professional sound card.

## Standards and References
Serial ADM is defined jointly in ITU-R B.S. 2076 and 2125. Streaming Serial
ADM output is supported over AES3 and SMPTE ST 337 which is defined in
SMPTE ST 2116.

PMD (Professional Metadata) is defined in SMPTE RDD 49.  A copy of this document is available to the public on the
[Dolby Professional Developer Portal](https://developer.dolby.com/dolby-professional/professional-technologies/dolby-atmos-and-nga/professional-metadata-pmd/).
PMD output is supported over AES3 and SMPTE ST 337 which is defined in
SMPTE ST 2109.

## System Requirements

### Operating System

PMD Studio is supported on Mac and Linux only.

Tested on Ubuntu 19.10 for Linux and OSX Mojave. There is a known issue with
OSX Catalina that the menu-bar is unresponsive. The simple
workaroud for the issue to take focus away to another application and then
return focus. The menu-bar will now respond normally.

### Real-time interface

The application supports real-time input of pre-rendered audio and combined
output of rendered audio and metadata.

To support real-time I/O, one of the following hardware options is required

- A Professional multichannel sound card with ALSA (Linux) or Core Audio Drivers (Mac)
- An NVIDIA ConnectX SmartNIC supporting the Rivermax SDK.

#### Professional Multichannel Sound card

The presence of the drivers can be checked using the -dl switch or
checking the device lists in the settings panel. The clock settings must
be sane such that input and output intefaces are using the same clock. The
application has been tested on the folowing hardware:

- RME HDSP MADI card on Ubuntu
- Digigram LX-IP with on Ubuntu
- BlackMagic Decklink 4K Extreme on OSX

## Running the Application

PMD Studio can be simply evoked from the command line without any options.
Some basic options are provided at the command line but most of these can
be overidden with settings in the application.

## Options

* -fb                  File-based mode (overrides streaming options)
* -f <filename>        Filename of file to load on launch
* -l <latency>         Input and output latency in seconds
* -device <option>     Device specific option
* -buf <samples>       Buffer size in samples

### File-based mode
This mode is forced when no streaming audio interfaces can be found. The -fb
switch allows the user to select file-based mode even when audio interfaces
are available. In file-based mode streaming audio and metadata outputs are
not available. XML files can be opened, edit and saved. Certain metadata
options are only available in file-based mode. These include divergence for
objects and extra configuration options for beds.

### Device Specific Options
-device list provides more information about the available hardware devices

### Latency and buffer size
These settings can be used to alter the latency PMD Studio requests from the
sound card and the buffer size that is used for data transfer between PMD
Studio and the driver. Nominal values are chosen by default. To minimize
latency smaller values may be selected. Depending on the performance of the
system decreasing latency may introduce audible artifacts. On older or slower
systems it may be necessary to increase latency to eliminate artifacts. A
modern multi-core system should have little difficulty with the default
settings.

## Basic Operation

When launched the application puts the audio streaming interface into
pass-through. This can be useful for checking basic connectivity. The
pass-through configuration is muted once changes are made on the
user-interface.

### Authoring Metadata

The user interface is split up into four sections:
* Audio Beds
* Audio Objects
* Audio Presentations
* Outputs

Each section has an 'add' button that creates a new entry to be populated
by the user. The beds and objects sections should be configured to reflect
the pre-rendered audio being presented on the streaming audio input.

Beds are audio elements composed of at least two channels. They follow a normal
channel configuration such as 5.1. In streaming mode stereo, 5.1 and 5.1.4
beds are supported. The channel ordering for a 5.1 bed is L, R, C, LFE, Ls, Rs.
For 5.1.4 the channel ordering is L, R, C, LFE, Ls, Rs, TFL, TFR, TBL, TBR
where TFL is top front left (T = Top, F = Front, B = Back, L = Left, R = Right).
The field start refers to the first physical channel of the bed on the input audio
multiplex. Multiple beds may refer to the same physical channels allowing different
gain settings for the different bed entries.

Audio objects work in a similar manner to beds but only represent a mono audio
element. This position of the audio element can be set using Cartesian
coordinate settings.

The 'En' checkbox enables or disables the bed or object.

The Presentations panel defines how the beds and objects are combined to form
presentations. Presentations are also know as presets or preselections.

One bed is allowed per presentation and is selected using the drop-down menu. The
'lang' specifies the language used by the audio in the presentation. Typically
this will be dictated by the language of included dialog objects, but beds can
be premixed with dialog as well, for example in a CM+AD (Complete Main plus
Audio Description) presentation. NLang or Name Language field specifies the language
used by the presentation description in the name field. Selection is by ISO
language code. Three methods exist for the setting of the name languagage. The
first of two automated methods is for the name language field to follow the
presentation language used by the audio. For example, using the following name
strings:
* English
* Francais
* Deutsch

Alternatively the name language can be made to fixed to a single setting defined
in settings panel (follow preset nlang). For example, if the preset NLang is set
to English then the name strings could be:
* English
* French
* German

The last method for the setting of the language is unlocked and allows a custom
nlang setting. In this mode the nlang setting is enabled on the main panel and can
be set to any language code independent of the audio presentation language setting.

### File menu operations

The user interface can be reset using the 'New' option under the file menu. PMD or
ADM files can be opened or saved using the file menu. The settings option opens
the settings panel. When the application is shut down via the quit option in this
menu the last known settings are saved in 'pmd_studio.cfg' in the current directory.
This file is loaded by default when PMD Studio is invoked to restore the last known
settings.

#### Audio Device Configuration

When using a professional sound card an *Audio Device Configuration* menu option will
be available. This allows selection of the input and output audio devices and the
number of channels to be used.

#### Stream Settings

When using an NVIDIA Rivermax enabled NIC, a stream setting menu option is available.
This allows selection of the input and output streams. Input streams are discovered
via multicast DNS and RTSP (Ravenna) or SAP (Dante). Only a single input stream can
be selected but multiple output streams can be defined. For each outputs stream the
name of the stream is defined as well as the output codec to be used and the channel
allocation. Channels are automatically allocated in order based on the number of
channels in each stream. Available codecs are SMPTE ST 2110-30/AES 67 with either
16 or 24 bits or SMPTE ST 2110-31/AM824. The former should not be used for metadata
outputs as SMPTE ST 2110-30 / AES67 is defined to only carry linear PCM. Output
streams are advertised on the network using both Ravenna and Dante methods.

Once the stream settings are applied the IP subsystem will be restarted with the
new settings. A progress bar may be displayed when searching for a Dante stream. This
process can take up to 30 seconds. If the input stream cannot be acquired, an error
message will be displayed.

#### Update

The update option creates
three file representations of the audio metadata:
* Serial ADM XML in sadm.xml
* PMD XML in pmd.xml
* PMD KLV binary format in pmd.klv

A script called 'update' in the current directory, if it exists, is then executed.
This function can be used to drive some method for sending the metadata to a
down-stream device. For example, a common use of this functionality is to perform
an HTTP Post of the XML to an encoder device.

### Configuring outputs

Two types of outputs are available: Audio and Metadata outputs. Audio outputs are
always rendered as per one of the presentation definitions. The presentation to be
rendered is selected using the down-drop menu. The output channel configuration can
be selected between stereo, 5.1 and 5.1.4. The start channel in the output audio
multiplex is defined. The last channel used by that output is calculated and
displayed. When the 'En' or enable checkbox is selected the audio output will be
live. Multiple audio outputs are possible by using the 'Add Audio Output Button'.

Each metadata output be be configured to generate Serial ADM or PMD output as
selected by the format drop-down menu. The Mode selection allows the use of either
frame-mode or subframe-mode as defined in SMPTE ST 337. Frame mode places the metadata
across two consecutive channels. Subframe places all the metadata in a single
mono channel. The metadata repetition rate is fixed at 25fps and is asynchronous.
When a metadata output is enabled PMD Studio enters live mode. In this state the
metadata configuration is fixed except for gain and positional settings. 

### Ember+ client

PMD Studio 1.7.3 introduces a simple Ember+ consumer (client) that allows an Ember+ 
provider (server) on the same network to control a restricted set of metadata values.
The server IP address and port are configured from the settings panel under the 
file menu. Connection to the server or console is initiated using the Connect option 
under "Console" on the menu-bar.

NOTE: The module is currently hard-coded for provider software that is not publicly 
available, and so not intended for use by the average user.

## Known Limitations

OSX Catalina requires focus to be moved from and to application to enable menu-bar.
Audio elements and outputs can be added to the user interface but cannot be
removed. Unused elements and outputs should just be disabled.

Metadata frames are always sent at 25fps.

## Trademarks

NVIDIA, Rivermax and ConnectX are trademarks and/or registered trademarks of NVIDIA Corporation in the U.S. and/or other countries.
BlackMagic and Decklink are trademarks and/or registered trademarks of Blackmagic Design Pty Ltd. 
Digigram is a registered trademark of Digigram S.A.
