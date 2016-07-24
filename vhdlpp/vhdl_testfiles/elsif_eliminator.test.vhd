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
   fooProc : process() is
      variable baz : natural := 4711;
   begin
      -- without else
      if (3 = 3) then
         baz := 3
      elsif (4 = 4) then
         baz := 4;
      elsif (5 = 5) then
         baz := 5;
      elsif (6 = 6) then
         baz := 6;
      end if;

      -- with else
      if (3 = 3) then
         baz := 3
      elsif (4 = 4) then
         baz := 4;
      elsif (5 = 5) then
         baz := 5;
      elsif (6 = 6) then
         baz := 6;
      else
         baz := 100000;
      end if;
   end process fooProc;
end beh;
