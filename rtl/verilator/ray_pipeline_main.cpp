#include <verilated.h>
#include "Vray_packet_pipeline.h"

#include <cstdint>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>

static void tick(Vray_packet_pipeline& dut) {
    dut.clk = 0;
    dut.eval();
    dut.clk = 1;
    dut.eval();
}

int main(int argc, char** argv) {
    Verilated::commandArgs(argc, argv);
    const std::string path = argc > 1 ? argv[1] : "build/generated_vectors.txt";
    std::ifstream in(path);
    if (!in) {
        std::cerr << "cannot open vector file: " << path << "\n";
        return 2;
    }

    Vray_packet_pipeline dut;
    dut.rst_n = 0;
    dut.in_valid = 0;
    for (int i = 0; i < 3; ++i) tick(dut);
    dut.rst_n = 1;
    tick(dut);

    long long rox, roy, roz, rdx, rdy, rdz, cx, cy, cz, radius, expected;
    std::uint64_t total = 0, passed = 0, cycles = 0;

    while (in >> rox >> roy >> roz >> rdx >> rdy >> rdz
              >> cx >> cy >> cz >> radius >> expected) {
        dut.ro_x = static_cast<std::int16_t>(rox);
        dut.ro_y = static_cast<std::int16_t>(roy);
        dut.ro_z = static_cast<std::int16_t>(roz);
        dut.rd_x = static_cast<std::int16_t>(rdx);
        dut.rd_y = static_cast<std::int16_t>(rdy);
        dut.rd_z = static_cast<std::int16_t>(rdz);
        dut.center_x = static_cast<std::int16_t>(cx);
        dut.center_y = static_cast<std::int16_t>(cy);
        dut.center_z = static_cast<std::int16_t>(cz);
        dut.radius = static_cast<std::uint16_t>(radius);
        dut.in_valid = 1;

        tick(dut); ++cycles;
        dut.in_valid = 0;

        bool seen = false;
        for (int guard = 0; guard < 8 && !seen; ++guard) {
            tick(dut); ++cycles;
            if (dut.out_valid) {
                seen = true;
                ++total;
                if (dut.hit == static_cast<unsigned>(expected & 1)) {
                    ++passed;
                } else {
                    std::cerr << "vector " << total
                              << " expected hit=" << expected
                              << " got=" << static_cast<int>(dut.hit)
                              << " disc=" << static_cast<long long>(dut.discriminant)
                              << "\n";
                }
            }
        }
        if (!seen) {
            std::cerr << "timeout waiting for out_valid at vector " << (total + 1) << "\n";
            return 3;
        }
    }

    std::cout << "Verilated ray pipeline\n"
              << "vectors=" << total
              << " passed=" << passed
              << " simulated_cycles=" << cycles
              << " cpp_model=Vray_packet_pipeline\n";

    dut.final();
    return passed == total && total > 0 ? 0 : 1;
}
