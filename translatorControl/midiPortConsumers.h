#ifndef MIDIPORTCONSUMERS_H
#define MIDIPORTCONSUMERS_H
#include <stdio.h>
#include "midiSource.h"

#ifdef DEBUG
#define PRINTF(args)    printf args
#else
#define PRINTF(args)
#endif

const int nbMaxMidiPortConsumers = 16;

/*********************************************************************************************************************/
//
// List of 1 Midi port consumers
//
/*********************************************************************************************************************/

class MidiPortConsumers
{
public:
    MidiPortConsumers();
    ~MidiPortConsumers();
    int getNbMidiPortConsumers(void);                     // accessor
    MidiSource *getMidiPortConsumer(int index);           // accessor
    void removeAllMidiPortConsumers(void);                // empty list
    void addConsumer(MidiSource * consumer);                // add a Midi port consumer to the list
    void printObject(void) const;                           // debug
private :
    int nbMidiPortConsumers;                              // nb of the Midi port consumers in the list
    MidiSource *midiPortConsumers[nbMaxMidiPortConsumers];  // list of the Midi Port consumer
};

#endif // MIDIPORTCONSUMERS_H
