#define DEBUG
#include "midivent.h"

/*
// *** static septet filter (remove Most Significat Bit)
septet Midivent::septetFilter(unsigned char byte)  {
    return(byte & (unsigned char)(0b01111111))
}
*/

// *** trivial Constructor
Midivent::Midivent() : typeName(None), noteId(0), velocity(0)
{
}

// *** Constructor
Midivent::Midivent(MidiventTypeName theType, septet data1, septet data2) :
    typeName(theType), noteId(septetFilter(data1)),velocity(septetFilter(data2))
{
}

// *** Destructor
Midivent::~Midivent()
{
}

// *** Debug
void Midivent::printObject(void) const
{
    char bla[16];
    switch (typeName)  {
        case None:
            sprintf(bla,"None"); break;
        case NoteOn:
            sprintf(bla,"NoteOn"); break;
        case NoteOff:
            sprintf(bla,"NoteOff"); break;
            break;
        default:
           sprintf(bla,"Unknown");
    }
    qCDebug(Mvent,"Midivent = (%s,%d)\n",bla,noteId);
}

// *** accessor
MidiventTypeName Midivent::getType(void) const
{
    return typeName;
}

/*
// *** accessor
int Midivent::getNote(void) const
{
    return (noteId);
}

// *** accessor
int Midivent::getVelocity(void) const
{
    return (velocity);
}
*/
