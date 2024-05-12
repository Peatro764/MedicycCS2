#ifndef NSINGLE_GLOBAL_H
#define NSINGLE_GLOBAL_H

#include <QtCore/QtGlobal>

#if defined(NSINGLE_LIB)
#  define NSINGLE_EXPORT Q_DECL_EXPORT
#else
#  define NSINGLE_EXPORT Q_DECL_IMPORT
#endif

#endif // NSINGLE_GLOBAL_H
