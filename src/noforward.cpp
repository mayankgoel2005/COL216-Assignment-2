#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <cstdlib>
using namespace std;


struct Latch1 {
    int pc;
    string instr;
};

struct Latch2 {
    int pc;
    int rs1;
    int rs2;
    int rd;
    int aluop;
    int alusrc;
    int branch;
    int memread;
    int memwrite;
    int regwrite;
    int memtoreg;
};

struct Latch3 {
    int rd;
    int zero;
    int result;
    int memread;
    int memwrite;
    int regwrite;
    int memtoreg;
};

struct Latch4 {
    int rd;
    int memtoreg;
    int regwrite;
};

class NonForwardingProcessor {
private:
    Latch1 L1;
    Latch2 L2;
    Latch3 L3;
    Latch4 L4;
    vector<int> REG;
    vector<string> instructions;
    int totalCycles;
public:
    NonForwardingProcessor(const vector<string>& instrs, int cycles)
        : instructions(instrs), totalCycles(cycles), REG(32, 0) {
        L1 = {0, ""};
        L2 = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
        L3 = {0, 0, 0, 0, 0, 0, 0};
        L4 = {0, 0, 0};
    }
    void IFStage(int pc, const string& hex) {
        string binary(32, '0');
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
        L1.pc = pc;
        L1.instr = binary;
    }
    int IDStage() {
        string opcode = L1.instr.substr(25, 7);
        L2.pc = L1.pc;
        if (opcode == "0110011") {
            L2.rd = stoi(L1.instr.substr(20, 5), nullptr, 2);
            L2.rs1 = stoi(L1.instr.substr(12, 5), nullptr, 2);
            L2.rs2 = stoi(L1.instr.substr(7, 5), nullptr, 2);
            L2.aluop = 2;
            L2.alusrc = 0;
            L2.branch = 0;
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
            L2.rd = stoi(L1.instr.substr(20, 5), nullptr, 2);
            L2.rs1 = stoi(L1.instr.substr(12, 5), nullptr, 2);
            L2.rs2 = -1;
            L2.aluop = 0;
            L2.alusrc = 1;
            L2.memread = 0;
            L2.memwrite = 0;
            L2.memtoreg = 0;
            L2.regwrite = 1;
            L2.branch = 0;
            if (opcode == "0000011") {
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
        } else if (opcode == "0100011") {
            L2.rd = -1;
            L2.rs1 = stoi(L1.instr.substr(12, 5), nullptr, 2);
            L2.rs2 = stoi(L1.instr.substr(7, 5), nullptr, 2);
            L2.aluop = 0;
            L2.alusrc = 1;
            L2.branch = 0;
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
        }
        return 0;
    }
    void EXStage() {
        L3.rd = L2.rd;
        L3.regwrite = L2.regwrite;
        L3.memtoreg = L2.memtoreg;
        if (L2.rs2 != -1) {
            L3.result = REG[L2.rs1] + REG[L2.rs2];
        } else {
            L3.result = REG[L2.rs1];
        }
        L3.memread = L2.memread;
        L3.memwrite = L2.memwrite;
        L3.zero = (L3.result == 0) ? 1 : 0;
    }
    void MEMStage() {
        L4.rd = L3.rd;
        L4.regwrite = L3.regwrite;
        L4.memtoreg = L3.memtoreg;
    }
    void WBStage() {
        if (L4.regwrite && L4.rd >= 0 && L4.rd < 32) {
            REG[L4.rd] = L3.result;
        }
    }
    void simulateCycle(int cycle, int &pc, bool stall) {
        cout << "Cycle " << cycle << ":\n";
        if (!stall && pc < (int)instructions.size()) {
            IFStage(pc, instructions[pc]);
            pc++;
        } else {
            cout << "Stall in IF stage.\n";
        }
        int hazard = IDStage();
        if (hazard > 0) {
            cout << "Hazard detected: inserting stall of type " << hazard << "\n";
        }
        EXStage();
        MEMStage();
        WBStage();
        cout << "Latch1 PC: " << L1.pc << "\n";
        cout << "Latch2 rd: " << L2.rd << ", rs1: " << L2.rs1 << ", rs2: " << L2.rs2 << "\n";
        cout << "Latch3 result: " << L3.result << "\n";
        cout << "-------------------------------\n";
    }
    void run() {
        int pc = 0;
        for (int cycle = 0; cycle < totalCycles; cycle++) {
            int hazard = IDStage();
            bool stall = (hazard > 0);
            simulateCycle(cycle, pc, stall);
        }
    }
};

int main(int argc, char* argv[]) {
    if (argc < 3) {
        cerr << "Usage: " << argv[0] << " <inputfile> <cyclecount>" << endl;
        return 1;
    }
    string filename = argv[1];
    int cycleCount = atoi(argv[2]);
    if (cycleCount <= 0) {
        cerr << "Cycle count must be positive." << endl;
        return 1;
    }
    ifstream infile(filename);
    if (!infile) {
        cerr << "Failed to open file: " << filename << endl;
        return 1;
    }
    vector<string> instrs;
    string line;
    while(getline(infile, line)) {
        if(!line.empty()) {
            instrs.push_back(line);
        }
    }
    infile.close();
    NonForwardingProcessor processor(instrs, cycleCount);
    processor.run();
    return 0;
}
