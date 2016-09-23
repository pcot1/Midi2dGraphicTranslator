#define DEBUG
#include "rtMidiPorts.h"

// debug
//#include "graphicDisplayer.h"
//extern GraphicDisplayer *grDispl;

//debug
extern char *bla;
                                                                // at the begenning, no rtMidiPortsManager
RtMidiPorts * RtMidiPorts::theRtMidiPortsManager = 0;

// *** static function to get the existing Midi In Ports Names
int  RtMidiPorts::scanRtMidiPortsNames(QWidget *widget, RtMidiIn *pRtMidiPort, QStringList &existingMidiPortsNames)
//int  RtMidiPorts::scanRtMidiPortsNames(RtMidiIn *pRtMidiPort, QStringList &existingMidiPortsNames)
{
    unsigned int nbMidiPorts;
    qCDebug(Minit,"entering scanRtMidiPortsNames(%p)\n",pRtMidiPort);
    qCInfo(Minit,"Scan Midi Port names\n");
    qCDebug(Minit,"Registering MetaType \"MidiMessage\" for future usage by QMetaObject::invokeMethod\n");
    qRegisterMetaType<MidiMessage>("MidiMessage");

    try {
        nbMidiPorts = pRtMidiPort->getPortCount();
    }
    catch (RtMidiError &error ) {
        QMessageBox::information(widget,"Error getting number of Midi Ports\n",error.what());
        exit(EXIT_FAILURE);
    }
    qCDebug(Minit,"rtMidiIn has found %d ports\n",nbMidiPorts);
    for (unsigned int i = 0; i < nbMidiPorts; i++) {
        try {
            existingMidiPortsNames.append((pRtMidiPort->getPortName(i)).c_str());
        }
        catch (RtMidiError &error ) {
            QMessageBox::information(widget,"Error getting a Midi Port name\n",error.what());
            exit( EXIT_FAILURE );
        }
    }
    qCDebug(Minit,"scanRtMidiPortsNames terminated, returning the number of Midi Ports found %d\n",nbMidiPorts);
    return((int)(nbMidiPorts));
}

// *** static function to give as callback to RtMidi lib
void  RtMidiPorts::rtMidiCallBack(double deltatime, std::vector<unsigned char> *message, void *userData)
{
   int midiPortIndex = (int)(userData);
   qCDebug(Mmsg,"rtMidi callback: entering static RtMidiPorts::rtMidiCallBack to process a message from Midi Port %d\n",midiPortIndex);
   qCDebug(Mmsg,"QMetaObject::invokeMethod this->receiveMidiMessage for processing by gui thread\n");
   QMetaObject::invokeMethod(theRtMidiPortsManager, "receiveMidiMessage",  Qt::QueuedConnection,  Q_ARG(MidiMessage,*message),  Q_ARG(int, midiPortIndex));
   qCDebug(Mmsg,"terminating static RtMidiPorts::rtMidiCallBack\n");
   return;
}

// *** Constructor
RtMidiPorts::RtMidiPorts() : QLabel("  discovering Midi Ports ...")
{
    qCDebug(Minit,"entering RtMidiPorts constructor\n");
    theRtMidiPortsManager = this;                                   // now a rtMidiPortsManager does exist !
    qCDebug(Minit,"centralized RtMidiPortsManager exists and is %p\n",this);

    setFixedSize(400,60);
    setFont(QFont("Staccato222 BT",30));
    move(0,0);
    this->show();
                                                                    // first, there is nothing
    for (int i = 0; i < nbMaxRtMidiPorts; i++) {
         rtMidiPorts[i] = 0;                                          // no  RtMidiIn
         rtMidiPortConsumers[i] = 0;                                    // no rtMidiPortConsumers
    }
                                                                    // create a first RtMidiIn
    nbRtMidiPorts = 1;
    qCDebug(Minit,"create rtMidiInPorts[0]\n");
    try {
        rtMidiPorts[0] = new RtMidiIn();
    }
    catch (RtMidiError &error ) {
        QMessageBox::information(this,"Error creating first RtMidiIn\n",error.what());
        exit(EXIT_FAILURE);
    }

                                                                    // discovers the exiting Input Midi Ports
    qCDebug(Minit,"use rtMidiInPorts[0] %p to scan existing Midi In Ports\n",rtMidiPorts[0]);
    int nbExistingMidiPorts = scanRtMidiPortsNames(this,rtMidiPorts[0],rtMidiPortsNames);
    //int nbExistingMidiPorts = scanRtMidiPortsNames(rtMidiPorts[0],rtMidiPortsNames);
    qCDebug(Minit,"Found %d Midi In Ports\n",nbExistingMidiPorts);
    for (int i = 1; i < nbExistingMidiPorts; i++) {
        qCDebug(Minit,"create rtMidiInPorts[%1d] for discovered Midi In Port %d\n",i,i);
        try {
            rtMidiPorts[i] = new RtMidiIn();
        }
        catch (RtMidiError &error ) {
            QMessageBox::information(this,"Error creating other RtMidiIn\n",error.what());
            exit(EXIT_FAILURE);
        }
    }
    nbRtMidiPorts = nbExistingMidiPorts;
    qCDebug(Minit,"now, each existing MIDI IN Port as its dedicated rtMidiInPorts instance\n");
                                                                    // create corresponding rtMidiPortConsumers
    for (int i = 0; i < nbRtMidiPorts; i++) {
         qCDebug(Minit,"add instance of RtMidiPortConsumers in rtMidiInPorts[%1d]\n",i);
         rtMidiPortConsumers[i] = new MidiPortConsumers();
    }
                                                                    // open RtMidi ports and set callback
    qCDebug(Minit,"calling \"startUseOfRtMidiPorts\" to make these rtMidiPorts usable\n");
    startUseOfRtMidiPorts();

    //QTimer *timer = new QTimer(this);
    //QObject::connect(timer, SIGNAL(timeout()), this, SLOT(hideWindow()));
    //timer->start(3000);
    QTimer::singleShot(3000,this, SLOT(hideWindow()));
    qCDebug(Minit,"terminating RtMidiPorts constructor\n");
}

// *** Destructor
RtMidiPorts::~RtMidiPorts()
{
    qCDebug(Minit,"RtMidiPorts destructor\n");
    for (int i = 0; i < nbRtMidiPorts; i++)
         rtMidiPorts[i] = 0;
}

// *** change the window message
void RtMidiPorts::setWindowMessage(QString &msg)
{
    qCDebug(Minit,"RtMidiPorts::setWindowMessage \"%s\"\n",qPrintable(msg));
    setText(msg);
}

// *** public slot show the rtMidiPorts window
void RtMidiPorts::showWindow(void)
{
    qCDebug(Minit,"RtMidiPorts::showWindow\n");
    show();
}

// *** public slot hide the rtMidiPorts window
void RtMidiPorts::hideWindow(void)
{
    qCDebug(Minit,"RtMidiPorts::hideWindow\n");
    hide();
}

// *** private: open existing RtMidiIn and set callback
void RtMidiPorts::startUseOfRtMidiPorts(void)
{
    //char bla[128];
    qCDebug(Minit,"entering startUseOfRtMidiPorts\n");
    qCDebug(Minit,"open the %1d existing RtMidiPorts\n",nbRtMidiPorts);
    for (int i = 0; i < nbRtMidiPorts; i++) {
                                                                    // open the Midi Input port corresponding index
        qCDebug(Minit,"openning MidiPort %1d\n",i);
        try {
            rtMidiPorts[i]->openPort(i);
        }
        catch (RtMidiError &error ) {
            sprintf(bla,"Error Openning Port %1d\n",i);
            QMessageBox::information(this,bla,error.what());
            exit(EXIT_FAILURE);
        }
                                                                    // give the callback to RtMidi lib
                                                                        // the call back should be a static function not a method
                                                                        // userdata should contain the list of MidiSources
        (rtMidiPortConsumers[i])->printObject();
        qCDebug(Minit,"associates callback to rtMidiPort %1d\n",i);
        try {
            rtMidiPorts[i]->setCallback(&RtMidiPorts::rtMidiCallBack,(void *)(i));
        }
        catch (RtMidiError &error ) {
            sprintf(bla,"Error set callback for MidiPort %1d\n",i);
            QMessageBox::information(this,bla,error.what());
            exit( EXIT_FAILURE );
        }
    }
    qCDebug(Minit,"terminating startUseOfRtMidiPorts\n");
    return;
}


// *** Debug printer
void RtMidiPorts::printObject(void) const
{
    qCDebug(Minit,"printing rtMidiInPorts contents:\n");
    qCDebug(Minit,"         nbRtMidiPorts = %2d\n",nbRtMidiPorts);
}

// *** receive MidiMessage method (not static) in GUI Thread
void  RtMidiPorts::receiveMidiMessage(MidiMessage message, int midiPortIndex)
{
   qCDebug(Mmsg,"RtMidiPorts::receiveMidiMessage : recieving a midi message from port %d\n",midiPortIndex);
   MidiPortConsumers *p = getConsumersOfMidiPortIndex(midiPortIndex);
   p->printObject();
   int nbMidiSources = p->getNbMidiPortConsumers();
   qCDebug(Mmsg,"RtMidiPorts::rtMidiCallBack propagate midi message to %1d MidiSources\n",nbMidiSources);
   for (int i = 0; i < nbMidiSources; i++) {
       MidiSource *receiver = (MidiSource *)(p->getMidiPortConsumer(i));
       receiver->receiveMidiMessage(&message);
   }
   return;
}

// *** accessor
int RtMidiPorts::getNbRtMidiPorts(void)
{
    return(nbRtMidiPorts);
}

// *** accessor
MidiPortConsumers *RtMidiPorts::getConsumersOfMidiPortIndex(int id)
{
    return(rtMidiPortConsumers[id]);
}

// *** accessor
QStringList * RtMidiPorts::getRtMidiPortsNames(void)
{
    return(&rtMidiPortsNames);
}



