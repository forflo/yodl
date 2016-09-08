library ieee;
use ieee.std_logic_1164.all;
use ieee.std_logic_arith.all;
use ieee.std_logic_unsigned.all;

entity clufu is
   port(A       : out std_logic;
        B       : out std_logic;
        Co      : out std_logic;
        sel    : in  std_logic_vector(1 downto 0);
        enable  : in  std_logic;
        enable2 : in  std_logic;
        clock    : in  std_logic);
end clufu;

architecture b of clufu is
   function rising_edge(c : in std_logic) return boolean;
begin
   process(A) is
   begin
      if A = B then
         if not A then
            if rising_edge(clock) then
               case sel is
                  when "00" =>
                     if rising_edge(clock) and enable = '1' and (not enable2 = '0') then
                        Co <= '0';
                     end if;
                  when "01" => Co <= '1';
                  when "10" => Co <= '1';
                  when "11" => Co <= '1';
               end case;
            end if;
         end if;
      end if;
   end process;
end b;
