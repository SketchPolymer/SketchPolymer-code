`timescale 1ns/1ps



module distri_sketch_top_tb();
	
		reg clk;
		reg [63:0] insert_freq_th_i;
		reg insert_i;
		reg [63:0] insert_id_i;
		reg [63:0] insert_latency_i;
		reg rst;
		reg search_i;
		reg [63:0] search_id_i;
		reg [63:0] search_quantile_i;
		
		reg [7:0] id_number;
		// wires                                               
		wire insert_end_o;
		wire search_end_o;
		wire [63:0]  search_latency_o;

		distri_sketch_top i1 (
		// port map - connection between master ports and signals/registers   
			.clk(clk),
			.rst(rst),
			.insert_i(insert_i),
			.insert_id_i(insert_id_i),
			.insert_latency_i(insert_latency_i),
			.insert_freq_th_i(insert_freq_th_i),
			.insert_end_o(insert_end_o),
			
			.search_i(search_i),
			.search_id_i(search_id_i),
			.search_quantile_i(search_quantile_i),
			.search_latency_o(search_latency_o),
			.search_end_o(search_end_o)
		);
 
	initial begin
		clk=0;
		rst=0;
		insert_i=0;
		search_i=0;
		
		#200	rst=1;
				insert_i=1;	
				insert_freq_th_i=64'd50;
				
				
				//50 ID=10,LA=10
				for (id_number = 0; id_number < 50; id_number = id_number + 1)
				begin
					#200 	insert_id_i=64'h10;
							insert_latency_i=64'h10;
				end
				
		#200	for (id_number = 0; id_number < 50; id_number = id_number + 1)
				begin
					#2100 	insert_id_i=64'h10;
								insert_latency_i=64'h10;
				end 
				
				//40 ID=10,LA=20
				for (id_number = 0; id_number < 50; id_number = id_number + 1)
				begin
					#200 	insert_id_i=64'h10;
							insert_latency_i=64'h20;
				end
				
		#200	for (id_number = 0; id_number < 40; id_number = id_number + 1)
				begin
					#2100 	insert_id_i=64'h10;
							insert_latency_i=64'h20;
				end 
				
				//5 ID=10,LA=30
				for (id_number = 0; id_number < 50; id_number = id_number + 1)
				begin
					#200 	insert_id_i=64'h10;
							insert_latency_i=64'h30;
				end
				
		#200	for (id_number = 0; id_number < 5; id_number = id_number + 1)
				begin
					#2100 	insert_id_i=64'h10;
							insert_latency_i=64'h30;
				end
				
				//3 ID=10,LA=40
				for (id_number = 0; id_number < 50; id_number = id_number + 1)
				begin
					#200 	insert_id_i=64'h10;
							insert_latency_i=64'h40;
				end
				
		#200	for (id_number = 0; id_number < 3; id_number = id_number + 1)
				begin
					#2100 	insert_id_i=64'h10;
							insert_latency_i=64'h40;
				end 
				
				//2 ID=10,LA=50
				for (id_number = 0; id_number < 50; id_number = id_number + 1)
				begin
					#200 	insert_id_i=64'h10;
							insert_latency_i=64'h50;
				end
				
		#200	for (id_number = 0; id_number < 2; id_number = id_number + 1)
				begin
					#2100 	insert_id_i=64'h10;
							insert_latency_i=64'h50;
				end 
		
      
		
		#1200	insert_i=0;	
				insert_id_i=64'h0;
				insert_latency_i=64'h0;
				search_i=1;
				search_id_i=64'h10;
				search_quantile_i=64'd52429;	//0.95*65536
				

				
				
		#1000000 $stop;
	end

	always #50 clk=~clk;
	

endmodule
