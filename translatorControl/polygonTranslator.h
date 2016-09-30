#ifndef POLYGONTRANSLATOR_H
#define POLYGONTRANSLATOR_H
#include "one2OneTranslator.h"


/*********************************************************************************************************************/
//
// translate 1 Midivent note in 1 polygon positionned according note within octave (X) and octave (Y)
//
/*********************************************************************************************************************/

class PolygonTranslator : public One2OneTranslator
{
    Q_OBJECT

public:
    PolygonTranslator(QWidget *parent = Q_NULLPTR);
    ~PolygonTranslator();
    void printObject(void) const;                                           // debug
public slots:
signals:
protected:
    QGraphicsItem *createGraphicsItem(septet note, septet velocity);
    void modifyGraphicsItem(QGraphicsItem *item, septet note, septet velocity);
    void positionGraphicsItem(QGraphicsItem *item, septet note, septet velocity);
private:
    QPushButton *rotate_Button;
    QLabel *form_Label;
    QComboBox *form_ComboBox;
    QHBoxLayout *form_Layout;
    QLabel *height_Label;
    QDial *height_Dial;
    QVBoxLayout *height_Layout;
    QLabel *position_Label;
    QDial *position_Dial;
    QVBoxLayout *position_Layout;
    QLabel *octaves_Label;
    QDial *octaves_Dial;
    QVBoxLayout *octaves_Layout;
    QHBoxLayout *polygon_Layout;
};

#endif //POLYGONTRANSLATOR_H
