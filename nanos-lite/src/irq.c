#include "common.h"

_Context *schedule(_Context *prev);
_Context *do_syscall(_Context *c);

static _Context *do_event(_Event e, _Context *c)
{
  switch (e.event)
  {
  case _EVENT_YIELD:
    return schedule(c);
  case _EVENT_SYSCALL:
    do_syscall(c);
    break;
  case _EVENT_IRQ_TIMER:
    //Log("irq_timer");
    _yield();
    break;
  default:
    panic("Unhandled event ID = %d\n", e.event);
    break;
  }

  return NULL;
}

void init_irq(void)
{
  Log("Initializing interrupt/exception handler...");
  _cte_init(do_event);
  //Log("end");
}
