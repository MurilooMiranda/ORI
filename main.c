/*

Autores:
  Murilo de Miranda Silva - RA: 812069
  Enzo Corradini - RA: 822915
  Eduardo Marcondes Nunes de Matos - 823020
*/


#include "Hash/TabelaHash.h"
#include "Set/Set.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h> 

// Carrega dados de um arquivo CSV para a tabela hash.
void carregaCSVParaHash(Hash* ha, const char* nomeArquivo) {
    if (ha == NULL) return;

    FILE *arquivo = fopen(nomeArquivo, "r");
    if (arquivo == NULL) {
        printf("Erro ao abrir o arquivo '%s'.\n", nomeArquivo);
        return;
    }

    char linha[1024];
    // Lê cada linha do arquivo.
    while (fgets(linha, sizeof(linha), arquivo)) {
        int id;
        char texto[256];

        // Extrai o ID e o texto da linha.
        if (sscanf(linha, "%d,%*d,%255[^\n]", &id, texto) == 2) {
            struct tweet novoTweet;
            novoTweet.id = id;
            strcpy(novoTweet.texto, texto);

            // Insere o tweet na tabela hash.
            insereHash_ComPalavras(ha, novoTweet);
        }
    }

    fclose(arquivo);
}


// Função auxiliar para criar um conjunto a partir de uma palavra.
Set *criarConjuntoPorPalavra(Hash *ha, const char *palavra) {
  Set *conjunto = criaSet();
  if (conjunto == NULL) {
      printf("Erro ao criar o conjunto.\n");
      return NULL;
  }

  if (ha != NULL && palavra != NULL) {
    // Converte a palavra de busca para minúsculas (para busca insensível a maiúsculas/minúsculas).
    char lower_palavra[50];
    int i;
    for (i = 0; palavra[i] != '\0' && i < 49; i++) {
        lower_palavra[i] = tolower(palavra[i]);
    }
    lower_palavra[i] = '\0';
         
    // Busca os IDs associados à palavra na tabela hash.
    int *ids = buscaPorPalavra(ha, lower_palavra);
    if (ids != NULL) {
      // Insere os IDs encontrados no conjunto.
      for (int k = 0; ids[k] != -1; k++) {
        insereSet(conjunto, ids[k]);
      }
    }
  }
  return conjunto;
}

// Processa um arquivo CSV para encontrar e imprimir tweets com IDs presentes no conjunto.
void processar_arquivo(Set *set_ids, const char *file_path) {
    FILE *file = fopen(file_path, "r");
    if (file == NULL) {
        printf("Erro ao abrir o arquivo '%s'.\n", file_path);
        return;
    }

    char line[1024];
    char temp_line[1024];

    // Ler linha por linha.
    while (fgets(line, sizeof(line), file)) {
        strcpy(temp_line, line);

        // Quebrar a linha em partes (ID, lixo, texto).
        char *id_str = strtok(temp_line, ",");
        strtok(NULL, ","); // Ignora o segundo campo.
        char *texto = strtok(NULL, "\n");

        if (id_str != NULL && texto != NULL) {
            // Verifica se o ID está no Set.
            int id = atoi(id_str);
            if (consultaSet(set_ids, id)) {
                printf("ID: %s -> Texto: %s\n", id_str, texto);
            }
        }
    }

    fclose(file);
}

// Função para interpretar e executar a consulta do usuário.
void executarConsulta(Hash *ha, const char *consulta) {
  Set *resultado = NULL;
  Set *conjunto1 = NULL;
  Set *conjunto2 = NULL;

  char palavra1[50], palavra2[50], operador[4];
  int numPalavras;

  // Tenta parsear a consulta para uma ou duas palavras e um operador.
  numPalavras = sscanf(consulta, "%49s %3s %49s", palavra1, operador, palavra2);

  if (numPalavras == 3) {
    printf("Consulta com duas palavras detectada: '%s %s %s'\n", palavra1, operador, palavra2);

    conjunto1 = criarConjuntoPorPalavra(ha, palavra1);
    conjunto2 = criarConjuntoPorPalavra(ha, palavra2);

    if (conjunto1 == NULL || conjunto2 == NULL) {
      printf("Erro ao criar conjuntos para a consulta.\n");
      // Libera conjuntos temporários se foram criados.
      if (conjunto1 != NULL) liberaSet(conjunto1);
      if (conjunto2 != NULL) liberaSet(conjunto2);
      return;
    } else {
      // Executa a operação de conjunto baseada no operador.
      if (strcmp(operador, "AND") == 0) {
        resultado = interseccaoSet(conjunto1, conjunto2);
        printf("Resultado da interseccao:\n");
      } 
      else if (strcmp(operador, "OR") == 0) {
        resultado = uniaoSet(conjunto1, conjunto2);
        printf("Resultado da uniao:\n");
      } 
      else if (strcmp(operador, "NOT") == 0) {
        resultado = diferencaSet(conjunto1, conjunto2);
        printf("Resultado da diferenca:\n");
      } else {
          printf("Operador '%s' inválido. Use 'AND', 'OR' ou 'NOT'.\n", operador);
      }
      
      // Se houver um resultado, processa o arquivo e libera o conjunto resultado.
      if (resultado != NULL) {
          if (tamanhoSet(resultado) > 0) {
              processar_arquivo(resultado, "corpus.csv");
          } else {
              printf("Nenhum tweet encontrado para a consulta.\n");
          }
          liberaSet(resultado);
      }
    }

  } else if (numPalavras == 1) {
    printf("Consulta com uma palavra detectada: '%s'\n", palavra1);
    // Cria um conjunto para a única palavra e processa o arquivo.
    resultado = criarConjuntoPorPalavra(ha, palavra1);
    if (resultado != NULL) {
        if (tamanhoSet(resultado) > 0) {
            printf("Resultado da pesquisa unica:\n");
            processar_arquivo(resultado, "corpus.csv");
        } else {
            printf("Nenhum tweet encontrado para a consulta.\n");
        }
        liberaSet(resultado);
    }
  } else {
    printf("Formato de consulta inválido. Use 'palavra' ou 'palavra1 OPERADOR palavra2' (OP = AND, OR, NOT).\n");
  }

  // Libera os conjuntos intermediários, se foram criados.
  if (conjunto1 != NULL) liberaSet(conjunto1);
  if (conjunto2 != NULL) liberaSet(conjunto2);
}

int main() {
  // Define o tamanho da tabela hash.
  int TABLE_SIZE = 300;
  // Cria a tabela hash.
  Hash *ha = criaHash(TABLE_SIZE);
  if (ha == NULL) {
      printf("Erro ao criar a tabela hash.\n");
      return 1;
  }

  // Carrega dados do arquivo CSV para a hash.
  carregaCSVParaHash(ha, "corpus.csv");
  // exportaHashParaCSV(ha,"exporta.csv"); // Descomente se quiser exportar

  char consulta[256];

  // Laço de repetição principal para interação com o usuário.
  while (1) {
    // Solicita a consulta do usuário.
    printf("\nDigite 'sair' ou insira sua consulta (ex: palavra, ou palavra1 AND palavra2): ");
    fgets(consulta, sizeof(consulta), stdin);

    // Remove o caractere de nova linha do final da string.
    consulta[strcspn(consulta, "\n")] = '\0'; 

    // Verifica se o usuário deseja sair.
    if (strcmp(consulta, "sair") == 0) {
      printf("Fechando buscador...\n");
      break;
    }

    // Executa a consulta do usuário.
    executarConsulta(ha, consulta);
  }

  // Libera a tabela hash antes de sair.
  liberaHash(ha);

  return 0;
}