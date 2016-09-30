#define DEBUG
#include "midiSource.h"
//debug
#include "graphicDisplayer.h"
extern GraphicDisplayer *grDispl;

                                                                        // managing index rules
inline int displayId2internalId(int i) {return(i-1);}                       // first internalId is 0
inline int internalId2displayId(int i) {return(i+1);}                       // first displayId is

int MidiSource::nbCreated = 0;

MidiSource::MidiSource(QWidget *pparent, QStringList *availableMidiPortNames) : QGroupBox(pparent)
{
                                                                        // set Source instanceId and title
    char bla[128];
    instanceId = nbCreated++;
    nbMidiPortNoteOn=0;
    nbMidiSourceNoteOn=0;
                                                                        // colorize the MidiSource Widget
    this->setPalette(QPalette(QColor(230,220,220,255)));
    this->setAutoFillBackground(true);


    sprintf(bla,"Midi Source #%02d",internalId2displayId(instanceId));
    setTitle(bla);
                                                                      // MidiSource Name
    //QLineEdit *MidiSourceName = new QLineEdit(bla);                         // user editable Source name
    MidiSourceName = new QLineEdit(bla);                         // user editable Source name
                                                                      // MidiPort Chooser
    //QLabel *MidiPortLabel = new QLabel("Midi Port:");
    MidiPortLabel = new QLabel(this);
    MidiPortLabel->setPixmap(QPixmap("./MIDI-icon.png"));

                                                                            // gui knows about MidiPorts from its RtMidi Manager
    MidiPorts_ComboBox = new QComboBox();
    MidiPorts_ComboBox->addItems(*availableMidiPortNames);
    midiPortId = 0;                                                         // if no Midi Port openned, appli has already stopped
    MidiPorts_ComboBox->setCurrentIndex(midiPortId);                        // force the firts Midi Port (at least one exists)
    QObject::connect(MidiPorts_ComboBox,SIGNAL(currentIndexChanged(int)),this,SLOT(changingMidiPort(int)));
    QObject::connect(this,SIGNAL(myMidiPortHasChanged(int,int)),pparent,SLOT(aMidiSourceHasChangedItsMidiPort(int,int)));
    //QHBoxLayout *MidiPortsLayout = new QHBoxLayout;
    MidiPortsLayout = new QHBoxLayout;
    MidiPortsLayout->addWidget(MidiPortLabel);
    MidiPortsLayout->addWidget(MidiPorts_ComboBox);
                                                                        // MidiChannel Chooser
    allChannels = false;
    midiChannelId = instanceId;
    //QLabel *MidiChannelLabel = new QLabel("Midi Channel:");
    MidiChannelLabel = new QLabel("Midi Channel:");
    QStringList *pMidiChannelNames = new QStringList("All");
    QString str = "1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16";
    pMidiChannelNames->append(str.split(','));

    MidiChannels_ComboBox = new QComboBox();
    MidiChannels_ComboBox->addItems(*pMidiChannelNames);
    MidiChannels_ComboBox->setCurrentIndex(internalId2displayId(instanceId));
    QObject::connect(MidiChannels_ComboBox,SIGNAL(currentIndexChanged(int)),this,SLOT(changingMidiChannel(int)));
    //QHBoxLayout *MidiChannelsLayout = new QHBoxLayout;
    MidiChannelsLayout = new QHBoxLayout;
    MidiChannelsLayout->addWidget(MidiChannelLabel);
    MidiChannelsLayout->addWidget(MidiChannels_ComboBox);

                                                                     // GroupBox Vertical Layout
    //QVBoxLayout *MidiSourceGroupLayout = new QVBoxLayout;
    MidiSourceGroupLayout = new QVBoxLayout;
    MidiSourceGroupLayout->addWidget(MidiSourceName);
    MidiSourceGroupLayout->addLayout(MidiPortsLayout);
    MidiSourceGroupLayout->addLayout(MidiChannelsLayout);
    this->setLayout(MidiSourceGroupLayout);
                                                                     // consumers initialisation
    nbConsumers = 0;
    for (int i = 0; i < nbMaxConsumers; i++)
        consumers[i] = 0;
}

MidiSource::~MidiSource()
{
    --nbCreated;
}

// *** protected: do remove items from graphic Layer of translators which are consumers
void MidiSource::cleanMidiventContextOfConsumers(void) const {

    for (int i = 0; i < nbConsumers; i++)   {
        consumers[i]->cleanMidiventContext();
    }
}


// *** Private Slot: changingMidiPort
void MidiSource::changingMidiPort(int newMidiPortId)
{
    qCDebug(GUmsu,"changingMidiPort of instance %d: (%d -> %d ) \n",instanceId,midiPortId,newMidiPortId);
    cleanMidiventContextOfConsumers();
    emit myMidiPortHasChanged(midiPortId,newMidiPortId);
    midiPortId = newMidiPortId;
}

// *** Private Slot: changingMidiChannel
void MidiSource::changingMidiChannel(int comboBoxId)
{
    qCDebug(GUmsu,"changingMidiChannel of instance %d: ",instanceId);
    cleanMidiventContextOfConsumers();
    if (comboBoxId == 0 )    {
        allChannels = true;
        midiChannelId = 0;
        qCDebug(GUmsu,"allChannels !\n",instanceId,midiChannelId,displayId2internalId(comboBoxId));
        return;
    }
    qCDebug(GUmsu,"(%d -> %d )\n",instanceId,midiChannelId,displayId2internalId(comboBoxId));
    allChannels = false;
    midiChannelId = displayId2internalId(comboBoxId);
}

// *** Public Slot: updateAvailableMidiPortList
void MidiSource::updateAvailableMidiPortList(QStringList *midiPortNames)
{
}

void  MidiSource::receiveMidiMessage(std::vector<unsigned char> *message)
{
    bool toBeSent = false;
    MidiventTypeName evtyp = None;
    int size = message->size();
    int channel = 0x0;
    unsigned char status = message->at(0);
    //qCDebug(Mmsg,"recieving Midi Message %p = (%x,...)\n",message,status);
    //qCDebug(Mmsg,"status & 0b10000000 = %x\n",(status & ((unsigned char)(0b10000000))));
    //qCDebug(Mmsg,"status & 0b10010000 = %x\n",(status & ((unsigned char)(0b10010000))));

    // midi message format checks
    if (size < 3)   {
        qCDebug(Mmsg,"drop Midi Message status 0x%x because size %d\n",status,size);
        return;                                                         // a "note" midi message is at least 1 byte status + 2 byte data
    }
    unsigned char data1 = message->at(1);
    unsigned char data2 = message->at(2);
    //qCDebug(Mmsg,"data1 & 0b10000000 = %x\n",(message->at(1) & ((unsigned char)(0b10000000))));
    //qCDebug(Mmsg,"data2 & 0b10000000 = %x\n",(message->at(2) & ((unsigned char)(0b10000000))));
    if (! (status & ((unsigned char)(0b10000000))) )    {
        qCWarning(Mmsg,"MidiSource #%02d: drop Midi Message(0x%x,%d,%d) because status MSB is not 1\n",internalId2displayId(instanceId),status,data1,data2);
        return;                                                         // first byte should be a "status" byte => 1st bit is 1
    }
    if ( ((status & (unsigned char)(0b10001111)) != status) && ((status & (unsigned char)(0b10011111)) != status) ) {
        qCDebug(Mmsg,"MidiSource #%02d: drop Midi Message(0x%x,%d,%d) because status is not 0x8n or 0x9n\n",internalId2displayId(instanceId),status,data1,data2);
        return;                                                         // I only consider "note" message: note off "8n" or note on "9n"
    }
    qCInfo(Mm2v,"MidiSource #%02d: transforming Midi Message %p = (0x%x,%d,%d) in Midivent\n",internalId2displayId(instanceId),message,status,data1,data2);
    /*
    if ((status & (unsigned char)(0b10001111)) == status)
        evtyp = NoteOff;                                                // if status is "8n", it's a note off
    if ((status & (unsigned char)(0b10011111)) == status)
        evtyp = NoteOn;                                                 // if status is "9n", it's a note on
    */
    if (status & (unsigned char)(0b00010000))
        evtyp = NoteOn;                                                // if status is "9n", it's a note on
    else
        evtyp = NoteOff;                                               // if status is "8n", it's a note off

    if ( (data1 & (unsigned char)(0b10000000)) || (data2 & (unsigned char)(0b10000000)) ) {
        qCDebug(Mmsg,"MidiSource #%02d: drop Midi Message(0x%x,%d,%d) because data has MSB = 1\n",internalId2displayId(instanceId),status,data1,data2);
        return;                                                         // 2 following byte should be "data" ones (1st bit = 0)
    }
    if (data2 == 0)
        evtyp = NoteOff;                                                // velocity = 0 => noteOff (even if status "9n")
    switch (evtyp)  {                                               // counting current nb of notes on for the Midi Port
       case NoteOn:
            qCDebug(Mm2v,"MidiSource #%02d: Midi Message(0x%x,%d,%d) is identified as NoteOn\n",internalId2displayId(instanceId),status,data1,data2);
            break;
       case NoteOff:
            qCDebug(Mm2v,"MidiSource #%02d: Midi Message(0x%x,%d,%d) is identified as NoteOn\n",internalId2displayId(instanceId),status,data1,data2);
            break;
       default:
            qCDebug(Mm2v,"MidiSource #%02d: Midi Message(0x%x,%d,%d) is NOT identified (None)\n",internalId2displayId(instanceId),status,data1,data2);
            ;
    }
                                                                        // midi channel extraction
    channel = (int)((status & ((unsigned char)(0b00001111))));  // midi channel is 4 LSB fo the status byte
    qCDebug(Mm2v,"MidiSource #%02d: Channel identified = %d (allChannels %s, midiChannelId %d)\n",internalId2displayId(instanceId),channel,(allChannels ? "true" : "false"),midiChannelId);

    switch (evtyp)  {                                               // counting current nb of notes on for the Midi Port
       case NoteOn:
            ++nbMidiPortNoteOn; break;
       case NoteOff:
            --nbMidiPortNoteOn;
            if (nbMidiPortNoteOn < 0)
                nbMidiPortNoteOn = 0;
            break;
       default:
            ;
    }

    if (allChannels || (channel == midiChannelId)) {
        toBeSent = true;                                                // Source is forwarding this channel
    } else {
           qCDebug(Mm2v,"MidiSource #%02d: drop Midi Message(0x%x,%d,%d) because channel %d rejected\n",internalId2displayId(instanceId),status,data1,data2,channel);
    }

    switch (evtyp)  {                                              // counting current nb of notes on for the Midi Source (port+channel)
       case NoteOn:
            ++nbMidiSourceNoteOn; break;
       case NoteOff:
            --nbMidiSourceNoteOn;
            if (nbMidiSourceNoteOn < 0)
                nbMidiSourceNoteOn = 0;
            break;
       default:
            ;
    }
    qCDebug(Mm2v,"nbMidiPortNoteOn %d, nbMidiSourceNoteOn %d\n",nbMidiPortNoteOn,nbMidiSourceNoteOn);

    if (nbConsumers <= 0)   {
        qCDebug(Mm2v,"MidiSource #%02d: drop Midi Message(0x%x,%d,%d) because NO resgistered consumers\n",internalId2displayId(instanceId),status,data1,data2);
        return;                                                         // nothing to forward is no consumers
    }
                                                                    // forwarding Midi event in a Midivent object to consumers
    qCDebug(Mvent,"just before sending Midivent to %1d comsumers (toBeSent = %s)\n",nbConsumers,(toBeSent?"true":"false"));
    if (toBeSent) {
        Midivent fevt(evtyp,data1,data2);
        //MidiGraphicTranslator *p = 0;
        for (int i = 0; i < nbConsumers; i++) {
            qCDebug(Mvent,"calling receiveMidivent of Consumer #%02d (%p) with ", internalId2displayId(i),consumers[i]);
            fevt.printObject();
            //p = (MidiGraphicTranslator *)(consumers[i]);
            //PRINTF(("Consumer considered as MidiGraphicTranslator is  %p \n", p));
            //p->receiveMidivent(&fevt);
            consumers[i]->printObject();
            consumers[i]->receiveMidivent(&fevt);
            //grDispl->drawSomething();
        }
    }

    return;
}

// *** accessor
int  MidiSource::getRtMidiInPortIndex(void) const
{
    return(MidiPorts_ComboBox->currentIndex());
}

// *** accessor
int  MidiSource::getNbConsumers(void) const
{
    return(nbConsumers);
}

// *** accessor
MidiventConsumer *MidiSource::getConsumer(int i) const
{
    return(consumers[i]);
}


int  MidiSource::addConsumer(MidiventConsumer *newConsumer)
{
    qCDebug(Minit,"addConsumer(%p) nbConsumers of instance %d was %d\n",newConsumer,instanceId,nbConsumers);
    if (nbConsumers >= nbMaxConsumers)
           return(1);
    consumers[nbConsumers] = newConsumer;
    ++nbConsumers;
    qCDebug(Minit,"nbConsumers of %d is %d\n",instanceId,nbConsumers);
    return(0);
}

int  MidiSource::removeConsumer(MidiventConsumer *oldConsumer)
{
    qCDebug(Minit,"removeConsumer(%p), nbConsumers of %d was %d\n",oldConsumer,instanceId,nbConsumers);
    bool NotFound = true;
    int idToRemove = nbConsumers;
    for (int i = 0; i < nbConsumers; i++)
        if (consumers[i] == oldConsumer)    {
            idToRemove = i;
            NotFound = false;
            break;
        }
    qCDebug(Minit,"oldConsumer notFound = %s, idToremove = %d\n",(NotFound ? "true" : "false"),idToRemove);
    if (NotFound)
        return(1);

    for (int i = idToRemove; i < (nbConsumers-1); i++)
        consumers[i] = consumers[i+1];
    consumers[nbConsumers-1] = 0;
    --nbConsumers;
    qCDebug(Minit,"nbConsumers of %d is %d\n",instanceId,nbConsumers);
    return(0);
}
