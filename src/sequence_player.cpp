#pragma comment(user, "license")

#include "modernmidi.h"
#include "sequence_player.h"
#include "midi_message.h"
#include "midi_utils.h"
#include "timer.h"

using namespace mm;

MidiSequencePlayer::MidiSequencePlayer() : shouldSequence(false)
{

}
	
MidiSequencePlayer::~MidiSequencePlayer()
{
	shouldSequence = false;

	if (sequencerThread.joinable())
		sequencerThread.join();
}

void MidiSequencePlayer::preprocessSequence()
{
	ticksPerBeat = internalSequence.ticksPerBeat > 0 ? internalSequence.ticksPerBeat : 100.0f;
	beatsPerMinute = internalSequence.startingTempo > 0 ? internalSequence.startingTempo : 120.0f;
	msPerTick = 60000.0 / beatsPerMinute /  ticksPerBeat;

	// Not used right now 
	double totalSequenceTicks = internalSequence.getEndTime();
	std::cout << "Total Sequence Ticks: " << totalSequenceTicks << std::endl;
	playTimeSeconds = float((totalSequenceTicks * msPerTick) / 1000.0);
	std::cout << "Play time in seconds " << playTimeSeconds << std::endl;

	//@todo: sanity check tracks

	// Look for tempo event, assume we only have one
	for (const auto track : internalSequence.tracks)
	{
		for (const auto mEvt : track.events)
		{
			if (mEvt->eventType == MIDI_EventSetTempo)
			{
				SetTempoEvent * ste = (SetTempoEvent *) mEvt.get();
				beatsPerMinute = float(60000000.0 / double (ste->microsecondsPerBeat));
				msPerTick = 60000.0 / beatsPerMinute /  ticksPerBeat;
			}
			else if (mEvt->eventType == MIDI_EventTimeSignature)
			{
				TimeSignatureEvent * tse = (TimeSignatureEvent *) mEvt.get();
				std::cout << "Numerator: " << tse->numerator; 
			}
		}
	}

	// Preprocess tracks into flat vector with timestamps
	int trackIdx = 0;
	for (const auto track : internalSequence.tracks)
	{
		double localElapsedTicks = 0;
		// Events in track
		for (const auto mEvt : track.events)
		{
			localElapsedTicks += mEvt->deltatime;
			double timestamp = ticksToSeconds(localElapsedTicks);
			addTimestampedEvent(eventList, trackIdx, timestamp, mEvt.get());
		}	
		trackIdx++;
	}

}

void MidiSequencePlayer::loadSequence(MidiSequence s)
{
	// Copy sequence locally
	internalSequence = s;
	preprocessSequence();
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
	sequencerThread.detach();

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

		// Spin until next loop 
		while ( (timer.running_time_ms()) <= (outputMsg.timestamp))
		{
			// Spinny spin spin.
		}

		if (shouldSequence == false) 
			break;

		eventQueue.push(outputMsg);

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
	
void MidiSequencePlayer::addTimestampedEvent(std::vector<MidiPlayerEvent> & list, int track, double now, MidiTrackEvent * ev)
{
	if (ev->eventType == MIDI_EventChannel)
	{
		ChannelEvent * ce = (ChannelEvent *) ev;
		list.push_back(MidiPlayerEvent(now, ce->midiCommand, ce->param1, ce->param2, track));
	}
}

float MidiSequencePlayer::length() const
{
	return playTimeSeconds;
}
