#ifndef IVL_UNIT_TESTS
#define IVL_UNIT_TESTS

#include <iostream>
#include <fstream>
#include <sstream>

// Where to dump design entities
const char *work_path = "ivl_vhdl_work";
const char *dump_design_entities_path = 0;
const char *dump_libraries_path       = 0;
const char *debug_log_path            = 0;

bool verbose_flag = false;
bool debug_elaboration = false;
std::ofstream debug_log_file;

#endif /* IVL_UNIT_TESTS */
