#pragma comment(user, "license")

#pragma once

#ifndef METAMIDI_THEORY_H
#define METAMIDI_THEORY_H

#include "metamidi.h"
#include <stdint.h>
#include <vector>

namespace mm 
{
	// The number of scales encoded in this module
	int scaleCount();

	// The name of the indexed scale. If out of range, "Unknown" is returned.
	char const * const scaleName(int scale);

	// The notes of the indexed scale. If out of range, all the notes will
	// be written to 0xff. The return value is the number of notes in the scale
	//
	// Notes will be zero based. The user should add the tonic note to get
	// a scale to play. For example, to play a scale in the key of C, add 60
	// to the values in the notes array.
	//
	// If the base note is >= 115, be careful not to play out of range notes.
	// MIDI notes range from 0 to 127.
	int scale(int index, uint8_t notes[9]);

	// The number of chords encoded in this module
	int chordsCount();

	// The name of the indexed chords. If out of range, "Unknown" is returned.
	char const*const chordName(int scale);

	// The notes of the indexed chord. If out of range, all the notes will
	// be written to 0xff. The return value is the number of notes in the chord
	//
	// inversion is the amount (positive or negative) to invert the chord by
	//
	// Notes will be zero based. The user should add the tonic note to get
	// a scale to play. For example, to play a scale in the key of C, add 60
	// to the values in the notes array.
	//
	// If inversion was non-zero, be careful that the adjusted notes are not out
	// of range. MIDI notes range from 0 to 127.
	int chord(int index, uint8_t notes[9]);
	int chord(int index, uint8_t notes[9], int inversion);

	// Returns a reference to an array containing the circle of fifths
	// Notes are zero based. The usual MIDI range caveats apply.
	uint8_t (&cirlceOfFifths())[12];

	// Returns a reference to an array containing bools indicating where
	// the black keys are on a piano keyboard. The 0th index indicates C.
	// blackKeys()[n % 12] answers the question for any note.
	bool (&blackKeys())[12];

	// Given a number of steps, and the number of beats that occurs during those
	// steps, fill in the result array with a pattern where those beats are
	// evenly distributed according to Bjorklund's algorithm.
	void rhythm(int steps, int beats, std::vector<uint8_t>& result);

} // mm

#endif
