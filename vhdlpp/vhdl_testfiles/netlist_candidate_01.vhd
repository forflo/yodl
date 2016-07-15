-- this vhdl source resembles following verilog code
-- module ripplemod(a, b, cin, sum, cout);
--     input [7:0] a;
--     input [7:0] b;
--     input cin;
--     output [7:0]sum;
--     output cout;
--     wire[6:0] c;
-- 
--     fulladd a1(a[0],b[0],cin,sum[0],c[0]);
--     fulladd a2(a[1],b[1],c[0],sum[1],c[1]);
--     fulladd a3(a[2],b[2],c[1],sum[2],c[2]);
--     fulladd a4(a[3],b[3],c[2],sum[3],c[3]);
--     fulladd a5(a[4],b[4],c[3],sum[4],c[4]);
--     fulladd a6(a[5],b[5],c[4],sum[5],c[5]);
--     fulladd a7(a[6],b[6],c[5],sum[6],c[6]);
--     fulladd a8(a[7],b[7],c[6],sum[7],cout);
-- endmodule
-- 
-- module fulladd(a, b, cin, sum, cout);
--     input a; input b; input cin;
--     output sum; output cout;
-- 
--     assign sum=(a^b^cin);
--     assign cout=((a&b)|(b&cin)|(a&cin));
-- endmodule
-- 
-- which expands to:
--
-- sum(0) <= a(0) xor b(0) xor cin
-- c(0) <= (a(0) and b(0)) or (b(0) and cin) or (a(0) and cin);
-- 
-- sum(1) <= a(1) xor b(1) xor c(0)
-- c(1) <= (a(1) and b(1)) or (b(1) and c(0)) or (a(1) and c(0));
-- 
-- sum(2) <= a(2) xor b(2) xor c(1)
-- c(2) <= (a(2) and b(2)) or (b(2) and c(1)) or (a(2) and c(1));
-- 
-- sum(3) <= a(3) xor b(3) xor c(2)
-- c(3) <= (a(3) and b(3)) or (b(3) and c(2)) or (a(3) and c(2));
-- 
-- sum(4) <= a(4) xor b(4) xor c(3)
-- c(4) <= (a(4) and b(4)) or (b(4) and c(3)) or (a(4) and c(3));
-- 
-- sum(5) <= a(5) xor b(5) xor c(4)
-- c(5) <= (a(5) and b(5)) or (b(5) and c(4)) or (a(5) and c(4));
-- 
-- sum(6) <= a(6) xor b(6) xor c(5)
-- c(6) <= (a(6) and b(6)) or (b(6) and c(5)) or (a(6) and c(5));
-- 
-- sum(7) <= a(7) xor b(7) xor c(6)
-- cout <= (a(7) and b(7)) or (b(7) and c(6)) or (a(7) and c(6));
----

library ieee;
use ieee.std_logic_1164.all;
use ieee.std_logic_arith.all;
use ieee.std_logic_unsigned.all;

entity adder is
    generic(n: natural := 2);
    port(	A: in std_logic_vector(7 downto 0);
	        B: in std_logic_vector(7 downto 0);
	        cin: in std_logic;
            cout: out std_logic;
	        sum:  out std_logic_vector(7 downto 0));
end adder;

architecture behv of adder is
    signal wire: std_logic_vector(8 downto 0);
begin
    wire(0) <= cin;
    cout <= wire(8);

    rippleCarry : for pos in 0 to 7 generate
        sum(pos) <= a(pos) xor b(pos) xor wire(pos);
        wire(pos + 1) <= 
            (a(pos) and b(pos)) or 
            (b(pos) and wire(pos)) or 
            (a(pos) and wire(pos));
    end generate rippleCarry;
end behv;
