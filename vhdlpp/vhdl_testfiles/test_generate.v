`ifndef __VHDL_STD_TYPES
`define __VHDL_STD_TYPES
typedef enum int {\false , \true } \boolean  ;
typedef enum int {\read_mode , \write_mode , \append_mode } \file_open_kind  ;
typedef enum int {\open_ok , \status_error , \name_error , \mode_error } \file_open_status  ;
`endif
module \driver (input wire logic \x  , 
output logic \f  );
genvar \mygen:i ;
for (\mygen:i = 32'd0; \mygen:i <= 32'd5; \mygen:i = \mygen:i + 1) begin : \mygen
genvar \mygen_nested:j ;
for (\mygen_nested:j = 32'd0; \mygen_nested:j <= \mygen:i ; \mygen_nested:j = \mygen_nested:j + 1) begin : \mygen_nested
always begin : __scope_1
int \foo   = \j ;
if (\x  === 1'b1) begin
\f  <= 1'b1;
end else begin
\f  <= 1'b0;
end
@(\x ); /* sensitivity list for process */
end /* __scope_1 */
end
end
endmodule
