#include "nemu.h"
#include <stdlib.h>

/* We use the POSIX regex functions to process regular expressions.
 * Type 'man regex' for more information about POSIX regex functions.
 */
#include <sys/types.h>
#include <regex.h>

enum
{
  TK_NOTYPE = 256,
  TK_LA,
  TK_EQ,
  TK_NEQ,
  TK_DEFER,
  TK_NEG,
  TK_DECNUM,
  TK_HEXNUM,
  TK_REG
  /* TODO: Add more token types */
};

static struct rule
{
  char *regex;
  int token_type;
} rules[] = {

    /* TODO: Add more rules.
   * Pay attention to the precedence level of different rules.
   */

    {" +", TK_NOTYPE}, // spaces
    {"\\)", ')'},      //right parenthsis
    {"\\(", '('},      //left parenthsis
    {"&&", TK_LA},     //logical and
    {"!=", TK_NEQ},
    {"==", TK_EQ}, // equal
    {"\\+", '+'},  // plus
    {"\\-", '-'},  // minus
    {"\\*", '*'},  // multiply
    {"/", '/'},    //divide
    {"[1-9][0-9]*", TK_DECNUM},
    {"0x[0-9]+", TK_HEXNUM},
    {"\\$[a-z][a-z][a-z]", TK_REG}};

#define NR_REGEX (sizeof(rules) / sizeof(rules[0]))

static regex_t re[NR_REGEX];
bool is_defer_neg(int);
uint32_t get_reg(char *);
uint32_t eval(int, int, bool *);

/* Rules are used for many times.
 * Therefore we compile them only once before any usage.
 */
void init_regex()
{
  int i;
  char error_msg[128];
  int ret;

  for (i = 0; i < NR_REGEX; i++)
  {
    ret = regcomp(&re[i], rules[i].regex, REG_EXTENDED);
    if (ret != 0)
    {
      regerror(ret, &re[i], error_msg, 128);
      panic("regex compilation failed: %s\n%s", error_msg, rules[i].regex);
    }
  }
}

typedef struct token
{
  int type;
  char str[32];
} Token;

Token tokens[32];
int nr_token;

static bool make_token(char *e)
{
  int position = 0;
  int i;
  regmatch_t pmatch;

  nr_token = 0;

  while (e[position] != '\0')
  {
    /* Try all rules one by one. */
    for (i = 0; i < NR_REGEX; i++)
    {
      if (regexec(&re[i], e + position, 1, &pmatch, 0) == 0 && pmatch.rm_so == 0)
      {
        char *substr_start = e + position;
        int substr_len = pmatch.rm_eo;

        Log("match rules[%d] = \"%s\" at position %d with len %d: %.*s",
            i, rules[i].regex, position, substr_len, substr_len, substr_start);
        position += substr_len;

        /* TODO: Now a new token is recognized with rules[i]. Add codes
         * to record the token in the array `tokens'. For certain types
         * of tokens, some extra actions should be performed.
         */
        switch (rules[i].token_type)
        {
        case TK_EQ:
        case TK_LA:
        case TK_NEQ:
        case '+':
        case '-':
        case '*':
        case '/':
        case ')':
        case '(':
          tokens[nr_token++].type = rules[i].token_type;
          break;
        case TK_DECNUM:
        case TK_HEXNUM:
          tokens[nr_token].type = rules[i].token_type;
          strncpy(tokens[nr_token++].str, e + position - substr_len, (substr_len > 32) ? 32 : substr_len);
          break;
        case TK_REG:
          tokens[nr_token].type = rules[i].token_type;
          strcpy(tokens[nr_token++].str, e + position - substr_len);
          break;
        default:
          TODO();
        }
        break;
      }
    }

    if (i == NR_REGEX)
    {
      printf("no match at position %d\n%s\n%*.s^\n", position, e, position, "");
      return false;
    }
  }
  for(i=0;i<nr_token;i++){
    Log("%d\t%s",tokens[i].type,tokens[i].str);
  }
  return true;
}

bool is_defer_neg(int tp)
{
  return (tp == '-') || (tp == '+') || (tp == '/') || (tp == '*') || (tp == '(') || (tp == TK_EQ) || (tp == TK_LA) || (tp = TK_NEQ);
}

int check_parentheses(int p, int q)
{
  int i;
  int cnt = 0;
  for (i = p; i <= q; i++)
  {
    if (tokens[i].type == '(')
      cnt++;
    else if (tokens[i].type == ')')
      cnt--;
    if (cnt < 0)
      return -1;
  }
  if(cnt == 0){
    if(tokens[p].type == '('&& tokens[q].type == ')'){
      return 0;
    }
    else 
      return 1;
  }
  return -1;
}

uint32_t get_reg(char *str)
{
  if (strcmp(str, "eax"))
    return cpu.eax;
  else if (strcmp(str, "ecx"))
    return cpu.ecx;
  else if (strcmp(str, "edx"))
    return cpu.edx;
  else if (strcmp(str, "ebx"))
    return cpu.ebx;
  else if (strcmp(str, "esp"))
    return cpu.esp;
  else if (strcmp(str, "ebp"))
    return cpu.ebp;
  else if (strcmp(str, "esi"))
    return cpu.esi;
  else if (strcmp(str, "edi"))
    return cpu.edi;
  else if (strcmp(str, "eip"))
    return cpu.eip;
  return 0;
}

uint32_t eval(int p, int q, bool *success)
{
  *success = true;
  if (p > q)
  {
    Log("p=%d>q=%d\tThe expression is invalid",p,q);
    *success = false;
    return 0;
  }
  else if (p == q)
  {
    if (tokens[p].type == TK_DECNUM)
      return atoi(tokens[p].str);
    else if (tokens[p].type == TK_HEXNUM)
      return strtol(tokens[p].str, NULL, 16);
    else if (tokens[p].type == TK_REG)
    {
      return get_reg(tokens[p].str);
    }
    else
    {
      Log("p=%d==q=%d\tThe expression is invalid",p,q);
      *success = false;
      return 0;
    }
  }
  else if (check_parentheses(p, q) == 0)
  {
    /* The expression is surrounded by a matched pair of parentheses.
     * If that is the case, just throw away the parentheses.
     */
    return eval(p + 1, q - 1, success);
  }
  else if(check_parentheses(p,q) == -1){
    Log("Parenthese didn't match");
    *success = false;
    return 0;
  }
  else 
  {
    int i = p;
    int key_op = 0;
    int op = p;
    int cnt = 0;
    uint32_t val1=0;
    uint32_t val2=0;
    if (tokens[p].type == TK_DEFER || tokens[p].type == TK_NEG)
    {
      if (p == q - 1)
      {
        if (tokens[p].type == TK_DECNUM)
        {
          key_op = tokens[p].type;
          val2 = atoi(tokens[p].str);
        }
        else if (tokens[p].type == TK_HEXNUM)
        {
          key_op = tokens[p].type;
          val2 = (uint32_t) strtol(tokens[p].str, NULL, 16);
        }
        else if (tokens[p].type == TK_REG)
        {
          key_op = tokens[p].type;
          val2 = get_reg(tokens[p].str);
        }
        else
        {
          Log("DEFER_NEG\tThe expression is invalid");
          *success = false;
          return 0;
        }
      }
      else if (check_parentheses(p + 1, q)==0)
      {
        key_op = tokens[i].type;
        val2 = eval(p + 1, q - 1, success);
      }
    }

    else 
    {
      for (i = p; i <= q; i++)
      {
        switch (tokens[i].type)
        {
        case '(':
          if ((cnt == 0) && (key_op != 0))
          {
            i = q + 1; //break out of the loop
          }
          else
            cnt++;
          break;

        case ')':
          cnt--;
          break;

        case TK_LA:
          if (cnt == 0)
          {
            key_op = tokens[i].type;
            op = i;
          }
          break;

        case TK_EQ:
        case TK_NEQ:
          if (cnt == 0 && ((key_op > TK_NEQ)|| (key_op == 0)))
          {
            key_op = tokens[i].type;
            op = i;
          }
          break;

        case '+':
        case '-':
          if (cnt == 0 && ((key_op == '*') || (key_op == '/') || (key_op == 0)))
          {
            key_op = tokens[i].type;
            op = i;
          }
          break;

        case '*':
        case '/':
          if ((cnt == 0) && (key_op == 0))
          {
            key_op = tokens[i].type;
            op = i;
          }
          break;

        default:
          break;
        }
      }
      val1 = eval(p, op - 1, success);
      val2 = eval(op + 1, q, success);
    }

    switch (key_op)
    {
    case '+':
      return val1 + val2;
    case '-':
      return val1 - val2;
    case '*':
      return val1 * val2;
    case TK_EQ:
      return val1 == val2;
    case TK_NEQ:
      return val1 != val2;
    case TK_LA:
      return val1 && val2;

    case '/':
      if (val2 == 0)
      {
        Log("Divide zero!!");
        *success = false;
        return 0;
      }
      else
        return val1 / val2;

    case TK_NEG:
      return -val2;

    case TK_DEFER:
      return vaddr_read(val2, 4);

    default:
      Log("key_op = %d is invalid",key_op);
      *success = false;
      return 0;
      break;
    }
  }
}

uint32_t expr(char *e, bool *success)
{
  if (!make_token(e))
  {
    *success = false;
    return 0;
  }

  /* TODO: Insert codes to evaluate the expression. */
  //TODO();
  Log("Now begin to check defer and negtive!\n");
  int i;
  for (i = 0; i < nr_token; i++)
  {
    if (tokens[i].type == '-' && (i == 0 || is_defer_neg(tokens[i - 1].type))){
      tokens[i].type = TK_NEG;
      Log("position %d is negtive!\n",i);
    }
      
    else if (tokens[i].type == '*' && (i == 0 || is_defer_neg(tokens[i - 1].type))){
      tokens[i].type = TK_DEFER;
      Log("position %d is defer!\n",i);
    }     
  }
  Log("It's time to get the result!\n");
  return eval(0, nr_token, success);
}