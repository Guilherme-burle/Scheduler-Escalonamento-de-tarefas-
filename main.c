#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_TAREFAS 100
#define MAX_INSTANCIAS 10000
#define TAM_NOME 50

typedef struct {
    char nome[TAM_NOME];
    int periodo;
    int deadline;
    int burst;
    int ordem;
} Tarefa;

typedef struct {
    int tarefa;
    int chegada;
    int deadline;
    int restante;
    int concluida;
    int perdida;
    int executou;
} Instancia;

int validar_tarefa(const Tarefa *tarefa)
{
    if (tarefa->periodo <= 0 ||
        tarefa->deadline <= 0 ||
        tarefa->burst <= 0)
        return 0;

    if (tarefa->burst > tarefa->deadline)
        return 0;

    if (tarefa->deadline > tarefa->periodo)
        return 0;

    return 1;
}

int comparar_rate(const void *a, const void *b)
{
    const Tarefa *ta = a;
    const Tarefa *tb = b;

    if (ta->periodo != tb->periodo)
        return ta->periodo - tb->periodo;

    return ta->ordem - tb->ordem;
}

int criar_instancias(
    Tarefa tarefas[],
    int num_tarefas,
    int tempo_total,
    Instancia instancias[])
{
    int quantidade = 0;

    for (int i = 0; i < num_tarefas; i++) {
        for (int chegada = 0;
             chegada < tempo_total;
             chegada += tarefas[i].periodo) {

            if (quantidade >= MAX_INSTANCIAS)
                return quantidade;

            instancias[quantidade].tarefa = i;
            instancias[quantidade].chegada = chegada;
            instancias[quantidade].deadline =
                chegada + tarefas[i].deadline;
            instancias[quantidade].restante = tarefas[i].burst;
            instancias[quantidade].concluida = 0;
            instancias[quantidade].perdida = 0;
            instancias[quantidade].executou = 0;

            quantidade++;
        }
    }

    return quantidade;
}

int escolher_instancia_rate(
    Tarefa tarefas[],
    Instancia instancias[],
    int quantidade,
    int tempo)
{
    int escolhida = -1;

    for (int i = 0; i < quantidade; i++) {
        if (instancias[i].chegada > tempo)
            continue;

        if (instancias[i].concluida || instancias[i].perdida)
            continue;

        if (tempo >= instancias[i].deadline)
            continue;

        if (escolhida == -1) {
            escolhida = i;
            continue;
        }

        int tarefa_atual = instancias[i].tarefa;
        int tarefa_escolhida = instancias[escolhida].tarefa;

        if (tarefas[tarefa_atual].periodo <
            tarefas[tarefa_escolhida].periodo) {

            escolhida = i;
        } else if (
            tarefas[tarefa_atual].periodo ==
            tarefas[tarefa_escolhida].periodo &&
            tarefas[tarefa_atual].ordem <
            tarefas[tarefa_escolhida].ordem) {

            escolhida = i;
        }
    }

    return escolhida;
}

int escolher_instancia_edf(
    Tarefa tarefas[],
    Instancia instancias[],
    int quantidade,
    int tempo)
{
    int escolhida = -1;

    for (int i = 0; i < quantidade; i++) {
        if (instancias[i].chegada > tempo)
            continue;

        if (instancias[i].concluida || instancias[i].perdida)
            continue;

        if (tempo >= instancias[i].deadline)
            continue;

        if (escolhida == -1) {
            escolhida = i;
            continue;
        }

        int tarefa_atual = instancias[i].tarefa;
        int tarefa_escolhida = instancias[escolhida].tarefa;

        if (instancias[i].deadline <
            instancias[escolhida].deadline) {

            escolhida = i;
        } else if (
            instancias[i].deadline ==
            instancias[escolhida].deadline &&
            tarefas[tarefa_atual].ordem <
            tarefas[tarefa_escolhida].ordem) {

            escolhida = i;
        }
    }

    return escolhida;
}

void verificar_deadlines(
    Instancia instancias[],
    int quantidade,
    int tempo)
{
    for (int i = 0; i < quantidade; i++) {
        if (instancias[i].concluida || instancias[i].perdida)
            continue;

        if (tempo >= instancias[i].deadline) {
            instancias[i].perdida = 1;
            instancias[i].restante = 0;
        }
    }
}

void executar_rate(
    Tarefa tarefas[],
    Instancia instancias[],
    int quantidade,
    int tempo_total,
    FILE *saida)
{
    fprintf(saida, "EXECUTION BY RATE\n");

    int tempo = 0;
    int inicio = -1;

    while (tempo < tempo_total) {
        verificar_deadlines(instancias, quantidade, tempo);

        int escolhida = escolher_instancia_rate(
            tarefas,
            instancias,
            quantidade,
            tempo);

        if (escolhida == -1) {
            if (inicio != -1) {
                fprintf(saida,
                        "[%s] for %d units - F\n",
                        tarefas[instancias[inicio].tarefa].nome,
                        tempo - inicio);
                inicio = -1;
            }

            tempo++;
            continue;
        }

        if (inicio != escolhida) {
            if (inicio != -1) {
                fprintf(saida,
                        "[%s] for %d units - F\n",
                        tarefas[instancias[inicio].tarefa].nome,
                        tempo - inicio);
            }

            inicio = escolhida;
        }

        instancias[escolhida].executou = 1;
        instancias[escolhida].restante--;
        tempo++;

        if (instancias[escolhida].restante == 0)
            instancias[escolhida].concluida = 1;
    }

    if (inicio != -1) {
        fprintf(saida,
                "[%s] for %d units - F\n",
                tarefas[instancias[inicio].tarefa].nome,
                tempo - inicio);
    }

    fprintf(saida, "\nLOST DEADLINES\n");

    for (int i = 0; i < quantidade; i++) {
        if (instancias[i].perdida) {
            fprintf(saida,
                    "[%s] %d\n",
                    tarefas[instancias[i].tarefa].nome,
                    instancias[i].tarefa);
        }
    }

    fprintf(saida, "\nCOMPLETE EXECUTION\n");

    for (int i = 0; i < quantidade; i++) {
        if (instancias[i].concluida) {
            fprintf(saida,
                    "[%s] %d\n",
                    tarefas[instancias[i].tarefa].nome,
                    instancias[i].tarefa);
        }
    }

    fprintf(saida, "\nKILLED\n");

    for (int i = 0; i < quantidade; i++) {
        if (!instancias[i].concluida &&
            !instancias[i].perdida &&
            instancias[i].chegada < tempo_total) {

            fprintf(saida,
                    "[%s] %d\n",
                    tarefas[instancias[i].tarefa].nome,
                    instancias[i].tarefa);
        }
    }
}

void executar_edf(
    Tarefa tarefas[],
    Instancia instancias[],
    int quantidade,
    int tempo_total,
    FILE *saida)
{
    fprintf(saida, "EXECUTION BY EDF\n");

    int tempo = 0;
    int inicio = -1;

    while (tempo < tempo_total) {
        verificar_deadlines(instancias, quantidade, tempo);

        int escolhida = escolher_instancia_edf(
            tarefas,
            instancias,
            quantidade,
            tempo);

        if (escolhida == -1) {
            if (inicio != -1) {
                fprintf(saida,
                        "[%s] for %d units - F\n",
                        tarefas[instancias[inicio].tarefa].nome,
                        tempo - inicio);
                inicio = -1;
            }

            tempo++;
            continue;
        }

        if (inicio != escolhida) {
            if (inicio != -1) {
                fprintf(saida,
                        "[%s] for %d units - F\n",
                        tarefas[instancias[inicio].tarefa].nome,
                        tempo - inicio);
            }

            inicio = escolhida;
        }

        instancias[escolhida].executou = 1;
        instancias[escolhida].restante--;
        tempo++;

        if (instancias[escolhida].restante == 0)
            instancias[escolhida].concluida = 1;
    }

    if (inicio != -1) {
        fprintf(saida,
                "[%s] for %d units - F\n",
                tarefas[instancias[inicio].tarefa].nome,
                tempo - inicio);
    }

    fprintf(saida, "\nLOST DEADLINES\n");

    for (int i = 0; i < quantidade; i++) {
        if (instancias[i].perdida) {
            fprintf(saida,
                    "[%s] %d\n",
                    tarefas[instancias[i].tarefa].nome,
                    instancias[i].tarefa);
        }
    }

    fprintf(saida, "\nCOMPLETE EXECUTION\n");

    for (int i = 0; i < quantidade; i++) {
        if (instancias[i].concluida) {
            fprintf(saida,
                    "[%s] %d\n",
                    tarefas[instancias[i].tarefa].nome,
                    instancias[i].tarefa);
        }
    }

    fprintf(saida, "\nKILLED\n");

    for (int i = 0; i < quantidade; i++) {
        if (!instancias[i].concluida &&
            !instancias[i].perdida &&
            instancias[i].chegada < tempo_total) {

            fprintf(saida,
                    "[%s] %d\n",
                    tarefas[instancias[i].tarefa].nome,
                    instancias[i].tarefa);
        }
    }
}

int main(int argc, char *argv[])
{
    if (argc != 3) {
        fprintf(stderr,
                "Uso: %s <rate|edf> <arquivo>\n",
                argv[0]);
        return 1;
    }

    if (strcmp(argv[1], "rate") != 0 &&
        strcmp(argv[1], "edf") != 0) {

        fprintf(stderr,
                "Erro: algoritmo deve ser rate ou edf.\n");
        return 1;
    }

    FILE *entrada = fopen(argv[2], "r");

    if (entrada == NULL) {
        fprintf(stderr,
                "Erro: nao foi possivel abrir o arquivo.\n");
        return 1;
    }

    int tempo_total;

    if (fscanf(entrada, "%d", &tempo_total) != 1 ||
        tempo_total <= 0) {

        fprintf(stderr,
                "Erro: tempo total invalido.\n");
        fclose(entrada);
        return 1;
    }

    Tarefa tarefas[MAX_TAREFAS];
    int num_tarefas = 0;

    while (1) {
        Tarefa tarefa;

        int resultado = fscanf(
            entrada,
            "%49s %d %d %d",
            tarefa.nome,
            &tarefa.periodo,
            &tarefa.deadline,
            &tarefa.burst);

        if (resultado == EOF)
            break;

        if (resultado != 4) {
            fprintf(stderr,
                    "Erro: entrada invalida.\n");
            fclose(entrada);
            return 1;
        }

        if (num_tarefas >= MAX_TAREFAS) {
            fprintf(stderr,
                    "Erro: numero maximo de tarefas excedido.\n");
            fclose(entrada);
            return 1;
        }

        tarefa.ordem = num_tarefas;

        if (!validar_tarefa(&tarefa)) {
            fprintf(stderr,
                    "Erro: parametros invalidos para tarefa %s.\n",
                    tarefa.nome);
            fclose(entrada);
            return 1;
        }

        tarefas[num_tarefas] = tarefa;
        num_tarefas++;
    }

    fclose(entrada);

    if (num_tarefas == 0) {
        fprintf(stderr,
                "Erro: nenhuma tarefa encontrada.\n");
        return 1;
    }

    qsort(
        tarefas,
        num_tarefas,
        sizeof(Tarefa),
        comparar_rate);

    Instancia instancias[MAX_INSTANCIAS];

    int quantidade = criar_instancias(
        tarefas,
        num_tarefas,
        tempo_total,
        instancias);

    const char *nome_saida = (strcmp(argv[1], "rate") == 0)
        ? "rate_gbm.out"
        : "edf_gbm.out";

    FILE *saida = fopen(nome_saida, "w");

    if (saida == NULL) {
        fprintf(stderr,
                "Erro: nao foi possivel criar arquivo de saida.\n");
        return 1;
    }

    if (strcmp(argv[1], "rate") == 0) {
        executar_rate(
            tarefas,
            instancias,
            quantidade,
            tempo_total,
            saida);
    } else {
        executar_edf(
            tarefas,
            instancias,
            quantidade,
            tempo_total,
            saida);
    }

    fclose(saida);

    return 0;
}