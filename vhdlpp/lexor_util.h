//FM. MA
#ifndef IVL_LEXOR_UTIL_GUARD
#define IVL_LEXOR_UTIL_GUARD

# include "parse_api.h"

# include <cmath>
# include <cassert>
# include <iostream>
# include <set>

//TODO: Is there any better way to do this?
//      does flex include a switch that can be used
//      to generate these forward declarations?
extern int yylex_init(yyscan_t *);
extern void yyset_in(FILE *, yyscan_t);
extern int yylex_destroy(yyscan_t);

class LexerUtil {
public:
    LexerUtil() {};

    static inline yyscan_t prepare_lexor(FILE *fd) {
        yyscan_t scanner;
        yylex_init(&scanner);
        yyset_in(fd, scanner);

        return scanner;
    }

    //FM. MA TODO: Remove this function!
    static inline void destroy_lexor(yyscan_t scanner) {
        yylex_destroy(scanner);
    }
};

#endif /* IVL_LEXOR_UTIL_GUARD */
