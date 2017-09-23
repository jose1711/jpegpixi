/* This file has been generated with opag 0.8.0.  */

#ifndef HDR_OPTHOTP
#define HDR_OPTHOTP 1

/* Set to 1 if option --threshold (-t) has been specified.  */
extern char opt_threshold;

/* Set to 1 if option --invert (-i) has been specified.  */
extern char opt_invert;

/* Set to 1 if option --comments (-c) has been specified.  */
extern char opt_comments;

/* Set to 1 if option --help has been specified.  */
extern char opt_help;

/* Set to 1 if option --version has been specified.  */
extern char opt_version;

/* Argument to option --threshold (-t).  */
extern const char *arg_threshold;

/* Parse command line options.  Return index of first non-option argument,
   or -1 if an error is encountered.  */
#if defined(__GNUC__) && (__GNUC__ > 3 || (__GNUC__ == 3 && __GNUC_MINOR__ >= 3))
extern int parse_options (const char *program_name, int argc, char **argv) __attribute__ ((nonnull));
#else
extern int parse_options (const char *program_name, int argc, char **argv);
#endif

#endif
