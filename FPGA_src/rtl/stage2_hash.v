`timescale 1ns / 1ps

module stage2_hash #(
    parameter               	HW  =   10,     //hash width
										DW  =   64      //data width
    )(
	 //system signals
	 input							clk,
	 input 							rst,
	 
	 //insert
    input                   	insert_i,
    input   [DW-1:0]        	insert_data_i,
	 input	[DW-1:0]				insert_latency,	//latency
    output                  	insert_end_o,
	 
	 //search
    input                   	search_a_i,
    input   [DW-1:0]        	search_a_data_i,
	 output  [DW-1:0]          search_a_freq,
	 output  [DW-1:0]          search_a_latency,
    output                  	search_a_end_o,
	 
	 //RAM A interface
    //side A	read
    output                  	rama_wea_o,
    output  [HW-1:0]        	rama_addra_o,
    output  [DW-1:0] 			rama_dina_o,
    input   [DW-1:0] 			rama_douta_i,

    //side B	write
    output                  	rama_web_o,
    output  [HW-1:0]        	rama_addrb_o,
    output  [DW-1:0] 			rama_dinb_o,
    input   [DW-1:0] 			rama_doutb_i,

    //RAM B interface
    //side A	read
    output                  	ramb_wea_o,
    output  [HW-1:0]        	ramb_addra_o,
    output  [DW-1:0] 			ramb_dina_o,
    input   [DW-1:0] 			ramb_douta_i,

    //side B	write
    output                  	ramb_web_o,
    output  [HW-1:0]        	ramb_addrb_o,
    output  [DW-1:0] 			ramb_dinb_o,
    input   [DW-1:0] 			ramb_doutb_i,
	 
	 //RAM C interface
    //side A	read
    output                  	ramc_wea_o,
    output  [HW-1:0]        	ramc_addra_o,
    output  [DW-1:0] 			ramc_dina_o,
    input   [DW-1:0] 			ramc_douta_i,

    //side B	write
    output                  	ramc_web_o,
    output  [HW-1:0]        	ramc_addrb_o,
    output  [DW-1:0] 			ramc_dinb_o,
    input   [DW-1:0] 			ramc_doutb_i,

    //RAM D interface
    //side A	read
    output                  	ramd_wea_o,
    output  [HW-1:0]        	ramd_addra_o,
    output  [DW-1:0] 			ramd_dina_o,
    input   [DW-1:0] 			ramd_douta_i,

    //side B	write
    output                  	ramd_web_o,
    output  [HW-1:0]        	ramd_addrb_o,
    output  [DW-1:0] 			ramd_dinb_o,
    input   [DW-1:0] 			ramd_doutb_i
	 
    );
	 
	 //------------Declare Signals----------------
	 //register
    reg								insert_r;
    reg								insert_rr;

    reg								search_a_r;
    reg								search_a_rr;
	 
	 reg		[DW-1:0]				insert_latency_r;
	 reg		[DW-1:0]				insert_latency_rr;
	 
    //hash calculate
    wire								hash_a_valid;
    wire    [DW-1:0]				hash_a_data;
    reg     [DW-1:0]				hash_a_data_r;
    reg     [DW-1:0]				hash_a_data_rr;

    wire								hasha_a_result_valid;
    wire    [HW-1:0]				hasha_a_result;
    reg     [HW-1:0]				hasha_a_result_r;
    wire								hashb_a_result_valid;
    wire    [HW-1:0]				hashb_a_result;
    reg     [HW-1:0]				hashb_a_result_r;

    //search hash table
    wire    [HW-1:0]				rama_rd_addra;
    wire    [HW-1:0]				ramb_rd_addra;
	 wire    [HW-1:0]				ramc_rd_addra;
    wire    [HW-1:0]				ramd_rd_addra;
	 
	 wire		[DW-1:0]				rama_counter_a;
	 wire		[DW-1:0]				ramb_counter_a;
	 wire		[DW-1:0]				ramc_latency_a;
	 wire		[DW-1:0]				ramd_latency_a;

    //compare and insert
	 wire		[DW-1:0]				counter_min;
	 wire		[DW-1:0]				latency_min;
	 

    wire                    	rama_wr;
    wire    [HW-1:0]        	rama_wr_addr;
    wire    [DW-1:0] 			rama_wr_data_insert;
    wire    [DW-1:0] 			rama_wr_data;

    wire                    	ramb_wr;
    wire    [HW-1:0]        	ramb_wr_addr;
    wire    [DW-1:0] 			ramb_wr_data_insert;
    wire    [DW-1:0] 			ramb_wr_data;
	 
	 wire                    	ramc_wr;
    wire    [HW-1:0]        	ramc_wr_addr;
    wire    [DW-1:0] 			ramc_wr_data_insert;
    wire    [DW-1:0] 			ramc_wr_data;
	 
	 wire                    	ramd_wr;
    wire    [HW-1:0]        	ramd_wr_addr;
    wire    [DW-1:0] 			ramd_wr_data_insert;
    wire    [DW-1:0] 			ramd_wr_data;

    //output
    reg                     	insert_end;
 
	 //---------------Processing------------------
	 //insert/search control signals
    always @(posedge clk or negedge rst) begin
        if (!rst) begin
            insert_r        <=  1'b0;
            insert_rr       <=  1'b0;
            search_a_r      <=  1'b0;
            search_a_rr     <=  1'b0;
				insert_latency_r<=  {DW{1'b0}};
				insert_latency_rr<= {DW{1'b0}};
        end
        else begin
            insert_r        <=  insert_i;
            insert_rr       <=  insert_r;
            search_a_r      <=  search_a_i;
            search_a_rr     <=  search_a_r;
				insert_latency_r<=  insert_latency;
				insert_latency_rr<= insert_latency_r;
        end
    end

    //step 1: hash calculate
    //for insert/search
    assign  hash_a_valid    =   insert_i | search_a_i;							//enable hash calculation
    assign  hash_a_data     =   insert_i?insert_data_i:							//hash parameter
                                search_a_i?search_a_data_i:{DW{1'h0}};	

    always @(posedge clk or negedge rst) begin										//storing hash parameter
        if (!rst) begin
            hash_a_data_r   <=  {DW{1'b0}};
            hash_a_data_rr  <=  {DW{1'b0}};
        end
        else begin
            hash_a_data_r   <=  hash_a_data;
            hash_a_data_rr  <=  hash_a_data_r;
        end
    end

    CRC32_D32 #(																				//CRC calculation hash1
        .HW                 (HW),
        .HIGH               (0)
    ) U0_CRC32_D32(
        .clk                (clk),
        .rst_n              (rst),

        .calc_data_i        (hash_a_data),
        .calc_en_i          (hash_a_valid),

        .crc32_o            (hasha_a_result),
        .crc32_valid_o      (hasha_a_result_valid)
    );

    CRC32_D32 #(																				//CRC calculation hash2
        .HW                 (HW),
        .HIGH               (1)
    ) U1_CRC32_D32(
        .clk                (clk),
        .rst_n              (rst),

        .calc_data_i        (hash_a_data),
        .calc_en_i          (hash_a_valid),

        .crc32_o            (hashb_a_result),
        .crc32_valid_o      (hashb_a_result_valid)
    );

    always @ (posedge clk or negedge rst) begin										//output hash1/hash2 results
        if (!rst) begin
            hasha_a_result_r    <=  {HW{1'b0}};
            hashb_a_result_r    <=  {HW{1'b0}};
        end
        else begin
            hasha_a_result_r    <=  hasha_a_result;
            hashb_a_result_r    <=  hashb_a_result;
        end
    end
	 
	 //step 2: search hash table
	 assign  rama_rd_addra       		=   	hasha_a_result;
	 assign  ramc_rd_addra       		=   	hasha_a_result;
    assign  ramb_rd_addra      		=   	hashb_a_result;
	 assign  ramd_rd_addra      		=   	hashb_a_result;
	 
	 assign  rama_counter_a				=		rama_douta_i[DW-1:0];
	 assign  ramb_counter_a				=  	ramb_douta_i[DW-1:0];
	 assign  ramc_latency_a				=		ramc_douta_i[DW-1:0];
	 assign  ramd_latency_a				=  	ramd_douta_i[DW-1:0];

	 //step 3: compare and insert
	 assign  counter_min					=		rama_counter_a < ramb_counter_a?rama_counter_a:ramb_counter_a;		//	count-min
	 assign  latency_min					=		ramc_latency_a < ramd_latency_a?ramc_latency_a:ramd_latency_a;		//	count-min latency
	
	 assign  rama_wr						=	   insert_rr;
    assign  rama_wr_addr				=  	hasha_a_result_r;
	 assign  ramc_wr						=	   insert_rr;
    assign  ramc_wr_addr				=  	hasha_a_result_r;

    assign  ramb_wr         			=  	insert_rr;
    assign  ramb_wr_addr    			=   	hashb_a_result_r;
	 assign  ramd_wr         			=  	insert_rr;
    assign  ramd_wr_addr    			=   	hashb_a_result_r;
	 
	 assign	rama_wr_data_insert  	=  	rama_counter_a+1;							//plus 1
	 assign	ramb_wr_data_insert  	=  	ramb_counter_a+1;
	 
	 assign	ramc_wr_data_insert  	=  	ramc_latency_a > insert_latency_rr?ramc_latency_a:insert_latency_rr;							//max
	 assign	ramd_wr_data_insert  	=  	ramd_latency_a > insert_latency_rr?ramd_latency_a:insert_latency_rr;	
	 
	 assign  rama_wr_data    			=   	rama_wr_data_insert;
    assign  ramb_wr_data    			=   	ramb_wr_data_insert;
	 assign  ramc_wr_data    			=   	ramc_wr_data_insert;
    assign  ramd_wr_data    			=   	ramd_wr_data_insert;
	 
	 //------------Output Signals-----------------
    //insert
    always @(posedge clk or negedge rst) begin
        if (!rst) begin
            insert_end          <=  1'b0;
        end
        else begin
            insert_end          <=  insert_rr;
        end
    end

    assign  insert_end_o			=   insert_end;

    //search
	 assign  search_a_freq			=   counter_min;
	 assign  search_a_latency		=   latency_min;
    assign  search_a_end_o			=   search_a_rr;

    //RAM A
    assign  rama_wea_o				=   1'b0;
    assign  rama_addra_o			=   rama_rd_addra;
    assign  rama_dina_o				=   {DW{1'b0}};

    assign  rama_web_o          	=   rama_wr;
    assign  rama_addrb_o       	=   rama_wr?rama_wr_addr:{HW{1'b0}};
    assign  rama_dinb_o        	=   rama_wr?rama_wr_data:{DW{1'b0}};

    //RAM B
    assign  ramb_wea_o          	=   1'b0;
    assign  ramb_addra_o        	=   ramb_rd_addra;
    assign  ramb_dina_o         	=   {DW{1'b0}};

    assign  ramb_web_o          	=   ramb_wr;
    assign  ramb_addrb_o        	=   ramb_wr?ramb_wr_addr:{HW{1'b0}};
    assign  ramb_dinb_o         	=   ramb_wr?ramb_wr_data:{DW{1'b0}};
	 
	 //RAM C
    assign  ramc_wea_o				=   1'b0;
    assign  ramc_addra_o			=   ramc_rd_addra;
    assign  ramc_dina_o				=   {DW{1'b0}};

    assign  ramc_web_o          	=   ramc_wr;
    assign  ramc_addrb_o       	=   ramc_wr?ramc_wr_addr:{HW{1'b0}};
    assign  ramc_dinb_o        	=   ramc_wr?ramc_wr_data:{DW{1'b0}};

    //RAM D
    assign  ramd_wea_o          	=   1'b0;
    assign  ramd_addra_o        	=   ramd_rd_addra;
    assign  ramd_dina_o         	=   {DW{1'b0}};

    assign  ramd_web_o          	=   ramd_wr;
    assign  ramd_addrb_o        	=   ramd_wr?ramd_wr_addr:{HW{1'b0}};
    assign  ramd_dinb_o         	=   ramd_wr?ramd_wr_data:{DW{1'b0}};
endmodule