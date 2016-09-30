#ifndef MIDIGRAPHICTRANSLATOR_H
#define MIDIGRAPHICTRANSLATOR_H
#include <stdio.h>
#include <stdlib.h>
#include "midivent.h"
#include "midiventConsumer.h"
#include "world.h"
#include <Qt>
#include <QWidget>
#include <QGroupBox>
#include <QComboBox>
#include <QLayout>
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


/*********************************************************************************************************************/
//
// Base Class of Midivent transformer to graphics
//
/*********************************************************************************************************************/

class MidiGraphicTranslator : public QGroupBox, public MidiventConsumer
{
    Q_OBJECT

public:
    MidiGraphicTranslator(QWidget *parent = Q_NULLPTR);
    virtual ~MidiGraphicTranslator();
                                                    // trivial accessor "get"
    int getInstanceId(void) const                                   { return(instanceId); }
    int getDId(void) const                                          { return(instanceId+1); }
    QString getTranslatorName(void) const                           { return(MidiGraphicTranslatorName->text()); }
    QGraphicsItemGroup *getTranslatorGraphicLayer(void) const       { return(graphicLayer); }
    septet getAverageReceivedNoteOn() const                         { return(averageReceivedNoteOn); }
    QString getNoteName(septet note) const;         // get the name of the given note
    int getOctaveIdOfNote(septet note) const                        { return((note/12)-2); }         // get the octave of the given note
    int getNoteIdInOctave(septet note) const                        { return(note%12); }             // get note id within its Octave (C = 0)
    void setTranslatorName(QString name);           // accessor "set"

    void doUpgradeNumberOfMidiSources(int nbMS);    // recieving an outside request MidiSource list update
    virtual void cleanMidiventContext(void);        // remove every results of previously received Midivent
    void cleanGraphicLayer(void);                   // remove graphics items from the scene
    //void registerMidiSource(void (*pf)(Midivent&), int msId);
    virtual void printObject(void) const;           // debug
    virtual void receiveMidivent(Midivent *pevt);   // trigger translator action
public slots:
    void terminate(void);                           // action of quit button
    void requireWidgetMvtFastReverse(void);         // action of rendering order button |<
    void requireWidgetMvtReverse(void);             // action of rendering order button <
    void requireWidgetMvtForward(void);             // action of rendering order button >
    void requireWidgetMvtFastForward(void);         // action of rendering order button >|
    void changingMidiSource(int msId);
signals:
    void suicide(int iId);                          // signal to gui to delete this translator
    void widgetMvtRequired(int iId, int mvt);       // signal to gui to change rendering order
    void registerMidiSource(int);                   // signal to gui to select MidiSource
    void unRegisterMidiSource(int);                 // signal to gui to deselect MidiSource
protected:
    void addLayout(QLayout *layout);                // add an applicative layout
    QGraphicsItemGroup *graphicLayer;               // the translator part of the scene
    QPointF generateRandomWorldCoordinates(void) const;     // utils
    qreal generateRandomRealNormalizedValue(void) const;    // utils
    void updateListOfMidiSourcesInComboBox(int nbMS);       // private to update MidiSource ComboBox
    void *getNoteThing(septet note) const;         // get the thing of the given note
    void setNoteThing(septet note, void *thing);   // set the note thing of the given note
    virtual void newNoteOnAnalysis(septet note, septet velocity);
    virtual void processNoteOn(septet note, septet velocity);
    virtual void newNoteOffAnalysis(septet note, septet velocity);
    virtual void processNoteOff(septet note, septet velocity);
private:
    static int nbCreated;                           // nb of already created translators
    int instanceId;                                 // id of this translator
    int MidiSourceId;                               // current MidiSource (None = -1)
    int nbNoteCurrentlyOn;                          // current Midi activity
    int nbRecievedNoteOn;                           // current Midi activity
    int sumReceivedNoteOn;
    septet averageReceivedNoteOn;
    void *noteThing[128];                           // pointer when note is pressed
                                                // Gui part
    QLineEdit *MidiGraphicTranslatorName;           // header : name
    QPushButton *quit_Button;                       // header : delete Translator
    QHBoxLayout *header_Layout;                     // header : horizontal layout
    QPushButton *FastReverse_Button;                // rendering order : |<
    QPushButton *Reverse_Button;                    // rendering order : <
    QPushButton *Forward_Button;                    // rendering order : >
    QPushButton *FastForward_Button;                // rendering order : >|
    QHBoxLayout *rendererOrder_Layout;              // rendering order : horizontal layout
    QLabel *MidiSource_Label;                       // MidiSource : gui part label
    QComboBox *MidiSources_ComboBox;                // MidiSource : selection
    QHBoxLayout *MidiSource_Layout;                 // MidiSource : horizontal layout
    QVBoxLayout *MidiGraphicTranslatorGroupLayout;  // MidiSource : global vertical layout
};

#endif // MIDIGRAPHICTRANSLATOR_H
