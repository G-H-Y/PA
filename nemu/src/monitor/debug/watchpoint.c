#include "monitor/watchpoint.h"
#include "monitor/expr.h"

#define NR_WP 32

static WP wp_pool[NR_WP];
static WP *head, *free_;

void init_wp_pool()
{
  int i;
  for (i = 0; i < NR_WP; i++)
  {
    wp_pool[i].NO = i;
    strcpy(wp_pool[i].exp, "");
    wp_pool[i].old_value = 0;
    wp_pool[i].hit = 0;
    wp_pool[i].next = &wp_pool[i + 1];
  }
  wp_pool[NR_WP - 1].next = NULL;

  head = NULL;
  free_ = wp_pool;
}

/* TODO: Implement the functionality of watchpoint */
WP *new_wp()
{
  if (free_ == NULL)
  {
    Log("no free wp!");
    assert(0);
  }
  WP *new_w = free_;
  free_ = free_->next;
  new_w->next = head;
  head = new_w;
  return new_w;
}

void free_wp(int NO)
{
  WP *p,*pre;
  p = head;
  pre = head;
  while((p!=NULL)&&(p->NO != NO)){
    pre = p;
    p = p->next;
  }
  if(pre->next == NULL){
    printf("no such wp!\n");
    assert(0);
  }
  pre->next = p->next;
  wp_pool[NO].next = free_;
  free_ = wp_pool+NO;
}

bool is_hit()
{
  bool hit = false;
  if (head != NULL)
  {
    WP *tmp = head;
    while (tmp != NULL)
    {
      bool s = true;
      int new_value = expr((*tmp).exp, &s);
      if (new_value != (*tmp).old_value)
      {
        (*tmp).hit++;
        printf("Hit watchpoint: %s\nnew_value = %d\nold_value = %d", (*tmp).exp, new_value, (*tmp).old_value);
        (*tmp).old_value = new_value;
        hit = true;
      }
    }
  }
  return hit;
}

void print_wp()
{
  if (head == NULL)
  {
    printf("no watchpoints!");
  }
  else
  {
    WP *tmp = head;
    printf("Num\t\tWhat\t\tHit Time(s)\n");
    while (tmp != NULL)
    {
      printf("%d\t\t%s\t\t%d\n", (*tmp).NO, (*tmp).exp, (*tmp).hit);
      tmp = tmp->next;
    }
  }
}
