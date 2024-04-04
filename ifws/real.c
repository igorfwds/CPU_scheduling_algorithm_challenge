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
    int arriving_time;
    int next_arriving_time;
    int running_for;
    int could_run;
    char feedback;
} Process;

Process *findNextProcess(Process *processes, int p_lines, int current_time);
void updateProcessState(Process *process, int current_time);
void executeProcess(Process *process, int current_time);
void populateProcess(Process *processesArray,int processLinesCount, FILE *file, int total_t);
int nextArrive(Process *processes, int p_lines, int current_time, int total_t);
void rate(int total_time, Process *processes, int p_lines);
void edf(int total_time, Process *processes, int p_lines);
int rateSort(const void *a, const void *b);


int main(int argc, char **argv) {
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
    int lixo;
    fscanf(ptr_file, "%d", &lixo);

    populateProcess(processes,p_lines, ptr_file, total_time);


    
    if(strcmp(argv[0], "./rate") == 0){
        printf("EXECUTION BY RATE\n");
        rate(total_time, processes, p_lines);

    }else if(strcmp(argv[0], "./edf") == 0){
        printf("\nEXECUTION BY EDF\n");    
        edf(total_time, processes, p_lines);
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
    

    fclose(ptr_file);
    free(processes);
    return 0;
}

void rate(int total_time,Process *processes, int p_lines ) {
    qsort(processes, p_lines, sizeof(Process), rateSort);
    int time = 0;
    int count_idle = 0;
    int executed = 0;
    // Process *next_process;
    while (time <= total_time) {
        for(int i=0; i < p_lines; i++){
            // printf("\n%s burst=> %d", processes[i].name,processes[i].remaining_burst);
            if(time == processes[i].arriving_time){
                processes[i].remaining_burst = processes[i].CPU_burst;
            }
            if(time == total_time){
                if(processes[i].remaining_burst > 0){
                processes[i].feedback = 'K';
                processes[i].killed++;
                printf("[%s] for %d - %c\n", processes[i].name, processes[i].running_for, processes[i].feedback);
                processes[i].running_for = 0;
                break;   
                }
            }else{
                if(time == processes[i].next_arriving_time && processes[i].remaining_burst > 0){
                    processes[i].feedback = 'L';
                    processes[i].lost++;
                    processes[i].remaining_burst = processes[i].CPU_burst;
                    processes[i].arriving_time += processes[i].period;
                    processes[i].next_arriving_time += processes[i].period;
                    printf("[%s] for %d - %c\n", processes[i].name, processes[i].running_for, processes[i].feedback);
                    processes[i].running_for = 0;
                    break;
                }
                if(processes[i].remaining_burst > 0 && time >= processes[i].arriving_time){
                    executeProcess(&processes[i], time);
                    executed = 1;
                }else if(processes[i].remaining_burst == 0){
                    if(time < processes[i].arriving_time){
                        printf("idle for %d\n", count_idle);
                        break;
                    }
                    processes[i].feedback = 'F';
                    processes[i].completed++;
                    processes[i].arriving_time += processes[i].period;
                    processes[i].next_arriving_time += processes[i].period;
                    printf("[%s] for %d - %c\n", processes[i].name, processes[i].running_for, processes[i].feedback);
                    processes[i].remaining_burst = processes[i].CPU_burst;
                    processes[i].running_for = 0;
                    executed = 0;

                    break;
                }
            }

        }
        // printf(" TEMPO=>%d\n", time);

        if(executed == 0){
            count_idle++;
        }
        time++;
    }
}

void populateProcess(Process *processesArray,int processLinesCount, FILE *file, int total_t){
    for (int i = 0; i < processLinesCount; i++) {
        processesArray[i].name = malloc(1000 * sizeof(char));
        if (processesArray[i].name == NULL) {
            printf("Erro ao alocar memória para o nome do processo.\n");
            
        }

        fscanf(file, "%s %d %d", processesArray[i].name, &processesArray[i].period, &processesArray[i].CPU_burst);

        processesArray[i].lost = 0;
        processesArray[i].completed = 0;
        processesArray[i].killed = 0;
        processesArray[i].remaining_burst = processesArray[i].CPU_burst;
        processesArray[i].feedback = 'H';
        processesArray[i].running_for = 0;
        int number_of_arives = (total_t/processesArray[i].period);
        processesArray[i].could_run = number_of_arives;
        processesArray[i].arriving_time = 0;
        processesArray[i].next_arriving_time = processesArray[i].period;
        
    }
}

void edf(int total_time, Process *processes, int p_lines){}

void executeProcess(Process *process, int current_time) {
    if (process->remaining_burst > 0) {
        process->remaining_burst--;
        process->running_for++;
    }
}



int rateSort(const void *a, const void *b) {
    Process *processA = (Process *)a;
    Process *processB = (Process *)b;
    return processA->period - processB->period;
}

Process *findNextProcess(Process *processes, int p_lines, int current_time) {
    Process *next_process = NULL;
    for (int i = 0; i < p_lines; i++) {
            if (processes[i].remaining_burst > 0) {
                if (next_process == NULL || processes[i].period < next_process->period)  {
                    next_process = &processes[i];
                    //estou aqui
                }
            }
    }
    return next_process;
}

void updateProcessState(Process *process, int current_time) {
    if (process->remaining_burst == 0) {
        process->feedback = 'F';
        process->completed ++;
        printf("[%s] for %d - %c\n", process->name, process->CPU_burst, process->feedback);
    } else if (process->remaining_burst > 0 && current_time == process->arriving_time) {
        printf("%s perdeu um ciclo de execução.\n", process->name);
    }
}

