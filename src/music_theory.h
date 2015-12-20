/*
Copyright (c) 2015, Dimitri Diakopoulos All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:

* Redistributions of source code must retain the above copyright notice, this
  list of conditions and the following disclaimer.

* Redistributions in binary form must reproduce the above copyright notice,
  this list of conditions and the following disclaimer in the documentation
  and/or other materials provided with the distribution.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#pragma once

#ifndef MODERNMIDI_THEORY_H
#define MODERNMIDI_THEORY_H

#include "modernmidi.h"
#include <stdint.h>
#include <vector>

namespace mm 
{
    const std::vector<int> circle_of_fifths = {0,7,2,9,4,11,6,1,8,3,10,5};
    
    // The number of scales encoded in this module
    int scaleCount();

    // The name of the indexed scale. If out of range, "Unknown" is returned.
    std::string name_for_scale_index(int scale);
    
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
    std::string name_for_chord_index(int scale);

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

} // mm

#endif
