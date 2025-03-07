#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <cstdlib>
using namespace std;


struct Latch1 {
    int pc;
    int ON;
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
};

struct Latch3 {
    int rd;
    int result;
    int memread;
    int memwrite;
    int regwrite;
    int memtoreg;
    int ON;
};

struct Latch4 {
    int rd;
    int memtoreg;
    int regwrite;
    int ON;
};
