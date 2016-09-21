#ifndef MIDIPORTCONSUMERS_H
#define MIDIPORTCONSUMERS_H
#include <stdio.h>
#include "logging.h"
#include "midiSource.h"

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
    int getNbMidiPortConsumers(void);                       // accessor
    MidiSource *getMidiPortConsumer(int index);             // accessor
    void removeAllMidiPortConsumers(void);                  // empty list
    void addConsumer(MidiSource * consumer);                // add a Midi port consumer to the list
    void printObject(void) const;                           // debug
private :
    int nbConsumers;                                        // nb of the Midi port consumers in the list
    MidiSource *consumers[nbMaxMidiPortConsumers];          // list of the Midi Port consumers
};

#endif // MIDIPORTCONSUMERS_H
