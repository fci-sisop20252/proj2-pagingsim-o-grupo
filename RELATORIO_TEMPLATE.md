# Relatório do Projeto 2: Simulador de Memória Virtual

**Disciplina:** Sistemas Operacionais
**Professor:** Lucas Figueiredo
**Data:**

## Integrantes do Grupo

- Kauan Rotondaro Dias Alves - 10440110
- luan  - Matrícula
- lucas  - Matrícula

---

## 1. Instruções de Compilação e Execução

### 1.1 Compilação

Descreva EXATAMENTE como compilar seu projeto. Inclua todos os comandos necessários.
Compilar o simulador usando gcc dessa maneira:

gcc simulador.c -o simulador

### 1.2 Execução
Para executar o codigo da maneira correta e tendo uma saida de txt com a compilação final use :

**Exemplo com FIFO:**
```bash
./simulador fifo tests/config_1.txt tests/acessos_1.txt > output.txt
```

**Exemplo com Clock:**
```bash
./simulador clock tests/config_1.txt tests/acessos_1.txt > output.txt
```

---

## 2. Decisões de Design

### 2.1 Estruturas de Dados

Descreva as estruturas de dados que você escolheu para representar:

**Tabela de Páginas:**
Tabela de Páginas

- Estrutura utilizada: array de structs, cada índice representa uma página virtual.
- Para cada página são armazenados:
- presente (V-bit)
- frame onde a página está carregada
- rbit (Referenced bit)
- Cada processo possui sua própria tabela, garantindo isolamento.
Justificativa:
   Um array é suficiente e eficiente, pois acessamos páginas por índice. É simples, rápido e direto para simular tabelas de páginas do sistema operacional.

**Frames Físicos:**
Frames Físicos

- Estrutura utilizada: array de structs, onde cada posição representa um frame da memória física.

Informações armazenadas:
- ocupado (indica se o frame está em uso)
- pid do processo dono da página
- pagina dentro do processo
- rbit do frame

   Justificativa:
   O array torna fácil percorrer todos os frames, encontrar frames livres e acessar diretamente a página carregada.

**Estrutura para FIFO:**
- Estrutura utilizada: fila circular implementada com array.

- Armazena índices de frames na ordem em que foram ocupados.

- Para substituir, basta remover o elemento no head.
   Justificativa:
   FIFO depende exclusivamente da ordem de chegada, e uma fila circular é a estrutura mais simples e eficiente para isso.

**Estrutura para Clock:**
- Usamos um ponteiro circular (índice inteiro) que avança de 0 até nframes - 1.
Em cada frame:
- Se rbit = 1, damos segunda chance e zeramos o bit.
- Se rbit = 0, este frame é selecionado como vítima.
   Justificativa:
   A implementação é leve e eficiente, imitando perfeitamente o comportamento do algoritmo Clock real.

### 2.2 Organização do Código

simulador.c
├── main()                          # Coordena todo o programa
├── achar_processo()                # Encontra processo pelo PID
├── achar_frame_livre()             # Retorna frame livre ou -1
├── escolher_vitima_clock()         # Implementação do algoritmo Clock
├── FIFO: fila circular             # push/pop para substituição
└── lógica principal de simulação   # Tradução, HIT, PAGE FAULT, etc.


### 2.3 Algoritmo FIFO

**Funcionamento (em palavras)**

- Cada página carregada ocupa um frame.
- O índice do frame é inserido no final da fila FIFO.
- Quando ocorre page fault e não há frame livre:
   - O frame no início da fila é selecionado como vítima.
   - Removemos ele (pop) e inserimos o novo frame (push).
- O FIFO não considera o uso recente, apenas a ordem de chegada.

### 2.4 Algoritmo Clock
- Mantemos um ponteiro circular que percorre os frames.

- Ao encontrar um frame:
   - Se rbit == 1:
      - Damos segunda chance: zeramos o rbit e avançamos.
   - Se rbit == 0:
      - Esta página é escolhida como vítima.
- Inserimos a nova página e o ponteiro avança para o próximo frame.
- O rbit é sempre setado como 1 em todo acesso (HIT ou PAGE FAULT).

### 2.5 Tratamento de Page Fault

Explique como seu código distingue e trata os dois cenários:

**Cenário 1: Frame livre disponível**
- Procuramos um frame com ocupado = 0.
- Carregamos a nova página:
   -  setamos presente=1, frame=f, rbit=1.
- Para FIFO: adicionamos o frame à fila.

**Cenário 2: Memória cheia (substituição)**
- Se o algoritmo é FIFO → seleciona vítima pelo pop da fila.
- Se Clock → usa ponteiro circular + rbit.
- Removemos a página antiga:
   - atualizamos sua tabela (presente=0)
- Inserimos a nova página no mesmo frame.
---

## 3. Análise Comparativa FIFO vs Clock

### 3.1 Resultados dos Testes

Preencha a tabela abaixo com os resultados de pelo menos 3 testes diferentes:

| Descrição do Teste | Total de Acessos | Page Faults FIFO | Page Faults Clock | Diferença |
|-------------------|------------------|------------------|-------------------|-----------|
| Teste 1 - Básico  |                |                 |                   |           |
| Teste 2 - Memória Pequena |          |                  |                   |           |
| Teste 3 - Simples |                 |                  |                  |           |
| Teste Próprio 1   |              |                 |                  |           |

### 3.2 Análise

Com base nos resultados acima, responda:

1. **Qual algoritmo teve melhor desempenho (menos page faults)?**

2. **Por que você acha que isso aconteceu?** Considere:
   - Como cada algoritmo escolhe a vítima
   - O papel do R-bit no Clock
   - O padrão de acesso dos testes

3. **Em que situações Clock é melhor que FIFO?**
   - Dê exemplos de padrões de acesso onde Clock se beneficia

4. **Houve casos onde FIFO e Clock tiveram o mesmo resultado?**
   - Por que isso aconteceu?

5. **Qual algoritmo você escolheria para um sistema real e por quê?**

---

## 4. Desafios e Aprendizados

### 4.1 Maior Desafio Técnico

Descreva o maior desafio técnico que seu grupo enfrentou durante a implementação:

- Qual foi o problema?
- Como identificaram o problema?
- Como resolveram?
- O que aprenderam com isso?

### 4.2 Principal Aprendizado

Descreva o principal aprendizado sobre gerenciamento de memória que vocês tiveram com este projeto:

- O que vocês não entendiam bem antes e agora entendem?
- Como este projeto mudou sua compreensão de memória virtual?
- Que conceito das aulas ficou mais claro após a implementação?

---

## 5. Vídeo de Demonstração

**Link do vídeo:** [Insira aqui o link para YouTube, Google Drive, etc.]

### Conteúdo do vídeo:

Confirme que o vídeo contém:

- [ ] Demonstração da compilação do projeto
- [ ] Execução do simulador com algoritmo FIFO
- [ ] Execução do simulador com algoritmo Clock
- [ ] Explicação da saída produzida
- [ ] Comparação dos resultados FIFO vs Clock
- [ ] Breve explicação de uma decisão de design importante

---

## Checklist de Entrega

Antes de submeter, verifique:

- [ ] Código compila sem erros conforme instruções da seção 1.1
- [ ] Simulador funciona corretamente com FIFO
- [ ] Simulador funciona corretamente com Clock
- [ ] Formato de saída segue EXATAMENTE a especificação do ENUNCIADO.md
- [ ] Testamos com os casos fornecidos em tests/
- [ ] Todas as seções deste relatório foram preenchidas
- [ ] Análise comparativa foi realizada com dados reais
- [ ] Vídeo de demonstração foi gravado e link está funcionando
- [ ] Todos os integrantes participaram e concordam com a submissão

---
## Referências
Liste aqui quaisquer referências que utilizaram para auxiliar na implementação (livros, artigos, sites, **links para conversas com IAs.**)


---

## Comentários Finais

Use este espaço para quaisquer observações adicionais que julguem relevantes (opcional).

---
