#include<stdio.h>
#include<stdlib.h>
#include<string.h>

void rate();
void edf();




typedef struct Process {
    char *name; 
    int period;
    int CPU_burst;
    int remaining_burst;
    int lost;
    int completed;
    int killed;
} Process;

int main(int argc, char *argv[]) {
    int total_time,p_lines;
    int count = 0;
    char line[256];

    if (argc < 2) {
        printf("Por favor, forneça o nome do arquivo como argumento.\n");
        return 1;
    }

    FILE *ptr_file = fopen(argv[1], "r");
    if (ptr_file == NULL) {
        printf("Erro ao abrir o arquivo.\n");
        return 1;
    }

    
    fscanf(ptr_file, "%d", &total_time); 

    while (fgets(line, sizeof(line), ptr_file)) { //cuidado com o tamanho da linha
        count++;
    }
    rewind(ptr_file); 

    p_lines = count - 1;
    

    Process *processes = malloc((p_lines) * sizeof(Process));
    if (processes == NULL) {
        printf("Erro ao alocar memória.\n");
        return 1;
    }

    fgets(line, sizeof(line), ptr_file);

    for (int i = 0; i < count - 1; i++) {
        processes[i].name = malloc(1000 * sizeof(char));
        if (processes[i].name == NULL) {
            printf("Erro ao alocar memória para o nome do processo.\n");
            return 1;
        }

        fscanf(ptr_file, "%s %d %d", processes[i].name, &processes[i].period, &processes[i].CPU_burst);
    }

    
    if(strcmp(argv[0], "./rate") == 0){
        rate(total_time, processes, p_lines);
    }else if(strcmp(argv[0], "./edf") == 0){
        edf(total_time, processes, p_lines);
    }

    
    fclose(ptr_file);
    free(processes);
    return 0;
}


void rate(int total_time, Process *processes, int p_lines){
    int time =0;


}

void edf(int total_time, Process *processes, int p_lines){
    int time =0;
    
}

