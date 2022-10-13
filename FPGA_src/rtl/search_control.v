`timescale 1ns / 1ps

module search_control#(
    parameter               	HW  =   10,     //hash width
										DW  =   64      //data width
    )(
	//system signals
	input clk,
	input rst,
	
	//external interface
	input 							search_control_en_i,
	input 	[DW-1:0]				search_id_i,
	input 	[DW-1:0]				search_quantile_i,
	
	output	reg	[DW-1:0]		w_latency_out,
	output	reg					whole_search_end,
	
	output	[DW-1:0]				search_id_o,
	output	[DW-1:0]				search_lantency_o,
	
	//internal interface
	input		[DW-1:0]				stage2_freq_out,
	input		[DW-1:0]				stage2_latency_out,
	
	input		[DW-1:0]				stage3_freq_out,
	input								stage4_freq_out,
	
	input								stage2_search_end,
	input								stage3_search_end,
	input								stage4_search_end,
	
	output						stage2_search,
	output						stage3_search,
	output						stage4_search
	);


	//------------Declare Signals----------------
	parameter	IDLE 		= 	3'b000;
	parameter	STAGE2 	= 	3'b001;
	parameter	STAGE4 	= 	3'b010;
	parameter	TMINUS1 	= 	3'b011;
	parameter	STAGE3 	= 	3'b100;
	parameter	OUTPUT 	= 	3'b101;
	
	reg		[DW-1:0]				stage2_frequency;
	reg		[DW-1:0]				stage2_latency;
	reg		[DW-1:0]				stage2_quantile;
	
	reg					stage2_search_temp;
	reg					stage3_search_temp;
	reg					stage4_search_temp;
	
	reg		[DW-1:0]				m;
	reg		[DW-1:0]				stage3_search_end_r;
	reg		[DW-1:0]				number_counter;

	reg[2:0]	current_state;
	reg[2:0]	next_state;
	
	always @ (posedge clk or negedge rst) begin										//output hash1/hash2 results
        if (!rst) begin
            stage3_search_end_r    <=  {HW{1'b0}};
        end
        else begin
            stage3_search_end_r    <=  stage3_search_end;
        end
    end 


	always @ (posedge clk or negedge rst) begin
		if(!rst)
			current_state <= IDLE;
		else
			current_state <= next_state;
	end

	always @ (*) begin
		case(current_state)
			IDLE: begin
				if(search_control_en_i) next_state = STAGE2;
				else   next_state = IDLE;
			end
			STAGE2: begin
				if(stage2_search_end) next_state = STAGE4;
				else    next_state = STAGE2;
			end
			STAGE4: begin
				if(stage4_search_end) begin
					if(stage4_freq_out)	next_state = STAGE3;
					else next_state = TMINUS1;
				end
				else    next_state = STAGE4;
			end
			TMINUS1: begin
				next_state = STAGE4;
			end
			STAGE3: begin
				if(stage3_search_end_r) begin
					  if(m>number_counter)	next_state = TMINUS1;
					  else	next_state = OUTPUT;
				end 
				else    next_state = STAGE3;
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
						whole_search_end  <=1'b0;
						number_counter  <=  0;
						w_latency_out	<= 0;
					end
			STAGE2:begin    
						stage2_search_temp  <=  1'b1;
						if(stage2_search_end) begin
							stage2_frequency  <=  stage2_freq_out;
							stage2_latency  <=  stage2_latency_out;
							stage2_quantile  <=  search_quantile_i;
							number_counter  <=  0;
							m  <=  (((64'd65536-search_quantile_i)*stage2_freq_out)>>16);
						end
					end
					
			STAGE4:begin
					stage2_search_temp  <=  1'b0;
					stage4_search_temp  <=  1'b1;
					end
		  
			TMINUS1:begin
					stage3_search_temp  <=  1'b0;
					stage4_search_temp  <=  1'b0;
					stage2_latency  <=  stage2_latency-1;
					
					end
		  
			STAGE3:begin
						stage4_search_temp  <=  1'b0;
						stage3_search_temp  <=  1'b1;
						if(stage3_search_end) begin
							number_counter  <=  number_counter+stage3_freq_out;
						end
					end
					
			OUTPUT:begin
						stage3_search_temp  <=  1'b0;
						w_latency_out  <=  (64'd1  <<  stage2_latency);
						whole_search_end  <=1'b1;
					end
					
			default: begin  
					
						end
		endcase
	end
	
	//------------Output Signals-----------------
	assign search_id_o  =  search_id_i;
	assign search_lantency_o  =  stage2_latency;
	
	
	one_cycle_signal	US2_one_cycle_signal(
		.clk(clk),
		.rst(rst),
		.start(stage2_search_temp),
		.one_cycle(stage2_search)
	);
	
	one_cycle_signal	US3_one_cycle_signal(
		.clk(clk),
		.rst(rst),
		.start(stage3_search_temp),
		.one_cycle(stage3_search)
	);
	
	one_cycle_signal	US4_one_cycle_signal(
		.clk(clk),
		.rst(rst),
		.start(stage4_search_temp),
		.one_cycle(stage4_search)
	);

endmodule
