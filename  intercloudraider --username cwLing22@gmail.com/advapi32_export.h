#ifndef ADVAPI32_EXPORT_H
#define ADVAPI32_EXPORT_H

#include <QtCore/qglobal.h>

#ifndef ADVAPI32_EXPORT
# if defined(ADVAPI32_MAKEDLL)
   /* We are building this library */
#  define ADVAPI32_EXPORT Q_DECL_EXPORT
# else
   /* We are using this library */
#  define ADVAPI32_EXPORT Q_DECL_IMPORT
# endif
#endif

#endif // ADVAPI32_EXPORT_H
