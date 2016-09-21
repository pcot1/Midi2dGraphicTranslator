#ifndef RTMIDIPORTS_H
#define RTMIDIPORTS_H
#include <stdio.h>
#include "logging.h"
#include <vector>
#include <QStringList>
#include <QMessageBox>
#include "RtMidi.h"
#include "midiPortConsumers.h"
#include "midiSource.h"

const int nbMaxRtMidiPorts = 16;

typedef std::vector<unsigned char> MidiMessage;

/*********************************************************************************************************************/
//
// MidiPort centralizer: only 1 allowed "open" for a given Midi port inside a RtMidi application
//
/*********************************************************************************************************************/

class RtMidiPorts : public QLabel
{
    Q_OBJECT

public:
                                                    // the RtMidi Lib callback (function, not method)
    static void rtMidiCallBack(double deltatime, std::vector<unsigned char> *message, void *userData);
    static RtMidiPorts* theRtMidiPortsManager;      // last (usually uniic) rtMidiPorts created

    RtMidiPorts();                                    // constructor discovers existing Midi ports
    ~RtMidiPorts();
    int getNbRtMidiPorts(void);                       // accessor
    QStringList *getRtMidiPortsNames(void);           // accessor
    MidiPortConsumers *getConsumersOfMidiPortIndex(int id);   // accessor
    void printObject(void) const;                       // debug
    Q_INVOKABLE void receiveMidiMessage(MidiMessage message, int portIndex);
    //Q_INVOKABLE void receiveMidiMessage(void);

private:
                                                        // should be used before instrance intialized
    static int scanRtMidiPortsNames(QWidget *widget, RtMidiIn *pRtMidiPort, QStringList &existingMidiPortsNames);
    //static int scanRtMidiPortsNames(RtMidiIn *pRtMidiPort, QStringList &existingMidiPortsNames);
                                                    // RtMidi Ports Management
    int nbRtMidiPorts;                                // nb of Midi ports  discovered using RtMidi lib
    RtMidiIn *rtMidiPorts[nbMaxRtMidiPorts];        // list of Midi ports
    MidiPortConsumers *rtMidiPortConsumers[nbMaxRtMidiPorts];   // list of consumer of each Midi port
    QStringList rtMidiPortsNames;                     // Midi ports names
    void startUseOfRtMidiPorts(void);                 // open and set callback for discovered Midi ports
};

#endif // RTMIDIPORTS_H
