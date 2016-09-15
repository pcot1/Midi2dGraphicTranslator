#include "Midivent.h"

Midivent::Midivent() : typeName(None), noteId(0)
{
}

Midivent::Midivent(MidiventTypeName theType, int theNote) : typeName(theType), noteId(theNote)
{
}

Midivent::~Midivent()
{
}

MidiventTypeName Midivent::getType(void)
{
    return typeName;
}

int Midivent::getNote(void)
{
    return (int)(noteId);
}
