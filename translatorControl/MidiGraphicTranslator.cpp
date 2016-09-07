#define DEBUG
#include "MidiGraphicTranslator.h"

                                                                        // managing index rules
inline int displayId2internalId(int i) {return(i-1);}                       // first internalId is 0
inline int internalId2displayId(int i) {return(i+1);}                       // first displayId is 1

int MidiGraphicTranslator::nbCreated = 0;

MidiGraphicTranslator::MidiGraphicTranslator(QWidget *pparent) : QGroupBox(pparent)
{
                                                                       // set Translator instanceId and title
    char bla[128];
    instanceId=nbCreated;
    ++nbCreated;
    sprintf(bla,"Graphic Translator #%02d",internalId2displayId(instanceId));
    setTitle(bla);                                                    //
                                                                      // Translator Name + Quit Button;
    sprintf(bla,"Translator #%02d",internalId2displayId(instanceId));
    MidiGraphicTranslatorName = new QLineEdit(bla);
    quit_Button = new QPushButton();
    quit_Button->setIcon(QIcon("./power-icon.png"));
    QHBoxLayout *header_Layout = new QHBoxLayout;
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
    QHBoxLayout *rendererOrder_Layout = new QHBoxLayout;
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
    PRINTF(("MidiSources_ComboBox current index %d\n",MidiSources_ComboBox->currentIndex()));
    MidiSourceId = undefinedMidiSource;
    QHBoxLayout *MidiSource_Layout = new QHBoxLayout;
    MidiSource_Layout->addWidget(MidiSource_Label);
    MidiSource_Layout->addWidget(MidiSources_ComboBox);
    QObject::connect(MidiSources_ComboBox,SIGNAL(currentIndexChanged(int)),this,SLOT(changingMidiSource(int)));
    QObject::connect(this,SIGNAL(unRegisterMidiSource(int)),pparent,SLOT(removeConsumerRequest(int)));
    QObject::connect(this,SIGNAL(registerMidiSource(int)),pparent,SLOT(addConsumerRequest(int)));
                                                                      // GroupBox Vertical Layout
    QVBoxLayout *MidiGraphicTranslatorGroupLayout = new QVBoxLayout;
    MidiGraphicTranslatorGroupLayout->addLayout(header_Layout);
    MidiGraphicTranslatorGroupLayout->addLayout(rendererOrder_Layout);
    MidiGraphicTranslatorGroupLayout->addLayout(MidiSource_Layout);
    setLayout(MidiGraphicTranslatorGroupLayout);
}

MidiGraphicTranslator::~MidiGraphicTranslator()
{
    if (MidiSourceId > undefinedMidiSource)                         // the Translator is no longer a MidiSource consumer
        emit unRegisterMidiSource(MidiSourceId);

}

void MidiGraphicTranslator::updateListOfMidiSourcesInComboBox(int nbMS)
{
    PRINTF((">>> updateListOfMidiSourcesInComboBox(%2d) of instance %d\n",nbMS,instanceId));
    int nbItems = MidiSources_ComboBox->count();
    int oldNbMS = nbItems - 1;
    PRINTF(("    update from %d to %d MidiSources\n",oldNbMS,nbMS));
    int initialCBMidiSourceId = MidiSources_ComboBox->currentIndex();
    PRINTF(("    old currentItem of Translator instance %d was %d\n",instanceId,initialCBMidiSourceId));
    if (initialCBMidiSourceId < 0)                                          // probably useless because current item is 1
        initialCBMidiSourceId = 0;
    if (initialCBMidiSourceId > internalId2displayId(nbMS-1)) {             // currentItem should not be > new last item
        MidiSources_ComboBox->setCurrentIndex(0);
        PRINTF(("    currentItem of Translator instance %d is now %d\n",instanceId,MidiSources_ComboBox->currentIndex()));
    }

    for (int i = nbItems-1; i >= nbMS; i--)
        MidiSources_ComboBox->removeItem(internalId2displayId(i));

    for (int i = nbItems-1; i < nbMS; i++) {
        char bla[8];
        sprintf(bla,"#%02d",internalId2displayId(i));
        MidiSources_ComboBox->addItem((QString)(bla));
    }
    PRINTF(("    currentItem of Translator instance %d is now %d\n",instanceId,MidiSources_ComboBox->currentIndex()));
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


int MidiGraphicTranslator::getInstanceId(void) const
{
    return(instanceId);
}

void MidiGraphicTranslator::recieveMidivent(Midivent &evt)
{
    printf("Translator %02d recieving Midivent: ", this->instanceId);
    switch (evt.getType())  {
        case None:
            printf("None\n"); break;
        case NoteOn:
            printf("NoteOn %3d\n",evt.getNote()); break;
        case NoteOff:
            printf("NoteOff %3d\n",evt.getNote()); break;
        default:
            printf("unknown \"%d\", \"%d\"\n",evt.getType(),evt.getNote()); break;
    }
}

void MidiGraphicTranslator::changingMidiSource(int displayId)
{
    if (displayId == undefinedMidiSource)               // the ComboBox is not initialized
        return;
    int msId = displayId2internalId(displayId);
    PRINTF(("changingMidiSource of instance %d: (%d -> %d ) \n",instanceId,MidiSourceId,msId));
    if (msId == MidiSourceId)
        return;
    if (MidiSourceId != undefinedMidiSource)
        emit unRegisterMidiSource(MidiSourceId);
    MidiSourceId = msId;
    emit registerMidiSource(MidiSourceId);
}

void MidiGraphicTranslator::recieveNumberOfMidiSources(int nbMS)
{
    PRINTF(("recieveNumberOfMidiSources(%d) in instance %d \n",nbMS,instanceId));
    updateListOfMidiSourcesInComboBox(nbMS);
}
