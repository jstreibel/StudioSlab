#ifndef ANALYTIC_BREATHER_H
#define ANALYTIC_BREATHER_H

#include "Common/CoreNativeTypes.h"

/* (* A static breather *) */
floatt getPhiValue(floatt t, floatt x, floatt v);
floatt getdPhidtValue(floatt t, floatt x, floatt v);

/* (* A complete breather *) */
floatt psi(floatt t, floatt x, floatt v, floatt u);
floatt dpsi(floatt t, floatt x, floatt v, floatt u);

#endif // ANALYTIC_BREATHER_H
