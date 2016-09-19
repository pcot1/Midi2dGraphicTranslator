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
    PRINTF(("entering scanRtMidiInPortsNames\n"));

    qRegisterMetaType<MidiMessage>("MidiMessage");

    try {
        nbMidiPorts = pRtMidiPort->getPortCount();
    }
    catch (RtMidiError &error ) {
        QMessageBox::information(widget,"Error getting number of Midi Ports",error.what());
        exit(EXIT_FAILURE);
    }
    PRINTF(("rtMidiIn has found %d ports\n",nbMidiPorts));
    for (unsigned int i = 0; i < nbMidiPorts; i++) {
        try {
            existingMidiPortsNames.append((pRtMidiPort->getPortName(i)).c_str());
        }
        catch (RtMidiError &error ) {
            QMessageBox::information(widget,"Error getting a Midi Port name",error.what());
            exit( EXIT_FAILURE );
        }
    }
    return((int)(nbMidiPorts));
}

// *** static function to give as callback to RtMidi lib
void  RtMidiPorts::rtMidiCallBack(double deltatime, std::vector<unsigned char> *message, void *userData)
{
   int midiPortIndex = (int)(userData);
   PRINTF(("RtMidiPorts::rtMidiCallBack : recieving a midi message from port %d\n",midiPortIndex));
   QMetaObject::invokeMethod(theRtMidiPortsManager, "receiveMidiMessage",  Qt::QueuedConnection,  Q_ARG(MidiMessage,*message),  Q_ARG(int, midiPortIndex));
   //QMetaObject::invokeMethod(theRtMidiPortsManager, "receiveMidiMessage",  Qt::QueuedConnection);
    /*
   MidiPortConsumers *p = (MidiPortConsumers *)(userData);
   p->printObject();
   int nbMidiSources = p->getNbMidiPortConsumers();
   PRINTF(("RtMidiInPorts::rtMidiCallBack propagate midi message to %1d MidiSources\n",nbMidiSources));
   for (int i = 0; i < nbMidiSources; i++) {
       MidiSource *receiver = (MidiSource *)(p->getMidiPortConsumer(i));
       receiver->receiveMidiMessage(message);
   }
   */
   //grDispl->drawSomething();
   return;
}

// *** Constructor
RtMidiPorts::RtMidiPorts() : QLabel("discovering Midi Ports ...")
{
    theRtMidiPortsManager = this;                                   // now a rtMidiPortsManager does exist !

                                                                    // first, there is nothing
    for (int i = 0; i < nbMaxRtMidiPorts; i++) {
         rtMidiPorts[i] = 0;                                          // no  RtMidiIn
         rtMidiPortConsumers[i] = 0;                                    // no rtMidiPortConsumers
    }
    this->show();
                                                                    // create a first RtMidiIn
    nbRtMidiPorts = 1;
    PRINTF(("create rtMidiInPorts[0]\n"));
    try {
        rtMidiPorts[0] = new RtMidiIn();
    }
    catch (RtMidiError &error ) {
        QMessageBox::information(this,"Error creating first RtMidiIn",error.what());
        exit(EXIT_FAILURE);
    }
                                                                    // discovers the exiting Input Midi Ports
    int nbExistingMidiPorts = scanRtMidiPortsNames(this,rtMidiPorts[0],rtMidiPortsNames);
    //int nbExistingMidiPorts = scanRtMidiPortsNames(rtMidiPorts[0],rtMidiPortsNames);
    for (int i = 1; i < nbExistingMidiPorts; i++) {
        PRINTF(("create rtMidiInPorts[%1d]\n",i));
        try {
            rtMidiPorts[i] = new RtMidiIn();
        }
        catch (RtMidiError &error ) {
            QMessageBox::information(this,"Error creating other RtMidiIn",error.what());
            exit(EXIT_FAILURE);
        }
    }
    nbRtMidiPorts = nbExistingMidiPorts;
                                                                    // create corresponding rtMidiPortConsumers
    for (int i = 0; i < nbRtMidiPorts; i++) {
         PRINTF(("add RtMidiPortConsumers in rtMidiInPorts[%1d]\n",i));
         rtMidiPortConsumers[i] = new MidiPortConsumers();
    }
                                                                    // open RtMidi ports and set callback
    startUseOfRtMidiPorts();
}

// *** Destructor
RtMidiPorts::~RtMidiPorts()
{
    for (int i = 0; i < nbRtMidiPorts; i++)
         rtMidiPorts[i] = 0;
}

// *** private: open existing RtMidiIn and set callback
void RtMidiPorts::startUseOfRtMidiPorts(void)
{
    //char bla[128];
    PRINTF((">>> open the %1d existing RtMidiIn\n",nbRtMidiPorts));
    for (int i = 0; i < nbRtMidiPorts; i++) {
                                                                    // open the Midi Input port corresponding index
        PRINTF(("    open MidiPort %1d\n",i));
        try {
            rtMidiPorts[i]->openPort(i);
        }
        catch (RtMidiError &error ) {
            sprintf(bla,"Error Openning Port %1d",i);
            QMessageBox::information(this,bla,error.what());
            exit(EXIT_FAILURE);
        }
                                                                    // give the callback to RtMidi lib
                                                                        // the call back should be a static function not a method
                                                                        // userdata should contain the list of MidiSources
        PRINTF(("    associates callback for MidiPort %1d\n",i));
        (rtMidiPortConsumers[i])->printObject();
        try {
            rtMidiPorts[i]->setCallback(&RtMidiPorts::rtMidiCallBack,(void *)(i));
        }
        catch (RtMidiError &error ) {
            sprintf(bla,"Error set callback for MidiPort %1d",i);
            QMessageBox::information(this,bla,error.what());
            exit( EXIT_FAILURE );
        }
    }
    return;
}


// *** Debug printer
void RtMidiPorts::printObject(void) const
{
    PRINTF((">>> print rtMidiInPorts contents\n"));

    PRINTF(("    nbRtMidiPorts = %2d\n",nbRtMidiPorts));
}

// *** receive MidiMessage method (not static) in GUI Thread
void  RtMidiPorts::receiveMidiMessage(MidiMessage message, int midiPortIndex)
{
   PRINTF(("RtMidiPorts::receiveMidiMessage : recieving a midi message from port %d\n",midiPortIndex));
   MidiPortConsumers *p = getConsumersOfMidiPortIndex(midiPortIndex);
   p->printObject();
   int nbMidiSources = p->getNbMidiPortConsumers();
   PRINTF(("RtMidiPorts::rtMidiCallBack propagate midi message to %1d MidiSources\n",nbMidiSources));
   for (int i = 0; i < nbMidiSources; i++) {
       MidiSource *receiver = (MidiSource *)(p->getMidiPortConsumer(i));
       receiver->receiveMidiMessage(&message);
   }
   return;
}
/*
void  RtMidiPorts::receiveMidiMessage(void)
{
   int midiPortIndex = 0;
   std::vector<unsigned char> *midimsg = new std::vector<unsigned char>;
   midimsg->push_back((unsigned char)(0b10010000));
   midimsg->push_back((unsigned char)(12));
   midimsg->push_back((unsigned char)(100));
   PRINTF(("RtMidiPorts::receiveMidiMessage : recieving a midi message from port %d\n",midiPortIndex));
   MidiPortConsumers *p = getConsumersOfMidiPortIndex(midiPortIndex);
   p->printObject();
   int nbMidiSources = p->getNbMidiPortConsumers();
   PRINTF(("RtMidiPorts::rtMidiCallBack propagate midi message to %1d MidiSources\n",nbMidiSources));
   for (int i = 0; i < nbMidiSources; i++) {
       MidiSource *receiver = (MidiSource *)(p->getMidiPortConsumer(i));
       //receiver->receiveMidiMessage(message);
       receiver->receiveMidiMessage(midimsg);
   }
   return;
}
*/

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



