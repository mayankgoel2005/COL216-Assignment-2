#include <iostream>
#include <vector>
#include <string>
#include <cstdlib>
#include <bits/stdc++.h>
using namespace std;

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
    int rd;
    int result;
    int memread;
    int memwrite;
    int regwrite;
    int memtoreg;
    int ON;
    int pc;
    int rs2;
    int nop;
    int ded;
    int j;
};

struct Latch4 {
    int rd;
    int memtoreg;
    int regwrite;
    int ON;
    int rs2;
    int nop;
    int pc;
    int res;
    int ded;
};

class FProcessor {
private:
    Latch1 L1;
    Latch2 L2;
    Latch3 L3;
    Latch4 L4;
    vector<int> REG;
    vector<string> instructions;
    vector<string> v;
    vector<vector<int>> ans;
    vector<int> MEM;
    int Cycles;
    int L0;
    int pc;
    int c;
    int pcc;
    int nope;

public:
    FProcessor(const vector<string>& instrs, int cycles)
        : instructions(instrs), Cycles(cycles), REG(32, 0), MEM(1024, 1), ans(instrs.size(),vector<int>(cycles,0)) {
        L1 = {-1, 0, 0};
        L2 = {-1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
        L3 = {0, 0, 0, 0, 0, 0, 0, -1, 0, 0, 0, 0};
        L4 = {0, 0, 0, 0, 0, 0, -1, 0, 0};
    }

    int IFSTAGE(int pc) {
        cout<<pc<<" "<<c;
        cout<<L2.pc<<L3.pc<<L4.pc<<pcc;
        if(pc!=-1 &&(L2.pc==pc || L3.pc==pc || L4.pc==pc || pc==pcc)){
            cout<<"yes"<<L1.pc;
            L0=0;
            return -1;
        }
        ans[pc][c]=1;
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
        v[pc]=binary;
        L1.pc = pc;
        return 1;
    }

    int IDSTAGE() {
        if(L2.nop){
            cout<<"nop";
            L3.nop=L2.nop;
            L2.pc=L1.pc;
            cout<<L1.pc;
            L2.nop=0;
            return 0;
        }
        L2.pc=L1.pc;
        cout<<L1.pc;
        cout<<L2.pc<<" "<<c;
        ans[L2.pc][c]=2;
        string opcode = v[L2.pc].substr(25, 7);
        if (opcode == "0110011") {
            L2.rd = stoi(v[L2.pc].substr(20, 5), nullptr, 2);
            L2.rs1 = stoi(v[L2.pc].substr(12, 5), nullptr, 2);
            L2.rs2 = stoi(v[L2.pc].substr(7, 5), nullptr, 2);

            if (L3.memread == 1 && (L3.rd == L2.rs1 || L3.rd == L2.rs2)) {
                L1.ON = 0;
                return 2;
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
            if (L3.memread == 1 && (L3.rd == L2.rs1)) {
                L1.ON = 0;
                return 2;
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
            if (L3.memread == 1 && (L3.rd == L2.rs1 || L3.rd == L2.rs2)) {
                L1.ON = 0;
                return 2;
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
            if (L3.regwrite && (L3.rd==L2.rs1 || L3.rd==L2.rs2)) {
                L1.ON = 0;
                return 2;
            }
            if(L4.memtoreg==1 && (L4.rd==L2.rs1)||(L4.rd==L2.rs2)){
                L1.ON=0;
                return 2;
            }
            string f3 = v[L2.pc].substr(17, 3);
            string imm12  = v[L2.pc].substr(0, 1);
            string imm10_5 = v[L2.pc].substr(1, 6);
            string imm4_1  = v[L2.pc].substr(20, 4);
            string imm11   = v[L2.pc].substr(24, 1);
            string imm_str = imm12 + imm11 + imm10_5 + imm4_1 + "0";
            int imm_val = stoi(imm_str, nullptr, 2);
            if (imm_str[0] == '1') {
                imm_val -= (1 << 13);
            }
            int op1 = (L3.regwrite && L3.rd == L2.rs1) ? L3.result : ((L4.regwrite && L4.rd == L2.rs1 && L4.res != -1) ? L4.res : REG[L2.rs1]);
            int op2 = (L3.regwrite && L3.rd == L2.rs2) ? L3.result : ((L4.regwrite && L4.rd == L2.rs2 && L4.res != -1) ? L4.res : REG[L2.rs2]);

            L2.rd = 0;
            if (f3 == "100") { // BLT
                if (op1<op2) {
                    L2.branch = imm_val;
                }
            } else if (f3 == "101") { // BGE
                if (op1>=op2) {
                    L2.branch = imm_val;
                }
            } else if (f3 == "000") { // BEQ
                if (op1==op2) {
                    L2.branch = imm_val;
                }
            } else if (f3 == "001") { // BNE
                if (op1!=op2) {
                    L2.branch = imm_val;
                }
            }
            L2.aluop = 1;
            L2.alusrc = 0;
            L2.memread = 0;
            L2.memwrite = 0;
            L2.memtoreg = 0;
            L2.regwrite = 0;
        }else if (opcode == "1101111") {
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
            if (L3.memread == 1 && (L3.rd == L2.rs1)) {
                L1.ON = 0;
                return 2;
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
        if(L3.nop){
            cout<<"nop";
            L4.nop=L3.nop;
            L3.nop=0;
            L3.pc=L2.pc;
            return;
        }
        cout<<L2.pc<<" "<<c;
        ans[L2.pc][c]=3;
        L3.rd = L2.rd;
        L3.rs2=L2.rs2;
        L3.pc = L2.pc;
        L3.memtoreg = L2.memtoreg;
        L3.memread = L2.memread;
        L3.memwrite = L2.memwrite;
        L3.regwrite = L2.regwrite;
        string opcode = v[L3.pc].substr(25, 7);
        if (L2.aluop == 2) {
            string f3 = v[L3.pc].substr(17, 3);
            string f7 = v[L3.pc].substr(0, 7);
            int op1 = (L3.regwrite && L3.rd == L2.rs1) ? L3.result : ((L4.regwrite && L4.rd == L2.rs1 && L4.res != -1) ? L4.res : REG[L2.rs1]);
            int op2 = (L3.regwrite && L3.rd == L2.rs2) ? L3.result : ((L4.regwrite && L4.rd == L2.rs2 && L4.res != -1) ? L4.res : REG[L2.rs2]);
            if (f3 == "000" && f7 == "0000000") {
                L3.result = op1 + op2;
            } else if (f3 == "000" && f7 == "0100000") {
                L3.result = op1 - op2;
            } else if (f3 == "000" && f7 == "0000001") {
                L3.result = op1 * op2;
            } else if (f3 == "100" && f7 == "0000001") {
                L3.result = op1 / op2;
            } else if (f3 == "111" && f7 == "0000000") {
                L3.result = op1 & op2;
            } else if (f3 == "110" && f7 == "0000000") {
                L3.result = op1 | op2;
            } else if (f3 == "100" && f7 == "0000000") {
                L3.result = op1 ^ op2;
            }
        }  else if(L2.aluop == 0 && L2.rs1 != -1) {
            string f3 = v[L3.pc].substr(17, 3);
            int imm;
            if(L3.memwrite == 1) {
                int imm_high = stoi(v[L3.pc].substr(0, 7), nullptr, 2);
                int imm_low  = stoi(v[L3.pc].substr(20, 5), nullptr, 2);
                imm = (imm_high << 5) | imm_low;
                if(v[L3.pc][0] == '1') {
                    imm -= (1 << 12);
                }
            } else {
                imm = stoi(v[L3.pc].substr(0, 12), nullptr, 2);
                if(v[L3.pc][0] == '1') {
                    imm -= (1 << 12);
                }
            }
            int op1 = (L3.regwrite && L3.rd == L2.rs1) ? L3.result : ((L4.regwrite && L4.rd == L2.rs1 && L4.res != -1) ? L4.res : REG[L2.rs1]);
            L3.result = op1 + imm;
        }else if(opcode == "1101111"){
            string inst = v[L3.pc]; 
            int imm_20   = (inst[0] - '0') << 20;
            int imm_10_1 = stoi(inst.substr(1, 10), nullptr, 2) << 1;
            int imm_11   = (inst[11] - '0') << 11;
            int imm_19_12 = stoi(inst.substr(12, 8), nullptr, 2) << 12;
            int imm = imm_20 | imm_19_12 | imm_11 | imm_10_1;
            if (inst[0] == '1')
                imm-=(1<<21);           
            L3.result = L3.pc + 1; 
            L3.j=L3.pc+imm/4; 
            cout<<imm<<L3.pc;        
        }else if(opcode=="1100111"){
            string inst=v[L3.pc];
            int imm=stoi(inst.substr(0,12),nullptr,2);
            if(v[L3.pc][0]=='1'){
                imm-=(1<<12);
            }
            int base = (L3.regwrite && L3.rd == L2.rs1) ? L3.result : ((L4.regwrite && L4.rd == L2.rs1 && L4.res != -1) ? L4.res : REG[L2.rs1]);
            L3.result = L3.pc+1;
            L3.j=base+imm/4;            
        }
    }

    void MEMSTAGE() {
        if(L4.nop){
            cout<<"nop";
            nope=L4.nop;
            L4.nop=0;
            L4.pc=L3.pc;
            return;
        }
        ans[L3.pc][c]=4;
        cout<<L3.pc<<" "<<c;
        L4.rd = L3.rd;
        L4.rs2=L3.rs2;
        L4.pc=L3.pc;
        L3.rs2=0;
        L3.rd=0;
        L4.regwrite = L3.regwrite;
        L4.memtoreg = L3.memtoreg;
        L3.memtoreg=0;
        L3.regwrite=0;
        if (L3.memread == 1) {
            if (L3.result >= 0 && L3.result < (int)MEM.size()) {
                L4.res = MEM[L3.result];
            } else {
                L4.res = -1;
            }
        } else if (L3.memwrite == 1) {
            if (L3.result >= 0 && L3.result < (int)MEM.size() &&
               L4.rs2 >= 0 && L4.rs2 < 32) {
                MEM[L3.result] = REG[L4.rs2];
            }
            L4.res = -1;
        } else {
            L4.res = L3.result;
        }

    }

    void WBSTAGE() {
        if(nope){
            cout<<"nop";
            nope=0;
            pcc=L4.pc;
            return;
        }
        pcc=L4.pc;
        ans[pcc][c]=5;
        cout<<pcc<<" "<<c;
        if (L4.regwrite && L4.rd >= 0 && L4.rd < 32 && L4.res != -1) {
            REG[L4.rd] = L4.res;
        }
        L4.regwrite=0;
        L4.rd=0;
    }
    void printpipeline(){
        map<int,string> mpp={{1,"IF"},{2,"ID"},{3,"EX"},{4,"MEM"},{5,"WB"}};
        for(auto e:ans){
            int k=-2;
            for(auto f:e){
                if(f==0){
                    cout<<"; ";
                }else{
                    if(f==k){
                        cout<<";-";
                    }
                    else{
                        cout<<";"<<mpp[f];
                    }
                }
                k=f;
            }
            cout<<endl;
        }
        return;
    }
    void run() {
        REG[4]=2;
        pc = 0;
        pcc=-1;
        nope=0;
        L0 = 1;
        int k=-1;
        int l=0;
        int ll=0;
        v.resize(instructions.size(), "");
        for(int cycle = 0; cycle < Cycles; cycle++) {
            c=cycle;
            if(L4.ON) {
                cout << "WB ";
                WBSTAGE();
                if(L3.ded){
                    L4.ON=0;
                    L4.ded=1;
                }
            }else{
                pcc=-1;
                cout<<"- ";
            }
            if(L3.ON) {
                cout << "MEM ";
                MEMSTAGE();
                L4.ON = 1;
                if(L2.ded){
                    L3.ON=0;
                    L3.ded=1;
                }
            } else {
                cout<<"- ";
                L4.ON = 0;
                L4.pc=-1;
            }
            if(L2.ON) {
                cout << "EX ";
                EXSTAGE();
                L3.ON = 1;
                if(L1.ded){
                    L2.ded=1;
                    L2.ON=0;
                }
            } else {
                cout<<"- ";
                L3.ON = 0;
                L3.pc=-1;
            }
            if(!L1.ded && k!=-1){
                L1.ON=1;
            }
            if(L1.ON) {
                cout << "ID ";
                k = IDSTAGE();
                L0=1;
                if(l==-1 && k==0){
                    L1.ded=1;
                }
            }else{
                L2.ON=0;
                if(k!=-1){
                    L0=0;
                }
                cout<<"- ";
                L2.pc=-1;
            }
            if(L0 && pc < (int)instructions.size()) {
                if(!L2.branch && !L3.j){
                    cout <<"IF ";
                }else if(L3.j){
                    cout<<"IFj ";
                    L2.nop=1;
                    L3.nop=1;
                }else{
                    cout<<"IFb";
                    L2.nop=1;
                }
                int x=IFSTAGE(pc);
                if(x==1){
                    if(L3.j){
                        cout<<pc<<L3.result;
                        pc=L3.j;
                        L3.j=0;
                    }
                    else if(L2.branch){
                        pc--;
                        pc+=(L2.branch/4);
                        L2.branch=0;
                    }
                    else{
                        pc++;
                    }
                    L1.ON=1;
                }else if(x==-1){
                    L2.nop=1;
                    L1.pc=-1;
                }
                if(pc==instructions.size()){
                    l=-1;
                }
            }else{
                L1.ON=0;
                cout<<"- ";
            }
            if(k == 2 || k == 1||k==-1) {
                L2.ON = 0;
            } else if(!L2.ded){
                L2.ON=1;
            }else{
                L2.ON=0;
            }
            cout<<endl;
        }
        printpipeline();
        cout<<REG[0]<<" "<<REG[1]<<" "<<REG[2]<<" "<<REG[3]<<" "<<REG[4]<<" "<<REG[5]<<" "<<REG[6]<<" "<<MEM[1];
    }
};

int main() {
    vector<string> instructions = {
        "00210093",        //addi x1 x2 2
        "00218113",        //addi x2 x3 2
        "fe408ce3",        //beq x1 x4 -8
        "004100b3"          //add x1 x2 x4
    };

    FProcessor processor(instructions, 20);
    processor.run();
    return 0;
}