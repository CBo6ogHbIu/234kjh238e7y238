#ifndef TIMERS_H
#define TIMERS_H
#include <stdbool.h>
#include "cmsis_os.h"

/// \note	active(running all the time) user timers
/// \note	add new timers ONLY to the back of the list
enum ACTIVE_TIMER{TIMER_1MS, TIMER_1S, ACTIVE_TIMERS_COUNT};

/// \note add subscriber(thread) to the mail-list of active timer
/// \note each subscriber can subscribe several timers
bool timers_add_subscribtion(const osThreadId * const subscriber, enum ACTIVE_TIMER timer);

/// \note	remove subscriber(thread) from the mail-list of all active timers
/// \note	other subscribers moves left one bit 
void timers_remove_subscription(const osThreadId * const subscriber);

/// \note	timer management
void timers_uninitialize(void);
bool timers_initialize(void);
bool timers_start(void);
bool timers_stop(void);
#endif
