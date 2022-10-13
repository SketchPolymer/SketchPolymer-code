`timescale 1ns / 1ps

(* dont_touch="true" *)

module distri_sketch_ram_top_2RAM#(
	parameter               DW  =   64,     //data width
									HW  =   10      //hash width
	)(
	//system signals
	input                   clk,
	input                   rst,

	//RAM A
	input                   rama_wea_i,
	input   [HW-1:0]        rama_addra_i,
	input   [DW-1:0] 		 	rama_dina_i,
	output  [DW-1:0] 		 	rama_douta_o,

	input                   rama_web_i,
	input   [HW-1:0]        rama_addrb_i,
	input   [DW-1:0] 		 	rama_dinb_i,
	output  [DW-1:0] 		 	rama_doutb_o,

	//RAM B
	input                   ramb_wea_i,
	input   [HW-1:0]        ramb_addra_i,
	input   [DW-1:0] 		 	ramb_dina_i,
	output  [DW-1:0] 		 	ramb_douta_o,

	input                   ramb_web_i,
	input   [HW-1:0]        ramb_addrb_i,
	input   [DW-1:0] 		 	ramb_dinb_i,
	output  [DW-1:0] 		 	ramb_doutb_o
	);

    
	dual_port_ram #(
		 .DPW        (HW),
		 .DW         (DW)
	) U0_dual_port_ram(
		 //port a
		 .clka       (clk),
		 .wea        (rama_wea_i),
		 .addra      (rama_addra_i),
		 .dina       (rama_dina_i),
		 .douta      (rama_douta_o),

		 //port b
		 .clkb       (clk),
		 .web        (rama_web_i),
		 .addrb      (rama_addrb_i),
		 .dinb       (rama_dinb_i),
		 .doutb      (rama_doutb_o)
	);

	dual_port_ram #(
		 .DPW        (HW),
		 .DW         (DW)
	) U1_dual_port_ram(
		 //port a
		 .clka       (clk),
		 .wea        (ramb_wea_i),
		 .addra      (ramb_addra_i),
		 .dina       (ramb_dina_i),
		 .douta      (ramb_douta_o),

		 //port b
		 .clkb       (clk),
		 .web        (ramb_web_i),
		 .addrb      (ramb_addrb_i),
		 .dinb       (ramb_dinb_i),
		 .doutb      (ramb_doutb_o)
	);

endmodule




module distri_sketch_ram_top_4RAM#(
	parameter               DW  =   32,     //data width
									HW  =   10      //hash width
	)(
	//system signals
	input                   clk,
	input                   rst,

	//RAM A
	input                   rama_wea_i,
	input   [HW-1:0]        rama_addra_i,
	input   [DW-1:0] 		 	rama_dina_i,
	output  [DW-1:0] 		 	rama_douta_o,

	input                   rama_web_i,
	input   [HW-1:0]        rama_addrb_i,
	input   [DW-1:0] 		 	rama_dinb_i,
	output  [DW-1:0] 		 	rama_doutb_o,

	//RAM B
	input                   ramb_wea_i,
	input   [HW-1:0]        ramb_addra_i,
	input   [DW-1:0] 		 	ramb_dina_i,
	output  [DW-1:0] 		 	ramb_douta_o,

	input                   ramb_web_i,
	input   [HW-1:0]        ramb_addrb_i,
	input   [DW-1:0] 		 	ramb_dinb_i,
	output  [DW-1:0] 		 	ramb_doutb_o,
	
	//RAM C
	input                   ramc_wea_i,
	input   [HW-1:0]        ramc_addra_i,
	input   [DW-1:0] 		 	ramc_dina_i,
	output  [DW-1:0] 		 	ramc_douta_o,

	input                   ramc_web_i,
	input   [HW-1:0]        ramc_addrb_i,
	input   [DW-1:0] 		 	ramc_dinb_i,
	output  [DW-1:0] 		 	ramc_doutb_o,
	
	//RAM D
	input                   ramd_wea_i,
	input   [HW-1:0]        ramd_addra_i,
	input   [DW-1:0] 		 	ramd_dina_i,
	output  [DW-1:0] 		 	ramd_douta_o,

	input                   ramd_web_i,
	input   [HW-1:0]        ramd_addrb_i,
	input   [DW-1:0] 		 	ramd_dinb_i,
	output  [DW-1:0] 		 	ramd_doutb_o
	);

    
	dual_port_ram #(
		 .DPW        (HW),
		 .DW         (DW)
	) U0_dual_port_ram(
		 //port a
		 .clka       (clk),
		 .wea        (rama_wea_i),
		 .addra      (rama_addra_i),
		 .dina       (rama_dina_i),
		 .douta      (rama_douta_o),

		 //port b
		 .clkb       (clk),
		 .web        (rama_web_i),
		 .addrb      (rama_addrb_i),
		 .dinb       (rama_dinb_i),
		 .doutb      (rama_doutb_o)
	);

	dual_port_ram #(
		 .DPW        (HW),
		 .DW         (DW)
	) U1_dual_port_ram(
		 //port a
		 .clka       (clk),
		 .wea        (ramb_wea_i),
		 .addra      (ramb_addra_i),
		 .dina       (ramb_dina_i),
		 .douta      (ramb_douta_o),

		 //port b
		 .clkb       (clk),
		 .web        (ramb_web_i),
		 .addrb      (ramb_addrb_i),
		 .dinb       (ramb_dinb_i),
		 .doutb      (ramb_doutb_o)
	);
	
	dual_port_ram #(
		 .DPW        (HW),
		 .DW         (DW)
	) U2_dual_port_ram(
		 //port a
		 .clka       (clk),
		 .wea        (ramc_wea_i),
		 .addra      (ramc_addra_i),
		 .dina       (ramc_dina_i),
		 .douta      (ramc_douta_o),

		 //port b
		 .clkb       (clk),
		 .web        (ramc_web_i),
		 .addrb      (ramc_addrb_i),
		 .dinb       (ramc_dinb_i),
		 .doutb      (ramc_doutb_o)
	);
	
	dual_port_ram #(
		 .DPW        (HW),
		 .DW         (DW)
	) U3_dual_port_ram(
		 //port a
		 .clka       (clk),
		 .wea        (ramd_wea_i),
		 .addra      (ramd_addra_i),
		 .dina       (ramd_dina_i),
		 .douta      (ramd_douta_o),

		 //port b
		 .clkb       (clk),
		 .web        (ramd_web_i),
		 .addrb      (ramd_addrb_i),
		 .dinb       (ramd_dinb_i),
		 .doutb      (ramd_doutb_o)
	);

endmodule
