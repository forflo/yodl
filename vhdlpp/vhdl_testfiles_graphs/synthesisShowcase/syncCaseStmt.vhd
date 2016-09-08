library ieee;
use ieee.std_logic_1164.all;
use ieee.std_logic_arith.all;
use ieee.std_logic_unsigned.all;

entity syncCase is
   port(A        : in  std_logic;
        sel      : in  std_logic_vector(2 downto 0);
        clock    : in  std_logic);
end syncCase;

architecture behv of syncCase is
   function rising_edge(c : in std_logic) return boolean;
begin
   process(A) is
   begin
      if rising_edge(clock) then
         case sel is
            when "000" => A <= '0';
            when "001" => A <= '1';
            when "010" => A <= '1';
            when "011" => A <= '1';
            when "100" => A <= '0';
            when "101" => A <= '1';
            when "110" => A <= '1';
            when "111" => A <= '1';
         end case;
      end if;
   end process;
end behv;
