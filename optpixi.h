/* This file has been generated with opag 0.8.0.  */

#ifndef HDR_OPTPIXI
#define HDR_OPTPIXI 1

/* Set to 1 if option --blocks-file (-f) has been specified.  */
extern char opt_blocks_file;

/* Set to 1 if option --method (-m) has been specified.  */
extern char opt_method;

/* Set to 1 if option --verbose (-v) has been specified.  */
extern char opt_verbose;

/* Set to 1 if option --info (-i) has been specified.  */
extern char opt_info;

/* Set to 1 if option --strip (-s) has been specified.  */
extern char opt_strip;

/* Set to 1 if option --help has been specified.  */
extern char opt_help;

/* Set to 1 if option --version has been specified.  */
extern char opt_version;

/* Argument to option --blocks-file (-f).  */
extern const char *arg_blocks_file;

/* Argument to option --method (-m).  */
extern const char *arg_method;

/* Parse command line options.  Return index of first non-option argument,
   or -1 if an error is encountered.  */
#if defined(__GNUC__) && (__GNUC__ > 3 || (__GNUC__ == 3 && __GNUC_MINOR__ >= 3))
extern int parse_options (const char *program_name, int argc, char **argv) __attribute__ ((nonnull));
#else
extern int parse_options (const char *program_name, int argc, char **argv);
#endif

#endif
