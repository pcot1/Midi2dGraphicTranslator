#include "one2oneTranslator.h"

//extern char *bla;

// managing index rules
inline int displayId2internalId(int i) {return(i-1);}                       // first internalId is 0
inline int internalId2displayId(int i) {return(i+1);}                       // first displayId is 1

const int normalDialSize = 40;

// *** Constructor
One2OneTranslator::One2OneTranslator(QWidget *pparent) : MidiGraphicTranslator(pparent)
{
    abstractItemColor = QColor(180,180,180,255);                               // default color is dark gray
    abstractItemSize = 12;                                                   // default sizeln = 1

    color_Button = new QPushButton("color");
    color_Button->setMaximumWidth(36);
    color_Button->setPalette(QPalette(abstractItemColor));
    color_Button->setAutoFillBackground(true);
    QObject::connect(color_Button,SIGNAL(clicked()),this,SLOT(setAbstractItemColor()));
    color_Box = new QGroupBox(this);
    color_Box->setPalette(QPalette(abstractItemColor));
    color_Box->setAutoFillBackground(true);
    color_Layout = new QHBoxLayout;
    color_Layout->addWidget(color_Button,0,Qt::AlignCenter);
    color_Box->setLayout(color_Layout);

    sizeln_Label = new QLabel("Size:");
    QFont smallerLabel = sizeln_Label->font();
    smallerLabel.setPointSize(7);
    sizeln_Label->setFont(smallerLabel);
    //qCDebug(GUtru,"font size = %d\n", (sizeln_Label->font().pointSize()));

    sizeln_Dial = new  QDial();
    sizeln_Dial->setMaximumSize(normalDialSize,normalDialSize);
    sizeln_Dial->setMinimum(1);
    sizeln_Dial->setMaximum(50);
    sizeln_Dial->setValue(10);
    sizeln_Dial->setWrapping(false);
    sizeln_Dial->setNotchesVisible(true);
    //sizeln_Dial->setTracking(false);
    //sizeln_Dial->setMaximumWidth(widthLayoutControlCell);
    QObject::connect(sizeln_Dial,SIGNAL(valueChanged(int)),this,SLOT(setAbstractItemSize(int)));

    abstractItem_Layout = new QHBoxLayout;
    //abstractItem_Layout->addWidget(color_Button,60,Qt::AlignCenter);
    abstractItem_Layout->addWidget(color_Box,60,Qt::AlignTop);
    abstractItem_Layout->addWidget(sizeln_Label,0,Qt::AlignRight);
    abstractItem_Layout->addWidget(sizeln_Dial,40,Qt::AlignTop);
    //abstractItem_Layout->setAlignment(Qt::AlignTop);
    addLayout(abstractItem_Layout);
}

// *** Destructor
One2OneTranslator::~One2OneTranslator()
{
}

void One2OneTranslator::printObject(void) const     {
    qCDebug(GUtru,">>> printObject: One2OneTranslator[%1d] is %p\n",getInstanceId(),this);
    MidiGraphicTranslator::printObject();
}

// *** private slot
void One2OneTranslator::setAbstractItemColor(void)  {

    abstractItemColor = QColorDialog::getColor(abstractItemColor, this);
    QPalette colorPalette(abstractItemColor);
    color_Button->setPalette(colorPalette);
    color_Box->setPalette(colorPalette);
    return;
}

// *** private slot
void One2OneTranslator::setAbstractItemSize(int value)  {

    qreal sizeln = (qreal)(value) / 10.0;
    qreal size = 3.0 * qExp(sizeln);
    abstractItemSize = 5 + (int)(size);
}

// *** first, let's analyse consequences of a new noteOn
void One2OneTranslator::newNoteOnAnalysis(septet note, septet velocity)   {
    MidiGraphicTranslator::newNoteOnAnalysis(note,velocity);
    return;
}

// *** transform a Midivent noteOn in graphic action
void One2OneTranslator::processNoteOn(septet note, septet velocity)   {

    qCDebug(GRgrp,"Translator #%02d One2OneTranslator::processNoteOn updates Translator graphic Layer %p\n",getDId(),graphicLayer);
    qCDebug(GRgrp,"Translator #%02d the graphicLayer scene is %p\n",getDId(),graphicLayer->scene());
    QGraphicsItem *item = createGraphicsItem(note,velocity);
    modifyGraphicsItem(item,note,velocity);
    positionGraphicsItem(item,note,velocity);
    graphicLayer->addToGroup(item);
    qCDebug(GRgrp,"Translator #%02d store the item in its scene %p\n",getDId(),item->scene());
    setNoteThing(note,(void *)(item));
    QList <QGraphicsItem *> itemList = graphicLayer->childItems();
    qCDebug(GRgrp,"Translator #%02d (%p) graphicLayer %p has %d items\n",getDId(),this,graphicLayer,itemList.size());
    return;
}

// *** first, let's analyse consequences of a new noteOff
void One2OneTranslator::newNoteOffAnalysis(septet note, septet velocity)   {
    MidiGraphicTranslator::newNoteOffAnalysis(note,velocity);
    return;
}

// *** transform a Midivent in graphic action
void One2OneTranslator::processNoteOff(unsigned char note, unsigned char velocity)   {

    MidiGraphicTranslator::processNoteOff(note, velocity);
    return;
}


