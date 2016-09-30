#define DEBUG
#include "midiGraphicTranslator.h"

// debug
char bla[128];
#include "graphicDisplayer.h"
extern GraphicDisplayer *grDispl;

                                                                    // No MidiSource selected
const int undefinedMidiSource = -1;
                                                                    // size of small buttons (rendering order)
const int smallButtonHeight = 23;
const int smallButtonWidth = 26;
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
    nbNoteCurrentlyOn = 0;
    nbRecievedNoteOn = 0;
    sumReceivedNoteOn = 0;
    averageReceivedNoteOn = 64;
    for (int i = 0; i < 128; i++)
            noteThing[i] = 0;

    // colorize the MidiSource Widget
    this->setPalette(QPalette(QColor(220,220,230,255)));
    this->setAutoFillBackground(true);
                                                                      // initialization of GUI part
    sprintf(bla,"Graphic Translator #%02d",internalId2displayId(instanceId));
    setTitle(bla);                                                    // the widget title
                                                                      // Translator Name + Quit Button;
    sprintf(bla,"Translator #%02d",internalId2displayId(instanceId));
    MidiGraphicTranslatorName = new QLineEdit(QString(bla));
    quit_Button = new QPushButton();
    quit_Button->setIcon(QIcon("./power-icon.png"));
    //QHBoxLayout *header_Layout = new QHBoxLayout;
    header_Layout = new QHBoxLayout;
    //header_Layout->addWidget(MidiGraphicTranslatorName,0,Qt::AlignTop);
    //header_Layout->addWidget(quit_Button,0,Qt::AlignTop);
    header_Layout->addWidget(MidiGraphicTranslatorName);
    header_Layout->addWidget(quit_Button);
    //header_Layout->setAlignment(Qt::AlignTop);
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
    //rendererOrder_Layout->addWidget(FastReverse_Button,0,Qt::AlignTop);
    //rendererOrder_Layout->addWidget(Reverse_Button,0,Qt::AlignTop);
    //rendererOrder_Layout->addWidget(Forward_Button,0,Qt::AlignTop);
    //rendererOrder_Layout->addWidget(FastForward_Button,0,Qt::AlignTop);
    rendererOrder_Layout->addWidget(FastReverse_Button);
    rendererOrder_Layout->addWidget(Reverse_Button);
    rendererOrder_Layout->addWidget(Forward_Button);
    rendererOrder_Layout->addWidget(FastForward_Button);
    //rendererOrder_Layout->setAlignment(Qt::AlignTop);
    QObject::connect(FastReverse_Button,SIGNAL(clicked()),this,SLOT(requireWidgetMvtFastReverse()));
    QObject::connect(Reverse_Button,SIGNAL(clicked()),this,SLOT(requireWidgetMvtReverse()));
    QObject::connect(Forward_Button,SIGNAL(clicked()),this,SLOT(requireWidgetMvtForward()));
    QObject::connect(FastForward_Button,SIGNAL(clicked()),this,SLOT(requireWidgetMvtFastForward()));
    QObject::connect(this,SIGNAL(widgetMvtRequired(int,int)),pparent,SLOT(moveMidiGraphicTranslators(int,int)));
                                                                       // MidiSource Label + ComboBox
    MidiSource_Label = new QLabel("Midi Source:");
    MidiSources_ComboBox = new QComboBox();
    MidiSources_ComboBox->addItem((QString)("None"));
    updateListOfMidiSourcesInComboBox(1);
    MidiSources_ComboBox->setCurrentIndex(0);
    qCDebug(GUtri,"Translator #%02d MidiSources_ComboBox current index %d\n",getDId(),MidiSources_ComboBox->currentIndex());
    MidiSourceId = undefinedMidiSource;
    //QHBoxLayout *MidiSource_Layout = new QHBoxLayout;
    MidiSource_Layout = new QHBoxLayout;
    MidiSource_Layout->addWidget(MidiSource_Label,0,Qt::AlignVCenter);
    MidiSource_Layout->addWidget(MidiSources_ComboBox,0,Qt::AlignTop);
    MidiSource_Layout->setAlignment(Qt::AlignTop);
    QObject::connect(MidiSources_ComboBox,SIGNAL(currentIndexChanged(int)),this,SLOT(changingMidiSource(int)));
    QObject::connect(this,SIGNAL(unRegisterMidiSource(int)),pparent,SLOT(removeMidiSourceConsumerRequest(int)));
    QObject::connect(this,SIGNAL(registerMidiSource(int)),pparent,SLOT(addMidiSourceConsumerRequest(int)));
                                                                    // GroupBox Vertical Layout
    //QVBoxLayout *MidiGraphicTranslatorGroupLayout = new QVBoxLayout;
    MidiGraphicTranslatorGroupLayout = new QVBoxLayout;
    MidiGraphicTranslatorGroupLayout->setSizeConstraint(QLayout::SetMinimumSize);
    MidiGraphicTranslatorGroupLayout->addLayout(header_Layout);
    MidiGraphicTranslatorGroupLayout->addLayout(rendererOrder_Layout);
    MidiGraphicTranslatorGroupLayout->addLayout(MidiSource_Layout);
    setLayout(MidiGraphicTranslatorGroupLayout);
                                                                    // the layer of the graphic scene of this translator
    graphicLayer = new QGraphicsItemGroup();
    QList <QGraphicsItem *> itemList = graphicLayer->childItems();
    qCDebug(GRgrp,"Translator #%02d (%p) groupItem %p contient %d items \n",getDId(),this,graphicLayer,itemList.size());
}

// *** Destructor
MidiGraphicTranslator::~MidiGraphicTranslator(void)
{
    if (MidiSourceId > undefinedMidiSource)                         // the Translator is no longer a MidiSource consumer
        emit unRegisterMidiSource(MidiSourceId);
    cleanMidiventContext();
    (graphicLayer->scene())->removeItem(graphicLayer);

}

void MidiGraphicTranslator::printObject(void) const
{
    qCDebug(GUtru,">>> printObject: MidiGraphicTranslator[%1d] showed as #%02d is %p\n",instanceId,getDId(),this);
}

// *** add Layout to main vetical Layout
void MidiGraphicTranslator::addLayout(QLayout *layout)   {

    layout->setAlignment(Qt::AlignTop);
    MidiGraphicTranslatorGroupLayout->setAlignment(Qt::AlignTop);
    MidiGraphicTranslatorGroupLayout->addLayout(layout);
    setLayout(MidiGraphicTranslatorGroupLayout);
    return;
}

// *** accessor
void MidiGraphicTranslator::setTranslatorName(QString name)
{
    MidiGraphicTranslatorName->setText(name);
    return;
}

void MidiGraphicTranslator::updateListOfMidiSourcesInComboBox(int nbMS)
{
    qCDebug(GUtru,"Translator #%02d updateListOfMidiSourcesInComboBox(%2d)\n",getDId(),nbMS);
    int nbItems = MidiSources_ComboBox->count();
    int oldNbMS = nbItems - 1;
    qCDebug(GUtru,"Translator #%02d update from %d to %d MidiSources\n",getDId(),oldNbMS,nbMS);
    int initialCBMidiSourceId = MidiSources_ComboBox->currentIndex();
    qCDebug(GUtru,"Translator #%02d old currentItem was %d\n",getDId(),initialCBMidiSourceId);
    if (initialCBMidiSourceId < 0)                                          // probably useless because current item is 1
        initialCBMidiSourceId = 0;
    if (initialCBMidiSourceId > internalId2displayId(nbMS-1)) {             // currentItem should not be > new last item
        MidiSources_ComboBox->setCurrentIndex(0);
        qCDebug(GUtru,"Translator #%02d currentItem is now %d\n",getDId(),MidiSources_ComboBox->currentIndex());
    }

    for (int i = nbItems-1; i >= nbMS; i--)
        MidiSources_ComboBox->removeItem(internalId2displayId(i));

    for (int i = nbItems-1; i < nbMS; i++) {
        char bla[8];
        sprintf(bla,"#%02d",internalId2displayId(i));
        MidiSources_ComboBox->addItem((QString)(bla));
    }
    qCDebug(GUtru,"Translator #%02d currentItem is now %d\n",getDId(),MidiSources_ComboBox->currentIndex());
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
    qCDebug(GRgrp,"Translator #%02d MidiGraphicTranslator::changingMidiSource(#%02d)\n",getDId(),displayId);
    if (displayId == undefinedMidiSource)                // the ComboBox is not initialized (normally never happen)
        return;
    if (displayId == 0)  {                               // no MidiSource
                                                            // delete all item from this translator
        QList <QGraphicsItem *> itemList;
        itemList = graphicLayer->childItems();
        qCWarning(GRgrp,"Translator #%02d cleaning graphicLayer %p which had %d items\n",getDId(),graphicLayer,itemList.size());
        for (int i = 0; i < itemList.size(); i++) {
            (graphicLayer->scene())->removeItem(itemList.at(i));
        }
    }

    int msId = displayId2internalId(displayId);
    qCDebug(GUtru,"Translator #%02d changingMidiSource (%d -> %d ) \n",getDId(),MidiSourceId,msId);
    if (msId == MidiSourceId)
        return;
    if (MidiSourceId != undefinedMidiSource)
        emit unRegisterMidiSource(MidiSourceId);
    MidiSourceId = msId;
    emit registerMidiSource(MidiSourceId);
}

void MidiGraphicTranslator::doUpgradeNumberOfMidiSources(int nbMS)
{
    qCDebug(GUtru,"Translator #%02d doUpgradeNumberOfMidiSources(%d)\n",getDId(),nbMS);
    updateListOfMidiSourcesInComboBox(nbMS);
}

// *** generate a normalized real value (in [0,1])
qreal MidiGraphicTranslator::generateRandomRealNormalizedValue(void) const
{
    return((qreal)(rand()%1000) / 1000.0);
}

// *** generate a point coordinate in the world
QPointF MidiGraphicTranslator::generateRandomWorldCoordinates(void) const
{
    const float zoomer = 32.0;
    int SX = (int) (worldWidth * zoomer);
    int SY = (int) (worldHeight * zoomer);
    int DX = (int) (worldUpLeftX * zoomer);
    int DY = (int) (worldUpLeftY * zoomer);
    qCDebug(GRgrp,"Translator #%02d SX %d SY %d DX %d DY %d => (%+3.1f,%+3.1f)\n",getDId(),SX,SY,DX,DY,(qreal)(DX + (rand()%SX)) / zoomer,(qreal)(DY + (rand()%SY)) / zoomer);
    return(QPointF((qreal) (DX + (rand()%SX)) / zoomer,(qreal) (DY + (rand()%SY)) / zoomer));
}

// *** store the thing of a note
void MidiGraphicTranslator::setNoteThing(septet note, void *thing) {

    qCDebug(GRgrp,"Translator #%02d store %p in note %d\n",getDId(),thing,note);
    noteThing[Midivent::septetFilter(note)] = thing;
    return;
}

// *** retreive the thing of a note
void *MidiGraphicTranslator::getNoteThing(septet note) const {

    qCDebug(GRgrp,"Translator #%02d retrive thing of note %d = %p\n",getDId(),note,noteThing[Midivent::septetFilter(note)]);
    return(noteThing[Midivent::septetFilter(note)]);
}

// *** get Note Name
QString MidiGraphicTranslator::getNoteName(septet note) const  {
    switch (note%12)    {
        case  0: return(QStringLiteral("C")); break;
        case  1: return(QStringLiteral("C#")); break;
        case  2: return(QStringLiteral("D")); break;
        case  3: return(QStringLiteral("Eb")); break;
        case  4: return(QStringLiteral("E")); break;
        case  5: return(QStringLiteral("F")); break;
        case  6: return(QStringLiteral("F#")); break;
        case  7: return(QStringLiteral("G")); break;
        case  8: return(QStringLiteral("G#")); break;
        case  9: return(QStringLiteral("A")); break;
        case 10: return(QStringLiteral("Bb")); break;
        case 11: return(QStringLiteral("B")); break;
        default:
            qCCritical(GUtru,"Translator #%02d unknown note\n",getDId());
    }
    return(QStringLiteral(" H"));
}

// *** clean Graphic Layer = remove all item of the graphic Layer from the scene
void MidiGraphicTranslator::cleanGraphicLayer(void) {

    qCDebug(GRgrp,"Translator #%02d decision to clean the whole graphicLayer %p\n",getDId(),graphicLayer);
    QList <QGraphicsItem *> itemList = graphicLayer->childItems();
    qCDebug(GRgrp,"Translator #%02d graphicLayer %p had %d items before cleaning\n",getDId(),graphicLayer,itemList.size());
    for (int i = 0; i < itemList.size(); i++) {
        //graphicLayer->removeFromGroup(itemList.at(i));
        (graphicLayer->scene())->removeItem(itemList.at(i));
    }
    itemList = graphicLayer->childItems();
    qCDebug(GRgrp,"Translator #%02d graphicLayer %p has now %d items after\n",getDId(),graphicLayer,itemList.size());
}

// *** clean remove every results of previously received Midivent
void MidiGraphicTranslator::cleanMidiventContext(void) {

    cleanGraphicLayer();
    nbNoteCurrentlyOn = 0;
    nbRecievedNoteOn = 0;
    sumReceivedNoteOn = 0;
    averageReceivedNoteOn = 64;
    for (int i = 0; i < 128; i++)   {
        if (noteThing[i] != 0)  {
            noteThing[i] = 0;
        }
    }
}


// *** process the reception of a midivent
void MidiGraphicTranslator::receiveMidivent(Midivent *pevt)
{
    //char bla[64];
    QGraphicsTextItem *pText = new  QGraphicsTextItem;
    QPointF where = generateRandomWorldCoordinates();
    QList <QGraphicsItem *> itemList;
    QGraphicsItem *item = 0;

    qCDebug(Mvent,"Translator #%02d (%p) receiving Midivent %p\n",getDId(),this,pevt);
    switch (pevt->getType())  {
        case None:
            qCDebug(Mvent,"Translator #%02d Midivent is \"None\"\n",getDId()); break;
        case NoteOn:
            newNoteOnAnalysis(pevt->getNote(), pevt->getVelocity());
            qCDebug(Mvent,"Translator #%02d Midivent is \"NoteOn %3d\" (%d notes currently on)\n",getDId(),pevt->getNote(),nbNoteCurrentlyOn);
            processNoteOn(pevt->getNote(), pevt->getVelocity());
            break;
        case NoteOff:
            newNoteOffAnalysis(pevt->getNote(), pevt->getVelocity());
            qCDebug(Mvent,"Translator #%02d Midivent is \"NoteOff %3d\" (%d notes currently on)\n",getDId(),pevt->getNote(),nbNoteCurrentlyOn);
            processNoteOff(pevt->getNote(), pevt->getVelocity());
            break;
        default:
            qCDebug(Mvent,"Translator #%02d unknown \"%d\", \"%d\"\n",getDId(),pevt->getType(),pevt->getNote());
    }
}


// *** first, let's analyse consequences of a new noteOn
void MidiGraphicTranslator::newNoteOnAnalysis(septet note, septet velocity)   {
    qCDebug(Mvent,"Translator #%02d MidiGraphicTranslator::newNoteOnAnalysis(%d,%d)\n",getDId(),note, velocity);
    ++nbRecievedNoteOn;
    ++nbNoteCurrentlyOn;
    sumReceivedNoteOn += (int)(note);
    averageReceivedNoteOn = (septet)(sumReceivedNoteOn / nbRecievedNoteOn);
    return;
}

// *** transform a Midivent noteOn in graphic action
void MidiGraphicTranslator::processNoteOn(septet note, septet velocity)   {

    qCDebug(GRgrp,"Translator#%02d MidiGraphicTranslator::processNoteOn updates Translator graphic Layer %p\n",getDId(),graphicLayer);
    QGraphicsTextItem *pText = new  QGraphicsTextItem;
    QPointF where = generateRandomWorldCoordinates();
    qCDebug(GRgrp,"Translator #%02d drawing text %p in (%+3.1f,%+3.1f)\n",getDId(),pText,where.x(),where.y());
    sprintf(bla,"Translator #%02d",getDId());
    pText->setPlainText(QString(bla));
    pText->setFont(QFont("Impact",20));
    pText->setPos(where);
    QRectF englob = pText->boundingRect();
    pText->moveBy(-englob.width()/2.0,-englob.height()/2.0);
    qCDebug(GRgrp,"Translator #%02d the graphicLayer scene is %p\n",getDId(),graphicLayer->scene());
    qCDebug(GRgrp,"Translator #%02d add text %p to group %p\n",getDId(),pText,graphicLayer);
    graphicLayer->addToGroup((QGraphicsItem *)(pText));
    qCDebug(GRgrp,"Translator #%02d store the item in its scene %p\n",getDId(),pText->scene());
    setNoteThing(note,(void *)((QGraphicsItem *)(pText)));
    QList <QGraphicsItem *> itemList = graphicLayer->childItems();
    qCDebug(GRgrp,"Translator #%02d (%p) graphicLayer %p has %d items\n",getDId(),this,graphicLayer,itemList.size());
    return;
}


// *** first, let's analyse consequences of a new noteOff
void MidiGraphicTranslator::newNoteOffAnalysis(septet note, septet velocity)   {
    qCDebug(Mvent,"Translator #%02d MidiGraphicTranslator::newNoteOffAnalysis(%d,%d)\n",getDId(), note, velocity);
    --nbNoteCurrentlyOn;
    if (nbNoteCurrentlyOn < 0)   {
        nbNoteCurrentlyOn = 0;
        qCDebug(Mvent,"Translator #%02d nb notes currently on corrected to %d)\n",internalId2displayId(instanceId),nbNoteCurrentlyOn);
    }
    return;
}

// *** transform a Midivent in graphic action
void MidiGraphicTranslator::processNoteOff(unsigned char note, unsigned char velocity)   {

    QList <QGraphicsItem *> itemList;
    qCDebug(GRgrp,"Translator #%02d MidiGraphicTranslator::processNoteOff updates Translator graphic Layer %p\n",getDId(),graphicLayer);

    QGraphicsItem *item = (QGraphicsItem *)(getNoteThing(note));
    if (item == 0)  {
        qCWarning(GRgrp,"Translator #%02d (%p) retreive NULL graphics item in noteThing for note %d\n",getDId(),this,note);
        // cleanGraphicLayer();
        return;
    }
    qCDebug(GRgrp,"Translator #%02d the item scene is %p\n",getDId(),item->scene());
    qCDebug(GRgrp,"Translator #%02d the graphicLayer scene is %p\n",getDId(),graphicLayer->scene());
    qCDebug(GRgrp,"Translator #%02d  (%p) remove item %p from graphicLayer %p\n",getDId(),this,item,graphicLayer);
    setNoteThing(note,0);
    (graphicLayer->scene())->removeItem(item);
    itemList = graphicLayer->childItems();
    qCDebug(GRgrp,"Translator #%02d graphicLayer %p has now %d items after\n",getDId(),graphicLayer,itemList.size());
    return;
}
