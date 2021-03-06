#include "monitor/monitor.h"
#include "monitor/expr.h"
#include "monitor/watchpoint.h"
#include "nemu.h"

#include <stdlib.h>
#include <readline/readline.h>
#include <readline/history.h>

void cpu_exec(uint64_t);

/* We use the `readline' library to provide more flexibility to read from stdin. */
char *rl_gets()
{
  static char *line_read = NULL;

  if (line_read)
  {
    free(line_read);
    line_read = NULL;
  }

  line_read = readline("(nemu) ");

  if (line_read && *line_read)
  {
    add_history(line_read);
  }

  return line_read;
}

static int cmd_c(char *args)
{
  cpu_exec(-1);
  return 0;
}

static int cmd_q(char *args)
{
  return -1;
}

static int cmd_help(char *args);

static int cmd_si(char *args);

static int cmd_info(char *args);

static int cmd_x(char *args);

static int cmd_p(char *args);

static int cmd_w(char *args);

static int cmd_d(char *args);

static struct
{
  char *name;
  char *description;
  int (*handler)(char *);
} cmd_table[] = {
    {"help", "Display informations about all supported commands", cmd_help},
    {"c", "Continue the execution of the program", cmd_c},
    {"q", "Exit NEMU", cmd_q},
    {"si", "Single step", cmd_si},
    {"info", "Print state", cmd_info},
    {"x", "Scan memory", cmd_x},
    {"p", "Evaluate expression", cmd_p},
    {"w", "Set watchpoints", cmd_w},
    {"d", "Delete watchpoints", cmd_d}

    /* TODO: Add more commands */

};

#define NR_CMD (sizeof(cmd_table) / sizeof(cmd_table[0]))

static int cmd_help(char *args)
{
  /* extract the first argument */
  char *arg = strtok(NULL, " ");
  int i;

  if (arg == NULL)
  {
    /* no argument given */
    for (i = 0; i < NR_CMD; i++)
    {
      printf("%s - %s\n", cmd_table[i].name, cmd_table[i].description);
    }
  }
  else
  {
    for (i = 0; i < NR_CMD; i++)
    {
      if (strcmp(arg, cmd_table[i].name) == 0)
      {
        printf("%s - %s\n", cmd_table[i].name, cmd_table[i].description);
        return 0;
      }
    }
    printf("Unknown command '%s'\n", arg);
  }
  return 0;
}

static int cmd_si(char *args)
{
  char *arg = strtok(NULL, " ");
  if (arg == NULL)
  {
    cpu_exec(1);
    return 0;
  }
  else
  {
    cpu_exec(atoi(arg));
  }
  return 0;
}

static int cmd_info(char *args)
{
  char *arg = strtok(NULL, " ");
  if (arg == NULL)
  {
    printf("please use command \"help\" to know more about \"info\"");
    return 0;
  }
  else if (strcmp(arg, "r") == 0)
  {
    printf("eax\t0x%04X\n", cpu.eax);
    printf("ebp\t0x%04X\n", cpu.ebp);
    printf("ebx\t0x%04X\n", cpu.ebx);
    printf("ecx\t0x%04X\n", cpu.ecx);
    printf("edi\t0x%04X\n", cpu.edi);
    printf("edx\t0x%04X\n", cpu.edx);
    printf("eip\t0x%04X\n", cpu.eip);
    printf("esi\t0x%04X\n", cpu.esi);
    printf("esp\t0x%04X\n", cpu.esp);
  }
  else if (strcmp(arg, "w") == 0){
    print_wp();
  }
  return 0;
}

static int cmd_x(char *args)
{
  char *n = strtok(NULL, " ");
  char *arg = strtok(NULL, " ");
  int i;
  uint32_t mem;
  vaddr_t addr;
  if (n == NULL || arg == NULL)
  {
    printf("please use command \"help\" to know more about \"x\"");
    return 0;
  }
  else
  {
    addr = strtol(arg, NULL, 16);
    for (i = 0; i < atoi(n); i++)
    {
      mem = vaddr_read(addr, 4);
      printf("0x%x\t0x%x\n", addr, mem);
      addr += 4;
    }
  }
  return 0;
}

static int cmd_p(char *args)
{
  //printf("%s\n",args);
  bool success = true;
  uint32_t res = expr(args, &success);
  if (success)
  {
    printf("%d\n", res);
  }
  else
  {
    printf("The expression is invalid!\n");
  }
  return 0;
}

static int cmd_w(char *args)
{
  bool success = true;
  uint32_t res = 0;
  res = expr(args, &success);
  if (success)
  {
    WP *wp = new_wp();
    (*wp).old_value = res;
    strcpy((*wp).exp, args);
  }
  else
  {
    printf("The wp expression is invalid!\n");
  }
  return 0;
}

static int cmd_d(char *args)
{
  char *n = strtok(NULL, " ");
  int no = atoi(n);
  free_wp(no);
  return 0;
}

void ui_mainloop(int is_batch_mode)
{
  if (is_batch_mode)
  {
    cmd_c(NULL);
    return;
  }

  for (char *str; (str = rl_gets()) != NULL;)
  {
    char *str_end = str + strlen(str);

    /* extract the first token as the command */
    char *cmd = strtok(str, " ");
    if (cmd == NULL)
    {
      continue;
    }

    /* treat the remaining string as the arguments,
     * which may need further parsing
     */
    char *args = cmd + strlen(cmd) + 1;
    if (args >= str_end)
    {
      args = NULL;
    }

#ifdef HAS_IOE
    extern void sdl_clear_event_queue(void);
    sdl_clear_event_queue();
#endif

    int i;
    for (i = 0; i < NR_CMD; i++)
    {
      if (strcmp(cmd, cmd_table[i].name) == 0)
      {
        if (cmd_table[i].handler(args) < 0)
        {
          return;
        }
        break;
      }
    }

    if (i == NR_CMD)
    {
      printf("Unknown command '%s'\n", cmd);
    }
  }
}
