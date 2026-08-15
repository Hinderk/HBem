#ifndef __TIMER_H
#define __TIMER_H

#include <cstdint>



/* A time value that is accurate to the nearest
   microsecond but also has a range of years.  */


struct timeval
{
    int64_t      tv_sec ;              /* Seconds.  */
    int64_t     tv_usec ;         /* Microseconds.  */
} ;



/* POSIX.1b structure for a time value.  This is like
   a `struct timeval' but features nanoseconds instead
   of microseconds.  */


struct timespec_smr
{
    int64_t      tv_sec ;              /* Seconds.  */
    int64_t     tv_nsec ;          /* Nanoseconds.  */
} ;


#endif // __TIMER_H
