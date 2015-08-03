#pragma comment(user, "license")

#pragma once

#ifndef MODERNMIDI_UTIL_H
#define MODERNMIDI_UTIL_H

#include "modernmidi.h"

namespace mm 
{
	// Convert a MIDI command byte into a descriptive string
	const char * commandName(uint8_t command);

	// Convert a note number to frequency
	// The default value for A is 440.0. A different tuning value
	// can be provided.
	float noteToFrequency(uint8_t note);
	float noteToFrequency(uint8_t, float A);
	
	// Convert a frequency to a note number
	// The default value for A is 440.0. A different tuning value
	// can be provided.
	uint8_t frequencyToNote(float freq);
	uint8_t frequencyToNote(float freq, float A);

} // mm

#endif