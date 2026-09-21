# RTL arithmetic contract

The hardware block implements the ray–sphere predicate used by the software renderer:

```text
oc       = ray_origin - sphere_center
a        = dot(ray_direction, ray_direction)
half_b   = dot(oc, ray_direction)
c        = dot(oc, oc) - radius²
disc     = half_b² - a·c
hit      = a > 0 and disc >= 0 and (half_b <= 0 or c <= 0)
```

## Numeric representation

Inputs use signed Q8.8 fixed point except `radius`, which is unsigned Q8.8. Products are retained in wide signed intermediates. The exported discriminant is a signed 64-bit Q32.32 value.

The final direction test matters. A non-negative discriminant proves the infinite line intersects the sphere, but it does not prove that the intersection lies in front of the ray origin. `half_b <= 0` accepts spheres ahead of the origin; `c <= 0` also accepts a ray that starts inside the sphere.

## Language split

- `rtl/verilog/ray_sphere_discriminant.v` is the synthesizable Verilog arithmetic core.
- `rtl/systemverilog/ray_packet_pipeline.sv` adds a two-cycle valid/data pipeline.
- `rtl/systemverilog/tb_ray_pipeline.sv` is the self-checking SystemVerilog testbench.
- `rtl/vhdl/ray_sphere_discriminant.vhd` independently expresses the same arithmetic contract in VHDL.
- `rtl/vhdl/tb_ray_sphere_discriminant.vhd` replays the same generated vectors for cross-language parity.

`tools/gen_scene.pl` generates deterministic integer cases and calculates the reference predicate before either simulator sees the vectors. This tests language parity; it is not formal equivalence proof.

## Current boundary

The RTL is a standalone accelerator candidate. The repository does not claim PCIe, AXI, DMA, FPGA synthesis results, or an end-to-end hardware speedup. A logical next step is wrapping the pipeline in AXI-Stream and replacing the CPU sphere predicate through a measurable host/FPGA transport path.
