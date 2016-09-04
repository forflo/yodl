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
        baz      : out std_logic_vector(7 downto 0);
        clock    : in  std_logic;
        clock2   : in  std_logic;
        sum      : out std_logic);
end adder;

architecture behv of adder is
   function rising_edge(c : in std_logic) return std_logic;
begin

   process(A) is
   begin
    baz <= "00101100";

      case "100" is
         when "000" => 
            if rising_edge(clock) then 
               if rising_edge(clock2) then
                   A <= '0';
               end if;
            end if;
         when "001" => A <= '1';
         when "010" => A <= '1';
         when "011" => B <= '1';
         when "100" => A <= '0';
         when "101" => A <= '1';
         when "110" => A <= '1';
         when "111" => A <= '1';
      end case;
   end process;
end behv;
