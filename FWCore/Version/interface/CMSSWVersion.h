#ifndef CMSSWVERSION
#define CMSSWVERSION

/*
 * These macros can be used in the following way:
 *
 *    #if CMSSW_VERSION_CODE >= CMSSW_VERSION(2,23,4)
 *       #include <newheader.h>
 *    #else
 *       #include <oldheader.h>
 *    #endif
 *
*/

#define PPCAT(A, B) A ## B
#define CMSSW_RELEASE PROJECT_VERSION
#define CMSSW_VERSION(a,b,c,d,e) ((PPCAT(a,UL) << 32) + (PPCAT(b,UL) << 24) + (PPCAT(c,UL) << 16) + (PPCAT(d,UL) << 8) + PPCAT(e,UL))
//#define CMSSW_VERSION_CODE CMSSW_VERSION(6,5,1) /* 394497 */

#endif
