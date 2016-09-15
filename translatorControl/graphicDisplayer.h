#ifndef GUI_H
#define GUI_H
#include <Qt>
#include <QWidget>
#include <QDial>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QGridLayout>
#include <QMessageBox>
#include <MidiSource.h>
#include <MidiGraphicTranslator.h>
#include <iostream>
#ifdef DEBUG
#define PRINTF(args)    printf args
#else
#define PRINTF(args)
#endif

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
    Gui();
    ~Gui();
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
    int    nbMidiSources;                                  // Midi Sources Ctrl
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
    void printObject(void) const;
};

#endif // GUI_H
