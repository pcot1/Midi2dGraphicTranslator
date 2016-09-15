#ifndef MIDISOURCE_H
#define MIDISOURCE_H
#include <stdio.h>
#include "RtMidi.h"
#include "Midivent.h"
#include "MidiConsumer.h"

#include <QWidget>
#include <QApplication>
#include <QWidget>
#include <QGroupBox>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QLineEdit>
#include <QLabel>
#include <QComboBox>
#include <QMessageBox>
#include <QString>
#include <QStringList>
#include <QTextStream>
#include <string>
#include <vector>

#ifdef DEBUG
#define PRINTF(args)    printf args
#else
#define PRINTF(args)
#endif

const int nbMaxConsumers = 32;

class MidiSource : public QGroupBox
{
public:
    MidiSource(QWidget *parent = Q_NULLPTR);
    ~MidiSource();
    int getNbConsumers(void);
    int addConsumer(MidiConsumer *newConsumer);
    int removeConsumer(MidiConsumer *oldCustomer);
private:
    static int nbCreated;
    int instanceId;
    unsigned int nbMidiPortNoteOn;
    unsigned int nbMidiSourceNoteOn;
    bool allChannels;
    int channelId;
    QLineEdit *MidiSourceName;
    QLabel *MidiPortLabel;
    QComboBox *MidiPorts_ComboBox;
    QLabel *MidiChannelLabel;
    QComboBox *MidiChannels_ComboBox;

    QStringList *scanMidiInputPorts(void);
    void rtMidiCallBack(double deltatime, std::vector<unsigned char> *message, void *userData);

    int nbConsumers;
    MidiConsumer *consumers[nbMaxConsumers];
};

#endif // MIDISOURCE_H
