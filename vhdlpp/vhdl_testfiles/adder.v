`ifndef __VHDL_STD_TYPES
`define __VHDL_STD_TYPES
typedef enum int {\false , \true } \boolean  ;
typedef enum int {\read_mode , \write_mode , \append_mode } \file_open_kind  ;
typedef enum int {\open_ok , \status_error , \name_error , \mode_error } \file_open_status  ;
`endif

module \adder #(parameter \n = 32'd2) (input wire logic[\n  - 32'd1:32'd0] \a  , 
input wire logic[\n  - 32'd1:32'd0] \b  , 
output wire logic \carry  , 
output wire logic[\n  - 32'd1:32'd0] \sum  );
wire logic[\n :32'd0] \result  ;
// vhdl_testfiles/adder.vhd:18
assign \result  = {1'b0, \a } + {1'b0, \b };
// vhdl_testfiles/adder.vhd:19
assign \sum  = \result [\n  - 32'd1:32'd0];
// vhdl_testfiles/adder.vhd:20
assign \carry  = \result [\n ];
endmodule
