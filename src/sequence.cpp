#pragma comment(user, "license")

#include "metamidi.h"
#include "sequence.h"
#include "midi_message.h"
#include <cmath>

namespace mm 
{
	// read a MIDI-style variable-length integer
	// (big-endian value in groups of 7 bits,
	// with top bit set to signify that another byte follows)
	int readVarInt(uint8_t const*& dataStart)
	{
		int result = 0;
		while (true) 
		{
			uint8_t b = *dataStart++;
			if (b & 0x80) 
			{
				result += (b & 0x7f);
				result <<= 7;
			} 
			else 
			{
				return result + b; // b is the last byte
			}
		}
	}
	
	int readInt16(uint8_t const * & dataStart)
	{
		int result = int( * dataStart++) << 8;
		result += int( * dataStart++);
		return result;
	}
	
	int readInt24(uint8_t const*& dataStart)
	{
		int result = int(*dataStart++) << 16;
		result += int(*dataStart++) << 8;
		result += int(*dataStart++);
		return result;
	}
	
	int readInt32(uint8_t const*& dataStart)
	{
		int result = int(*dataStart++) << 24;
		result += int(*dataStart++) << 16;
		result += int(*dataStart++) << 8;
		result += int(*dataStart++);
		return result;
	}
	
	MidiTrackEvent * parseEvent(uint8_t const*& dataStart, uint8_t lastEventTypeByte)
	{
		uint8_t eventTypeByte = *dataStart++;
		
		if ((eventTypeByte & 0xf0) == 0xf0) 
		{
			/* system / meta event */
			if (eventTypeByte == 0xff) 
			{
				/* meta event */
				uint8_t subtypeByte = *dataStart++;
				int length = readVarInt(dataStart);
				switch(subtypeByte) 
				{
					case 0x00: 
					{
						if (length != 2) throw std::runtime_error("Expected length for sequenceNumber event is 2");
						SequenceNumberEvent* event = new SequenceNumberEvent();
						event->number = * (uint16_t*) dataStart;
						dataStart += 2;
						return event;
					}
					case 0x01: 
					{
						TextEvent* event = new TextEvent();
						event->text.assign((char*) dataStart, length);
						dataStart += length;
						return event;
					}
					case 0x02: 
					{
						CopyrightNoticeEvent* event = new CopyrightNoticeEvent();
						event->text.assign((char*) dataStart, length);
						dataStart += length;
						return event;
					}
					case 0x03: 
					{
						TrackNameEvent* event = new TrackNameEvent();
						event->text.assign((char*) dataStart, length);
						dataStart += length;
						return event;
					}
					case 0x04: 
					{
						InstrumentNameEvent* event = new InstrumentNameEvent();
						event->text.assign((char*) dataStart, length);
						dataStart += length;
						return event;
					}
					case 0x05: 
					{
						LyricsEvent* event = new LyricsEvent();
						event->text.assign((char*) dataStart, length);
						dataStart += length;
						return event;
					}
					case 0x06: 
					{
						MarkerEvent* event = new MarkerEvent();
						event->text.assign((char*) dataStart, length);
						dataStart += length;
						return event;
					}
					case 0x07: 
					{
						CuePointEvent* event = new CuePointEvent();
						event->text.assign((char*) dataStart, length);
						dataStart += length;
						return event;
					}
					case 0x20: 
					{
						if (length != 1) throw std::runtime_error("Expected length for midiChannelPrefix event is 1");
						MidiChannelPrefixEvent* event = new MidiChannelPrefixEvent();
						event->channel = *(uint8_t*) dataStart;
						++dataStart;
						return event;
					}
					case 0x2f: 
					{
						if (length != 0) throw std::runtime_error("Expected length for endOfTrack event is 0");
						EndOfTrackEvent* event = new EndOfTrackEvent();
						return event;
					}
					case 0x51: 
					{
						if (length != 3) throw std::runtime_error("Expected length for setTempo event is 3");
						SetTempoEvent* event = new SetTempoEvent();
						event->microsecondsPerBeat = readInt24(dataStart);
						return event;
					}
					case 0x54: 
					{
						if (length != 5) throw std::runtime_error("Expected length for smpteOffset event is 5");
						SmpteOffsetEvent* event = new SmpteOffsetEvent();
						uint8_t hourByte = *dataStart++;
						switch (hourByte & 0x60) 
						{
							case 0x00: event->framerate = 24; break;
							case 0x20: event->framerate = 25; break;
							case 0x40: event->framerate = 29; break;
							case 0x60: event->framerate = 30; break;
						}
						event->hour = hourByte & 0x1f;
						event->min = int(*dataStart++);
						event->sec = int(*dataStart++);
						event->frame = int(*dataStart++);
						event->subframe = int(*dataStart++);
						return event;
					}
					case 0x58: 
					{
						if (length != 4) throw std::runtime_error("Expected length for timeSignature event is 4");
						TimeSignatureEvent* event = new TimeSignatureEvent();
						event->numerator = int(*dataStart++);
						event->denominator = int(powf(2.0f, float(*dataStart++)));
						event->metronome = int(*dataStart++);
						event->thirtyseconds = int(*dataStart++);
						return event;
					}
					case 0x59: 
					{
						if (length != 2) throw std::runtime_error("Expected length for keySignature event is 2");
						KeySignatureEvent* event = new KeySignatureEvent();
						event->key = int(*dataStart++);
						event->scale = int(*dataStart++);
						return event;
					}
					case 0x7f: 
					{
						SequencerSpecificEvent* event = new SequencerSpecificEvent();
						event->data = new uint8_t[length];
						memcpy(event->data, dataStart, length);
						dataStart += length;
						return event;
					}
				}

				UnknownEvent* event = new UnknownEvent();
				event->data = new uint8_t[length];
				memcpy(event->data, dataStart, length);
				dataStart += length;
				return event;
			}

			else if (eventTypeByte == 0xf0) 
			{
				int length = readVarInt(dataStart);
				SysExEvent* event = new SysExEvent();
				event->data = new uint8_t[length];
				memcpy(event->data, dataStart, length);
				dataStart += length;
				return event;
			}

			else if (eventTypeByte == 0xf7) 
			{
				int length = readVarInt(dataStart);
				DividedSysExEvent* event = new DividedSysExEvent();
				event->data = new uint8_t[length];
				memcpy(event->data, dataStart, length);
				dataStart += length;
				return event;
			}
			else 
			{
				throw std::runtime_error("Unrecognised MIDI event type byte"); // eventTypeByte;
			}
		}
		else 
		{
			// channel event
			ChannelEvent * event = new ChannelEvent();
			int param1;

			// Running status... 
			if ((eventTypeByte & 0x80) == 0) 
			{
				// running status - reuse lastEventTypeByte as the event type.
				// eventTypeByte is actually the first parameter
				param1 = eventTypeByte;
				eventTypeByte = lastEventTypeByte;
			}

			else 
			{
				param1 = int(*dataStart++);
				lastEventTypeByte = eventTypeByte;
			}

			event->midiCommand = eventTypeByte;
			event->param1 = param1;
			event->param2 = 0xff;  // don't transmit this value (@tofix ??)

			switch (eventTypeByte & 0xf0)
			{
				case 0x80: // note off
				event->param2 = int(*dataStart++);
				return event;
				case 0x90: // note on
					event->param2 = int(*dataStart++); // velocity
					return event;
				case 0xa0: // after touch
					event->param2 = int(*dataStart++); // amount
					return event;
				case 0xb0: // controller
					event->param2 = int(*dataStart++); // amount
					return event;
				case 0xc0: // program change
				return event;
				case 0xd0: // channel after touch
				return event;
				case 0xe0: // pitch bend
				event->param2 = int(*dataStart++);
				return event;
				default:
				throw std::runtime_error("Unrecognised MIDI event type");
			}
		}
		throw std::runtime_error("Unparsed event");
	}

	MidiSequence::MidiSequence() : tracks(0), ticksPerBeat(240), startingTempo(120)
	{
		
	}

	MidiSequence::~MidiSequence()
	{
		clearTracks();
	}
	
	void MidiSequence::clearTracks()
	{
		tracks.clear();
	}
	
	void MidiSequence::parseInternal(const std::vector<uint8_t> & buffer)
	{
		const uint8_t * dataPtr = buffer.data();
		
		int headerId = readInt32(dataPtr);
		int headerLength = readInt32(dataPtr);

		if (headerId != 'MThd' || headerLength != 6) 
		{
			std::cerr << "Bad .mid file - couldn't parse header" << std::endl;
			return;
		}
		
		readInt16(dataPtr); // format type... 
		int trackCount = readInt16(dataPtr);
		int timeDivision = readInt16(dataPtr);
		
		// CBB: deal with the SMPTE style time coding
		// timeDivision is described here http://www.sonicspot.com/guide/midifiles.html
		if (timeDivision & 0x8000) 
		{
			std::cerr << "Found SMPTE time frames" << std::endl;
			//int fps = (timeDivision >> 16) & 0x7f;
			//int ticksPerFrame = timeDivision & 0xff;
			// given beats per second, timeDivision should be derivable.
			return;
		}
		
		startingTempo = 120.0f; // midi default 
		ticksPerBeat = float(timeDivision); // ticks per beat (a beat is defined as a quarter note)
		
		for (int i = 0; i < trackCount; ++i) 
		{
			MidiTrack track;

			headerId = readInt32(dataPtr);
			headerLength = readInt32(dataPtr);

			if (headerId != 'MTrk') 
			{
				std::cerr << "Bad .mid file - couldn't find track" << std::endl;
				return;
			}

			uint8_t const * dataEnd = dataPtr + headerLength;

			uint8_t runningEvent = 0;

			while (dataPtr < dataEnd) 
			{
				int duration = readVarInt(dataPtr);

				auto ev = std::shared_ptr<MidiTrackEvent>(parseEvent(dataPtr, runningEvent));

				ev->deltatime = duration;

				ChannelEvent * ce = dynamic_cast<ChannelEvent*>(ev.get());
				if (ce)
				{
					runningEvent = ce->midiCommand;
				}
				
				// Double check
				track.events.push_back(ev);
			}

			tracks.push_back(track);
		}

	}

	// In ticks
	double MidiSequence::getEndTime()
	{
		double totalLength = 0;
		for (const auto t : tracks)
		{
			double localLength = 0;
			auto events = t.events;
			for (const auto e : events)
			{
				localLength += e->deltatime;
			}

			if (localLength > totalLength) 
			{
				totalLength = localLength;
			}
		}
		return totalLength;
	}

	void MidiSequence::parse(const std::vector<uint8_t> & buffer)
	{
		clearTracks();
		parseInternal(buffer);
	}

} // mm
