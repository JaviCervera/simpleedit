#include "time.h"
#if defined(_WIN32)
#include <Windows.h>
#elif defined(__APPLE__)
#include <CoreServices/CoreServices.h>
#endif


int Millisecs() {
    //return (int)time(NULL)*1000;
#if defined(_WIN32)
   return timeGetTime();
#elif defined(__linux)
   unsigned int t;
   struct timeval tv;
   gettimeofday ( &tv, 0 );
   t = tv.tv_sec * 1000;
   t += tv.tv_usec / 1000;
   return t;
#elif defined(__APPLE__)
    double t;
    UnsignedWide uw;
    Microseconds( &uw );
    t=(uw.hi<<(32-9))|(uw.lo>>9);
    return t/(1000.0/512.0);
#endif
}
