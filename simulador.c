#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    int presente;   // 0 = não está na RAM, 1 = está na RAM
    int frame;      // índice do frame onde está
    int rbit;       // referenced bit
} EntradaPagina;

typedef struct {
    int pid;
    int tamanho_virtual;
    int num_paginas;
    EntradaPagina *tabela;
} Processo;

typedef struct {
    int ocupado;    // 0 = livre, 1 = ocupado
    int pid;        // dono do frame
    int pagina;     // página carregada
    int rbit;       // referenced do frame
} Frame;

/* ---------------- FIFO ---------------- */
typedef struct {
    int *fila;   // guarda frames na ordem de chegada
    int head, tail, count, cap;
} FilaFIFO;

void fifo_init(FilaFIFO *q, int cap) {
    q->fila = (int*) malloc(sizeof(int) * cap);
    q->head = q->tail = q->count = 0;
    q->cap = cap;
}

void fifo_push(FilaFIFO *q, int frame) {
    q->fila[q->tail] = frame;
    q->tail = (q->tail + 1) % q->cap;
    q->count++;
}

int fifo_pop(FilaFIFO *q) {
    int f = q->fila[q->head];
    q->head = (q->head + 1) % q->cap;
    q->count--;
    return f;
}

void fifo_free(FilaFIFO *q) {
    free(q->fila);
}

/* ------------- Utilidades ------------- */
Processo* achar_processo(Processo *procs, int nprocs, int pid) {
    for (int i = 0; i < nprocs; i++) {
        if (procs[i].pid == pid) return &procs[i];
    }
    return NULL;
}

int achar_frame_livre(Frame *frames, int nframes) {
    for (int i = 0; i < nframes; i++) {
        if (!frames[i].ocupado) return i;
    }
    return -1;
}

/* ---------------- CLOCK ---------------- */
int escolher_vitima_clock(Frame *frames, int nframes,
                          Processo *procs, int nprocs,
                          int *ponteiro_clock) {

    while (1) {
        int i = *ponteiro_clock;

        if (frames[i].ocupado) {
            if (frames[i].rbit == 0) {
                int vitima = i;
                *ponteiro_clock = (i + 1) % nframes;
                return vitima;
            } else {
                // segunda chance
                frames[i].rbit = 0;
                Processo *pvit = achar_processo(procs, nprocs, frames[i].pid);
                if (pvit) {
                    pvit->tabela[frames[i].pagina].rbit = 0;
                }
            }
        }

        *ponteiro_clock = (i + 1) % nframes;
    }
}

/* ---------------- MAIN ---------------- */
int main(int argc, char *argv[]) {
    if (argc != 4) {
        fprintf(stderr, "Uso: ./simulador <algoritmo> <arquivo_config> <arquivo_acessos>\n");
        return 1;
    }

    char algoritmo[16];
    strcpy(algoritmo, argv[1]);

    int usar_fifo = 0, usar_clock = 0;
    if (strcmp(algoritmo, "fifo") == 0) usar_fifo = 1;
    else if (strcmp(algoritmo, "clock") == 0) usar_clock = 1;
    else {
        fprintf(stderr, "Algoritmo inválido. Use fifo ou clock.\n");
        return 1;
    }

    /* ----- Lê config ----- */
    FILE *fcfg = fopen(argv[2], "r");
    if (!fcfg) { perror("Erro ao abrir arquivo_config"); return 1; }

    int num_frames, tam_pagina, num_procs;
    if (fscanf(fcfg, "%d", &num_frames) != 1) return 1;
    if (fscanf(fcfg, "%d", &tam_pagina) != 1) return 1;
    if (fscanf(fcfg, "%d", &num_procs) != 1) return 1;

    Processo *procs = (Processo*) malloc(sizeof(Processo) * num_procs);

    for (int i = 0; i < num_procs; i++) {
        int pid, tamvirt;
        fscanf(fcfg, "%d %d", &pid, &tamvirt);

        procs[i].pid = pid;
        procs[i].tamanho_virtual = tamvirt;
        procs[i].num_paginas = (tamvirt + tam_pagina - 1) / tam_pagina; // ceil
        procs[i].tabela = (EntradaPagina*) malloc(sizeof(EntradaPagina) * procs[i].num_paginas);

        for (int p = 0; p < procs[i].num_paginas; p++) {
            procs[i].tabela[p].presente = 0;
            procs[i].tabela[p].frame = -1;
            procs[i].tabela[p].rbit = 0;
        }
    }
    fclose(fcfg);

    /* ----- Inicializa RAM ----- */
    Frame *frames = (Frame*) malloc(sizeof(Frame) * num_frames);
    for (int i = 0; i < num_frames; i++) {
        frames[i].ocupado = 0;
        frames[i].pid = -1;
        frames[i].pagina = -1;
        frames[i].rbit = 0;
    }

    FilaFIFO fila_fifo;
    int ponteiro_clock = 0;

    if (usar_fifo) fifo_init(&fila_fifo, num_frames);

    /* ----- Processa acessos ----- */
    FILE *facc = fopen(argv[3], "r");
    if (!facc) { perror("Erro ao abrir arquivo_acessos"); return 1; }

    int total_acessos = 0;
    int total_faults = 0;

    int pid, addr;
    while (fscanf(facc, "%d %d", &pid, &addr) == 2) {
        total_acessos++;

        Processo *proc = achar_processo(procs, num_procs, pid);
        if (!proc) continue; // entrada inválida (não deve ocorrer)

        int pagina = addr / tam_pagina;
        int desloc = addr % tam_pagina;

        // HIT
        if (proc->tabela[pagina].presente) {
            int f = proc->tabela[pagina].frame;

            proc->tabela[pagina].rbit = 1;
            frames[f].rbit = 1;

            printf("Acesso: PID %d, Endereço %d (Página %d, Deslocamento %d) -> HIT: Página %d (PID %d) já está no Frame %d\n",
                   pid, addr, pagina, desloc, pagina, pid, f);
            continue;
        }

        // PAGE FAULT
        total_faults++;

        int frame_livre = achar_frame_livre(frames, num_frames);

        if (frame_livre != -1) {
            // Frame livre
            frames[frame_livre].ocupado = 1;
            frames[frame_livre].pid = pid;
            frames[frame_livre].pagina = pagina;
            frames[frame_livre].rbit = 1;

            proc->tabela[pagina].presente = 1;
            proc->tabela[pagina].frame = frame_livre;
            proc->tabela[pagina].rbit = 1;

            if (usar_fifo) fifo_push(&fila_fifo, frame_livre);

            printf("Acesso: PID %d, Endereço %d (Página %d, Deslocamento %d) -> PAGE FAULT -> Página %d (PID %d) alocada no Frame livre %d\n",
                   pid, addr, pagina, desloc, pagina, pid, frame_livre);

        } else {
            // Memória cheia, substituição
            int vitima_frame;

            if (usar_fifo) {
                vitima_frame = fifo_pop(&fila_fifo);
            } else {
                vitima_frame = escolher_vitima_clock(frames, num_frames, procs, num_procs, &ponteiro_clock);
            }

            int pid_antigo = frames[vitima_frame].pid;
            int pag_antiga = frames[vitima_frame].pagina;

            Processo *proc_antigo = achar_processo(procs, num_procs, pid_antigo);
            if (proc_antigo) {
                proc_antigo->tabela[pag_antiga].presente = 0;
                proc_antigo->tabela[pag_antiga].frame = -1;
                proc_antigo->tabela[pag_antiga].rbit = 0;
            }

            // coloca a nova página no frame da vítima
            frames[vitima_frame].pid = pid;
            frames[vitima_frame].pagina = pagina;
            frames[vitima_frame].rbit = 1;

            proc->tabela[pagina].presente = 1;
            proc->tabela[pagina].frame = vitima_frame;
            proc->tabela[pagina].rbit = 1;

            if (usar_fifo) fifo_push(&fila_fifo, vitima_frame);

            printf("Acesso: PID %d, Endereço %d (Página %d, Deslocamento %d) -> PAGE FAULT -> Memória cheia. Página %d (PID %d) (Frame %d) será desalocada. -> Página %d (PID %d) alocada no Frame %d\n",
                   pid, addr, pagina, desloc,
                   pag_antiga, pid_antigo, vitima_frame,
                   pagina, pid, vitima_frame);
        }
    }
    fclose(facc);

    /* ----- Resumo final ----- */
    printf("--- Simulação Finalizada (Algoritmo: %s)\n", algoritmo);
    printf("Total de Acessos: %d\n", total_acessos);
    printf("Total de Page Faults: %d\n", total_faults);

    /* ----- Limpeza ----- */
    if (usar_fifo) fifo_free(&fila_fifo);

    for (int i = 0; i < num_procs; i++) free(procs[i].tabela);
    free(procs);
    free(frames);

    return 0;
}
