#include<stdio.h>
#include<stdlib.h>
#include<string.h>




typedef struct Process {
    char *name; 
    int period;
    int CPU_burst;
    int remaining_burst;
    int lost;
    int completed;
    int killed;
    int deadline;
    char feedback[2];
} Process;


void rate(int total_time, Process *processes, int p_lines);
void edf(int total_time, Process *processes, int p_lines);
int rateSort(const void *a, const void *b);



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
        printf("\nEXECUTION BY RATE\n");
        rate(total_time, processes, p_lines);

    }else if(strcmp(argv[0], "./edf") == 0){
        printf("\nEXECUTION BY EDF\n");    
        edf(total_time, processes, p_lines);
    }

    

    fclose(ptr_file);
    free(processes);
    return 0;
}


void rate(int total_time, Process *processes, int p_lines){
    int time = 0;
    qsort(processes, p_lines, sizeof(Process), rateSort);
    
    int i = 0;
    while(time <= total_time){
        processes[i].feedback[0] = 'H';
        processes[i].feedback[1] = 0;
        processes[i].remaining_burst = processes[i].CPU_burst;
        processes[i].completed = 0;
        processes[i].killed = 0;
        processes[i].lost = 0;
        if( time == 0){
            processes[i].deadline = processes[i].period; 
        }else{
            processes[i].deadline += processes[i].period; 
        }
        while(processes[i].remaining_burst > 0){
            if(time >= processes[i].deadline){
                processes[i].lost++;
                processes[i].deadline += processes[i].period;
            }
            time++;
            processes[i].remaining_burst--;
            if(processes[i].remaining_burst == 0){
                processes[i].completed++;
                processes[i].feedback[0] = 'F';
                i++;
            }
        }
        
        time++;
    }

    for(int p = 0; p < p_lines; p++){
        printf("[%s] for %d units - %s\n", processes[p].name, processes[p].lost, processes[p].feedback);
    }

    printf("\nLOST DEADLINES\n");
    for(int p = 0; p < p_lines; p++){
        printf("[%s] %d\n", processes[p].name, processes[p].lost);
    }
    printf("\nCOMPLETE EXECUTION\n");
    for(int p = 0; p < p_lines; p++){
        printf("[%s] %d\n", processes[p].name, processes[p].completed);
    }
    printf("\nKILLED\n");
    for(int p = 0; p < p_lines; p++){
        printf("[%s] %d\n", processes[p].name, processes[p].killed);
    }
}

void edf(int total_time, Process *processes, int p_lines){
    int time = 0;
    while(time <= total_time){
        int i =0;
        processes[i].remaining_burst = processes[i].CPU_burst;
        if( time == 0){
            processes[i].deadline = processes[i].period; 
        }else{
            processes[i].deadline += processes[i].period; 
        }
        while(processes[i].remaining_burst > 0){
            if(time >= processes[i].deadline){
                processes[i].lost++;
                processes[i].deadline += processes[i].period;
            }
            time++;
            processes[i].remaining_burst--;
        }
    }

    
}

int rateSort(const void *a, const void *b) {
    Process *processA = (Process *)a;
    Process *processB = (Process *)b;
    return processA->period - processB->period;
}
