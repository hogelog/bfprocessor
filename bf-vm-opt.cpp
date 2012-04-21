#include <stdio.h>
#include <vector>
#include <stack>

#define MEMSIZE 30000

struct Instruction {
    char op;
    int value;
};
struct ExeCode {
    void *addr;
    int value;
};
class Compiler {
    private:
        std::vector<Instruction>* const insns;
        int incdec, move;
        std::stack<int> pcstack;
        std::vector<Instruction> stackinsn;
        void flush_op(char op) {
            if (is_current_op(op)) {
                insns->push_back(stackinsn.back());
                stackinsn.pop_back();
            }
        }
        bool is_current_op(char op) {
            return stackinsn.size() != 0 && stackinsn.back().op == op;
        }
        void check_reset_zero() {
            if (insns->size() < 3)
                return;
            Instruction c1 = insns->at(insns->size() - 3);
            Instruction c2 = insns->at(insns->size() - 2);
            Instruction c3 = insns->at(insns->size() - 1);
            if (c1.op != '[' || c2.op != 'c' || c2.value != -1 || c3.op != ']')
                return;
            insns->pop_back();
            insns->pop_back();
            insns->pop_back();
            push('Z', 0);
        }
        void push_stack(char op, int i) {
            if (is_current_op(op)) {
                stackinsn.back().value += i;
            } else {
                Instruction insn = {op, i};
                stackinsn.push_back(insn);
            }
        }
    public:
        Compiler(std::vector<Instruction>* insns) :
            insns(insns), incdec(0), move(0) {
        }
        void push_incdec(int i) {
            flush_op('m');
            push_stack('c', i);
        }
        void push_move(int i) {
            flush_op('c');
            push_stack('m', i);
        }
        void push(char op, int value) {
            Instruction insn = {op, value};
            insns->push_back(insn);
        }
        void push_simple(char op) {
            flush_op('m');
            flush_op('c');
            push(op, 0);
        }
        void push_open() {
            flush_op('m');
            flush_op('c');
            pcstack.push(insns->size());
            push('[', 0);
        }
        void push_close() {
            flush_op('m');
            flush_op('c');
            int open = pcstack.top();
            (*insns)[open].value = insns->size();
            push(']', open - 1);
            check_reset_zero();
            pcstack.pop();
        }
        void push_end() {
            push_simple('\0');
        }
};
void parse(std::vector<Instruction> &insns, FILE *input) {
    Compiler compiler(&insns);
    int ch = 0;
    while ((ch=getc(input)) != EOF) {
        switch (ch) {
            case '+':
                compiler.push_incdec(1);
                break;
            case '-':
                compiler.push_incdec(-1);
                break;
            case '>':
                compiler.push_move(1);
                break;
            case '<':
                compiler.push_move(-1);
                break;
            case ',':
            case '.':
                compiler.push_simple(ch);
                break;
            case '[':
                compiler.push_open();
                break;
            case ']':
                compiler.push_close();
                break;
        }
    }
    compiler.push_end();
}
void debug(std::vector<Instruction> &insns) {
    for (size_t pc=0;;++pc) {
        Instruction insn = insns[pc];
        switch(insn.op) {
            case '+':
            case '-':
            case '>':
            case '<':
            case ',':
            case '.':
            case 'Z':
                putchar(insn.op);
                break;
            case '[':
            case ']':
            case 'c':
            case 'm':
                putchar(insn.op);
                printf("%d", insn.value);
                break;
            case '\0':
                return;
        }
    }
}
void execute(std::vector<Instruction> &insns, int membuf[MEMSIZE]) {
    ExeCode exec[insns.size()];
    for (size_t pc=0;;++pc) {
        Instruction insn = insns[pc];
        exec[pc].value = insn.value;
        switch(insn.op) {
            case ',':
                exec[pc].addr = &&LABEL_GET;
                break;
            case '.':
                exec[pc].addr = &&LABEL_PUT;
                break;
            case '[':
                exec[pc].addr = &&LABEL_OPEN;
                break;
            case ']':
                exec[pc].addr = &&LABEL_CLOSE;
                break;
            case 'c':
                exec[pc].addr = &&LABEL_INCDEC;
                break;
            case 'm':
                exec[pc].addr = &&LABEL_MOVE;
                break;
            case 'Z':
                exec[pc].addr = &&LABEL_RESET;
                break;
            case '\0':
                exec[pc].addr = &&LABEL_END;
                goto LABEL_START;
        }
    }
LABEL_START:
    int *mem = membuf;
    int pc = -1;
    ExeCode ecode;

#define NEXT_LABEL \
    ecode = exec[++pc]; \
    goto *ecode.addr

    NEXT_LABEL;
LABEL_GET:
    *mem = getchar();
    NEXT_LABEL;
LABEL_PUT:
    putchar(*mem);
    NEXT_LABEL;
LABEL_OPEN:
    if (*mem == 0) {
        pc = ecode.value;
    }
    NEXT_LABEL;
LABEL_CLOSE:
    pc = ecode.value;
    NEXT_LABEL;
LABEL_INCDEC:
    *mem += ecode.value;
    NEXT_LABEL;
LABEL_MOVE:
    mem += ecode.value;
    NEXT_LABEL;
LABEL_RESET:
    *mem = 0;
    NEXT_LABEL;
LABEL_END:
    ;
}
int main() {
    static int membuf[MEMSIZE];
    std::vector<Instruction> insns;
    parse(insns, stdin);
//    debug(insns);
    execute(insns, membuf);
    return 0;
}
