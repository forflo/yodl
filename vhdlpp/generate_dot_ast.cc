#define XTL_DEFAULT_SYNTAX    'S'

#include <math.h>
#include <iostream>
#include <typeinfo>
#include <ivl_assert.h>

#include "mach7/match.hpp"                 // Support for Match statement
#include "mach7/patterns/constructor.hpp"  // Support for constructor patterns
#include "mach7/patterns/n+k.hpp"          // Support for n+k patterns

#include "architec.h"
#include "entity.h"
#include "expression.h"
#include "sequential.h"
#include "subprogram.h"
#include "vsignal.h"
#include "std_types.h"

using namespace mch; // Mach7's library namespace

class DotGenerator {
public:
    DotGenerator();
    ~DotGenerator() = default;
}

DotGenerator::DotGenerator()
