#include <stdio.h>
#include <string.h>
#include <regex.h>
#include <getopt.h>
#include <stdlib.h>

typedef struct options {
  int flag_e;
  int flag_i;
  int flag_v;
  int flag_c;
  int flag_l;
  int flag_n;
  int flag_h;
  int flag_s;
  int flag_f;
  int flag_o;
  char patterns[4096];
  int pattern_flag;
  int file_count;
} opt;

int parser (int argc, char **argv, opt *options);
int flag_f_patterns(char **argv, opt *options);
int output(char **argv, opt *options);
void print_flag_o(char *tmp_line, regex_t regex, opt *options, int str_count, char **argv);

int main(int argc, char** argv) {
    int res = 0;
    opt options = {0};
    if (argc > 2) {
        parser(argc, argv, &options);
        if (res == 0) {
            if (options.pattern_flag == 0) {
                strcpy(options.patterns, argv[optind]); //sprintf??
                optind++;
            }
            if (optind + 1 < argc) {
                options.file_count = 1;
            }
            while (optind < argc) {
                output(argv, &options);
                optind++;
            }
        } else {
            if (!options.flag_s) {
            switch(res) {
                case 1: fprintf(stderr, "grep: %s: No such file or directory\n",
                                argv[optind]);
                    break;
                case 2: fprintf(stderr, "grep: invalid option/n usage: grep [-abcDEFGHhIiJLlmnOoqRSsUVvwxZ] [-A num] [-B num] [-C[num]] [-e pattern] [-f file] [--binary-files=value] [--color=when] [--context[=num]] [--directories=action] [--label] [--line-buffered] [--null] [pattern] [file ...]");
            }
            }
        }
    }
    return 0;
}

int parser (int argc, char **argv, opt *options) {
    int res = 0;
    int opt;
    while ((opt = (getopt_long(argc, argv, "e:ivclnhsf:o", 0,0))) != -1) {
        switch (opt) {
            case 'e':
                options->flag_e = 1;
                if (options->pattern_flag > 0) {
                    strcat(options->patterns, "|");
                }
                strcat(options->patterns, optarg);
                options->pattern_flag++;
                break;
            case 'i':
                options->flag_i = 1;
                break;
            case 'v':
                options->flag_v = 1;
                break;
            case 'c':
                options->flag_c = 1;
                break;
            case 'l':
                options->flag_l = 1;
                break;
            case 'n':
                options->flag_n = 1;
                break;
            case 'h':
                options->flag_h = 1;
                break;
            case 's':
                options->flag_s = 1;
                break;
            case 'f':
              options->flag_f = 1;
                flag_f_patterns(argv, options);
              break;
            case 'o':
              options->flag_o = 1;
              break;
        case '?':
          res = 2;
          break;
        }
    }
    return res;
}

int flag_f_patterns(char **argv, opt *options) {
    int res = 0;
    int cur;
    unsigned int length_pattern = 0;
    int flag_f_pattern = 0;
    FILE *fl = fopen(argv[optind-1], "r");
    if (fl != NULL) {
        if (options->pattern_flag > 0)
            strcat(options->patterns, "|");
        while ((cur = fgetc(fl)) != EOF) {
            length_pattern = strlen(options->patterns);
            if ((cur == '\0') || (cur == '\n')) {
                if (flag_f_pattern == 0) {
                    strcat(options->patterns, "|");
                    flag_f_pattern = 1;
                } else {
                    strcat(options->patterns, ".");
                }
            } else {
                options->patterns[length_pattern] = cur;
                flag_f_pattern = 0;
            }
        }
        options->file_count++;
    } else {
        res = 1;
    }
    fclose(fl);
    return res;
}

int output(char **argv, opt *options) {
    FILE *file_name = fopen(argv[optind], "r");
    regex_t regex;
      size_t len;
      char *tmp_line = NULL;
      int cflags = REG_EXTENDED;
      int success;
      int str_count = 0;
      int str_match = 0;
    int i;
    if (options->flag_i) cflags = REG_EXTENDED | REG_ICASE;
    if (file_name) {
        if (regcomp(&regex, options->patterns, cflags) == 0) {
            while (getline(&tmp_line, &len, file_name) != -1) {
                str_count++;
                if ((options->flag_o == 1) && (options->flag_l == 0) && (options->flag_v == 0) && (options->flag_c == 0)) {
                    print_flag_o(tmp_line, regex, options, str_count, argv);
                } else {
                    if (tmp_line) {
                        success = regexec(&regex, tmp_line, 0, NULL, 0);
                        if (((success == 0) && (options->flag_v == 0)) ||
                                        ((success == 1) && (options->flag_v == 1))) {
                                      str_match++;
                                      i = 0;
                            if (options->flag_c == 0) {
                                if (options->flag_l == 1) {
                                    printf("%s\n", argv[optind]);
                                    break;
                                }
                                if (((options->file_count == 1) && (options->flag_h == 0))) {
                                    printf("%s:", argv[optind]);
                                  }
                                if (options->flag_n == 1) {
                                    printf("%d:", str_count);
                                }
                                while (tmp_line[i]) {
                                    if (tmp_line[i] != '\n')
                                        printf("%c", tmp_line[i]);
                                    i++;
                                }
                                printf("\n");
                            }
                    }
                }
            }
        }
            free(tmp_line);
            tmp_line = NULL;
            len = 0;
    }
        regfree(&regex);
    } else {
        if(!options->flag_s) {
            fprintf(stderr, "grep: %s: No such file or directory\n", argv[optind]);
        }
    }
    if ((options->flag_c == 1) && (str_match == 0)) {
        if (options->file_count == 1) {
          if (options->flag_h == 0) {
            printf("%s:%d\n", argv[optind], str_match);
          } else {
            printf("%d\n", str_match);
          }
        } else {
          if (options->flag_l == 1) printf("%d\n", str_match);
          if ((options->flag_l = 0)) printf("%d\n", str_match);
        }
      }
      if ((options->flag_c == 1) && (str_match > 0)) {
        if (options->file_count == 1) {
          if (options->flag_l == 1) {
            if (options->flag_h == 0) {
              printf("%s:1\n%s\n", argv[optind], argv[optind]);
            } else if (options->flag_h == 1) {
              printf("1\n%s\n", argv[optind]);
            }
          } else if (options->flag_h == 1) {
            printf("%d\n", str_match);
          } else {
            printf("%s:%d\n", argv[optind], str_match);
          }
        } else {
          if (options->flag_l == 1) printf("1\n%s\n", argv[optind]);
          if (options->flag_l == 0) printf("%d\n", str_match);
        }
      }
      fclose(file_name);
    return 0;
}

void print_flag_o(char *tmp_line, regex_t regex, opt *options, int str_count, char **argv) {
    regmatch_t pmatch[1];
    int cur = 0;
    while (regexec(&regex, tmp_line, 1, pmatch, 0)) {
        if (cur == 0) {
            if (options->file_count == 1) {
                if ((options->flag_h == 0) && (options->flag_n == 0)) printf("%s:", argv[optind]);
                        if ((options->flag_h == 0) && (options->flag_n == 1)) printf("%s:", argv[optind]);
                        if ((options->flag_h == 1) && (options->flag_n == 1)) printf("%d:", str_count);
                        if ((options->flag_h == 0) && (options->flag_n == 1)) printf("%d:", str_count);
            } else {
                if (options->flag_n == 1) printf("%d:", str_count);
            }
        }
        for (int i = pmatch[0].rm_so; i < pmatch[0].rm_eo; i++) {
            printf("%c", tmp_line[i]);
            cur++;
        }
        printf("\n");
        tmp_line = tmp_line + pmatch[0].rm_eo;
    }
}
