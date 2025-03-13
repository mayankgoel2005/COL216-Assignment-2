#include <iostream>
#include <vector>
#include <string>
#include <cstdlib>
#include <bits/stdc++.h>
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
    int branch;
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
};

struct Latch4 {
    int rd;
    int memtoreg;
    int regwrite;
    int ON;
    int rs2;
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
    vector<vector<int>> ans;
    vector<int> MEM;
    int Cycles;
    int L0;
    int pc;

public:
    NFProcessor(const vector<string>& instrs, int cycles)
        : instructions(instrs), Cycles(cycles), REG(32, 0), MEM(1024, 1), ans(cycles,vector<int>(5,0)) {
        L1 = {0, 0};
        L2 = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
        L3 = {0, 0, 0, 0, 0, 0, 0, 0};
        L4 = {0, 0, 0, 0, 0};
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
        v[pc]=binary;
        L1.pc = pc;
        return 1;
    }

    int IDSTAGE() {
        L2.pc=L1.pc;
        if(L2.branch){
            cout<<"IFf"<<endl;
            IFSTAGE(L1.pc);
            L2.rd=0;
            (L1.pc)++;
            pc++;
            L2.branch=0;
        }
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
            if (L3.regwrite == 1 && (L3.rd == L2.rs1 || L3.rd == L2.rs2)) {
                L1.ON = 0;
                return 2;
            } else if (L4.regwrite == 1 && (L4.rd == L2.rs1 || L4.rd == L2.rs2)) {
                L1.ON = 0;
                return 1;
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
            
            L2.rd = -1;
            if (f3 == "100") { // BLT
                if (REG[L2.rs1] < REG[L2.rs2]) {
                    L2.branch = imm_val;
                }
            } else if (f3 == "101") { // BGE
                if (REG[L2.rs1] >= REG[L2.rs2]) {
                    L2.branch = imm_val;
                }
            } else if (f3 == "000") { // BEQ
                if (REG[L2.rs1] == REG[L2.rs2]) {
                    L2.branch = imm_val;
                }
            } else if (f3 == "001") { // BNE
                if (REG[L2.rs1] != REG[L2.rs2]) {
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
        if(L2.branch){
            L2.rd=0;
            L3.rd=0;
            return;
        }
        L3.rd = L2.rd;
        L3.rs2=L2.rs2;
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
            L3.result = REG[L2.rs1] + imm;
        }
    }

    void MEMSTAGE() {
        if(L2.branch){
            L3.rd=0;
            L2.rd=0;
            return;
        }
        L4.rd = L3.rd;
        L4.rs2=L3.rs2;
        L3.rs2=0;
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
               L4.rs2 >= 0 && L4.rs2 < 32) {
                MEM[L3.result] = REG[L4.rs2];
                cout<<L3.result<<" "<<L4.rs2;
            }
            L4.res = nullptr;
        } else {
            L4.res = &L3.result;
        }
        
    }

    void WBSTAGE() {
        if(L2.branch){
            L3.rd=0;
            L2.rd=0;
            return;
        }
        if (L4.regwrite && L4.rd >= 0 && L4.rd < 32 && L4.res != nullptr) {
            REG[L4.rd] = *L4.res;
        }
        L4.regwrite=0;
        L4.rd=0;
    }
    void printpipeline(){
        vector<pair<int,vector<int>>> fin;
        vector<vector<int>> vis(Cycles,vector<int>(5,0));
        for(int i=0;i<Cycles;i++){
            if(vis[i][4]){
                continue;
            }
            if(ans[i][0]==0){
                break;
            }
            int pt=i;
            int j=4;
            pair<int,vector<int>> temp;
            temp.first=i;
            while(j>=0 && pt<Cycles){
                if(j<=2){
                    temp.second.push_back(5-j);
                    vis[pt][j]=1;
                    j--;
                    pt++;
                }else if(j==3){
                    if(pt + 1 < Cycles && ans[pt+1][2]==1){
                        temp.second.push_back(2);
                        vis[pt][j]=1;
                        j--;
                        pt++;
                    }else{
                        temp.second.push_back(2);
                        vis[pt][j]=1;
                        pt++;
                    }
                }else{
                    if(pt + 1 < Cycles && vis[pt+1][3]==0){
                        temp.second.push_back(1);
                        vis[pt][j]=1;
                        j--;
                        pt++;
                    }else{
                        temp.second.push_back(1);
                        vis[pt][j]=1;
                        pt++;
                    }
                }
            }
            fin.push_back(temp);
        }
        map<int,string> mpp={{1,"IF"},{2,"ID"},{3,"EX"},{4,"MEM"},{5,"WB"}};
        for(auto e:fin){
            for(int i=1;i<=e.first;i++){
                cout<<"; ";
            }
            int k=0;
            for(auto f:e.second){
                if(f==k){
                    cout<<";-";
                }else{
                    cout<<";"<<mpp[f];
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
        L0 = 1;
        int k=-1;
        int l=0;
        int ll=0;
        v.resize(instructions.size(), "");
        for(int cycle = 0; cycle < Cycles; cycle++) {
            if(L4.ON) {
                cout << "WB ";
                ans[cycle][4]=1;
                WBSTAGE();
                if(l==-1){
                    L4.ON=0;
                }
            }else{
                cout<<"- ";
            }
            if(L3.ON) {
                cout << "MEM ";
                MEMSTAGE();
                ans[cycle][3]=1;
                L4.ON = 1;
                if(l==-1){
                    L3.ON=0;
                }
            } else {
                cout<<"- ";
                L4.ON = 0;
            }
            if(L2.ON) {
                cout << "EX ";
                ans[cycle][2]=1;
                EXSTAGE();
                L3.ON = 1;
                if(l==-1){
                    ll--;
                    L2.ON=0;
                }
            } else {
                cout<<"- ";
                L3.ON = 0;
            }
            if(ll>-2 && k!=-1 && !(L3.regwrite == 1 && (L3.rd == L2.rs1 || L3.rd == L2.rs2))||(L4.regwrite == 1 && (L4.rd == L2.rs1 || L4.rd == L2.rs2))){
                L1.ON=1;
            }
            if(L1.ON) {
                cout << "ID ";
                ans[cycle][1]=1;
                k = IDSTAGE();
                L0=1;
                if(l==-1 && k==0){
                    L1.ON=0;
                    ll--;
                }
            }else{
                L2.ON=0;
                if(k!=-1){
                    L0=0;
                }
                cout<<"- ";
            }
            if(L0 && pc < (int)instructions.size()) {
                if(!L2.branch){
                    cout <<"IF ";
                }else{
                    cout<<"IFb ";
                }

                ans[cycle][0]=1;
                int x=IFSTAGE(pc);
                if(x==1){
                    if(L2.branch){
                        pc--;
                        pc+=(L2.branch/4);
                        L1.pc=pc;
                    }
                    else{
                        pc++;
                    }
                    L1.ON=1;
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
            } else if(ll!=-3){
                L2.ON=1;
            }else{
                L2.ON=0;
            }
            cout << endl;
        }
        printpipeline();
        cout<<REG[0]<<" "<<REG[1]<<" "<<REG[2]<<" "<<REG[3]<<" "<<REG[4]<<" "<<MEM[1];
    }
};

int main() {
    vector<string> instructions = {
        "00022083", // lw x1, 0(x4)
        "00208113",  // addi x2, x1, 2   (depends on the value loaded into x1)
        "00400193", // addi x3, x0, 4   (dummy instruction; does not use x1)
    };

    NFProcessor processor(instructions, 20);
    processor.run();
    return 0;
}