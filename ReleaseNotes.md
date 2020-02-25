PMD 1.7.1 release notes
=======================

Professional Metadata (PMD) is a format for specifying and
transmitting 'next-generation' audio authoring metadata in
low-latency, real-time broadcaster workflows. For example, an
outside-broadcast mixing truck might encode Atmos-enabled content
using this format before it transmits their content to the studio.

PMD is conveyed either in XML, for human-readable, file-based
workflows, or a compact binary representation for low-latency
real-time workflows.  The PMD library provides a data structure that
can be programmatically built and then translated to and from XML and
to and from this binary format.

The PMD library contains experimental support for conversion between
PMD and a restricted subset of the serialized Audio Definition Model
(sADM) format.

Supported/Tested Platforms:
---------------------------
- 64-bit Windows, Microsoft Visual Studio 2015 compiler
- 64-bit OSX, GNU compiler
- 64-bit Linux, GNU compiler
- 32-bit Linux, GNU compiler
- Build files for other platforms may be provided, but are not tested

Components:
-----------
- Core PMD library
- pmd_tool application: convert between various file-based representations
- pmd_studio application: GUI-based tool for creating PMD XML files (alpha
  release)
- pmd_realtime application: experimental code for streaming PMD with PCM

Testing:
--------
- pmd_unit_test application: small collection of developer unit tests
- pmd_test application: extensive test of library features, will run for a
  very long time!
- pmd_fuzz application: "fuzz" testing for PMD
- sadm_fuzz application: "fuzz" testing for sADM

Known issues:
-------------
- To build the test applications on Linux, you need the ICU unicode library and
  header files (http://site.icu-project.org/home)
- For pmd_studio on Linux and OSX, you need libui (https://github.com/andlabs/libui)
- For pmd_realtime on Linux you need ALSA (https://alsa-project.org/wiki/Main_Page)

Changes since 1.7.0:
--------------------
- Fixed a bug in the PCM extractor causing framing errors.
- Updated the sADM implementation to be consistent with the ITU-R BS.2076-2 and
  BS.2125-0 specification documents.

Changes since 1.6.0:
--------------------
- New version of the pmd_studio application, v0.2 (alpha).
- Fixed pmd_studio to always add the IAT payload.
- Added the pmd_unit_test application.
- Fixed googletest library build to eliminate multiply-defined symbols.
- Changed libui build to enable C++ exception handling.
- Refined equality predicate functions for PMD models.
- [PMDLIB-1] Increased the number of bytes reserved for a PMD entity name from 64 to 67.
- Fixed copying of a PMD model to include AEN list.
- Corrected the encoding of APD speaker configuration.
- Fixed encoding/decoding of program boundary in PLD.
- [PMDLIB-42] Fix the deserialization of the title of an untitled program.
- Added support to the PMD extractor and KLV reader for additional status reporting.
- [PMDLIB-44(a)] Rewrote the PCM extractor to be driven by the actual PA spacing instead
  of the expected PA spacing.
- [PMDLIB-44(b)] Added "try frame" feature to the PCM augmentor and pmd_tool.
- [PMDLIB-38] Allow single-channel or an odd number of channels in the input to the PCM
  augmentor and pmd_tool when appropriate instead of requiring a minimum of or a multiple
  of two channels.
- [PMDLIB-51] Added payload logging to pmd_tool.
- Cleaned up help message for pmd_tool.
- In pmd_test, fixed the encoding of dialnorm values for testing EEP and ETD.


Changes since 1.5.0:
--------------------
- [DPF-2423] bug to make sure APN are transmitted out of ED2 decoder
- serial ADM support (Dolby Profile)
    - XML reader/writer
    - sADM-in-SMPTE 337m PCM reader/writer
    - converter to and from PMD with unit tests
    - sADM positive XML fuzzer
- experimental 'pmd_studio' UI that authors very simple metadata, up to 4 beds, 4 objects
  and 4 presentations only.


Changes since 1.4.1:
--------------------
- PMD XML schema fix: 'use="required"' attribute added to line 388, specifying that
  the language attribute is required part of presentation name
- Fix to PMD-OAMDI generation: pay proper attention to start and duration of metadata
- Fix PCM+PMD augmentation bug when length of last block of 1st video frame is 256
  samples too long
- add 32-sample guard band at start of video frame (PCM+PMD)
- fix Google address sanitize bugs
- fix object divergence computation in the PMD programmatic API, and fix allowed
  range of divergence to map to [-1,+1] range of x coordinate.
- add 8x1 option to ED2v2 stream generation (where only 8x1 Dolby E program configurations
  are allowed)
- add bed gains in OAMDI generation
- [DPF-2011] allow model sizes to be constrained by 'max entity count' constraints
  at init time
- Increase IAT timestamp automatically at every frame
- Restrict MTx0 transmission slot to include only ABD,AOD,APD and HED payloads
- create pmd_realtime app that can read and write PCM+PMD from PC audio cards via
  portaudio library.  Add support for HTTP transmission and reception of PMD in XML
  format.
- correct return code bug in dlb_pmd_add_presentation2()

 
Changes since 1.4.0:
--------------------
- bug fix to PMD ED2 layout rules


Changes since 1.3.0:
--------------------
- coverity fixes
- minor tidy of pmd_tool.c and it's support for generating pseudo-random models
- correct sense of Y object co-ordinate. Previously 1.0 meant back and -1.0 meant
  front (following the Dolby OAMD order).  Now we follow the standard ADM order,
  where 1.0 means 'front' and -1.0 means 'back'


Changes since 1.2.0:
--------------------

- change Pd value for substream 0 of ED2 systems so that it is 1 word
  less than the others, not 2.  This means the set of ED2 Pd word
  values are distinct from the set of DE Pd word values.
- fix coverity warnings
- fix valgrind warning
- fix clang compiler warnings
- correct the allowed range of LUFS values in the PMD loudness API
	- from [-102.3, 0.0]
	- to [-102.4, +102.3]
- fix MGI ingestion error: we can now generate presentations after receiving
  the first MGI packet, not waiting for 8.


Changes since 1.1:
------------------

- [DPF-1900] clarify how to use the dlb_pmd_presentation, dlb_pmd_bed
  and dlb_pmd_object lookup APIs
- [DPF-1881] add an API to create individual audio signals
- [DPF-1895] add profile/levels to XML schemas
- [DPF-1925] add const constraints to PMD API functions
- [DPF-1856] make 1st ED2 substream's Pd value 2 words shorter than all
  other substreams: a quick way to identify beginning of a new
  ED2 system when multiple are delivered on the same SDI or
  MADI interface
     
- merge in T3 team's translators from DE metadata to ED2v2, and ED2v1
  metadata to ED2v2.
- enhance XML parser's validation of presentation config strings: check
  that the string matches the actual presentation
- add an API to retrieve the OAMDI structure pointer after OAMD
  generation from a model
- add random model generator, and use it to create pmd_fuzz, a positive
  fuzzer (i.e., it checks that things that should work do)

- bugfix: [DPF-1853] fix segfault when errorline wasn't passed in to dlb_xmlpmd_string_read
- bugfix: [BPP-15550] objects with idx > 8 could not be selected as part of a presentation
- bugfix: [DPF-1883] fix issues when selecting PMD signals from within a set, not just at beginning
- bugfix: [BM-775] ensure all channels after a rearrangement are correctly encoded
- bugfix: [DPF-1645] fix memory corruption bug in ed2 arrangement code
- bugfix: metadata set ingestion: allow metadata sets with non-contiguous signals and elements
- bugfix: allow more than one loudness payload in PMD XML schema
- bugfix: fix wrong return value in dlb_xmlpmd_file_read
- bugfix: dlb_pmd_equality - check presentation names for equality
- bugfix: dlb_pmd_set_loudness - fix problem setting loudness correction type
- bugfix: write all channel exclusions of HED payload to XML
