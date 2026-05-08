#include <stdio.h>

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
    int dados;
} instrucao;

typedef struct {
    instrucao inst[256];
    int tamanho;
}memoria_instrucao;

typedef struct {
    int reg[8];
    int IR;      
    int MDR;      
    int A, B;     
    int ULASaida; 
    int Zero; 
    int pc;
}Banco;

typedef struct{
    int flag_overflow;
    int resultado;
    int flag_zero;
}resultado_ula; 

typedef struct{
    Banco hist_banco;
}back_simulador;

typedef struct {
    int PCEsc;       // 1 bit
    int IouD;        // 1 bit
    int EscMem;      // 1 bit
    int IREsc;       // 1 bit
    int MemParaReg;  // 1 bit
    int EscReg;      // 1 bit
    int ULAFonteB;   // 2 bits
    int ULAFonteA;   // 1 bit
    int ControleUla; // 3 bits
    int FontePC;     // 2 bits
    int RegDst;      // 1 bit
    int Branch;      // 1 bit
}sinais_ctrl;

typedef struct {                // 8 bits
    Banco banco_regs;       // 8 registradores e regs especificos
    sinais_ctrl sinais;      // sinais de controle
    int estado;              // estado atual da FSM
    int total_clocks;        // contador de clocks
}multiciclo;

void lerMemoria(memoria_instrucao *mInst);

void carregarInstrucoes(memoria_instrucao *mInst, FILE *fp);

void imprimirMemoria(memoria_instrucao *mInst);

void imprimirDetalhesInstrucoes(memoria_instrucao *mInst);

void ler_registradores(multiciclo *cpu);

void escrever_registrador(multiciclo *cpu, int valor);

int executar_ula(multiciclo*cpu, int entrada_a, int entrada_b);

void executar_ula_com_mux(multiciclo *cpu);

