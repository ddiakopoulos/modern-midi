# midi

templates! cpp11! 

## todo
- rtaudio midi output
- rtaudio midi interface
- generic concurrent queue / timer
- cross-platform spinny timer
- thread priority 
- platform macros
- common.h
- euclidean rhythm generator
- tick2frame, second2tick, frame2tick
- bpm utilities 
- find attached midi devices by name
- write midi files
- generate scales
- tuning conversions
- midi clock generator
- controller remapping

#ifdef _MSC_VER  
#define INLINE __forceinline
#else
#define INLINE inline
#endif

#define PI 3.1415926535897932384626433832795
#define EULER 2.7182818284590452353602874713527
#define SQRT2 1.4142135623730950488016887242097
#define ONE_OVER_SQRT2 0.70710678118654752440084436210485
#define LN10 2.3025850929940456840179914546844
#define ONE_OVER_LN10 0.43429448190325182765112891891661
#define LN2 0.69314718055994530941723212145818
#define ONE_OVER_LN2 1.4426950408889634073599246810019
#define SEMITONE_FACTOR 1.0594630943592952645618252949463

//some useful macros for VST-Plug-In-Development:
//calculates the logarithm to an arbitrary base:
#define LOGB(x, B) (log((double)x)/log((double)B))

//calculates the logarithm to base 2:
#define LOG2(x) (1.4426950408889634073599246810019 * log((double)x))



//conversion from a pitch offset value in semitones to a frequency scale factor:
#define PITCHOFFSET2FREQFACTOR(pitchOffset) (pow(2, (((double)pitchOffset)/12)))

//conversion from a frequency scale factor to a pitch offset in semitones:
#define FREQFACTOR2PITCHOFFSET(freqFactor) (12*LOG2((double)freqFactor))

//conversion from an amplitude-value to a level value in dB (amplitude 1 corresponds to 0 dB):
#define AMP2DB(amp) (20*log10((double)amp))

//conversion from a level value in dB to an amplitude-value:
#define DB2AMP(dB) (pow(10, (((double)dB)/20)))

INLINE double amp2dB(double amp)
{
  return 8.6858896380650365530225783783321 * log(amp);
  //return 20*log10(amp); // naive version
}

INLINE double amp2dBWithCheck(double amp, double lowAmplitude)
{
  if( amp >= lowAmplitude )
    return 8.6858896380650365530225783783321 * log(amp);
  else
    return 8.6858896380650365530225783783321 * log(lowAmplitude);
}

https://github.com/dliganov/Chaotic-DAW/blob/0d5c204887a43669ed86a52925571b52829a19a3/rosic/basics/GlobalFunctions.h
https://github.com/dliganov/Chaotic-DAW/blob/0d5c204887a43669ed86a52925571b52829a19a3/Awful_utils_common.h

https://github.com/ptarabbia/midiFileWriter/blob/master/midiFileWriter.cpp

https://github.com/perttu606/midi-clock

https://github.com/ahiijny/SynthTest/blob/7d4c473395cbdad6bdc6cac57249bcc73a6a1327/MidiManipulationClasses/MidiWriter.cpp

https://github.com/mynameisntrick/musikinformatik/blob/0ee302c79b370ae9124e0aab907756efb72c59d1/TestMidi/Midi/MidiMessage.h
https://github.com/mynameisntrick/musikinformatik/blob/0ee302c79b370ae9124e0aab907756efb72c59d1/TestMidi/Midi/Binasc.h
https://github.com/craigsapp/improv/blob/master/include/MidiFile.h
https://github.com/craigsapp/midifile -> write routine is pretty legit 