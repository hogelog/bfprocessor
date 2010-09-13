#include <stdio.h>
#include <string>

#define MEMSIZE 30000

void parse(std::string& codes, FILE *input) {
    int ch;
    while ((ch=getc(input)) != EOF) {
        switch (ch) {
            case '+':
            case '-':
            case '>':
            case '<':
            case ',':
            case '.':
            case '[':
            case ']':
                codes.push_back(ch);
                break;
        }
    }
}
void execute(std::string& codes, int membuf[MEMSIZE]) {
    int *mem =  membuf;
    int depth = 0;
    for (size_t pc=0;pc<codes.size();++pc) {
        switch(codes[pc]) {
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
                    depth = 1;
                    while (depth != 0) {
                        switch(codes[++pc]) {
                            case '[':
                                ++depth;
                                break;
                            case ']':
                                --depth;
                                break;
                        }
                    }
                    break;
                }
                break;
            case ']':
                depth = -1;
                while (depth != 0) {
                    switch(codes[--pc]) {
                        case '[':
                            ++depth;
                            break;
                        case ']':
                            --depth;
                            break;
                    }
                }
                --pc;
                break;
        }
    }
}
int main() {
    static int membuf[MEMSIZE];
    std::string codes;
    parse(codes, stdin);
    execute(codes, membuf);
    return 0;
}
