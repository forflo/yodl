library ieee;
use ieee.std_logic_1164.all;
use ieee.std_logic_arith.all;
use ieee.std_logic_unsigned.all;

entity adder is
   port(A        : in  std_logic;
        B        : in  std_logic;
        C        : in  std_logic;
        carryIn  : in  std_logic;
        carryOut : out std_logic;
        fnord    : out std_logic;
        sum      : out std_logic);
end adder;

architecture behv of adder is
begin

   process(A) is
   begin
      case A is
         when '0' =>
            case B is
               when '0' => sum <= '0';
               when '1' => sum <= '1';
            end case;
         when '1' =>
            case C is
               when '0' => sum <= '0';
               when '1' => sum <= '1';
            end case;
      end case;
   end process;
end behv;
