#define DEBUG
#include "midiPortConsumers.h"

// *** Constructor
MidiPortConsumers::MidiPortConsumers()
{
    removeAllMidiPortConsumers();
}

// *** Destructor
MidiPortConsumers::~MidiPortConsumers()
{
}

// *** Debug Print of what contains the Object
void MidiPortConsumers::printObject(void) const
{
     PRINTF((">>> MidiPortConsumers %p has %1d consumers\n",this,nbMidiPortConsumers));
     for (int i = 0; i < nbMidiPortConsumers; i++)
        PRINTF(("    Consumer %1d is 0x%p\n",i,midiPortConsumers[i]));
}


int MidiPortConsumers::getNbMidiPortConsumers(void)
{
    return(nbMidiPortConsumers);
}

MidiSource *MidiPortConsumers::getMidiPortConsumer(int id)
{
    return(midiPortConsumers[id]);
}



void MidiPortConsumers::removeAllMidiPortConsumers(void)
{
    nbMidiPortConsumers = 0;
    for (int i = 0; i < nbMaxMidiPortConsumers; i++)
        midiPortConsumers[i] = 0;
}

void MidiPortConsumers::addConsumer(MidiSource * consumer)
{
    PRINTF((">>> addConsumer 0x%p with MidiSource 0x%p\n",this,consumer));
    midiPortConsumers[nbMidiPortConsumers] = consumer;
    ++nbMidiPortConsumers;
    printObject();
}
