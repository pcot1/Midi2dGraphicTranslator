#include "noteNameTranslator.h"

// managing index rules
inline int displayId2internalId(int i) {return(i-1);}                       // first internalId is 0
inline int internalId2displayId(int i) {return(i+1);}                       // first displayId is 1

// *** Constructor
NoteNameTranslator::NoteNameTranslator(QWidget *pparent) : One2OneTranslator(pparent)
{
    char bla[16];
    sprintf(bla,"noteName #%02d",internalId2displayId(getInstanceId()));
    setTranslatorName(QString(bla));
}

// *** Destructor
NoteNameTranslator::~NoteNameTranslator()
{
}

void NoteNameTranslator::printObject(void) const     {
    One2OneTranslator::printObject();
}

// *** transform a Midivent noteOn in graphic action
QGraphicsItem *NoteNameTranslator::createGraphicsItem(septet note, septet velocity)   {

    char bla[16];
    qCDebug(GRgrp,"NoteNameTranslator::createGraphicsItem(%d,%d)\n",note,velocity);
    QGraphicsTextItem *pText = new  QGraphicsTextItem;
    pText->setFont(QFont("Impact",getAbstractItemSize()));
    qCDebug(GRgrp,"note is %s octave #%d\n",qPrintable(getNoteName(note)),getNoteOctave(note));
    sprintf(bla,"%s[%1d]",qPrintable(getNoteName(note)),getNoteOctave(note));
    pText->setPlainText(QString(bla));
    pText->setDefaultTextColor(getAbstractItemColor());
    qCDebug(GRgrp,"return item %p to be added to the group %p\n",(QGraphicsItem *)pText,graphicLayer);
    return((QGraphicsItem *)pText);
}

// *** transform a previously created graphic item thanks to note parameters
void NoteNameTranslator::modifyGraphicsItem(QGraphicsItem *item, septet note, septet velocity)   {

    qCDebug(GRgrp,"NoteNameTranslator::modifyGraphicsItem(%p,%d,%d)\n",item,note,velocity);
    return;
}

// *** position a previously created graphic item thanks to note parameters
void NoteNameTranslator::positionGraphicsItem(QGraphicsItem *item, septet note, septet velocity)   {

    qCDebug(GRgrp,"NoteNameTranslator::positionGraphicsItem(%p,%d,%d)\n",item,note,velocity);
    QPointF where = generateRandomWorldCoordinates();
    qCDebug(GRgrp,"drawing item %p in (%+3.1f,%+3.1f)\n",item,where.x(),where.y());
    item->setPos(where);
    return;
}
