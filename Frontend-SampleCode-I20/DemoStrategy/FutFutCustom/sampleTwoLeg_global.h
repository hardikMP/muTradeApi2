#ifndef SAMPLE2_GLOBAL_H
#define SAMPLE2_GLOBAL_H

#include <QtCore/qglobal.h>

#if defined(SAMPLE2_LIBRARY)
#  define SAMPLE2SHARED_EXPORT Q_DECL_EXPORT
#else
#  define SAMPLE2SHARED_EXPORT Q_DECL_IMPORT
#endif

#endif // SAMPLE_STRATEGY_WITH_BASE_STRATEGY_GLOBAL_H