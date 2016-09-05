library ieee;
use ieee.std_logic_1164.all;
use ieee.std_logic_arith.all;
use ieee.std_logic_unsigned.all;

entity adder is
   port(A      : out std_logic;
        B      : out std_logic;
        sig    : in  std_logic_vector(1 downto 0);
        C      : in  std_logic);
end adder;

architecture behv of adder is
   function rising_edge(c : in std_logic) return boolean;
begin
   process(A) is
   begin
      if A = B then
         if not A then
            case sig is
               when "00" => C <= '0';
               when "01" => C <= '0';
               when "10" => C <= '1';
               when "11" => C <= '0';
            end case;
         end if;
      end if;
   end process;
end behv;
