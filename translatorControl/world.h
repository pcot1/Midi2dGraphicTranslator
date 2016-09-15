#ifndef MIDIGRAPHICTRANSLATOR_H
#define MIDIGRAPHICTRANSLATOR_H
#include <stdio.h>
#include "Midivent.h"
#include "MidiConsumer.h"
#include <Qt>
#include <QWidget>
#include <QGroupBox>
#include <QComboBox>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QLineEdit>
#include <QPushButton>
#include <QLabel>
#include <QGraphicsItem>
#include <QGraphicsItemGroup>
#include <QGraphicsTextItem>

//#include <QMessageBox>
#include <string>
#include <vector>

#ifdef DEBUG
#define PRINTF(args)    printf args
#else
#define PRINTF(args)
#endif

const int undefinedMidiSource = -1;

const int smallButtonHeight = 26;
const int smallButtonWidth = 26;

const float worldUpLeftX = -4.0;
const float worldUpLeftY = -3.0;
const float worldWidth = 8.0;
const float worldHeight = 6.0;

class MidiGraphicTranslator : public QGroupBox, public MidiConsumer
{
    Q_OBJECT

public:
    MidiGraphicTranslator(QWidget *parent = Q_NULLPTR);
    ~MidiGraphicTranslator();
    int getInstanceId(void) const;
    void recieveMidivent(Midivent &evt);
    void recieveNumberOfMidiSources(int nbMS);

    //void registerMidiSource(void (*pf)(Midivent&), int msId);
public slots:
    void terminate(void);
    void requireWidgetMvtFastReverse(void);
    void requireWidgetMvtReverse(void);
    void requireWidgetMvtForward(void);
    void requireWidgetMvtFastForward(void);
    void changingMidiSource(int msId);
signals:
    void suicide(int iId);
    void widgetMvtRequired(int iId, int mvt);
    void registerMidiSource(int);
    void unRegisterMidiSource(int);


private:
    static int nbCreated;
    int instanceId;
    unsigned int nbMidiRecievedNoteOn;
    QLineEdit *MidiGraphicTranslatorName;
    QPushButton *quit_Button;
    QPushButton *FastReverse_Button;
    QPushButton *Reverse_Button;
    QPushButton *Forward_Button;
    QPushButton *FastForward_Button;
    int MidiSourceId;
    QLabel *MidiSource_Label;
    QComboBox *MidiSources_ComboBox;
    void updateListOfMidiSourcesInComboBox(int nbMS);
};

#endif // MIDIGRAPHICTRANSLATOR_H
