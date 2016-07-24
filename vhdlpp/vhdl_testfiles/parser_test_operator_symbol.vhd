-- parser testfile
----
library ieee;
use ieee.std_logic_1164.all;
use ieee.std_logic_arith.all;
use ieee.std_logic_unsigned.all;

entity dummy is
   port (Cin, x, y : in  std_logic;
         s, Cout   : out std_logic);
end dummy;

architecture a of dummy is
begin
   -- of course, this makes no sense.
   -- but according to p. 8.1 of VHDL-2008
   -- this has to be (at least syntactically) possible
   add   : s    <= "+" xor "*" xor "+";
end a;
