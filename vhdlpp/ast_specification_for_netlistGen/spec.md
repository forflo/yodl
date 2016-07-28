# Modified stquential statement list

```
sequential_statement ::= 
    wait_statement |
    simple_signal_assignment_statement |
    simple_variable_assignment_statement |
    case_statement
```

The following structures have to be expressed
by only using the above mentioned grammar rules:

```
    conditional_signal_assignment 
        => if_statement 
        => case_statement and simple_signal_assignment_statement
    selected_signal_assignment 
        => case_statement
    conditional_variable_assignment 
        => if_statement 
        => case_statement and simple_variable_assignment_statement
    selected_variable_assignment 
        => case_statement
    if_statement 
        => case_statement
    loop_statement 
        => eliminated through loop unroller
    next_statement 
        => see above
    exit_statement 
        => see above
    procedure_call_statement 
        => eliminatied through procedure inliner
    return_statement 
        => see above
```
Where `=>` is a notation for `desugars to`

# Modified concurrent statement list

```
concurrent_statement ::=
    block_statement |
    process_statement |
    component_instantiation_statement
```

The following structures of `lists of concurrent statements` have
to be expressed by only using the above mentioned grammar rules

```
    concurrent_procedure_call_statement => process_statement
    concurrent_assertiona_statement => process_statement
    concurrent_signal_assignment_statement => process_statement
    genreate_statement => unrolled by generate evaluator, 
                          after that, this rule has to be applied
                          recursively on all concurrent statements
                          inside of the generated block statements, 
                          Since generate constructs always evaluate
                          to block statements; at least according to the
                          VHDL 2008 language reference standard.

```

# Constraints on expressions

- rvalue expressions only can have a type of `std_logic` or `std_logic_vector`
- lvalue expressions only can have the same type as above and, of course, must
  match the type of the assigned rvalue

# Desugaring of expressions using arrays with dimensionality greater 1 or records
## Assignment

Let the following types be defined:

```
type arr1 is array (0 to 1, 0 to 1) of std_logic

type rec1 is record
    foo, bar : std_logic;
    baz : arr1;
end record;
```

Now, let the following signals/variables be defined:

```
fooBlock : block is
    signal sig : arr1 := (('0', '1'), ('-', '-'));
    variable var : rec1 := (foo => '0', bar => '1', baz => (('0', '1'), ('1', '0')));
begin
    if (sig(0,0) = '0') then
        var := (foo => '1', bar => '0', baz => (('0', '0'), ('0', '0')));
    end if;
end block fooBlock;
```

Above code shall now be explanatory desugared.

```
fooBlock : block is
    -- since already declared signals and variables can not
    -- be easily deleted. This step is left for the 
    -- final cleanup and deallocation step of the AST
    signal sig : arr1 := (('0', '1'), ('-', '-'));
    variable var : rec1 := (foo => '0', bar => '1', baz => (('0', '1'), ('1', '0')));
    
    -- desugared declarations. 
    -- Note how everything maps to std_logic
    signal sig_arr1_0_0 := '0';
    signal sig_arr1_0_1 := '1';
    signal sig_arr1_1_0 := '-';
    signal sig_arr1_1_1 := '-';

    variable var_rec1_foo := '0';
    variable var_rec1_bar := '1';
    variable var_rec1_baz_arr1_0_0 := '0';
    variable var_rec1_baz_arr1_0_1 := '1';
    variable var_rec1_baz_arr1_1_0 := '1';
    variable var_rec1_baz_arr1_1_1 := '0';
begin
    if (sig_arr1_0_0 = '0') then
        var := (foo => '1', bar => '0', baz => (('0', '0'), ('0', '0')));
    end if;
end block fooBlock;
```
