#include "Set.h"    
#include "../ArvoreAVL/ArvoreAVL.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Definição do tipo Set
struct set {
  ArvAVL *arv;
  int qtd;
  struct iterator *iter; 
};

// Cria e inicializa um novo conjunto (Set).
Set *criaSet() {
  Set *s = (Set *)malloc(sizeof(Set));
  if (s != NULL) {
    s->arv = cria_ArvAVL();
    if (s->arv == NULL) { 
        free(s);
        return NULL;
    }
    s->qtd = 0;
    s->iter = NULL;
  }
  return s;
}

// Libera toda a memória alocada para o conjunto.
void liberaSet(Set *s) {
  if (s != NULL) {
    libera_ArvAVL(s->arv); 

    struct iterator *current_iter = s->iter;
    while (current_iter != NULL) {
      struct iterator *next_iter = current_iter->prox;
      free(current_iter);
      current_iter = next_iter;
    }
    s->iter = NULL; 

    free(s);
  }
}

// Insere um número inteiro no conjunto.
int insereSet(Set *s, int num) {
  if (s == NULL)
    return 0;

  if (insere_ArvAVL(s->arv, num)) {
    s->qtd++;
    return 1;
  } else
    return 0;
}

// Remove um número inteiro do conjunto.
int removeSet(Set *s, int num) {
  if (s == NULL)
    return 0;

  if (remove_ArvAVL(s->arv, num)) {
    s->qtd--;
    return 1;
  } else
    return 0;
}

// Retorna o tamanho (quantidade de elementos) do conjunto.
int tamanhoSet(Set *s) {
  if (s == NULL)
    return 0;

  return s->qtd;
}

// Consulta se um número inteiro existe no conjunto.
int consultaSet(Set *s, int num) {
  if (s == NULL)
    return 0;

  return consulta_ArvAVL(s->arv, num);
}

// Imprime os elementos do conjunto em ordem.
void imprimeSet(Set *s) {
  if (s == NULL)
    return;

  emOrdem_ArvAVL(s->arv);
  printf("\n");
}

// Inicia o iterador do conjunto para percorrer seus elementos.
void beginSet(Set *s) {
  if (s == NULL)
    return;

  struct iterator *current_iter = s->iter;
  while (current_iter != NULL) {
      struct iterator *next_iter = current_iter->prox;
      free(current_iter);
      current_iter = next_iter;
  }
  s->iter = NULL; 

  iterator_ArvAVL(s->arv, &(s->iter));
}

// Verifica se o iterador do conjunto chegou ao fim.
int endSet(Set *s) {
  if (s == NULL || s->iter == NULL)
    return 1;
  return 0;
}

// Move o iterador do conjunto para o próximo elemento.
void nextSet(Set *s) {
  if (s == NULL || s->iter == NULL)
    return;

  s->iter = s->iter->prox;
}

// Obtém o valor do elemento atual apontado pelo iterador.
void getItemSet(Set *s, int *num) {
  if (s == NULL || s->iter == NULL || num == NULL) {
    return;
  }
  *num = s->iter->valor;
}

// Calcula a união entre dois conjuntos (A U B).
Set *uniaoSet(Set *A, Set *B) {
  if (A == NULL || B == NULL)
    return NULL;
  int x;
  Set *C = criaSet();
  if (C == NULL) return NULL; 

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

// Calcula a interseção entre dois conjuntos (A ∩ B).
Set *interseccaoSet(Set *A, Set *B) {
  if (A == NULL || B == NULL)
    return NULL;
  int x;
  Set *C = criaSet();
  if (C == NULL) return NULL; 

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

// Calcula a diferença entre dois conjuntos (A - B).
Set *diferencaSet(Set *A, Set *B) {
  if (A == NULL || B == NULL) {
    return NULL;
  }

  int x;
  Set *C = criaSet();
  if (C == NULL) return NULL; 

  for (beginSet(A); !endSet(A); nextSet(A)) {
    getItemSet(A, &x);
    if (!consultaSet(B, x)) {
      insereSet(C, x);
    }
  }

  return C;
}

// Avalia uma expressão booleana entre dois conjuntos usando um operador.
Set *avaliaExpressao(Set *left, char *operador, Set *right) {
  if (left == NULL || right == NULL || operador == NULL) return NULL; 
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