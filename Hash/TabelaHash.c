// https://computinglife.wordpress.com/2008/11/20/why-do-hash-functions-use-prime-numbers/
// http://stackoverflow.com/questions/2624192/good-hash-function-for-strings

#include "TabelaHash.h" 
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define  MAX_IDS 100 // Maximum IDs to store per word search

// Structure for a node in the linked list for separate chaining
typedef struct NoHash {
    struct tweet data;
    struct NoHash *prox;
} NoHash;

// Definição do tipo Hash
struct hash {
  int qtd, TABLE_SIZE;
  NoHash **itens; // Array of pointers to NoHash (linked list heads)
};

Hash *criaHash(int TABLE_SIZE) {
  Hash *ha = (Hash *)malloc(sizeof(Hash));
  if (ha != NULL) {
    int i;
    ha->TABLE_SIZE = TABLE_SIZE;
    ha->itens = (NoHash **)malloc(TABLE_SIZE * sizeof(NoHash *));
    if (ha->itens == NULL) {
      free(ha);
      return NULL;
    }
    ha->qtd = 0;
    for (i = 0; i < ha->TABLE_SIZE; i++)
      ha->itens[i] = NULL; // Initialize all list heads to NULL
  }
  return ha;
}

void liberaHash(Hash *ha) {
  if (ha != NULL) {
    int i;
    for (i = 0; i < ha->TABLE_SIZE; i++) {
      NoHash *current = ha->itens[i];
      while (current != NULL) {
        NoHash *next = current->prox;
        free(current);
        current = next;
      }
    }
    free(ha->itens);
    free(ha);
  }
}

int valorString(const char *str) { // Changed to const char*
  int i, valor = 7;
  int tam = strlen(str);
  for (i = 0; i < tam; i++)
    valor = 31 * valor + (int)str[i];
  return (valor & 0x7FFFFFFF); // Ensures positive hash value
}

/*
A partir daqui funções criadas para o caso específico do trabalho
*/

// Função para exportar os dados da tabela hash para um arquivo CSV
void exportaHashParaCSV(Hash *ha, const char *nomeArquivo) {
  if (ha == NULL)
    return;

  FILE *arquivo = fopen(nomeArquivo, "w");
  if (arquivo == NULL) {
    printf("Erro ao abrir o arquivo para exportação.\n");
    return;
  }

  // Percorrer a tabela hash
  for (int i = 0; i < ha->TABLE_SIZE; i++) {
    NoHash *current = ha->itens[i];
    while (current != NULL) {
      struct tweet *tt = &(current->data); // Get the tweet data

      // Para cada palavra no tweet, escreva no CSV
      for (int j = 0; j < tt->qtd_palavras; j++) {
        fprintf(arquivo, "%s,%d\n", tt->palavras[j], tt->id);
      }
      current = current->prox;
    }
  }

  fclose(arquivo);
}

// Função para extrair palavras de uma string e ignorar símbolos, números ou espaços
void extrairPalavras(const char *frase, struct tweet *tw) {
  int i = 0, j = 0, k = 0;
  char palavra[20]; // Buffer for a single word

  // Ensure tw->qtd_palavras is reset for each tweet
  tw->qtd_palavras = 0;

  while (frase[i] != '\0' && k < 256) { // Added boundary check for k
    if (isalpha(frase[i])) {
      if (j < 19) { // Ensure word buffer doesn't overflow
        palavra[j++] = tolower(frase[i]);
      }
    } else if (j > 0) {
      palavra[j] = '\0';
      strcpy(tw->palavras[k++], palavra);
      j = 0; // Reset word buffer index
    }
    i++;
  }

  if (j > 0 && k < 256) { // Handle the last word if it's not followed by a non-alpha char
    palavra[j] = '\0';
    strcpy(tw->palavras[k++], palavra);
  }
  tw->qtd_palavras = k;
}

// Função modificada de inserção para processar e armazenar cada palavra na hash
int insereHash_ComPalavras(Hash *ha, struct tweet tt) {
  if (ha == NULL || ha->qtd >= ha->TABLE_SIZE)
    return 0; // Check if hash is full (though with chaining, it's more about performance)

  // Calculate the hash position for the tweet's text
  int pos = valorString(tt.texto) % ha->TABLE_SIZE;

  // Create a new node for the tweet
  NoHash *novoNo = (NoHash *)malloc(sizeof(NoHash));
  if (novoNo == NULL) {
    return 0; // Memory allocation failed
  }
  novoNo->data = tt; // Copy the tweet data
  novoNo->prox = NULL;

  // Extract and store words within the new node's tweet data
  extrairPalavras(tt.texto, &(novoNo->data));

  // Insert at the beginning of the linked list at the calculated position (separate chaining)
  novoNo->prox = ha->itens[pos];
  ha->itens[pos] = novoNo;
  ha->qtd++;

  return 1;
}

// Função para encontrar a palavra buscada e retornar a lista de ids     
int *buscaPorPalavra(Hash *ha, const char *palavra) {
    static int ids[MAX_IDS + 1]; // +1 for the sentinel -1
    int num_ids = 0;          

    // Limpar o vetor de IDs antes de usá-lo
    for (int i = 0; i < MAX_IDS + 1; i++) {
        ids[i] = -1;
    }

    // Verificar se a tabela hash e a palavra são válidas
    if (ha == NULL || palavra == NULL) {
        printf("Hash ou palavra inválida.\n");
        return ids;
    }

    // Traverse all linked lists (all buckets) to find the word
    for (int i = 0; i < ha->TABLE_SIZE; i++) {
        NoHash *current = ha->itens[i];
        while (current != NULL) {
            // For each tweet in the linked list, check its words
            for (int j = 0; j < current->data.qtd_palavras; j++) {
                // Perform case-insensitive comparison (palavra is already lowercased in main)
                if (strcmp(current->data.palavras[j], palavra) == 0) {
                    // Check if ID is already in the list to avoid duplicates (sets will handle this later, but good practice)
                    int already_added = 0;
                    for (int k = 0; k < num_ids; k++) {
                        if (ids[k] == current->data.id) {
                            already_added = 1;
                            break;
                        }
                    }
                    if (!already_added) {
                        if (num_ids < MAX_IDS) {
                            ids[num_ids++] = current->data.id;
                        } else {
                            // Max IDs reached, can print a warning or resize if dynamic
                            // printf("Aviso: Limite de MAX_IDS (%d) atingido para a palavra '%s'.\n", MAX_IDS, palavra);
                            break; // Stop adding for this word
                        }
                    }
                }
            }
            current = current->prox; // Move to the next node in the linked list
        }
    }

    ids[num_ids] = -1; // Sentinel value
    return ids;
}