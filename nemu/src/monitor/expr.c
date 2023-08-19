#include "nemu.h"
#include "cpu/reg.h"
#include "memory/memory.h"

#include <stdlib.h>

/* We use the POSIX regex functions to process regular expressions.
 * Type 'man regex' for more information about POSIX regex functions.
 */
#include <sys/types.h>
#include <regex.h>
uint32_t look_up_symtab(char *sym, bool *success);
enum
{
    TK_NOTYPE = 256,
    TK_PLUS = '+',
    TK_SUB = '-',
    TK_MUL = '*',
    TK_DIV = '/',
    TK_AND = '&',
    TK_OR = '|',
    TK_NEQ = '!',
    TK_EQ = '=',
    TK_LPAREN,
    TK_RPAREN,
    TK_REG,
    TK_DEREF,
    TK_NUM,
    TK_HXNUM,
    TK_MINUS,
    TK_SYMB

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

    {" +", TK_NOTYPE},               // spaces
    {"\\+", '+'},                    // plus
    {"-", '-'},                      // minus
    {"\\*", '*'},                    // mul
    {"/", '/'},                      // div
    {"0[xX][0-9a-fA-F]+", TK_HXNUM}, // hex integer
    {"[0-9]+", TK_NUM},              // decimal integer
    {"\\$[a-zA-Z]+", TK_REG},        // regisiter
    {"\\(", TK_LPAREN},              // left paren
    {"\\)", TK_RPAREN},              // right paren
    {"==", TK_EQ},                   // equal
    {"!=", TK_NEQ},                  // not equal
    {"&&", TK_AND},                  //and
    {"\\|\\|", TK_OR},               //or
    {"[A-Za-z_][A-Za-z0-9_]*",TK_SYMB}     //symbol
};

#define NR_REGEX (sizeof(rules) / sizeof(rules[0]))

static regex_t re[NR_REGEX];

/* Rules are used for more times.
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
			assert(ret != 0);
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

				printf("match regex[%d] at position %d with len %d: %.*s\n", i, position, substr_len, substr_len, substr_start);
				position += substr_len;
            if (rules[i].token_type == TK_NOTYPE)
            {
              continue;
            }
            Token new_token;
            new_token.type = rules[i].token_type;
            strncpy(new_token.str, "", 32);
            /* TODO: Now a new token is recognized with rules[i]. Add codes
             * to record the token in the array `tokens'. For certain types
             * of tokens, some extra actions should be performed.
             */
            switch (rules[i].token_type)
            {
            case TK_PLUS:
            case TK_SUB:
            case TK_MUL:
            case TK_DIV:
            case TK_OR:
            case TK_AND:
            case TK_EQ:
            case TK_NEQ:
              new_token.str[1] = '\0';
              new_token.str[0] = rules[i].token_type;
              break;
            case TK_RPAREN:
            case TK_LPAREN:
              new_token.str[1] = '\0';
              new_token.str[0] = rules[i].token_type == TK_LPAREN ? '(' : ')';
              break;
            case TK_NUM:
            case TK_HXNUM:            
            case TK_SYMB:
              if(substr_len >= 32){
                printf("too long token %.*s at %d\n",substr_len,substr_start, position);
                return false;
              }
              strncpy(new_token.str, substr_start, substr_len);
              break;
            case TK_REG:
            if(substr_len >= 5){
                printf("not valid reg name %.*s at %d\n", substr_len-1,substr_start+1, position);
                return false;
            }
              strncpy(new_token.str, substr_start + 1, substr_len - 1);
              break;
            default:
              printf("Not implemented token\n");
              return false;
            }
            tokens[nr_token] = new_token;
            nr_token++;
            break;
          }
        }

		if (i == NR_REGEX)
		{
			printf("no match at position %d\n%s\n%*.s^\n", position, e, position, "");
			return false;
		}
	}

	return true;
}

u_int32_t eval(u_int32_t p, u_int32_t q, bool *success);

u_int32_t expr(char *e, bool *suc)
{
  if (!make_token(e))
  {
    *suc = false;
    return 0;
  }
  for (int i = 0; i < nr_token; i++)
  {
    if ((i == 0 || (tokens[i - 1].type != TK_NUM && tokens[i - 1].type != TK_HXNUM && tokens[i - 1].type != TK_RPAREN && tokens[i - 1].type != TK_REG && tokens[i-1].type != TK_SYMB)))
    {
      if (!strcmp(tokens[i].str, "-"))
      {
        tokens[i].type = TK_MINUS;
      }
      else if (!strcmp(tokens[i].str, "*"))
      {
        tokens[i].type = TK_DEREF;
      }
    }
  }
  bool success;
  /* TODO: Insert codes to evaluate the expression. */
  u_int32_t ret = eval(0, nr_token - 1, &success);
  if (success)
  {
    *suc = success;
    return ret;
  }
  else
  {
    *suc = false;
    return 0;
  }
}

bool check_parentheses(u_int32_t p, u_int32_t q, bool loose)
{
  int i = 0;
  for (u_int32_t j = p; j <= q; j++)
  {
    if (tokens[j].type == TK_LPAREN)
    {
      i++;
    }
    else if (tokens[j].type == TK_RPAREN)
    {
      i--;
    }
    if (i == 0)
    {
      if (!loose && j != q)
      {
        return false;
      }
    }
    if (i < 0)
    {
      return false;
    }
  }
  return i == 0;
}

u_int32_t priority(char c)
{
  if (c == '+' || c == '-')
  {
    return 1;
  }
  else if (c == '*' || c == '/' || c == '&' || c == '|' || c == '!' || c == '=')
  {
    return 2;
  }
  else
  {
    return 114514;
  }
}

u_int32_t mainOp(u_int32_t p, u_int32_t q)
{
  u_int32_t index = p;
  u_int32_t prior = 114514;
  int depth=0;
  bool inParen=false;
  for (int i = p; i < q; i++)
  {
    switch (tokens[i].type)
    {
    case TK_PLUS:
    case TK_SUB:
    case TK_MUL:
    case TK_DIV:
    case TK_NEQ:
    case TK_EQ:
    case TK_AND:
    case TK_OR:
      if (priority(tokens[i].type) <= prior && !inParen)
      {
        index = i;
        prior = priority(tokens[i].type);
      }
      break;
    case TK_LPAREN:
      depth++;
      break;
    case TK_RPAREN:
      depth--;
      break;
    default:
      break;
    }
    inParen = depth!=0;
  }
  return index;
}

u_int32_t eval(u_int32_t p, u_int32_t q, bool *success)
{
  if (p > q)
  {
    *success = false;
    return 0;
  }
  else if (p == q)
  {
    u_int32_t i;
    if (tokens[p].type == TK_NUM)
    {
      sscanf(tokens[p].str, "%u", &i);
    }
    else if (tokens[p].type == TK_HXNUM)
    {
      sscanf(tokens[p].str, "%x", &i);
    }
    else if (tokens[p].type == TK_REG)
    {
      bool suc = false;
      i = get_reg_val(tokens[p].str, &suc);
      if (!suc)
      {
        printf("Oops something is wrong\n");
        *success = false;
        return 0;
      }
    }else if (tokens[p].type == TK_SYMB){
        bool suc = false;
		i = look_up_symtab(tokens[p].str, &suc);
		if(!suc){
		    printf("Unknown symbol : %s\n", tokens[p].str);
		    *success = false;
		    return 0;
		}
    }
    else
    {
      *success = false;
      return 0;
    }
    *success = true;
    if (p > 0 && tokens[p - 1].type == TK_DEREF)
    {
      if (i > 0)
        return i;
      *success = false;
      return 0;
    }
    return i;
  }
  else if (check_parentheses(p, q, false))
  {
    /* The expression is surrounded by a matched pair of parentheses.
     * If that is the case, just throw away the parentheses.
     */
    return eval(p + 1, q - 1, success);
  }
  else
  {
    *success = true;
    u_int32_t val1 = 0;
    u_int32_t op = mainOp(p, q);
    if (op > 0)
    {
      val1 = eval(p, op - 1, success);
    }
    u_int32_t val2 = eval(op + 1, q, success);

    switch (tokens[op].type)
    {
    case '+':
      return val1 + val2;
    case '-':
      return val1 - val2;
    case '*':
      return val1 * val2;
    case '/':
      if (val2)
      {
        return val1 / val2;
      }
      printf("You divide zero, stop.\n");
      *success = false;
      return 0;
    case '=':
      return val1 == val2;
    case '!':
      return val1 != val2;
    case '&':
      return val1 && val2;
    case '|':
      return val1 || val2;
    case TK_DEREF:
      return vaddr_read(val2,1, 4);
    case TK_MINUS:
      return -val2;
    default:
      *success = false;
      printf("Unknown kid : %c\n", tokens[op].type);
      return 0;
    }
  }
}