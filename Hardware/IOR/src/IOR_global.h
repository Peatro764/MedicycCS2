#ifndef IOR_GLOBAL_H
#define IOR_GLOBAL_H

#include <QtCore/QtGlobal>

#if defined(IOR_LIB)
#  define IOR_EXPORT Q_DECL_EXPORT
#else
#  define IOR_EXPORT Q_DECL_IMPORT
#endif

#endif // IOR_GLOBAL_H
