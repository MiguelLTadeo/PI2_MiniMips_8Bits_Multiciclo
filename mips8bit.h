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

typedef struct{
    instrucao inst[256];
    int tamanho;
}memoria_instrucao;

typedef struct{
    int reg[8];
    instrucao *IR;       //reg de instrucao
    int MDR;      //reg de dados
    int A, B;     //reg dos dados lidos do banco de regs
    int ULASaida; //saída da ula
    int pc;
    int flag_zero;
}Banco;

typedef struct{
    int flag_overflow;
    int resultado;
    int flag_zero;
}resultado_ula; 

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

typedef struct{
    int R;
    int I;
    int J;
    int Indef;
}estatisticas;

typedef struct{
    multiciclo hist_cpu;
    memoria_instrucao hist_mem;
    estatisticas stats;
}back_simulador;

void lerMemoria(memoria_instrucao *mInst);

void converterInstrucao(instrucao *nova_instrucao);

void carregarInstrucoes(memoria_instrucao *mInst, FILE *fp);

void imprimirMemoria(memoria_instrucao *mInst);

void imprimirDetalhesInstrucoes(memoria_instrucao mInst);

void ler_registradoresRs_Rt(multiciclo *cpu);

void escrever_registrador(multiciclo *cpu, int valor);

int executar_ula(multiciclo*cpu, int entrada_a, int entrada_b);

void executar_ula_com_mux(multiciclo *cpu);

void definir_sinais(multiciclo *cpu);

void executar_estado(multiciclo *cpu, memoria_instrucao *mem);

void transicionar_estado(multiciclo *cpu, int opcode);

void simular(multiciclo *cpu, memoria_instrucao *mem, estatisticas *stats, int ativaStats);

void printaInstrucaoAsm(instrucao inst);

void clock(multiciclo *cpu, memoria_instrucao *mem, estatisticas *stats, int ativaStats);

char *traduzEstado(int estado);

void imprimirRegistradores(multiciclo *cpu);

void salvarAsm(memoria_instrucao mInst);

void salvarMem(memoria_instrucao mInst);

void intParaBinario16(int valor, char *buffer);

void contabilizaEstatisticas(instrucao inst,estatisticas *stats);

void imprimirEstatisticas(estatisticas stats);
