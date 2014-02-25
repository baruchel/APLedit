#define VERSION "1.0"
#define COPYRIGHT "(c) 2014"
#define INITFILE "~/.inputrc"

#define _GNU_SOURCE

#include <stdlib.h>
#include <dlfcn.h>

#ifdef READLINE_LIBRARY
#  include "readline.h"
#else
#  include <readline/readline.h>
#endif

char prompt1[64], prompt_buf[64];
char *prompt2, *color;

int mode = 0;
int category = 0;
int item = 0;
char *aplchar;

int  change_mode(int, int);
int  change_category_up(int, int);
int  change_category_down(int, int);
int  change_item_up(int, int);
int  change_item_down(int, int);
int  insert_aplchar(int, int);
char *get_prompt(void);
char *(*original_readline)(const char*);

char *readline (const char *prompt) {
  static int first = 0;

  if (0 == first) {
    first = 1;
    original_readline = dlsym(RTLD_NEXT, "readline");
    rl_readline_name = "APLedit";
    rl_add_defun("apledit-mode", change_mode, -1);
    rl_add_defun("apledit-category-up", change_category_up, -1);
    rl_add_defun("apledit-category-down", change_category_down, -1);
    rl_add_defun("apledit-item-up", change_item_up, -1);
    rl_add_defun("apledit-item-down", change_item_down, -1);
    rl_add_defun("apledit-insert-char", insert_aplchar, -1);
    /* Quick and dirty use of prompt2 variable for something else */
    prompt2 = getenv("APLEDIT_INITFILE");
    if (NULL == prompt2) {
      rl_read_init_file(INITFILE);
    } else {
      rl_read_init_file(prompt2);
    }
    /* Now prompt2 is used as it has to be */
    prompt2 = getenv("APLEDIT_PROMPT");
    if (NULL == prompt2) {
      prompt2 = "[%s %s] \0";
    }
    color = getenv("APLEDIT_COLOR_BANNER");
    if (NULL == color) {
      color = "\0";
    }
    if(!getenv("APLEDIT_NOBANNER")) {
      fprintf(stderr, "  %sAPLedit %s by Th. Baruchel %s.\n",
        color,VERSION, COPYRIGHT);
      fprintf(stderr, "  Released under the terms of the GNU Public License.\n");
      fprintf(stderr,"\n");
    }
    color = getenv("APLEDIT_COLOR");
    if (NULL == color) {
      color = "\0";
    }
  }

  strcpy(prompt1,prompt);

  fprintf(stderr,"%s",color);
  return (*original_readline)(get_prompt());
}

int change_mode(int count, int key)
{
  /* toggle the mode */
  mode = !mode;
  rl_message(get_prompt());
  return(0);
}

int change_category_up(int count, int key)
{
  if (mode==1) {
    category--;
    item = 0;
    if(category<0) {
      category = 6;
    }
    rl_message(get_prompt());
  }
  return(0);
}

int change_category_down(int count, int key)
{
  if (mode==1) {
    category++;
    item = 0;
    if(category>6) {
      category = 0;
    }
    rl_message(get_prompt());
  }
  return(0);
}

int change_item_up(int count, int key)
{
  if (mode==1) {
    item--;
    switch(category) {
      case 0:
        if (item<0) item = 14;
        break;
      case 1:
        if (item<0) item = 11;
        break;
      case 2:
      case 3:
        if (item<0) item = 10;
        break;
      case 4:
        if (item<0) item = 4;
        break;
      case 5:
        if (item<0) item = 13;
        break;
      case 6:
        if (item<0) item = 8;
        break;
    }
    rl_message(get_prompt());
  }
  return(0);
}

int change_item_down(int count, int key)
{
  if (mode==1) {
    item++;
    switch(category) {
      case 0:
        if (item>14) item = 0;
        break;
      case 1:
        if (item>11) item = 0;
        break;
      case 2:
      case 3:
        if (item>10) item = 0;
        break;
      case 4:
        if (item>4) item = 0;
        break;
      case 5:
        if (item>13) item = 0;
        break;
      case 6:
        if (item>8) item = 0;
        break;
    }
    rl_message(get_prompt());
  }
  return(0);
}

int insert_aplchar(int count, int key) {
  if (mode==1) {
    rl_insert_text(aplchar);
    rl_refresh_line(0, 0);
  }
  return(0);
}

char * get_prompt(void)
{
  char catname[7][7] = {
    "Math. \0", "Logic.\0", "Struc.\0",
    "Selec.\0", "Searc.\0", "Misce.\0", "Opera.\0"
  };

  if (mode==0) {
    return prompt1;
  }

  switch(category) {
    case 0: /* math */
      switch(item) {
        case 0: /* addition */
          aplchar = "+\0";
          break;
        case 1: /* substraction */
          aplchar = "-\0";
          break;
        case 2: /* division */
          aplchar = "\xc3\xb7\0";
          break;
        case 3: /* multiplication */
          aplchar = "\xc3\x97\0";
          break;
        case 4: /* */
          aplchar = "\xe2\x8c\x88\0";
          break;
        case 5: /* */
          aplchar = "\xe2\x8c\x8a\0";
          break;
        case 6: /* */
          aplchar = "*\0";
          break;
        case 7: /* */
          aplchar = "!\0";
          break;
        case 8: /* */
          aplchar = "|\0";
          break;
        case 9: /* */
          aplchar = "\xe2\x8d\x9f\0";
          break;
        case 10: /* */
          aplchar = "\xe2\x97\x8b\0";
          break;
        case 11: /* */
          aplchar = "\xe2\x8c\xb9\0";
          break;
        case 12: /* */
          aplchar = "\xe2\x8a\xa5\0";
          break;
        case 13: /* */
          aplchar = "\xe2\x8a\xa4\0";
          break;
        case 14: /* */
          aplchar = "?\0";
          break;
      }
      break;
    case 1: /* logical and comparison */
      switch(item) {
        case 0: /* */
          aplchar = "\xe2\x88\xa7\0";
          break;
        case 1: /* */
          aplchar = "\xe2\x88\xa8\0";
          break;
        case 2: /* */
          aplchar = "\xe2\x8d\xb2\0";
          break;
        case 3: /* */
          aplchar = "\xe2\x8d\xb1\0";
          break;
        case 4: /* */
          aplchar = "<\0";
          break;
        case 5: /* */
          aplchar = ">\0";
          break;
        case 6: /* */
          aplchar = "\xe2\x89\xa4\0";
          break;
        case 7: /* */
          aplchar = "\xe2\x89\xa5\0";
          break;
        case 8: /* */
          aplchar = "=\0";
          break;
        case 9: /* */
          aplchar = "\xe2\x89\xa0\0";
          break;
        case 10: /* */
          aplchar = "\xe2\x89\xa1\0";
          break;
        case 11: /* */
          aplchar = "\xe2\x89\xa2\0";
          break;
      }
      break;
    case 2: /* structural */
      switch(item) {
        case 0: /* */
          aplchar = "\xe2\x8d\xb4\0";
          break;
        case 1: /* */
          aplchar = ",\0";
          break;
        case 2: /* */
          aplchar = "\xe2\x8d\xaa\0";
          break;
        case 3: /* */
          aplchar = "\xe2\x8c\xbd\0";
          break;
        case 4: /* */
          aplchar = "\xe2\x8a\x96\0";
          break;
        case 5: /* */
          aplchar = "\xe2\x8d\x89\0";
          break;
        case 6: /* */
          aplchar = "\xe2\x86\x91\0";
          break;
        case 7: /* */
          aplchar = "\xe2\x86\x93\0";
          break;
        case 8: /* */
          aplchar = "\xe2\x8a\x82\0";
          break;
        case 9: /* */
          aplchar = "\xe2\x89\xa1\0";
          break;
        case 10: /* */
          aplchar = "\xe2\x88\x8a\0";
          break;
      }
      break;
    case 3: /* selection and set operations */
      switch(item) {
        case 0: /* */
          aplchar = "\xe2\x8c\xb7\0";
          break;
        case 1: /* */
          aplchar = "\xe2\x8a\x83\0";
          break;
        case 2: /* */
          aplchar = "/\0";
          break;
        case 3: /* */
          aplchar = "\xe2\x8c\xbf\0";
          break;
        case 4: /* */
          aplchar = "\\\0";
          break;
        case 5: /* */
          aplchar = "\xe2\x8d\x80\0";
          break;
        case 6: /* */
          aplchar = "~\0";
          break;
        case 7: /* */
          aplchar = "\xe2\x88\xaa\0";
          break;
        case 8: /* */
          aplchar = "\xe2\x88\xa9\0";
          break;
        case 9: /* */
          aplchar = "\xe2\x8a\xa3\0";
          break;
        case 10: /* */
          aplchar = "\xe2\x8a\xa2\0";
          break;
      }
      break;
    case 4: /* search and sort */
      switch(item) {
        case 0: /* */
          aplchar = "\xe2\x8d\xb3\0";
          break;
        case 1: /* */
          aplchar = "\xe2\x88\x8a\0";
          break;
        case 2: /* */
          aplchar = "\xe2\x8d\xb7\0";
          break;
        case 3: /* */
          aplchar = "\xe2\x8d\x8b\0";
          break;
        case 4: /* */
          aplchar = "\xe2\x8d\x92\0";
          break;
      }
      break;
    case 5: /* miscellaneous */
      switch(item) {
        case 0: /* */
          aplchar = "\xc2\xaf\0";
          break;
        case 1: /* */
          aplchar = "\\\0";
          break;
        case 2: /* */
          aplchar = "\xe2\x86\x92\0";
          break;
        case 3: /* */
          aplchar = "\xe2\x86\x90\0";
          break;
        case 4: /* */
          aplchar = "\xe2\x8d\xac\0";
          break;
        case 5: /* */
          aplchar = "\xe2\x8d\x8e\0";
          break;
        case 6: /* */
          aplchar = "\xe2\x8d\x95\0";
          break;
        case 7: /* */
          aplchar = "\xe2\x8b\x84\0";
          break;
        case 8: /* */
          aplchar = "\xe2\x8d\x9d\0";
          break;
        case 9: /* */
          aplchar = "\xe2\x88\x87\0";
          break;
        case 10: /* */
          aplchar = "\xe2\x8d\xba\0";
          break;
        case 11: /* */
          aplchar = "\xe2\x8d\xb5\0";
          break;
        case 12: /* */
          aplchar = "\xe2\x8e\x95\0";
          break;
        case 13: /* */
          aplchar = "\xe2\x8d\x9e\0";
          break;
      }
      break;
    case 6: /* operators */
      switch(item) {
        case 0: /* */
          aplchar = "\xc2\xa8\0";
          break;
        case 1: /* */
          aplchar = "\xe2\x8d\xa8\0";
          break;
        case 2: /* */
          aplchar = "\xe2\x8d\xa3\0";
          break;
        case 3: /* */
          aplchar = ".\0";
          break;
        case 4: /* */
          aplchar = "\xe2\x88\x98\0";
          break;
        case 5: /* */
          aplchar = "/\0";
          break;
        case 6: /* */
          aplchar = "\\\0";
          break;
        case 7: /* */
          aplchar = "\xe2\x8c\xbf\0";
          break;
        case 8: /* */
          aplchar = "\xe2\x8d\x80\0";
          break;
      }
      break;
  }

  sprintf(prompt_buf, prompt2, catname[category], aplchar);
  return prompt_buf;
}
