PMD 1.7.3.34 release notes
==========================

Professional Metadata (PMD) is a format for specifying and
transmitting 'next-generation' audio authoring metadata in
low-latency, real-time broadcaster workflows. For example, an
outside-broadcast mixing truck might encode Atmos-enabled content
using this format before it transmits the content to the studio.

PMD is conveyed either in XML, for human-readable, file-based
workflows, or a compact binary representation for low-latency
real-time workflows.  The PMD library provides a data structure that
can be programmatically built and then translated to and from XML and
to and from this binary format.

The PMD library contains support for conversion between PMD and an
object-based audio subset of the serialized Audio Definition Model
(S-ADM) format.

Supported/Tested Platforms:
---------------------------
- 64-bit Windows, Microsoft Visual Studio 2015 and 2017 compilers
- 64-bit OSX, clang compiler
- 64-bit Linux, GNU compiler
- 32-bit Linux, GNU compiler
- Build files for other platforms may be provided, but are not tested

Components:
-----------
- Core PMD library, static and dynamic link versions
- pmd_tool library: higher-level file-based operations, static and dynamic
  link versions
- pmd_tool application: convert between various file-based representations
- pmd_studio application: GUI-based tool for creating PMD and S-ADM XML files,
  streaming audio containing PMD or S-ADM metadata, and interfacing with
  Lawo consoles and applications via the Ember+ library
- pmd_realtime application: experimental code for streaming PMD with PCM

Test applications:
------------------
- pmd_unit_test application: collection of developer unit tests
- pmd_test application: extensive test of library features, will run for a
  very long time!
- pmd_fuzz application: "fuzz" testing for PMD
- sadm_fuzz application: "fuzz" testing for S-ADM

Known issues:
-------------
- To build the test applications on Linux, you need the ICU unicode library and
  header files (http://site.icu-project.org/home)
- For pmd_realtime on Linux you need ALSA (https://alsa-project.org/wiki/Main_Page)
- pmd_studio will not build on Windows

Changes since 1.7.3.33:
-----------------------
- Revise a section of documentation in the PmdStudioQsg.md file.
- Add "por" to the list of supported language codes in pmd_studio.
- Rewrite audioChannelFormat parsing code in the S-ADM reader to make typeLabel
  and typeDefinition optional; and if both are present, check that they are
  consistent.

Changes since 1.7.2:
--------------------
- Major improvements to the pmd_studio application.
- PMDLIB-197: Improve error handling.  Added an error callback mechanism in the
  PMD library API.
- PMDLIB-193: object coordinates are slightly inaccurate in S-ADM.  Compensate
  in S-ADM output for quantization of coordinate values in the PMD model.
- PMDLIB-180: S-ADM Stereo 2.0 beds have illegal audioPackFormatID value of
  AP_00011000.  Start the counter at 1001 instead of 1000.
- PMDLIB-173: XML files may contain numbers with comma for decimal point in
  certain locales.  Locale-proofed the XML writers.
- PMDLIB-167: Add s337m wrapping bit depth configuration.  Added a function to
  the PMD library API to control wrapping bit depth.
- PMDLIB-146: Change pmd_studio language codes to bibliographic.

Changes since 1.7.1:
--------------------
- PMDLIB-75: Got "flowID" attribute of frameFormat element working end-to-end.
- If memory pointer is NULL, use malloc() to acquire memory in dlb_pmd_init() and
  dlb_pmd_init_constrained().  Call free() in dlb_pmd_finish() as appropriate.
- Added a dynamic library/shared object build for the PMD library.  Added a build
  for pmd_unit_test (pmd_unit_test_dynamic) that uses the shared library.
- Added bed gain to pmd_studio.
- Added bit depth 24 SMPTE 337m unwrapping (previously only 20-bit was supported).
- Added support for ADM common definitions, and increased the number of serial ADM
  tags that are recognized.
- PMDLIB-108: S-ADM transportTrackFormat element was, incorrectly, a child of
  frameFormat; when writing, move it out one level to be a child of frameHeader
  instead. Allow reading of transportTrackFormat in the old positioning for
  backwards compatibility.
- PMDLIB-109: S-ADM frameFormat ID is specified to have 11 hexadecimal digits.
  Changed the code to read 11 digits, and also 8 for backwards compatibility,
  and write 11.
- PMDLIB-101: S-ADM -- Allow multiple audioObject instances in an audioContent.
- PMDLIB-130: S-ADM encoding -- the SMPTE 337m encoder was saying it was not
  including an assemble info (AI) word, but writing it anyway, causing the decoder
  to be unable to read the bitstream.
- PMDLIB-129: S-ADM decoding -- If the PCM+PMD extractor was not initialized for
  S-ADM, and encountered S-ADM, the library would crash with a memory access error. 
  Now it just ignores the S-ADM.
- Added the frame capture module: dlb_pmd_frame_captor_xxx().  This will find and
  extract PMD from a capture of a PCM+PMD stream.
- Changed the compiler toolchain from GNU to clang on OSX.
- Moved pmd_tool functionality into a library named dlb_pmd_tool_lib, leaving
  just main() in the application.  This makes file-based PMD and S-ADM operations
  available to other applications by linking to the library.  Added a DLL/shared
  object version of the library and a version of pmd_tool (pmd_tool_dynamic) that
  uses it.
- PMDLIB-127: Improve the documentation for using the DLB_PMD_VSYNC_NONE value
  for the video sync parameter in the PCM+PMD augmentor and extractor.
- PMDLIB-131: S-ADM decoding -- reject multi-burst transmission mode; reject use of
  assemble info (AI) word for full-frame mode.  In other words, reject use of the
  AI word, at least until we support multi-burst mode.
- PMDLIB-133: S-ADM decoding -- frame format (FF) word must be present and must
  indicate gzip format, otherwise reject the data burst.
- pmd_studio: Added real-time streaming functionality to support metadata emulation
  and monitoring.
- Significantly reorganized the SMPTE 337m code module to enable writing correctly
  an odd number of data burst words in frame mode.
- PMDLIB-138: Reading PLD payload incorrectly resets presentation index.
- PMDLIB-139: Reading PLD incorrectly associates loudness correction type with
  dialog gating practice.
- PMDLIB-125: Enhance the "try frame" feature to work with serial ADM.
- PMDLIB-149: S-ADM -- count correctly the number of tracks for generating the
  transportTrackFormat element.
- S-ADM: added support for additional elements and attributes needed for the EBU
  trials held in March and June, 2020 (PMDLIB-155).  Changed S-ADM output encoding
  to use bit depth 24 SMPTE 337m wrapping (PMDLIB-62).
- PMDLIB-140: S-ADM -- always write the cartesian element for audioBlockFormat.
- PMDLIB-164: Don't write out S-ADM common definitions.
- PMDLIB-148: In pmd_studio, change the flowID/IAT UUID each time we change the
  model.
- PMDLIB-159: S-ADM -- write out changedIDs element only when necessary; don't
  write it in full-frame transmission mode.
- PMDLIB-156: S-ADM -- correct the audioContent dialog element for objects of type
  generic.
- PMDLIB-160: S-ADM -- audioObject must be able to refer to multiple children.
- PMDLIB-162: S-ADM -- handle audio object grouping correctly.
- PMDLIB-163: S-ADM -- detect incomplete or inconsistent usage of coordinate type
  and cartesian element in audioBlockFormat.
- PMDLIB-145: S-ADM -- added preliminary support for audioFormatCustom element.
- S-ADM -- detect custom PMD 7.0.4 bed representation and generate the correct S-ADM
  structures.
- PMDLIB-174: pmd_tool -- add "--version" command line argument, and update how
  the version information is printed.
- PMDLIB-177: S-ADM -- some generated audioChannelFormat/audoBlockFormat objects for
  DirectSpeakers had incorrect names, speaker labels, and/or positions.
- PMDLIB-178: S-ADM -- gzip header is missing from SMPTE-wrapped, compressed
  databurst.

Changes since 1.7.0:
--------------------
- Fixed a bug in the PCM extractor causing framing errors.
- Updated the S-ADM implementation to be consistent with the ITU-R BS.2076-2 and
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
    - S-ADM-in-SMPTE 337m PCM reader/writer
    - converter to and from PMD with unit tests
    - S-ADM positive XML fuzzer
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
