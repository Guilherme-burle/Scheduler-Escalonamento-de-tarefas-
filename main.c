#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_TAREFAS 100
#define TAM_NOME 50

typedef struct {
    char nome[TAM_NOME];
    int periodo;
    int deadline;
    int burst;
} Tarefa;

int validar_tarefa(const Tarefa *tarefa)
{
    if (tarefa->periodo <= 0 ||
        tarefa->deadline <= 0 ||
        tarefa->burst <= 0) {
        return 0;
    }

    if (tarefa->burst > tarefa->deadline) {
        return 0;
    }

    if (tarefa->deadline > tarefa->periodo) {
        return 0;
    }

    return 1;
}

const char *nome_saida(const char *algoritmo)
{
    if (strcmp(algoritmo, "rate") == 0) {
        return "rate_gbm.out";
    }

    return "edf_gbm.out";
}

int main(int argc, char *argv[])
{
    if (argc != 3) {
        fprintf(stderr,
                "Erro: uso correto: ./scheduler <rate|edf> <arquivo>\n");
        return 1;
    }

    if (strcmp(argv[1], "rate") != 0 &&
        strcmp(argv[1], "edf") != 0) {
        fprintf(stderr,
                "Erro: algoritmo deve ser 'rate' ou 'edf'.\n");
        return 1;
    }

    FILE *arquivo = fopen(argv[2], "r");

    if (arquivo == NULL) {
        fprintf(stderr,
                "Erro: nao foi possivel abrir o arquivo '%s'.\n",
                argv[2]);
        return 1;
    }

    int tempo_total;

    if (fscanf(arquivo, "%d", &tempo_total) != 1) {
        fprintf(stderr,
                "Erro: tempo total de simulacao invalido.\n");

        fclose(arquivo);
        return 1;
    }

    if (tempo_total <= 0) {
        fprintf(stderr,
                "Erro: tempo total deve ser positivo.\n");

        fclose(arquivo);
        return 1;
    }

    Tarefa tarefas[MAX_TAREFAS];
    int quantidade_tarefas = 0;

    while (quantidade_tarefas < MAX_TAREFAS) {

        Tarefa tarefa;

        int resultado = fscanf(
            arquivo,
            "%49s %d %d %d",
            tarefa.nome,
            &tarefa.periodo,
            &tarefa.deadline,
            &tarefa.burst
        );

        if (resultado == EOF) {
            break;
        }

        if (resultado != 4) {
            fprintf(stderr,
                    "Erro: tarefa malformada no arquivo de entrada.\n");

            fclose(arquivo);
            return 1;
        }

        if (!validar_tarefa(&tarefa)) {
            fprintf(stderr,
                    "Erro: tarefa '%s' possui valores invalidos.\n",
                    tarefa.nome);

            fclose(arquivo);
            return 1;
        }

        tarefas[quantidade_tarefas] = tarefa;
        quantidade_tarefas++;
    }

    if (quantidade_tarefas == MAX_TAREFAS) {
        Tarefa extra;

        if (fscanf(
                arquivo,
                "%49s %d %d %d",
                extra.nome,
                &extra.periodo,
                &extra.deadline,
                &extra.burst
            ) == 4) {

            fprintf(stderr,
                    "Erro: numero de tarefas excede o limite de %d.\n",
                    MAX_TAREFAS);

            fclose(arquivo);
            return 1;
        }
    }

    fclose(arquivo);

    const char *arquivo_saida = nome_saida(argv[1]);

    FILE *saida = fopen(arquivo_saida, "w");

    if (saida == NULL) {
        fprintf(stderr,
                "Erro: nao foi possivel criar o arquivo de saida.\n");
        return 1;
    }

    fprintf(saida, "SIMULACAO\n");
    fprintf(saida, "ALGORITMO: %s\n", argv[1]);
    fprintf(saida, "TEMPO TOTAL: %d\n", tempo_total);
    fprintf(saida, "TAREFAS: %d\n", quantidade_tarefas);

    for (int i = 0; i < quantidade_tarefas; i++) {
        fprintf(
            saida,
            "%s %d %d %d\n",
            tarefas[i].nome,
            tarefas[i].periodo,
            tarefas[i].deadline,
            tarefas[i].burst
        );
    }

    fclose(saida);

    return 0;
}