#ifndef ONE2ONETRANSLATOR_H
#define ONE2ONETRANSLATOR_H
#include "midiGraphicTranslator.h"
#include <QDial>
#include <QtMath>


/*********************************************************************************************************************/
//
// Base Abstract Class of 1 Midivent transformer to 1 graphic item
//
/*********************************************************************************************************************/

class One2OneTranslator : public MidiGraphicTranslator
{
    Q_OBJECT

public:
    One2OneTranslator(QWidget *parent = Q_NULLPTR);
    virtual ~One2OneTranslator();
    virtual void printObject(void) const;           // debug
public slots:
signals:
protected:
    virtual void processNoteOn(septet note, septet velocity);
    virtual void processNoteOff(septet note, septet velocity);
    virtual QGraphicsItem *createGraphicsItem(septet note, septet velocity);
    virtual void modifyGraphicsItem(QGraphicsItem *item, septet note, septet velocity);
    virtual void positionGraphicsItem(QGraphicsItem *item, septet note, septet velocity);
    QString getNoteName(septet note) const;        // get the name of the given note
    int getNoteOctave(septet note) const;          // get the octave of the given note
private slots:
    void setAbstractItemColor(void);
    void setAbstractItemSize(int);
private:
    QColor abstractItemColor;
    //float sizeln;
    int abstractItemSize;
                                                // One2One dedicated (abstactTtemProperties) Gui par
    QPushButton *color_Button;                      // abstactTtemProperties : color of the item
    QLabel *sizeln_Label;                           // abstactTtemProperties : size label for QDial
    QDial *sizeln_Dial;                             // abstactTtemProperties : ln(size) of the item
    QBoxLayout *abstractItem_Layout;                // abstactTtemProperties : (probably horizontal) layout
};

#endif //ONE2ONETRANSLATOR_H
