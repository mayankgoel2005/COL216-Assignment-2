#include <iostream>
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
    int pc;
};

struct Latch4 {
    int rd;
    int memtoreg;
    int regwrite;
    int ON;
    int* res;
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
    int L0;

public:
    NFProcessor(const vector<string>& instrs, int cycles)
        : instructions(instrs), Cycles(cycles), REG(32, 0), MEM(1024, 0) {
        L1 = {0, 0};
        L2 = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
        L3 = {0, 0, 0, 0, 0, 0, 0};
        L4 = {0, 0, 0, 0};
    }

    int IFSTAGE(int pc) {
        if(pc!=0 && L1.ON==0){
            L0=0;
            return 0;
        }
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
        return 1;
    }

    int IDSTAGE() {
        L2.pc=L1.pc;
        string opcode = v[L2.pc].substr(25, 7);
        if (opcode == "0110011") {
            L2.rd = stoi(v[L2.pc].substr(20, 5), nullptr, 2);
            L2.rs1 = stoi(v[L2.pc].substr(12, 5), nullptr, 2);
            L2.rs2 = stoi(v[L2.pc].substr(7, 5), nullptr, 2);
        
            if (L3.regwrite == 1 && (L3.rd == L2.rs1 || L3.rd == L2.rs2)) {
                L1.ON = 0;
                return 2;
            } else if (L4.regwrite == 1 && (L4.rd == L2.rs1 || L4.rd == L2.rs2)) {
                L1.ON = 0;
                return 1;
            }
            L2.aluop = 2;
            L2.alusrc = 0;
            L2.memread = 0;
            L2.memwrite = 0;
            L2.memtoreg = 0;
            L2.regwrite = 1;
        } else if (opcode == "0010011" || opcode == "0000011") {
            L2.rd = stoi(v[L2.pc].substr(20, 5), nullptr, 2);
            L2.rs1 = stoi(v[L2.pc].substr(12, 5), nullptr, 2);
            L2.rs2 = -1;
            if (L3.regwrite == 1 && (L3.rd == L2.rs1 || L3.rd == L2.rs2)) {
                L1.ON = 0;
                return 2;
            } else if (L4.regwrite == 1 && (L4.rd == L2.rs1 || L4.rd == L2.rs2)) {
                L1.ON = 0;
                return 1;
            }
            L2.aluop = 0;
            L2.alusrc = 1;
            L2.memread = 0;
            L2.memwrite = 0;
            L2.memtoreg = 0;
            L2.regwrite = 1;
            if (opcode == "0000011") {
                L2.memtoreg = 1;
                L2.memread = 1;
            }
        } else if (opcode == "0100011") {
            L2.rd = -1;
            L2.rs1 = stoi(v[L2.pc].substr(12, 5), nullptr, 2);
            L2.rs2 = stoi(v[L2.pc].substr(7, 5), nullptr, 2);
            if (L3.regwrite == 1 && (L3.rd == L2.rs1 || L3.rd == L2.rs2)) {
                L1.ON = 0;
                return 2;
            } else if (L4.regwrite == 1 && (L4.rd == L2.rs1 || L4.rd == L2.rs2)) {
                L1.ON = 0;
                return 1;
            }
            L2.aluop = 0;
            L2.alusrc = 1;
            L2.regwrite = 0;
            L2.memtoreg = 0;
            L2.memread = 0;
            L2.memwrite = 1;
        } else if (opcode == "1100011") {
            L2.rs2 = stoi(v[L2.pc].substr(7, 5), nullptr, 2);
            L2.rs1 = stoi(v[L2.pc].substr(12, 5), nullptr, 2);
            L2.rd = -1;
            L2.aluop = 1;
            L2.alusrc = 0;
            L2.memread = 0;
            L2.memwrite = 0;
            L2.memtoreg = 0;
            L2.regwrite = 0;
        } else if (opcode == "1101111") {
            L2.rd = stoi(v[L2.pc].substr(20, 5), nullptr, 2);
            L2.rs1 = -1;
            L2.rs2 = -1;
            L2.aluop = 0;
            L2.alusrc = 1;
            L2.memread = 0;
            L2.memwrite = 0;
            L2.memtoreg = 0;
            L2.regwrite = 1;
        } else if (opcode == "1100111") {
            L2.rs1 = stoi(v[L2.pc].substr(12, 5), nullptr, 2);
            L2.rs2 = -1;
            L2.rd = stoi(v[L2.pc].substr(20, 5), nullptr, 2);
            if (L3.regwrite == 1 && (L3.rd == L2.rs1 || L3.rd == L2.rs2)) {
                L1.ON = 0;
                return 2;
            } else if (L4.regwrite == 1 && (L4.rd == L2.rs1 || L4.rd == L2.rs2)) {
                L1.ON = 0;
                return 1;
            }
            L2.memread = 0;
            L2.alusrc = 1;
            L2.aluop = 0;
            L2.memwrite = 0;
            L2.memtoreg = 0;
            L2.regwrite = 1;
        }
        return 0;
    }

    void EXSTAGE() {
        L3.rd = L2.rd;
        L3.pc = L2.pc;
        L3.memtoreg = L2.memtoreg;
        L3.memread = L2.memread;
        L3.memwrite = L2.memwrite;
        L3.regwrite = L2.regwrite;
        if (L2.aluop == 2) {
            string f3 = v[L3.pc].substr(17, 3);
            string f7 = v[L3.pc].substr(0, 7);
            if (f3 == "000" && f7 == "0000000") {
                L3.result = REG[L2.rs1] + REG[L2.rs2];
            } else if (f3 == "000" && f7 == "0100000") {
                L3.result = REG[L2.rs1] - REG[L2.rs2];
            } else if (f3 == "000" && f7 == "0000001") {
                L3.result = REG[L2.rs1] * REG[L2.rs2];
            } else if (f3 == "100" && f7 == "0000001") {
                L3.result = REG[L2.rs1] / REG[L2.rs2];
            } else if (f3 == "111" && f7 == "0000000") {
                L3.result = REG[L2.rs1] & REG[L2.rs2];
            } else if (f3 == "110" && f7 == "0000000") {
                L3.result = REG[L2.rs1] | REG[L2.rs2];
            } else if (f3 == "100" && f7 == "0000000") {
                L3.result = REG[L2.rs1] ^ REG[L2.rs2];
            }
        } else if (L2.aluop == 0 && L2.rs1 != -1) {
            string f3 = v[L3.pc].substr(17, 3);
            int imm = stoi(v[L3.pc].substr(0, 12), nullptr, 2);
            string f7 = v[L3.pc].substr(25, 7);
            if (f3 == "010" && L3.memwrite == 1) {
                L3.result = REG[L2.rs1] + imm;
            } else if (f3 == "000") {
                L3.result = REG[L2.rs1] + imm;
            } else if (f3 == "111") {
                L3.result = REG[L2.rs1] & imm;
            } else if (f3 == "110") {
                L3.result = REG[L2.rs1] | imm;
            } else if (f3 == "100") {
                L3.result = REG[L2.rs1] ^ imm;
            } else if (f3 == "010" && L3.memtoreg == 1) {
                L3.result = REG[L2.rs1] + imm;
            }
        }
    }

    void MEMSTAGE() {
        L4.rd = L3.rd;
        L3.rd=0;
        L4.regwrite = L3.regwrite;
        L4.memtoreg = L3.memtoreg;
        L3.memtoreg=0;
        L3.regwrite=0;
        if (L3.memread == 1) {
            if (L3.result >= 0 && L3.result < (int)MEM.size()) {
                L4.res = &MEM[L3.result];
            } else {
                L4.res = nullptr;
            }
        } else if (L3.memwrite == 1) {
            if (L3.result >= 0 && L3.result < (int)MEM.size() && 
               L4.rd >= 0 && L4.rd < 32) {
                MEM[L3.result] = REG[L4.rd];
            }
            L4.res = nullptr;
        } else {
            L4.res = &L3.result;
        }
        
    }

    void WBSTAGE() {
        if (L4.regwrite && L4.rd >= 0 && L4.rd < 32 && L4.res != nullptr) {
            REG[L4.rd] = *L4.res;
        }
        L4.regwrite=0;
        L4.rd=0;
    }

    void run() {
        int pc = 0;
        L0 = 1;
        int k=-1;
        for(int cycle = 0; cycle < Cycles; cycle++) {
            if(L4.ON) {
                cout << "WB ";
                WBSTAGE();
            }else{
                cout<<"- ";
            }
            if(L3.ON) {
                cout << "MEM ";
                MEMSTAGE();
                L4.ON = 1;
            } else {
                cout<<"- ";
                L4.ON = 0;
            }
            if(L2.ON==1) {
                cout << "EX ";
                EXSTAGE();
                L3.ON = 1;
            } else {
                cout<<"- ";
                L3.ON = 0;
            }
            if(k!=-1 && !(L3.regwrite == 1 && (L3.rd == L2.rs1 || L3.rd == L2.rs2))||(L4.regwrite == 1 && (L4.rd == L2.rs1 || L4.rd == L2.rs2))){
                L1.ON=1;
            }
            if(L1.ON) {
                cout << "ID ";
                k = IDSTAGE();
                L0=1;
                L2.ON=1;
            }else if(k!=-1){
                L2.ON=0;
                L0=0;
                cout<<"- ";
            }
            if(L0 && pc < (int)instructions.size()) {
                cout << "IF ";
                int x=IFSTAGE(pc);
                if(x==1){
                    pc++;
                    L1.ON=1;
                }
            }else{
                L1.ON=0;
                cout<<"- ";
            }
            if(k == 2 || k == 1||k==-1) {
                L2.ON = 0;
            } else{
                L2.ON=1;
            }
            cout << endl;
        }
    }
};

int main() {
    vector<string> instructions = {
        "00002083", // lw x1, 0(x4)
        "19022103", // lw x2, 400(x4)
        "002081b3", // add x3, x1, x2
        "00322023", // sw x3, 0(x4)
        "ffc20213", // addi x4, x4, -4
        "007302b3"  // add x5, x6, x7
    };

    NFProcessor processor(instructions, 16);
    processor.run();
    return 0;
}