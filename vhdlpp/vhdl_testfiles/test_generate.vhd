
library ieee;
use ieee.std_logic_1164.all;

entity driver is
    port(x: in std_logic;
         F: out std_logic);
end Driver;  

architecture behav of driver is
    
begin
    mygen: for I in 5 downto 0 generate
        mygen_nested: for J in I downto 0 generate
            process(x) is
                variable foo : integer := J;
            begin
                if (x='1') then
                    F <= '1';
                else
                    F <= '0';
                end if;
            end process;
        end generate mygen_nested;
    end generate mygen;
end behav;
