#pragma comment(user, "license")

#include "modernmidi.h"
#include "sequence_player.h"
#include "midi_message.h"
#include "midi_utils.h"
#include "timer.h"

using namespace mm;

MidiSequencePlayer::MidiSequencePlayer(MidiOutput & output) : shouldSequence(false), output(output)
{

}
	
MidiSequencePlayer::~MidiSequencePlayer()
{
	shouldSequence = false;

	if (sequencerThread.joinable())
		sequencerThread.join();
}


void MidiSequencePlayer::loadSingleTrack(const MidiTrack & track, double ticksPerBeat, double beatsPerMinute)
{
	reset();

    this->ticksPerBeat = ticksPerBeat;
    this->beatsPerMinute = beatsPerMinute;
	msPerTick = 60000.0 / beatsPerMinute / ticksPerBeat;

    double localElapsedTicks = 0;

    // Events in track
    for (auto m : track)
    {
        localElapsedTicks += m->tick;
        double deltaTimestampInSeconds = ticksToSeconds(localElapsedTicks);
        if (m->m->getMessageType() == (uint8_t) MessageType::NOTE_ON) addTimestampedEvent(0, deltaTimestampInSeconds, m); // already checks if non-meta message
    }

    //std::cout << "Track Idx: " << trackIdx << std::endl;
    //std::cout << "Local Elapsed Ticks " << localElapsedTicks << std::endl;
    
}

void MidiSequencePlayer::loadMultipleTracks(const std::vector<MidiTrack> & tracks, double ticksPerBeat, double beatsPerMinute)
{
    // Unimplemented
}

void MidiSequencePlayer::start()
{
	if (sequencerThread.joinable()) 
		sequencerThread.join();

	shouldSequence = true;
	sequencerThread = std::thread(&MidiSequencePlayer::run, this);

#if defined(MM_PLATFORM_WINDOWS)
	HANDLE threadHandle = sequencerThread.native_handle();
	auto err = SetThreadPriority(threadHandle, THREAD_PRIORITY_TIME_CRITICAL);
	if (err == 0)
	{
		std::cerr << "SetThreadPriority() failed: " << GetLastError() << std::endl;
	}

	err = SetThreadAffinityMask(threadHandle, 1);
	if (err == 0)
	{
		std::cerr<< "SetThreadAffinityMask() failed: " << GetLastError() << std::endl;
	}
#endif
    
	//sequencerThread.detach();

	if (startedEvent)
		startedEvent();
}
	
void MidiSequencePlayer::run()
{
	double lastTime = 0;
	size_t eventCursor = 0;

	PlatformTimer timer;
	timer.start();

	while (eventCursor < eventList.size())
	{
		auto outputMsg = eventList[eventCursor];
        
		//std::cout << "Delta: " << lastTime - outputMsg.timestamp << std::endl;

		while((timer.running_time_s()) <= (outputMsg.timestamp))
		{
			continue; // Spinny spin spin.
		}

		output.send(*outputMsg.msg);

		if (shouldSequence == false) 
			break;

		lastTime = outputMsg.timestamp;
		eventCursor++;
	}

	timer.stop(); 

	if (loop && shouldSequence == true) 
		run();
		
	if (stoppedEvent)
		stoppedEvent();
}

void MidiSequencePlayer::stop()
{
	shouldSequence = false;
}
	
void MidiSequencePlayer::addTimestampedEvent(int track, double now, std::shared_ptr<TrackEvent> ev)
{
	if (ev->m->isMetaEvent() == false)
	{
		eventList.push_back(MidiPlayerEvent(now, ev->m, track));
	}
}

float MidiSequencePlayer::length() const
{
	return playTimeSeconds;
}
