
library ieee;
use ieee.std_logic_1164.all;
use ieee.std_logic_arith.all;
use ieee.std_logic_unsigned.all;

entity ForLoop is
   generic(n : natural := 2);
   port(A     : in  std_logic_vector(n - 1 downto 0);
        B     : in  std_logic_vector(n - 1 downto 0);
        carry : out std_logic;
        sum   : out std_logic_vector(n - 1 downto 0));
end ForLoop;

architecture behaviour of ForLoop is
   signal result : std_logic_vector(n downto 0);
begin
   gen : for i in 1 to 2 generate
      sum <= i;
   end generate gen;
end behaviour;
