#ifndef MIDISOURCE_H
#define MIDISOURCE_H
#include <stdio.h>
#include "logging.h"
#include <vector>
#include "midivent.h"
#include "midiventConsumer.h"
#include <Qt>
#include <QWidget>
#include <QGroupBox>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QLineEdit>
#include <QLabel>
#include <QComboBox>
#include <QMessageBox>
#include <QTextStream>
#include <string>

// Debug
#include "midiGraphicTranslator.h"



const int nbMaxConsumers = 32;

/*********************************************************************************************************************/
//
// MidiSource: transform Midi message to Midivent and send them to registered consumers
//
/*********************************************************************************************************************/

class MidiSource : public QGroupBox
{
        Q_OBJECT

public:
    MidiSource(QWidget *parent = Q_NULLPTR, QStringList *availableMidiPortNames = 0);
    ~MidiSource();
    int getRtMidiInPortIndex() const;               // accessor
    int getNbConsumers(void) const;                 // accessor
    int addConsumer(MidiventConsumer *newConsumer);     // to add a generated Midivent listener
    int removeConsumer(MidiventConsumer *oldCustomer);  // to remove a generated Midivent listener
    MidiventConsumer *getConsumer(int i) const;
    void receiveMidiMessage(std::vector<unsigned char> *message); // Midi Message enter point
public slots:
    void updateAvailableMidiPortList(QStringList *midiPortNames);   //request update of MidiPort list
signals:
    void myMidiPortHasChanged(int,int);             // signal to gui to change listening Midi Port
private slots:
    void changingMidiPort(int index);               // action of MidiPort combo box
    void changingMidiChannel(int index);            // action of MidiChannel combo box
private:
    static int nbCreated;                           // nb of already created translators
    int instanceId;                                 // id of this MidiSource
    int nbMidiPortNoteOn;                           // current Midi Port activity
    int nbMidiSourceNoteOn;                         // current MidiQSource activity after filtering
    int midiPortId;                                 // index of selected input Midi port
    bool allChannels;                               // Flag listening to all channels
    int midiChannelId;                              // channel selected
    int nbConsumers;                                // number of generated Midivent listeners
    MidiventConsumer *consumers[nbMaxConsumers];    // list of generated Midivent listeners
                                                // Gui part
    QLineEdit *MidiSourceName;                      // header : MidiSource name
    QLabel *MidiPortLabel;                          // MidiPort : gui part label
    QComboBox *MidiPorts_ComboBox;                  // MidiPort : selection combo box
    QHBoxLayout *MidiPortsLayout;                   // MidiPort : horizontal layout
    QLabel *MidiChannelLabel;                       // MidiChannel : gui part label
    QComboBox *MidiChannels_ComboBox;               // MidiChannel : selection combo box
    QHBoxLayout *MidiChannelsLayout;                // MidiChannel : horizontal layout
    QVBoxLayout *MidiSourceGroupLayout;             // global MidiSource vertical layout

};

#endif // MIDISOURCE_H
