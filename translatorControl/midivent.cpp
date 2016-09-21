#define DEBUG
#include "midivent.h"

// *** trivial Constructor
Midivent::Midivent() : typeName(None), noteId(0)
{
}

// *** Constructor
Midivent::Midivent(MidiventTypeName theType, int theNote) : typeName(theType), noteId(theNote)
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

// *** accessor
int Midivent::getNote(void) const
{
    return (int)(noteId);
}
