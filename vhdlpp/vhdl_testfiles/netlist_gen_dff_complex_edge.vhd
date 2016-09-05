library ieee;
use ieee.std_logic_1164.all;
use ieee.std_logic_arith.all;
use ieee.std_logic_unsigned.all;

entity adder is
   port(A      : out std_logic;
        B      : out std_logic;
        C      : in  std_logic;
        clock  : in  std_logic;
        enable2 : in std_logic;
        enable : in  std_logic);
end adder;

architecture behv of adder is
   function rising_edge(c : in std_logic) return boolean;
begin
   process(A) is
   begin
      if rising_edge(clock) and enable = '1' and not (enable2 = '0') then
         A <= '0';
         B <= C;
      end if;
   end process;
end behv;
