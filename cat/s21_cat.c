#include <stdio.h>
#include <stdlib.h>
//#include <string.h>
#include <getopt.h>

typedef struct options {
  int flagb;
  int flage;
  int flagn;
  int flags;
  int flagt;
  int flagv;
  int line;
} opt;

int parser(int atgc, char **argv, opt *options, struct option *long_options);
int output(char **argv, opt *options);
char print_flag_s(FILE *t, char cur, char next_cur, opt *options);
char print_flag_v(int cur, opt *options, char next_cur);
char print_n_flag(opt *options, char cur, char next_cur);

int main(int argc, char **argv) {
  int res = 0;
  struct option long_options[] = {{"number", 0, 0, 'n'},
                                  {"squeeze-blank", 0, 0, 's'},
                                  {"number-nonblank", 0, 0, 'b'},
                                  {0, 0, 0, 0}};
  opt options = {0};
  if (argc > 1) {
    res = parser(argc, argv, &options, long_options);
    if (options.flagb) {
      options.flagn = 0;
    }
    if (res > 0) {
      switch (res) {
      case 1:
        fprintf(stderr, "usage: cat [-benstvTE] [file ...]\n");
        break;
      case 2:
        fprintf(stderr, "Usage: %s [-%s] %s [file...]\n", argv[0], "+benstvTE",
                "[--number-nonblank] [--number] [--squeeze-blank]");
        break;
      }
    } else {
      while (optind < argc) {
        res = output(argv, &options);
        if (res > 0) {
          fprintf(stderr, "No such file or directory\n");
          break;
          break;
        }
        optind++;
      }
    }
  }
  return res;
}

int parser(int argc, char **argv, opt *options, struct option *long_options) {
  int option_index = 0;
  int opt;
  int res = 0;
  while ((opt = getopt_long(argc, argv, "+benstvTE", long_options,
                            &option_index)) != -1) {
    switch (opt) {
    case 'b':
      options->flagb = 1;
      break;
    case 'e':
      options->flage = 1;
      options->flagv = 1;
      break;
    case 'n':
      options->flagn = 1;
      break;
    case 's':
      options->flags = 1;
      break;
    case 't':
      options->flagt = 1;
      options->flagv = 1;
      break;
    case 'v':
      options->flagv = 1;
      break;
    case 'T':
      options->flagt = 1;
      break;
    case 'E':
      options->flage = 1;
      break;
    case '?':
      res = 1;
      break;
    }
    if (optind >= argc) {
      res = 2;
    }
  }
  return res;
}

int output(char **argv, opt *options) {
  int res = 0;
  FILE *t = fopen(argv[optind], "r");
  int next_cur = '\0';
  int counter = 0;
  int str_count = 0;
  options->line = 1;
  if (t != NULL) {
    int cur = fgetc(t);
    if (options->flagn) {
      printf("%6.d\t", options->line);
    }
    while (cur != EOF) {
      next_cur = fgetc(t);
      if (options->flagb) {
        if (cur != '\n') {
          if (counter == 0) {
            printf("%6d\t", ++str_count);
            counter = 1;
          }
        } else {
          counter = 0;
        }
      }
      if (options->flags) {
        next_cur = print_flag_s(t, cur, next_cur, options);
      }
      if (options->flage && cur == '\n') {
        printf("$");
      }
      if (options->flagt && cur == '\t') {
        printf("^I");
      } else if (options->flagv) {
        print_flag_v(cur, options, next_cur);
      } else {
        printf("%c", cur);
        if (options->flagn) {
          print_n_flag(options, cur, next_cur);
        } else if (options->flagv) {
          print_flag_v(cur, options, next_cur);
        }
      }
      cur = next_cur;
    }
  } else {
    res = 3;
  }
  fclose(t);
  return res;
}

char print_flag_s(FILE *t, char cur, char next_cur, opt *options) {
  if (cur == '\n' && next_cur == '\n') {
    while (next_cur == '\n') {
      next_cur = fgetc(t);
    }
    if (options->flage) {
      printf("$");
    }
    printf("\n");
    print_n_flag(options, cur, next_cur);
  }
  return next_cur;
}

char print_flag_v(int cur, opt *options, char next_cur) {
  if ((cur >= 0) && (cur < 32) && (cur != '\t') && (cur != '\n')) {
    printf("^");
    cur += 64;
  }
  if (cur == 127) {
    printf("^?");
  } else {
    printf("%c", cur);
    print_n_flag(options, cur, next_cur);
  }
  return 0;
}

char print_n_flag(opt *options, char cur, char next_cur) {
  if ((options->flagn) && (cur == '\n') && (next_cur != EOF)) {
    printf("%6d\t", ++options->line);
  }
  return 0;
}
