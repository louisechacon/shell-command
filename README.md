# Shell Command

Um interpretador de linha de comando em C++ para executar comandos internos e externos.

## Requisitos

- Sistema Linux ou outro ambiente compatível;
- Compilador g++ instalado;
- Terminal para executar o programa.

## Como compilar

```bash
mkdir -p bin
g++ shell.cpp -o bin/shell
```

## Como executar

Depois de compilar, execute o programa a partir da pasta do projeto:

```bash
./bin/shell
```

## Comandos internos

| Comando | Descrição |
| --- | --- |
| `history` | Mostra o histórico de comandos executados. |
| `history -c` | Limpa o histórico. |
| `cd <diretorio>` | Altera o diretório atual. |
| `pwd` | Mostra o diretório atual. |
| `exit` | Encerra o shell. |

## Comandos externos

Os comandos externos são programas do sistema, como `ls`, `echo` e outros. Quando o shell recebe um comando que não é interno, ele tenta executá-lo como processo externo. Nesse projeto, a execução é feita usando `fork()` e `execve()`, e a busca é feita em diretórios do sistema, como `/bin`.

## Encerramento

Para sair da aplicação, execute dentro da shell:

```text
$ > exit
```