library ieee;
use ieee.std_logic_1164.all;
use ieee.std_logic_arith.all;
use ieee.std_logic_unsigned.all;

entity adder is
   port(D       : out  std_logic_vector(2 downto 0);
        clock   : in  std_logic;
        enable2 : in  std_logic;
        enable  : in  std_logic);
end adder;

architecture behv of adder is
   function rising_edge(c : in std_logic) return boolean;
begin
   process(A) is
   begin
      if rising_edge(clock) then
         D(0) <= '0';
         D(1) <= '1';
         D(2) <= '1';
      end if;
   end process;
end behv;
