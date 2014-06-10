#ifndef ASSERT_H
#define ASSERT_H

#ifndef NDEBUG
#define _P(X) #X
#define _T(X) _P(X)
#define assert(X) assert_m (X,__FILE__ "(" _T(__LINE__) ") : " #X)
#else
#define assert(X)
#endif

void assert_m (int,char const *) ;

#endif
