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

void MidiPortConsumers::removeMidiPortConsumer(MidiSource * consumer)
{
    qCDebug(Minit,"MidiPortConsumers::removeMidiPortConsumer %p\n",consumer);
    int targetConsumerId = -1;
    for (int i = 0; i < nbConsumers; i++)
        if (consumers[i] == consumer)
            targetConsumerId = i;
    if (targetConsumerId < 0)   {
        qCDebug(Minit,"MidiPortConsumer (MidiSource) %p not found in this MidiPortsConsumers %p\n",consumer,this);
        return;
    }
    for (int i = targetConsumerId; i < (nbConsumers-1); i++)
        consumers[i] = consumers[i+1];
    --nbConsumers;
    consumers[nbConsumers] = 0;
    return;
}

void MidiPortConsumers::addMidiPortConsumer(MidiSource * consumer)
{
    qCDebug(Minit,"add a MidiPortConsumer %p = MidiSource %p",this,consumer);
    consumers[nbConsumers] = consumer;
    ++nbConsumers;
    printObject();
}
