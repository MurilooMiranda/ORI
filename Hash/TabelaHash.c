// https://computinglife.wordpress.com/2008/11/20/why-do-hash-functions-use-prime-numbers/
// http://stackoverflow.com/questions/2624192/good-hash-function-for-strings

#include "TabelaHash.h"
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define MAX_IDS 100 // Número máximo de IDs por busca de palavra

// Fator de carga para redimensionamento (ex: 75% de ocupação)
#define LOAD_FACTOR_THRESHOLD 0.75
#define MAX_WORD_LEN 20 // Tamanho máximo de uma palavra

// Estrutura para um nó da lista encadeada (encadeamento separado)
typedef struct NoHash {
    struct tweet data;
    struct NoHash *prox;
} NoHash;

// Definição da estrutura da tabela Hash
struct hash {
    int qtd;        // Quantidade de elementos na hash
    int TABLE_SIZE; // Tamanho atual da tabela
    NoHash **itens; // Array de ponteiros para as cabeças das listas
};

// Função auxiliar para verificar se um número é primo
static int is_prime(int n) {
    if (n <= 1) return 0;
    if (n <= 3) return 1;
    if (n % 2 == 0 || n % 3 == 0) return 0;
    for (int i = 5; i * i <= n; i = i + 6)
        if (n % i == 0 || n % (i + 2) == 0)
            return 0;
    return 1;
}

// Função auxiliar para encontrar o próximo número primo
static int get_next_prime(int n) {
    if (n % 2 == 0) n++; // Começa com um número ímpar
    while (!is_prime(n)) {
        n += 2;
    }
    return n;
}

// Cria e inicializa uma nova tabela hash.
Hash *criaHash(int TABLE_SIZE) {
    Hash *ha = (Hash *)malloc(sizeof(Hash));
    if (ha != NULL) {
        ha->TABLE_SIZE = TABLE_SIZE;
        ha->itens = (NoHash **)malloc(TABLE_SIZE * sizeof(NoHash *));
        if (ha->itens == NULL) {
            free(ha);
            return NULL;
        }
        ha->qtd = 0;
        for (int i = 0; i < ha->TABLE_SIZE; i++)
            ha->itens[i] = NULL;
    }
    return ha;
}

// Libera toda a memória alocada para a tabela hash, incluindo as palavras dinâmicas dos tweets.
void liberaHash(Hash *ha) {
    if (ha != NULL) {
        for (int i = 0; i < ha->TABLE_SIZE; i++) {
            NoHash *current = ha->itens[i];
            while (current != NULL) {
                NoHash *next = current->prox;

                // Libera a memória alocada para as palavras do tweet
                for (int j = 0; j < current->data.qtd_palavras; j++) {
                    free(current->data.palavras[j]);
                }
                free(current->data.palavras); // Libera o array de ponteiros

                free(current); // Libera o nó da lista
                current = next;
            }
        }
        free(ha->itens); // Libera o array de ponteiros das listas
        free(ha);        // Libera a estrutura da hash
    }
}

// Calcula o valor hash de uma string.
int valorString(const char *str) {
    int i, valor = 7;
    int tam = strlen(str);
    for (i = 0; i < tam; i++)
        valor = 31 * valor + (int)str[i];
    return (valor & 0x7FFFFFFF);
}

// Redimensiona a tabela hash para um novo tamanho.
static int redimensionaHash(Hash *ha) {
    if (ha == NULL) return 0;

    int old_size = ha->TABLE_SIZE;
    int new_size = get_next_prime(old_size * 2);

    // Evita redimensionar para um tamanho menor ou igual ao atual
    if (new_size <= old_size) {
        // Isso pode acontecer se old_size * 2 não for muito maior e o próximo primo for o mesmo ou menor.
        // Ou se o new_size for tão grande que cause overflow (improvável aqui).
        return 0; // Não redimensiona ou indica erro.
    }

    NoHash **new_itens = (NoHash **)malloc(new_size * sizeof(NoHash *));
    if (new_itens == NULL) {
        fprintf(stderr, "Erro de alocacao de memoria para redimensionamento da hash.\n");
        return 0; // Falha no redimensionamento
    }

    for (int i = 0; i < new_size; i++) {
        new_itens[i] = NULL;
    }

    // Reinsere todos os elementos da tabela antiga na nova tabela
    for (int i = 0; i < old_size; i++) {
        NoHash *current = ha->itens[i];
        while (current != NULL) {
            NoHash *node_to_move = current;
            current = current->prox;

            // Calcula a nova posição na tabela redimensionada usando a palavra do tweet
            // Atenção: O hash deve ser consistente. Se você hash o texto do tweet para o bucket,
            // e depois as palavras para a busca, certifique-se de que o hashing para o bucket
            // use uma chave estável, como o ID ou o próprio texto do tweet.
            // Aqui, estou mantendo o hash do texto do tweet como antes.
            int new_pos = valorString(node_to_move->data.texto) % new_size;

            node_to_move->prox = new_itens[new_pos];
            new_itens[new_pos] = node_to_move;
        }
    }

    free(ha->itens);

    ha->itens = new_itens;
    ha->TABLE_SIZE = new_size;

    printf("Tabela Hash redimensionada para o tamanho: %d\n", ha->TABLE_SIZE);
    return 1;
}

// Exporta os dados da tabela hash para um arquivo CSV.
void exportaHashParaCSV(Hash *ha, const char *nomeArquivo) {
    if (ha == NULL)
        return;

    FILE *arquivo = fopen(nomeArquivo, "w");
    if (arquivo == NULL) {
        printf("Erro ao abrir o arquivo para exportação.\n");
        return;
    }

    for (int i = 0; i < ha->TABLE_SIZE; i++) {
        NoHash *current = ha->itens[i];
        while (current != NULL) {
            struct tweet *tt = &(current->data);

            // Exporta as palavras dinamicamente alocadas
            for (int j = 0; j < tt->qtd_palavras; j++) {
                if (tt->palavras[j] != NULL) {
                    fprintf(arquivo, "%s,%d\n", tt->palavras[j], tt->id);
                }
            }
            current = current->prox;
        }
    }

    fclose(arquivo);
}

// Extrai palavras de uma frase e as armazena dinamicamente na estrutura do tweet.
void extrairPalavras(const char *frase, struct tweet *tw) {
    int i = 0, j = 0, word_count = 0;
    char temp_palavra[MAX_WORD_LEN];

    // Contagem inicial de palavras para pré-alocação
    int temp_word_count = 0;
    int k = 0; // temporary word index
    while (frase[i] != '\0') {
        if (isalpha(frase[i])) {
            k++;
        } else if (k > 0) {
            temp_word_count++;
            k = 0;
        }
        i++;
    }
    if (k > 0) { // last word
        temp_word_count++;
    }

    tw->qtd_palavras = temp_word_count;
    tw->palavras = (char **)malloc(tw->qtd_palavras * sizeof(char *));
    if (tw->palavras == NULL && tw->qtd_palavras > 0) {
        fprintf(stderr, "Erro de alocacao de memoria para tw->palavras.\n");
        tw->qtd_palavras = 0; // Reset count if allocation fails
        return;
    }

    i = 0; // Reset index for actual extraction
    while (frase[i] != '\0' && word_count < tw->qtd_palavras) {
        if (isalpha(frase[i])) {
            if (j < MAX_WORD_LEN - 1) { // Garante espaço para '\0'
                temp_palavra[j++] = tolower(frase[i]);
            }
        } else if (j > 0) {
            temp_palavra[j] = '\0';
            tw->palavras[word_count] = (char *)malloc(strlen(temp_palavra) + 1);
            if (tw->palavras[word_count] == NULL) {
                fprintf(stderr, "Erro de alocacao de memoria para palavra individual.\n");
                // Liberar memória já alocada para palavras anteriores e sair
                for (int l = 0; l < word_count; l++) {
                    free(tw->palavras[l]);
                }
                free(tw->palavras);
                tw->palavras = NULL;
                tw->qtd_palavras = 0;
                return;
            }
            strcpy(tw->palavras[word_count], temp_palavra);
            word_count++;
            j = 0;
        }
        i++;
    }

    if (j > 0 && word_count < tw->qtd_palavras) { // Adiciona a última palavra, se houver
        temp_palavra[j] = '\0';
        tw->palavras[word_count] = (char *)malloc(strlen(temp_palavra) + 1);
        if (tw->palavras[word_count] == NULL) {
            fprintf(stderr, "Erro de alocacao de memoria para ultima palavra individual.\n");
            for (int l = 0; l < word_count; l++) {
                free(tw->palavras[l]);
            }
            free(tw->palavras);
            tw->palavras = NULL;
            tw->qtd_palavras = 0;
            return;
        }
        strcpy(tw->palavras[word_count], temp_palavra);
        word_count++;
    }
    tw->qtd_palavras = word_count; // Atualiza a quantidade final de palavras
}

// Insere um tweet na tabela hash, processando suas palavras.
// Inclui verificação para redimensionamento dinâmico.
int insereHash_ComPalavras(Hash *ha, struct tweet tt) {
    if (ha == NULL)
        return 0;

    // A extração de palavras e alocação dinâmica deve ocorrer antes da inserção no NoHash.
    // Faça uma cópia do tweet para garantir que a memória dinâmica de palavras seja controlada.
    struct tweet *tweet_copy = (struct tweet *)malloc(sizeof(struct tweet));
    if (tweet_copy == NULL) return 0;

    *tweet_copy = tt; // Copia id e texto (texto é estático)
    tweet_copy->palavras = NULL; // Zera para extrair_palavras preencher

    extrairPalavras(tt.texto, tweet_copy); // Aloca as palavras dinamicamente para tweet_copy

    if (tweet_copy->palavras == NULL && tweet_copy->qtd_palavras > 0) { // Falha na alocação de palavras
        free(tweet_copy);
        return 0;
    }

    // Verifica o fator de carga antes de inserir
    if (((double)ha->qtd / ha->TABLE_SIZE) >= LOAD_FACTOR_THRESHOLD) {
        if (!redimensionaHash(ha)) {
            // Se o redimensionamento falhar, liberamos a memória do tweet_copy e suas palavras
            for (int j = 0; j < tweet_copy->qtd_palavras; j++) {
                free(tweet_copy->palavras[j]);
            }
            free(tweet_copy->palavras);
            free(tweet_copy);
            return 0;
        }
    }

    int pos = valorString(tweet_copy->texto) % ha->TABLE_SIZE; // Usa o texto copiado para o hash

    NoHash *novoNo = (NoHash *)malloc(sizeof(NoHash));
    if (novoNo == NULL) {
        // Se o nó falhar, liberamos o tweet_copy e suas palavras
        for (int j = 0; j < tweet_copy->qtd_palavras; j++) {
            free(tweet_copy->palavras[j]);
        }
        free(tweet_copy->palavras);
        free(tweet_copy);
        return 0;
    }

    novoNo->data = *tweet_copy; // Copia a struct tweet já com as palavras alocadas
    free(tweet_copy); // Libera a struct tweet temporária, mas não seus dados dinâmicos

    novoNo->prox = ha->itens[pos];
    ha->itens[pos] = novoNo;
    ha->qtd++;

    return 1;
}

// Busca por uma palavra na tabela hash e retorna um array de IDs de tweets que a contêm.
int *buscaPorPalavra(Hash *ha, const char *palavra) {
    static int ids[MAX_IDS + 1]; // Array estático para IDs (limitação para 100 IDs)
    int num_ids = 0;

    // Inicializa o array de IDs com -1 para indicar o fim ou não encontrado
    for (int i = 0; i < MAX_IDS + 1; i++) {
        ids[i] = -1;
    }

    if (ha == NULL || palavra == NULL) {
        // printf("Hash ou palavra inválida.\n"); // Descomente para debug se necessário
        return ids;
    }

    // A palavra de busca também deve ser normalizada (minúsculas)
    char palavra_lower[MAX_WORD_LEN];
    int len_palavra = strlen(palavra);
    if (len_palavra >= MAX_WORD_LEN) len_palavra = MAX_WORD_LEN - 1;
    for (int i = 0; i < len_palavra; i++) {
        palavra_lower[i] = tolower(palavra[i]);
    }
    palavra_lower[len_palavra] = '\0';


    // Percorre toda a tabela hash, pois a busca é por palavra contida no tweet,
    // e não pelo hash do tweet completo.
    for (int i = 0; i < ha->TABLE_SIZE; i++) {
        NoHash *current = ha->itens[i];
        while (current != NULL) {
            // Itera sobre as palavras extraídas de cada tweet
            for (int j = 0; j < current->data.qtd_palavras; j++) {
                if (current->data.palavras[j] != NULL &&
                    strcmp(current->data.palavras[j], palavra_lower) == 0) {
                    
                    // Verifica se o ID já foi adicionado para evitar duplicatas
                    int already_added = 0;
                    for (int k = 0; k < num_ids; k++) {
                        if (ids[k] == current->data.id) {
                            already_added = 1;
                            break;
                        }
                    }
                    // Adiciona o ID se não for duplicado e houver espaço
                    if (!already_added) {
                        if (num_ids < MAX_IDS) {
                            ids[num_ids++] = current->data.id;
                        } else {
                            // Se atingir MAX_IDS, para de adicionar mas continua buscando
                            // para garantir que a pesquisa encontre todas as correspondências,
                            // embora apenas as primeiras 100 sejam retornadas.
                            break; 
                        }
                    }
                }
            }
            current = current->prox;
        }
    }

    ids[num_ids] = -1; // Marca o fim dos IDs encontrados
    return ids;
}