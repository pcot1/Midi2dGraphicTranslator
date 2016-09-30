#ifndef NOTENAMETRANSLATOR_H
#define NOTENAMETRANSLATOR_H
#include "one2OneTranslator.h"
#include <QRectF>


/*********************************************************************************************************************/
//
// translate 1 Midivent note in 1 string with the note name
//
/*********************************************************************************************************************/

class NoteNameTranslator : public One2OneTranslator
{
    Q_OBJECT

public:
    NoteNameTranslator(QWidget *parent = Q_NULLPTR);
    ~NoteNameTranslator();
    void printObject(void) const;                                           // debug
public slots:
signals:
protected:
    QGraphicsItem *createGraphicsItem(septet note, septet velocity);
    void modifyGraphicsItem(QGraphicsItem *item, septet note, septet velocity);
    void positionGraphicsItem(QGraphicsItem *item, septet note, septet velocity);
private:
};

#endif //ONE2ONETRANSLATOR_H
