#ifndef MIDIVENTCONSUMER_H
#define MIDIVENTCONSUMER_H
#include <stdio.h>
#include "logging.h"
#include "midivent.h"

class MidiventConsumer
{
public:
    MidiventConsumer(void);
    virtual ~MidiventConsumer(void);
    virtual void receiveMidivent(Midivent *pevt) = 0;
    virtual void printObject(void) const;
};

#endif // MIDIVENTCONSUMER_H
