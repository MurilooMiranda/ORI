#!/bin/bash

# Nome do executável do seu programa
EXECUTABLE="./buscador"

# Nome do arquivo de casos de teste
TEST_CASES_FILE="test_cases.txt"

# Nome do arquivo de log de saída
OUTPUT_LOG_FILE="output_log.txt"

echo "Iniciando testes do buscador de tweets..."
echo "A saída será redirecionada para $OUTPUT_LOG_FILE"
echo "---------------------------------------------------" > "$OUTPUT_LOG_FILE"

# Compila o programa (caso não tenha sido compilado ou haja alterações)
echo "Compilando o projeto..."
gcc main.c Set/Set.c ArvoreAVL/ArvoreAVL.c Hash/TabelaHash.c -o buscador -Wall -Wextra 2>&1 | tee -a "$OUTPUT_LOG_FILE"
if [ $? -ne 0 ]; then
    echo "Erro na compilação. Abortando testes." | tee -a "$OUTPUT_LOG_FILE"
    exit 1
fi
echo "Compilação concluída." | tee -a "$OUTPUT_LOG_FILE"
echo "---------------------------------------------------" | tee -a "$OUTPUT_LOG_FILE"


# Loop através de cada linha no arquivo de casos de teste
while IFS= read -r line || [[ -n "$line" ]]; do
    # Ignora linhas de comentário (começam com #) e linhas vazias
    if [[ "$line" =~ ^#.* ]] || [[ -z "$line" ]]; then
        echo "$line" >> "$OUTPUT_LOG_FILE" # Adiciona o comentário/linha vazia ao log
        continue
    fi

    # Se a linha for o próprio comando 'sair', tratamos de forma diferente para não enviar 'sair' duas vezes
    if [ "$line" == "sair" ]; then
        echo "" >> "$OUTPUT_LOG_FILE"
        echo "Executando teste final: '$line'" | tee -a "$OUTPUT_LOG_FILE"
        echo "Entrada: $line" >> "$OUTPUT_LOG_FILE"
        echo "$line" | $EXECUTABLE >> "$OUTPUT_LOG_FILE" 2>&1
        echo "Teste concluído." >> "$OUTPUT_LOG_FILE"
        echo "---------------------------------------------------" >> "$OUTPUT_LOG_FILE"
        break # Sai do loop após o comando 'sair' final
    fi

    echo "" >> "$OUTPUT_LOG_FILE" # Adiciona uma linha em branco para melhor legibilidade
    echo "Executando teste: '$line'" | tee -a "$OUTPUT_LOG_FILE"
    echo "Entrada: $line" >> "$OUTPUT_LOG_FILE"
    
    # Envia a consulta e em seguida o comando 'sair' para encerrar o programa
    (echo "$line"; echo "sair") | $EXECUTABLE >> "$OUTPUT_LOG_FILE" 2>&1
    
    echo "Teste concluído." >> "$OUTPUT_LOG_FILE"
    echo "---------------------------------------------------" >> "$OUTPUT_LOG_FILE"
done < "$TEST_CASES_FILE"

echo "Todos os testes foram executados. Verifique '$OUTPUT_LOG_FILE' para os resultados."