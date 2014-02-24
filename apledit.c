#include <unistd.h>
#include <stdlib.h>

#include <stdio.h>
#include <sys/types.h>

#include <termios.h>	/* xxx - should make this more general */

#ifdef READLINE_LIBRARY
#  include "readline.h"
#else
#  include <readline/readline.h>
#endif

void process_line(char *line);
int  change_mode(int, int);
int  change_category_up(int, int);
int  change_category_down(int, int);
int  change_item_up(int, int);
int  change_item_down(int, int);
int  insert_aplchar(int, int);
char *get_prompt(void);

int mode = 0;
int category = 0;
int item = 0;
int cursor_pos;
char *aplchar;
char prompt_buf[64], line_buf[4096];
tcflag_t old_lflag;
cc_t     old_vtime;
struct termios term;
char escape = 27;
char *prompt1;
char *prompt2;
char *leave;
FILE *out;
char *filename;

int main(int argc, char **argv) {
    fd_set fds;
    int i;

    aplchar = "\0"; filename = NULL;
    out = stderr;

    prompt1 = getenv("APLEDIT_PROMPT1");
    prompt2 = getenv("APLEDIT_PROMPT2");
    leave = getenv("APLEDIT_LEAVE");

    if(!prompt1) prompt1 = "    \0";
    if(!prompt2) prompt2 = "[%s %s]\0";
    if(!leave) leave = "\0";

    for (i=1;i<argc;i++) {
      if (0 == strcmp(argv[i],"-e")) { out = stderr; filename = NULL; }
      if (0 == strcmp(argv[i],"-o")) { out = stdout; filename = NULL; }
      if (0 == strcmp(argv[i],"-f")) {
        if (i < argc-1) {
          filename = argv[i+1]; i++;
        } else {
          fprintf(stderr,"error: expected file name for -f\n");
          exit(EXIT_FAILURE);
        }
      }
    }

    if(filename) {
      out = fopen(filename,"a");
      if (!out) {
        fprintf(stderr,"error: unable to open the file '%s'\n",filename);
        exit(EXIT_FAILURE);
      }
    }


    /* Adjust the terminal slightly before the handler is installed. Disable
     * canonical mode processing and set the input character time flag to be
     * non-blocking.
     */
    if( tcgetattr(STDIN_FILENO, &term) < 0 ) {
        perror("tcgetattr");
        exit(1);
    }
    old_lflag = term.c_lflag;
    old_vtime = term.c_cc[VTIME];
    term.c_lflag &= ~ICANON;
    term.c_cc[VTIME] = 1;
    /* COMMENT LINE BELOW - see above */
    if( tcsetattr(STDIN_FILENO, TCSANOW, &term) < 0 ) {
        perror("tcsetattr");
        exit(1);
    }

    rl_readline_name = "APLedit";

    rl_add_defun("apledit-mode", change_mode, -1);
    rl_add_defun("apledit-category-up", change_category_up, -1);
    rl_add_defun("apledit-category-down", change_category_down, -1);
    rl_add_defun("apledit-item-up", change_item_up, -1);
    rl_add_defun("apledit-item-down", change_item_down, -1);
    rl_add_defun("apledit-insert-char", insert_aplchar, -1);
    rl_callback_handler_install(get_prompt(), process_line);


    while(1) {
      FD_ZERO(&fds);
      FD_SET(fileno(stdin), &fds);

      if( select(FD_SETSIZE, &fds, NULL, NULL, NULL) < 0) {
        perror("select");
        exit(1);
      }

      if( FD_ISSET(fileno(stdin), &fds) ) {
        rl_callback_read_char();
      }
    }
}

void process_line(char *line)
{
  if( line == NULL ) {
    fprintf(out, "%s\n",leave); fflush(out);

    /* reset the old terminal setting before exiting */
    term.c_lflag     = old_lflag;
    term.c_cc[VTIME] = old_vtime;
    if( tcsetattr(STDIN_FILENO, TCSANOW, &term) < 0 ) {
        perror("tcsetattr");
        exit(1);
    }
    exit(0);
  }

  fprintf(out, "%s\n", line); fflush(out);

  if (line && *line) {
    add_history(line);
  }

  free (line);
}

int change_mode(int count, int key)
{
  /* toggle the mode */
  mode = !mode;

  /* save away the current contents of the line */
  strcpy(line_buf, rl_line_buffer);
  cursor_pos = rl_point;

  /* install a new handler which will change the prompt and erase the current line */
  rl_callback_handler_install(get_prompt(), process_line);

  /* insert the old text on the new line */
  rl_insert_text(line_buf);
  rl_point = cursor_pos;

  /* redraw the current line - this is an undocumented function. It invokes the
   * redraw-current-line command.
   */
  rl_refresh_line(0, 0);
}

int change_category_up(int count, int key)
{
  if (mode==1) {
    category--;
    item = 0;
    if(category<0) {
      category = 6;
    }
    strcpy(line_buf, rl_line_buffer);
    cursor_pos = rl_point;
    rl_callback_handler_install(get_prompt(), process_line);
    rl_insert_text(line_buf);
    rl_point = cursor_pos;
    rl_refresh_line(0, 0);
  }
}

int change_category_down(int count, int key)
{
  if (mode==1) {
    category++;
    item = 0;
    if(category>6) {
      category = 0;
    }
    strcpy(line_buf, rl_line_buffer);
    cursor_pos = rl_point;
    rl_callback_handler_install(get_prompt(), process_line);
    rl_insert_text(line_buf);
    rl_point = cursor_pos;
    rl_refresh_line(0, 0);
  }
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
    strcpy(line_buf, rl_line_buffer);
    cursor_pos = rl_point;
    rl_callback_handler_install(get_prompt(), process_line);
    rl_insert_text(line_buf);
    rl_point = cursor_pos;
    rl_refresh_line(0, 0);
  }
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
    strcpy(line_buf, rl_line_buffer);
    cursor_pos = rl_point;
    rl_callback_handler_install(get_prompt(), process_line);
    rl_insert_text(line_buf);
    rl_point = cursor_pos;
    rl_refresh_line(0, 0);
  }
}

/*
   Variable: char * rl_line_buffer
           This is the line gathered so far. You are welcome to modify the contents of
           the line, but see 2.4.5 Allowing Undoing. The function rl_extend_line_buffer
           is available to increase the memory allocated to rl_line_buffer.

   Variable: int rl_point
           The offset of the current cursor position in rl_line_buffer (the point).

   Variable: int rl_end
           The number of characters present in rl_line_buffer. When rl_point is at the
           end of the line, rl_point and rl_end are equal.

   Function: void rl_extend_line_buffer (int len)
           Ensure that rl_line_buffer has enough space to hold len characters, possibly
           reallocating it if necessary.

   Function: void rl_redisplay (void)
           Change what's displayed on the screen to reflect the current contents of
           rl_line_buffer.
*/

int insert_aplchar(int count, int key) {
  if (mode==1) {
    rl_insert_text(aplchar);
    rl_refresh_line(0, 0);
  }
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
