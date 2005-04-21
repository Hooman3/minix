#include "timers.h"

/*===========================================================================*
 *				tmrs_settimer				     *
 *===========================================================================*/
void tmrs_settimer(tmrs, tp, exp_time, watchdog)
timer_t **tmrs;				/* pointer to timers queue */
timer_t *tp;				/* the timer to be added */
clock_t exp_time;			/* its expiration time */
tmr_func_t watchdog;			/* watchdog function to be run */
{
/* Activate a timer to run function 'fp' at time 'exp_time'. If the timer is
 * already in use it is first removed from the timers queue. Then, it is put
 * in the list of active timers with the first to expire in front.
 * The caller responsible for scheduling a new alarm for the timer if needed. 
 */
  timer_t **atp;

  /* Possibly remove an old timer. Then set the timer's variables. */
  if (tp->tmr_exp_time != TMR_NEVER)
  	(void) tmrs_clrtimer(tmrs,tp);
  tp->tmr_exp_time = exp_time;
  tp->tmr_func = watchdog;

  /* Add the timer to the active timers. The next timer due is in front. */
  for (atp = tmrs; *atp != NULL; atp = &(*atp)->tmr_next) {
	if (exp_time < (*atp)->tmr_exp_time) break;
  }
  tp->tmr_next = *atp;
  *atp = tp;
}

