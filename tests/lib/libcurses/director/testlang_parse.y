%{
/*	$NetBSD: testlang_parse.y,v 1.1 2011/04/10 09:55:09 blymn Exp $	*/

/*-
 * Copyright 2009 Brett Lymn <blymn@NetBSD.org>
 *
 * All rights reserved.
 *
 * This code has been donated to The NetBSD Foundation by the Author.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. The name of the author may not be used to endorse or promote products
 *    derived from this software withough specific prior written permission
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 * OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
 * NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 * THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 *
 */
#include <assert.h>
#include <errno.h>
#include <fcntl.h>
#include <poll.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <sys/syslimits.h>
#include <time.h>
#include "returns.h"

#define YYDEBUG 1

extern int verbose;
extern int cmdpipe[2];
extern int slvpipe[2];
extern int master;
extern struct pollfd readfd;
extern char *check_path;
extern char *cur_file;		/* from director.c */

size_t line;

static int input_delay;

/* time delay between inputs chars - default to 0.1ms minimum to prevent
 * problems with input tests
 */
#define DELAY_MIN 0.1
static struct timespec delay_spec = {0, 1000 * DELAY_MIN};
static char *input_str;	/* string to feed in as input */
static bool no_input;	/* don't need more input */

#define READ_PIPE  0
#define WRITE_PIPE 1

char *returns_enum_names[] = {
	"unused", "numeric", "string", "byte", "ERR", "OK", "NULL", "not NULL",
	"variable", "reference", "returns count", "slave error"
};

typedef enum {
	arg_static,
	arg_byte,
	arg_var
} args_state_t;

static char *args_enum_names[] = {
	"static", "byte", "var"
};

typedef struct {
	args_state_t	arg_type;
	size_t		arg_len;
	char		*arg_string;
	int		var_index;
} args_t;

typedef struct {
	char		*function;
	int		nrets;		/* number of returns */
	returns_t	*returns;	/* array of expected returns */
	int		nargs;		/* number of arguments */
	args_t		*args;		/* arguments for the call */
} cmd_line_t;

static cmd_line_t	command;

typedef struct {
	char *name;
	size_t len;
	returns_enum_t type;
	void *value;
} var_t;

static size_t nvars; 		/* Number of declared variables */
static var_t *vars; 		/* Variables defined during the test. */

static int	check_function_table(char *, const char *[], int);
static int	find_var_index(char *);
static void 	assign_arg(args_state_t, void *);
static int	assign_var(char *);
void		init_parse_variables(int);
static void	validate(int, void *);
static void	validate_return(char *, char *, int);
static void	validate_variable(int, returns_enum_t, void *, int, int);
static void	validate_byte(returns_t *, returns_t *, int);
static void	write_cmd_pipe(char *);
static void	write_cmd_pipe_args(args_state_t, void *);
static void	read_cmd_pipe(returns_t *);
static void	write_func_and_args();
static void	compare_streams(char *, bool);
static void	do_function_call(int);
static void	save_slave_output(bool);
static void	validate_type(returns_enum_t, returns_t *, int);
static void	set_var(returns_enum_t, char *, void *);
static void	validate_reference(int, void *);
static char	*numeric_or(char *, char *);
static char	*get_numeric_var(char *);

static const char *input_functions[] = {
	"inch", "getch", "getnstr", "getstr", "innstr", "instr", "mvgetnstr",
	"mvgetstr", "mvinchstr", "mvinchnstr", "mvgetnstr", "mvgetstr",
	"mvinchstr", "mvinchnstr", "winch", "wgetch", "wgetnstr", "wgetstr",
	"winchnstr", "winchstr", "winnstr", "winstr"
};

static const unsigned ninput_functions =
	sizeof(input_functions) / sizeof(char *);

saved_data_t saved_output;

%}

%union {
	char *string;
	returns_t *retval;
}

%token <string> PATH
%token <string> STRING
%token <retval> BYTE
%token <string> VARNAME
%token <string> FILENAME
%token <string> VARIABLE
%token <string> REFERENCE
%token <string> NULL_RET
%token <string> NON_NULL
%token <string> ERR_RET
%token <string> OK_RET
%token <string> numeric
%token <string> DELAY
%token <string> INPUT
%token <string> COMPARE
%token <string> COMPAREND
%token <string> ASSIGN
%token EOL CALL CHECK NOINPUT OR LHB RHB
%token CALL2 CALL3 CALL4 DRAIN

%nonassoc OR

%%

statement	:	/* empty */
		| assign statement
		| call statement
		| call2 statement
		| call3 statement
		| call4 statement
		| check statement
		| delay statement
		| input statement
		| noinput statement
		| compare statement
		| comparend statement
		| eol statement
		;

assign		: ASSIGN VARNAME numeric {set_var(ret_number, $2, $3);} eol
		| ASSIGN VARNAME LHB expr RHB {set_var(ret_number, $2, $<string>4);} eol
		| ASSIGN VARNAME STRING {set_var(ret_string, $2, $3);} eol
		| ASSIGN VARNAME BYTE {set_var(ret_byte, $2, $3);} eol
		;

call		: CALL result fn_name args eol {
	do_function_call(1);
}
		;

call2		: CALL2 result result fn_name args eol {
	do_function_call(2);
}
		;

call3		: CALL3 result result result fn_name args eol {
	do_function_call(3);
}
		;

call4		: CALL4 result result result result fn_name args eol {
	do_function_call(4);
 }
		;

check		: CHECK var returns eol {
	returns_t ret_var;
	var_t *vptr;
	if (command.returns[0].return_index == -1)
		err(1, "Undefined variable in check statement, line %d"
		    " of file %s", line, cur_file);

	if (verbose)
		fprintf(stderr, "Checking contents of variable %s for %s\n",
			vars[command.returns[0].return_index].name,
			returns_enum_names[command.returns[1].return_type]);

	if (((command.returns[1].return_type == arg_byte) &&
	     (vars[command.returns[0].return_index].type != ret_byte)) ||
	    ((command.returns[1].return_type == arg_static) &&
	     (vars[command.returns[0].return_index].type != ret_string)))
		err(1, "Var type %s (%d) does not match return type %s (%d)",
		    returns_enum_names[vars[command.returns[0].return_index].type],
		    vars[command.returns[0].return_index].type,
		    returns_enum_names[command.returns[1].return_type],
		    command.returns[1].return_type);

	switch (command.returns[1].return_type) {
	case ret_err:
		validate_variable(0, ret_string, "ERR",
				  command.returns[0].return_index, 0);
		break;

	case ret_ok:
		validate_variable(0, ret_string, "OK",
				  command.returns[0].return_index, 0);
		break;

	case ret_null:
		validate_variable(0, ret_string, "NULL",
				  command.returns[0].return_index, 0);
		break;

	case ret_nonnull:
		validate_variable(0, ret_string, "NULL",
				  command.returns[0].return_index, 1);
		break;

	case ret_string:
	case ret_number:
		if (verbose)
			fprintf(stderr, " %s == returned %s\n",
				command.returns[1].return_value,
				vars[command.returns[0].return_index].value);
		validate_variable(0, ret_string,
				  command.returns[1].return_value,
				  command.returns[0].return_index, 0);
		break;

	case ret_byte:
		vptr = &vars[command.returns[0].return_index];
		ret_var.return_len = vptr->len;
		ret_var.return_type = vptr->type;
		ret_var.return_value = vptr->value;
		validate_byte(&ret_var, &command.returns[1], 0);
		break;

	default:
		err(1, "Malformed check statement at line %d "
		    "of file %s", line, cur_file);
		break;
	}

	init_parse_variables(0);
 }
		;

delay		: DELAY numeric eol {
	/* set the inter-character delay */
	if (sscanf($2, "%d", &input_delay) == 0)
		err(1, "delay specification %s could not be converted to "
		    "numeric at line %d of file %s", $2, line, cur_file);
	if (verbose)
		fprintf(stderr, "Set input delay to %d ms\n", input_delay);

	if (input_delay < DELAY_MIN)
		input_delay = 1000 * DELAY_MIN; /* ms to ns */
	/*
	 * Fill in the timespec structure now ready for use later.
	 * The delay is specified in milliseconds so convert to timespec
	 * values
	 */
	delay_spec.tv_sec = input_delay / 1000;
	delay_spec.tv_nsec = (input_delay - 1000 * delay_spec.tv_sec) * 1000;

	init_parse_variables(0);
 }
	;

input		: INPUT STRING eol {
	if (input_str != NULL) {
		fprintf(stderr, "WARNING: Discarding unused input string at "
			"line %d of file %s\n", line, cur_file);
		free(input_str);
	}

	if ((input_str = malloc(strlen($2) + 1)) == NULL)
		err(2, "Cannot allocate memory for input string");

	strlcpy(input_str, $2, strlen($2) + 1);
}
	;


noinput		: NOINPUT eol {
	if (input_str != NULL) {
		fprintf(stderr, "WARNING: Discarding unused input string at "
			"line %d of file %s\n", line, cur_file);
		free(input_str);
	}

	no_input = true;
 }

compare		: COMPARE PATH eol
		| COMPARE FILENAME eol
{
	compare_streams($2, true);
}
	;


comparend	: COMPAREND PATH eol
		| COMPAREND FILENAME eol
{
	compare_streams($2, false);
}
	;


result		: returns
		| var
		| reference
		;

returns		: numeric { assign_rets(ret_number, $1); }
		| LHB expr RHB { assign_rets(ret_number, $<string>2); }
		| STRING { assign_rets(ret_string, $1); }
		| BYTE { assign_rets(ret_byte, (void *) $1); }
		| ERR_RET { assign_rets(ret_err, NULL); }
		| OK_RET { assign_rets(ret_ok, NULL); }
		| NULL_RET { assign_rets(ret_null, NULL); }
		| NON_NULL { assign_rets(ret_nonnull, NULL); }
		;

var		: VARNAME {
	assign_rets(ret_var, $1);
 }
		;

reference	: VARIABLE {
	assign_rets(ret_ref, $1);
 }

fn_name		: VARNAME {
	if (command.function != NULL)
		free(command.function);

	command.function = malloc(strlen($1) + 1);
	if (command.function == NULL)
		err(1, "Could not allocate memory for function name");
	strcpy(command.function, $1);
 }
		;

expr		: numeric
		| VARIABLE
			{ $<string>$ = get_numeric_var($1); }
		| expr OR expr
			{ $<string>$ = numeric_or($<string>1, $<string>3); }
		;

args		: /* empty */
		| LHB expr RHB { assign_arg(arg_static, $<string>2); } args
		| numeric { assign_arg(arg_static, $1); } args
		| STRING { assign_arg(arg_static, $1); } args
		| BYTE { assign_arg(arg_byte, $1); } args
		| PATH { assign_arg(arg_static, $1); } args
		| FILENAME { assign_arg(arg_static, $1); } args
		| VARNAME { assign_arg(arg_static, $1); } args
		| VARIABLE  { assign_arg(arg_var, $1); } args
		;

eol		: EOL
		;

%%

/*
 * Get the value of a variable, error if the variable has not been set or
 * is not a numeric type.
 */
static char *
get_numeric_var(char *var)
{
	int index;

	if ((index = find_var_index(var)) < 0)
		err(1, "Variable %s is undefined", var);

	if (vars[index].type != ret_number)
		err(1, "Variable %s is not a numeric type");

	return vars[index].value;
}

/*
 * Perform a bitwise OR on two numbers and return the result.
 */
static char *
numeric_or(char *n1, char *n2)
{
	unsigned long i1, i2, result;
	char *ret;

	i1 = strtoul(n1, NULL, 10);
	i2 = strtoul(n2, NULL, 10);

	result = i1 | i2;
	asprintf(&ret, "%lu", result);

	if (verbose)
		fprintf(stderr,
			"numeric or of 0x%lx (%s) and 0x%lx (%s) results"
			" in 0x%lx (%s)\n",
			i1, n1, i2, n2, result, ret);

	return ret;
}

/*
 * Assign the value given to the named variable.
 */
static void
set_var(returns_enum_t type, char *name, void *value)
{
	int index;
	char *number;
	returns_t *ret;

	index = find_var_index(name);
	if (index < 0)
		index = assign_var(name);

	vars[index].type = type;
	if ((type == ret_number) || (type == ret_string)) {
		number = value;
		vars[index].len = strlen(number) + 1;
		vars[index].value = malloc(vars[index].len + 1);
		if (vars[index].value == NULL)
			err(1, "Could not malloc memory for assign string");
		strcpy(vars[index].value, number);
	} else {
		/* can only be a byte value */
		ret = value;
		vars[index].len = ret->return_len;
		vars[index].value = malloc(vars[index].len);
		if (vars[index].value == NULL)
			err(1, "Could not malloc memory to assign byte string");
		memcpy(vars[index].value, ret->return_value, vars[index].len);
	}
}

/*
 * Add a new variable to the vars array, the value will be assigned later,
 * when a test function call returns.
 */
static int
assign_var(char *varname)
{
	var_t *temp;
	char *name;

	if ((name = malloc(strlen(varname) + 1)) == NULL)
		err(1, "Alloc of varname failed");

	if ((temp = (var_t *) realloc(vars, sizeof(var_t) * (nvars + 1)))
	    == NULL) {
		free(name);
		err(1, "Realloc of vars array failed");
	}

	strcpy(name, varname);
	vars = temp;
	vars[nvars].name = name;
	vars[nvars].len = 0;
	vars[nvars].value = NULL;
	nvars++;

	return (nvars - 1);
}

/*
 * Allocate and assign a new argument of the given type.
 */
static void
assign_arg(args_state_t arg_type, void *arg)
{
	args_t *temp, cur;
	char *str;
	returns_t *ret;

	if (verbose)
		printf("function is >%s<, adding arg >%s< type %s\n",
		       command.function, arg, args_enum_names[arg_type]);

	cur.arg_type = arg_type;
	if (arg_type != arg_var) {
		if (arg_type != arg_byte) {
			str = arg;
			cur.arg_len = strlen(str);
			cur.arg_string = malloc(cur.arg_len + 1);
			if (cur.arg_string == NULL)
				err(1,
				    "Could not malloc memory for arg string");
			strcpy(cur.arg_string, arg);
		} else {
			ret = arg;
			cur.arg_len = ret->return_len;
			cur.arg_string = malloc(cur.arg_len);
			if (cur.arg_string == NULL)
				err(1,
				    "Could not malloc memory for arg bytes");
			memcpy(cur.arg_string, ret->return_value,
			       cur.arg_len);
		}
	} else {
		cur.var_index = find_var_index(arg);
		if (cur.var_index < 0)
			err(1, "Invalid variable %s at line %d of file %s",
			    arg, line, cur_file);
		cur.arg_type = ret_string;
	}

	temp = realloc(command.args, sizeof(args_t) * (command.nargs + 1));
	if (temp == NULL)
		err(1, "Failed to reallocate args");
	command.args = temp;
	memcpy(&command.args[command.nargs], &cur, sizeof(args_t));
	command.nargs++;
}

/*
 * Allocate and assign a new return.
 */
static void
assign_rets(returns_enum_t ret_type, void *ret)
{
	returns_t *temp, cur;
	char *ret_str;
	returns_t *ret_ret;

	cur.return_type = ret_type;
	if (ret_type != ret_var) {
		if ((ret_type == ret_number) || (ret_type == ret_string)) {
			ret_str = ret;
			cur.return_len = strlen(ret_str) + 1;
			cur.return_value = malloc(cur.return_len + 1);
			if (cur.return_value == NULL)
				err(1,
				    "Could not malloc memory for arg string");
			strcpy(cur.return_value, ret_str);
		} else if (ret_type == ret_byte) {
			ret_ret = ret;
			cur.return_len = ret_ret->return_len;
			cur.return_value = malloc(cur.return_len);
			if (cur.return_value == NULL)
				err(1,
				    "Could not malloc memory for byte string");
			memcpy(cur.return_value, ret_ret->return_value,
			       cur.return_len);
		} else if (ret_type == ret_ref) {
			if ((cur.return_index = find_var_index(ret)) < 0)
				err(1, "Undefined variable reference");
		}
	} else {
		cur.return_index = find_var_index(ret);
		if (cur.return_index < 0)
			cur.return_index = assign_var(ret);
	}

	temp = realloc(command.returns,
		       sizeof(returns_t) * (command.nrets + 1));
	if (temp == NULL)
		err(1, "Failed to reallocate returns");
	command.returns = temp;
	memcpy(&command.returns[command.nrets], &cur, sizeof(returns_t));
	command.nrets++;
}

/*
 * Find the given variable name in the var array and return the index
 * return -1 if var is not found.
 */
static int
find_var_index(char *var_name)
{
	int result, i;

	result = -1;

	for (i = 0; i < nvars; i++) {
		if (strcmp(var_name, vars[i].name) == 0) {
			result = i;
			break;
		}
	}

	return result;
}

/*
 * Check the given function name in the given table of names, return 1 if
 * there is a match.
 */
static int check_function_table(char *function, const char *table[],
				int nfunctions)
{
	int i;

	for (i = 0; i < nfunctions; i++) {
		if ((strlen(function) == strlen(table[i])) &&
		    (strcmp(function, table[i]) == 0))
			return 1;
	}

	return 0;
}

/*
 * Compare the output from the slave against the given file and report
 * any differences.
 */
static void
compare_streams(char *filename, bool discard)
{
	char check_file[PATH_MAX], drain, ref, data;
	struct pollfd fds[2];
	int nfd, check_fd, result;

	/*
	 * Don't prepend check path iff check file has an absolute
	 * path.
	 */
	if (filename[0] != '/') {
		if (strlcpy(check_file, check_path, sizeof(check_file))
		    >= sizeof(check_file))
			err(2, "CHECK_PATH too long");

		if (strlcat(check_file, "/", sizeof(check_file))
		    >= sizeof(check_file))
			err(2, "Could not append / to check file path");
	} else {
		check_file[0] = '\0';
	}

	if (strlcat(check_file, filename, sizeof(check_file))
	    >= sizeof(check_file))
		err(2, "Path to check file path overflowed");

	if ((check_fd = open(check_file, O_RDONLY, 0)) < 0)
		err(2, "failed to open file %s line %d of file %s",
		    check_file, line, cur_file);

	fds[0].fd = check_fd;
	fds[0].events = POLLIN;
	fds[0].revents = 0;
	fds[1].fd = master;
	fds[1].events = POLLIN;
	fds[1].revents = 0;

	nfd = 2;
	/*
	 * if we have saved output then only check for data in the
	 * reference file since the slave data may already be drained.
	 */
	if (saved_output.count > 0)
		nfd = 1;

	while (poll(&fds[0], nfd, 500) == nfd) {
		if ((result = read(check_fd, &ref, 1)) < 1) {
			if (result != 0) {
				fprintf(stderr, "Bad read on file %s\n",
					check_file);
				exit(2);
			} else {
				break;
			}
		}

		if (saved_output.count > 0) {
			data = saved_output.data[saved_output.readp];
			saved_output.count--;
			saved_output.readp++;
			/* run out of saved data, switch to file */
			if (saved_output.count == 0)
				nfd = 2;
		} else {
			if (read(master, &data, 1) < 1)
				err(2, "Bad read on slave pty");
		}

		if (verbose) {
			fprintf(stderr, "Comparing reference byte 0x%x (%c)"
				" against slave byte 0x%x (%c)\n", ref,
				(ref >= ' ')? ref : '-',
				data, (data >= ' ')? data : '-');
		}

		if (ref != data) {
			fprintf(stderr, "refresh data from slave does "
				"not match expected from file %s, "
				"line %d of file %s\n",
				check_file, line, cur_file);

			if (!verbose)
				exit(2);
		}

		fds[0].revents = 0;
		fds[1].revents = 0;
	}


	if (saved_output.count > 0)
		fprintf(stderr, "Warning: excess saved data from "
			"slave at line %d of file %s\n", line, cur_file);

	/* discard any excess saved output if required */
	if (discard) {
		saved_output.count = 0;
		saved_output.readp = 0;
	}

	fds[0].revents = 0;
	fds[1].revents = 0;
	if ((result = poll(&fds[0], 2, 0)) != 0) {
		if (result == -1)
			err(2, "poll of file descriptors failed");

		if ((fds[1].revents && POLLIN) == POLLIN) {
			save_slave_output(true);
		} else {
			/*
			 * handle excess in file if it exists.  Poll
			 * says there is data until EOF is read.
			 * Check next read is EOF, if it is not then
			 * the file really has more data than the
			 * slave produced so flag this as a warning.
			 */
			result = read(check_fd, &drain, 1);
			if (result == -1)
				err("read of data file failed");

			if (result > 0) {
				fprintf(stderr, "Error: excess data "
					"in file %s\n", check_file);
				if (!verbose)
					exit(2);
			}
		}
	}

	close(check_fd);
}

/*
 * Pass a function call and arguments to the slave and wait for the
 * results.  The variable nresults determines how many returns we expect
 * back from the slave.  These results will be validated against the
 * expected returns or assigned to variables.
 */
static void
do_function_call(int nresults)
{
#define MAX_RESULTS 4
	char *p;
	int i, do_input, nfd;
	struct pollfd fds[3];
	returns_t response[MAX_RESULTS], returns_count;

	assert(nresults <= MAX_RESULTS);

	do_input = check_function_table(command.function, input_functions,
					ninput_functions);

	write_func_and_args();

	/*
	 * We should get the number of returns back here, grab it before
	 * doing input otherwise it will confuse the input poll
	 */
	read_cmd_pipe(&returns_count);
	if (returns_count.return_type != ret_count )
		err(2, "expected return type of ret_count but received %s",
		    returns_enum_names[returns_count.return_type]);

	if (verbose)
		fprintf(stderr, "Expect %d results from slave, slave "
			"reported %d\n", nresults, returns_count.return_len);

	if ((no_input == false) && (do_input == 1)) {
		if (verbose)
			fprintf(stderr, "doing input with inputstr >%s<\n",
				input_str);

		if (input_str == NULL) {
			fprintf(stderr, "Error: Call to input function at "
				" line %d of file %s but no input defined",
				line, cur_file);
			exit(2);
		}

		fds[0].fd = slvpipe[READ_PIPE];
		fds[0].events = POLLIN;
 		p = input_str;
		save_slave_output(false);
		while(*p != '\0') {
			if (verbose) {
				fprintf(stderr, "Writing char >%c< to slave\n",
					*p);
				fflush(stderr);
			}

			nanosleep(&delay_spec, NULL);

			write(master, p, 1);
			save_slave_output(false);

			p++;
			fds[0].revents = 0;

			/* check for slave function returning unexpectedly */
			if ((poll(&fds[0], 1, 10) > 0) && (*p != '\0')) {
				fprintf(stderr, "Warning: slave function "
					"returned before end of input string"
					" at line %d of file %s\n",
					line, cur_file);
				break;
			}
		}

		if (verbose) {
			fprintf(stderr, "Input done.\n");
		}

		/* done with the input string, free the resources */
		free(input_str);
		input_str = NULL;
	}

	if (verbose) {
		fds[0].fd = slvpipe[READ_PIPE];
		fds[0].events = POLLIN;
		fds[0].revents = 0;

		fds[1].fd = slvpipe[WRITE_PIPE];
		fds[1].events = POLLOUT;
		fds[1].revents = 0;

		fds[2].fd = master;
		fds[2].events = POLLIN | POLLOUT;
		fds[2].revents = 0;

		i = poll(&fds[0], 3, 1000);
		fprintf(stderr, "Poll returned %d\n", i);
		for (i = 0; i < 3; i++) {
			fprintf(stderr, "revents for fd[%d] = 0x%x\n",
				i, fds[i].revents);
		}
	}

	/* drain any trailing output */
	save_slave_output(false);

	for (i = 0; i < returns_count.return_len; i++) {
		read_cmd_pipe(&response[i]);
	}

	/*
	 * Check for a slave error in the first return slot, if the
	 * slave errored then we may not have the number of returns we
	 * expect but in this case we should report the slave error
	 * instead of a return count mismatch.
	 */
	if ((returns_count.return_len > 0) &&
	    (response[0].return_type == ret_slave_error))
		err(2, "Slave returned error: %s", response[0].return_value);

	if (returns_count.return_len != nresults)
		err(2, "Incorrect number of returns from slave, expected %d "
		    "but received %d", nresults, returns_count.return_len);

	if (verbose) {
		for (i = 0; i < nresults; i++) {
			if ((response[i].return_type != ret_byte) &&
			    (response[i].return_type != ret_err) &&
			    (response[i].return_type != ret_ok))
				fprintf(stderr,
					"received response >%s< "
					"expected", response[i].return_value);
			else
				fprintf(stderr, "received");

			fprintf(stderr, " return_type %s\n",
				returns_enum_names[command.returns[i].return_type]);
		}
	}

	for (i = 0; i < nresults; i++) {
		if (command.returns[i].return_type != ret_var) {
			validate(i, &response[i]);
		} else {
			vars[command.returns[i].return_index].len =
				response[i].return_len;
			vars[command.returns[i].return_index].value =
				response[i].return_value;
			vars[command.returns[i].return_index].type =
				response[i].return_type;
		}
	}

	if (saved_output.count > 0)
		fprintf(stderr, "Warning: excess data from slave at line %d"
			" of file %s\n", line, cur_file);

	init_parse_variables(0);
}

/*
 * Write the function and command arguments to the command pipe.
 */
static void
write_func_and_args()
{
	int i;

	if (verbose)
		fprintf(stderr, "calling function >%s<\n", command.function);

	write_cmd_pipe(command.function);
	for (i = 0; i < command.nargs; i++) {
		if (command.args[i].arg_type == arg_var)
			write_cmd_pipe_args(command.args[i].arg_type,
					    &vars[command.args[i].var_index]);
		else
			write_cmd_pipe_args(command.args[i].arg_type,
					    &command.args[i]);
	}

	write_cmd_pipe(NULL); /* signal end of arguments */
}

/*
 * Initialise the command structure - if initial is non-zero then just set
 * everything to sane values otherwise free any memory that was allocated
 * when building the structure.
 */
void
init_parse_variables(int initial)
{
	int i, result;
	struct pollfd slave_pty;

	if (initial == 0) {
		free(command.function);
		for (i = 0; i < command.nrets; i++) {
			if (command.returns[i].return_type == ret_number)
				free(command.returns[i].return_value);
		}
		free(command.returns);

		for (i = 0; i < command.nargs; i++) {
			if (command.args[i].arg_type != arg_var)
				free(command.args[i].arg_string);
		}
		free(command.args);
	} else {
		line = 0;
		input_delay = 0;
		vars = NULL;
		nvars = 0;
		input_str = NULL;
		saved_output.allocated = 0;
		saved_output.count = 0;
		saved_output.readp = 0;
		saved_output.data = NULL;
	}

	no_input = false;
	command.function = NULL;
	command.nargs = 0;
	command.args = NULL;
	command.nrets = 0;
	command.returns = NULL;

	/*
	 * Check the slave pty for stray output from the slave, at this
	 * point we should not see any data as it should have been
	 * consumed by the test functions.  If we see data then we have
	 * either a bug or are not handling an output generating function
	 * correctly.
	 */
	slave_pty.fd = master;
	slave_pty.events = POLLIN;
	slave_pty.revents = 0;
	result = poll(&slave_pty, 1, 0);

	if (result < 0)
		err(2, "Poll of slave pty failed");
	else if (result > 0)
		fprintf(stderr,
			"Warning: unexpected data from slave at line %d"
			" of file %s\n",
			line, cur_file);
}

/*
 * Validate the response against the expected return.  The variable
 * index is the index into the rets array in command.
 */
static void
validate(int index, void *data)
{
	char *response;
	returns_t *byte_response;

	byte_response = data;
	if ((command.returns[index].return_type != ret_byte) &&
	    (command.returns[index].return_type != ret_err) &&
	    (command.returns[index].return_type != ret_ok))
		response = byte_response->return_value;

	switch (command.returns[index].return_type) {
	case ret_err:
		validate_type(ret_err, byte_response, 0);
		break;

	case ret_ok:
		validate_type(ret_ok, byte_response, 0);
		break;

	case ret_null:
		validate_return("NULL", response, 0);
		break;

	case ret_nonnull:
		validate_return("NULL", response, 1);
		break;

	case ret_string:
	case ret_number:
		validate_return(command.returns[index].return_value,
				response, 0);
		break;

	case ret_ref:
		validate_reference(index, response);
		break;

	case ret_byte:
		validate_byte(&command.returns[index], byte_response, 0);
		break;

	default:
		err(1, "Malformed statement at line %d of file %s",
		    line, cur_file);
		break;
	}
}

/*
 * Validate the return against the contents of a variable.
 */
static void
validate_reference(int index, void *data)
{
	char *response;
	returns_t *byte_response;
	var_t *varp;

	varp = &vars[command.returns[index].return_index];

	byte_response = data;
	if (command.returns[index].return_type != ret_byte)
		response = data;

	if (verbose)
		fprintf(stderr,
			"validate_reference: return type of %s, value %s \n",
			returns_enum_names[varp->type], varp->value);

	switch (varp->type) {
	case ret_string:
	case ret_number:
		validate_return(varp->value, response, 0);
		break;

	case ret_byte:
		validate_byte(varp->value, byte_response, 0);
		break;

	default:
		err(1,
		    "Invalid return type for reference at line %d of file %s",
		    line, cur_file);
		break;
	}
}

/*
 * Validate the return type against the expected type, throw an error
 * if they don't match.
 */
static void
validate_type(returns_enum_t expected, returns_t *value, int check)
{
	if (((check == 0) && (expected != value->return_type)) ||
	    ((check == 1) && (expected == value->return_type)))
		err(1, "Validate expected type %s %s %s line %d of file %s",
		    returns_enum_names[expected],
		    (check == 0)? "matching" : "not matching",
		    returns_enum_names[value->return_type], line, cur_file);

	if (verbose)
		fprintf(stderr, "Validate expected type %s %s %s line %d"
			" of file %s\n",
			returns_enum_names[expected],
			(check == 0)? "matching" : "not matching",
			returns_enum_names[value->return_type], line, cur_file);
}

/*
 * Validate the return value against the expected value, throw an error
 * if they don't match.
 */
static void
validate_return(char *expected, char *value, int check)
{
	if (((check == 0) && strcmp(expected, value) != 0) ||
	    ((check == 1) && strcmp(expected, value) == 0))
		err(1, "Validate expected %s %s %s line %d of file %s",
		    expected,
		    (check == 0)? "matching" : "not matching", value,
		    line, cur_file);
	if (verbose)
		fprintf(stderr, "Validated expected value %s %s %s "
			"at line %d of file %s\n", expected,
			(check == 0)? "matches" : "does not match",
			value, line, cur_file);
}

/*
 * Validate the return value against the expected value, throw an error
 * if they don't match expectations.
 */
static void
validate_byte(returns_t *expected, returns_t *value, int check)
{
	/*
	 * No chance of a match if lengths differ...
	 */
	if ((check == 0) && (expected->return_len != value->return_len))
	    err(1, "Byte validation failed, length mismatch");

	/*
	 * If check is 0 then we want to throw an error IFF the byte streams
	 * do not match, if check is 1 then throw an error if the byte
	 * streams match.
	 */
	if (((check == 0) && memcmp(expected->return_value, value->return_value,
				    value->return_len) != 0) ||
	    ((check == 1) && (expected->return_len == value->return_len) &&
	     memcmp(expected->return_value, value->return_value,
		    value->return_len) == 0))
		err(1, "Validate expected %s byte stream at line %d"
		    "of file %s",
		    (check == 0)? "matching" : "not matching", line, cur_file);
	if (verbose)
		fprintf(stderr, "Validated expected %s byte stream "
			"at line %d of file %s\n",
			(check == 0)? "matching" : "not matching",
			line, cur_file);
}

/*
 * Validate the variable at index against the expected value, throw an
 * error if they don't match, if check is non-zero then the match is
 * negated.
 */
static void
validate_variable(int ret, returns_enum_t type, void *value, int index, int check)
{
	char *str;
	returns_t *retval;
	var_t *varptr;

	varptr = &vars[command.returns[ret].return_index];

	if (varptr->value == NULL)
		err(1, "Variable %s has no value assigned to it");


	if (varptr->type != type)
		err(1, "Variable %s is not the expected type");

	if (type != ret_byte) {
		if ((((check == 0) && strcmp(value, varptr->value) != 0))
		    || ((check == 1) && strcmp(value, varptr->value) == 0))
			err(1, "Variable %s contains %s instead of %s"
			    " value %s at line %d of file %s",
			    varptr->name,varptr->value,
			    (check == 0)? "expected" : "not matching", value,
			    line, cur_file);
		if (verbose)
			fprintf(stderr, "Variable %s contains %s value "
				"%s at line %d of file %s\n",
				varptr->name,
				(check == 0)? "expected" : "not matching",
				varptr->value, line, cur_file);
	} else {
		if ((check == 0) && (retval->return_len != varptr->len))
			err(1, "Byte validation failed, length mismatch");

		/*
		 * If check is 0 then we want to throw an error IFF
		 * the byte streams do not match, if check is 1 then
		 * throw an error if the byte streams match.
		 */
		if (((check == 0) && memcmp(retval->return_value, varptr->value,
					    varptr->len) != 0) ||
		    ((check == 1) && (retval->return_len == varptr->len) &&
		     memcmp(retval->return_value, varptr->value,
			    varptr->len) == 0))
			err(1, "Validate expected %s byte stream at line %d"
			    " of file %s",
			    (check == 0)? "matching" : "not matching",
			    line, cur_file);
		if (verbose)
			fprintf(stderr, "Validated expected %s byte stream "
				"at line %d of file %s\n",
				(check == 0)? "matching" : "not matching",
				line, cur_file);
	}
}

/*
 * Write a string to the command pipe - we feed the number of bytes coming
 * down first to allow storage allocation and then follow up with the data.
 * If cmd is NULL then feed a -1 down the pipe to say the end of the args.
 */
static void
write_cmd_pipe(char *cmd)
{
	args_t arg;
	size_t len;

	if (cmd == NULL)
		len = 0;
	else
		len = strlen(cmd);

	arg.arg_type = arg_static;
	arg.arg_len = len;
	arg.arg_string = cmd;
	write_cmd_pipe_args(arg.arg_type, &arg);

}

static void
write_cmd_pipe_args(args_state_t type, void *data)
{
	var_t *var_data;
	args_t *arg_data;
	int len, send_type;
	void *cmd;

	arg_data = data;
	if (type != arg_var) {
		if ((arg_data->arg_len == 0) && (arg_data->arg_string == NULL))
			len = -1;
		else
			len = arg_data->arg_len;
		cmd = arg_data->arg_string;
		if (type == arg_byte)
			send_type = ret_byte;
		else
			send_type = ret_string;
	} else {
		var_data = data;
		len = var_data->len;
		cmd = var_data->value;
		if (var_data->type == arg_byte)
			send_type = ret_byte;
		else
			send_type = ret_string;
	}

	if (verbose)
		fprintf(stderr, "Writing type %s to command pipe\n",
			returns_enum_names[send_type]);

	if (write(cmdpipe[WRITE_PIPE], &send_type, sizeof(int)) < 0) {
		perror("command pipe write for type failed");
		exit(1);
	}

	if (verbose)
		fprintf(stderr, "Writing length %d to command pipe\n", len);

	if (write(cmdpipe[WRITE_PIPE], &len, sizeof(int)) < 0) {
		perror("command pipe write for length failed");
		exit(1);
	}

	if (len > 0) {
		if (verbose)
			fprintf(stderr, "Writing data >%s< to command pipe\n",
				cmd);
		if (write(cmdpipe[WRITE_PIPE], cmd, len) < 0) {
			perror("command pipe write of data failed");
			exit(1);
		}
	}
}

/*
 * Read a response from the command pipe, first we will receive the
 * length of the response then the actual data.
 */
static void
read_cmd_pipe(returns_t *response)
{
	int len, type;
	struct pollfd readfd[2];
	char *str;

	/*
	 * Check if there is data to read - just in case slave has died, we
	 * don't want to block on the read and just hang.  We also check
	 * output from the slave because the slave may be blocked waiting
	 * for a flush on its stdout.
	 */
	readfd[0].fd = slvpipe[READ_PIPE];
	readfd[0].events = POLLIN;
	readfd[1].fd = master;
	readfd[1].events = POLLIN;

	do {
		readfd[0].revents = 0;
		readfd[1].revents = 0;

		if (poll(readfd, 2, 4000) == 0) {
			fprintf(stderr,
				"command pipe read timeout on line %d"
				" of file %s\n",
				line, cur_file);
			exit(2);
		}

		if ((readfd[1].revents & POLLIN) == POLLIN) {
			if (verbose)
				fprintf(stderr,
					"draining output from slave\n");
			save_slave_output(false);
		}
	}
	while((readfd[1].revents & POLLIN) == POLLIN);

	if (read(slvpipe[READ_PIPE], &type, sizeof(int)) < 0) {
		perror("command pipe read for type failed");
		exit(1);
	}
	response->return_type = type;

	if ((type != ret_ok) && (type != ret_err) && (type != ret_count)) {
		if (read(slvpipe[READ_PIPE], &len, sizeof(int)) < 0) {
			perror("command pipe read for length failed");
			exit(1);
		}
		response->return_len = len;

		if (verbose)
			fprintf(stderr,
				"Reading %d bytes from command pipe\n", len);

		if ((response->return_value = malloc(len + 1)) == NULL)
			err(1, "Failed to alloc memory for cmd pipe read");

		if (read(slvpipe[READ_PIPE], response->return_value, len) < 0) {
			perror("command pipe read of data failed");
			exit(1);
		}

		if (response->return_type != ret_byte) {
			str = response->return_value;
			str[len] = '\0';

			if (verbose)
				fprintf(stderr, "Read data >%s< from pipe\n",
					response->return_value);
		}
	} else {
		response->return_value = NULL;
		if (type == ret_count) {
			if (read(slvpipe[READ_PIPE], &len, sizeof(int)) < 0) {
				perror("command pipe read for number of "
				       "returns failed");
				exit(1);
			}
			response->return_len = len;
		}

		if (verbose)
			fprintf(stderr, "Read type %s from pipe\n",
				returns_enum_names[type]);
	}
}

/*
 * Check for writes from the slave on the pty, save the output into a
 * buffer for later checking if discard is false.
 */
#define MAX_DRAIN 256

static void
save_slave_output(bool discard)
{
	struct pollfd fd;
	char *new_data, drain[MAX_DRAIN];
	size_t to_allocate;
	int result, i;

	fd.fd = master;
	fd.events = POLLIN;
	fd.revents = 0;

	result = 0;
	while (1) {
		if (result == -1)
			err(2, "poll of slave pty failed");
		result = MAX_DRAIN;
		if ((result = read(master, &drain, result)) < 0) {
			if (errno == EAGAIN)
				break;
			else
				err(2, "draining slave pty failed");
		}

		fd.revents = 0;
		if (discard != true) {
			if (result >
			    (saved_output.allocated - saved_output.count)) {
				to_allocate = 1024 * ((result / 1024) + 1);

				if ((new_data = realloc(saved_output.data,
					saved_output.allocated + to_allocate))
				    == NULL)
					err(2, "Realloc of saved_output failed");
				saved_output.data = new_data;
				saved_output.allocated += to_allocate;
			}

			if (verbose) {
				fprintf(stderr, "count = %d, allocated = %d\n",
					saved_output.count,
					saved_output.allocated);
				for (i = 0; i < result; i++) {
					fprintf(stderr, "Saving slave output "
						"0x%x (%c)\n", drain[i],
						(drain[i] >= ' ')? drain[i] : '-');
				}
			}

			memcpy(&saved_output.data[saved_output.count], drain,
			       result);
			saved_output.count += result;

			if (verbose) {
				fprintf(stderr, "count = %d, allocated = %d\n",
					saved_output.count,
					saved_output.allocated);
			}
		} else {
			if (verbose) {
				for (i=0; i < result; i++) {
					fprintf(stderr, "Discarding slave "
						"output 0x%x (%c)\n",
						drain[i],
						(drain[i] >= ' ')? drain[i] : '-');
				}
			}
		}
	}
}

static void
yyerror(const char *msg)
{
	warnx("%s in line %zu of file %s", msg, line, cur_file);
}
