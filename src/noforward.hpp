#ifndef NOFORWARD_HPP
#define NOFORWARD_HPP

#include <iostream>
#include <vector>
#include <string>
#include <algorithm>

struct Latch1 {
    int pc;
    int ON;
    int ded;
};

struct Latch2 {
    int pc;
    int rs1;
    int rs2;
    int rd;
    int aluop;
    int alusrc;
    int memread;
    int memwrite;
    int regwrite;
    int memtoreg;
    int ON;
    int branch;
    int nop;
    int ded;
};

struct Latch3 {
    int pc;
    int rd;
    int aluout;
    int rs2val;
    int memread;
    int memwrite;
    int regwrite;
    int memtoreg;
    int ON;
    int j;
    int nop;
    int ded;
};

struct Latch4 {
    int pc;
    int rd;
    int wbvalue;
    int regwrite;
    int ON;
    int nop;
};

class NFProcessor {
private:
    Latch1 L1;
    Latch2 L2;
    Latch3 L3;
    Latch4 L4;
    std::vector<int> REG;
    std::vector<std::string> opcodes;
    std::vector<std::string> v;
    std::vector<std::vector<int>> ans;
    std::vector<int> MEM;
    int Cycles;
    int L0;
    int pc;
    int c;
    int pcc;
    int nope;

public:
    NFProcessor(const std::vector<std::string>& opcodes, int cycles);
    void printpipeline();
    void run();
};

#endif // NOFORWARD_HPP