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
                                                // No MidiSource selected
const int undefinedMidiSource = -1;
                                                // siez of small buttons (rendering order)
const int smallButtonHeight = 26;
const int smallButtonWidth = 26;

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
    int getInstanceId(void) const;                  // accessor
    QGraphicsItemGroup *getGraphics(void) const;    // accessor
    virtual void receiveMidivent(Midivent *pevt);   // trigger translator action
    void receiveNumberOfMidiSources(int nbMS);      // request MidiSource list update
    void printObject(void) const;                   // debug
    //void registerMidiSource(void (*pf)(Midivent&), int msId);
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
    QPointF generateRandomWorldCoordinates(void) const; // utils
    void updateListOfMidiSourcesInComboBox(int nbMS);   // private to update MidiSource ComboBox
private:
    static int nbCreated;                           // nb of already created translators
    int instanceId;                                 // id of this translator
    int MidiSourceId;                               // current MidiSource (None = -1)
    unsigned int nbMidiRecievedNoteOn;              // current Midi activity
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
                                                // Rendering part
    QGraphicsItemGroup *graphicLayer;
};

#endif // MIDIGRAPHICTRANSLATOR_H
