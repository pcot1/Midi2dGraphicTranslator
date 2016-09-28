#include "one2oneTranslator.h"

//extern char *bla;

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
    //abstractItem_Layout->addWidget(color_Button,60,Qt::AlignCenter);
    abstractItem_Layout->addWidget(color_Box,60,Qt::AlignCenter);
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

    abstractItemColor = QColorDialog::getColor(abstractItemColor, this);
    QPalette colorPalette(abstractItemColor);
    color_Button->setPalette(colorPalette);
    color_Box->setPalette(colorPalette);
    //color_Button->setStyleSheet("background-color:black;");
    /*QPalette pal = color_Button->palette();

    pal.setColor(QPalette::Button, abstractItemColor);
    pal.setColor(QPalette::Light, abstractItemColor);
    pal.setColor(QPalette::Midlight, abstractItemColor);
    pal.setColor(QPalette::Mid, abstractItemColor);
    pal.setColor(QPalette::Dark, abstractItemColor);
    //color_Button->setContentsMargins(5,5,5,5);
    color_Button->setAutoFillBackground(true);
    color_Button->setPalette(QPalette(abstractItemColor));
    //color_Button->update();
    */
     /*
    color_Button->setPalette(QPalette(abstractItemColor));

    color_Button->setAutoFillBackground(true);
    color_Button->update();
    */

      /*
      QPalette pal = color_Button->palette();
      pal.setColor(QPalette::Button, abstractItemColor);
      color_Button->setAutoFillBackground(true);
      color_Button->setPalette(pal);
      color_Button->update();
      */

    /*
    const QColorDialog::ColorDialogOptions options = QFlag(colorDialogOptionsWidget->value());
    const QColor color = QColorDialog::getColor(Qt::green, this, "Select Color", options);

          if (color.isValid()) {
              colorLabel->setText(color.name());
              colorLabel->setPalette(QPalette(color));
              colorLabel->setAutoFillBackground(true);
          }
          */
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

