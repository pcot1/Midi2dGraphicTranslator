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
#include "RtMidi.h"
#include "Midivent.h"
#include "rtMidiPortConsumers.h"
#include "MidiSource.h"
#include "MidiGraphicTranslator.h"
#include "graphicDisplayer.h"

#ifdef DEBUG
#define PRINTF(args)    printf args
#else
#define PRINTF(args)
#endif

const int nbMaxRtMidiPorts = 16;

const int nbMaxMidiSources = 16;
const int nbMaxMidiSourcesPerRow = 8;
const int nbMaxMidiGraphicTranslators = 16;
const int nbMaxMidiGraphicTranslatorsPerRow = 8;
const int widthLayoutActiveCell = 140;
const int widthLayoutControlCell = 90;

class Gui : public QWidget
{
    Q_OBJECT

public:
    static void rtMidiCallBack(double deltatime, std::vector<unsigned char> *message, void *userData);
    Gui();
    ~Gui();
    int getNbRtMidiPorts(void);
    QStringList *getRtMidiInPortNames;
    int getNbMidiSources(void);
    int getNbMidiGraphicTranslators(void);
public slots:
    void removeWidgetsFromGuiLayout(void);
    void installWidgetsInGuiLayout(void);
    void setNbMidiSources(int);
    void addMidiGraphicTranslator(void);
    void deleteMidiGraphicTranslator(int id);
    void moveMidiGraphicTranslators(int translatorInstanceId, int mvt);
    void addConsumerRequest(int MidiSourceId);
    void removeConsumerRequest(int MidiSourceId);
private:
    void printObject(void) const;
                                                            // RtMidi Ports Management
    int    nbMidiPorts;
    RtMidiIn *rtMidiInPorts[nbMaxMidiPorts];
    RtMidiPortConsumers *rtMidiPortConsumers[nbMaxMidiPorts];
    QStringList *rtMidiInPortNames;
    voidscanMidiInputPorts(void);

    int    nbMidiSources;                                   // Midi Sources Ctrl
    QDial *nbMidiSources_Dial;
    QLabel *nbMidiSources_Label;
    QVBoxLayout *MidiSourcesCtrl_Layout;
    MidiSource *MidiSources[nbMaxMidiSources];
    void propagateNbMidiSourcesUpdate(void);
    int    nbMidiGraphicTranslators;                        // Midi Graphic Translators Ctrl
    QLabel *nbMidiGraphicTranslators_Label;
    QPushButton *addMidiGraphicTranslators_Button;
    QVBoxLayout *MidiGraphicTranslatorsCtrl_Layout;
    MidiGraphicTranslator *MidiGraphicTranslators[nbMaxMidiGraphicTranslators];
    int translatorInstanceIds[nbMaxMidiGraphicTranslators];
    int translatorRenderingOrder[nbMaxMidiGraphicTranslators];
                                                            // Gui Layout
    QGridLayout *layout;
    int MidiSourcesFirstRow;
    int MidiSourcesNbRows;
    int MidiGraphicTranslatorsFirstRow;
    int MidiGraphicTranslatorsNbRows;
                                                            // GraphicDisplayer (2D rendering engine + window)
    GraphicDisplayer *gDispl;
};

#endif // GUI_H
