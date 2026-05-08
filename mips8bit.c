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
    int dados;
} instrucao;

typedef struct {
    instrucao inst[256];
    int tamanho;
}memoria_instrucao;

typedef struct {
    int reg[8];
    instrucao IR;       //reg de instrucao
    int MDR;      //reg de dados
    int A, B;     //reg dos dados lidos do banco de regs
    int ULASaida; //saída da ula
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

void converterInstrucao(instrucao *nova_instrucao) {
    char opcode[5], rs[4], rt[4], rd[4], funct[4], imm[7], addr[13];

    // Extrai o Opcode (primeiros 4 bits)
    strncpy(opcode, nova_instrucao->inst_char, 4);
    opcode[4] = '\0';
    nova_instrucao->opcode = (int)strtol(opcode, NULL, 2);
    
    switch(nova_instrucao->opcode) {
        case 0: // Tipo-R (add, sub, and, or)
            nova_instrucao->tipo_inst = tipo_R;
            
            // RS (bits 4 a 6)
            strncpy(rs, nova_instrucao->inst_char + 4, 3);
            rs[3] = '\0';
            nova_instrucao->rs = (int)strtol(rs, NULL, 2);

            // RT (bits 7 a 9)
            strncpy(rt, nova_instrucao->inst_char + 7, 3);
            rt[3] = '\0';
            nova_instrucao->rt = (int)strtol(rt, NULL, 2);

            // RD (bits 10 a 12)
            strncpy(rd, nova_instrucao->inst_char + 10, 3);
            rd[3] = '\0';  
            nova_instrucao->rd = (int)strtol(rd, NULL, 2);

            // FUNCT (bits 13 a 15)
            strncpy(funct, nova_instrucao->inst_char + 13, 3);
            funct[3] = '\0';
            nova_instrucao->funct = (int)strtol(funct, NULL, 2);
            break;

        case 4:  // addi
        case 11: // lw
        case 15: // sw
        case 8:  // beq
            nova_instrucao->tipo_inst = tipo_I;

            strncpy(rs, nova_instrucao->inst_char + 4, 3);
            rs[3] = '\0';
            nova_instrucao->rs = (int)strtol(rs, NULL, 2);

            strncpy(rt, nova_instrucao->inst_char + 7, 3);
            rt[3] = '\0';
            nova_instrucao->rt = (int)strtol(rt, NULL, 2);

            // Imediato (6 bits: 10 a 15)
            strncpy(imm, nova_instrucao->inst_char + 10, 6);
            imm[6] = '\0';
            nova_instrucao->imm = (int)strtol(imm, NULL, 2);

            // Extensão de sinal (Complemento de 2 para 6 bits)
            if (nova_instrucao->inst_char[10] == '1') {
                nova_instrucao->imm -= 64; 
            }
            break;

        case 2: // Tipo-J (jump)
            nova_instrucao->tipo_inst = tipo_J;
            strncpy(addr, nova_instrucao->inst_char + 4, 12); 
            addr[12] = '\0'; 
            nova_instrucao->addr = (int)strtol(addr, NULL, 2);
            break;
    }
}

void carregarInstrucoes(memoria_instrucao *mInst, FILE *fp) {
    char linha[32];
    
    for(int i = 0; i < 256; i++) {
        strcpy(mInst->inst[i].inst_char, "0000000000000000");
        mInst->inst[i].dados = 0;
    }

    mInst->tamanho = 0;

    while (mInst->tamanho < 256 && fgets(linha, sizeof(linha), fp) != NULL) {
        
        linha[strcspn(linha, "\r\n")] = 0;

        if (strcmp(linha, ".data") == 0){
            mInst->tamanho=128;
            continue;
        }

        if (mInst->tamanho > 127) {
            int indice;
            char valorBin[20];

            if (sscanf(linha, "%d:%s", &indice, valorBin) == 2) {
                if (indice >= 0 && indice < 256) {
                        strncpy(mInst->inst[indice].inst_char, valorBin, 16);
                        mInst->inst[indice].inst_char[16] = '\0';
                        mInst->inst[indice].dados = (int)strtol(valorBin, NULL, 2);
                }
            }
            else{
                strncpy(mInst->inst[mInst->tamanho].inst_char, linha, 16);
                mInst->inst[mInst->tamanho].inst_char[16] = '\0';
                mInst->inst[mInst->tamanho].dados = (int)strtol(linha, NULL, 2);
            }
        }else{
            strncpy(mInst->inst[mInst->tamanho].inst_char, linha, 16);
            mInst->inst[mInst->tamanho].inst_char[16] = '\0';
            converterInstrucao(&mInst->inst[mInst->tamanho]);
        }

        mInst->tamanho++;
    }
}


void lerMemoria(memoria_instrucao *mInst){
    char arquivo[100];

    printf("Digite o nome do arquivo (sem o .dat): \n");
    scanf("%94s", arquivo); 

    strcat(arquivo, ".mem");

    FILE *fp = fopen(arquivo, "r");

    if (fp == NULL) {
        printf("Erro ao abrir arquivo de dados!\n");
        return;
    }

    printf("Carregando memoria a partir de '%s'...\n", arquivo);
    carregarInstrucoes(mInst, fp);

    fclose(fp);
    printf("Memoria carregada com sucesso!\n");
}


void imprimirMemoria(memoria_instrucao *mInst) {
    printf("\n============================================================================================\n");
    printf("                                            MEMORIA                                         \n");
    printf("============================================================================================\n");
    

    printf(" [ SECAO DE INSTRUCOES ]           |  [ SECAO DE DADOS (128-255) ]\n");
    printf(" End. | Binario                    |  End. | Binario           | Valor Dec.\n");
    printf(" -----|----------------------------|-------|-------------------|------------\n");

    for (int i = 0; i < 128; i++) {
        int idxDado = i + 128; 


        printf(" %03d  | %-26s | ", i, mInst->inst[i].inst_char);

        printf(" %03d  | %-17s | %-10d\n", idxDado, mInst->inst[idxDado].inst_char, mInst->inst[idxDado].dados);
    }

    printf("============================================================================================\n");
    printf("                                              FIM                                           \n");
    printf("============================================================================================\n\n");
}



void imprimirDetalhesInstrucoes(memoria_instrucao *mInst) {
    printf("\n===================================================================================================\n");
    printf("                                  DECODIFICACAO DE INSTRUCOES (0-127)                             \n");
    printf("===================================================================================================\n");
    printf(" End. | Binario          | Tipo | Op | Rs | Rt | Rd | Funct | Imm/Addr \n");
    printf(" -----|------------------|------|----|----|----|----|-------|----------\n");

    for (int i = 0; i < 128; i++) {
        // Só printamos se a instrução não for zero (ou se você quiser ver tudo, remova o if)
        if (strcmp(mInst->inst[i].inst_char, "0000000000000000") != 0) {
            
            char tipo;
            if (mInst->inst[i].tipo_inst == tipo_R) tipo = 'R';
            else if (mInst->inst[i].tipo_inst == tipo_I) tipo = 'I';
            else tipo = 'J';

            // Print formatado com larguras fixas para manter o alinhamento
            printf(" %03d  | %s |  %c   | %02d | %02d | %02d | %02d |   %d   | %d\n",
                   i, 
                   mInst->inst[i].inst_char,
                   tipo,
                   mInst->inst[i].opcode,
                   mInst->inst[i].rs,
                   mInst->inst[i].rt,
                   mInst->inst[i].rd,
                   mInst->inst[i].funct,
                   (tipo == 'J' ? mInst->inst[i].addr : mInst->inst[i].imm)
            );
        }
    }
    printf("===================================================================================================\n\n");
}

// Leitura do banco
// Slide 19: rs = RI[11-9], rt = RI[8-6], rd = RI[5-3]
void ler_registradores(multiciclo *cpu) {
    int rs = cpu->banco_regs.IR.rs;
    int rt = cpu->banco_regs.IR.rt;  // bits [8-6]
    
    cpu->banco_regs.A = cpu->banco_regs.reg[rs];
    cpu->banco_regs.B = cpu->banco_regs.reg[rt];
}

// Escrita no banco
// Controlada por EscReg e RegDst
void escrever_registrador(multiciclo *cpu, int valor) {
    if (cpu->sinais.EscReg) {
        int rt = (cpu->banco_regs.IR >> 6) & 0x7;  // bits [8-6]
        int rd = (cpu->banco_regs.IR >> 3) & 0x7;  // bits [5-3]
        
        // RegDst define qual registrador escrever
        // 0 = rt (tipo I), 1 = rd (tipo R)
        int reg_dest = (cpu->sinais.RegDst) ? rd : rt;
        
        cpu->banco_regs.reg[reg_dest] = valor;
    }
}

int executar_ula(multiciclo*cpu, int entrada_a, int entrada_b) {
    int resultado = 0;
    
    switch(cpu->sinais.ControleUla) {
        case 0b000: // Adição (add, addi, lw, sw, PC+1)
            resultado = entrada_a + entrada_b;
            break;
        case 0b010: // Subtração (sub, beq)
            resultado = entrada_a - entrada_b;
            break;
        case 0b100: // AND
            resultado = entrada_a & entrada_b;
            break;
        case 0b101: // OR
            resultado = entrada_a | entrada_b;
            break;
    }
    
    // Flag Zero (usado pelo beq)
    cpu->banco_regs.Zero = (resultado == 0) ? 1 : 0;
    
    // Limita a 8 bits
    resultado = resultado & 0xFF;
    
    return resultado;
}

void executar_ula_com_mux(multiciclo *cpu) {
    int entrada_a, entrada_b;
    
    // MUX ULAFonteA
    // 0 = PC, 1 = registrador A
    entrada_a = (cpu->sinais.ULAFonteA == 0) 
                ? cpu->banco_regs.pc
                : cpu->banco_regs.A;
    
    // MUX ULAFonteB (2 bits)
    // 00 = B, 01 = 1, 10 = SignExt(RI[5-0])
    switch(cpu->sinais.ULAFonteB) {
        case 0b00:
            entrada_b = cpu->banco_regs.B;
            break;
        case 0b01:
            entrada_b = 1;
            break;
        case 0b10:
            // Extensão de sinal de 6 bits para 8 bits
            // RI[5-0]
            int imm = cpu->banco_regs.IR & 0x3F;
            // Se bit 5 = 1, estende com 1s
            if (imm & 0x20) imm |= 0xC0;
            entrada_b = imm;
            break;
    }
    
    // Executa ULA e salva resultado
    cpu->banco_regs.ULASaida = executar_ula(cpu, entrada_a, entrada_b);
}

void definir_sinais(multiciclo *cpu) {
    // Zera todos os sinais primeiro
    cpu->sinais.PCEsc      = 0;
    cpu->sinais.IouD       = 0;
    cpu->sinais.EscMem     = 0;
    cpu->sinais.IREsc      = 0;
    cpu->sinais.MemParaReg = 0;
    cpu->sinais.EscReg     = 0;
    cpu->sinais.ULAFonteB  = 0;
    cpu->sinais.ULAFonteA  = 0;
    cpu->sinais.ControleUla= 0;
    cpu->sinais.FontePC    = 0;
    cpu->sinais.RegDst     = 0;
    cpu->sinais.Branch     = 0;
    
    switch(cpu->estado) {
        case 0: // Busca
            cpu->sinais.PCEsc      = 1;
            cpu->sinais.IouD       = 0;
            cpu->sinais.IREsc      = 1;
            cpu->sinais.ULAFonteB  = 0b01; // B = 1
            cpu->sinais.ULAFonteA  = 0;    // A = PC
            cpu->sinais.ControleUla= 0b000; // Adição
            cpu->sinais.FontePC    = 0b00;  // PC + 1
            cpu->sinais.RegDst     = 1;
            break;
            
        case 1: // Decodificação
            cpu->sinais.ULAFonteA  = 0;    // A = PC
            cpu->sinais.ULAFonteB  = 0b10; // B = SignExt
            cpu->sinais.ControleUla= 0b000; // Adição
            cpu->sinais.RegDst     = 1;
            break;
            
        case 2: // Cálculo endereço (lw/sw/addi)
            cpu->sinais.ULAFonteA  = 1;    // A = reg A
            cpu->sinais.ULAFonteB  = 0b10; // B = SignExt
            cpu->sinais.ControleUla= 0b000; // Adição
            break;
            
        case 3: // Acesso memória (lw lê)
            cpu->sinais.IouD       = 1;
            cpu->sinais.ULAFonteA  = 1;
            cpu->sinais.ULAFonteB  = 0b10;
            break;
            
        case 4: // Fim lw (escreve registrador)
            cpu->sinais.EscReg     = 1;
            cpu->sinais.MemParaReg = 1;
            cpu->sinais.RegDst     = 0;    // escreve em rt
            cpu->sinais.ULAFonteA  = 1;
            cpu->sinais.ULAFonteB  = 0b10;
            break;
            
        case 5: // sw (escreve memória)
            cpu->sinais.EscMem     = 1;
            cpu->sinais.IouD       = 1;
            cpu->sinais.ULAFonteA  = 1;
            cpu->sinais.ULAFonteB  = 0b10;
            break;
            
        case 6: // addi (escreve registrador)
            cpu->sinais.EscMem     = 0;
            cpu->sinais.EscReg     = 1;
            cpu->sinais.RegDst     = 0;    // escreve em rt
            cpu->sinais.MemParaReg = 0;
            cpu->sinais.ULAFonteA  = 1;
            cpu->sinais.ULAFonteB  = 0b10;
            break;
            
        case 7: // Execução tipo R
            cpu->sinais.ULAFonteA  = 1;    // A = reg A
            cpu->sinais.ULAFonteB  = 0b00; // B = reg B
            // ControleUla vem do Funct
            cpu->sinais.ControleUla= cpu->banco_regs.IR & 0x7; // RI[2-0]
            cpu->sinais.RegDst     = 1;
            break;
            
        case 8: // Fim tipo R (escreve registrador)
            cpu->sinais.RegDst     = 1;    // escreve em rd
            cpu->sinais.EscReg     = 1;
            cpu->sinais.MemParaReg = 0;
            break;
            
        case 9: // beq
            cpu->sinais.ULAFonteA  = 1;    // A = reg A
            cpu->sinais.ULAFonteB  = 0b00; // B = reg B
            cpu->sinais.ControleUla= 0b010; // Subtração
            cpu->sinais.Branch     = 1;
            cpu->sinais.PCEsc      = 0;
            cpu->sinais.FontePC    = 0b01;  // ULASaída
            break;
            
        case 10: // jump
            cpu->sinais.PCEsc      = 1;
            cpu->sinais.FontePC    = 0b10;  // RI[7-0]
            break;
    }
}

void executar_estado(multiciclo *cpu, memoria_instrucao mem) {
    // Define sinais do estado atual
    definir_sinais(cpu);
    
    // Endereço de memória (IouD)
    // 0 = PC, 1 = ULASaída
    int endereco_mem = (cpu->sinais.IouD == 0) 
                       ? cpu->banco_regs.pc
                       : cpu->banco_regs.ULASaida;
    
    switch(cpu->estado) {
        case 0: // Busca
            // IR = Mem[PC]
            cpu->banco_regs.IR = /* lê da sua memória */ ;
            // ULA calcula PC + 1
            executar_ula_com_mux(cpu);
            // PC = PC + 1
            cpu->banco_regs.pc = cpu->banco_regs.ULASaida;
            cpu->estado = 1;
            break;
            
        case 1: // Decodificação
            // Lê registradores
            ler_registradores(cpu);
            // ULA calcula endereço branch
            executar_ula_com_mux(cpu);
            // Salva em ULASaida para possível branch
            // Transição baseada no opcode
            int opcode = (cpu->inter.IR >> 12) & 0xF;
            transicionar_estado(cpu, opcode);
            break;
            
        case 2: // Cálculo endereço
            executar_ula_com_mux(cpu);
            int op = (cpu->inter.IR >> 12) & 0xF;
            if (op == 0xB) cpu->estado = 3;      // lw
            else if (op == 0xF) cpu->estado = 5; // sw
            else if (op == 0x4) cpu->estado = 6; // addi
            break;
            
        case 3: // lw lê memória
            cpu->inter.MDR = mem[cpu->inter.ULASaida].dado;
            cpu->estado = 4;
            break;
            
        case 4: // lw escreve registrador
            escrever_registrador(cpu, cpu->inter.MDR);
            cpu->estado = 0;
            break;
            
        case 5: // sw escreve memória
            mem[cpu->inter.ULASaida].dado = cpu->inter.B;
            cpu->estado = 0;
            break;
            
        case 6: // addi escreve registrador
            escrever_registrador(cpu, cpu->inter.ULASaida);
            cpu->estado = 0;
            break;
            
        case 7: // Execução tipo R
            executar_ula_com_mux(cpu);
            cpu->estado = 8;
            break;
            
        case 8: // Fim tipo R
            escrever_registrador(cpu, cpu->inter.ULASaida);
            cpu->estado = 0;
            break;
            
        case 9: // beq
            executar_ula_com_mux(cpu);
            // Lógica: PCEsc = Branch AND Zero
            if (cpu->ctrl.Branch && cpu->inter.Zero) {
                cpu->PC = cpu->inter.ULASaida;
            }
            cpu->estado = 0;
            break;
            
        case 10: // jump
            // PC = RI[7-0]
            cpu->PC = cpu->inter.IR & 0xFF;
            cpu->estado = 0;
            break;
    }
    
    // Incrementa contador de clocks
    cpu->total_clocks++;
}

// typedef struct {
//     int PC;                  // 8 bits
//     Banco banco_regs;       // 8 registradores e regs especificos
//     sinais_ctrl sinais;      // sinais de controle
//     int estado;              // estado atual da FSM
//     int total_clocks;        // contador de clocks
// }multiciclo;