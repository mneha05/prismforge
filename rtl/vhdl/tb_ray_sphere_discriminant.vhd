library ieee;
use ieee.std_logic_1164.all;
use ieee.numeric_std.all;
use std.textio.all;

entity tb_ray_sphere_discriminant is
  generic (
    VECTOR_PATH : string := "rtl/test_vectors.txt"
  );
end entity;

architecture test of tb_ray_sphere_discriminant is
  signal ro_x, ro_y, ro_z             : signed(15 downto 0);
  signal rd_x, rd_y, rd_z             : signed(15 downto 0);
  signal center_x, center_y, center_z : signed(15 downto 0);
  signal radius                       : unsigned(15 downto 0);
  signal hit                          : std_logic;
  signal discriminant                 : signed(63 downto 0);
begin
  dut: entity work.ray_sphere_discriminant_vhdl
    port map (
      ro_x => ro_x, ro_y => ro_y, ro_z => ro_z,
      rd_x => rd_x, rd_y => rd_y, rd_z => rd_z,
      center_x => center_x, center_y => center_y, center_z => center_z,
      radius => radius, hit => hit, discriminant => discriminant
    );

  stimulus: process
    file vectors : text open read_mode is VECTOR_PATH;
    variable row : line;
    variable v_ro_x, v_ro_y, v_ro_z : integer;
    variable v_rd_x, v_rd_y, v_rd_z : integer;
    variable v_cx, v_cy, v_cz, v_radius, expected : integer;
    variable total : integer := 0;
  begin
    while not endfile(vectors) loop
      readline(vectors, row);
      read(row, v_ro_x); read(row, v_ro_y); read(row, v_ro_z);
      read(row, v_rd_x); read(row, v_rd_y); read(row, v_rd_z);
      read(row, v_cx); read(row, v_cy); read(row, v_cz);
      read(row, v_radius); read(row, expected);
      ro_x <= to_signed(v_ro_x, 16); ro_y <= to_signed(v_ro_y, 16); ro_z <= to_signed(v_ro_z, 16);
      rd_x <= to_signed(v_rd_x, 16); rd_y <= to_signed(v_rd_y, 16); rd_z <= to_signed(v_rd_z, 16);
      center_x <= to_signed(v_cx, 16); center_y <= to_signed(v_cy, 16); center_z <= to_signed(v_cz, 16);
      radius <= to_unsigned(v_radius, 16);
      wait for 1 ns;
      total := total + 1;
      if expected = 1 then
        assert hit = '1' report "expected hit on vector " & integer'image(total) severity error;
      else
        assert hit = '0' report "expected miss on vector " & integer'image(total) severity error;
      end if;
    end loop;
    report "PASS: " & integer'image(total) & " VHDL vectors" severity note;
    wait;
  end process;
end architecture;
