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
    sprintf(bla,"Midi Source #%02d",internalId2displayId(instanceId));
    setTitle(bla);
                                                                      // MidiSource Name
    //QLineEdit *MidiSourceName = new QLineEdit(bla);                         // user editable Source name
    MidiSourceName = new QLineEdit(bla);                         // user editable Source name
                                                                      // MidiPort Chooser
    //QLabel *MidiPortLabel = new QLabel("Midi Port:");
    MidiPortLabel = new QLabel("Midi Port:");
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
    allChannels = true;
    channelId = 0;
    //QLabel *MidiChannelLabel = new QLabel("Midi Channel:");
    MidiChannelLabel = new QLabel("Midi Channel:");
    QStringList *pMidiChannelNames = new QStringList("All");
    QString str = "1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16";
    pMidiChannelNames->append(str.split(','));

    MidiChannels_ComboBox = new QComboBox();
    MidiChannels_ComboBox->addItems(*pMidiChannelNames);
    MidiChannels_ComboBox->setCurrentIndex(0);
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

// *** Private Slot: changingMidiPort
void MidiSource::changingMidiPort(int newMidiPortId)
{
    PRINTF(("changingMidiPort of instance %d: (%d -> %d ) \n",instanceId,midiPortId,newMidiPortId));
    emit myMidiPortHasChanged(midiPortId,newMidiPortId);
    midiPortId = newMidiPortId;
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
    PRINTF(("recieving Midi Message %p = (%x,...)\n",message,status));
    PRINTF(("status & 0b10000000 = %x\n",(status & ((unsigned char)(0b10000000)))));
    PRINTF(("status & 0b10010000 = %x\n",(status & ((unsigned char)(0b10010000)))));

    // midi message format checks
    if (size < 3)
        return;                                                         // a "note" midi message is at least 1 byte status + 2 byte data
    unsigned char data1 = message->at(1);
    unsigned char data2 = message->at(2);
    PRINTF(("transforming Midi Message %p = (%x,%d,%d) in Midivent\n",message,status,data1,data2));
    PRINTF(("data1 & 0b10000000 = %x\n",(message->at(1) & ((unsigned char)(0b10000000)))));
    PRINTF(("data2 & 0b10000000 = %x\n",(message->at(2) & ((unsigned char)(0b10000000)))));
    if (! (status & ((unsigned char)(0b10000000))) )
        return;                                                         // first byte should be a "status" byte => 1st bit is 1
    if ( ((status & (unsigned char)(0b10000000)) != status) && ((status & (unsigned char)(0b10010000)) != status) )
        return;                                                         // I only consider "note" message: note off "8n" or note on "9n"
    if ((status & (unsigned char)(0b10000000)) == status)
        evtyp = NoteOff;                                                // if status is "8n", it's a note off
    if ((status & (unsigned char)(0b10010000)) == status)
        evtyp = NoteOn;                                                 // if status is "9n", it's a note on
    if ( (data1 & (unsigned char)(0b10000000)) || (data2 & (unsigned char)(0b10000000)) )
        return;                                                         // 2 following byte should be "data" ones (1st bit = 0)
                                                                    // midi channel extraction
    channel = (int)((status & ((unsigned char)(0b00001111))));  // midi channel is 4 LSB fo the status byte

    if (data2 == 0)
        evtyp = NoteOff;                                                // velocity = 0 => noteOff (even if status "9n")
    switch (evtyp)  {                                               // counting current nb of notes on for the Midi Port
       case NoteOn:
            ++nbMidiPortNoteOn; break;
       case NoteOff:
            --nbMidiPortNoteOn; break;
       default:
            ;
    }

    if (allChannels || (channel == channelId))
        toBeSent = true;                                                // Source is forwarding this channel

    switch (evtyp)  {                                              // counting current nb of notes on for the Midi Source (port+channel)
       case NoteOn:
            ++nbMidiSourceNoteOn; break;
       case NoteOff:
            --nbMidiSourceNoteOn; break;
       default:
            ;
    }

    if (nbConsumers <= 0)
        return;                                                         // nothing to forward is no consumers
                                                                    // forwarding Midi event in a Midivent object to consumers
    PRINTF(("just before sending Midivent to %1d comsumers (toBeSent = %s)\n",nbConsumers,(toBeSent?"true":"false")));
    if (toBeSent) {
        Midivent fevt(evtyp,data1);
        //MidiGraphicTranslator *p = 0;
        for (int i = 0; i < nbConsumers; i++) {
            PRINTF(("calling receiveMidivent of Consumer #%02d (0x%p) with ", internalId2displayId(i),consumers[i]));
            fevt.printObject();
            //p = (MidiGraphicTranslator *)(consumers[i]);
            //PRINTF(("Consumer considered as MidiGraphicTranslator is  0x%p \n", p));
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
    PRINTF(("addConsumer(%p) nbConsumers of instance %d was %d\n",newConsumer,instanceId,nbConsumers));
    if (nbConsumers >= nbMaxConsumers)
           return(1);
    consumers[nbConsumers] = newConsumer;
    ++nbConsumers;
    PRINTF(("nbConsumers of %d is %d\n",instanceId,nbConsumers));
    return(0);
}

int  MidiSource::removeConsumer(MidiventConsumer *oldConsumer)
{
    PRINTF(("removeConsumer(%p), nbConsumers of %d was %d\n",oldConsumer,instanceId,nbConsumers));
    bool NotFound = true;
    int idToRemove = nbConsumers;
    for (int i = 0; i < nbConsumers; i++)
        if (consumers[i] == oldConsumer)    {
            idToRemove = i;
            NotFound = false;
            break;
        }
    PRINTF(("oldConsumer notFound = %s, idToremove = %d\n",(NotFound ? "true" : "false"),idToRemove));
    if (NotFound)
        return(1);

    for (int i = idToRemove; i < (nbConsumers-1); i++)
        consumers[i] = consumers[i+1];
    consumers[nbConsumers-1] = 0;
    --nbConsumers;
    PRINTF(("nbConsumers of %d is %d\n",instanceId,nbConsumers));
    return(0);
}
