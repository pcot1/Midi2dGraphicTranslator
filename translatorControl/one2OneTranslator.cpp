#include "one2oneTranslator.h"

//extern char *bla;

// *** Constructor
One2OneTranslator::One2OneTranslator(QWidget *pparent) : MidiGraphicTranslator(pparent)
{
    abstractItemColor = QColor(180,180,180,255);                               // default color is dark gray
    //sizeln = 0;                                                 // default sizeln = 0 (size = 1)
    abstractItemSize = 12;                                                   // default sizeln = 1 (sizeln = 0)

    color_Button = new QPushButton("color");
    color_Button->setMaximumWidth(40);
    color_Button->setPalette(QPalette(abstractItemColor));
    color_Button->setAutoFillBackground(true);
    QObject::connect(color_Button,SIGNAL(clicked()),this,SLOT(setAbstractItemColor()));

    sizeln_Label = new QLabel("Size:");
    QFont smallerLabel = sizeln_Label->font();
    smallerLabel.setPointSize(7);
    sizeln_Label->setFont(smallerLabel);
    //qCWarning(GUtru,"font size = %d\n", (sizeln_Label->font().pointSize()));

    sizeln_Dial = new  QDial();
    sizeln_Dial->setMaximumSize(40,40);
    sizeln_Dial->setMinimum(0);
    sizeln_Dial->setMaximum(40);
    sizeln_Dial->setValue(10);
    sizeln_Dial->setWrapping(false);
    sizeln_Dial->setNotchesVisible(true);
    //sizeln_Dial->setTracking(false);
    //sizeln_Dial->setMaximumWidth(widthLayoutControlCell);
    QObject::connect(sizeln_Dial,SIGNAL(valueChanged(int)),this,SLOT(setAbstractItemSize(int)));

    abstractItem_Layout = new QHBoxLayout;
    abstractItem_Layout->addWidget(color_Button,60,Qt::AlignCenter);
    abstractItem_Layout->addWidget(sizeln_Label,0,Qt::AlignRight);
    abstractItem_Layout->addWidget(sizeln_Dial,40,Qt::AlignTop);
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

}

// *** private slot
void One2OneTranslator::setAbstractItemSize(int value)  {

    qreal sizeln = (qreal)(value) / 10.0;
    qreal size = 3.0 * qExp(sizeln);
    abstractItemSize = 5 + (int)(size);
}

// *** get Note Name
QString One2OneTranslator::getNoteName(septet note) const  {
    switch (note%12)    {
        case  0: return(QStringLiteral(" C")); break;
        case  1: return(QStringLiteral("C#")); break;
        case  2: return(QStringLiteral(" D")); break;
        case  3: return(QStringLiteral("Eb")); break;
        case  4: return(QStringLiteral(" E")); break;
        case  5: return(QStringLiteral(" F")); break;
        case  6: return(QStringLiteral("F#")); break;
        case  7: return(QStringLiteral(" G")); break;
        case  8: return(QStringLiteral("G#")); break;
        case  9: return(QStringLiteral(" A")); break;
        case 10: return(QStringLiteral("Bb")); break;
        case 11: return(QStringLiteral(" B")); break;
        default:
            qCCritical(GUtru,"unknpown note\n");
    }
    return(QStringLiteral(" H"));
}

// *** get Note Octave
int One2OneTranslator::getNoteOctave(septet note) const  {
    return((note/12)-2);
}

// *** transform a Midivent noteOn in graphic action
void One2OneTranslator::processNoteOn(septet note, septet velocity)   {

    qCDebug(GRgrp,"One2OneTranslator::processNoteOn updates Translator graphic Layer %p\n",graphicLayer);
    qCDebug(GRgrp,"the graphicLayer scene is %p\n",graphicLayer->scene());
    QGraphicsItem *item = createGraphicsItem(note,velocity);
    modifyGraphicsItem(item,note,velocity);
    positionGraphicsItem(item,note,velocity);
    graphicLayer->addToGroup(item);
    qCDebug(GRgrp,"store the item in its scene %p\n",item->scene());
    setNoteThing(note,(void *)(item));
    QList <QGraphicsItem *> itemList = graphicLayer->childItems();
    qCDebug(GRgrp,"TR #%02d (%p) graphicLayer %p has %d items\n",getInstanceId(),this,graphicLayer,itemList.size());
    return;
}

// *** transform a Midivent in graphic action
void One2OneTranslator::processNoteOff(unsigned char note, unsigned char velocity)   {

    MidiGraphicTranslator::processNoteOff(note, velocity);
    return;
}

// *** transform a Midivent noteOn in graphic action
QGraphicsItem *One2OneTranslator::createGraphicsItem(septet note, septet velocity)   {

    char bla[16];
    qCDebug(GRgrp,"One2OneTranslator::createGraphicsItem(%d,%d)\n",note,velocity);
    QGraphicsTextItem *pText = new  QGraphicsTextItem;
    pText->setFont(QFont("Impact",abstractItemSize));
    qCDebug(GRgrp,"note is %s octave #%d\n",qPrintable(getNoteName(note)),getNoteOctave(note));
    sprintf(bla,"%s[%1d]",qPrintable(getNoteName(note)),getNoteOctave(note));
    pText->setPlainText(QString(bla));
    pText->setDefaultTextColor(abstractItemColor);
    qCDebug(GRgrp,"return item %p to be added to the group %p\n",(QGraphicsItem *)pText,graphicLayer);
    return((QGraphicsItem *)pText);
}

// *** transform a previously created graphic item thanks to note parameters
void One2OneTranslator::modifyGraphicsItem(QGraphicsItem *item, septet note, septet velocity)   {

    qCDebug(GRgrp,"One2OneTranslator::modifyGraphicsItem(%p,%d,%d)\n",item,note,velocity);
    return;
}

// *** position a previously created graphic item thanks to note parameters
void One2OneTranslator::positionGraphicsItem(QGraphicsItem *item, septet note, septet velocity)   {

    qCDebug(GRgrp,"One2OneTranslator::positionGraphicsItem(%p,%d,%d)\n",item,note,velocity);
    QPointF where = generateRandomWorldCoordinates();
    qCDebug(GRgrp,"drawing item %p in (%+3.1f,%+3.1f)\n",item,where.x(),where.y());
    item->setPos(where);
    return;
}
