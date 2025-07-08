#include <stdio.h>
#include <stdlib.h>
#include "ArvoreAVL.h" //inclui os Protótipos

// This struct is fine, no changes needed here.
struct NO{
    int info;
    int altura;
    struct NO *esq;
    struct NO *dir;
};

ArvAVL* cria_ArvAVL(){
    ArvAVL* raiz = (ArvAVL*) malloc(sizeof(ArvAVL));
    if(raiz != NULL)
        *raiz = NULL;
    return raiz;
}

void libera_NO(struct NO* no){
    if(no == NULL)
        return;
    libera_NO(no->esq);
    libera_NO(no->dir);
    free(no);
    // no = NULL; // Setting to NULL here has no effect on the caller's pointer
}

void libera_ArvAVL(ArvAVL* raiz){
    if(raiz == NULL)
        return;
    libera_NO(*raiz);//libera cada nó
    free(raiz);//libera a raiz
}

int altura_NO(struct NO* no){
    if(no == NULL)
        return -1;
    else
    return no->altura;
}

int fatorBalanceamento_NO(struct NO* no){
    return altura_NO(no->esq) - altura_NO(no->dir); // Correct calculation for balance factor
}

int maior(int x, int y){
    if(x > y)
        return x;
    else
        return y;
}

int estaVazia_ArvAVL(ArvAVL *raiz){
    if(raiz == NULL)
        return 1;
    if(*raiz == NULL)
        return 1;
    return 0;
}

int totalNO_ArvAVL(ArvAVL *raiz){
    if (raiz == NULL || *raiz == NULL) // Combined null checks
        return 0;
    int alt_esq = totalNO_ArvAVL(&((*raiz)->esq));
    int alt_dir = totalNO_ArvAVL(&((*raiz)->dir));
    return(alt_esq + alt_dir + 1);
}

int altura_ArvAVL(ArvAVL *raiz){
    if (raiz == NULL || *raiz == NULL) // Combined null checks
        return 0;
    int alt_esq = altura_ArvAVL(&((*raiz)->esq));
    int alt_dir = altura_ArvAVL(&((*raiz)->dir));
    if (alt_esq > alt_dir)
        return (alt_esq + 1);
    else
        return(alt_dir + 1);
}

void preOrdem_ArvAVL(ArvAVL *raiz){
    if(raiz == NULL)
        return;
    if(*raiz != NULL){
        
        printf("No %d: %d\n",(*raiz)->info,altura_NO(*raiz));
        preOrdem_ArvAVL(&((*raiz)->esq));
        preOrdem_ArvAVL(&((*raiz)->dir));
    }
}

void emOrdem_ArvAVL(ArvAVL *raiz){
    if(raiz == NULL)
        return;
    if(*raiz != NULL){
        emOrdem_ArvAVL(&((*raiz)->esq));
        printf("%d, ",(*raiz)->info);
        emOrdem_ArvAVL(&((*raiz)->dir));
    }
}

void posOrdem_ArvAVL(ArvAVL *raiz){
    if(raiz == NULL)
        return;
    if(*raiz != NULL){
        posOrdem_ArvAVL(&((*raiz)->esq));
        posOrdem_ArvAVL(&((*raiz)->dir));
        printf("%d\n",(*raiz)->info);
    }
}

int consulta_ArvAVL(ArvAVL *raiz, int valor){
    if(raiz == NULL || *raiz == NULL) // Combined null checks
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
// Rotations are standard AVL rotations, no major changes required here.
void RotacaoLL(ArvAVL *A){//LL
    struct NO *B;
    B = (*A)->esq;
    (*A)->esq = B->dir;
    B->dir = *A;
    (*A)->altura = maior(altura_NO((*A)->esq),altura_NO((*A)->dir)) + 1;
    B->altura = maior(altura_NO(B->esq),(*A)->altura) + 1;
    *A = B;
}

void RotacaoRR(ArvAVL *A){//RR
    struct NO *B;
    B = (*A)->dir;
    (*A)->dir = B->esq;
    B->esq = (*A);
    (*A)->altura = maior(altura_NO((*A)->esq),altura_NO((*A)->dir)) + 1;
    B->altura = maior(altura_NO(B->dir),(*A)->altura) + 1;
    (*A) = B;
}

void RotacaoLR(ArvAVL *A){//LR
    RotacaoRR(&(*A)->esq);
    RotacaoLL(A);
}

void RotacaoRL(ArvAVL *A){//RL
    RotacaoLL(&(*A)->dir);
    RotacaoRR(A);
}

int insere_ArvAVL(ArvAVL *raiz, int valor){
    int res;
    if(*raiz == NULL){//árvore vazia ou nó folha
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
            // Balanceamento após inserção na subárvore esquerda
            if(fatorBalanceamento_NO(atual) == 2){ // Use actual balance factor
                if(fatorBalanceamento_NO(atual->esq) == 1){ // Use actual balance factor of child
                    RotacaoLL(raiz);
                }else{ // -1
                    RotacaoLR(raiz);
                }
            }
        }
    }else{
        if(valor > atual->info){
            if((res = insere_ArvAVL(&(atual->dir), valor)) == 1){
                // Balanceamento após inserção na subárvore direita
                if(fatorBalanceamento_NO(atual) == -2){ // Use actual balance factor
                    if(fatorBalanceamento_NO(atual->dir) == -1){ // Use actual balance factor of child
                        RotacaoRR(raiz);
                    }else{ // 1
                        RotacaoRL(raiz);
                    }
                }
            }
        }else{
            return 0; // Valor já existe na árvore
        }
    }

    atual->altura = maior(altura_NO(atual->esq),altura_NO(atual->dir)) + 1;

    return res;
}

struct NO* procuraMenor(struct NO* atual){
    struct NO *no1 = atual;
    struct NO *no2 = atual->esq;
    while(no2 != NULL){
        no1 = no2;
        no2 = no2->esq;
    }
    return no1;
}

int remove_ArvAVL(ArvAVL *raiz, int valor){
    if(*raiz == NULL){ // valor não existe na árvore
        return 0;
    }

    int res;
    if(valor < (*raiz)->info){
        res = remove_ArvAVL(&(*raiz)->esq,valor);
        if(res == 1){ // If removal was successful in left subtree, check balance
            if(fatorBalanceamento_NO(*raiz) == -2){ // Right heavy
                if(fatorBalanceamento_NO((*raiz)->dir) <= 0) // RR or RL
                    RotacaoRR(raiz);
                else
                    RotacaoRL(raiz);
            }
        }
    } else if((*raiz)->info < valor){
        res = remove_ArvAVL(&(*raiz)->dir, valor);
        if(res == 1){ // If removal was successful in right subtree, check balance
            if(fatorBalanceamento_NO(*raiz) == 2){ // Left heavy
                if(fatorBalanceamento_NO((*raiz)->esq) >= 0) // LL or LR
                    RotacaoLL(raiz);
                else
                    RotacaoLR(raiz);
            }
        }
    } else { // (*raiz)->info == valor - Node to be removed
        if(((*raiz)->esq == NULL || (*raiz)->dir == NULL)){// nó tem 1 filho ou nenhum
            struct NO *oldNode = (*raiz);
            if((*raiz)->esq != NULL)
                *raiz = (*raiz)->esq;
            else
                *raiz = (*raiz)->dir;
            free(oldNode);
            res = 1;
        }else { // nó tem 2 filhos
            struct NO* temp = procuraMenor((*raiz)->dir); // Find the smallest in the right subtree
            (*raiz)->info = temp->info; // Copy its info to the current node
            res = remove_ArvAVL(&(*raiz)->dir, temp->info); // Remove the copied node from the right subtree
            if(res == 1){
                if(fatorBalanceamento_NO(*raiz) == 2){ // Left heavy after removal in right subtree
                    if(fatorBalanceamento_NO((*raiz)->esq) >= 0)
                        RotacaoLL(raiz);
                    else
                        RotacaoLR(raiz);
                }
            }
        }
    }

    if (*raiz != NULL && res == 1) // Only update height if node still exists and removal was successful
        (*raiz)->altura = maior(altura_NO((*raiz)->esq),altura_NO((*raiz)->dir)) + 1;
    
    return res;
}


// Modified iterator_ArvAVL to build the list in ascending order (in-order traversal)
// This will make `beginSet`, `nextSet`, `endSet` work more intuitively.
void iterator_ArvAVL(ArvAVL *raiz, struct iterator **iter){
    if(raiz == NULL)
        return;
    if(*raiz != NULL){
        // Traverse left subtree
        iterator_ArvAVL(&((*raiz)->esq),iter);

        // Process current node (add to list)
        struct iterator* new_node = (struct iterator*) malloc(sizeof(struct iterator));
        if (new_node == NULL) {
            // Handle memory allocation error
            fprintf(stderr, "Erro de alocacao de memoria para o iterador da Arvore AVL.\n");
            return;
        }
        new_node->valor = (*raiz)->info;
        new_node->prox = NULL; // Initially, this new node is the last one in the list

        // Append to the end of the existing list (or start a new list)
        if (*iter == NULL) {
            *iter = new_node;
        } else {
            struct iterator *current = *iter;
            while (current->prox != NULL) {
                current = current->prox;
            }
            current->prox = new_node;
        }

        // Traverse right subtree
        iterator_ArvAVL(&((*raiz)->dir),iter);
    }
}