library ieee;
use ieee.std_logic_1164.all;
use ieee.std_logic_arith.all;
use ieee.std_logic_unsigned.all;

-- dummy entity
entity ent is
   generic(n : natural := 2);
   port(A     : in  std_logic_vector(n - 1 downto 0);
        B     : in  std_logic_vector(n - 1 downto 0);
        carry : out std_logic;
        sum   : out std_logic_vector(n - 1 downto 0));
end ent;

architecture beh of ent is
   signal result : std_logic_vector(n downto 0);
begin
   -- this should be parsable
   result <= foo(fnord => 3, foobar => 4)(3);
end beh;
