---------------------------------------------------------------
-- FIR Digital Filter Design (DSP example)
-- tested by Weijun Zhang, 04/2001
--
-- VHDL Data-Flow modeling
-- KEYWORD:
-- generate, array, range, constant and subtype
---------------------------------------------------------------

library IEEE;                           -- declare the library
use IEEE.std_logic_1164.all;
use IEEE.std_logic_arith.all;

---------------------------------------------------------------

entity FIR_filter is
   port(rst      : in  std_logic;
        clk      : in  std_logic;
        coef_ld  : in  std_logic;
        start    : in  std_logic;
        o_enable : in  std_logic;
        bypass   : in  std_logic;
        Xn_in    : in  std_logic_vector(3 downto 0);
        Yn_in    : in  std_logic_vector(15 downto 0);
        Xn_out   : out std_logic_vector(3 downto 0);
        Yn_out   : out std_logic_vector(15 downto 0));
end FIR_filter;

architecture BEH of FIR_filter is
   constant K : integer := 4;           -- circuit has four stages

   -- use type and subtype to define the complex signals

   subtype bit4 is std_logic_vector(3 downto 0);
   subtype bit8 is std_logic_vector(7 downto 0);
   subtype bit16 is std_logic_vector(15 downto 0);

   type klx4 is array (K downto 0) of bit4;
   type kx8 is array (K-1 downto 0) of bit8;
   type klx8 is array (K downto 0) of bit8;
   type kx16 is array (K-1 downto 0) of bit16;
   type klx16 is array (K downto 0) of bit16;

   -- define signal in type of arrays
   signal REG1, REG2, COEF : klx4;
   signal MULT8            : kx8;
   signal MULT16           : kx16;
   signal SUM              : klx16;
   signal Xn_tmp           : bit4;
   signal Yn_tmp           : bit16;

begin

   -- initialize the first stage of FIR circuit
   REG2(K) <= Xn_in when (start = '1')
              else (REG2(K)'range => '0');
   REG1(K) <= (REG1(K)'range => '0');
   SUM(K)  <= Yn_in;
   COEF(K) <= Xn_in;

   -- start the computation, use generate to obtain the
   -- multiple stages

   gen8 : for j in K-1 downto 0 generate

      stages : process (rst, clk)
      begin

         if (rst = '0') then
            REG1(j)   <= (REG1(j)'range   => '0');
            REG2(j)   <= (REG2(j)'range   => '0');
            COEF(j)   <= (COEF(j)'range   => '0');
            MULT16(j) <= (MULT16(j)'range => '0');
            SUM(j)    <= (SUM(j)'range    => '0');
         elsif (clk'event and clk = '1') then
            REG1(j) <= REG2(j+1);
            REG2(j) <= REG1(j);
            if (coef_ld = '1') then
               COEF(j) <= COEF(j+1);
            end if;
            MULT8(j)              <= signed(REG1(j))*signed(COEF(j));
            MULT16(j)(7 downto 0) <= MULT8(j);
            if (MULT8(j)(7) = '0') then
               MULT16(j)(15 downto 8) <= "00000000";
            else
               MULT16(j)(15 downto 8) <= "11111111";
            end if;
            SUM(j) <= signed(MULT16(j))+signed(SUM(j+1));
         end if;

      end process;

   end generate;

   -- control the outputs by concurrent statements

   Xn_tmp <= Xn_in when bypass = '1' else
             REG2(0) when coef_ld = '0' else
             COEF(0);

   Yn_tmp <= Yn_in when bypass = '1' else
             SUM(0);

   Xn_out <= Xn_tmp when o_enable = '0' else
             (Xn_out'range => 'Z');

   Yn_out <= Yn_tmp when o_enable = '0' else
             (Yn_out'range => 'Z');

end BEH;
