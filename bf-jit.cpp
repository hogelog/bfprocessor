#include <stdio.h>
#include <string>
#include <sstream>
#include <vector>
#include <stack>

#define MEMSIZE 30000
#define CODESIZE 50000

#include "xbyak/xbyak.h"

char* toLabel(char ch, int num) {
    static char labelbuf[BUFSIZ];
    snprintf(labelbuf, sizeof(labelbuf), "%c%d", ch, num);
    return labelbuf;
}
void parse(Xbyak::CodeGenerator &gen, FILE *input, int membuf[MEMSIZE]) {
    gen.push(gen.ebx);

    Xbyak::Reg32 memreg = gen.ebx;
    Xbyak::Reg32 eax = gen.eax;
    Xbyak::Address mem = gen.dword[memreg];

    gen.mov(memreg, (int) membuf);

    std::stack<int> labelStack;
    int labelNum = 0;
    std::string label;
    int ch;
    while((ch=getc(input)) != EOF) {
        switch (ch) {
            case '+':
                gen.inc(mem);
                break;
            case '-':
                gen.dec(mem);
                break;
            case '>':
                gen.add(memreg, 4);
                break;
            case '<':
                gen.add(memreg, -4);
                break;
            case ',':
                gen.call((void*) getchar);
                gen.mov(mem, gen.eax);
                break;
            case '.':
                gen.push(mem);
                gen.call((void*) putchar);
                gen.pop(gen.eax);
                break;
            case '[':
                gen.L(toLabel('L', labelNum));
                gen.mov(gen.eax, mem);
                gen.test(gen.eax, gen.eax);
                gen.jz(toLabel('R', labelNum), Xbyak::CodeGenerator::T_NEAR);

                labelStack.push(labelNum);
                ++labelNum;
                break;
            case ']':
                int beginNum = labelStack.top();
                labelStack.pop();

                gen.jmp(toLabel('L', beginNum), Xbyak::CodeGenerator::T_NEAR);
                gen.L(toLabel('R', beginNum));
                break;
        }
    }
    gen.pop(gen.ebx);
    gen.ret();
}
void execute(Xbyak::CodeGenerator &gen) {
    void (*codes)() = (void (*)()) gen.getCode();
    codes();
}
int main() {
    static int membuf[MEMSIZE];
    Xbyak::CodeGenerator gen(CODESIZE);
    parse(gen, stdin, membuf);
    execute(gen);
    return 0;
}
