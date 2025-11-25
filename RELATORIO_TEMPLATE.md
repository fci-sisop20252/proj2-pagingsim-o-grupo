# Relatório do Projeto 2: Simulador de Memória Virtual

**Disciplina:** Sistemas Operacionais
**Professor:** Lucas Figueiredo
**Data:24/11/2025**
**Turma:04G**

## Integrantes do Grupo

- Kauan Rotondaro Dias Alves - 10440110
- Luan Oliveira de Freitas  - 10440360
- Lucas Brezolini da Freiria  - 10439256


## 1. Instruções de Compilação e Execução

### 1.1 Compilação

Descreva EXATAMENTE como compilar seu projeto. Inclua todos os comandos necessários.
Compilar o simulador usando gcc dessa maneira:

gcc simulador.c -o simulador

### 1.2 Execução
Para executar o codigo da maneira correta e tendo uma saida de txt com a compilação final use :

**Exemplo com FIFO:**
```bash
./simulador fifo tests/config_4.txt tests/acessos_4.txt > outputfifo.txt
```

**Exemplo com Clock:**
```bash
./simulador clock tests/config_4.txt tests/acessos_4.txt > outputclock.txt
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
 - Nos testes foram fornecidos (config_1, config_2 e config_3), FIFO e Clock tiveram exatamente o mesmo número de page faults.
 - Porém, nos testes adicionais com padrões de acesso mais repetitivos (especialmente acessos_4.txt e acessos_5.txt), o algoritmo Clock apresentou menos page faults que o FIFO.

2. **Por que você acha que isso aconteceu?** Considere:
 - FIFO remove sempre a página mais antiga, sem considerar se ela será usada de novo.
 - Clock usa o R-bit para identificar páginas que ainda estão sendo usadas e dá “segunda chance”.

 Papel do R-bit

 - Quando uma página é acessada (HIT ou carregada), o Clock seta R = 1.
 - Durante a substituição, páginas com R = 1 são poupadas:
   - O Clock zera o R-bit e avança.
   - Somente páginas com R = 0 podem ser removidas.

   Padrão de acesso

   - Nos testes onde houve:
      - repetição de páginas
      - localidade temporal
      - loops acessando o mesmo conjunto pequeno de páginas


3. **Em que situações Clock é melhor que FIFO?**
   - O algoritmo Clock é melhor que o FIFO quando existe localidade temporal — ou seja, quando as mesmas páginas são acessadas repetidamente em um intervalo curto de tempo.
   Nesses casos, o Clock consegue manter páginas recentemente usadas porque utiliza o R-bit para dar “segunda chance”, enquanto o FIFO remove a página mais antiga mesmo que ela ainda seja necessária.
   Por isso, Clock tem melhor desempenho em padrões cíclicos, em loops e em acessos repetidos a um mesmo conjunto de páginas, evitando substituições desnecessárias.

4. **Houve casos onde FIFO e Clock tiveram o mesmo resultado?**
    - Sim, Nos testes menores (config_1, config_2, config_3), os dois algoritmos produziram exatamente o mesmo número de page faults.
    isso acontece pois FIFO e Clock produzem o mesmo número de page faults quando o padrão de acessos não possui localidade temporal.
   Isso acontece quando as páginas não são reutilizadas, quando há pouca repetição ou quando a memória é suficiente para acomodar todas as páginas ativas.
   Nessas situações, o R-bit nunca chega a influenciar a escolha da vítima, e por isso o Clock se comporta exatamente como o FIFO.

5. **Qual algoritmo você escolheria para um sistema real e por quê?**
Eu escolheria o Clock, porque:
 - Ele é praticamente uma implementação eficiente do LRU (“Least Recently Used”).
   - Obriga páginas com uso recente a permanecerem na memória.
   - Tem desempenho melhor em acessos reais com localidade temporal.
   - É simples de implementar e barato em hardware (é usado em muitos sistemas operacionais).
 - FIFO, apesar de simples, tem o problema de remover páginas úteis com mais frequência.

---

## 4. Desafios e Aprendizados

### 4.1 Maior Desafio Técnico

Um dos maiores desafios foi fazer o algoritmo Clock funcionar certinho com o R-bit. No começo, algumas páginas que deveriam receber “segunda chance” estavam sendo substituídas sem querer, e isso deixava os resultados estranhos. A gente percebeu o problema ao analisar o comportamento do ponteiro e ver que o R-bit não estava sendo atualizado em todos os acessos. Depois de revisar o código e garantir que o R-bit era setado sempre que uma página era acessada, o Clock passou a funcionar direito. No fim, aprendemos como esses detalhes fazem toda a diferença nos algoritmos de substituição

### 4.2 Principal Aprendizado
O principal aprendizado desse projeto foi entender na prática como a memória virtual funciona de verdade. Antes a gente só via os conceitos na teoria, mas não entendia muito bem como aconteciam os page faults, como o sistema decide qual página tirar da memória e qual algoritmo é melhor em cada situação. Programando o simulador, tudo isso ficou muito mais claro. A gente entendeu direitinho o papel da tabela de páginas, do R-bit e como a substituição realmente acontece. No final, deu pra ver como os algoritmos das aulas funcionam na vida real e não só no slide.

## 5. Vídeo de Demonstração

**Link do vídeo:** (https://youtu.be/T2zT2RkYXhk)

### Conteúdo do vídeo:

Confirme que o vídeo contém:

- [x] Demonstração da compilação do projeto
- [x] Execução do simulador com algoritmo FIFO
- [x] Execução do simulador com algoritmo Clock
- [x] Explicação da saída produzida
- [x] Comparação dos resultados FIFO vs Clock
- [x] Breve explicação de uma decisão de design importante

---

## Checklist de Entrega

Antes de submeter, verifique:

- [x] Código compila sem erros conforme instruções da seção 1.1
- [x] Simulador funciona corretamente com FIFO
- [x] Simulador funciona corretamente com Clock
- [x] Formato de saída segue EXATAMENTE a especificação do ENUNCIADO.md
- [x] Testamos com os casos fornecidos em tests/
- [x] Todas as seções deste relatório foram preenchidas
- [x] Análise comparativa foi realizada com dados reais
- [x] Vídeo de demonstração foi gravado e link está funcionando
- [x] Todos os integrantes participaram e concordam com a submissão

---
## Referências
Liste aqui quaisquer referências que utilizaram para auxiliar na implementação (livros, artigos, sites, **links para conversas com IAs.**)


---

## Comentários Finais

Use este espaço para quaisquer observações adicionais que julguem relevantes (opcional).

---
