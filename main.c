#include <stdio.h>
#include <unistd.h>
#include "mips8bit.h"
#include <string.h>

int main(){

    int op = 0;

    memoria_instrucao mem_inst;

    multiciclo cpu;

    cpu.estado = 0;
    cpu.total_clocks = 0;

    // Zera a CPU e a memória logo ao abrir o programa
    memset(&cpu, 0, sizeof(multiciclo));
    for(int i = 0; i < 256; i++) {
        strcpy(mem_inst.inst[i].inst_char, "0000000000000000");
        mem_inst.inst[i].dados = 0;
    }
    
    //memoria_dados mem_dados;

    //Banco regs;

    //instrucao i;

    //regs = limpaLixoBancoRegs(regs);

    //mem_dados = limpaLixoDados(mem_dados);

    //back_simulador historico[256];
    //limpaLixoInstrucoes(mem_instrucao);

    //int topo = 0;

    do{
        printf("1 - Carregar memória (.mem)\n");
        printf("3 - Imprimir memórias (instruções e dados)\n");
        printf("4 - Imprimir banco de registradores\n");
        printf("5 - Imprimir todo o simulador (registradores e memórias)\n");
        printf("6 - Salvar .asm\n");
        printf("7 - Salvar .dat\n");
        printf("8 - Executar Programa (run)\n");
        printf("9 - Dar um clock (Step)\n");
        printf("10 - Volta uma instrução (Back)\n");

        scanf("%d", &op);

        getchar();

        switch (op){
            case 1:
                lerMemoria(&mem_inst);
            break;
            case 2:
                imprimirDetalhesInstrucoes(mem_inst);
            break;
            case 3:
                imprimirMemoria(&mem_inst);

            break;
            case 4:


            break;
            case 5:

            break;
            case 6:

            break;
            case 7:

            break;
            case 8:
                printf("\nIniciando Simulacao...\n");
                imprimirDetalhesInstrucoes(mem_inst);
                simular(&cpu, mem_inst); 
                break;

                
            break;
            case 9:
                clock(&cpu, mem_inst);
            break;
            case 10:

            
            break;
            default:

            break;
        }
    }while(op!=12);

    return 0;
}