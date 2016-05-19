---------------------------------------
-- driver (ESD book figure 2.3)		
--
-- two descriptions provided
----------------------------------------

library ieee;
use ieee.std_logic_1164.all;

----------------------------------------

entity driver is
port(	x: in std_logic;
	F: out std_logic
);
end Driver;  

----------------------------------------

architecture behaviour of driver is
begin

    myproc: process(x)
    begin
        -- compare to truth table
        if (x='1') then
            F <= '1';
        else
            F <= '0';
        end if;
    end process myproc;

end behaviour;
