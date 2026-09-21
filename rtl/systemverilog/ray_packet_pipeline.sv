`timescale 1ns/1ps

module ray_packet_pipeline (
    input  logic clk,
    input  logic rst_n,
    input  logic in_valid,
    input  logic signed [15:0] ro_x, ro_y, ro_z,
    input  logic signed [15:0] rd_x, rd_y, rd_z,
    input  logic signed [15:0] center_x, center_y, center_z,
    input  logic        [15:0] radius,
    output logic out_valid,
    output logic hit,
    output logic signed [63:0] discriminant
);
    logic valid_q;
    logic signed [15:0] ro_x_q, ro_y_q, ro_z_q;
    logic signed [15:0] rd_x_q, rd_y_q, rd_z_q;
    logic signed [15:0] center_x_q, center_y_q, center_z_q;
    logic        [15:0] radius_q;
    logic core_hit;
    logic signed [63:0] core_discriminant;

    ray_sphere_discriminant core (
        .ro_x(ro_x_q), .ro_y(ro_y_q), .ro_z(ro_z_q),
        .rd_x(rd_x_q), .rd_y(rd_y_q), .rd_z(rd_z_q),
        .center_x(center_x_q), .center_y(center_y_q), .center_z(center_z_q),
        .radius(radius_q), .hit(core_hit), .discriminant(core_discriminant)
    );

    always_ff @(posedge clk or negedge rst_n) begin
        if (!rst_n) begin
            valid_q <= 1'b0;
            out_valid <= 1'b0;
            hit <= 1'b0;
            discriminant <= '0;
            ro_x_q <= '0; ro_y_q <= '0; ro_z_q <= '0;
            rd_x_q <= '0; rd_y_q <= '0; rd_z_q <= '0;
            center_x_q <= '0; center_y_q <= '0; center_z_q <= '0;
            radius_q <= '0;
        end else begin
            valid_q <= in_valid;
            out_valid <= valid_q;
            if (in_valid) begin
                ro_x_q <= ro_x; ro_y_q <= ro_y; ro_z_q <= ro_z;
                rd_x_q <= rd_x; rd_y_q <= rd_y; rd_z_q <= rd_z;
                center_x_q <= center_x; center_y_q <= center_y; center_z_q <= center_z;
                radius_q <= radius;
            end
            if (valid_q) begin
                hit <= core_hit;
                discriminant <= core_discriminant;
            end
        end
    end
endmodule
