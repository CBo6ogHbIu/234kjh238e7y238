#ifndef DIAGNOSTICS_H
#define DIAGNOSTICS_H
#include <stdbool.h>
#include "rtl.h"
/*----------------------------------------------------------------------------
 *      initialize visual diagnostic of controller's work
 *---------------------------------------------------------------------------*/      
bool diag_initialize(void);
bool diag_start(void);
bool diag_stop(void);
OS_TID diag_get_task_id(void);
/*----------------------------------------------------------------------------
 *      checks result
 *---------------------------------------------------------------------------*/        
bool check(bool);
#endif
