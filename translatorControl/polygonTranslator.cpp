#include "polygonTranslator.h"

                                                                     // managing index rules
inline int displayId2internalId(int i) {return(i-1);}                       // first internalId is 0
inline int internalId2displayId(int i) {return(i+1);}                       // first displayId is 1

const int normalDialSize = 40;
const int smallDialSize = 38;

// *** Constructor
PolygonTranslator::PolygonTranslator(QWidget *pparent) : One2OneTranslator(pparent)
{
    char bla[16];
    sprintf(bla,"polygon #%02d",internalId2displayId(getInstanceId()));
    setTranslatorName(QString(bla));

    rotate_Button = new QPushButton();
    rotate_Button->setIcon(QIcon("./rotate-icon-20.png"));
    rotate_Button->setCheckable(true);
    rotate_Button->setChecked(false);
    form_Label = new QLabel(QStringLiteral("form:"));
    QFont smallerLabel = form_Label->font();
    smallerLabel.setPointSize(7);
    form_ComboBox = new QComboBox();
    form_ComboBox->addItem(QStringLiteral("note"));
    form_ComboBox->addItem(QIcon("./circle-icon-20.png"),QStringLiteral("..."));
    form_ComboBox->addItem(QIcon("./square-icon-20.png"),QStringLiteral(" 4"));
    form_ComboBox->setCurrentIndex(0);
    form_Layout = new QHBoxLayout;
    form_Layout->addWidget(rotate_Button,0,Qt::AlignCenter);
    form_Layout->addWidget(form_Label,0,Qt::AlignRight);
    form_Layout->addWidget(form_ComboBox,0,Qt::AlignRight);
    //form_Layout->setAlignment(Qt::AlignTop);
    addLayout(form_Layout);

    height_Label = new QLabel(QStringLiteral("height"));
    height_Label->setFont(smallerLabel);
    height_Dial = new  QDial();
    height_Dial->setMaximumSize(smallDialSize,smallDialSize);
    height_Dial->setMinimum(1);
    height_Dial->setMaximum(100);
    height_Dial->setValue(70);
    height_Dial->setWrapping(false);
    height_Dial->setNotchesVisible(true);
    height_Dial->setTracking(false);
    height_Layout = new QVBoxLayout;
    height_Layout->addWidget(height_Label,0,Qt::AlignCenter);
    height_Layout->addWidget(height_Dial,0,Qt::AlignCenter);
    
    position_Label = new QLabel(QStringLiteral("position"));
    position_Label->setFont(smallerLabel);
    position_Dial = new  QDial();
    position_Dial->setMaximumSize(smallDialSize,smallDialSize);
    position_Dial->setMinimum(-10);
    position_Dial->setMaximum(10);
    position_Dial->setValue(0);
    position_Dial->setWrapping(false);
    position_Dial->setNotchesVisible(true);
    position_Dial->setTracking(false);
    position_Layout = new QVBoxLayout;
    position_Layout->addWidget(position_Label,0,Qt::AlignCenter);
    position_Layout->addWidget(position_Dial,0,Qt::AlignCenter);
    
    octaves_Label = new QLabel(QStringLiteral("octaves"));
    octaves_Label->setFont(smallerLabel);
    octaves_Dial = new  QDial();
    octaves_Dial->setMaximumSize(smallDialSize,smallDialSize);
    octaves_Dial->setMinimum(1);
    octaves_Dial->setMaximum(11);
    octaves_Dial->setValue(3);
    octaves_Dial->setWrapping(false);
    octaves_Dial->setNotchesVisible(true);
    octaves_Dial->setTracking(false);
    octaves_Layout = new QVBoxLayout;
    octaves_Layout->addWidget(octaves_Label,0,Qt::AlignCenter);
    octaves_Layout->addWidget(octaves_Dial,0,Qt::AlignCenter);
    
    polygon_Layout = new QHBoxLayout;
    polygon_Layout->addLayout(height_Layout);
    polygon_Layout->addLayout(position_Layout);
    polygon_Layout->addLayout(octaves_Layout);
    addLayout(polygon_Layout);
    
}

// *** Destructor
PolygonTranslator::~PolygonTranslator()
{
}

void PolygonTranslator::printObject(void) const     {
    One2OneTranslator::printObject();
}

// *** transform a Midivent noteOn in graphic action
QGraphicsItem *PolygonTranslator::createGraphicsItem(septet note, septet velocity)   {

    int size = getAbstractItemSize();
    QRectF englob(0,0,size,size);
    QGraphicsItem *item = 0;
    QGraphicsTextItem *pText = 0;
    QGraphicsEllipseItem *pEllipse = 0;
    QGraphicsRectItem *pRect = 0;
    qCDebug(GRgrp,"PolygonTranslator::createGraphicsItem(%d,%d)\n",note,velocity);
    switch(form_ComboBox->currentIndex())	{
    	case	0:				// noteName (no octave indication)
            pText = new QGraphicsTextItem(getNoteName(note));
    		pText->setFont(QFont("Impact",getAbstractItemSize()));
    		pText->setDefaultTextColor(getAbstractItemColor());
    		item = (QGraphicsItem *)pText;
    		break;
    	case	1:				// circle
    		pEllipse = new QGraphicsEllipseItem(englob);
    		pEllipse->setBrush(QBrush(getAbstractItemColor()));
            //pEllipse->setPen(QPen(Qt::NoPen));
    		item = (QGraphicsItem *)pEllipse;
    		break;
    	case 	2:				// square
    		pRect = new QGraphicsRectItem(englob);
    		pRect->setBrush(QBrush(getAbstractItemColor()));
            //pRect->setPen(QPen(Qt::NoPen));
    		item = (QGraphicsItem *)pRect;
    		break;
    	default:
    		qCWarning(GRgrp,"Translator #%02d : unknown form to draw\n",getDId());
    }
    qCDebug(GRgrp,"return item %p to be added to the group %p\n",item,graphicLayer);
    return(item);
}

// *** transform a previously created graphic item thanks to note parameters
void PolygonTranslator::modifyGraphicsItem(QGraphicsItem *item, septet note, septet velocity)   {

    qCDebug(GRgrp,"Translator #%02d: PolygonTranslator::modifyGraphicsItem(%p,%d,%d)\n",getDId(),item,note,velocity);
    if (rotate_Button->isChecked()) {
        qreal angle = generateRandomRealNormalizedValue() * 360.0;
        item->setRotation(angle);
        qCDebug(GRgrp,"Translator #%02d: apply rotation of %+3.1f\n",getDId(),angle);
    }
    return;
}

// *** position a previously created graphic item thanks to note parameters
void PolygonTranslator::positionGraphicsItem(QGraphicsItem *item, septet note, septet velocity)   {

    QRectF rectBounds = item->boundingRect();
    qCDebug(GRgrp,"Translator #%02d PolygonTranslator::positionGraphicsItem(%p,%d,%d)\n",getDId(),item,note,velocity);
    qreal marge = rectBounds.width()/2.0;
    qreal X = (worldUpLeftX + marge) + ((qreal)(getNoteIdInOctave(note)+ 0.5) * ((worldWidth - (2.0 * marge))/11.0));
    qCDebug(GRgrp,"Translator #%02d notetype %d X = %+3.1f\n",getDId(),getNoteIdInOctave(note),X);
    qreal height = (height_Dial->value() / 100.0) * worldHeight;
    qreal centralY = -(position_Dial->value() / 10.0) * (worldHeight / 2.0);
    qreal Y = centralY;
    int nbDisplayedOctaves = octaves_Dial->value();
    int highestOctave = getOctaveIdOfNote(getAverageReceivedNoteOn()) + (nbDisplayedOctaves / 2);
    int lowestOctave = highestOctave - nbDisplayedOctaves + 1;
    int octaveOfCurrentNote = getOctaveIdOfNote(note);
    if (octaveOfCurrentNote < lowestOctave)
            octaveOfCurrentNote = lowestOctave;
    if (octaveOfCurrentNote > highestOctave)
            octaveOfCurrentNote = highestOctave;
    if (nbDisplayedOctaves > 1) {
        Y += (0.5 - (qreal)(octaveOfCurrentNote - lowestOctave) / (qreal)(nbDisplayedOctaves-1)) * height;
    }
    qCDebug(GRgrp,"Translator #%02d octave %d average %d, height %+3.1f centralY %+3.1f  Y= %+3.1f\n",getDId(),octaveOfCurrentNote,getOctaveIdOfNote(getAverageReceivedNoteOn()),height,centralY,Y);
    qCDebug(GRgrp,"Translator #%02d drawing item %p in (%+3.1f,%+3.1f)\n",getDId(),item,X,Y);
    item->setPos(QPointF(X,Y));
                                                                        // correct position as graphic item is not centered in (0.0,0.0)
    item->moveBy(-rectBounds.width()/2.0,-rectBounds.height()/2.0);
    qCDebug(GRgrp,"Translator #%02d item moveOf(%+3.1f,%+3.1f)\n",getDId(),-rectBounds.width()/2.0,-rectBounds.height()/2.0);
    return;
}
