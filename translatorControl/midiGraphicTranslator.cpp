#define DEBUG
#include "midiGraphicTranslator.h"

// debug
char bla[128];
#include "graphicDisplayer.h"
extern GraphicDisplayer *grDispl;


                                                                     // managing index rules
inline int displayId2internalId(int i) {return(i-1);}                       // first internalId is 0
inline int internalId2displayId(int i) {return(i+1);}                       // first displayId is 1

int MidiGraphicTranslator::nbCreated = 0;

// *** Constructor
MidiGraphicTranslator::MidiGraphicTranslator(QWidget *pparent) : QGroupBox(pparent), MidiventConsumer()
{
                                                                       // set Translator instanceId and title
    //char bla[128];
    instanceId=nbCreated;
    ++nbCreated;
    sprintf(bla,"Graphic Translator #%02d",internalId2displayId(instanceId));
    setTitle(bla);                                                    //
                                                                      // Translator Name + Quit Button;
    sprintf(bla,"Translator #%02d",internalId2displayId(instanceId));
    MidiGraphicTranslatorName = new QLineEdit(bla);
    quit_Button = new QPushButton();
    quit_Button->setIcon(QIcon("./power-icon.png"));
    //QHBoxLayout *header_Layout = new QHBoxLayout;
    header_Layout = new QHBoxLayout;
    header_Layout->addWidget(MidiGraphicTranslatorName);
    header_Layout->addWidget(quit_Button);
    QObject::connect(quit_Button,SIGNAL(clicked()),this,SLOT(terminate()));
    QObject::connect(this,SIGNAL(suicide(int)),pparent,SLOT(deleteMidiGraphicTranslator(int)));
                                                                      // Buttons FR,R,F,FF
    FastReverse_Button = new QPushButton("|<");
    FastReverse_Button->setMaximumSize(smallButtonWidth,smallButtonHeight);
    Reverse_Button = new QPushButton("<");
    Reverse_Button->setMaximumSize(smallButtonWidth,smallButtonHeight);
    Forward_Button = new QPushButton(">");
    Forward_Button->setMaximumSize(smallButtonWidth,smallButtonHeight);
    FastForward_Button = new QPushButton(">|");
    FastForward_Button->setMaximumSize(smallButtonWidth,smallButtonHeight);
    //QHBoxLayout *rendererOrder_Layout = new QHBoxLayout;
    rendererOrder_Layout = new QHBoxLayout;
    rendererOrder_Layout->addWidget(FastReverse_Button);
    rendererOrder_Layout->addWidget(Reverse_Button);
    rendererOrder_Layout->addWidget(Forward_Button);
    rendererOrder_Layout->addWidget(FastForward_Button);
    QObject::connect(FastReverse_Button,SIGNAL(clicked()),this,SLOT(requireWidgetMvtFastReverse()));
    QObject::connect(Reverse_Button,SIGNAL(clicked()),this,SLOT(requireWidgetMvtReverse()));
    QObject::connect(Forward_Button,SIGNAL(clicked()),this,SLOT(requireWidgetMvtForward()));
    QObject::connect(FastForward_Button,SIGNAL(clicked()),this,SLOT(requireWidgetMvtFastForward()));
    QObject::connect(this,SIGNAL(widgetMvtRequired(int,int)),pparent,SLOT(moveMidiGraphicTranslators(int,int)));
                                                                       // MidiSource Label + ComboBox
    MidiSource_Label = new QLabel("Midi Source:");
    MidiSources_ComboBox = new QComboBox();
    MidiSources_ComboBox->addItem((QString)("None"));
    updateListOfMidiSourcesInComboBox(2);
    MidiSources_ComboBox->setCurrentIndex(0);
    qCDebug(GUtri,"MidiSources_ComboBox current index %d\n",MidiSources_ComboBox->currentIndex());
    MidiSourceId = undefinedMidiSource;
    //QHBoxLayout *MidiSource_Layout = new QHBoxLayout;
    MidiSource_Layout = new QHBoxLayout;
    MidiSource_Layout->addWidget(MidiSource_Label);
    MidiSource_Layout->addWidget(MidiSources_ComboBox);
    QObject::connect(MidiSources_ComboBox,SIGNAL(currentIndexChanged(int)),this,SLOT(changingMidiSource(int)));
    QObject::connect(this,SIGNAL(unRegisterMidiSource(int)),pparent,SLOT(removeConsumerRequest(int)));
    QObject::connect(this,SIGNAL(registerMidiSource(int)),pparent,SLOT(addConsumerRequest(int)));
                                                                    // GroupBox Vertical Layout
    //QVBoxLayout *MidiGraphicTranslatorGroupLayout = new QVBoxLayout;
    MidiGraphicTranslatorGroupLayout = new QVBoxLayout;
    MidiGraphicTranslatorGroupLayout->addLayout(header_Layout);
    MidiGraphicTranslatorGroupLayout->addLayout(rendererOrder_Layout);
    MidiGraphicTranslatorGroupLayout->addLayout(MidiSource_Layout);
    setLayout(MidiGraphicTranslatorGroupLayout);
                                                                    // the layer of the graphic scene of this translator
    graphicLayer = new QGraphicsItemGroup();
    QList <QGraphicsItem *> itemList = graphicLayer->childItems();
    qCDebug(GRgrp,"Translator #%02d (0x%p) groupItem 0x%p contient %d items \n",internalId2displayId(instanceId),this,graphicLayer,itemList.size());
// Debug
    /*
    PRINTF(("Graphic Translator %d is %p\n",instanceId,this));
    Midivent evt(NoteOn,64);
    evt.printObject();
    receiveMidivent(&evt);
    */
}

// *** Destructor
MidiGraphicTranslator::~MidiGraphicTranslator(void)
{
    if (MidiSourceId > undefinedMidiSource)                         // the Translator is no longer a MidiSource consumer
        emit unRegisterMidiSource(MidiSourceId);

}

void MidiGraphicTranslator::printObject(void) const
{
    qCDebug(GUtru,">>> printObject: MidiGraphicTranslator[%1d] showed as #%02d is 0x%p\n",instanceId,internalId2displayId(instanceId),this);
}

// *** accessor
int MidiGraphicTranslator::getInstanceId(void) const
{
    return(instanceId);
}

// *** accessor
QGraphicsItemGroup *MidiGraphicTranslator::getGraphics(void) const
{
    return(graphicLayer);
}

void MidiGraphicTranslator::updateListOfMidiSourcesInComboBox(int nbMS)
{
    qCDebug(GUtru,">>> updateListOfMidiSourcesInComboBox(%2d) of instance %d\n",nbMS,instanceId);
    int nbItems = MidiSources_ComboBox->count();
    int oldNbMS = nbItems - 1;
    qCDebug(GUtru,"    update from %d to %d MidiSources\n",oldNbMS,nbMS);
    int initialCBMidiSourceId = MidiSources_ComboBox->currentIndex();
    qCDebug(GUtru,"    old currentItem of Translator instance %d was %d\n",instanceId,initialCBMidiSourceId);
    if (initialCBMidiSourceId < 0)                                          // probably useless because current item is 1
        initialCBMidiSourceId = 0;
    if (initialCBMidiSourceId > internalId2displayId(nbMS-1)) {             // currentItem should not be > new last item
        MidiSources_ComboBox->setCurrentIndex(0);
        qCDebug(GUtru,"    currentItem of Translator instance %d is now %d\n",instanceId,MidiSources_ComboBox->currentIndex());
    }

    for (int i = nbItems-1; i >= nbMS; i--)
        MidiSources_ComboBox->removeItem(internalId2displayId(i));

    for (int i = nbItems-1; i < nbMS; i++) {
        char bla[8];
        sprintf(bla,"#%02d",internalId2displayId(i));
        MidiSources_ComboBox->addItem((QString)(bla));
    }
    qCDebug(GUtru,"    currentItem of Translator instance %d is now %d\n",instanceId,MidiSources_ComboBox->currentIndex());
}


void MidiGraphicTranslator::terminate(void)
{
    emit suicide(instanceId);
}

void MidiGraphicTranslator::requireWidgetMvtFastReverse(void)   {
    emit widgetMvtRequired(instanceId,-99);
}
void MidiGraphicTranslator::requireWidgetMvtReverse(void)   {
    emit widgetMvtRequired(instanceId,-1);
}
void MidiGraphicTranslator::requireWidgetMvtForward(void)   {
    emit widgetMvtRequired(instanceId,1);
}
void MidiGraphicTranslator::requireWidgetMvtFastForward(void)   {
    emit widgetMvtRequired(instanceId,99);
}


void MidiGraphicTranslator::changingMidiSource(int displayId)
{
    if (displayId == undefinedMidiSource)               // the ComboBox is not initialized
        return;
    int msId = displayId2internalId(displayId);
    qCDebug(GUtru,"changingMidiSource of instance %d: (%d -> %d ) \n",instanceId,MidiSourceId,msId);
    if (msId == MidiSourceId)
        return;
    if (MidiSourceId != undefinedMidiSource)
        emit unRegisterMidiSource(MidiSourceId);
    MidiSourceId = msId;
    emit registerMidiSource(MidiSourceId);
}

void MidiGraphicTranslator::receiveNumberOfMidiSources(int nbMS)
{
    qCDebug(GUtru,"receiveNumberOfMidiSources(%d) in instance %d \n",nbMS,instanceId);
    updateListOfMidiSourcesInComboBox(nbMS);
}

// *** generate a point coordinate in the world
QPointF MidiGraphicTranslator::generateRandomWorldCoordinates(void) const
{
    const float zoomer = 32.0;
    int SX = (int) (worldWidth * zoomer);
    int SY = (int) (worldHeight * zoomer);
    int DX = (int) (worldUpLeftX * zoomer);
    int DY = (int) (worldUpLeftY * zoomer);
    qCDebug(GRgrp,"SX %d SY %d DX %d DY %d => (%+3.1f,%+3.1f)\n",SX,SY,DX,DY,(qreal)(DX + (rand()%SX)) / zoomer,(qreal)(DY + (rand()%SY)) / zoomer);
    return(QPointF((qreal) (DX + (rand()%SX)) / zoomer,(qreal) (DY + (rand()%SY)) / zoomer));
}

// *** transform a Midivent in graphic action
void MidiGraphicTranslator::receiveMidivent(Midivent *pevt)
{
    //char bla[64];
    QGraphicsTextItem *pText = new  QGraphicsTextItem;
    QPointF where = generateRandomWorldCoordinates();
    QList <QGraphicsItem *> itemList;

    qCDebug(Mvent,"Translator #%02d (0x%p) receiving Midivent 0x%p\n", internalId2displayId(instanceId),this,pevt);
    switch (pevt->getType())  {
        case None:
            qCDebug(Mvent,"Midivent is \"None\"\n"); break;
        case NoteOn:
            qCDebug(Mvent,"Midivent is \"NoteOn %3d\"\n",pevt->getNote());
            qCDebug(GRgrp,"update Translator graphic Layer 0x%p\n",graphicLayer);
            grDispl->removeItemFromScene(graphicLayer);
            qCDebug(GRgrp,"drawing text 0x%p in (%+3.1f,%+3.1f)\n",pText,where.x(),where.y());
            sprintf(bla,"#%02d: (%+3.1f,%+3.1f)",instanceId,where.x(),where.y());
            pText->setPlainText(QString(bla));
            pText->setPos(where);
            qCDebug(GRgrp,"add text 0x%p to group 0x%p\n",pText,graphicLayer);
            //grDispl->drawSomething();
            //grDispl->addItemInScene(pText);
            graphicLayer->addToGroup((QGraphicsItem *)(pText));
            //graphicLayer->update();
            itemList = graphicLayer->childItems();
            qCDebug(GRgrp,"groupItem 0x%p contient %d items\n",graphicLayer,itemList.size());
            grDispl->addItemToScene(graphicLayer);
            grDispl->update();
            break;
        case NoteOff:
            qCDebug(Mvent,"Midivent is \"NoteOff %3d\"\n",pevt->getNote());
            itemList = graphicLayer->childItems();
            qCDebug(GRgrp,"groupItem 0x%p contient %d items before \n",graphicLayer,itemList.size());
            for (int i = 0; i < itemList.size(); i++) {
                //graphicLayer->removeFromGroup(itemList.at(i));
                (graphicLayer->scene())->removeItem(itemList.at(i));
            }
            itemList = graphicLayer->childItems();
            qCDebug(GRgrp,"groupItem 0x%p contient %d items after\n",graphicLayer,itemList.size());
            break;
        default:
            qCDebug(Mvent,"unknown \"%d\", \"%d\"\n",pevt->getType(),pevt->getNote());
    }
}
