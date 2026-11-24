# Gerenciador de Memória Virtual

Implementação em C de um gerenciador de memória virtual com paginação sob demanda,
tabela de páginas e TLB. Projeto da disciplina **Infraestrutura de Software**,
baseado no *Designing a Virtual Memory Manager* (Silberschatz, *Operating System
Concepts*, 10ª ed.).

O programa recebe uma lista de endereços lógicos, traduz cada um para o endereço
físico correspondente e mostra o byte que está guardado lá.

---

## A ideia

Um programa enxerga um espaço de memória grande e contínuo (o espaço **lógico**),
mas a memória **física** é menor. A tradução entre os dois é feita por páginas:

- a memória lógica é dividida em **páginas** e a física em **frames**, todos de 256 bytes;
- a **tabela de páginas** diz em qual frame cada página está, ou se ela não está na memória;
- quando o programa acessa uma página que não está na memória, acontece um
  **page fault**: a página é lida do disco (`BACKING_STORE.bin`) e colocada num frame;
- a **TLB** é um cache pequeno com as traduções mais recentes, consultado antes da tabela.

| | Quantidade |
|---|---|
| Endereço lógico | 16 bits (0 a 65535) |
| Páginas | 256, de 256 bytes cada |
| Frames | 128, de 256 bytes cada |
| Entradas da TLB | 16 |

Como existem 256 páginas e só 128 frames, a memória enche. A partir daí, cada page
fault precisa escolher uma página para sair: é a **substituição de páginas**.

---

## Compilação

Precisa de `gcc`, `make` e `pthread` (Linux, Unix ou macOS).

```sh
make
```

Gera o executável `vm`. O Makefile também gera versões para os outros fluxos pedidos:

| Comando | Executável | TLB | Frames |
|---|---|---|---|
| `make` | `vm` | busca com uma thread por entrada | 128 |
| `make vm_sem_threads` | `vm_sem_threads` | busca sequencial | 128 |
| `make vm_sem_tlb` | `vm_sem_tlb` | desligada | 128 |
| `make vm_sem_tlb_256` | `vm_sem_tlb_256` | desligada | 256 |
| `make clean` | apaga todos os executáveis | | |

O código é o mesmo nos quatro. O que muda é qual arquivo da TLB entra na compilação
(`tlb_busca_threads.c`, `tlb_busca_seq.c` ou `tlb_desativada.c`) e, no último, o número
de frames, passado com `-DNUM_FRAMES=256`.

---

## Execução

```sh
./vm <arquivo de endereços> <fifo|lru> <fifo|lru>
```

- **1º argumento:** arquivo com um endereço lógico por linha.
- **2º argumento:** algoritmo de substituição de **páginas**.
- **3º argumento:** algoritmo de substituição da **TLB**.

```sh
./vm addresses.txt lru fifo
```

O `BACKING_STORE.bin` precisa estar na pasta onde o programa roda. O resultado
completo vai para o arquivo **`correct.txt`**, e na tela aparece só o resumo.

Nos executáveis sem TLB os três argumentos continuam obrigatórios, mas o terceiro
não é usado.

### Saída

Uma linha por endereço:

```
Virtual address: 16916 TLB: 0 Physical address: 20 Value: 0
```

- `TLB`: a entrada da TLB (0 a 15) onde a tradução foi encontrada ou guardada;
- `Value`: o byte lido, **com sinal** (`signed char`), por isso aparecem valores negativos.

E, no final:

```
Number of Translated Addresses = 1000
Page Faults = 539
Page Fault Rate = 0.539
TLB Hits = 54
TLB Hit Rate = 0.054
```

Sem TLB, o campo `TLB:` e as duas últimas linhas não aparecem.

---

## Passo a passo de uma tradução

Usando o primeiro endereço do `addresses.txt`, **16916**:

1. **Separar página e offset.**
   `16916 / 256 = 66` (página) e `16916 % 256 = 20` (offset, a posição dentro da página).
2. **Procurar a página 66 na TLB.** No começo ela está vazia: **TLB miss**.
3. **Consultar a tabela de páginas.** `tabelaPages[66]` vale `-1`: a página não está na
   memória, então é **page fault**.
4. **Buscar no disco.** `ler_pagina` faz `fseek` até o byte `66 × 256` do
   `BACKING_STORE.bin` e lê 256 bytes para o frame livre (o frame 0).
5. **Atualizar as tabelas.** `tabelaPages[66] = 0` e `tabelaframe[0] = 66`. A tradução
   também é guardada na TLB.
6. **Montar o endereço físico.** `0 × 256 + 20 = 20`. Só a página é trocada pelo frame;
   o offset continua o mesmo.
7. **Ler o valor** em `memoria[0][20]`.

Se outro endereço da página 66 aparecer depois, ela já está na TLB (ou pelo menos na
tabela), e não é preciso ir ao disco de novo.

---

## Substituição de páginas

O vetor `tabelaframe` guarda o caminho inverso da tabela de páginas (frame → página).
Ele é usado para saber **qual página está no frame que vai ser reaproveitado**, para
marcá-la com `-1` na tabela de páginas. Sem isso, a tabela continuaria dizendo que a
página antiga está num frame que já tem outra.

- **FIFO:** sai a página que entrou primeiro. O `proximoFrame` anda em círculo
  (0, 1, …, 127, 0, 1, …), e como os frames foram preenchidos nessa ordem, ele sempre
  aponta para o mais antigo.
- **LRU:** sai a página que está há mais tempo sem ser usada. Um contador `tempo` sobe
  a cada endereço traduzido e é anotado em `ultimoUso[frame]` em **todo** acesso, com ou
  sem page fault. A vítima é o frame com o menor `ultimoUso`.

Enquanto ainda há frame livre, os dois fazem a mesma coisa: usam o próximo frame vazio.

Quando uma página sai da memória, a tradução dela também é **apagada da TLB**, senão a
TLB devolveria um frame que agora guarda outra página.

## Substituição na TLB

Cada entrada da TLB tem um carimbo de tempo, e a que sai é a de menor carimbo. As
entradas vazias têm `-1`, então são ocupadas primeiro.

- **FIFO:** o carimbo só é gravado quando a tradução entra.
- **LRU:** o carimbo também é renovado a cada TLB hit.

## Busca na TLB com threads

Numa TLB real, o hardware compara a página com todas as entradas ao mesmo tempo. Para
simular isso, `tlb_busca_threads.c` cria **16 threads, uma por entrada**. Cada thread olha
só a sua entrada e escreve o resultado só na sua própria estrutura, então nenhuma disputa
o mesmo dado e não é preciso mutex. Depois o programa espera todas (`pthread_join`) e vê
qual encontrou a página.

`tlb_busca_seq.c` faz a mesma busca com um laço comum. As duas versões dão a mesma saída.

---

## Erros tratados

O programa não quebra: mostra uma mensagem de erro e sai com código 1 quando

- o número de argumentos está errado;
- o 2º ou o 3º argumento não é `fifo` nem `lru`;
- o arquivo de endereços não existe ou não abre;
- uma linha não é um número inteiro de 0 a 65535 (`abc`, `12x`, `-5`, `70000`);
- uma linha é longa demais para o buffer;
- o arquivo não tem nenhum endereço (vazio ou só com linhas em branco);
- o `BACKING_STORE.bin` não abre ou não tem a página pedida;
- há falha ao ler o arquivo de endereços ou ao gravar o `correct.txt`.

Linhas em branco e espaços antes ou depois do número são ignorados.

O arquivo de endereços é conferido inteiro **antes** da tradução. Se tiver erro em
qualquer linha, o programa para sem criar nem alterar o `correct.txt`, em vez de
deixar uma saída pela metade.

---

## Organização do código

| Arquivo | O que faz |
|---|---|
| `vm.c` | `main`: valida os argumentos, percorre os endereços e coordena TLB, memória e saída |
| `arquivos.c` / `.h` | abrir arquivos, ler e validar os endereços, ler páginas do `BACKING_STORE.bin` |
| `memoria.c` / `.h` | memória física, tabela de páginas, page faults, FIFO e LRU de páginas |
| `tlb.c` / `.h` | entradas da TLB, FIFO e LRU da TLB |
| `tlb_busca_threads.c` | busca na TLB com 16 threads |
| `tlb_busca_seq.c` | busca na TLB sem threads |
| `tlb_desativada.c` | versão "vazia" da TLB, para os executáveis sem TLB |
| `Makefile` | compila os quatro executáveis e limpa |

O estado da memória e da TLB fica em variáveis `static` dentro de `memoria.c` e `tlb.c`.
O `main` não mexe nelas diretamente, só chama funções.

---

## Resultados com `addresses.txt`

| Páginas | TLB | Page faults | TLB hits |
|---|---|---|---|
| FIFO | FIFO | 538 | 54 |
| FIFO | LRU | 538 | 55 |
| LRU | FIFO | 539 | 54 |
| LRU | LRU | 539 | 55 |

- Sem TLB, os page faults são os mesmos (538 com FIFO, 539 com LRU).
- Com 256 frames são **244**: o número de páginas diferentes no arquivo, já que nenhuma
  precisa sair da memória.
- `./vm addresses.txt lru fifo` gera um `correct.txt` idêntico ao de referência do
  professor.
- Com esses endereços, que são aleatórios e têm pouca localidade, a LRU não ganha da
  FIFO.
