library ieee;
use ieee.std_logic_1164.all;
use ieee.numeric_std.all;

entity ray_sphere_discriminant_vhdl is
  port (
    ro_x, ro_y, ro_z             : in  signed(15 downto 0);
    rd_x, rd_y, rd_z             : in  signed(15 downto 0);
    center_x, center_y, center_z : in  signed(15 downto 0);
    radius                       : in  unsigned(15 downto 0);
    hit                          : out std_logic;
    discriminant                 : out signed(63 downto 0)
  );
end entity;

architecture rtl of ray_sphere_discriminant_vhdl is
begin
  process(all)
    variable oc_x, oc_y, oc_z : signed(16 downto 0);
    variable radius_s         : signed(16 downto 0);
    variable a, half_b, c     : signed(63 downto 0);
    variable radius_squared   : signed(63 downto 0);
    variable disc             : signed(63 downto 0);
  begin
    oc_x := resize(ro_x, 17) - resize(center_x, 17);
    oc_y := resize(ro_y, 17) - resize(center_y, 17);
    oc_z := resize(ro_z, 17) - resize(center_z, 17);
    radius_s := signed('0' & radius);

    a := resize(rd_x * rd_x, 64)
       + resize(rd_y * rd_y, 64)
       + resize(rd_z * rd_z, 64);
    half_b := resize(oc_x * resize(rd_x, 17), 64)
            + resize(oc_y * resize(rd_y, 17), 64)
            + resize(oc_z * resize(rd_z, 17), 64);
    radius_squared := resize(radius_s * radius_s, 64);
    c := resize(oc_x * oc_x, 64)
       + resize(oc_y * oc_y, 64)
       + resize(oc_z * oc_z, 64)
       - radius_squared;
    disc := resize(half_b * half_b, 64) - resize(a * c, 64);

    discriminant <= disc;
    if a > 0 and disc >= 0 and (half_b <= 0 or c <= 0) then
      hit <= '1';
    else
      hit <= '0';
    end if;
  end process;
end architecture;
