`timescale 1ns / 1ps

module log2fun#(
    parameter   DW  =   64      //data width
    )(
	input wire [DW-1:0] in,
	//input wire clk,
	//input wire rst_n,

	output wire [DW-1:0] out
	);
// 方案一
function integer clogb2 (input integer bit_depth);
begin
	for(clogb2=0; bit_depth>0; clogb2=clogb2+1)
		bit_depth = bit_depth>>1;
end
endfunction

assign out = clogb2(in-1'b1);

endmodule
