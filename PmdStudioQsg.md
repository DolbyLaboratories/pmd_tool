# PMD Studio Quick Start Guide
# Version 1.7.3

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
output of rendered audio and metadata. A Professional multichannel sound card
is required with drivers supporting either ALSA for Linux or Core Audio for
Mac. The presence of the drivers can be checked using the -dl switch or
checking the device lists in the settings panel. The clock settings must
be sane such that input and output intefaces are using the same clock. The
application has been tested using an RME HDSP MADI card with Ubuntu 19.10.
For AES3 I/O the Lynx AES16 and RME AES32 cards are recommend although these
have not been tested. For AES67 / SMPTE 2110-30/31 support the Digigram LX-IP
is recommended although this also has not been tested.

## Running the Application

PMD Studio can be simply evoked from the command line without any options.
Some basic options are provided at the command line but most of these can
be overidden with settings in the application.

* -fb                  File-based mode (overrides streaming options)
* -di <name>           Name of device to use for input
* -do <name>           Name of device to use for output
* -dl                  List input and output device names and details
* -c <channels>        Number of input and output channels
* -f <filename>        Filename of file to load on launch
* -l <latency>         Input and output latency in seconds
* -buf <samples>       Buffer size in samples
* -am824               Select am824 framing for metadata output (for use with ALSA AES67 driver)

### File-based mode
This mode is forced when no streaming audio interfaces can be found. The -fb
switch allows the user to select file-based mode even when audio interfaces
are available. In file-based mode streaming audio and metadata outputs are
not available. XML files can be opened, edit and saved. Certain metadata
options are only available in file-based mode. These include divergence for
objects and extra configuration options for beds.

### Device Selection
The -di and -do are used to select the streaming interfaces for input and
output. Names are used to identify the devices. A list of the names of
available devices can be obtained using the -dl switch.
These settings are normally obtaining from a configuration file that
is loaded upon invocation. The input and output device selection can be
modified from the settings within the application.

### Channels
By default PMD Studio tries to acquire 32 input and output mono channels for
use by the application. When used with AES3 or SDI sound cards it may be that
32 channels are not available. In this case the number of channels available to
the application can be limited by this setting. The number of input and output
channels available to the application is always the same.

### Latency and buffer size
These settings can be used to alter the latency PMD Studio requests from the
sound card and the buffer size that is used for data transfer between PMD
Studio and the driver. Nominal values are chosen by default. To minimize
latency smaller values may be selected. Depending on the performance of the
system decreasing latency may introduce audible artifacts. On older or slower
systems it may be necessary to increase latency to eliminate artifacts. A
modern multi-core system should have little difficulty with the default
settings.

### AM824 mode
This mode should be selected when the application is used with the ALSA
AES67 driver available [here](https://github.com/bondagit/aes67-linux-daemon).
The use of this switch rearranges the 24-bit metadata output to be compatible
with AM824 or SMPTE ST 2110-31 rather than regular SMPTE ST 337 output which
is the default. In AM824 mode the top 8 bits are used for the AES3 PCUV bits
that carry the channel status. The metadata output is always marked as
non-audio and Professional in the AES3 channel status. In the default mode
only the audio bits are transmitted to be directly compatible with AES3, SDI
and MADI etc.

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

This Ember+ client is hard coded to interface with "AvatusMD Connector" (software) as 
a proof of concept, however there are (untested!) ways in which one could use PMD Studio
to interface with other servers.

#### Getting the Ember+ client to work with other servers
*NOTE: The Ember+ client in PMD Studio is made for, and tested against, AvatusMD
Connector only. Any use outside of this constraint is not guaranteed to work.*

* **Option 1 - Modifying the server:** If the target Ember+ server is flexible enough to
change the Ember+ structure, it could be modified to mirror that of AvatusMD Connector:  
`/AvatusMDConnector/Fader <x>/Gain`  
Where `<x>` is an integer in the range 1<= x <= 8 (values 1-4 control bed gains and 
5-8 object gains), and "Gain" is an Integer-type Ember+ parameter, optionally bound to 
the range -26 and 6 (dB) (as values less than -25 or greater than 6 are clipped to -inf
and 6 respectively).

* **Option 2 - Modifying PMD Studio:** If, for whatever reason, you can't modify the
server, you could try modifying PMD Studio's source code to target different Ember+
parameter addresses (in the file "dlb_pmd/frontend/pmd_studio/pmd_studio_console_emberplus.cpp"):
    1. Near the top of the file: The eight PATH_AVATUS_FADER_*_GAIN variables contain the 
    identifier paths to each of AvatusMD Connector's parameters as a string array. Edit these to 
    match the identifier path of the desired parameter in the server.
    2. In the PMDStudioConsoleEmberPlus constructor: Change the value (currently '`3`') of the 
	second argument of each of the calls to PMDStudioConsoleEmberPlus::CallbackManager::queue(), 
	to equal the new length of each of the arrays; for example, `["foo", "bar"]` has length 2.

## Known Limitations

OSX Catalina requires focus to be moved from and to application to enable menu-bar.
Audio elements and outputs can be added to the user interface but cannot be
removed. Unused elements and outputs should just be disabled.
