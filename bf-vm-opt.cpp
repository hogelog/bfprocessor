#include <stdio.h>
#include <vector>
#include <stack>

#define MEMSIZE 30000

struct Instruction {
    char op;
    int value;
};
class Compiler {
    private:
        std::vector<Instruction>* const codes;
        int incdec, move;
        std::stack<int> pcstack;
        std::vector<Instruction> stackcode;
        void flushMove() {
            if (is_current_op('m')) {
                codes->push_back(stackcode.back());
                stackcode.pop_back();
            }
        }
        void flushIncdec() {
            if (is_current_op('c')) {
                codes->push_back(stackcode.back());
                stackcode.pop_back();
            }
        }
        bool is_current_op(char op) {
            return stackcode.size() != 0 && stackcode.back().op == op;
        }
        void push_stack(char op, int i) {
            if (is_current_op(op)) {
                stackcode.back().value += i;
            } else {
                Instruction code = {op, i};
                stackcode.push_back(code);
            }
        }
    public:
        Compiler(std::vector<Instruction>* codes) :
            codes(codes), incdec(0), move(0) {
        }
        void push_incdec(int i) {
            flushMove();
            push_stack('c', i);
        }
        void push_move(int i) {
            flushIncdec();
            push_stack('m', i);
        }
        void push(char op, int value) {
            Instruction code = {op, value};
            codes->push_back(code);
        }
        void push_simple(char op) {
            flushMove();
            flushIncdec();
            push(op, 0);
        }
        void push_open() {
            flushMove();
            flushIncdec();
            pcstack.push(codes->size());
            push('[', 0);
        }
        void push_close() {
            flushMove();
            flushIncdec();
            int open = pcstack.top();
            pcstack.pop();
            (*codes)[open].value = codes->size();
            push(']', open - 1);
        }
};
void parse(std::vector<Instruction> &codes, FILE *input) {
    Compiler optimizer(&codes);
    int ch = 0;
    while ((ch=getc(input)) != EOF) {
        switch (ch) {
            case '+':
                optimizer.push_incdec(1);
                break;
            case '-':
                optimizer.push_incdec(-1);
                break;
            case '>':
                optimizer.push_move(1);
                break;
            case '<':
                optimizer.push_move(-1);
                break;
            case ',':
            case '.':
                optimizer.push_simple(ch);
                break;
            case '[':
                optimizer.push_open();
                break;
            case ']':
                optimizer.push_close();
                break;
        }
    }
    optimizer.push_simple('\0');
}
void debug(std::vector<Instruction> &codes) {
    for (size_t pc=0;;++pc) {
        Instruction code = codes[pc];
        switch(code.op) {
            case '+':
            case '-':
            case '>':
            case '<':
            case ',':
            case '.':
                putchar(code.op);
                break;
            case '[':
            case ']':
            case 'c':
            case 'm':
                putchar(code.op);
                printf("%d", code.value);
                break;
            case '\0':
                return;
        }
    }
}
void execute(std::vector<Instruction> &codes, int membuf[MEMSIZE]) {
    int *mem = membuf;
    for (size_t pc=0;;++pc) {
        Instruction code = codes[pc];
        switch(code.op) {
            case ',':
                *mem = getchar();
                break;
            case '.':
                putchar(*mem);
                break;
            case '[':
                if (*mem == 0) {
                    pc = code.value;
                }
                break;
            case ']':
                pc = code.value;
                break;
            case 'c':
                *mem += code.value;
                break;
            case 'm':
                mem += code.value;
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
//    debug(codes);
    execute(codes, membuf);
    return 0;
}
