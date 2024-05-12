#ifndef OMRON_GLOBAL_H
#define OMRON_GLOBAL_H

#include <QtCore/QtGlobal>

#if defined(OMRON_LIB)
#  define OMRON_EXPORT Q_DECL_EXPORT
#else
#  define OMRON_EXPORT Q_DECL_IMPORT
#endif

#endif // OMRON_GLOBAL_H
