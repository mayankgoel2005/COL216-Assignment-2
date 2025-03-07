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
class NFProcessor {
    private:
        Latch1 L1;
        Latch2 L2;
        Latch3 L3;
        Latch4 L4;
        vector<int> REG;
        vector<string> instructions;
        vector<string> v;
        vector<int> MEM;
        int Cycles;
    
    public:
        NFProcessor(const vector<string>& instrs, int cycles)
            : instructions(instrs), Cycles(cycles), REG(32, 0) {
            L1 = {0, 0};
            L2 = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
            L3 = {0, 0, 0, 0, 0, 0, 0};
            L4 = {0, 0, 0, 0};
        }
    
        void IFSTAGE(int pc) {
            string binary(32, '0');
            string hex = instructions[pc];
            int pt = 0;
            for (char e : hex) {
                int a = stoi(string(1, e), nullptr, 16);
                int k = 3;
                while (k >= 0) {
                    binary[pt + k] = (a % 2) + '0';
                    a /= 2;
                    k--;
                }
                pt += 4;
            }
            v.push_back(binary);
            L1.pc = pc;
            L1.ON = 1;
        }
    
        int IDSTAGE() {
            L2.pc = L1.pc;
            string opcode = v[L2.pc].substr(25, 7);
            if (opcode == "0110011") { // add sub div mul
                L2.rd = stoi(v[L2.pc].substr(20, 5), nullptr, 2);
                L2.rs1 = stoi(v[L2.pc].substr(12, 5), nullptr, 2);
                L2.rs2 = stoi(v[L2.pc].substr(7, 5), nullptr, 2);
                L2.aluop = 2;
                L2.alusrc = 0;
                L2.memread = 0;
                L2.memwrite = 0;
                L2.memtoreg = 0;
                L2.regwrite = 1;
                if (L3.regwrite && (L3.rd == L2.rs1 || L3.rd == L2.rs2)) {
                    return 2;
                } else if (L4.regwrite && (L4.rd == L2.rs1 || L4.rd == L2.rs2)) {
                    return 1;
                } else {
                    return 0;
                }
            } else if (opcode == "0010011" || opcode == "0000011") {
                L2.rd = stoi(v[L2.pc].substr(20, 5), nullptr, 2); // addi subi
                L2.rs1 = stoi(v[L2.pc].substr(12, 5), nullptr, 2);
                L2.rs2 = -1;
                L2.aluop = 0;
                L2.alusrc = 1;
                L2.memread = 0;
                L2.memwrite = 0;
                L2.memtoreg = 0;
                L2.regwrite = 1;
                if (opcode == "0000011") { // load
                    L2.memtoreg = 1;
                    L2.memread = 1;
                }
                if (L3.regwrite && L3.rd == L2.rs1) {
                    return 2;
                } else if (L4.regwrite && L4.rd == L2.rs1) {
                    return 1;
                } else {
                    return 0;
                }
            } else if (opcode == "0100011") { // store
                L2.rd = -1;
                L2.rs1 = stoi(v[L2.pc].substr(12, 5), nullptr, 2);
                L2.rs2 = stoi(v[L2.pc].substr(7, 5), nullptr, 2);
                L2.aluop = 0;
                L2.alusrc = 1;
                L2.regwrite = 0;
                L2.memtoreg = 0;
                L2.memread = 0;
                L2.memwrite = 1;
                if (L3.regwrite && (L3.rd == L2.rs1 || L3.rd == L2.rs2)) {
                    return 2;
                } else if (L4.regwrite && (L4.rd == L2.rs1 || L4.rd == L2.rs2)) {
                    return 1;
                } else {
                    return 0;
                }
            } else if (opcode == "1100011") { // branch
                L2.rs2 = stoi(v[L2.pc].substr(7, 5), nullptr, 2);
                L2.rs1 = stoi(v[L2.pc].substr(12, 5), nullptr, 2);
                L2.rd = -1;
                L2.aluop = 1;
                L2.alusrc = 0;
                L2.memread = 0;
                L2.memwrite = 0;
                L2.memtoreg = 0;
                L2.regwrite = 0;
                if (L3.regwrite == 1 && (L3.rd == L2.rs1 || L3.rd == L2.rs2)) {
                    return 2;
                } else if (L4.regwrite == 1 && (L4.rd == L2.rs1 || L4.rd == L2.rs2)) {
                    return 1;
                } else {
                    return 0;
                }
            } else if (opcode == "1101111") { // jal
                L2.rd = stoi(v[L2.pc].substr(20, 5), nullptr, 2);
                L2.rs1 = -1;
                L2.rs2 = -1;
                L2.aluop = 0;
                L2.alusrc = 1;
                L2.memread = 0;
                L2.memwrite = 0;
                L2.memtoreg = 0;
                L2.regwrite = 1;
                return 0;
            } else if (opcode == "1100111") { // jalr
                L2.rs1 = stoi(v[L2.pc].substr(12, 5), nullptr, 2);
                L2.rs2 = -1;
                L2.rd = stoi(v[L2.pc].substr(20, 5), nullptr, 2);
                L2.memread = 0;
                L2.alusrc = 1;
                L2.aluop = 0;
                L2.memwrite = 0;
                L2.memtoreg = 0;
                L2.regwrite = 1;
                if (L3.regwrite && L3.rd == L2.rs1) {
                    return 2;
                } else if (L4.regwrite && L4.rd == L2.rs1) {
                    return 1;
                } else {
                    return 0;
                }
            }
            return -1;
        }
    
        int EXSTAGE(int pc, int rs1, int rs2, int rd, int aluop, int alusrc) {
            L3.rd = rd;
            L3.memtoreg = L2.memtoreg;
            L3.memread = L2.memread;
            L3.memwrite = L2.memwrite;
            L3.regwrite = L2.regwrite;
            if (L2.aluop == 2) {
                string f3 = v[pc].substr(17, 3);
                string f7 = v[pc].substr(0, 7);
                if (f3 == "000" && f7 == "0000000") {
                    REG[L3.rd] = REG[rs1] + REG[rs2];
                } else if (f3 == "000" && f7 == "0100000") {
                    REG[L3.rd] = REG[rs1] - REG[rs2];
                } else if (f3 == "000" && f7 == "0000001") {
                    REG[L3.rd] = REG[rs1] * REG[rs2];
                } else if (f3 == "100" && f7 == "0000001") {
                    REG[L3.rd] = REG[rs1] / REG[rs2];
                } else if (f3 == "111" && f7 == "0000000") {
                    REG[L3.rd] = REG[rs1] & REG[rs2];
                } else if (f3 == "110" && f7 == "0000000") {
                    REG[L3.rd] = REG[rs1] | REG[rs2];
                } else if (f3 == "100" && f7 == "0000000") {
                    REG[L3.rd] = REG[rs1] ^ REG[rs2];
                }
                return -1;
            } else if (L2.aluop == 0 && rs1 != -1) {
                string f3 = v[pc].substr(17, 3);
                int imm = stoi(v[pc].substr(0, 12), nullptr, 2);
                string f7 = v[pc].substr(25, 7);
                if (f3 == "010" && L3.memwrite == 1) {
                    return REG[rs1] + imm;
                } else if (f3 == "000") {
                    REG[L3.rd] = REG[rs1] + imm;
                } else if (f3 == "111") {
                    REG[L3.rd] = REG[rs1] & imm;
                } else if (f3 == "110") {
                    REG[L3.rd] = REG[rs1] | imm;
                } else if (f3 == "100") {
                    REG[L3.rd] = REG[rs1] ^ imm;
                } else if (f3 == "010" && L3.memtoreg == 1) {
                    return REG[rs1] + imm;
                } else if (f3 == "000" && f7 == "1100111") {
                    // Handle special case for jalr
                }
                return -1;
            }
            return -1;
        }
    
        int* MEMSTAGE(int mem, int memr, int source) {
            L4.rd = L3.rd;
            L4.regwrite = L3.regwrite;
            L4.memtoreg = L3.memtoreg;
            if (memr == 1) {
                return &MEM[mem];
            } else if (memr == 0) {
                MEM[mem] = source;
                return NULL;
            } else {
                return NULL;
            }
        }
    
        void WBSTAGE(int* wb) {
            if (L4.regwrite && L4.rd >= 0 && L4.rd < 32 && wb != NULL) {
                REG[L4.rd] = *wb;
            }
        }
    };