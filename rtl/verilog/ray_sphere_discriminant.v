`timescale 1ns/1ps

// Fixed-point Q8.8 ray/sphere hit predicate.
// Tests whether a forward ray intersects a sphere without computing sqrt(t).
module ray_sphere_discriminant (
    input  wire signed [15:0] ro_x,
    input  wire signed [15:0] ro_y,
    input  wire signed [15:0] ro_z,
    input  wire signed [15:0] rd_x,
    input  wire signed [15:0] rd_y,
    input  wire signed [15:0] rd_z,
    input  wire signed [15:0] center_x,
    input  wire signed [15:0] center_y,
    input  wire signed [15:0] center_z,
    input  wire        [15:0] radius,
    output reg                  hit,
    output reg signed [63:0]    discriminant
);
    reg signed [16:0] oc_x;
    reg signed [16:0] oc_y;
    reg signed [16:0] oc_z;
    reg signed [63:0] a;
    reg signed [63:0] half_b;
    reg signed [63:0] c;
    reg signed [63:0] radius_squared;

    always @* begin
        oc_x = $signed(ro_x) - $signed(center_x);
        oc_y = $signed(ro_y) - $signed(center_y);
        oc_z = $signed(ro_z) - $signed(center_z);

        a = $signed(rd_x) * $signed(rd_x)
          + $signed(rd_y) * $signed(rd_y)
          + $signed(rd_z) * $signed(rd_z);
        half_b = $signed(oc_x) * $signed(rd_x)
               + $signed(oc_y) * $signed(rd_y)
               + $signed(oc_z) * $signed(rd_z);
        radius_squared = $signed({1'b0, radius}) * $signed({1'b0, radius});
        c = $signed(oc_x) * $signed(oc_x)
          + $signed(oc_y) * $signed(oc_y)
          + $signed(oc_z) * $signed(oc_z)
          - radius_squared;

        discriminant = half_b * half_b - a * c;
        hit = (a > 0) && (discriminant >= 0) && ((half_b <= 0) || (c <= 0));
    end
endmodule
