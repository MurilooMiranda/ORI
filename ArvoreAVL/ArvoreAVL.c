#include <stdio.h>
#include <stdlib.h>
#include "ArvoreAVL.h" 

struct NO{
    int info;
    int altura;
    struct NO *esq;
    struct NO *dir;
};

// Cria e inicializa uma nova árvore AVL vazia.
ArvAVL* cria_ArvAVL(){
    ArvAVL* raiz = (ArvAVL*) malloc(sizeof(ArvAVL));
    if(raiz != NULL)
        *raiz = NULL;
    return raiz;
}

// Libera recursivamente a memória de um nó e seus descendentes.
void libera_NO(struct NO* no){
    if(no == NULL)
        return;
    libera_NO(no->esq);
    libera_NO(no->dir);
    free(no);
}

// Libera toda a memória alocada para a árvore AVL.
void libera_ArvAVL(ArvAVL* raiz){
    if(raiz == NULL)
        return;
    libera_NO(*raiz);
    free(raiz);
}

// Retorna a altura de um nó.
int altura_NO(struct NO* no){
    if(no == NULL)
        return -1;
    else
    return no->altura;
}

// Calcula o fator de balanceamento de um nó.
int fatorBalanceamento_NO(struct NO* no){
    return altura_NO(no->esq) - altura_NO(no->dir);
}

// Retorna o maior entre dois inteiros.
int maior(int x, int y){
    if(x > y)
        return x;
    else
        return y;
}

// Verifica se a árvore AVL está vazia.
int estaVazia_ArvAVL(ArvAVL *raiz){
    if(raiz == NULL)
        return 1;
    if(*raiz == NULL)
        return 1;
    return 0;
}

// Calcula o número total de nós na árvore AVL.
int totalNO_ArvAVL(ArvAVL *raiz){
    if (raiz == NULL || *raiz == NULL)
        return 0;
    int alt_esq = totalNO_ArvAVL(&((*raiz)->esq));
    int alt_dir = totalNO_ArvAVL(&((*raiz)->dir));
    return(alt_esq + alt_dir + 1);
}

// Calcula a altura da árvore AVL.
int altura_ArvAVL(ArvAVL *raiz){
    if (raiz == NULL || *raiz == NULL)
        return 0;
    int alt_esq = altura_ArvAVL(&((*raiz)->esq));
    int alt_dir = altura_ArvAVL(&((*raiz)->dir));
    if (alt_esq > alt_dir)
        return (alt_esq + 1);
    else
        return(alt_dir + 1);
}

// Percorre a árvore em pré-ordem (raiz, esquerda, direita) e imprime os nós.
void preOrdem_ArvAVL(ArvAVL *raiz){
    if(raiz == NULL)
        return;
    if(*raiz != NULL){
        
        printf("No %d: %d\n",(*raiz)->info,altura_NO(*raiz));
        preOrdem_ArvAVL(&((*raiz)->esq));
        preOrdem_ArvAVL(&((*raiz)->dir));
    }
}

// Percorre a árvore em ordem (esquerda, raiz, direita) e imprime os nós.
void emOrdem_ArvAVL(ArvAVL *raiz){
    if(raiz == NULL)
        return;
    if(*raiz != NULL){
        emOrdem_ArvAVL(&((*raiz)->esq));
        printf("%d, ",(*raiz)->info);
        emOrdem_ArvAVL(&((*raiz)->dir));
    }
}

// Percorre a árvore em pós-ordem (esquerda, direita, raiz) e imprime os nós.
void posOrdem_ArvAVL(ArvAVL *raiz){
    if(raiz == NULL)
        return;
    if(*raiz != NULL){
        posOrdem_ArvAVL(&((*raiz)->esq));
        posOrdem_ArvAVL(&((*raiz)->dir));
        printf("%d\n",(*raiz)->info);
    }
}

// Consulta se um valor específico existe na árvore AVL.
int consulta_ArvAVL(ArvAVL *raiz, int valor){
    if(raiz == NULL || *raiz == NULL)
        return 0;
    struct NO* atual = *raiz;
    while(atual != NULL){
        if(valor == atual->info){
            return 1;
        }
        if(valor > atual->info)
            atual = atual->dir;
        else
            atual = atual->esq;
    }
    return 0;
}

//=================================

// Realiza a rotação simples à esquerda-esquerda (LL) para balancear a árvore.
void RotacaoLL(ArvAVL *A){
    struct NO *B;
    B = (*A)->esq;
    (*A)->esq = B->dir;
    B->dir = *A;
    (*A)->altura = maior(altura_NO((*A)->esq),altura_NO((*A)->dir)) + 1;
    B->altura = maior(altura_NO(B->esq),(*A)->altura) + 1;
    *A = B;
}

// Realiza a rotação simples à direita-direita (RR) para balancear a árvore.
void RotacaoRR(ArvAVL *A){
    struct NO *B;
    B = (*A)->dir;
    (*A)->dir = B->esq;
    B->esq = (*A);
    (*A)->altura = maior(altura_NO((*A)->esq),altura_NO((*A)->dir)) + 1;
    B->altura = maior(altura_NO(B->dir),(*A)->altura) + 1;
    (*A) = B;
}

// Realiza a rotação dupla à esquerda-direita (LR) para balancear a árvore.
void RotacaoLR(ArvAVL *A){
    RotacaoRR(&(*A)->esq);
    RotacaoLL(A);
}

// Realiza a rotação dupla à direita-esquerda (RL) para balancear a árvore.
void RotacaoRL(ArvAVL *A){
    RotacaoLL(&(*A)->dir);
    RotacaoRR(A);
}

// Insere um valor na árvore AVL, mantendo-a balanceada.
int insere_ArvAVL(ArvAVL *raiz, int valor){
    int res;
    if(*raiz == NULL){
        struct NO *novo;
        novo = (struct NO*)malloc(sizeof(struct NO));
        if(novo == NULL)
            return 0;

        novo->info = valor;
        novo->altura = 0;
        novo->esq = NULL;
        novo->dir = NULL;
        *raiz = novo;
        return 1;
    }

    struct NO *atual = *raiz;
    if(valor < atual->info){
        if((res = insere_ArvAVL(&(atual->esq), valor)) == 1){
            if(fatorBalanceamento_NO(atual) == 2){
                if(fatorBalanceamento_NO(atual->esq) == 1){
                    RotacaoLL(raiz);
                }else{
                    RotacaoLR(raiz);
                }
            }
        }
    }else{
        if(valor > atual->info){
            if((res = insere_ArvAVL(&(atual->dir), valor)) == 1){
                if(fatorBalanceamento_NO(atual) == -2){
                    if(fatorBalanceamento_NO(atual->dir) == -1){
                        RotacaoRR(raiz);
                    }else{
                        RotacaoRL(raiz);
                    }
                }
            }
        }else{
            return 0;
        }
    }

    atual->altura = maior(altura_NO(atual->esq),altura_NO(atual->dir)) + 1;

    return res;
}

// Encontra o nó com o menor valor em uma subárvore.
struct NO* procuraMenor(struct NO* atual){
    struct NO *no1 = atual;
    struct NO *no2 = atual->esq;
    while(no2 != NULL){
        no1 = no2;
        no2 = no2->esq;
    }
    return no1;
}

// Remove um valor da árvore AVL, mantendo-a balanceada.
int remove_ArvAVL(ArvAVL *raiz, int valor){
    if(*raiz == NULL){ 
        return 0;
    }

    int res;
    if(valor < (*raiz)->info){
        res = remove_ArvAVL(&(*raiz)->esq,valor);
        if(res == 1){ 
            if(fatorBalanceamento_NO(*raiz) == -2){ 
                if(fatorBalanceamento_NO((*raiz)->dir) <= 0)
                    RotacaoRR(raiz);
                else
                    RotacaoRL(raiz);
            }
        }
    } else if((*raiz)->info < valor){
        res = remove_ArvAVL(&(*raiz)->dir, valor);
        if(res == 1){ 
            if(fatorBalanceamento_NO(*raiz) == 2){ 
                if(fatorBalanceamento_NO((*raiz)->esq) >= 0)
                    RotacaoLL(raiz);
                else
                    RotacaoLR(raiz);
            }
        }
    } else { 
        if(((*raiz)->esq == NULL || (*raiz)->dir == NULL)){
            struct NO *oldNode = (*raiz);
            if((*raiz)->esq != NULL)
                *raiz = (*raiz)->esq;
            else
                *raiz = (*raiz)->dir;
            free(oldNode);
            res = 1;
        }else {
            struct NO* temp = procuraMenor((*raiz)->dir);
            (*raiz)->info = temp->info;
            res = remove_ArvAVL(&(*raiz)->dir, temp->info);
            if(res == 1){
                if(fatorBalanceamento_NO(*raiz) == 2){
                    if(fatorBalanceamento_NO((*raiz)->esq) >= 0)
                        RotacaoLL(raiz);
                    else
                        RotacaoLR(raiz);
                }
            }
        }
    }

    if (*raiz != NULL && res == 1)
        (*raiz)->altura = maior(altura_NO((*raiz)->esq),altura_NO((*raiz)->dir)) + 1;
    
    return res;
}

// Constrói uma lista encadeada dos elementos da árvore em ordem crescente.
void iterator_ArvAVL(ArvAVL *raiz, struct iterator **iter){
    if(raiz == NULL)
        return;
    if(*raiz != NULL){
        iterator_ArvAVL(&((*raiz)->esq),iter);

        struct iterator* new_node = (struct iterator*) malloc(sizeof(struct iterator));
        if (new_node == NULL) {
            fprintf(stderr, "Erro de alocacao de memoria para o iterador da Arvore AVL.\n");
            return;
        }
        new_node->valor = (*raiz)->info;
        new_node->prox = NULL;

        if (*iter == NULL) {
            *iter = new_node;
        } else {
            struct iterator *current = *iter;
            while (current->prox != NULL) {
                current = current->prox;
            }
            current->prox = new_node;
        }

        iterator_ArvAVL(&((*raiz)->dir),iter);
    }
}