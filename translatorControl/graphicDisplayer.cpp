#define DEBUG
#include <stdio.h>
#include "gui.h"


Gui::Gui() : QWidget()
{
                                                                    // MidiSources CTRL: label + Dial
    nbMidiSources_Label = new QLabel(" 2 Midi Sources");
    nbMidiSources_Label->setFixedWidth(widthLayoutControlCell);
    nbMidiSources_Label->setAlignment(Qt::AlignHCenter);
    nbMidiSources_Dial = new  QDial();
    nbMidiSources_Dial->setMinimum(1);
    nbMidiSources_Dial->setMaximum(nbMaxMidiSources);
    nbMidiSources_Dial->setValue(2);
    nbMidiSources_Dial->setWrapping(false);
    nbMidiSources_Dial->setNotchesVisible(true);
    nbMidiSources_Dial->setTracking(false);
    nbMidiSources_Dial->setMaximumWidth(widthLayoutControlCell);
    QObject::connect(nbMidiSources_Dial,SIGNAL(valueChanged(int)),this,SLOT(setNbMidiSources(int)));
    MidiSourcesCtrl_Layout = new QVBoxLayout;
    MidiSourcesCtrl_Layout->addWidget(nbMidiSources_Label);
    MidiSourcesCtrl_Layout->addWidget(nbMidiSources_Dial);
                                                                    // Midi Graphic Translators CTRL:  Label + Button
    nbMidiGraphicTranslators_Label = new QLabel(" 3 Translators");
    nbMidiGraphicTranslators_Label->setFixedWidth(widthLayoutControlCell);
    nbMidiGraphicTranslators_Label->setAlignment(Qt::AlignHCenter);
    addMidiGraphicTranslators_Button = new  QPushButton("Add");
    addMidiGraphicTranslators_Button->setMaximumWidth(widthLayoutControlCell);
    QObject::connect(addMidiGraphicTranslators_Button,SIGNAL(clicked()),this,SLOT(addMidiGraphicTranslator()));
    MidiGraphicTranslatorsCtrl_Layout = new QVBoxLayout;
    MidiGraphicTranslatorsCtrl_Layout->addWidget(nbMidiGraphicTranslators_Label);
    MidiGraphicTranslatorsCtrl_Layout->addWidget(addMidiGraphicTranslators_Button);
                                                                    // MidiSources and MidiGraphicTranslators init
    nbMidiSources = 0;
    for (int i = 0; i < nbMaxMidiSources; i++)
        MidiSources[i] = 0;
    nbMidiGraphicTranslators = 0;
    for (int i = 0; i < nbMaxMidiGraphicTranslators; i++) {
        MidiGraphicTranslators[i] = 0;
        translatorRenderingOrder[i] = 0;
    }
    MidiSources[0] = new MidiSource();
    (MidiSources[0])->setFixedWidth(widthLayoutActiveCell);
    MidiSources[1] = new MidiSource();
    (MidiSources[1])->setFixedWidth(widthLayoutActiveCell);
    nbMidiSources = 2;
    MidiGraphicTranslators[0] = new MidiGraphicTranslator(this);
    translatorInstanceIds[0] = (MidiGraphicTranslators[0])->getInstanceId();
    translatorRenderingOrder[0] = 0;
    (MidiGraphicTranslators[0])->setFixedWidth(widthLayoutActiveCell);
    MidiGraphicTranslators[1] = new MidiGraphicTranslator(this);
    translatorInstanceIds[1] = (MidiGraphicTranslators[1])->getInstanceId();
    translatorRenderingOrder[1] = 1;
    (MidiGraphicTranslators[1])->setFixedWidth(widthLayoutActiveCell);
    MidiGraphicTranslators[2] = new MidiGraphicTranslator(this);
    translatorInstanceIds[2] = (MidiGraphicTranslators[2])->getInstanceId();
    translatorRenderingOrder[2] = 2;
    (MidiGraphicTranslators[2])->setFixedWidth(widthLayoutActiveCell);
    nbMidiGraphicTranslators = 3;

    layout = new QGridLayout;
    layout->setSizeConstraint(QLayout::SetFixedSize);
    MidiSourcesFirstRow = 0;
    MidiGraphicTranslatorsFirstRow = 1;
    layout->addLayout(MidiSourcesCtrl_Layout,MidiSourcesFirstRow,0);
    layout->addLayout(MidiGraphicTranslatorsCtrl_Layout,MidiGraphicTranslatorsFirstRow,0);
    layout->addWidget(MidiSources[0],MidiSourcesFirstRow,1);
    layout->addWidget(MidiSources[1],MidiSourcesFirstRow,2);
    layout->addWidget(MidiGraphicTranslators[0],MidiGraphicTranslatorsFirstRow,translatorRenderingOrder[0]+1);
    layout->addWidget(MidiGraphicTranslators[1],MidiGraphicTranslatorsFirstRow,translatorRenderingOrder[1]+1);
    layout->addWidget(MidiGraphicTranslators[2],MidiGraphicTranslatorsFirstRow,translatorRenderingOrder[2]+1);
    setLayout(layout);
}

Gui::~Gui()
{
}

void Gui::printObject(void) const
{
    PRINTF((">>> print GUI contents\n"));

    PRINTF((">   nbMidiSources = %2d: first row = %1d, nb rows %1d\n",nbMidiSources,MidiSourcesFirstRow,MidiSourcesNbRows));
    PRINTF(("    MidiSources = "));
    for (int i = 0; i < nbMidiSources-1; i++)
            PRINTF(("0x%p, ",(void *)MidiSources[i]));
    PRINTF(("0x%p\n",(void *)MidiSources[nbMidiSources-1]));
    PRINTF(("    nbConsumers of each MidiSource = "));
    for (int i = 0; i < nbMidiSources-1; i++)
            PRINTF(("%d, ",(MidiSources[i])->getNbConsumers()));
    PRINTF(("%d\n",(MidiSources[nbMidiSources-1])->getNbConsumers()));

    PRINTF((">   nbMidiGraphicTranslators = %2d, first row = %1d, nb rows %1d\n",nbMidiGraphicTranslators,MidiGraphicTranslatorsFirstRow,MidiGraphicTranslatorsNbRows));
    PRINTF(("    MidiGraphicTranslators = "));
    for (int i = 0; i < nbMidiGraphicTranslators-1; i++)
            PRINTF(("0x%p, ",(void *)MidiGraphicTranslators[i]));
    PRINTF(("0x%p\n",(void *)MidiGraphicTranslators[nbMidiGraphicTranslators-1]));
    PRINTF(("    translatorInstanceIds = "));
    for (int i = 0; i < nbMidiGraphicTranslators-1; i++)
            PRINTF(("%2d, ",translatorInstanceIds[i]));
    PRINTF(("%2d\n",translatorInstanceIds[nbMidiGraphicTranslators-1]));
    PRINTF(("    translatorRenderingOrder ="));
    for (int i = 0; i < nbMidiGraphicTranslators-1; i++)
            PRINTF(("%2d, ",translatorRenderingOrder[i]));
    PRINTF(("%2d\n", translatorRenderingOrder[nbMidiGraphicTranslators-1]));
}




int Gui::getNbMidiSources(void)
{
    return(nbMidiSources);
}

int Gui::getNbMidiGraphicTranslators(void)
{
    return(nbMidiGraphicTranslators);
}


void Gui::removeWidgetsFromGuiLayout(void)
{
    for (int i = nbMidiGraphicTranslators-1; i >= 0; i--)
         layout->removeWidget(MidiGraphicTranslators[translatorRenderingOrder[i]]);
    layout->removeItem(MidiGraphicTranslatorsCtrl_Layout);
    for (int i = nbMidiSources-1; i >= 0; i--)
         layout->removeWidget(MidiSources[i]);
    layout->removeItem(MidiSourcesCtrl_Layout);
}

void Gui::installWidgetsInGuiLayout(void)
{
                                                                           // First, determine nb rows of the grid
    MidiSourcesFirstRow = 0;
    if (nbMidiSources % nbMaxMidiSourcesPerRow == 0)
        MidiSourcesNbRows = nbMidiSources / nbMaxMidiSourcesPerRow;
    else
        MidiSourcesNbRows = (nbMidiSources / nbMaxMidiSourcesPerRow) + 1;
    MidiGraphicTranslatorsFirstRow = MidiSourcesNbRows;
    if (nbMidiGraphicTranslators % nbMaxMidiGraphicTranslatorsPerRow == 0)
        MidiGraphicTranslatorsNbRows = nbMidiGraphicTranslators / nbMaxMidiGraphicTranslatorsPerRow;
    else
        MidiGraphicTranslatorsNbRows = (nbMidiGraphicTranslators / nbMaxMidiGraphicTranslators) + 1;
                                                                           // install the 2 controlers
    layout->addLayout(MidiSourcesCtrl_Layout,MidiSourcesFirstRow,0);
    layout->addLayout(MidiGraphicTranslatorsCtrl_Layout,MidiGraphicTranslatorsFirstRow,0);
                                                                           // install MidiSources
    int Row = 0;
    int Column = 0;
    for (int i = 0; i < nbMidiSources; i++) {
        Row = MidiSourcesFirstRow + (i / nbMaxMidiSourcesPerRow);
        Column = 1 + (i % nbMaxMidiSourcesPerRow);
        layout->addWidget(MidiSources[i], Row, Column);
    }
                                                                            // install MidiGraphicTranslators
    for (int i = 0; i < nbMidiGraphicTranslators; i++) {
        Row = MidiGraphicTranslatorsFirstRow + (i / nbMaxMidiGraphicTranslatorsPerRow);
        Column = 1 + (i % nbMaxMidiGraphicTranslatorsPerRow);
        layout->addWidget(MidiGraphicTranslators[translatorRenderingOrder[i]],Row,Column);
    }
                                                                            // activate new Grid Layout
    setLayout(layout);
}

void Gui::addMidiGraphicTranslator(void)
{
    if (nbMidiGraphicTranslators >= nbMaxMidiGraphicTranslators)
        return;

    removeWidgetsFromGuiLayout();

    MidiGraphicTranslators[nbMidiGraphicTranslators] = new MidiGraphicTranslator(this);
    (MidiGraphicTranslators[nbMidiGraphicTranslators])->setFixedWidth(widthLayoutActiveCell);
    (MidiGraphicTranslators[nbMidiGraphicTranslators])->recieveNumberOfMidiSources(nbMidiSources);
    translatorInstanceIds[nbMidiGraphicTranslators] = (MidiGraphicTranslators[nbMidiGraphicTranslators])->getInstanceId();
    translatorRenderingOrder[nbMidiGraphicTranslators] = nbMidiGraphicTranslators;
    ++nbMidiGraphicTranslators;
    QString bla;    bla.sprintf("%2d Translators",nbMidiGraphicTranslators);
    nbMidiGraphicTranslators_Label->setText(bla);

    installWidgetsInGuiLayout();
}

void Gui::deleteMidiGraphicTranslator(int iId)
{
    PRINTF((">>> deleteMidiGraphicTranslator(%2d)\n",iId));
    printObject();
    removeWidgetsFromGuiLayout();

    int guiId = 0;                                              // re compact the translatorInstanceIds array
    for (int i = 0; i < nbMidiGraphicTranslators; i++)
        if (translatorInstanceIds[i] == iId) {
            guiId = i;
            break;
        }
    PRINTF(("guiId = %2d\n",guiId));
    for (int i = guiId; i < (nbMidiGraphicTranslators-1); i++)
        translatorInstanceIds[i] = translatorInstanceIds[i+1];

    int rendererId = 0;                                         // re compact the translatorRenderingOrder array
    for (int i = 0; i < nbMidiGraphicTranslators; i++)
        if (translatorRenderingOrder[i] == guiId) {
            rendererId = i;
            break;
        }
    PRINTF(("rendererId = %2d\n",rendererId));
    for (int i = rendererId; i < (nbMidiGraphicTranslators-1); i++)
        translatorRenderingOrder[i] = translatorRenderingOrder[i+1];


    delete MidiGraphicTranslators[guiId];                       // delete the translator instance
                                                                // re compact MidiGraphicTranslators pointer array
    for (int i = guiId; i < (nbMidiGraphicTranslators-1); i++)
        MidiGraphicTranslators[i] = MidiGraphicTranslators[i+1];
    --nbMidiGraphicTranslators;

    for (int i = 0; i < nbMidiGraphicTranslators; i++)
        if (translatorRenderingOrder[i] >= guiId)
            --translatorRenderingOrder[i];

    QString bla;    bla.sprintf("%2d Translators",nbMidiGraphicTranslators);
    nbMidiGraphicTranslators_Label->setText(bla);

    installWidgetsInGuiLayout();
    printObject();
}

void Gui::moveMidiGraphicTranslators(int iId, int mvt)
{
    PRINTF((">>> moveMidiGraphicTranslators(%2d,%d)\n",iId,mvt));
    printObject();
    removeWidgetsFromGuiLayout();

    int guiId = 0;                                              // find guiId
    for (int i = 0; i < nbMidiGraphicTranslators; i++)
        if (translatorInstanceIds[i] == iId) {
            guiId = i;
            break;
        }
    PRINTF(("guiId = %2d\n",guiId));
    int actualPosition = 0;
    for (int i = 0; i < nbMidiGraphicTranslators; i++)
        if (translatorRenderingOrder[i] == guiId) {
            actualPosition = i;
            break;
        }
    int destination = actualPosition + mvt;
    if (destination < 0)    destination = 0;
    if (destination >= nbMidiGraphicTranslators)    destination = nbMidiGraphicTranslators-1;
    PRINTF(("actualPosition = %2d, destination = %2d\n",actualPosition, destination));

    for (int i = actualPosition; i < destination; i++)
        translatorRenderingOrder[i] = translatorRenderingOrder[i+1];
    for (int i = actualPosition; i > destination; i--)
        translatorRenderingOrder[i] = translatorRenderingOrder[i-1];
    translatorRenderingOrder[destination] = guiId;

    installWidgetsInGuiLayout();
    printObject();
}

void Gui::setNbMidiSources(int wish)
{
    int start;
    if (wish < 1)   wish=1;
    if (wish > nbMaxMidiSources)   wish=nbMaxMidiSources;

    removeWidgetsFromGuiLayout();

    start = nbMidiSources;
    for (int n = start; n < wish; n++) {
        MidiSources[n] = new MidiSource();
        (MidiSources[n])->setFixedWidth(widthLayoutActiveCell);
        ++nbMidiSources;
    }

    start = nbMidiSources;
    for (int n = start; n > wish; n--) {
        delete MidiSources[n-1];
        --nbMidiSources;
        layout->removeWidget(MidiSources[n]);
    }

    QString bla;    bla.sprintf("%2d Midi Sources",nbMidiSources);
    nbMidiSources_Label->setText(bla);

    propagateNbMidiSourcesUpdate();
    installWidgetsInGuiLayout();
}

void Gui::propagateNbMidiSourcesUpdate(void)
{
    for (int i = 0; i < nbMidiGraphicTranslators; i++)
       (MidiGraphicTranslators[i])-> recieveNumberOfMidiSources(nbMidiSources);
}

void Gui::addConsumerRequest(int MidiSourceId)
{
    PRINTF(("addConsumerRequest(%d)\n",MidiSourceId));
    (MidiSources[MidiSourceId])->addConsumer((MidiConsumer *)(QObject::sender()));
}

void Gui::removeConsumerRequest(int MidiSourceId)
{
     PRINTF(("removeConsumerRequest(%d)\n",MidiSourceId));
    (MidiSources[MidiSourceId])->removeConsumer((MidiConsumer *)(QObject::sender()));
}
