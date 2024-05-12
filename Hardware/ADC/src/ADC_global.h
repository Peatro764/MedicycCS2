#ifndef ADC_GLOBAL_H
#define ADC_GLOBAL_H

#include <QtCore/QtGlobal>

#if defined(ADC_LIB)
#  define ADC_EXPORT Q_DECL_EXPORT
#else
#  define ADC_EXPORT Q_DECL_IMPORT
#endif

#endif // ADC_GLOBAL_H
