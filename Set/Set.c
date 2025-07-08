#include "Set.h"       //inclui os Protótipos
#include "ArvoreAVL.h" //inclui os Protótipos
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Definição do tipo Set
struct set {
  ArvAVL *arv;
  int qtd;
  struct iterator *iter; // This iterator is for the AVL tree, not directly for the set's external iteration
};

Set *criaSet() {
  Set *s = (Set *)malloc(sizeof(Set));
  if (s != NULL) {
    s->arv = cria_ArvAVL();
    if (s->arv == NULL) { // Check if AVL tree creation failed
        free(s);
        return NULL;
    }
    s->qtd = 0;
    s->iter = NULL;
  }
  return s;
}

void liberaSet(Set *s) {
  if (s != NULL) {
    libera_ArvAVL(s->arv); // This will also free the internal iterator of ArvAVL

    // The 'iter' in Set struct is likely meant for an external iteration which is recreated in beginSet.
    // So, no need to free it here if it's always recreated in beginSet.
    // However, if iterator_ArvAVL allocates new nodes *each time* and stores them in s->iter,
    // then we need to free them here.
    // Based on iterator_ArvAVL, it builds a new linked list. So, it needs to be freed.
    struct iterator *current_iter = s->iter;
    while (current_iter != NULL) {
      struct iterator *next_iter = current_iter->prox;
      free(current_iter);
      current_iter = next_iter;
    }
    s->iter = NULL; // Ensure it's null after freeing

    free(s);
  }
}

int insereSet(Set *s, int num) {
  if (s == NULL)
    return 0;

  if (insere_ArvAVL(s->arv, num)) {
    s->qtd++;
    return 1;
  } else
    return 0;
}

int removeSet(Set *s, int num) {
  if (s == NULL)
    return 0;

  if (remove_ArvAVL(s->arv, num)) {
    s->qtd--;
    return 1;
  } else
    return 0;
}

int tamanhoSet(Set *s) {
  if (s == NULL)
    return 0;

  return s->qtd;
}

int consultaSet(Set *s, int num) {
  if (s == NULL)
    return 0;

  return consulta_ArvAVL(s->arv, num);
}

void imprimeSet(Set *s) {
  if (s == NULL)
    return;

  emOrdem_ArvAVL(s->arv);
  printf("\n"); // Add a newline for better formatting after printing
}

void beginSet(Set *s) {
  if (s == NULL)
    return;

  // Free existing iterator before creating a new one to prevent memory leaks on repeated calls
  struct iterator *current_iter = s->iter;
  while (current_iter != NULL) {
      struct iterator *next_iter = current_iter->prox;
      free(current_iter);
      current_iter = next_iter;
  }
  s->iter = NULL; // Reset to NULL before rebuilding

  // Inicia o iterador da árvore AVL
  // iterator_ArvAVL builds the list in reverse order (right-root-left), so we need to reverse it or traverse differently.
  // The current iterator_ArvAVL builds a linked list from the AVL tree in reverse in-order.
  // To get it in ascending order (typical for set iteration), we would need to reverse this list,
  // or modify iterator_ArvAVL to build it in ascending order.
  // For now, I'll assume the current 'iterator_ArvAVL' behavior is acceptable,
  // but it's important to note the traversal order.
  iterator_ArvAVL(s->arv, &(s->iter));
}

int endSet(Set *s) {
  // Verifica se o iterador está no fim
  if (s == NULL || s->iter == NULL)
    return 1;
  return 0;
}

void nextSet(Set *s) {
  if (s == NULL || s->iter == NULL)
    return;

  // Mover o iterador para o próximo item
  s->iter = s->iter->prox;
}

void getItemSet(Set *s, int *num) {
  if (s == NULL || s->iter == NULL || num == NULL) {
    // Handle error or return a specific value if the iterator is invalid or num is NULL
    return;
  }
  *num = s->iter->valor;
}

Set *uniaoSet(Set *A, Set *B) {
  if (A == NULL || B == NULL)
    return NULL;
  int x;
  Set *C = criaSet();
  if (C == NULL) return NULL; // Check for allocation failure

  for (beginSet(A); !endSet(A); nextSet(A)) {
    getItemSet(A, &x);
    insereSet(C, x);
  }

  for (beginSet(B); !endSet(B); nextSet(B)) {
    getItemSet(B, &x);
    insereSet(C, x);
  }

  return C;
}

Set *interseccaoSet(Set *A, Set *B) {
  if (A == NULL || B == NULL)
    return NULL;
  int x;
  Set *C = criaSet();
  if (C == NULL) return NULL; // Check for allocation failure

  // Optimize by iterating over the smaller set
  if (tamanhoSet(A) < tamanhoSet(B)) {
    for (beginSet(A); !endSet(A); nextSet(A)) {
      getItemSet(A, &x);
      if (consultaSet(B, x))
        insereSet(C, x);
    }
  } else {
    for (beginSet(B); !endSet(B); nextSet(B)) {
      getItemSet(B, &x);
      if (consultaSet(A, x))
        insereSet(C, x);
    }
  }
  return C;
}

Set *diferencaSet(Set *A, Set *B) {
  if (A == NULL || B == NULL) {
    return NULL;
  }

  int x;
  Set *C = criaSet();
  if (C == NULL) return NULL; // Check for allocation failure

  for (beginSet(A); !endSet(A); nextSet(A)) {
    getItemSet(A, &x);
    if (!consultaSet(B, x)) {
      insereSet(C, x);
    }
  }

  return C;
}

Set *avaliaExpressao(Set *left, char *operador, Set *right) {
  if (left == NULL || right == NULL || operador == NULL) return NULL; // Added null checks
  Set *result = NULL;
  if (strcmp(operador, "AND") == 0) {
    result = interseccaoSet(left, right);
  } else if (strcmp(operador, "OR") == 0) {
    result = uniaoSet(left, right);
  } else if (strcmp(operador, "NOT") == 0) {
    result = diferencaSet(left, right);
  }
  return result;
}