`timescale 1ns / 1ps

module distri_sketch_top#(
    parameter              	HW  =   10,     		//hash width
										DW  =   64      		//data width
    )(
	//system signals
	input                		clk,
	input								rst,
	
	//insert
	input                		insert_i,				//start insert
	input   		[DW-1:0]    	insert_id_i,			//id
	input   		[DW-1:0]    	insert_latency_i,		//latency
	input   		[DW-1:0]    	insert_freq_th_i,		//frequency threshold
	output                  	insert_end_o,			//insert finish flag
	
	//search
	input                		search_i,				//start search
	input   		[DW-1:0]    	search_id_i,			//id
	input   		[DW-1:0]    	search_quantile_i,	//quantile
	output		[DW-1:0]   	 	search_latency_o,		//latency with gived quantile
	output                  	search_end_o			//search finish flag
);

	//------------Declare Signals----------------
	//stage1 RAM
	//RAM A
    wire                   	rama_wea_s1;
    wire    	[HW-1:0]       rama_addra_s1;
    wire    	[DW-1:0] 		rama_dina_s1;
    wire    	[DW-1:0] 		rama_douta_s1;

    wire                    	rama_web_s1;
    wire    	[HW-1:0]       rama_addrb_s1;
    wire    	[DW-1:0] 		rama_dinb_s1;
    wire    	[DW-1:0] 		rama_doutb_s1;

    //RAM B
    wire                    	ramb_wea_s1;
    wire    	[HW-1:0]       ramb_addra_s1;
    wire    	[DW-1:0] 		ramb_dina_s1;
    wire    	[DW-1:0] 		ramb_douta_s1;

    wire                    	ramb_web_s1;
    wire    	[HW-1:0]       ramb_addrb_s1;
    wire    	[DW-1:0] 		ramb_dinb_s1;
    wire    	[DW-1:0] 		ramb_doutb_s1;
	 
	 //stage2 RAM
	 //RAM A
    wire                   	rama_wea_s2;
    wire    	[HW-1:0]       rama_addra_s2;
    wire    	[DW-1:0] 		rama_dina_s2;
    wire    	[DW-1:0] 		rama_douta_s2;

    wire                    	rama_web_s2;
    wire    	[HW-1:0]       rama_addrb_s2;
    wire    	[DW-1:0] 		rama_dinb_s2;
    wire    	[DW-1:0] 		rama_doutb_s2;

    //RAM B
    wire                    	ramb_wea_s2;
    wire    	[HW-1:0]       ramb_addra_s2;
    wire    	[DW-1:0] 		ramb_dina_s2;
    wire    	[DW-1:0] 		ramb_douta_s2;

    wire                    	ramb_web_s2;
    wire    	[HW-1:0]       ramb_addrb_s2;
    wire    	[DW-1:0] 		ramb_dinb_s2;
    wire    	[DW-1:0] 		ramb_doutb_s2;
	 
	 //RAM C
    wire                  		ramc_wea_s2;
    wire  		[HW-1:0]       ramc_addra_s2;
    wire  		[DW-1:0] 		ramc_dina_s2;
    wire   		[DW-1:0] 		ramc_douta_s2;

    wire                  		ramc_web_s2;
    wire  		[HW-1:0]       ramc_addrb_s2;
    wire  		[DW-1:0] 		ramc_dinb_s2;
    wire   		[DW-1:0] 		ramc_doutb_s2;

    //RAM D
    wire                  		ramd_wea_s2;
    wire  		[HW-1:0]       ramd_addra_s2;
    wire  		[DW-1:0] 		ramd_dina_s2;
    wire   		[DW-1:0] 		ramd_douta_s2;

    wire                  		ramd_web_s2;
    wire  		[HW-1:0]       ramd_addrb_s2;
    wire  		[DW-1:0] 		ramd_dinb_s2;
    wire   		[DW-1:0] 		ramd_doutb_s2;
	 
	 //stage3 RAM
	 //RAM A
    wire                   	rama_wea_s3;
    wire    	[HW-1:0]       rama_addra_s3;
    wire    	[DW-1:0] 		rama_dina_s3;
    wire    	[DW-1:0] 		rama_douta_s3;

    wire                    	rama_web_s3;
    wire    	[HW-1:0]       rama_addrb_s3;
    wire    	[DW-1:0] 		rama_dinb_s3;
    wire    	[DW-1:0] 		rama_doutb_s3;

    //RAM B
    wire                    	ramb_wea_s3;
    wire    	[HW-1:0]       ramb_addra_s3;
    wire    	[DW-1:0] 		ramb_dina_s3;
    wire    	[DW-1:0] 		ramb_douta_s3;

    wire                    	ramb_web_s3;
    wire    	[HW-1:0]       ramb_addrb_s3;
    wire    	[DW-1:0] 		ramb_dinb_s3;
    wire    	[DW-1:0] 		ramb_doutb_s3;
	 
	 //stage4 RAM
	 //RAM A
    wire                   	rama_wea_s4;
    wire    	[HW-1:0]       rama_addra_s4;
    wire    	[DW-1:0] 		rama_dina_s4;
    wire    	[DW-1:0] 		rama_douta_s4;

    wire                    	rama_web_s4;
    wire    	[HW-1:0]       rama_addrb_s4;
    wire    	[DW-1:0] 		rama_dinb_s4;
    wire    	[DW-1:0] 		rama_doutb_s4;

    //RAM B
    wire                    	ramb_wea_s4;
    wire    	[HW-1:0]       ramb_addra_s4;
    wire    	[DW-1:0] 		ramb_dina_s4;
    wire    	[DW-1:0] 		ramb_douta_s4;

    wire                    	ramb_web_s4;
    wire    	[HW-1:0]       ramb_addrb_s4;
    wire    	[DW-1:0] 		ramb_dinb_s4;
    wire    	[DW-1:0] 		ramb_doutb_s4;
	 
	 //interconnected wire
	 wire			[DW-1:0]			search_id;
	 wire			[DW-1:0]			search_latency;
	 
	 wire			[DW-1:0]			stage2_freq_out;
	 wire			[DW-1:0]			stage2_latency_out;
	 wire			[DW-1:0]			stage3_freq_out;
	 wire			[DW-1:0]			stage4_freq_out;

	 wire								stage2_search_end;
	 wire								stage3_search_end;
	 wire								stage4_search_end;
	 
	 wire								stage2_search;				//search start control
	 wire								stage3_search;
	 wire								stage4_search;
	 
	 wire			[DW-1:0]			insert_id;
	 wire			[DW-1:0]			insert_latency;
	 wire			[DW-1:0]			insert_freq_th;
	 
	 wire								stage1_insert_end;
	 wire								stage2_insert_end;
	 wire								stage3_insert_end;
	 wire								stage4_insert_end;
	 
	 wire								stage1_insert;				//insert start control
	 wire								stage2_insert;
	 wire								stage3_insert;
	 wire								stage4_insert;
	 
	 
	 //Insert control module
	 insert_control#(
			.HW                 (HW),   
			.DW                 (DW)
    ) U_insert_control(
	 //system signals
	.clk(clk),
	.rst(rst),
	
	 //external interface
	.insert_control_en_i(insert_i),
	.insert_id_i(insert_id_i),
	.insert_latency_i(insert_latency_i),
	.insert_freq_th_i(insert_freq_th_i),

	.whole_insert_end(insert_end_o),
	.insert_id_i_o(insert_id),
	.insert_latency_o(insert_latency),
	.insert_freq_th_o(insert_freq_th),
	
	.stage1_insert_end(stage1_insert_end),
	.stage2_insert_end(stage2_insert_end),
	.stage3_insert_end(stage3_insert_end),
	.stage4_insert_end(stage4_insert_end),
	
	.stage1_insert(stage1_insert),
	.stage2_insert(stage2_insert),
	.stage3_insert(stage3_insert),
	.stage4_insert(stage4_insert)
	 
	 );
	 
	 //Search control module
	 search_control#(
			.HW                 (HW),   
			.DW                 (DW)
    ) U_search_control(
	 //system signals
	.clk(clk),
	.rst(rst),
	
	 //external interface
	.search_control_en_i(search_i),
	.search_id_i(search_id_i),
	.search_quantile_i(search_quantile_i),
	
	.w_latency_out(search_latency_o),
	.whole_search_end(search_end_o),
	
	.search_id_o(search_id),
	.search_lantency_o(search_latency),
	
	 //internal interface
	.stage2_freq_out(stage2_freq_out),
	.stage2_latency_out(stage2_latency_out),
	
	.stage3_freq_out(stage3_freq_out),
	.stage4_freq_out(stage4_freq_out),
	
	.stage2_search_end(stage2_search_end),
	.stage3_search_end(stage3_search_end),
	.stage4_search_end(stage4_search_end),
	
	.stage2_search(stage2_search),
	.stage3_search(stage3_search),
	.stage4_search(stage4_search)
	 
	 );
	 
	 //Stage1 insert or search
	 stage1_hash #(
			.HW                 (HW),   
			.DW                 (DW)
    ) U_stage1_hash(
			//system signals
			.clk(clk),
			.rst(rst),

			//insert
			.insert_i(stage1_insert),
			.insert_data_i(insert_id),
			.freq_threshold(insert_freq_th),	//frequency threshold
			.insert_end_o(stage1_insert_end),

			//search
			.search_a_i(),
			.search_a_data_i(),
			.search_a_freq(),
			.search_a_end_o(),

			//RAM A interface
			//side A	read
			.rama_wea_o(rama_wea_s1),
			.rama_addra_o(rama_addra_s1),
			.rama_dina_o(rama_dina_s1),
			.rama_douta_i(rama_douta_s1),

			//side B	write
			.rama_web_o(rama_web_s1),
			.rama_addrb_o(rama_addrb_s1),
			.rama_dinb_o(rama_dinb_s1),
			.rama_doutb_i(rama_doutb_s1),

			//RAM B interface
			//side A	read
			.ramb_wea_o(ramb_wea_s1),
			.ramb_addra_o(ramb_addra_s1),
			.ramb_dina_o(ramb_dina_s1),
			.ramb_douta_i(ramb_douta_s1),

			//side B	write
			.ramb_web_o(ramb_web_s1),
			.ramb_addrb_o(ramb_addrb_s1),
			.ramb_dinb_o(ramb_dinb_s1),
			.ramb_doutb_i(ramb_doutb_s1)
    );
	 
	 //Stage2 insert or search
	 stage2_hash #(
			.HW                 (HW),   
			.DW                 (DW)
    ) U_stage2_hash(
			//system signals
			.clk(clk),
			.rst(rst),

			//insert
			.insert_i(stage2_insert),
			.insert_data_i(insert_id),
			.insert_latency(insert_latency),	//frequency threshold
			.insert_end_o(stage2_insert_end),

			//search
			.search_a_i(stage2_search),
			.search_a_data_i(search_id),
			.search_a_freq(stage2_freq_out),
			.search_a_latency(stage2_latency_out),
			.search_a_end_o(stage2_search_end),

			//RAM A interface
			//side A	read
			.rama_wea_o(rama_wea_s2),
			.rama_addra_o(rama_addra_s2),
			.rama_dina_o(rama_dina_s2),
			.rama_douta_i(rama_douta_s2),

			//side B	write
			.rama_web_o(rama_web_s2),
			.rama_addrb_o(rama_addrb_s2),
			.rama_dinb_o(rama_dinb_s2),
			.rama_doutb_i(rama_doutb_s2),

			//RAM B interface
			//side A	read
			.ramb_wea_o(ramb_wea_s2),
			.ramb_addra_o(ramb_addra_s2),
			.ramb_dina_o(ramb_dina_s2),
			.ramb_douta_i(ramb_douta_s2),

			//side B	write
			.ramb_web_o(ramb_web_s2),
			.ramb_addrb_o(ramb_addrb_s2),
			.ramb_dinb_o(ramb_dinb_s2),
			.ramb_doutb_i(ramb_doutb_s2),
			
			//RAM C interface
			//side A	read
			.ramc_wea_o(ramc_wea_s2),
			.ramc_addra_o(ramc_addra_s2),
			.ramc_dina_o(ramc_dina_s2),
			.ramc_douta_i(ramc_douta_s2),

			//side B	write
			.ramc_web_o(ramc_web_s2),
			.ramc_addrb_o(ramc_addrb_s2),
			.ramc_dinb_o(ramc_dinb_s2),
			.ramc_doutb_i(ramc_doutb_s2),

			//RAM D interface
			//side A	read
			.ramd_wea_o(ramd_wea_s2),
			.ramd_addra_o(ramd_addra_s2),
			.ramd_dina_o(ramd_dina_s2),
			.ramd_douta_i(ramd_douta_s2),

			//side B	write
			.ramd_web_o(ramd_web_s2),
			.ramd_addrb_o(ramd_addrb_s2),
			.ramd_dinb_o(ramd_dinb_s2),
			.ramd_doutb_i(ramd_doutb_s2)
    );
	 
	 //Stage3 insert or search
	 stage3_hash #(
			.HW                 (HW),   
			.DW                 (DW)
    ) U_stage3_hash(
			//system signals
			.clk(clk),
			.rst(rst),

			//insert
			.insert_i(stage3_insert),
			.insert_data_i(insert_id),
			.insert_latency_i(insert_latency),	
			.insert_end_o(stage3_insert_end),

			//search
			.search_a_i(stage3_search),
			.search_a_data_i(search_id),
			.search_a_latency_i(search_latency),
			.search_a_freq_o(stage3_freq_out),
			.search_a_end_o(stage3_search_end),

			//RAM A interface
			//side A	read
			.rama_wea_o(rama_wea_s3),
			.rama_addra_o(rama_addra_s3),
			.rama_dina_o(rama_dina_s3),
			.rama_douta_i(rama_douta_s3),

			//side B	write
			.rama_web_o(rama_web_s3),
			.rama_addrb_o(rama_addrb_s3),
			.rama_dinb_o(rama_dinb_s3),
			.rama_doutb_i(rama_doutb_s3),

			//RAM B interface
			//side A	read
			.ramb_wea_o(ramb_wea_s3),
			.ramb_addra_o(ramb_addra_s3),
			.ramb_dina_o(ramb_dina_s3),
			.ramb_douta_i(ramb_douta_s3),

			//side B	write
			.ramb_web_o(ramb_web_s3),
			.ramb_addrb_o(ramb_addrb_s3),
			.ramb_dinb_o(ramb_dinb_s3),
			.ramb_doutb_i(ramb_doutb_s3)
    );
	 
	 //Stage4 insert or search
	 stage4_hash #(
			.HW                 (HW),   
			.DW                 (DW)
    ) U_stage4_hash(
			//system signals
			.clk(clk),
			.rst(rst),

			//insert
			.insert_i(stage4_insert),
			.insert_data_i(insert_id),
			.insert_latency_i(insert_latency),	
			.insert_end_o(stage4_insert_end),

			//search
			.search_a_i(stage4_search),
			.search_a_data_i(search_id),
			.search_a_latency_i(search_latency),
			.search_a_freq_o(stage4_freq_out),
			.search_a_end_o(stage4_search_end),

			//RAM A interface
			//side A	read
			.rama_wea_o(rama_wea_s4),
			.rama_addra_o(rama_addra_s4),
			.rama_dina_o(rama_dina_s4),
			.rama_douta_i(rama_douta_s4),

			//side B	write
			.rama_web_o(rama_web_s4),
			.rama_addrb_o(rama_addrb_s4),
			.rama_dinb_o(rama_dinb_s4),
			.rama_doutb_i(rama_doutb_s4),

			//RAM B interface
			//side A	read
			.ramb_wea_o(ramb_wea_s4),
			.ramb_addra_o(ramb_addra_s4),
			.ramb_dina_o(ramb_dina_s4),
			.ramb_douta_i(ramb_douta_s4),

			//side B	write
			.ramb_web_o(ramb_web_s4),
			.ramb_addrb_o(ramb_addrb_s4),
			.ramb_dinb_o(ramb_dinb_s4),
			.ramb_doutb_i(ramb_doutb_s4)
    );
	 
	 //stage1 RAM
	 distri_sketch_ram_top_2RAM #(
        .DW                 (DW),   //data width
        .HW                 (HW)    //hash width
    ) U1_distri_sketch_ram_top_2RAM(
        //system signals
        .clk                (clk),
        .rst                (rst),

        //RAM A
        .rama_wea_i         (rama_wea_s1),
        .rama_addra_i       (rama_addra_s1),
        .rama_dina_i        (rama_dina_s1),
        .rama_douta_o       (rama_douta_s1),

        .rama_web_i         (rama_web_s1),
        .rama_addrb_i       (rama_addrb_s1),
        .rama_dinb_i        (rama_dinb_s1),
        .rama_doutb_o       (rama_doutb_s1),

        //RAM B
        .ramb_wea_i         (ramb_wea_s1),
        .ramb_addra_i       (ramb_addra_s1),
        .ramb_dina_i        (ramb_dina_s1),
        .ramb_douta_o       (ramb_douta_s1),

        .ramb_web_i         (ramb_web_s1),
        .ramb_addrb_i       (ramb_addrb_s1),
        .ramb_dinb_i        (ramb_dinb_s1),
        .ramb_doutb_o       (ramb_doutb_s1)
    );
	 
	 //stage2 RAM
	 distri_sketch_ram_top_4RAM #(
        .DW                 (DW),   //data width
        .HW                 (HW)    //hash width
    ) U_distri_sketch_ram_top_4RAM(
        //system signals
        .clk                (clk),
        .rst                (rst),

        //RAM A
        .rama_wea_i         (rama_wea_s2),
        .rama_addra_i       (rama_addra_s2),
        .rama_dina_i        (rama_dina_s2),
        .rama_douta_o       (rama_douta_s2),

        .rama_web_i         (rama_web_s2),
        .rama_addrb_i       (rama_addrb_s2),
        .rama_dinb_i        (rama_dinb_s2),
        .rama_doutb_o       (rama_doutb_s2),

        //RAM B
        .ramb_wea_i         (ramb_wea_s2),
        .ramb_addra_i       (ramb_addra_s2),
        .ramb_dina_i        (ramb_dina_s2),
        .ramb_douta_o       (ramb_douta_s2),

        .ramb_web_i         (ramb_web_s2),
        .ramb_addrb_i       (ramb_addrb_s2),
        .ramb_dinb_i        (ramb_dinb_s2),
        .ramb_doutb_o       (ramb_doutb_s2),
		  
		  //RAM C
        .ramc_wea_i         (ramc_wea_s2),
        .ramc_addra_i       (ramc_addra_s2),
        .ramc_dina_i        (ramc_dina_s2),
        .ramc_douta_o       (ramc_douta_s2),

        .ramc_web_i         (ramc_web_s2),
        .ramc_addrb_i       (ramc_addrb_s2),
        .ramc_dinb_i        (ramc_dinb_s2),
        .ramc_doutb_o       (ramc_doutb_s2),

        //RAM D
        .ramd_wea_i         (ramd_wea_s2),
        .ramd_addra_i       (ramd_addra_s2),
        .ramd_dina_i        (ramd_dina_s2),
        .ramd_douta_o       (ramd_douta_s2),

        .ramd_web_i         (ramd_web_s2),
        .ramd_addrb_i       (ramd_addrb_s2),
        .ramd_dinb_i        (ramd_dinb_s2),
        .ramd_doutb_o       (ramd_doutb_s2)
    );
	 
	 //stage3 RAM
	 distri_sketch_ram_top_2RAM #(
        .DW                 (DW),   //data width
        .HW                 (HW)    //hash width
    ) U2_distri_sketch_ram_top_2RAM(
        //system signals
        .clk                (clk),
        .rst                (rst),

        //RAM A
        .rama_wea_i         (rama_wea_s3),
        .rama_addra_i       (rama_addra_s3),
        .rama_dina_i        (rama_dina_s3),
        .rama_douta_o       (rama_douta_s3),

        .rama_web_i         (rama_web_s3),
        .rama_addrb_i       (rama_addrb_s3),
        .rama_dinb_i        (rama_dinb_s3),
        .rama_doutb_o       (rama_doutb_s3),

        //RAM B
        .ramb_wea_i         (ramb_wea_s3),
        .ramb_addra_i       (ramb_addra_s3),
        .ramb_dina_i        (ramb_dina_s3),
        .ramb_douta_o       (ramb_douta_s3),

        .ramb_web_i         (ramb_web_s3),
        .ramb_addrb_i       (ramb_addrb_s3),
        .ramb_dinb_i        (ramb_dinb_s3),
        .ramb_doutb_o       (ramb_doutb_s3)
    );
	 
	 //stage4 RAM
	 distri_sketch_ram_top_2RAM #(
        .DW                 (DW),   //data width
        .HW                 (HW)    //hash width
    ) U3_distri_sketch_ram_top_2RAM(
        //system signals
        .clk                (clk),
        .rst              	 (rst),

        //RAM A
        .rama_wea_i         (rama_wea_s4),
        .rama_addra_i       (rama_addra_s4),
        .rama_dina_i        (rama_dina_s4),
        .rama_douta_o       (rama_douta_s4),

        .rama_web_i         (rama_web_s4),
        .rama_addrb_i       (rama_addrb_s4),
        .rama_dinb_i        (rama_dinb_s4),
        .rama_doutb_o       (rama_doutb_s4),

        //RAM B
        .ramb_wea_i         (ramb_wea_s4),
        .ramb_addra_i       (ramb_addra_s4),
        .ramb_dina_i        (ramb_dina_s4),
        .ramb_douta_o       (ramb_douta_s4),

        .ramb_web_i         (ramb_web_s4),
        .ramb_addrb_i       (ramb_addrb_s4),
        .ramb_dinb_i        (ramb_dinb_s4),
        .ramb_doutb_o       (ramb_doutb_s4)
    );

	
endmodule	
