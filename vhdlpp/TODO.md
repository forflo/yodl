# Ad-hoc todos

- Add simple stateful lambda (or traverser class)
  that searches for constructs not meant for simulation

## parse.y

Not all names can be parsed
Missing non-terminals/terminals are:
- character literal
- slice name
- attribute name
- external name

For more details visit: (VHDL-2008, p 8.1)

## enum_overloads.cc

- TODO: overload operator `>>` for use with `operator_symbol_t`
