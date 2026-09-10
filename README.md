# Escalonamento de Tarefas Críticas de Voo

Projeto de simulação de escalonamento de tarefas utilizando os algoritmos **Rate Monotonic (RATE)** e **Earliest Deadline First (EDF)**.

## Estrutura

* `main.c` — código principal do projeto, contendo a leitura e validação da entrada, criação das instâncias e execução dos algoritmos RATE e EDF.
* `Makefile` — automatiza a compilação e a limpeza dos arquivos gerados.
* `README.md` — documentação e instruções básicas de uso.

## Compilação

Na raiz do projeto, execute:

```bash
make
```

Isso irá gerar o executável:

```text
scheduler
```

Para remover o executável e os arquivos de saída:

```bash
make clean
```

## Uso

O programa recebe o algoritmo e o arquivo de entrada:

```bash
./scheduler rate <arquivo>
```

ou:

```bash
./scheduler edf <arquivo>
```

### Exemplo

Crie um arquivo `voo.txt`:

```text
100
ATT 20 12 8
NAV 50 30 15
```

Execute:

```bash
./scheduler rate voo.txt
```

A saída será salva em:

```text
rate_gbm.out
```

Para executar com EDF:

```bash
./scheduler edf voo.txt
```

A saída será salva em:

```text
edf_gbm.out
```

## Formato da entrada

A primeira linha contém o tempo total da simulação:

```text
[TEMPO TOTAL]
```

As linhas seguintes possuem:

```text
[NOME] [PERÍODO] [DEADLINE] [BURST]
```

Os valores devem ser positivos. O `burst` deve ser menor ou igual ao `deadline`, e o `deadline` deve ser menor ou igual ao `período`.

## Saída

O arquivo de saída apresenta:

* a ordem de execução das tarefas;
* períodos de ociosidade;
* tarefas que perderam o deadline;
* tarefas que terminaram a execução;
* tarefas que permaneceram pendentes quando a simulação terminou.
