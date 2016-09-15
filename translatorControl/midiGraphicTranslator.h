#ifndef MIDIGRAPHICTRANSLATOR_H
#define MIDIGRAPHICTRANSLATOR_H
#include <stdio.h>
#include "Midivent.h"
#include "MidiConsumer.h"
#include <QWidget>
#include <QGroupBox>
#include <QHBoxLayout>
#include <QLineEdit>
#include <QPushButton>
#include <QIcon>

#include <QVBoxLayout>
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

const int undefinedMidiSource = -1;

const int smallButtonHeight = 26;
const int smallButtonWidth = 26;

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
