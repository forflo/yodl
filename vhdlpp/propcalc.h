#ifndef IVL_PROPCALC
#define IVL_PROPCALC

#include <string>
#include <vector>
#include <map>
#include <set>

struct PropcalcFormula {
    virtual ~PropcalcFormula() = default;

};

struct PropcalcVar : PropcalcFormula {
    std::string name_;
    PropcalcVar(const char *n) : name_(n) {}
};

struct PropcalcConstant : PropcalcFormula {
    bool value_;
    PropcalcConstant(const bool v) : value_(v) {}
};

struct PropcalcTerm : PropcalcFormula {
    enum operator_t { AND, OR, NOR, XOR, NAND, IFTHEN };
    PropcalcFormula *l_, *r_;
    operator_t op_;
    PropcalcTerm(PropcalcFormula *a, operator_t o, PropcalcFormula *b)
        : l_(a), r_(b), op_(o) {}
};

class PropcalcApi {
public:
    static bool evaluate(PropcalcFormula *,
                         std::map<std::string, bool> &);
    static void extractNames(PropcalcFormula *, std::set<std::string> &);
    static bool proove(PropcalcFormula *form);

private:
    static bool prooveH(PropcalcFormula *form,
                        std::vector<std::string> todo,
                        std::map<std::string, bool> m);
};

#endif /* IVL_PROPCALC */
