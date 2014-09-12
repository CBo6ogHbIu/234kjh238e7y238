#ifndef DIAGNOSTICS_H
#define DIAGNOSTICS_H
#include <stdbool.h>
/*----------------------------------------------------------------------------
 *      initialize visual diagnostic of controller's work
 *---------------------------------------------------------------------------*/      
bool diag_initialize(void);
bool diag_start(void);
bool diag_stop(void);

/*----------------------------------------------------------------------------
 *      checks result
 *---------------------------------------------------------------------------*/        
bool check(bool);
#endif
