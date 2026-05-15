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

    back_simulador historico[256];
    int topo = 0;

    // Zera a CPU e a memória logo ao abrir o programa
    memset(&cpu, 0, sizeof(multiciclo));
    for(int i = 0; i < 256; i++) {
        strcpy(mem_inst.inst[i].inst_char, "0000000000000000");
        converterInstrucao(&mem_inst.inst[i]);
    }
    mem_inst.tamanho = 0;

    // IR aponta pra uma instrução válida (mesmo que zerada)
    cpu.banco_regs.IR = &mem_inst.inst[0];

    estatisticas stats;

    stats.I=0;
    stats.R=0;
    stats.J=0;
    stats.Indef=0;
    int ativaStats=0;
    
    //memoria_dados mem_dados;

    //Banco regs;

    //instrucao i;

    //regs = limpaLixoBancoRegs(regs);

    //mem_dados = limpaLixoDados(mem_dados);

    //back_simulador historico[256];
    //limpaLixoInstrucoes(mem_instrucao);

    //int topo = 0;

    do{
        printf("-----------------------------------------------------------\n");
        printf("1 - Carregar memória (.mem)\n");
        printf("2 - Imprimir instruções\n");
        printf("3 - Imprimir memória(dados e intruções)\n");
        printf("4 - Imprimir banco de registradores\n");
        printf("5 - Imprimir todo o simulador (registradores e memórias)\n");
        printf("6 - Salvar .asm\n");
        printf("7 - Salvar .dat\n");
        printf("8 - Executar Programa (run)\n");
        printf("9 - Dar um clock (Step)\n");
        printf("10 - Volta uma instrução (Back)\n");
        printf("11 - Ativar estatísticas\n");
        printf("12 - Fechar programa\n");
        printf("-----------------------------------------------------------\n");
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
                imprimirRegistradores(&cpu);

            break;
            case 5:
                imprimirMemoria(&mem_inst);
                imprimirRegistradores(&cpu);
            break;
            case 6:
                salvarAsm(mem_inst);
            break;
            case 7:
                salvarMem(mem_inst);
            break;
            case 8:
                printf("\nIniciando Simulacao...\n");
                imprimirDetalhesInstrucoes(mem_inst);
                simular(&cpu, &mem_inst, &stats, ativaStats); 
                break;

            break;
            case 9:  // step — salvar hist antes de dar o clock
                if(topo < 256){
                    historico[topo].hist_cpu = cpu;
                    historico[topo].hist_mem = mem_inst;
                    topo++;
                    clock(&cpu, &mem_inst, &stats, ativaStats);
                }else{
                    printf("Limite de histórico atingido!\n");
                }
            break;
            case 10:  // back
                if(topo == 0){
                    printf("Nenhum estado anterior.\n");
                }else{
                    topo--;
                    cpu = historico[topo].hist_cpu;
                    mem_inst = historico[topo].hist_mem;
                    printf("Voltou um clock. Estado atual: %s | PC: %d | Clocks: %d\n",
                        traduzEstado(cpu.estado), cpu.banco_regs.pc, cpu.total_clocks);
                }
            break;
            case 11:
                if(ativaStats==0){
                    printf("LIGADO!\n");
                    ativaStats=1;
                }else{
                    printf("DESLIGADO!\n");
                    ativaStats=0;
                }
            break;
            default:

            break;
        }
    }while(op!=12);

    return 0;
}
