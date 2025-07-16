# Buscador de Tweets - Projeto Final de ORI

Este repositório contém o projeto final para a disciplina de **ORI (Organização e Recuperação da Informação)**. O projeto é um **buscador de posts em um corpus de Tweets**, desenvolvido em **C**, utilizando estruturas de dados como **Árvores AVL**, **Tabelas Hash** e **Conjuntos** para dar suporte à recuperação eficiente baseada em palavras-chave.

**Autores:**

Murilo de Miranda Silva - RA: 812069

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
├── README.md           # Arquivo com informações sobre o projeto e como executá-lo
├── buscador            # O executável compilado do seu programa
├── output_log.txt      # Arquivo para logs de saída ou resultados de processamento
├── run_tests.sh        # Script shell para rodar testes automatizados
├── test_cases.txt      # Arquivo contendo os casos de teste para o script
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

Para colocar o buscador de tweets em funcionamento, siga estes passos:

1.  **Verifique o `corpus.csv`**: Certifique-se de que o arquivo `corpus.csv` esteja presente no diretório raiz do seu projeto e contenha dados válidos. Ele é rastreado usando o Git LFS, então garanta que você tenha o Git LFS instalado e que o arquivo foi baixado corretamente (veja a seção "Gerenciamento de Arquivos Grandes").

2.  **Compile o Projeto**: Você tem duas opções para compilar e executar o projeto:

    -----

    ### 2.1 Compilação Manual e Uso Interativo

    Este método é para interagir diretamente com o buscador via terminal.

      * **Compile**: Utilize um compilador C (como o `gcc`) para compilar todos os arquivos fonte. No terminal, a partir do diretório raiz do projeto, execute:

        ```bash
        gcc main.c Hash/TabelaHash.c Set/Set.c ArvoreAVL/ArvoreAVL.c -o buscador -Wall -Wextra
        ```

          * `-o buscador`: Define o nome do arquivo executável como `buscador`.
          * `-Wall -Wextra`: Ativa avisos adicionais que ajudam a identificar potenciais problemas no código.

      * **Execute**: Após a compilação bem-sucedida, execute o programa:

        ```bash
        ./buscador
        ```

        O programa então solicitará suas consultas no terminal. Para sair, basta digitar `sair`.

    -----

    ### 2.2 Testes Automatizados com o Script

    Este método utiliza o script fornecido para executar uma série de casos de teste predefinidos automaticamente.

      * **Execute o Script**: Simplesmente execute o seguinte comando:

        ```bash
        ./run_tests.sh
        ```

        Este script irá compilar seu projeto e, em seguida, executar todos os casos de teste definidos no arquivo `test_cases.txt`. Um log detalhado da saída de cada teste será gerado no arquivo `output_log.txt`.


## 🧾 Observações Importantes

  * Para otimização da busca, toda a pontuação é removida e as palavras são normalizadas (convertidas para minúsculas) antes de serem indexadas na Tabela Hash.
  * O sistema atual **não suporta expressões booleanas aninhadas** (ex: `(A AND B) OR C`). As consultas são limitadas a uma palavra única ou duas palavras conectadas por um único operador.
  * A implementação priorizou a **modularização do código** e a demonstração prática da utilização de estruturas de dados como Tabelas Hash, Conjuntos e Árvores AVL, conforme abordado na disciplina de ORI.

-----

## 📦 Gerenciamento de Arquivos Grandes (Git LFS)

⚠️ O arquivo corpus.csv é rastreado usando o Git LFS (Large File Storage).

Para clonar e executar este projeto corretamente, é fundamental ter o Git LFS instalado:

 ```bash
   # Configuração única (se ainda não estiver instalado)
   git lfs install

   # Clonar o repositório (os arquivos LFS serão baixados automaticamente)
   git clone https://github.com/MurilooMiranda/ORI.git
 ```

Caso você já tenha clonado o repositório antes de instalar o Git LFS, execute o seguinte comando para baixar o conteúdo real do corpus.csv:

```bash
   git lfs pull
 ```

Isso garante que o arquivo corpus.csv seja baixado corretamente e esteja disponível para o programa.

