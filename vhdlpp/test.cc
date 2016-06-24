// FM. MA

#include <fstream>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <sys/stat.h>
#include <cerrno>
#include <limits>
#include <vector>
#include <map>
#include <iostream>
#include <math.h>

// code base specific includes
#include "vhdlpp_config.h"
#include "generate_graph.h"
#include "version_base.h"
#include "simple_tree.h"
#include "StringHeap.h"
#include "entity.h"
#include "compiler.h"
#include "sequential.h"
#include "library.h"
#include "std_funcs.h"
#include "std_types.h"
#include "architec.h"
#include "parse_api.h"
#include "traverse_all.h"
#include "vtype.h"

// testing library
#define CATCH_CONFIG_MAIN
#include "catch.h"


unsigned int Fac(unsigned int number){
    return number <= 1 ? number : Fac(number-1)*number;
}

TEST_CASE("Factorials are computed", "[Fac]"){
    REQUIRE(Fac(1) == 1);
    REQUIRE(Fac(2) == 2);
    REQUIRE(Fac(3) == 6);
    REQUIRE(Fac(10) == 3628800);
}

