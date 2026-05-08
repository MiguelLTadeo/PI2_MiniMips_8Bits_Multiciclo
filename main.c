#include <stdio.h>
#include <unistd.h>
#include "mips8bit.h"
#include <string.h>

int main(){

    int op = 0;

    memoria_instrucao mem_inst;

    multiciclo cpu;

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
        printf("1 - Carregar memória de instruções (.mem)\n");
        printf("2 - Carregar memória de Dados (.dat)\n");
        printf("3 - Imprimir memórias (instruções e dados)\n");
        printf("4 - Imprimir banco de registradores\n");
        printf("5 - Imprimir todo o simulador (registradores e memórias)\n");
        printf("6 - Salvar .asm\n");
        printf("7 - Salvar .dat\n");
        printf("8 - Executa Programa (run)\n");
        printf("9 - Executa uma instrução (Step)\n");
        printf("10 - Volta uma instrução (Back)\n");

        scanf("%d", &op);

        getchar();

        switch (op){
            case 1:
                lerMemoria(&mem_inst);
                //int contaLinhas = carregarMemoriaInstrucao(mem_instrucao); //carrega instruções e retornar número de instruções
            break;
            case 2:
                imprimirDetalhesInstrucoes(&mem_inst);
                //carregarMemoriaDados(&mem_dados);
            break;
            case 3:
                imprimirMemoria(&mem_inst);
                //imprimirMemoriaInstrucao(mem_instrucao, 256);
                //imprimirMemoriaDados(mem_dados);
            break;
            case 4:
                //imprimeBancoRegs(regs);

                printf("\nIniciando Simulacao...\n");
                // Passamos o endereco da CPU e da memoria
                simular(&cpu, &mem_inst); 
                break;
            break;
            case 5:
                clock(&cpu, &mem_inst);
               // printaSimulador(regs, &mem_dados, mem_instrucao);
            break;
            case 6:
                //salvarAsm(mem_instrucao, contaLinhas);
            break;
            case 7:
                //salvarMemoriaDados(mem_dados);
            break;
            case 8:
                // while (regs.pc<256){// passa pelos 256 endereços de instrução
                //     printf("==================\n");
                //     printf("PC == %d \n\n", regs.pc);
                //     i = converterInstrucao(mem_instrucao[regs.pc]);//converte a linha da matriz mem_instrução em uma struct instrução
                //     regs = controleMiniMips(i, regs, &mem_dados);//executa a instrução
                //     imprimeBancoRegs(regs);
                //     printaInstrucaoAsm(i);
                //     printf("==================\n\n");
                //     sleep(1);
                // }       
            break;
            case 9:
                // if (mem_instrucao[regs.pc][0] == '\0') { //verifica se a memória de instruções está vazia
                //     printf("Aviso: A instrução atual está vazia. O programa terminou ou não foi carregado.\n");
                // } else if(topo<256) {

                //     historico[topo].hist_banco = regs; // salva registradores
                //     historico[topo].hist_dados = mem_dados; // salva memória de dados
                //     topo++; //adiciona 1 ao topo para a proxima instrução

                //     i = converterInstrucao(mem_instrucao[regs.pc]); //converte a linha da matriz mem_instrução em uma struct instrução
                //     printaInstrucaoAsm(i); //executa a instrução
                //     printf("PC == %d \n", regs.pc);
                //     regs = controleMiniMips(i, regs, &mem_dados);

                // } else{
                //     printf("Limite de histórico atingido!\n");
                // }
            break;
            case 10:
                // if(topo == 0){ 
                //     printf("Nenhum estado anterior.\n");
                // }else{
                //     topo--; //volta para o topo anterior
                //     regs = historico[topo].hist_banco; //guarda o banco anterior no banco de registradores
                //     mem_dados = historico[topo].hist_dados; //guarda a memória anterior na memória de dados
                //     printf("Voltou uma instrução. PC e Memória de Dados restaurados. PC atual: %d\n", regs.pc);
                // }

            break;
            default:

            break;
        }
    }while(op!=12);

    return 0;
}