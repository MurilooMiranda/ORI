
/*

Autores:
    Murilo de Miranda Silva - RA: 812069
*/


#include "Hash/TabelaHash.h"
#include "Set/Set.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h> 

void carregaCSVParaHash(Hash* ha, const char* nomeArquivo) {
    if (ha == NULL) return;

    FILE *arquivo = fopen(nomeArquivo, "r");
    if (arquivo == NULL) {
        printf("Erro ao abrir o arquivo '%s'.\n", nomeArquivo);
        return;
    }

    char linha[1024];
    while (fgets(linha, sizeof(linha), arquivo)) {
        int id;
        char texto[256];

        // Supondo que o CSV tenha o formato: id, valor1, texto
        // Using %*d to ignore the second integer, and %255[^\n] to read the rest of the line
        // Added a check for sscanf return value for robustness
        if (sscanf(linha, "%d,%*d,%255[^\n]", &id, texto) == 2) {
            struct tweet novoTweet;
            novoTweet.id = id;
            strcpy(novoTweet.texto, texto);

            // Insere na hash
            insereHash_ComPalavras(ha, novoTweet);
        }
    }

    fclose(arquivo);
}


// Função auxiliar para criar um conjunto a partir de uma palavra
Set *criarConjuntoPorPalavra(Hash *ha, const char *palavra) {
  Set *conjunto = criaSet();
  if (conjunto == NULL) {
      printf("Erro ao criar o conjunto.\n");
      return NULL;
  }

  if (ha != NULL && palavra != NULL) {
    // Convert the search word to lowercase for case-insensitive search
    char lower_palavra[50];
    int i;
    for (i = 0; palavra[i] != '\0' && i < 49; i++) {
        lower_palavra[i] = tolower(palavra[i]);
    }
    lower_palavra[i] = '\0';
         
    int *ids = buscaPorPalavra(ha, lower_palavra);
    if (ids != NULL) {
      for (int k = 0; ids[k] != -1; k++) {
        insereSet(conjunto, ids[k]);
      }
    }
  }
  return conjunto;
}

void processar_arquivo(Set *set_ids, const char *file_path) {
    FILE *file = fopen(file_path, "r");
    if (file == NULL) {
        printf("Erro ao abrir o arquivo '%s'.\n", file_path);
        return;
    }

    char line[1024]; // Increased buffer size for lines
    char temp_line[1024]; // Temporary buffer for strtok, as it modifies the string

    // Ler linha por linha
    while (fgets(line, sizeof(line), file)) {
        strcpy(temp_line, line); // Copy to temp_line because strtok modifies the original string

        // Quebrar a linha em partes (ID, lixo, texto)
        char *id_str = strtok(temp_line, ",");
        strtok(NULL, ","); // Ignorar a parte do lixo
        char *texto = strtok(NULL, "\n"); // Read until newline

        if (id_str != NULL && texto != NULL) {
            // Verificar se o ID está no Set
            int id = atoi(id_str);
            if (consultaSet(set_ids, id)) {
                printf("ID: %s -> Texto: %s\n", id_str, texto);
            }
        }
    }

    fclose(file);
}

// Função para interpretar e executar a consulta do usuário
void executarConsulta(Hash *ha, const char *consulta) {
  Set *resultado = NULL;
  Set *conjunto1 = NULL;
  Set *conjunto2 = NULL;

  char palavra1[50], palavra2[50], operador[4];
  int numPalavras;

  // Tenta ler o formato (palavra1 AND palavra2), (palavra1 OR palavra2), ou (palavra1 NOT palavra2)
  numPalavras = sscanf(consulta, "%49s %3s %49s", palavra1, operador, palavra2);

  if (numPalavras == 3) {
    printf("Consulta com duas palavras detectada: '%s %s %s'\n", palavra1, operador, palavra2);

    conjunto1 = criarConjuntoPorPalavra(ha, palavra1);
    conjunto2 = criarConjuntoPorPalavra(ha, palavra2);

    if (conjunto1 == NULL || conjunto2 == NULL) {
      printf("Erro ao criar conjuntos para a consulta.\n");
      liberaSet(conjunto1); // Ensure cleanup
      liberaSet(conjunto2); // Ensure cleanup
      return;
    } else {
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
      
      if (resultado != NULL) {
          processar_arquivo(resultado, "corpus.csv");
          liberaSet(resultado); // Free the result set
      }
    }

  } else if (numPalavras == 1) {
    printf("Consulta com uma palavra detectada: '%s'\n", palavra1);
    resultado = criarConjuntoPorPalavra(ha, palavra1);
    printf("Resultado da pesquisa unica:\n");
    if (resultado != NULL) {
        processar_arquivo(resultado, "corpus.csv");
    }
  } else {
    printf("Formato de consulta inválido. Use 'palavra' ou 'palavra1 OPERADOR palavra2' (OP = AND, OR, NOT).\n");
    return;
  }

  // Free intermediate sets
  liberaSet(conjunto1);
  liberaSet(conjunto2);
}

int main() {
  // Criando a tabela hash
  int TABLE_SIZE = 300; // Consider making this larger for more data
  Hash *ha = criaHash(TABLE_SIZE);
  if (ha == NULL) {
      printf("Erro ao criar a tabela hash.\n");
      return 1;
  }

  // Carregando dados do arquivo CSV e inserindo na hash
  carregaCSVParaHash(ha, "corpus.csv");
  // Exportando dados da hash para um arquivo CSV (para visualização)
  // exportaHashParaCSV(ha,"exporta.csv"); // Uncomment if you want to export

  char consulta[256];

  // Laço de repetição principal
  while (1) {
    // Solicita a consulta do usuário
    printf("\nDigite 'sair' ou insira sua consulta (ex: palavra, ou palavra1 AND palavra2): ");
    fgets(consulta, sizeof(consulta), stdin);

    // Remove o caractere de nova linha
    consulta[strcspn(consulta, "\n")] = '\0'; 

    // Verifica se o usuário deseja sair
    if (strcmp(consulta, "sair") == 0) {
      printf("Fechando buscador...\n");
      break;
    }

    // Executa a consulta do usuário
    executarConsulta(ha, consulta);
  }

  // Libera a tabela hash
  liberaHash(ha);

  return 0;
}
