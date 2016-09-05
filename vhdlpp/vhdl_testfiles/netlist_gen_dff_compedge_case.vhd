library ieee;
use ieee.std_logic_1164.all;
use ieee.std_logic_arith.all;
use ieee.std_logic_unsigned.all;

entity adder is
   port(D       : out  std_logic;
        A       : out  std_logic;
        sel     : in std_logic_vector(2 downto 0);
        clock   : in  std_logic;
        enable2 : in  std_logic;
        enable  : in  std_logic);
end adder;

architecture behv of adder is
   function rising_edge(c : in std_logic) return boolean;
begin
   process(A) is
   begin
      if rising_edge(clock) and enable = '1' and not (enable2 = '0') then
         case sel is
            when "000" => D <= '0';
            when "001" => D <= '1';
            when "010" => D <= '0';
            when "011" => D <= '1';
            when "100" => A <= '0';
            when "101" => D <= '1';
            when "110" => D <= '1';
            when "111" => D <= '0';
         end case;
      end if;
   end process;
end behv;
