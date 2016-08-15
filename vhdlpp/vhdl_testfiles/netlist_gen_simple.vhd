library ieee;
use ieee.std_logic_1164.all;
use ieee.std_logic_arith.all;
use ieee.std_logic_unsigned.all;

entity adder is
   port(A        : in  std_logic;
        B        : in  std_logic;
        carryIn  : in  std_logic;
        carryOut : out std_logic;
        fnord    : out std_logic;
        sum      : out std_logic);
end adder;

architecture behv of adder is
begin
--   sum      <= A xor B xor carryIn;
   sum <= '0';
   carryOut <= (a and b) or
               (b and carryIn) or
               (a and carryIn);

   fnord <= ('1' or '0') and '1';
end behv;
