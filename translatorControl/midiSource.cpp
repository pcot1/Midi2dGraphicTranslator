#define DEBUG
#include "MidiSource.h"

                                                                        // managing index rules
inline int displayId2internalId(int i) {return(i-1);}                       // first internalId is 0
inline int internalId2displayId(int i) {return(i+1);}                       // first displayId is

int MidiSource::nbCreated = 0;

MidiSource::MidiSource(QWidget *pparent) : QGroupBox(pparent)
{
                                                                        // set Source instanceId and title
    char bla[128];
    instanceId = nbCreated++;
    nbMidiPortNoteOn=0;
    nbMidiSourceNoteOn=0;
    sprintf(bla,"Midi Source #%02d",internalId2displayId(instanceId));
    setTitle(bla);
                                                                      // MidiSource Name
    QLineEdit *MidiSourceName = new QLineEdit(bla);                         // user editable Source name
                                                                      // MidiPort Chooser
    QLabel *MidiPortLabel = new QLabel("Midi Port:");
    QStringList *pMidiPortNames = scanMidiInputPorts();
    //MidiPorts_ComboBox = new QComboBox(MidiSourceGroup);
    MidiPorts_ComboBox = new QComboBox();
    MidiPorts_ComboBox->addItems(*pMidiPortNames);
    QHBoxLayout *MidiPortsLayout = new QHBoxLayout;
    MidiPortsLayout->addWidget(MidiPortLabel);
    MidiPortsLayout->addWidget(MidiPorts_ComboBox);
                                                                      // MidiChannel Chooser
    allChannels = true;
    channelId = 0;
    QLabel *MidiChannelLabel = new QLabel("Midi Channel:");
    QStringList *pMidiChannelNames = new QStringList("All");
    QString str = "1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16";
    pMidiChannelNames->append(str.split(','));

    MidiChannels_ComboBox = new QComboBox();
    MidiChannels_ComboBox->addItems(*pMidiChannelNames);
    MidiChannels_ComboBox->setCurrentIndex(0);
    QHBoxLayout *MidiChannelsLayout = new QHBoxLayout;
    MidiChannelsLayout->addWidget(MidiChannelLabel);
    MidiChannelsLayout->addWidget(MidiChannels_ComboBox);

                                                                     // GroupBox Vertical Layout
    QVBoxLayout *MidiSourceGroupLayout = new QVBoxLayout;
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

QStringList* MidiSource::scanMidiInputPorts()
{
    RtMidiIn    *midiin = 0;
    QStringList *pPortNames = new QStringList();
    QString affichable;

                                                                    // RtMidiIn constructor
    try {
        midiin = new RtMidiIn();
    }
    catch (RtMidiError &error ) {
        QMessageBox::information(this,"Error",error.what());
        exit( EXIT_FAILURE );
    }
                                                                    // Check inputs.
    unsigned int nPorts = midiin->getPortCount();
    //QTextStream(&affichable) << "There are " << nPorts << " MIDI input sources available";
    //QMessageBox::information(this,"",affichable);
    for (unsigned int i=0; i<nPorts; i++) {
        try {
            pPortNames->append((midiin->getPortName(i)).c_str());
        }
        catch (RtMidiError &error ) {
            QMessageBox::information(this,"Error",error.what());
            delete midiin;
        }
    }
    //QMessageBox::information(this,"",pPortNames->join("\n"));
    return pPortNames;
}

void  MidiSource::rtMidiCallBack(double deltatime, std::vector<unsigned char> *message, void *userData)
{
    bool toBeSent = false;
    MidiventTypeName evtyp = None;
    int size = message->size();
    int channel = 0x0;
                                                                    // midi message format checks
    if (size < 3)
        return;                                                         // a "note" midi message is at least 1 byte status + 2 byte data
    if (! (message->at(0) & ((unsigned char)(0b10000000))))
        return;                                                         // first byte should be a "status" byte => 1st bit is 1
    if (! ((message->at(0) & ((unsigned char)(0b00010000))) || (message->at(0) & ((unsigned char)(0b00100000)))))
        return;                                                         // I only consider "note" message: note off "8n" or note on "9n"
    if (message->at(0) & ((unsigned char)(0b00010000)))
        evtyp = NoteOff;                                                // if status is "8n", it's a note off
    if (message->at(0) & ((unsigned char)(0b00100000)))
        evtyp = NoteOn;                                                 // if status is "9n", it's a note on
    if ((message->at(1) & ((unsigned char)(0b10000000))) || (message->at(2) & ((unsigned char)(0b10000000))))
        return;                                                         // 2 following byte should be "data" ones (1st bit = 0)
                                                                    // midi channel extraction
    channel = (int)((message->at(0) & ((unsigned char)(0b00001111))));  // midi channel is 4 LSB fo the status byte

    if (message->at(2) == 0)
        evtyp = NoteOff;                                                // velocity = 0 => noteOff (even if status "8n")
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
    if (toBeSent) {
        Midivent fevt = Midivent(evtyp,message->at(1));
        for (int i = 0; i < nbConsumers; i++)
            consumers[i]->recieveMidivent(fevt);
    }

    return;
}

int  MidiSource::getNbConsumers(void)
{
    return(nbConsumers);
}

int  MidiSource::addConsumer(MidiConsumer *newConsumer)
{
    PRINTF(("addConsumer(%p) nbConsumers of instance %d was %d\n",newConsumer,instanceId,nbConsumers));
    if (nbConsumers >= nbMaxConsumers)
           return(1);
    consumers[nbConsumers] = newConsumer;
    ++nbConsumers;
    PRINTF(("nbConsumers of %d is %d\n",instanceId,nbConsumers));
    return(0);
}

int  MidiSource::removeConsumer(MidiConsumer *oldConsumer)
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
