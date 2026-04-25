#include <stdio.h>
#include <string.h>
#include <stdlib.h>

typedef enum{
    tipo_I,
    tipo_J,
    tipo_R,
    tipo_OUTROS
} classe_inst;

typedef struct{
    classe_inst tipo_inst;
    char inst_char[17];
    int opcode;
    int rs;
    int rt;
    int rd;
    int funct;
    int imm;
    int addr;
} instrucao;

typedef struct {
    int dados[256];
    int tamanho;
}memoria_dados;

typedef struct {
    int reg[8];
    int pc;
}Banco;

typedef struct{
    int flag_overflow;
    int resultado;
    int flag_zero;
}resultado_ula; 

typedef struct{
    Banco hist_banco;
    memoria_dados hist_dados;
}back_simulador;

