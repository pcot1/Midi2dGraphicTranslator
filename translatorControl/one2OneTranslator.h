#ifndef ONE2ONETRANSLATOR_H
#define ONE2ONETRANSLATOR_H
#include "midiGraphicTranslator.h"
#include <QColorDialog>
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
    virtual void printObject(void) const;               // debug
public slots:
signals:
protected:
    virtual void newNoteOnAnalysis(septet note, septet velocity);
    virtual void processNoteOn(septet note, septet velocity);
    virtual void newNoteOffAnalysis(septet note, septet velocity);
    virtual void processNoteOff(septet note, septet velocity);
    virtual QGraphicsItem *createGraphicsItem(septet note, septet velocity) = 0;
    virtual void modifyGraphicsItem(QGraphicsItem *item, septet note, septet velocity) = 0;
    virtual void positionGraphicsItem(QGraphicsItem *item, septet note, septet velocity) = 0;
    QColor getAbstractItemColor(void) const     { return(abstractItemColor); }
    int getAbstractItemSize(void) const         { return(abstractItemSize); }
private slots:
    void setAbstractItemColor(void);
    void setAbstractItemSize(int);
private:
    QColor abstractItemColor;
    int abstractItemSize;
                                                // One2One dedicated (abstactTtemProperties) Gui par
    QGroupBox *color_Box;                           // abstactTtemProperties : color of the item
    QHBoxLayout *color_Layout;                       // layout for the pushButton inside the GroupBox
    QPushButton *color_Button;                      // abstactTtemProperties : set color of the item
    QLabel *sizeln_Label;                           // abstactTtemProperties : size label for QDial
    QDial *sizeln_Dial;                             // abstactTtemProperties : ln(size) of the item
    QBoxLayout *abstractItem_Layout;                // abstactTtemProperties : (probably horizontal) layout
};

#endif //ONE2ONETRANSLATOR_H
