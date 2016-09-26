#define DEBUG
#include "gui.h"
// DEBUG
#include "midivent.h"

                                                                    // managing index rules
inline int displayId2internalId(int i) {return(i-1);}                       // first internalId is 0
inline int internalId2displayId(int i) {return(i+1);}                       // first displayId is 1


Gui::Gui(RtMidiPorts *theRtMidiInPortsManager) : QWidget()
{
    qCInfo(GUinit,"entering Gui constructor\n");
                                                                    // RtMidi input ports management
    rtMidiInPortsManager = theRtMidiInPortsManager;                     // dedicated class RtMidiInPorts to manage RtMidi lib
    //rtMidiInPortsManager->setVisible(false);                            // hide the window QLabel because MidiPort discovering is done

                                                                    // create the single GraphicDisplayer
    gDispl = new GraphicDisplayer;

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
    MidiSources[0] = new MidiSource(this,theRtMidiInPortsManager->getRtMidiPortsNames());
    (rtMidiInPortsManager->getConsumersOfMidiPortIndex(0))->addConsumer(MidiSources[0]);
    (MidiSources[0])->setFixedWidth(widthLayoutActiveCell);
    MidiSources[1] = new MidiSource(this,theRtMidiInPortsManager->getRtMidiPortsNames());
    (rtMidiInPortsManager->getConsumersOfMidiPortIndex(0))->addConsumer(MidiSources[1]);
    (MidiSources[1])->setFixedWidth(widthLayoutActiveCell);
    nbMidiSources = 2;
    MidiGraphicTranslators[0] = new MidiGraphicTranslator(this);
    translatorInstanceIds[0] = (MidiGraphicTranslators[0])->getInstanceId();
    gDispl->addItemToScene(MidiGraphicTranslators[0]->getTranslatorGraphicLayer());
    translatorRenderingOrder[0] = 0;
    (MidiGraphicTranslators[0])->setFixedWidth(widthLayoutActiveCell);
    MidiGraphicTranslators[1] = new MidiGraphicTranslator(this);
    translatorInstanceIds[1] = (MidiGraphicTranslators[1])->getInstanceId();
    gDispl->addItemToScene(MidiGraphicTranslators[1]->getTranslatorGraphicLayer());
    translatorRenderingOrder[1] = 1;
    (MidiGraphicTranslators[1])->setFixedWidth(widthLayoutActiveCell);
    //MidiGraphicTranslators[2] = new MidiGraphicTranslator(this);
    MidiGraphicTranslators[2] = new One2OneTranslator(this);
    translatorInstanceIds[2] = (MidiGraphicTranslators[2])->getInstanceId();
    gDispl->addItemToScene(MidiGraphicTranslators[2]->getTranslatorGraphicLayer());
    translatorRenderingOrder[2] = 2;
    (MidiGraphicTranslators[2])->setFixedWidth(widthLayoutActiveCell);
    nbMidiGraphicTranslators = 3;
                                                                     // Gui final layout
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
                                                                    // Expose Gui window
    this->show();
    this->move(0,100);
    qCInfo(GUinit,"terminating Gui constructor\n");
}

Gui::~Gui()
{
    qCInfo(GUinit,"Gui destructor\n");
}

void Gui::printObject(void) const
{

    qCDebug(GUupd,">>> print GUI contents");

    qCDebug(GUupd,">   nbMidiSources = %2d: first row = %1d, nb rows %1d\n",nbMidiSources,MidiSourcesFirstRow,MidiSourcesNbRows);
    qCDebug(GUupd,"    MidiSources = ");
    for (int i = 0; i < nbMidiSources-1; i++)
            qCDebug(GUupd,"%p, ",(void *)MidiSources[i]);
    qCDebug(GUupd,"%p\n",(void *)MidiSources[nbMidiSources-1]);
    qCDebug(GUupd,"    nbConsumers of each MidiSource = ");
    for (int i = 0; i < nbMidiSources-1; i++)
            qCDebug(GUupd,"%d, ",(MidiSources[i])->getNbConsumers());
    qCDebug(GUupd,"%d\n",(MidiSources[nbMidiSources-1])->getNbConsumers());

    qCDebug(GUupd,">   nbMidiGraphicTranslators = %2d, first row = %1d, nb rows %1d\n",nbMidiGraphicTranslators,MidiGraphicTranslatorsFirstRow,MidiGraphicTranslatorsNbRows);
    qCDebug(GUupd,"    MidiGraphicTranslators = ");
    for (int i = 0; i < nbMidiGraphicTranslators-1; i++)
            qCDebug(GUupd,"%p, ",(void *)MidiGraphicTranslators[i]);
    qCDebug(GUupd,"%p\n",(void *)MidiGraphicTranslators[nbMidiGraphicTranslators-1]);
    qCDebug(GUupd,"    translatorInstanceIds = ");
    for (int i = 0; i < nbMidiGraphicTranslators-1; i++)
            qCDebug(GUupd,"%2d, ",translatorInstanceIds[i]);
    qCDebug(GUupd,"%2d\n",translatorInstanceIds[nbMidiGraphicTranslators-1]);
    qCDebug(GUupd,"    translatorRenderingOrder =");
    for (int i = 0; i < nbMidiGraphicTranslators-1; i++)
            qCDebug(GUupd,"%2d, ",translatorRenderingOrder[i]);
    qCDebug(GUupd,"%2d\n", translatorRenderingOrder[nbMidiGraphicTranslators-1]);
}



// *** accessor
int Gui::getNbMidiSources(void)
{
    return(nbMidiSources);
}

// *** create/delete MidiSources
void Gui::setNbMidiSources(int wish)
{
    qCInfo(GUinit,"entering Gui::setNbMidiSources(%1d)\n",wish);
    int start;
    if (wish < 1)   wish=1;
    if (wish > nbMaxMidiSources)   wish=nbMaxMidiSources;

    removeWidgetsFromGuiLayout();

    start = nbMidiSources;
    for (int n = start; n < wish; n++) {
        MidiSources[n] = new MidiSource(this,rtMidiInPortsManager->getRtMidiPortsNames());
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
    qCInfo(GUinit,"terminating Gui::setNbMidiSources\n");
}

// *** Slot : tell the RtMidi Manager a MidiSource has changed its Midi Port
void Gui::aMidiSourceHasChangedItsMidiPort(int oldIndex, int newIndex)
{
    qCInfo(GUinit,"Gui::aMidiSourceHasChangedItsMidiPort(%1d,%1d)\n",oldIndex,newIndex);
                                                                    // this implementation does not really "modify"
                                                                    // the list of MidiSources that listen to each
                                                                    // Midi Port but rebuilt it from scratch
    MidiPortConsumers *pConsumers = 0;
    int nbMidiPorts = rtMidiInPortsManager->getNbRtMidiPorts();

    for (int i = 0; i < nbMidiPorts; i++) {
        pConsumers = rtMidiInPortsManager->getConsumersOfMidiPortIndex(i);
        pConsumers->removeAllMidiPortConsumers();
    }
    for (int i = 0; i < nbMidiSources; i++) {
        pConsumers = rtMidiInPortsManager->getConsumersOfMidiPortIndex(MidiSources[i]->getRtMidiInPortIndex());
        pConsumers->addConsumer(MidiSources[i]);
    }
}

// *** accessor
int Gui::getNbMidiGraphicTranslators(void)
{
    return(nbMidiGraphicTranslators);
}

// *** delete all widgets from gui layout
void Gui::removeWidgetsFromGuiLayout(void)
{
    qCInfo(GUupd,"entering Gui::removeWidgetsFromGuiLayout\n");
    for (int i = nbMidiGraphicTranslators-1; i >= 0; i--)
         layout->removeWidget(MidiGraphicTranslators[translatorRenderingOrder[i]]);
    layout->removeItem(MidiGraphicTranslatorsCtrl_Layout);
    for (int i = nbMidiSources-1; i >= 0; i--)
         layout->removeWidget(MidiSources[i]);
    layout->removeItem(MidiSourcesCtrl_Layout);
    qCInfo(GUupd,"terminating Gui::removeWidgetsFromGuiLayout\n");
}

// *** install all widgets in gui layout
void Gui::installWidgetsInGuiLayout(void)
{
    qCInfo(GUupd,"entering Gui::installWidgetsInGuiLayout\n");
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
    qCInfo(GUupd,"terminating Gui::installWidgetsInGuiLayout\n");
}

void Gui::addMidiGraphicTranslator(void)
{
    qCInfo(GUupd,"entering Gui::addMidiGraphicTranslator\n");
    if (nbMidiGraphicTranslators >= nbMaxMidiGraphicTranslators)
        return;

    removeWidgetsFromGuiLayout();

    MidiGraphicTranslators[nbMidiGraphicTranslators] = new MidiGraphicTranslator(this);
    (MidiGraphicTranslators[nbMidiGraphicTranslators])->setFixedWidth(widthLayoutActiveCell);
    (MidiGraphicTranslators[nbMidiGraphicTranslators])->receiveNumberOfMidiSources(nbMidiSources);
    translatorInstanceIds[nbMidiGraphicTranslators] = (MidiGraphicTranslators[nbMidiGraphicTranslators])->getInstanceId();
    translatorRenderingOrder[nbMidiGraphicTranslators] = nbMidiGraphicTranslators;
    gDispl->addItemToScene(MidiGraphicTranslators[nbMidiGraphicTranslators]->getTranslatorGraphicLayer());
    ++nbMidiGraphicTranslators;
    QString bla;    bla.sprintf("%2d Translators",nbMidiGraphicTranslators);
    nbMidiGraphicTranslators_Label->setText(bla);

    installWidgetsInGuiLayout();
    qCInfo(GUupd,"terminating Gui::addMidiGraphicTranslator\n");
}

void Gui::deleteMidiGraphicTranslator(int iId)
{
    qCInfo(GUupd,"entering Gui::deleteMidiGraphicTranslator(%2d)\n",iId);
    printObject();
    gDispl->removeItemFromScene(MidiGraphicTranslators[iId]->getTranslatorGraphicLayer());
    removeWidgetsFromGuiLayout();

    int guiId = 0;                                              // re compact the translatorInstanceIds array
    for (int i = 0; i < nbMidiGraphicTranslators; i++)
        if (translatorInstanceIds[i] == iId) {
            guiId = i;
            break;
        }
    qCDebug(GUupd,"guiId = %2d\n",guiId);
    for (int i = guiId; i < (nbMidiGraphicTranslators-1); i++)
        translatorInstanceIds[i] = translatorInstanceIds[i+1];

    int rendererId = 0;                                         // re compact the translatorRenderingOrder array
    for (int i = 0; i < nbMidiGraphicTranslators; i++)
        if (translatorRenderingOrder[i] == guiId) {
            rendererId = i;
            break;
        }
    qCDebug(GUupd,"rendererId = %2d\n",rendererId);
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
    qCInfo(GUupd,"terminating Gui::deleteMidiGraphicTranslator\n");
}

void Gui::moveMidiGraphicTranslators(int iId, int mvt)
{
    qCInfo(GUupd,"entering Gui::moveMidiGraphicTranslators(%2d,%d)\n",iId,mvt);
    printObject();
    gDispl->emptyScene();
    removeWidgetsFromGuiLayout();

    int guiId = 0;                                              // find guiId
    for (int i = 0; i < nbMidiGraphicTranslators; i++)
        if (translatorInstanceIds[i] == iId) {
            guiId = i;
            break;
        }
    qCDebug(GUupd,"guiId = %2d\n",guiId);
    int actualPosition = 0;
    for (int i = 0; i < nbMidiGraphicTranslators; i++)
        if (translatorRenderingOrder[i] == guiId) {
            actualPosition = i;
            break;
        }
    int destination = actualPosition + mvt;
    if (destination < 0)    destination = 0;
    if (destination >= nbMidiGraphicTranslators)    destination = nbMidiGraphicTranslators-1;
    qCDebug(GUupd,"actualPosition = %2d, destination = %2d\n",actualPosition, destination);

    for (int i = actualPosition; i < destination; i++)
        translatorRenderingOrder[i] = translatorRenderingOrder[i+1];
    for (int i = actualPosition; i > destination; i--)
        translatorRenderingOrder[i] = translatorRenderingOrder[i-1];
    translatorRenderingOrder[destination] = guiId;

    for (int i = 0; i < nbMidiGraphicTranslators; i++)
        gDispl->addItemToScene(MidiGraphicTranslators[translatorRenderingOrder[i]]->getTranslatorGraphicLayer());

    installWidgetsInGuiLayout();
    printObject();
    qCInfo(GUupd,"terminating Gui::moveMidiGraphicTranslators\n");
}

void Gui::propagateNbMidiSourcesUpdate(void)
{
    qCInfo(GUupd,"Gui::propagateNbMidiSourcesUpdate\n");
    for (int i = 0; i < nbMidiGraphicTranslators; i++)
       (MidiGraphicTranslators[i])-> receiveNumberOfMidiSources(nbMidiSources);
}

void Gui::addConsumerRequest(int MidiSourceId)
{
    qCInfo(GUupd,"Gui::addConsumerRequest(%1d)\n",MidiSourceId);
    MidiventConsumer *MC = dynamic_cast<MidiventConsumer *>(QObject::sender());
    qCDebug(GUupd,"addConsumerRequest for MidiSource %d concerning sender QObject %p seen as MidiConsumer %p\n",MidiSourceId,QObject::sender(),MC);
    qCDebug(GUupd,"MidiSource #%02d will sent Midivent to MidiConsumer %p\n",internalId2displayId(MidiSourceId),MC);
    (MidiSources[MidiSourceId])->addConsumer(MC);
}

void Gui::removeConsumerRequest(int MidiSourceId)
{
     qCInfo(GUupd,"Gui::removeConsumerRequest(%1d)\n",MidiSourceId);
     MidiventConsumer *MC = dynamic_cast<MidiventConsumer *>(QObject::sender());
     (MidiSources[MidiSourceId])->removeConsumer(MC);
}
