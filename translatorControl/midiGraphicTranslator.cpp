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
    nbMidiRecievedNoteOn = 0;
    for (int i = 0; i < 128; i++)
            noteThing[i] = 0;

                                                                      // initialization of GUI part
    sprintf(bla,"Graphic Translator #%02d",internalId2displayId(instanceId));
    setTitle(bla);                                                    // the widget title
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
    qCDebug(GRgrp,"Translator #%02d (%p) groupItem %p contient %d items \n",internalId2displayId(instanceId),this,graphicLayer,itemList.size());
}

// *** Destructor
MidiGraphicTranslator::~MidiGraphicTranslator(void)
{
    if (MidiSourceId > undefinedMidiSource)                         // the Translator is no longer a MidiSource consumer
        emit unRegisterMidiSource(MidiSourceId);

}

void MidiGraphicTranslator::printObject(void) const
{
    qCDebug(GUtru,">>> printObject: MidiGraphicTranslator[%1d] showed as #%02d is %p\n",instanceId,internalId2displayId(instanceId),this);
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

// *** store the thing of a note
void MidiGraphicTranslator::setNoteThing(unsigned char noteId, void *thing) {

    qCDebug(GRgrp,"store %p in note %d\n",thing,noteId);
    noteThing[noteId & (unsigned char)(0b01111111)] = thing;
    return;
}

// *** retreive the thing of a note
void *MidiGraphicTranslator::getNoteThing(unsigned char noteId) const {

    qCDebug(GRgrp,"retrive thing of note %d = %p\n",noteId,noteThing[noteId & (unsigned char)(0b01111111)]);
    return(noteThing[noteId & (unsigned char)(0b01111111)]);
}


// *** transform a Midivent in graphic action
void MidiGraphicTranslator::receiveMidivent(Midivent *pevt)
{
    //char bla[64];
    QGraphicsTextItem *pText = new  QGraphicsTextItem;
    QPointF where = generateRandomWorldCoordinates();
    QList <QGraphicsItem *> itemList;
    QGraphicsItem *item = 0;

    qCDebug(Mvent,"Translator #%02d (%p) receiving Midivent %p\n", internalId2displayId(instanceId),this,pevt);
    switch (pevt->getType())  {
        case None:
            qCDebug(Mvent,"Midivent is \"None\"\n"); break;
        case NoteOn:
            ++nbMidiRecievedNoteOn;
            qCDebug(Mvent,"Midivent is \"NoteOn %3d\" (%d notes currently on)\n",pevt->getNote(),nbMidiRecievedNoteOn);
            qCDebug(GRgrp,"update Translator graphic Layer %p\n",graphicLayer);
            //grDispl->removeItemFromScene(graphicLayer);
            qCDebug(GRgrp,"drawing text %p in (%+3.1f,%+3.1f)\n",pText,where.x(),where.y());
            //sprintf(bla,"#%02d: (%+3.1f,%+3.1f)",instanceId,where.x(),where.y());
            sprintf(bla,"#%02d",instanceId);
            pText->setPlainText(QString(bla));
            pText->setPos(where);
            qCDebug(GRgrp,"the graphicLayer scene is %p\n",graphicLayer->scene());
            qCDebug(GRgrp,"add text %p to group %p\n",pText,graphicLayer);
            //grDispl->drawSomething();
            //grDispl->addItemInScene(pText);
            graphicLayer->addToGroup((QGraphicsItem *)(pText));
            qCDebug(GRgrp,"the item scene is %p\n",pText->scene());
            setNoteThing(pevt->getNote(),(void *)((QGraphicsItem *)(pText)));
            //graphicLayer->update();
            itemList = graphicLayer->childItems();
            qCDebug(GRgrp,"TR #%02d (%p) graphicLayer %p has %d items\n",instanceId,this,graphicLayer,itemList.size());
            //grDispl->addItemToScene(graphicLayer);
            //grDispl->update();
            break;
        case NoteOff:
            --nbMidiRecievedNoteOn;
            qCDebug(Mvent,"Midivent is \"NoteOff %3d\" (%d notes currently on)\n",pevt->getNote(),nbMidiRecievedNoteOn);
            if (nbMidiRecievedNoteOn < 0)   {
                nbMidiRecievedNoteOn = 0;
                qCDebug(Mvent,"nb notes currently on corrected to %d)\n",nbMidiRecievedNoteOn);
            }
            item = (QGraphicsItem *)(getNoteThing(pevt->getNote()));
            if (item == 0)  {
                qCWarning(GRgrp,"TR #%02d (%p) retrive NULL graphics item in noteThing for note %d\n",instanceId,this,pevt->getNote());
                qCDebug(GRgrp,"TR #%02d decision to clean the whole graphicLayer %p\n",instanceId,this,graphicLayer);
                itemList = graphicLayer->childItems();
                qCDebug(GRgrp,"graphicLayer %p had %d items before \n",graphicLayer,itemList.size());
                for (int i = 0; i < itemList.size(); i++) {
                    //graphicLayer->removeFromGroup(itemList.at(i));
                    (graphicLayer->scene())->removeItem(itemList.at(i));
                }
                itemList = graphicLayer->childItems();
                qCDebug(GRgrp,"graphicLayer %p has now %d items after\n",graphicLayer,itemList.size());
                return;
            }
            qCDebug(GRgrp,"the item scene is %p\n",item->scene());
            qCDebug(GRgrp,"the graphicLayer scene is %p\n",graphicLayer->scene());
            qCDebug(GRgrp,"TR #%02d (%p) remove item %p from graphicLayer %p\n",instanceId,this,item,graphicLayer);
            setNoteThing(pevt->getNote(),0);
            (graphicLayer->scene())->removeItem(item);
            itemList = graphicLayer->childItems();
            qCDebug(GRgrp,"graphicLayer %p has now %d items after\n",graphicLayer,itemList.size());
            break;
        default:
            qCDebug(Mvent,"unknown \"%d\", \"%d\"\n",pevt->getType(),pevt->getNote());
    }
}
