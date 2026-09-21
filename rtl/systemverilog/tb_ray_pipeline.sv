`timescale 1ns/1ps

module tb_ray_pipeline;
    logic clk = 0;
    logic rst_n = 0;
    logic in_valid = 0;
    logic signed [15:0] ro_x, ro_y, ro_z;
    logic signed [15:0] rd_x, rd_y, rd_z;
    logic signed [15:0] center_x, center_y, center_z;
    logic [15:0] radius;
    logic out_valid, hit;
    logic signed [63:0] discriminant;
    integer vectors, status, expected, passed = 0, total = 0;
    integer i_ro_x, i_ro_y, i_ro_z, i_rd_x, i_rd_y, i_rd_z;
    integer i_cx, i_cy, i_cz, i_radius;
    string vector_path;

    always #5 clk = ~clk;

    ray_packet_pipeline dut (.*);

    task automatic run_vector;
        begin
            @(negedge clk);
            ro_x = i_ro_x; ro_y = i_ro_y; ro_z = i_ro_z;
            rd_x = i_rd_x; rd_y = i_rd_y; rd_z = i_rd_z;
            center_x = i_cx; center_y = i_cy; center_z = i_cz;
            radius = i_radius;
            in_valid = 1;
            @(negedge clk);
            in_valid = 0;
            wait(out_valid === 1'b1);
            #1;
            total = total + 1;
            if (hit === expected[0]) passed = passed + 1;
            else $error("vector %0d: expected hit=%0d, got %0d (disc=%0d)",
                        total, expected, hit, discriminant);
        end
    endtask

    initial begin
        if (!$value$plusargs("VECTORS=%s", vector_path))
            vector_path = "rtl/test_vectors.txt";
        vectors = $fopen(vector_path, "r");
        if (!vectors) $fatal(1, "cannot open %s", vector_path);

        repeat (3) @(negedge clk);
        rst_n = 1;
        while (!$feof(vectors)) begin
            status = $fscanf(vectors, "%d %d %d %d %d %d %d %d %d %d %d\n",
                i_ro_x, i_ro_y, i_ro_z, i_rd_x, i_rd_y, i_rd_z,
                i_cx, i_cy, i_cz, i_radius, expected);
            if (status == 11) run_vector();
        end
        $fclose(vectors);
        if (passed != total) $fatal(1, "%0d/%0d RTL vectors passed", passed, total);
        $display("PASS: %0d/%0d SystemVerilog vectors", passed, total);
        $finish;
    end
endmodule
