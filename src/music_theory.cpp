#pragma comment(user, "license")

#include "music_theory.h"

namespace mm 
{
	
struct Scale
{
	char const*const name;
	uint8_t notes[9];
};

const int sc = 59;
	
// This table is found at http,//www.manythings.org/music/pianotheory/
// When it was obtained on 2012 Nov 24, it was dual licenced under GPL and CC-0.
// In this instance, the CC-0 license has been selected.
Scale scales[sc] = {
	"Aeolian",              0,2,3,5, 7, 8,10,     0xff,0xff,
	"Altered",              0,1,3,4, 6, 8,10,     0xff,0xff,
	"Altered b7",           0,1,3,4, 6, 8, 9,     0xff,0xff,
	"Arabian",              0,2,4,5, 6, 8,10,     0xff,0xff,
	"Augmented",            0,4,7,8,11,           0xff,0xff,0xff,0xff,
	"Balinese",             0,1,3,7, 8,           0xff,0xff,0xff,0xff,
	"Blues",                0,3,5,6, 7,10,        0xff,0xff,0xff,
	"Byzantine",            0,1,4,5, 7, 8,11,     0xff,0xff,
	"Chinese",              0,4,6,7,11,           0xff,0xff,0xff,0xff,
	"Chinese Mongolian",    0,2,4,7, 9,           0xff,0xff,0xff,0xff,
	"Diminished (H-W)",     0,1,3,4, 6, 7, 9,10,  0xff,
	"Diminished (W-H)",     0,2,3,5, 6, 8, 9,11,  0xff,
	"Dorian",               0,2,3,5, 7, 9,10,     0xff,0xff,
	"Dorian b2",            0,1,3,5, 7, 9,10,     0xff,0xff,
	"Dorian #4",            0,2,3,6, 7, 9,10,     0xff,0xff,
	"Double Harmonic",      0,1,4,5, 7, 8,11,     0xff,0xff,
	"Enigmatic",            0,1,4,6, 8,10,11,     0xff,0xff,
	"Egyptian",             0,2,5,7,10,           0xff,0xff,0xff,0xff,
	"Eight Tone Spanish",   0,1,1,4, 5, 6, 8,10,  0xff,
	"Harmonic Minor",       0,2,3,5, 7, 8,11,     0xff,0xff,
	"Hindu",                0,2,4,5, 7, 8,10,     0xff,0xff,
	"Hirajoshi",            0,2,3,7, 8,           0xff,0xff,0xff,0xff,
	"Hungarian Major",      0,3,4,6, 7, 9,10,     0xff,0xff,
	"Hungarian Minor",      0,2,3,6, 7, 8,11,     0xff,0xff,
	"Hungarian Gypsy",      0,2,3,6, 7, 8,11,     0xff,0xff,
	"Ichikosucho",          0,2,4,5, 6, 7, 9,11,  0xff,
	"Ionian",               0,2,4,5, 7, 9,11,     0xff,0xff,
	"Ionian Aug",           0,2,4,5, 8, 9,11,     0xff,0xff,
	"Kumoi",                0,2,3,7, 9,           0xff,0xff,0xff,0xff,
	"Leading Whole Tone",   0,2,4,6, 8,10,11,     0xff,0xff,
	"Locrian",              0,1,3,5, 6, 8,10,     0xff,0xff,
	"Locrian 2",            0,2,3,5, 6, 8,10,     0xff,0xff,
	"Locrian 6",            0,1,3,5, 6, 9,10,     0xff,0xff,
	"Lydian",               0,2,4,6, 7, 9,11,     0xff,0xff,
	"Lydian Aug",           0,2,4,6, 8, 9,11,     0xff,0xff,
	"Lydian b7",            0,2,4,6, 7, 9,10,     0xff,0xff,
	"Lydian #9",            0,3,4,6, 7, 9,11,     0xff,0xff,
	"Lydian Diminished",    0,2,3,6, 7, 9,11,     0xff,0xff,
	"Lydian Minor",         0,2,4,6, 7, 8,10,     0xff,0xff,
	"Melodic Minor",        0,2,3,5, 7, 9,11,     0xff,0xff,
	"Mixolydian",           0,2,4,5, 7, 9,10,     0xff,0xff,
	"Mixolydian b6",        0,2,4,5, 7, 8,10,     0xff,0xff,
	"Mohammedan",           0,2,3,5, 7, 8,11,     0xff,0xff,
	"Neopolitan",           0,1,3,5, 7, 8,11,     0xff,0xff,
	"Neopolitan Major",     0,1,3,5, 7, 9,11,     0xff,0xff,
	"Neopolitan Minor",     0,1,3,5, 7, 8,10,     0xff,0xff,
	"Overtone",             0,2,4,6, 7, 9,10,     0xff,0xff,
	"Pelog",                0,1,3,7, 8,           0xff,0xff,0xff,0xff,
	"Pentatonic Major",     0,2,4,7, 9,           0xff,0xff,0xff,0xff,
	"Pentatonic Minor",     0,3,5,7,10,           0xff,0xff,0xff,0xff,
	"Persian",              0,1,4,5, 6, 8,11,     0xff,0xff,
	"Phrygian",             0,1,3,5, 7, 8,10,     0xff,0xff,
	"Phrygian Major",       0,1,4,5, 7, 8,10,     0xff,0xff,
	"Prometheus",           0,2,4,6, 9,10,        0xff,0xff,0xff,
	"Prometheus Neopolitan",0,1,4,6, 9,10,        0xff,0xff,0xff,
	"Purvi Theta",          0,1,4,6, 7, 8,11,     0xff,0xff,
	"Six Tone Symmetrical", 0,1,4,5, 8, 9,        0xff,0xff,0xff,
	"Todi Theta",           0,1,3,6, 7, 8,11,     0xff,0xff,
	"Whole Tone",           0,2,4,6, 8,10,        0xff,0xff,0xff
};
	
int scaleCount() { return sc; }
	
char const*const scaleName(int scale)
{
	if (scale < 0 || scale >= sc)
		return "Unknown";
		
	return scales[scale].name;
}
	
int scale(int index, uint8_t notes[9])
{
	if (index < 0 || index >= sc) 
	{
		for (int i = 0; i < 9; ++i)
			notes[i] = 0xff;
		return 0;
	}
		
	int ret = -1;
	for (int i = 0; i < 9; ++i) 
	{
		uint8_t n = scales[index].notes[i];
		if (ret < 0 && n == 0xff)
			ret = i;
			
		notes[i] = n;
	}
	return ret;
}

struct Chord
{
	char const*const name;
	uint8_t notes[8];
};
	
const int cc = 42;
	
// This table is found at http,//www.manythings.org/music/pianotheory/
// When it was obtained on 2012 Nov 24, it was dual licenced under GPL and CC-0.
// In this instance, the CC-0 license has been selected.
Chord chords[cc] = {
	"Major",         0,4,7,             0xff,0xff,0xff,0xff,0xff,
	"Minor",         0,3,7,             0xff,0xff,0xff,0xff,0xff,
	"Sus4",          0,5,7,             0xff,0xff,0xff,0xff,0xff,
	"b5",            0,4,6,             0xff,0xff,0xff,0xff,0xff,
	"Diminished",    0,3,6,             0xff,0xff,0xff,0xff,0xff,
	"Augmented",     0,4,8,             0xff,0xff,0xff,0xff,0xff,
	"Major 6",       0,4,7,9,           0xff,0xff,0xff,0xff,
	"Minor 6",       0,3,7,9,           0xff,0xff,0xff,0xff,
	"7",             0,4,7,10,          0xff,0xff,0xff,0xff,
	"7sus4",         0,5,7,10,          0xff,0xff,0xff,0xff,
	"Minor 7",       0,3,7,10,          0xff,0xff,0xff,0xff,
	"Minor 7 +9",    0,3,7,10,14,       0xff,0xff,0xff,
	"Minor 7 +9+11", 0,3,7,10,14,17,    0xff,0xff,
	"Major 7",       0,4,7,11,          0xff,0xff,0xff,0xff,
	"Major 7 +9",    0,4,7,11,14,       0xff,0xff,0xff,
	"Major 7 +#11",  0,4,7,11,14,18,    0xff,0xff,
	"Major7(13)",    0,4,7,11,21,       0xff,0xff,0xff,
	"Major7(9,13)",  0,4,7,11,14,21,    0xff,0xff,
	"Major7#5",      0,4,8,11,          0xff,0xff,0xff,0xff,
	"Major7#5(9)",   0,4,8,11,14,       0xff,0xff,0xff,
	"MinMaj7",       0,3,7,11,          0xff,0xff,0xff,0xff,
	"MinMaj7(9)",    0,3,7,11,14,       0xff,0xff,0xff,
	"7b5",           0,4,6,10,          0xff,0xff,0xff,0xff,
	"Minor7b5",      0,3,6,10,          0xff,0xff,0xff,0xff,
	"Augmented 7",   0,4,8,10,          0xff,0xff,0xff,0xff,
	"Diminished 7",  0,3,6,9,           0xff,0xff,0xff,0xff,
	"Add9",          0,4,7,14,          0xff,0xff,0xff,0xff,
	"Minor +9",      0,3,7,14,          0xff,0xff,0xff,0xff,
	"Major 6 +9",    0,4,7,9,14,        0xff,0xff,0xff,
	"Minor 6 +9",    0,3,7,9,14,        0xff,0xff,0xff,
	"7(9)",          0,4,7,10,14,       0xff,0xff,0xff,
	"7(b9)",         0,4,7,10,13,       0xff,0xff,0xff,
	"7(#9)",         0,4,7,10,15,       0xff,0xff,0xff,
	"7(13)",         0,4,7,10,21,       0xff,0xff,0xff,
	"7(b13)",        0,4,7,10,20,       0xff,0xff,0xff,
	"7(9,13)",       0,4,7,10,14,21,    0xff,0xff,
	"7(b9,13)",      0,4,7,10,13,21,    0xff,0xff,
	"7(#9,13)",      0,4,7,10,15,21,    0xff,0xff,
	"7(b9,b13)",     0,4,7,10,13,20,    0xff,0xff,
	"7(#9,b13)",     0,4,7,10,15,20,    0xff,0xff,
	"7(9,#11)",      0,4,7,10,14,18,    0xff,0xff,
	"7(9,#11,13)",   0,4,7,10,14,18,21, 0xff,
};
	
int chordCount() { return cc; }
	
char const*const chordName(int index)
{
	if (index < 0 || index >= cc)
		return "Unknown";
		
	return chords[index].name;
}
	
int chord(int index, uint8_t notes[8])
{
	if (index < 0 || index >= cc) 
	{
		for (int i = 0; i < 8; ++i)
			notes[i] = 0xff;
		return 0;
	}
		
	int ret = -1;
	for (int i = 0; i < 8; ++i) 
	{
		uint8_t n = chords[index].notes[i];
		if (ret < 0 && n == 0xff)
			ret = i;
			
		notes[i] = n;
	}
	return ret;
}
	
int chord(int index, uint8_t notes[8], int inversion)
{
	int n = chord(index, notes);

	if (!n)
		return n;
		
	for (int i = 0; i < n; ++i)
		notes[i] = (notes[i] + inversion) % 12;

	return n;
}
	
uint8_t (&cirlceOfFifths())[12]
{
	static uint8_t fifths[12] = {0,7,2,9,4,11,6,1,8,3,10,5};
	return fifths;
}
	
bool (&blackKeys())[12]
{
	const bool b = true;
	const bool w = false;
	static bool keys[12] = {w,b,w,b,w,w,b,w,b,w,b,w};
	return keys;
}

/*
	A port to C++ of an implementation of the Bjorklund algorithm as implemented in JavaScript 
	by Jack Rutherford.
	 
	http://blog.fader.co.uk/post/11519018856/bjorklund-algorithm-and-euclidean-rhythms
	 
	Inspired by the paper 'The Euclidean Algorithm Generates Traditional Musical Rhythms'
	by Godfried Toussaint
	 
	http://cgm.cs.mcgill.ca/~godfried/publications/banff.pdf
	 
	This is a port of the original algorithm by E. Bjorklund found in the paper 'The Theory of 
	Rep-Rate Pattern Generation in the SNS Timing Systems' by E. Bjorklund.
	 
	https://ics-web.sns.ornl.gov/timing/Rep-Rate%20Tech%20Note.pdf
	*/
	 
	void build(int level, int r,  std::vector<uint8_t>& pattern, std::vector<int>& counts, std::vector<int>& remainders) 
	{
	r++;

	if (level > -1) 
	{
	 	for (int i=0; i < counts[level]; i++) 
	 	{
	 		build(level-1, r, pattern, counts, remainders);
	 	}
	 	if (remainders[level] != 0) 
	 	{
	 		build(level-2, r, pattern, counts, remainders);
	 	}
	} 
	else if (level == -1)
	{
	 	pattern.push_back(0);
	} 
	else if (level == -2)
	{
	 	pattern.push_back(1);
	}
	}
	 
	void rhythm(int steps, int pulses, std::vector<uint8_t> & pattern) 
	{
	pattern.clear();
	pattern.reserve(steps);

	if (pulses > steps || pulses == 0 || steps == 0) 
	{
	 	return;
	}
	 	
	std::vector<int> counts;
	std::vector<int> remainders;
	 	
	int divisor = steps - pulses;
	remainders.push_back(pulses);
	int level = 0;
	 	
	while(true) 
	{
	 	counts.push_back(divisor / remainders[level]);

	 	remainders.push_back(divisor % remainders[level]);

	 	divisor = remainders[level];

	 	level += 1;

	 	if (remainders[level] <= 1) 
	 	{
	 		break;
	 	}
	}
	 	
	counts.push_back(divisor);
	 	
	build(level, 0, pattern, counts, remainders);
	}

} // mm
