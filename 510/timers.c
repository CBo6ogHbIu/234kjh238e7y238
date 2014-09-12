#include "timers.h"
#include "cmsis_os.h"

const char MAX_SUBSCRIBERS_COUNT = 6;										//	refer to RTX_Conf_CM.c, #define OS_TASKCNT
bool  is_initialized = false;

osTimerId timers[ACTIVE_TIMERS_COUNT]					= {NULL};	//	active timers ids
char mail_lists[ACTIVE_TIMERS_COUNT] 					= {0x00};	//	active timers mail lists, order should be the same as timers. 0x00 == no subscribers
osThreadId subscribers[MAX_SUBSCRIBERS_COUNT] = {NULL};	//	subscribers ids

bool timers_add_subscribtion(const osThreadId * const subscriber, enum ACTIVE_TIMER timer ){
	char i;
	if(!is_initialized)
		return false;
	
	for(i = 0; i < MAX_SUBSCRIBERS_COUNT; i++)						//	if record already exists or the back of list is reached
		if(subscribers[i] == *subscriber || subscribers[i] == NULL){
			subscribers[i] = *subscriber;											//	add==replace record
			break;
		}
		
	mail_lists[timer] |= 1 << i;
	return true;
}

void timers_remove_subscription(const osThreadId * const subscriber){
	char i, j, index;
	
	if(!is_initialized)
		return;
	
	for(i = 0; i < MAX_SUBSCRIBERS_COUNT; i++)
		if(subscribers[i] == *subscriber)
			break;
	
	index = i;																						//	index to delete
		
	for( ; i < MAX_SUBSCRIBERS_COUNT - 1 && subscribers[i + 1] != 0; i++)
		subscribers[i] = subscribers[i + 1];								//	move others records left => all records after deleted becomes broken + last record doubles
		
	subscribers[i] = 0x00;																//	fix last record doubling
		
	for(i = 0; i < ACTIVE_TIMERS_COUNT; i++){
		mail_lists[i] &= ~ ( 1 << index );									//	remove deleted record from all mail lists
		
		for(j = index; j < MAX_SUBSCRIBERS_COUNT - 1 && subscribers[j] != 0; j++)
			if(mail_lists[i] &= 1<<(j + 1) ){
				mail_lists[i] &= ~ ( 1 << ( j + 1 ) );						//	remove broken record
				mail_lists[i] |= 1 << j;													//	fix record
			}
	}
}

void timer_1ms (void const *argument)  {
	char i;
	if(is_initialized == false)
		return;	
	for(i = 0; i < MAX_SUBSCRIBERS_COUNT; i++)
	if(mail_lists[TIMER_1MS] & 1 << i)
		osSignalSet(subscribers[i], 0x0001);
}


void timer_1s (void const *argument)  {
	char i;
	if(is_initialized == false)
		return;	
	for(i = 0; i < MAX_SUBSCRIBERS_COUNT; i++)
	if(mail_lists[TIMER_1S] & 1 << i)
		osSignalSet(subscribers[i], 0x0001);
}
osTimerDef(ms_1, timer_1ms);
osTimerDef(s_1, timer_1s);

bool timers_initialize(void){
	osTimerId t_id = NULL;
	//	1ms timer
	t_id = osTimerCreate(osTimer(ms_1), osTimerPeriodic, NULL);
	if(t_id != NULL)
		timers[TIMER_1MS] = t_id;
	else
		return false;
	
	t_id = NULL;
	//	1s timer
	t_id = osTimerCreate(osTimer(s_1), osTimerPeriodic, NULL);
	if(t_id != NULL)
		timers[TIMER_1S] = t_id;	
	else 
		return false;
	return is_initialized = true;
}

bool timers_start(void){
	bool result = true;	
	if(is_initialized == false)
		return false;
	result &= timers[TIMER_1MS] != NULL && osTimerStart(timers[TIMER_1MS], 1)   == osOK;
	result &= timers[TIMER_1S]  != NULL && osTimerStart(timers[TIMER_1S], 1000) == osOK;
	return result;
}
bool timers_stop(void){
	char i;
	bool result = true;	

	if(is_initialized == false)
		return false;	

	for(i = 0; i < ACTIVE_TIMERS_COUNT; i++)
		result &= timers[i] != NULL && osTimerStop(timers[i])	== osOK;
	
	return result;	
}

void timers_uninitialize(void){
	char i;

	if(is_initialized == false)
		return;	

	for(i = 0; i < ACTIVE_TIMERS_COUNT; i++)
		if(timers[i] != NULL)
			osTimerDelete(timers[i]);

		is_initialized = false;
}
