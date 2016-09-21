#ifndef MIDIVENT_H
#define MIDIVENT_H
#include <stdio.h>
#include "logging.h"

typedef enum midiventTypeName { None, NoteOn, NoteOff } MidiventTypeName;

class Midivent
{
public:
    Midivent();
    Midivent(MidiventTypeName theType, int theNote = 0);
    ~Midivent();
    MidiventTypeName getType(void) const;
    int getNote(void) const;
    void printObject(void) const;
private:
    MidiventTypeName    typeName;
    unsigned char       noteId;
};


#endif // MIDIVENT_H
