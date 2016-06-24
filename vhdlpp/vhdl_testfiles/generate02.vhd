library ieee;
use ieee.std_logic_1164.all;

entity driver is
    port(x: in std_logic;
         F: out std_logic);
end Driver;

architecture behav of driver is
begin
    mygen: for I in 2 downto 1 generate
        mygen_nested: for J in I downto 1 generate
            my_if : if (I /= J) generate

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
            end generate my_if;
        end generate mygen_nested;
    end generate mygen;
end behav;

--
-- elaboration von mygen
--

mygen : block
  constant i : integer := 2;
begin
  mygen_nested : block
    constant j : integer := 2;
  begin
    -- fällt weg wegen (2 /= 2) = false
  end mygen_nested;

  mygen_nested : block
    constant j : integer := 1;
  begin
    my_if : block
    begin
      myproc : process(x) is
        variable foo : integer := 1;
        variable bar : integer := 2;
      begin
        if (x = '1') then
          f <= '1';
        else
          f <= '0';
        end if;
      end process myproc;
    end block my_if;
  end mygen_nested;
end mygen;

mygen : block
  constant i : integer := 1;
begin
  mygen_nested : block
    constant j : integer := 1;
  begin
    -- fällt weg wegen (1 /= 1) = false
  end block mygen_nested;
end block mygen;
