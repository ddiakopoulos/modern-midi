struct MidiMessage 

- Copy
- Move

- Time stamp (get/set/add/remove)

int            getTempoMicro        (void);
int            getTempoMicroseconds (void);
double         getTempoSeconds      (void);
double         getTempoBPM          (void);
double         getTempoTPS          (int tpq);
double         getTempoSPT          (int tpq);

int            isMetaMessage        (void);
int            isMeta               (void);
int            isNoteOff            (void);
int            isNoteOn             (void);
int            isNote               (void);
int            isAftertouch         (void);
int            isController         (void);
int            isTimbre             (void);
int            isPatchChange        (void);
int            isPressure           (void);
int            isPitchbend          (void);

int            getCommandNibble     (void);
int            getCommandByte       (void);
int            getChannelNibble     (void);
int            getChannel           (void);

isForChannel
setChannel

isSysEx
getSysEx
getSysExSize

MakeNoteOn
MakeNoteOff
MakeControlChange
MakePitchBend
MakeSysEx
MakeProgramChange
MakeControlChange
MakeAftertouch
MakePolyPressure
MakeQuarterFrame
MakeSongPosition
MakeSongSelect
MakeTuneRequest

isAllNotesOff
isAllSoundOff

 getMetaEventType, isTrackMetaEvent, isEndOfTrackMetaEvent,
isTextMetaEvent, isTrackNameEvent, isTempoMetaEvent, isTimeSignatureMetaEvent,
isKeySignatureMetaEvent, isMidiChannelMetaEvent
isTrackMetaEvent
isEndOFTrackMetaEvent



