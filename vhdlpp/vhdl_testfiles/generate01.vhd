library ieee;
use ieee.std_logic_1164.all;

entity ent is
    port(x: in std_logic;
         F: out std_logic);
end ent;

architecture beh of ent is
begin
    mygen: for I in 5 downto 0 generate
        mygen_nested: for J in I downto 0 generate

            myproc: process(x) is
                variable foo : integer := J;
                variable bar : integer := I;
            begin
                if (x='1') then
                    F <= '1';
                else
                    F <= '0';
                end if;
            end process myproc;
        end generate mygen_nested;
    end generate mygen;
end beh;
