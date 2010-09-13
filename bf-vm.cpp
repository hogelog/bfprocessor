#include <stdio.h>
#include <vector>
#include <stack>

#define MEMSIZE 30000

struct Instruction {
    char op;
    int jmp;
};
void parse(std::vector<Instruction> &codes, FILE *input) {
    int ch = 0;
    std::stack<int> pcstack;
    Instruction code;
    while ((ch=getc(input)) != EOF) {
        code.op = ch;
        switch (ch) {
            case '+':
            case '-':
            case '>':
            case '<':
            case ',':
            case '.':
                codes.push_back(code);
                break;
            case '[':
                pcstack.push(codes.size());
                codes.push_back(code);
                break;
            case ']':
                int begin = pcstack.top();
                pcstack.pop();
                codes[begin].jmp = codes.size();
                code.jmp = begin - 1;
                codes.push_back(code);
                break;
        }
    }
    code.op = '\0';
    codes.push_back(code);
}
void execute(std::vector<Instruction> &codes, int membuf[MEMSIZE]) {
    int *mem = membuf;
    for (size_t pc=0;;++pc) {
        Instruction code = codes[pc];
        switch(code.op) {
            case '+':
                ++*mem;
                break;
            case '-':
                --*mem;
                break;
            case '>':
                ++mem;
                break;
            case '<':
                --mem;
                break;
            case ',':
                *mem = getchar();
                break;
            case '.':
                putchar(*mem);
                break;
            case '[':
                if (*mem == 0) {
                    pc = code.jmp;
                }
                break;
            case ']':
                pc = code.jmp;
                break;
            case '\0':
                return;
        }
    }
}
int main() {
    static int membuf[MEMSIZE];
    std::vector<Instruction> codes;
    parse(codes, stdin);
    execute(codes, membuf);
    return 0;
}
