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
void MidiPortConsumers::printObject() const
{
     //if (theLoggingCategory == 0)
     //       theLoggingCategory = Minit;
     qCDebug(Minit,"MidiPortConsumers %p has %1d consumers",this,nbConsumers);
     for (int i = 0; i < nbConsumers; i++)
        qCDebug(Minit,"Midi Port Consumer (MidiSource) %1d is %p\n",i,consumers[i]);

}


int MidiPortConsumers::getNbMidiPortConsumers(void)
{
    return(nbConsumers);
}

MidiSource *MidiPortConsumers::getMidiPortConsumer(int id)
{
    return(consumers[id]);
}



void MidiPortConsumers::removeAllMidiPortConsumers(void)
{
    nbConsumers = 0;
    for (int i = 0; i < nbMaxMidiPortConsumers; i++)
        consumers[i] = 0;
}

void MidiPortConsumers::addConsumer(MidiSource * consumer)
{
    qCDebug(Minit,"addConsumer %p with MidiSource %p",this,consumer);
    consumers[nbConsumers] = consumer;
    ++nbConsumers;
    printObject();
}
