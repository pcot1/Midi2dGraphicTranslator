#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <QApplication>
#include "rtMidiPorts.h"
#include "gui.h"

int main(int argc, char *argv[])
{
    setvbuf(stdout, (char*)NULL, _IONBF, 0);            // just for debug printf to console
    srand (time(NULL));                                 // init (seed) random generator
    QApplication app(argc, argv);
                                                        // a single object to manage Midi ports via RtMidi lib
    RtMidiPorts *pCentralRtMidiInPortsManager = new RtMidiPorts();
    Gui guiWindow(pCentralRtMidiInPortsManager);        // start the Gui (and the view)
    return app.exec();
}


