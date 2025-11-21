//
// Created by joao on 11/20/25.
//

#ifndef STUDIOSLAB_FIRSTRUN_H
#define STUDIOSLAB_FIRSTRUN_H

#include <utility>
#define SLAB_JOIN_IMPL(a,b) a##b
#define SLAB_JOIN(a,b)      SLAB_JOIN_IMPL(a,b)

 /* Usage:
 OnFirstRun {
 // code that should run once per call site
 }
 */
#define OnFirstRun \
static bool SLAB_JOIN(slab_once, LINE) = true; \
if (std::exchange(SLAB_JOIN(slab_once, LINE), false))

//  - Thread-safe macro with code parameter:
//  - Include <mutex>. Use when multiple threads may hit the same site.

/* Usage:
OnFirstRunTS({
// code that should run once per call site (thread-safe)
});
*/
#define OnFirstRunTS(CODE) \
do { \
static std::once_flag SLAB_JOIN(slab_once_flag, LINE); \
std::call_once(SLAB_JOIN(slab_once_flag, LINE), [&]{ CODE; }); \
} while (0)



#endif //STUDIOSLAB_FIRSTRUN_H