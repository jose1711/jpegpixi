/* This file has been generated with opag 0.8.0.  */
/* This file is part of jpegpixi, a program to interpolate pixels in
   JFIF image files.
   Copyright (C) 2003, 2004, 2005 Martin Dickopp

   Jpegpixi is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published
   by the Free Software Foundation; either version 2 of the License,
   or (at your option) any later version.

   Jpegpixi is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with jpegpixi; if not, write to the Free Software
   Foundation, 51 Franklin St, Fifth Floor, Boston, MA 02110-1301,
   USA.  */

#ifdef HAVE_CONFIG_H
# include <config.h>
#endif
#include "util.h"

#include <stdio.h>


#ifndef STR_ERR_UNKNOWN_LONG_OPT
# define STR_ERR_UNKNOWN_LONG_OPT   _("%s: unrecognized option `--%s'\n")
#endif

#ifndef STR_ERR_LONG_OPT_AMBIGUOUS
# define STR_ERR_LONG_OPT_AMBIGUOUS _("%s: option `--%s' is ambiguous\n")
#endif

#ifndef STR_ERR_MISSING_ARG_LONG
# define STR_ERR_MISSING_ARG_LONG   _("%s: option `--%s' requires an argument\n")
#endif

#ifndef STR_ERR_UNEXPEC_ARG_LONG
# define STR_ERR_UNEXPEC_ARG_LONG   _("%s: option `--%s' doesn't allow an argument\n")
#endif

#ifndef STR_ERR_UNKNOWN_SHORT_OPT
# define STR_ERR_UNKNOWN_SHORT_OPT  _("%s: unrecognized option `-%c'\n")
#endif

#ifndef STR_ERR_MISSING_ARG_SHORT
# define STR_ERR_MISSING_ARG_SHORT  _("%s: option `-%c' requires an argument\n")
#endif

#define STR_HELP_BLOCKS_FILE "\
  -f, --blocks-file=FILE    read pixel block specifications from FILE\n"

#define STR_HELP_METHOD "\
  -m, --method=METHOD       use interpolation method METHOD [default: linear]\n"

#define STR_HELP_VERBOSE "\
  -v, --verbose             display coordinates and size of each pixel block\n\
                              that is interpolated\n"

#define STR_HELP_INFO "\
  -i, --info                display information about the image\n"

#define STR_HELP_STRIP "\
  -s, --strip               do not copy comment and extra markers\n"

#define STR_HELP_HELP "\
      --help                display this help text and exit\n"

#define STR_HELP_VERSION "\
      --version             display version information and exit\n"

#define STR_HELP _("\
  -f, --blocks-file=FILE    read pixel block specifications from FILE\n\
  -m, --method=METHOD       use interpolation method METHOD [default: linear]\n\
  -v, --verbose             display coordinates and size of each pixel block\n\
                              that is interpolated\n\
  -i, --info                display information about the image\n\
  -s, --strip               do not copy comment and extra markers\n\
      --help                display this help text and exit\n\
      --version             display version information and exit\n")

/* Set to 1 if option --blocks-file (-f) has been specified.  */
char opt_blocks_file;

/* Set to 1 if option --method (-m) has been specified.  */
char opt_method;

/* Set to 1 if option --verbose (-v) has been specified.  */
char opt_verbose;

/* Set to 1 if option --info (-i) has been specified.  */
char opt_info;

/* Set to 1 if option --strip (-s) has been specified.  */
char opt_strip;

/* Set to 1 if option --help has been specified.  */
char opt_help;

/* Set to 1 if option --version has been specified.  */
char opt_version;

/* Argument to option --blocks-file (-f).  */
const char *arg_blocks_file;

/* Argument to option --method (-m).  */
const char *arg_method;

/* Parse command line options.  Return index of first non-option argument,
   or -1 if an error is encountered.  */
int parse_options (const char *const program_name, const int argc, char **const argv)
{
  static const char *const optstr__blocks_file = "blocks-file";
  static const char *const optstr__method = "method";
  static const char *const optstr__verbose = "verbose";
  static const char *const optstr__info = "info";
  static const char *const optstr__strip = "strip";
  static const char *const optstr__help = "help";
  static const char *const optstr__version = "version";
  int i = 0;
  opt_blocks_file = 0;
  opt_method = 0;
  opt_verbose = 0;
  opt_info = 0;
  opt_strip = 0;
  opt_help = 0;
  opt_version = 0;
  arg_blocks_file = 0;
  arg_method = 0;
  while (++i < argc)
  {
    const char *option = argv [i];
    if (*option != '-')
      return i;
    else if (*++option == '\0')
      return i;
    else if (*option == '-')
    {
      const char *argument;
      size_t option_len;
      ++option;
      if ((argument = strchr (option, '=')) == option)
        goto error_unknown_long_opt;
      else if (argument == 0)
        option_len = strlen (option);
      else
        option_len = argument++ - option;
      switch (*option)
      {
       case '\0':
        return i + 1;
       case 'b':
        if (strncmp (option + 1, optstr__blocks_file + 1, option_len - 1) == 0)
        {
          if (argument != 0)
            arg_blocks_file = argument;
          else if (++i < argc)
            arg_blocks_file = argv [i];
          else
          {
            option = optstr__blocks_file;
            goto error_missing_arg_long;
          }
          opt_blocks_file = 1;
          break;
        }
        goto error_unknown_long_opt;
       case 'h':
        if (strncmp (option + 1, optstr__help + 1, option_len - 1) == 0)
        {
          if (argument != 0)
          {
            option = optstr__help;
            goto error_unexpec_arg_long;
          }
          opt_help = 1;
          return i + 1;
        }
        goto error_unknown_long_opt;
       case 'i':
        if (strncmp (option + 1, optstr__info + 1, option_len - 1) == 0)
        {
          if (argument != 0)
          {
            option = optstr__info;
            goto error_unexpec_arg_long;
          }
          opt_info = 1;
          break;
        }
        goto error_unknown_long_opt;
       case 'm':
        if (strncmp (option + 1, optstr__method + 1, option_len - 1) == 0)
        {
          if (argument != 0)
            arg_method = argument;
          else if (++i < argc)
            arg_method = argv [i];
          else
          {
            option = optstr__method;
            goto error_missing_arg_long;
          }
          opt_method = 1;
          break;
        }
        goto error_unknown_long_opt;
       case 's':
        if (strncmp (option + 1, optstr__strip + 1, option_len - 1) == 0)
        {
          if (argument != 0)
          {
            option = optstr__strip;
            goto error_unexpec_arg_long;
          }
          opt_strip = 1;
          break;
        }
        goto error_unknown_long_opt;
       case 'v':
        if (strncmp (option + 1, optstr__verbose + 1, option_len - 1) == 0)
        {
          if (option_len <= 3)
            goto error_long_opt_ambiguous;
          if (argument != 0)
          {
            option = optstr__verbose;
            goto error_unexpec_arg_long;
          }
          opt_verbose = 1;
          break;
        }
        if (strncmp (option + 1, optstr__version + 1, option_len - 1) == 0)
        {
          if (option_len <= 3)
            goto error_long_opt_ambiguous;
          if (argument != 0)
          {
            option = optstr__version;
            goto error_unexpec_arg_long;
          }
          opt_version = 1;
          return i + 1;
        }
       default:
       error_unknown_long_opt:
        fprintf (stderr, STR_ERR_UNKNOWN_LONG_OPT, program_name, option);
        return -1;
       error_long_opt_ambiguous:
        fprintf (stderr, STR_ERR_LONG_OPT_AMBIGUOUS, program_name, option);
        return -1;
       error_missing_arg_long:
        fprintf (stderr, STR_ERR_MISSING_ARG_LONG, program_name, option);
        return -1;
       error_unexpec_arg_long:
        fprintf (stderr, STR_ERR_UNEXPEC_ARG_LONG, program_name, option);
        return -1;
      }
    }
    else
      do
      {
        switch (*option)
        {
         case 'f':
          if (option [1] != '\0')
            arg_blocks_file = option + 1;
          else if (++i < argc)
            arg_blocks_file = argv [i];
          else
            goto error_missing_arg_short;
          option = "\0";
          opt_blocks_file = 1;
          break;
         case 'i':
          opt_info = 1;
          break;
         case 'm':
          if (option [1] != '\0')
            arg_method = option + 1;
          else if (++i < argc)
            arg_method = argv [i];
          else
            goto error_missing_arg_short;
          option = "\0";
          opt_method = 1;
          break;
         case 's':
          opt_strip = 1;
          break;
         case 'v':
          opt_verbose = 1;
          break;
         default:
          fprintf (stderr, STR_ERR_UNKNOWN_SHORT_OPT, program_name, *option);
          return -1;
         error_missing_arg_short:
          fprintf (stderr, STR_ERR_MISSING_ARG_SHORT, program_name, *option);
          return -1;
        }
      } while (*++option != '\0');
  }
  return i;
}



/* Display text in response to the --help command line option.  */
void
display_help_text (void)
{
    printf (_("Usage: %s [OPTION]... SOURCE DEST [[D:]X,Y[,S]|[,SX,SY]]...\n"
              "Interpolate pixels in JFIF image files.\n\n"
              "Pixel block specification:\n"
              "  D     can be `V' or `v' (vertical 1D interpolation),\n"
              "               `H' or `h' (horizontal 1D interpolation),\n"
              "               `2'        (2D interpolation) [default];\n"
              "  X,Y   specifies the top left corner of the pixel block to be interpolated;\n"
              "  S     specifies the size of the block [default: 1];\n"
              "  SX,SY specifies separate sizes for the X and Y direction.\n"
              "All numbers can be absolute coordinates/sizes, or percentages of the image\n"
              "size (if followed by a `%%' character).\n\n"
              "Options:\n"), invocation_name);
    fputs (STR_HELP, stdout);

    /* TRANSLATORS: Please align the right text column in the
       "Interpolation methods" section with the right text column in the
       "Options" section.  */
    fputs (_("\nInterpolation methods:\n"
             "  0, av, average            average of adjacent pixels\n"
             "  1, li, linear             (bi)linear interpolation\n"
             "  2, qu, quadratic          (bi)quadratic interpolation\n"
             "  3, cu, cubic              (bi)cubic interpolation\n"), stdout);

    /* TRANSLATORS: Please include the information that bug reports
       should be send in English.  E.g., translate "Please report bugs
       (in English) to ...".  */
    fputs (_("\nPlease report bugs to <martin@zero-based.org>.\n"), stdout);
}



/* Display text in response to the --version command line option.  */
void
display_version_text (void)
{
    fputs ("jpegpixi (" PACKAGE_NAME ") " PACKAGE_VERSION "\n", stdout);

    /* TRANSLATORS: Please leave the copyright statement intact, but replace
       "(C)" with the "copyright sign" (Unicode character <U00A9>).  */
    fputs (_("Copyright (C) 2002, 2003, 2004, 2005 Martin Dickopp\n\n"), stdout);

    /* TRANSLATORS: Please don't translate the warranty disclaimer
       literally, but replace it with a text which has a legal effect as
       close as possible to the original in the jurisdiction(s) where
       your language is used. If unsure, replace it with a translation
       of "There is no warranty, to the extent allowed by law."  */
    fputs (_("This program is free software; it may be copied and/or modified under the\n"
             "terms of the GNU General Public License version 2 or (at your option) any\n"
             "later version. There is NO warranty; not even for MERCHANTABILITY or FIT-\n"
             "NESS FOR A PARTICULAR PURPOSE.\n"), stdout);
}
