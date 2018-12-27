#ifndef __WATCHPOINT_H__
#define __WATCHPOINT_H__

#include "common.h"

typedef struct watchpoint {
  int NO;
  char exp[32];
  int old_value;
  int hit;
  struct watchpoint *next;

  /* TODO: Add more members if necessary */


} WP;
/*
#define NR_WP 32ss

static WP wp_pool[NR_WP];
static WP *head, *free_;
*/

WP* new_wp();
void free_wp(int);
bool is_hit();
void print_wp();

#endif
