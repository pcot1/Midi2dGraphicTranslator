#include <stdio.h>
#include "logging.h"
#include <qapplication.h>
                                                        // logging catagory association with QLoggingCategory instances
Q_LOGGING_CATEGORY(Minit, "midi.init");                     // initialization of MIDI processing (RtMidi)
Q_LOGGING_CATEGORY(Mmsg,  "midi.flow.message");             // processing MIDI messages
Q_LOGGING_CATEGORY(Mvent, "midi.flow.midivent");            // processing Midivent
Q_LOGGING_CATEGORY(GUinit,"gui.main.init");                 // initialization of the gui window
Q_LOGGING_CATEGORY(GUupd, "gui.main.update");               // update of the gui window
Q_LOGGING_CATEGORY(GUmsi, "gui.ms.init");                   // intialization of MidiSource Gui
Q_LOGGING_CATEGORY(GUmsu, "gui.ms.update");                 // update of MidiSource Gui
Q_LOGGING_CATEGORY(GUtri, "gui.tr.init");                   // intialization of Translator Gui
Q_LOGGING_CATEGORY(GUtru, "gui.tr.update");                 // update of Translator Gui
Q_LOGGING_CATEGORY(GRinit,"graphics.engine.init");          // initialization of graphics engine (scene + view)
Q_LOGGING_CATEGORY(GRupd, "graphics.engine.update");        // update of graphics engine (scene + view)
Q_LOGGING_CATEGORY(GRscn, "graphics.objs.scene");           // filling scene with objects (first level)
Q_LOGGING_CATEGORY(GRgrp, "graphics.objs.group");           // filling groups with objects (second level)

QString messagePattern =(QStringLiteral("%{if-debug}%{file}[%{line}]: %{endif}%{message}"));

void Logging::myMessageOutput(QtMsgType type, const QMessageLogContext &context, const QString &msg) {
    QString formatedMsg = qFormatLogMessage(type,context,msg);
    QByteArray localMsg = formatedMsg.toLocal8Bit();
    fprintf(stdout, "%s", localMsg.constData());
    fflush(stdout);
}

Logging::Logging() {
    messagePattern =(QStringLiteral("%{if-debug}[%{line}]: %{endif}%{message}"));
    qInstallMessageHandler(myMessageOutput);
    qSetMessagePattern(messagePattern);
}

Logging::~Logging() {
}
