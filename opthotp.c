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
   Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301,
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

#define STR_HELP_THRESHOLD "\
  -t, --threshold=THRESHOLD    specify threshold between noise and signal\n\
                                 [default: 10%]\n"

#define STR_HELP_INVERT "\
  -i, --invert                 invert image (i.e. find dead pixels in an\n\
                                 otherwise white image)\n"

#define STR_HELP_COMMENTS "\
  -c, --comments               output comments with luminosities of hot pixel\n\
                                 blocks\n"

#define STR_HELP_HELP "\
      --help                   display this help text and exit\n"

#define STR_HELP_VERSION "\
      --version                display version information and exit\n"

#define STR_HELP _("\
  -t, --threshold=THRESHOLD    specify threshold between noise and signal\n\
                                 [default: 10%]\n\
  -i, --invert                 invert image (i.e. find dead pixels in an\n\
                                 otherwise white image)\n\
  -c, --comments               output comments with luminosities of hot pixel\n\
                                 blocks\n\
      --help                   display this help text and exit\n\
      --version                display version information and exit\n")

/* Set to 1 if option --threshold (-t) has been specified.  */
char opt_threshold;

/* Set to 1 if option --invert (-i) has been specified.  */
char opt_invert;

/* Set to 1 if option --comments (-c) has been specified.  */
char opt_comments;

/* Set to 1 if option --help has been specified.  */
char opt_help;

/* Set to 1 if option --version has been specified.  */
char opt_version;

/* Argument to option --threshold (-t).  */
const char *arg_threshold;

/* Parse command line options.  Return index of first non-option argument,
   or -1 if an error is encountered.  */
int parse_options (const char *const program_name, const int argc, char **const argv)
{
  static const char *const optstr__threshold = "threshold";
  static const char *const optstr__invert = "invert";
  static const char *const optstr__comments = "comments";
  static const char *const optstr__help = "help";
  static const char *const optstr__version = "version";
  int i = 0;
  opt_threshold = 0;
  opt_invert = 0;
  opt_comments = 0;
  opt_help = 0;
  opt_version = 0;
  arg_threshold = 0;
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
       case 'c':
        if (strncmp (option + 1, optstr__comments + 1, option_len - 1) == 0)
        {
          if (argument != 0)
          {
            option = optstr__comments;
            goto error_unexpec_arg_long;
          }
          opt_comments = 1;
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
        if (strncmp (option + 1, optstr__invert + 1, option_len - 1) == 0)
        {
          if (argument != 0)
          {
            option = optstr__invert;
            goto error_unexpec_arg_long;
          }
          opt_invert = 1;
          break;
        }
        goto error_unknown_long_opt;
       case 't':
        if (strncmp (option + 1, optstr__threshold + 1, option_len - 1) == 0)
        {
          if (argument != 0)
            arg_threshold = argument;
          else if (++i < argc)
            arg_threshold = argv [i];
          else
          {
            option = optstr__threshold;
            goto error_missing_arg_long;
          }
          opt_threshold = 1;
          break;
        }
        goto error_unknown_long_opt;
       case 'v':
        if (strncmp (option + 1, optstr__version + 1, option_len - 1) == 0)
        {
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
         case 'c':
          opt_comments = 1;
          break;
         case 'i':
          opt_invert = 1;
          break;
         case 't':
          if (option [1] != '\0')
            arg_threshold = option + 1;
          else if (++i < argc)
            arg_threshold = argv [i];
          else
            goto error_missing_arg_short;
          option = "\0";
          opt_threshold = 1;
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
    printf (_("Usage: %s [OPTION]... JPEG-FILE [PIXEL-BLOCKS-FILE]\n"
              "Find hot pixels in an otherwise black JPEG image (default) or dead pixels in an\n"
              "otherwise white JPEG image (if the `--invert' option is specified). Write their\n"
              "coordinates to a pixel blocks file suitable for the jpegpixi program.\n\n"
              "Options:\n"), invocation_name);
    fputs (STR_HELP, stdout);

    /* TRANSLATORS: Please include the information that bug reports
       should be send in English.  E.g., translate "Please report bugs
       (in English) to ...".  */
     fputs (_("\nPlease report bugs to <martin@zero-based.org>.\n"), stdout);
}



/* Display text in response to the --version command line option.  */
void
display_version_text (void)
{
    fputs ("jpeghotp (" PACKAGE_NAME ") " PACKAGE_VERSION "\n", stdout);

    /* TRANSLATORS: Please leave the copyright statement intact, but replace
       "(C)" with the "copyright sign" (Unicode character <U00A9>).  */
    fputs (_("Copyright (C) 2003, 2004, 2005 Martin Dickopp\n\n"), stdout);

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
