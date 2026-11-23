# Gerenciador de Memoria Virtual

Simulador de memoria virtual para a disciplina de Infraestrutura de Software
(Implementacao 6/7), baseado no projeto "Designing a Virtual Memory Manager" do
livro *Operating System Concepts* (Silberschatz).

O programa le enderecos logicos de 16 bits, traduz cada um para o endereco fisico
usando uma TLB e uma tabela de paginas, e imprime o valor (byte com sinal) lido
do arquivo `BACKING_STORE.bin`, que simula o disco.

## Parametros da simulacao

| Item | Valor |
|---|---|
| Endereco logico | 16 bits (8 bits de pagina + 8 bits de deslocamento) |
| Tamanho de pagina e de frame | 256 bytes |
| Paginas virtuais | 256 |
| Frames fisicos | 128 (preenchidos de 0 a 127; depois disso ha substituicao) |
| Entradas da TLB | 16 |

## Compilacao

Requer `gcc`, `make` e a biblioteca de threads POSIX (Linux, Unix ou macOS).

```
make                  # gera ./vm, com a busca na TLB feita por 16 threads
make vm_sem_threads   # gera ./vm_sem_threads, com a busca sequencial na TLB
make vm_sem_tlb       # gera ./vm_sem_tlb, sem TLB, com 128 frames
make vm_sem_tlb_256   # gera ./vm_sem_tlb_256, sem TLB, com 256 frames
make clean            # remove os executaveis
```

Todos usam o mesmo codigo, exceto a parte da TLB, que o Makefile escolhe:

- `vm`: `tlb.c` + `tlb_busca_threads.c` (uma thread por entrada da TLB).
- `vm_sem_threads`: `tlb.c` + `tlb_busca_seq.c` (um slot de cada vez).
- `vm_sem_tlb` e `vm_sem_tlb_256`: `tlb_desativada.c` (a TLB nao guarda nada; a traducao
  sempre passa pela tabela de paginas). A saida nao tem o campo `TLB:` nem as linhas
  de TLB Hits. O 3o argumento continua sendo exigido, mas e ignorado.
- Com 256 frames nao ha substituicao de paginas: os page faults sao so as paginas distintas
  do arquivo (244 em `addresses.txt`).

## Uso

```
./vm <arquivo de enderecos> <fifo|lru> <fifo|lru>
```

- 1o argumento: arquivo com um endereco logico (inteiro decimal, 0 a 65535) por linha.
- 2o argumento: algoritmo de substituicao de **paginas** na memoria fisica.
- 3o argumento: algoritmo de substituicao de entradas da **TLB**.

O arquivo `BACKING_STORE.bin` precisa estar no diretorio onde o programa e executado.
O resultado completo e gravado em `correct.txt`, no diretorio onde o programa e executado
(se o arquivo ja existir, ele e sobrescrito). Na tela aparece so o resumo.
Para comparar com o arquivo do professor (que usa quebra de linha do Windows):

```
./vm addresses.txt lru fifo
diff --strip-trailing-cr correct.txt correct_original.txt && echo IDENTICO
```

O `correct.txt` so e criado depois que o arquivo de enderecos foi validado inteiro.
Se houver qualquer erro, o programa nao cria nem altera o `correct.txt`.

### Formato da saida

Uma linha por endereco:

```
Virtual address: 16916 TLB: 0 Physical address: 20 Value: 0
```

O campo `TLB` e o numero do slot (0 a 15): o slot onde a traducao foi encontrada
(TLB hit) ou onde foi guardada (TLB miss). Depois das linhas vem o resumo:

```
Number of Translated Addresses = 1000
Page Faults = 539
Page Fault Rate = 0.539
TLB Hits = 54
TLB Hit Rate = 0.054
```

## Algoritmos de substituicao

Cada frame (e cada entrada da TLB) guarda um carimbo de tempo, e a vitima e sempre
o de **menor carimbo**:

- **FIFO:** o carimbo so e gravado quando a pagina (ou traducao) entra. O menor
  carimbo e o mais antigo.
- **LRU:** o carimbo tambem e atualizado a cada uso. O menor carimbo e o que ficou
  mais tempo sem ser usado.

Quando uma pagina e expulsa da memoria fisica, a traducao dela tambem e removida da
TLB, para a TLB nunca devolver um frame que ja guarda outra pagina.

## Tratamento de erros

O programa nao quebra: imprime uma mensagem em `stderr` e termina com codigo 1 quando:

- o numero de argumentos esta errado;
- o 2o ou o 3o argumento nao e `fifo` nem `lru`;
- o arquivo de enderecos nao existe ou nao pode ser aberto;
- alguma linha do arquivo de enderecos nao e um inteiro entre 0 e 65535;
- alguma linha do arquivo de enderecos tem mais de 998 caracteres;
- ocorre uma falha ao ler o arquivo de enderecos ou ao gravar o `correct.txt`;
- o arquivo de enderecos nao contem nenhum endereco;
- `BACKING_STORE.bin` nao pode ser aberto ou lido.

Linhas em branco no arquivo de enderecos sao ignoradas.

## Estrutura do codigo

| Arquivo | Responsabilidade |
|---|---|
| `vm.c` | `main`: valida argumentos, le os enderecos, coordena TLB e memoria e imprime |
| `memoria.c` / `memoria.h` | Memoria fisica, tabela de paginas, page faults e substituicao FIFO/LRU de paginas |
| `tlb.c` / `tlb.h` | Armazenamento da TLB e substituicao FIFO/LRU de entradas |
| `tlb_busca_seq.c` | Busca sequencial na TLB (sem threads) |
| `tlb_busca_threads.c` | Busca na TLB com uma thread por entrada |
| `tlb_desativada.c` | TLB desligada, usada nos executaveis `vm_sem_tlb` e `vm_sem_tlb_256` |
| `arquivos.c` / `arquivos.h` | Abertura de arquivos, leitura e validacao dos enderecos, leitura do backing store |

## Resultados de referencia

Com `addresses.txt` (1000 enderecos):

| Pagina | TLB | Page faults | TLB hits |
|---|---|---|---|
| FIFO | FIFO | 538 | 54 |
| LRU | FIFO | 539 | 54 |

A saida de `./vm addresses.txt lru fifo` e identica ao arquivo de referencia fornecido.
