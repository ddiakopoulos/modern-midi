# ModernMIDI

ModernMIDI brings together a number of support classes to read, manipulate, and generate MIDI data. For realtime input/output, the library gently wraps RtMidi for cross-platform hardware IO. Furthermore, ModernMIDI includes a comprehensive file io module for reading and writing format-0 files. Other features include common music theory concepts expessed in MIDI, along with a decently performing real-time file player. It's currently thin on the example/documentation front, but more samples will be forthcoming.

In various forms, it has been used in performances with the [Seattle Symphony](http://www.geekwire.com/2015/conducting-with-kinect-seattle-symphony-to-use-microsofts-3d-sensor-in-world-premiere-performance/), installations at the [Jewish Contemporary Museum in SF](http://www.thecjm.org/about/press/press-releases/956-pour-crever-by-trimpin), and in many smaller Arduino prototypes in conjunction with [HIDUINO](https://github.com/ddiakopoulos/hiduino).

This code is functional but rarely maintained. For a newer take on midi library support in C++, see [RtMidi17](https://github.com/jcelerier/RtMidi17).

## Building / Usage
To run the included exampes, use the included VS2013 or XCode 6 projects. ModernMIDI will build on any platform with a recent C++11 compiler. At present, ModernMIDI is not packaged into a static or dynamic library -- instead, the usage model is that developers will compile the small number of files directly into a new project.

ModernMIDI inherits from the linkage requirements of RtMidi. RtMidi needs a preprocessor macro defined in the project configuration to select the correct backend (e.g. `__MACOSX_CORE__`). On OSX the following frameworks must be included in a project: `CoreAudio.framework`, `CoreMIDI.framework`, and `CoreFoundation.framework`. On Windows, `winmm.lib` must be included although this is automatically handled in `modernmidi.h`.

## Acknowledgements
ModernMIDI is a continuation of the functionality developed in [LabMIDI](https://github.com/meshula/LabMidi) by [@meshula](https://twitter.com/meshula). 

## License
ModernMIDI is released under the simplified BSD 2-clause license. See the COPYING file for more information on the (license-compatible) third party code included within ModernMIDI. 
