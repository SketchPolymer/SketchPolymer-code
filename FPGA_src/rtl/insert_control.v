`timescale 1ns / 1ps

module insert_control#(
    parameter               	HW  =   10,     //hash width
										DW  =   64      //data width
    )(
	//system signals
	input clk,
	input rst,
	
	//external interface
	input 							insert_control_en_i,
	input 	[DW-1:0]				insert_id_i,
	input 	[DW-1:0]				insert_latency_i,
	input 	[DW-1:0]				insert_freq_th_i,


	output	reg					whole_insert_end,
	output 	[DW-1:0]				insert_id_i_o,
	output 	[DW-1:0]				insert_latency_o,
	output 	[DW-1:0]				insert_freq_th_o,
	
	
	
	//internal interface
	input								stage1_insert_end,
	input								stage2_insert_end,
	input								stage3_insert_end,
	input								stage4_insert_end,
	
	output	reg					stage1_insert,
	output							stage2_insert,
	output							stage3_insert,
	output							stage4_insert
	);


	//------------Declare Signals----------------
	parameter	IDLE 		= 	3'b000;
	parameter	STAGE1 	= 	3'b001;
	parameter	STAGE2 	= 	3'b010;
	parameter	STAGE3 	= 	3'b011;
	parameter	STAGE4 	= 	3'b100;
	parameter	OUTPUT 	= 	3'b101;
	

	reg[2:0]	current_state;
	reg[2:0]	next_state;
	
	reg		[DW-1:0]				insert_id;
	reg		[DW-1:0]				insert_latency;
	reg		[DW-1:0]				insert_freq_th;
	
	reg					stage2_insert_temp;
	reg					stage3_insert_temp;
	reg					stage4_insert_temp;
	
	wire					stage2_insert_temp_o;
	wire					stage3_insert_temp_o;
	wire					stage4_insert_temp_o;


	always @ (posedge clk or negedge rst) begin
		if(!rst)
			current_state <= IDLE;
		else
			current_state <= next_state;
	end

	always @ (*) begin
		case(current_state)
			IDLE: begin
				if(insert_control_en_i) next_state = STAGE1;
				else   next_state = IDLE;
			end
			STAGE1: begin
				if(stage1_insert_end) next_state = STAGE2;
				else    next_state = STAGE1;
			end
			STAGE2: begin
				if(stage2_insert_end) next_state = STAGE3;
				else    next_state = STAGE2;
			end
			STAGE3: begin
				if(stage3_insert_end) next_state = STAGE4;
				else    next_state = STAGE3;
			end
			STAGE4: begin
				if(stage4_insert_end) next_state = OUTPUT;
				else    next_state = STAGE4;
			end		
			OUTPUT: begin
				next_state = IDLE;
			end
			default : next_state = IDLE;
		endcase
	end

	
	always @ (posedge clk or negedge rst) begin
		case(current_state)
			IDLE: begin
					end
					
			STAGE1:begin
						stage1_insert  <=  1'b1;
					 end
					
			STAGE2:begin    
						stage1_insert  <=  1'b0;
						stage2_insert_temp  <=  1'b1;
					 end
					
			STAGE3:begin    
						stage2_insert_temp  <=  1'b0;
						stage3_insert_temp  <=  1'b1;
					 end
		  
		   STAGE4:begin    
						stage3_insert_temp  <=  1'b0;
						stage4_insert_temp  <=  1'b1;
					 end
					
			OUTPUT:begin
						stage4_insert_temp  <=  1'b0;
						whole_insert_end  <=1'b1;
					 end
					
			default: begin  
					
						end
		endcase
	end
	
	//------------Output Signals-----------------
	assign 		insert_id_i_o  =  insert_id_i;
	assign		insert_freq_th_o  =  insert_freq_th_i;
	
	assign 		stage2_insert  =  stage2_insert_temp_o;
	assign 		stage3_insert  =  stage3_insert_temp_o;
	assign		stage4_insert  =  stage4_insert_temp_o;
	
	log2fun U_log2fun(
		.in(insert_latency_i),
		.out(insert_latency_o)
	);
	
//	one_cycle_signal	U_one_cycle_signal(
//		.clk(clk),
//		.rst(rst),
//		.start(stage1_insert_temp),
//		.one_cycle(stage1_insert)
//	)
	
	one_cycle_signal	U2_one_cycle_signal(
		.clk(clk),
		.rst(rst),
		.start(stage2_insert_temp),
		.one_cycle(stage2_insert_temp_o)
	);
	
	one_cycle_signal	U3_one_cycle_signal(
		.clk(clk),
		.rst(rst),
		.start(stage3_insert_temp),
		.one_cycle(stage3_insert_temp_o)
	);
	
	one_cycle_signal	U4_one_cycle_signal(
		.clk(clk),
		.rst(rst),
		.start(stage4_insert_temp),
		.one_cycle(stage4_insert_temp_o)
	);

endmodule


module one_cycle_signal(
	input		clk,
	input		rst,
	input		start,
	output	reg	one_cycle
);

reg start_flag;

always @ (posedge clk or negedge rst) begin
	if(!rst) begin
	
	end
	else begin
		if(start)
			start_flag  <=  1'b1;
		else
			start_flag  <=  1'b0;
		if(start_flag)
			one_cycle  <=  1'b0;
		else if(start) begin
			one_cycle  <=  1'b1;
			start_flag  <=  1'b1;
		end
		else
			one_cycle  <=  1'b0;		
	end
	

end

endmodule
