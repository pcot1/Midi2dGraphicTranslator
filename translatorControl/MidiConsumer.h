#ifndef MIDICONSUMER_H
#define MIDICONSUMER_H
#include <stdio.h>
#include "Midivent.h"

class MidiConsumer
{
public:
    virtual void recieveMidivent(Midivent &evt) = 0;
};

#endif // MIDICONSUMER_H
