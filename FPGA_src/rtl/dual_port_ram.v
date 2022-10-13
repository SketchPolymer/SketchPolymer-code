`timescale 1ns / 1ps

(* dont_touch="true" *)

module dual_port_ram #(
    parameter               DPW     =   10,
                            DW      =   64
    )(
    //port a
    input                   clka,
    input                   wea,
    input       [DPW-1:0]   addra,
    input       [DW-1:0]    dina,
    output  reg [DW-1:0]    douta,

    //port b
    input                   clkb,
    input                   web,
    input       [DPW-1:0]   addrb,
    input       [DW-1:0]    dinb,
    output  reg [DW-1:0]    doutb
    );

    //mem declare
    reg     [DW-1:0]    ram_block   [2**DPW-1:0];

    //"initial" will be ignored at implementation
    integer i;
    initial begin
        for (i=0;i<2**DPW;i=i+1) begin
            ram_block[i]    =   {DW{1'b0}};
        end

        douta               =   {DW{1'b0}};
        doutb               =   {DW{1'b0}};
    end

    //port a
    always @ (posedge clka) begin
        if (wea) begin
            ram_block[addra]    <=  dina;
        end
        douta                   <=  ram_block[addra];
    end

    //port b
    always @ (posedge clkb) begin
        if (web) begin
            ram_block[addrb]    <=  dinb;
        end
        doutb                   <=  ram_block[addrb];
    end

endmodule
