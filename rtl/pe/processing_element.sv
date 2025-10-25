//=============================================================
// Author: Makari Green (xk4r1x)
// File: processing_element.sv
// Description: Core multiply-accumulate (MAC) Processing Element with internal accumulator
//=============================================================
module processing_element #(
        parameter DATA_WIDTH = 8,
        parameter ACC_WIDTH = 32
)(
        input logic clk,
        input logic rst_n,
        
        // Input data flow
        input logic [DATA_WIDTH-1:0] data_in,
        input logic [DATA_WIDTH-1:0] weight_in,
        input logic [ACC_WIDTH-1:0] partial_sum_in,
        
        // Control signals
        input logic valid_in,
        input logic accumulate_en,
        input logic clear_acc,
        input logic use_partial_sum,
        
        // Output data flow
        output logic [DATA_WIDTH-1:0] data_out,
        output logic [DATA_WIDTH-1:0] weight_out,
        output logic [ACC_WIDTH-1:0] partial_sum_out,
        output logic valid_out
);

        // Internal registers
        logic [ACC_WIDTH-1:0] accumulator;
        logic [DATA_WIDTH-1:0] data_reg;
        logic [DATA_WIDTH-1:0] weight_reg;
        logic [2*DATA_WIDTH-1:0] product;
        logic [ACC_WIDTH-1:0] sum;
        logic valid_reg;
        
        // Pipeline stage 1: Multiply
        always_ff @(posedge clk or negedge rst_n) begin
                if (!rst_n) begin
                        product <= '0;
                        data_reg <= '0;
                        weight_reg <= '0;
                        valid_reg <= '0;
                end else begin
                        if (valid_in) begin
                                product <= data_in * weight_in;
                                data_reg <= data_in;
                                weight_reg <= weight_in;
                        end
                        valid_reg <= valid_in;
                end
        end
        
        // Pipeline stage 2: Accumulation
        always_ff @(posedge clk or negedge rst_n) begin
                if (!rst_n) begin
                        accumulator <= '0;
                end else if (clear_acc) begin
                        accumulator <= '0;
                end else if (valid_reg) begin
                        if (accumulate_en) begin
                                accumulator <= accumulator + ACC_WIDTH'(product);
                        end else begin
                                accumulator <= ACC_WIDTH'(product);
                        end
                end
        end
        
        // Output computation
        always_comb begin
                if (use_partial_sum) begin
                        sum = partial_sum_in + accumulator;
                end else begin
                        sum = accumulator;
                end
        end
        
        // Pipeline stage 3: Output registers
        always_ff @(posedge clk or negedge rst_n) begin
                if (!rst_n) begin
                        partial_sum_out <= '0;
                        data_out <= '0;
                        weight_out <= '0;
                        valid_out <= '0;
                end else begin
                        partial_sum_out <= sum;
                        data_out <= data_reg;
                        weight_out <= weight_reg;
                        valid_out <= valid_reg;
                end
        end

endmodule
