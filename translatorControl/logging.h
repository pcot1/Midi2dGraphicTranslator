#ifndef LOGGING_H
#define LOGGING_H
#include <QLoggingCategory>


Q_DECLARE_LOGGING_CATEGORY(Minit);              // midi.init
Q_DECLARE_LOGGING_CATEGORY(Mmsg);               // midi.flow.message
Q_DECLARE_LOGGING_CATEGORY(Mvent);              // midi.flow.midivent
Q_DECLARE_LOGGING_CATEGORY(GUinit);             // gui.main.init
Q_DECLARE_LOGGING_CATEGORY(GUupd);              // gui.main.update
Q_DECLARE_LOGGING_CATEGORY(GUmsi);              // gui.ms.init
Q_DECLARE_LOGGING_CATEGORY(GUmsu);              // gui.ms.update
Q_DECLARE_LOGGING_CATEGORY(GUtri);              // gui.tr.init
Q_DECLARE_LOGGING_CATEGORY(GUtru);              // gui.tr.update
Q_DECLARE_LOGGING_CATEGORY(GRinit);             // graphics.engine.init
Q_DECLARE_LOGGING_CATEGORY(GRupd);              // graphics.engine.update
Q_DECLARE_LOGGING_CATEGORY(GRscn);              // graphics.objs.scene
Q_DECLARE_LOGGING_CATEGORY(GRgrp);              // graphics.objs.group


class Logging
{
public:
    static void myMessageOutput(QtMsgType type, const QMessageLogContext &context, const QString &msg);
    Logging();
    ~Logging();
private:
    QString         messagePattern;
};



#endif // LOGGING_H
