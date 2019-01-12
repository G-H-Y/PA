#include "common.h"

_Context* schedule(_Context *prev);
_Context *do_syscall(_Context *c);

static _Context *do_event(_Event e, _Context *c)
{
  switch (e.event)
  {
  case _EVENT_YIELD:
    Log("event ID = %d\n", e.event);
    _Context *tf = schedule(c);
       //Log("tf = %d",tf);   
    return tf;
  case _EVENT_SYSCALL:
    do_syscall(c);
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
}
