#ifndef RTMIDI_GLOBAL_H
#define RTMIDI_GLOBAL_H

#include <QtCore/qglobal.h>

#if defined(RTMIDI_LIBRARY)
#  define RTMIDISHARED_EXPORT Q_DECL_EXPORT
#else
#  define RTMIDISHARED_EXPORT Q_DECL_IMPORT
#endif

#endif // RTMIDI_GLOBAL_H
