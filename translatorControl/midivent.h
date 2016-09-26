#ifndef MIDIVENT_H
#define MIDIVENT_H
#include <stdio.h>
#include "logging.h"

typedef enum midiventTypeName { None, NoteOn, NoteOff } MidiventTypeName;
typedef unsigned char septet;

class Midivent
{
public:
    static septet septetFilter(unsigned char byte)  { return(byte & (unsigned char)(0b01111111)); }
    Midivent();
    Midivent(MidiventTypeName theType, septet data1 = 0, septet data2 = 0);
    ~Midivent();
    MidiventTypeName getType(void) const;
    septet getNote(void) const              { return(noteId); }
    septet getVelocity(void) const          { return(velocity); }
    void printObject(void) const;
private:
    MidiventTypeName    typeName;
    unsigned char       noteId;
    unsigned char       velocity;
};


#endif // MIDIVENT_H
