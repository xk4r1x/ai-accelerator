// tb_pe.cpp - Proper pipeline management
#include "Vprocessing_element.h"
#include "verilated.h"
#include <iostream>

void clock_cycle(Vprocessing_element* pe) {
    pe->clk = 1;
    pe->eval();
    pe->clk = 0;
    pe->eval();
}

void reset(Vprocessing_element* pe) {
    pe->rst_n = 0;
    pe->valid_in = 0;
    clock_cycle(pe);
    pe->rst_n = 1;
    clock_cycle(pe);
}

int main() {
    Vprocessing_element* pe = new Vprocessing_element;
    
    std::cout << "========================================" << std::endl;
    std::cout << "  PE Testbench (Internal Accumulator)" << std::endl;
    std::cout << "========================================\n" << std::endl;
    
    int passed = 0;
    int failed = 0;
    
    // Initialize
    pe->clk = 0;
    pe->rst_n = 0;
    pe->data_in = 0;
    pe->weight_in = 0;
    pe->partial_sum_in = 0;
    pe->valid_in = 0;
    pe->accumulate_en = 0;
    pe->clear_acc = 0;
    pe->use_partial_sum = 0;
    pe->eval();
    
    // Test 1: Reset
    std::cout << "Test 1: Reset" << std::endl;
    reset(pe);
    clock_cycle(pe);
    clock_cycle(pe);
    clock_cycle(pe);
    
    if (pe->partial_sum_out == 0) {
        std::cout << "  PASS\n" << std::endl;
        passed++;
    } else {
        std::cout << "  FAIL\n" << std::endl;
        failed++;
    }
    
    // Test 2: Simple multiply
    std::cout << "Test 2: 5 * 3 = 15" << std::endl;
    reset(pe);
    
    pe->data_in = 5;
    pe->weight_in = 3;
    pe->valid_in = 1;
    pe->accumulate_en = 0;
    
    clock_cycle(pe);
    clock_cycle(pe);
    clock_cycle(pe);
    
    std::cout << "  Expected: 15, Got: " << pe->partial_sum_out << std::endl;
    if (pe->partial_sum_out == 15) {
        std::cout << "  PASS\n" << std::endl;
        passed++;
    } else {
        std::cout << "  FAIL\n" << std::endl;
        failed++;
    }
    
    // Test 3: Accumulation with pipeline flush
    std::cout << "Test 3: 10*2 + 20*3 = 80" << std::endl;
    reset(pe);
    
    // First MAC: 10 * 2 = 20
    pe->data_in = 10;
    pe->weight_in = 2;
    pe->valid_in = 1;
    pe->accumulate_en = 0;
    
    clock_cycle(pe);
    clock_cycle(pe);
    clock_cycle(pe);
    
    // Turn off valid to flush pipeline
    pe->valid_in = 0;
    clock_cycle(pe);
    
    // Second MAC: 20 * 3 = 60, accumulate
    pe->data_in = 20;
    pe->weight_in = 3;
    pe->valid_in = 1;
    pe->accumulate_en = 1;
    
    clock_cycle(pe);
    clock_cycle(pe);
    clock_cycle(pe);
    
    std::cout << "  Expected: 80, Got: " << pe->partial_sum_out << std::endl;
    if (pe->partial_sum_out == 80) {
        std::cout << "  PASS\n" << std::endl;
        passed++;
    } else {
        std::cout << "  FAIL\n" << std::endl;
        failed++;
    }
    
    // Test 4: Clear
    std::cout << "Test 4: Clear accumulator" << std::endl;
    
    pe->valid_in = 0;
    pe->clear_acc = 1;
    clock_cycle(pe);
    pe->clear_acc = 0;
    clock_cycle(pe);
    clock_cycle(pe);
    
    std::cout << "  Expected: 0, Got: " << pe->partial_sum_out << std::endl;
    if (pe->partial_sum_out == 0) {
        std::cout << "  PASS\n" << std::endl;
        passed++;
    } else {
        std::cout << "  FAIL\n" << std::endl;
        failed++;
    }
    
    // Test 5: Partial sum
    std::cout << "Test 5: 100 + 5*3 = 115" << std::endl;
    reset(pe);
    
    pe->data_in = 5;
    pe->weight_in = 3;
    pe->valid_in = 1;
    pe->accumulate_en = 0;
    
    clock_cycle(pe);
    clock_cycle(pe);
    
    pe->partial_sum_in = 100;
    pe->use_partial_sum = 1;
    clock_cycle(pe);
    
    std::cout << "  Expected: 115, Got: " << pe->partial_sum_out << std::endl;
    if (pe->partial_sum_out == 115) {
        std::cout << "  PASS\n" << std::endl;
        passed++;
    } else {
        std::cout << "  FAIL\n" << std::endl;
        failed++;
    }
    
    // Summary
    std::cout << "\n========================================" << std::endl;
    std::cout << "  Results: " << passed << " passed, " << failed << " failed" << std::endl;
    std::cout << "========================================" << std::endl;
    
    if (failed == 0) {
        std::cout << "\n  ALL TESTS PASSED!\n" << std::endl;
    }
    
    delete pe;
    return (failed == 0) ? 0 : 1;
}
