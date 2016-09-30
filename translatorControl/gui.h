#ifndef GUI_H
#define GUI_H
#include <stdio.h>
#include <Qt>
#include <QWidget>
#include <QDial>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QGridLayout>
#include <QMessageBox>
#include "rtMidiPorts.h"
#include "midiPortConsumers.h"
#include "midiSource.h"
#include "midiGraphicTranslator.h"
#include "one2OneTranslator.h"
#include "noteNameTranslator.h"
#include "polygonTranslator.h"
#include "graphicDisplayer.h"

const int nbMaxMidiSources = 16;
const int nbMaxMidiSourcesPerRow = 8;
const int nbMaxMidiGraphicTranslators = 16;
const int nbMaxMidiGraphicTranslatorsPerRow = 8;
const int widthLayoutActiveCell = 148;
const int widthLayoutControlCell = 90;

/*********************************************************************************************************************/
//
// Gui window: single instrance to control the whole application
//      a single centralized instance of RtMidi port manager should exist before launching the gui
//      gui creates the rendering window
//
/*********************************************************************************************************************/

class Gui : public QWidget
{
    Q_OBJECT

public:
    Gui(RtMidiPorts *theRtMidiInPortsManager);                // the Midi port manager shoud pre-exist
    ~Gui();
    int getNbRtMidiPorts(void);                                 // accessor
    int getNbMidiSources(void);                                 // accessor
    int getNbMidiGraphicTranslators(void);                      // accessor
    void printObject(void) const;                               // debug

    GraphicDisplayer *gDispl;                                   // the rendering machine

public slots:
    void removeWidgetsFromGuiLayout(void);                      // action required = remove all widgets from Gui layout
    void installWidgetsInGuiLayout(void);                       // action required = install all widgets in Gui layout
    void setNbMidiSources(int);                                 // action required = create/delete MidiSources
    void aMidiSourceHasChangedItsMidiPort(int oldIndex, int newIndex); // action required = update Midi ports listener lists
    void addMidiGraphicTranslator(void);                        // action required = create a new Graphic Translator
    void deleteMidiGraphicTranslator(int id);                   // action required = delete an existing Graphic Translator
    void moveMidiGraphicTranslators(int translatorInstanceId, int mvt); // action required = change rendering order of Graphic Translators
    void addMidiSourceConsumerRequest(int MidiSourceId);                  // action required = add a GraphicTranslator as consumer of a MidiSource
    void removeMidiSourceConsumerRequest(int MidiSourceId);               // action required = remove a GraphicTranslator as consumer of a MidiSource
private:
                                                            // Midi Ports Management
    RtMidiPorts *rtMidiInPortsManager;                        // RtMidi Ports Management (use of RtMidi lib)
    //QStringList *midiInPortNames;
                                                            // Midi Sources Ctrl
    int    nbMidiSources;                                       // nb of MidiSources
    MidiSource *MidiSources[nbMaxMidiSources];                  // list of MidiSources
    void propagateNbMidiSourcesUpdate(void);                    // request midiSource consumers update becuase nb of MidiSources has changed
                                                            // Midi Graphic Translators Ctrl
    int    nbMidiGraphicTranslators;                            // nb of MidiGraphic Translators
    MidiGraphicTranslator *MidiGraphicTranslators[nbMaxMidiGraphicTranslators]; // list of MidiGraphicTranslators
    int translatorInstanceIds[nbMaxMidiGraphicTranslators];     // list if instanceId of MidiGraphicTranslators
    int translatorRenderingOrder[nbMaxMidiGraphicTranslators];  // rendering order of MidiGraphicTranslators

                                                            // Gui own part
    QLabel *nbMidiSources_Label;                                // MidiSource Ctrl : name + nb
    QDial *nbMidiSources_Dial;                                  // MidiSource Ctrl : nb of MidiSources
    QVBoxLayout *MidiSourcesCtrl_Layout;                        // MidiSource Ctrl : vertical layout
    QLabel *nbMidiGraphicTranslators_Label;                     // GraphicTranslator Ctrl : name + nb
    QComboBox *midiGraphicTranslatorType_ComboBox;              // GrapgicTranslator Ctrl : type of next translator
    QPushButton *addMidiGraphicTranslators_Button;              // GraphicTranslator Ctrl : create a new one
    QVBoxLayout *MidiGraphicTranslatorsCtrl_Layout;             // GraphicTranslator Ctrl : vertical layout
                                                            // Gui Layout for MidiSources and GraphicTranslator
    QGridLayout *layout;                                        // main layout for gui own part plus MidiSources and Translators
    int MidiSourcesFirstRow;                                    // id of the first row of MidiSources gui (always 0)
    int MidiSourcesNbRows;                                      // nb rows of MidiSources gui (1 or 2)
    int MidiGraphicTranslatorsFirstRow;                         // id of the first row of translators gui (1 or 2)
    int MidiGraphicTranslatorsNbRows;                           // nb rows of Translators gui (1 or 2)
                                                            // GraphicDisplayer (2D rendering engine + window)
    //GraphicDisplayer *gDispl;                                   // the rendering machine

};

#endif // GUI_H
