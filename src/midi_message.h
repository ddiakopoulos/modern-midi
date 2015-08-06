#pragma comment(user, "license")

#pragma once

#ifndef MODERNMIDI_MESSAGE_H
#define MODERNMIDI_MESSAGE_H

#include "modernmidi.h"
#include <stdint.h>
#include <vector>
#include <memory>
#include <assert.h>

namespace mm
{
    
    /////////////////////////////
    // (Channel) Message Type  //
    /////////////////////////////
    
    enum class MessageType : uint8_t
    {
		INVALID             = 0x0,
        // Standard Message
        NOTE_OFF           = 0x80,
        NOTE_ON            = 0x90,
        POLY_PRESSURE      = 0xA0,
        CONTROL_CHANGE     = 0xB0,
        PROGRAM_CHANGE     = 0xC0,
        AFTERTOUCH		   = 0xD0,
        PITCH_BEND         = 0xE0,
        
        // System Common Messages
        SYSTEM_EXCLUSIVE   = 0xF0,
        TIME_CODE          = 0xF1,
        SONG_POS_POINTER   = 0xF2,
        SONG_SELECT        = 0xF3,
        RESERVED1          = 0xF4,
        RESERVED2          = 0xF5,
        TUNE_REQUEST       = 0xF6,
        EOX                = 0xF7,
        
        // System Realtime Messages
        TIME_CLOCK         = 0xF8,
        RESERVED3          = 0xF9,
        START              = 0xFA,
        CONTINUE           = 0xFB,
        STOP               = 0xFC,
        RESERVED4          = 0xFD,
        ACTIVE_SENSING     = 0xFE,
        SYSTEM_RESET       = 0xFF
    };
    
    //////////////////////////
    // (Meta) Message Type  //
    //////////////////////////
    
    enum class MetaEventType : uint8_t
    {
        SEQUENCE_NUMBER 	= 0x00,
        TEXT 				= 0x01,
        COPYRIGHT 			= 0x02,
        TRACK_NAME 			= 0x03,
        INSTRUMENT 			= 0x04,
        LYRIC 				= 0x05,
        MARKER 				= 0x06,
        CUE 				= 0x07,
        PATCH_NAME 			= 0x08,
        DEVICE_NAME 		= 0x09,
        END_OF_TRACK 		= 0x2F,
        TEMPO_CHANGE 		= 0x51,
        SMPTE_OFFSET 		= 0x54,
        TIME_SIGNATURE 		= 0x58,
        KEY_SIGNATURE 		= 0x59,
        PROPRIETARY 		= 0x7F,
        UNKNOWN 			= 0xFF
    };
    
	inline uint32_t make_variable_length(std::vector<uint8_t> & buffer, uint64_t number) 
	{
		uint64_t value = number;

	   if (value >= (1 << 28)) 
		   throw std::runtime_error("meta too large");

	   buffer[0] = (value >> 21) & 0x7F;
	   buffer[1] = (value >> 14) & 0x7F;
	   buffer[2] = (value >>  7) & 0x7F;
	   buffer[3] = (value >>  0) & 0x7F;

	   int flag = 0;
	   int length = -1;

	   for (int i = 0; i < 3; i++) 
	   {
		  if (buffer[i] != 0)
			 flag = 1;

		  if (flag) 
			 buffer[i] |= 0x80;

		  if (length == -1 && buffer[i] >= 0x80) 
			 length = 4-i;
	   }

	   if (length == -1) 
		  length = 1;

	   if (length < 4) 
		  for (int i = 0; i < length; i++) 
			 buffer[i] = buffer[4 - length + i];

	   return length;
	}

	//////////////////
    // MidiMessage  //
    //////////////////

    // Channels are indexed @ 1 to 16 (not 0-15)
    struct MidiMessage
    {
        MidiMessage() { data = {0, 0, 0}; }
        MidiMessage(const uint8_t b1, const uint8_t b2, const uint8_t b3, const double ts = 0) : timestamp(ts) { data = {b1, b2, b3}; }
        MidiMessage(const uint8_t b1, const  uint8_t b2, const double ts = 0) : timestamp(ts) { data = {b1, b2}; }
        MidiMessage(const MidiMessage & rhs) { *this = rhs; }
        
        MidiMessage & operator = (const MidiMessage & rhs)
        {
            data = rhs.data;
            timestamp = rhs.timestamp;
            return *this;
        }
        
        bool usesChannel(const int channel) const
        {
            assert(channel > 0 && channel <= 16);
            return ((data[0] & 0xF) == channel - 1) && ((data[0] & 0xF0) != 0xF0);
        }
        
        int getChannel() const
        {
            if ((data[0] & 0xF0) != 0xF0)
                return (data[0] & 0xF) + 1;
            return 0;
        }
        
        unsigned char operator [] (size_t i) {  return data.at(i); }
        
        unsigned char operator [] (size_t i) const { return data.at(i); }
        
        bool isMetaEvent() const { return data[0] == 0xFF; }
        
        uint8_t getMetaEventSubtype() const
        {
            if (!isMetaEvent()) return -1;
            return data[1];
        }
        
        uint8_t getMessageType() const
        {
            if (data[0] >= uint8_t(MessageType::SYSTEM_EXCLUSIVE)) { return data[0] & 0xFF; }
            else { return data[0] & 0xF0; }
        }
        
        bool isNoteOnOrOff() const
        {
            const auto status = getMessageType();
            return (status == (uint8_t) MessageType::NOTE_ON) || (status == (uint8_t) MessageType::NOTE_OFF);
        }
        
        size_t messageSize() const { return data.size(); }
        
        double timestamp = 0;

        std::vector<unsigned char> data;
    };
    
    ///////////////////////
    // Message Factories //
    ///////////////////////
    
	// Channel Events

    inline uint8_t MakeCommand(const MessageType type, const int channel)
    {
        return (uint8_t) ((uint8_t) type | mm::clamp<uint8_t> (channel, 0, channel - 1));
    }
    
    inline MidiMessage MakeNoteOn(uint8_t channel, uint8_t note, uint8_t velocity)
    {
        return MidiMessage(MakeCommand(MessageType::NOTE_ON, channel), note, velocity);
    }
    
    inline MidiMessage MakeNoteOff(uint8_t channel, uint8_t note, uint8_t velocity)
    {
        return MidiMessage(MakeCommand(MessageType::NOTE_OFF, channel), note, velocity);
    }
    
    inline MidiMessage MakeControlChange(uint8_t channel, uint8_t control, uint8_t value)
    {
        return MidiMessage(MakeCommand(MessageType::CONTROL_CHANGE, channel), control, value);
    }
    
    inline MidiMessage MakeProgramChange(uint8_t channel, uint8_t value)
    {
        return MidiMessage(MakeCommand(MessageType::PROGRAM_CHANGE, channel), value);
    }
    
    inline MidiMessage MakePitchBend(uint8_t channel, int value)
    {
        return MidiMessage(MakeCommand(MessageType::PITCH_BEND, channel), value & 0x7F, (uint8_t)((value >> 7) & 0x7F));
    }
    
    inline MidiMessage MakePitchBend(uint8_t channel, uint8_t lsb, uint8_t msb)
    {
        return MidiMessage(MakeCommand(MessageType::PITCH_BEND, channel), lsb, msb);
    }
    
    inline MidiMessage MakePolyPressure(uint8_t channel, uint8_t note, uint8_t value)
    {
        return MidiMessage(MakeCommand(MessageType::POLY_PRESSURE, channel), note, value);
    }
    
    inline MidiMessage MakeAftertouch(uint8_t channel, uint8_t value)
    {
        return MidiMessage(MakeCommand(MessageType::AFTERTOUCH, channel), value);
    }

	// Meta Events

	inline MidiMessage MakeEndOfTrackMetaEvent()
    {
        return MidiMessage(0xFF, 0x2F, 0, 0.0);
    }

	inline MidiMessage MakeTextMetaEvent(MetaEventType textType, std::string text)
	{
		std::vector<uint8_t> message;

		std::vector<uint8_t> size = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};

		auto length = text.size();
		auto varLength =  make_variable_length(size, length);

		message.resize(2 + varLength + length);
		message[0] = 0xFF;	// set meta field
		message[1] = (uint8_t) textType & 0x7F; // set meta subtype

		for (int i = 0; i < varLength; i++)
			message[2 + i] = size[i]; // set meta length

		for (int i = 0; i < length; i++) 
			message[2 + varLength + i] = text.data()[i]; // set data

		MidiMessage m;
		m.data.resize(message.size());
		memcpy(m.data.data(), message.data(), message.size());
		return m;
	}

    ///////////////
    // Utilities //
    ///////////////
    
    inline std::string StringFromMessageType(MessageType status)
    {
        switch(status)
        {
            case MessageType::NOTE_OFF: return "Note Off";
            case MessageType::NOTE_ON: return "Note On";
            case MessageType::CONTROL_CHANGE: return "Control Change";
            case MessageType::PROGRAM_CHANGE: return "Program Change";
            case MessageType::PITCH_BEND: return "Pitch Bend";
            case MessageType::POLY_PRESSURE: return "Poly Aftertouch";
            case MessageType::AFTERTOUCH: return "Aftertouch";
            case MessageType::SYSTEM_EXCLUSIVE: return "Sysex";
            case MessageType::TIME_CODE: return "Time Code";
            case MessageType::SONG_POS_POINTER: return "Song Pos";
            case MessageType::SONG_SELECT: return "Song Select";
            case MessageType::TUNE_REQUEST: return "Tune Request";
            case MessageType::EOX: return "Sysex End";
            case MessageType::TIME_CLOCK: return "Time Clock";
            case MessageType::START: return "Start";
            case MessageType::CONTINUE: return "Continue";
            case MessageType::STOP: return "Stop";
            case MessageType::ACTIVE_SENSING: return "Active Sensing";
            case MessageType::SYSTEM_RESET: return "System Reset";
            default: return "Unknown";
        }
    }
    
} // mm

#endif