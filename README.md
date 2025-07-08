# Buscador de Tweets - Projeto Final de ORI

Este repositório contém o projeto final para a disciplina de **ORI (Organização e Recuperação da Informação)**. O projeto é um **buscador de posts em um corpus de Tweets**, desenvolvido em **C**, utilizando estruturas de dados como **Árvores AVL**, **Tabelas Hash** e **Conjuntos** para dar suporte à recuperação eficiente baseada em palavras-chave.

-----

## 🧠 Descrição do Projeto

O objetivo principal deste projeto é criar uma ferramenta de linha de comando que permita:

  * Carregar um arquivo `.csv` contendo múltiplos posts no formato de tweets.
  * Indexar as palavras de cada post usando uma **Tabela Hash**, garantindo eficiência na busca e no armazenamento.
  * Permitir que o usuário realize **buscas booleanas** complexas utilizando os operadores lógicos `AND`, `OR` e `NOT`.
  * Exibir a lista de tweets que correspondem à consulta fornecida pelo usuário, de forma clara e organizada.
  * Utilizar **Árvores AVL** e **Conjuntos** para gerenciar e combinar os resultados das consultas de maneira otimizada.

### 📄 Formato de Entrada

Cada linha no arquivo CSV (`corpus.csv`) representa um tweet, seguindo o formato:

```
id,ignorado,texto
```

Exemplo:

```
14,0, jb isn't showing in australia any more!
```

-----

## ⚙️ Tecnologias e Estruturas de Dados Utilizadas

  * **Tabela Hash**: Essencial para a **indexação das palavras**, mapeando cada termo aos IDs dos posts que o contêm. A implementação lida eficientemente com colisões via **encadeamento separado (separate chaining)**.
  * **Conjunto (Set)**: Representa os conjuntos de resultados das consultas de busca e é fundamental para a realização das operações booleanas. Internamente, ele utiliza **Árvores AVL** para armazenar os IDs de forma única e ordenada.
  * **Árvore AVL**: Garante operações de inserção, remoção e busca em Conjuntos com complexidade **O(log n)**, assegurando que o sistema mantenha alta performance, mesmo com grandes volumes de dados, através de seu auto-balanceamento.
  * **Leitura de CSV**: Implementação robusta para interpretar e carregar os dados do `corpus.csv`, processando cada linha para extrair o ID e o texto do tweet.

-----

## 🔎 Sintaxe de Consulta

O usuário pode inserir consultas no terminal seguindo um formato intuitivo:

  * **Busca por uma única palavra:**
    ```
    hello
    ```
  * **Consultas com duas palavras e um operador:**
    ```
    goodbye AND exams
    frank OR lampard
    world NOT human
    ```

Todos os termos são tratados sem distinção de maiúsculas/minúsculas e a pontuação é ignorada durante a fase de indexação, tornando a busca mais flexível.

-----

## 🗂️ Estrutura do Projeto

A organização do projeto segue uma abordagem modular para facilitar a manutenção e a compreensão:

```
.
├── main.c              # Ponto de entrada do programa e interpretador de consultas
├── corpus.csv          # Conjunto de dados de entrada (tweets) [não incluído diretamente no repositório]
├── exporta.csv         # Mapa de palavra-ID indexado (arquivo gerado pelo programa)
│
├── Hash/
│   ├── TabelaHash.c    # Lógica da Tabela Hash (indexação e busca por palavra)
│   └── TabelaHash.h
│
├── Set/
│   ├── Set.c           # Implementação das operações de Conjunto usando árvores AVL
│   └── Set.h
│
├── ArvoreAVL/
│   ├── ArvoreAVL.c     # Implementação da árvore AVL (inserção, remoção, balanceamento)
│   └── ArvoreAVL.h
```

-----

## ▶️ Como Compilar e Executar

Para colocar o buscador em funcionamento, siga estes passos:

1.  **Verifique o `corpus.csv`**: Certifique-se de que o arquivo `corpus.csv` esteja presente no diretório raiz do projeto e contenha dados válidos.

2.  **Compile o Projeto**: Utilize um compilador C (como o `gcc`) para compilar todos os arquivos fonte. No terminal, na raiz do projeto, execute:

    ```bash
    gcc main.c Hash/TabelaHash.c Set/Set.c ArvoreAVL/ArvoreAVL.c -o buscador -Wall -Wextra
    ```

      * `Set/Set.c` e `ArvoreAVL/ArvoreAVL.c` são referenciados com seus caminhos corretos.
      * `-o buscador`: Define o nome do arquivo executável.
      * `-Wall -Wextra`: Ativa avisos adicionais que ajudam a identificar potenciais problemas no código.

3.  **Execute o Programa**: Após a compilação bem-sucedida, rode o executável:

    ```bash
    ./buscador
    ```

O programa irá então pedir suas consultas no terminal. Para sair, basta digitar `sair`.

-----

## 📤 Saída

O buscador exibirá no terminal os textos completos dos tweets que correspondem à sua consulta.

-----

## 🧾 Observações Importantes

  * Para otimização da busca, toda a pontuação é removida e as palavras são normalizadas (convertidas para minúsculas) antes de serem indexadas na Tabela Hash.
  * O sistema atual **não suporta expressões booleanas aninhadas** (ex: `(A AND B) OR C`). As consultas são limitadas a uma palavra única ou duas palavras conectadas por um único operador.
  * A implementação priorizou a **modularização do código** e a demonstração prática da utilização de estruturas de dados como Tabelas Hash, Conjuntos e Árvores AVL, conforme abordado na disciplina de ORI.

-----

