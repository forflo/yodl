/*
 * Usage:  vhdlpp [flags] sourcefile...
 * Flags:
 *
 **  -D <token>
 *     This activates various sorts of debugging aids. The <token>
 *     specifies which debugging aid to activate. Valid tokens are:
 *
 *     yydebug | no-yydebug
 *        Enable (disable) debug prints from the bison parser
 *
 *     libraries=<path>
 *        Enable debugging of library support by dumping library
 *        information to the file named <path>.
 *
 *     elaboration=<path>
 *        Enable debugging of elaboration by dumping elaboration
 *        process information to the file named <path>.
 *
 *     entities=<path>
 *        Enable debugging of elaborated entities by writing the
 *        elaboration results to the file named <path>.
 *
 **  -v
 *     Verbose operation. Display verbose non-debug information.
 *
 **  -V
 *     Version. Print the version of this binary.
 *
 **  -w <path>
 *     Work path. This sets the path to the working library
 *     directory. I write into that directory files for packages that
 *     I declare, and I read from that directory packages that are
 *     already declared. The default path is "ivl_vhdl_work".
 */
