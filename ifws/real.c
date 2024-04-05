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
    int is_running;
    int last_remaining_burst;
    char feedback;
} Process;

void executeProcess(Process *process, int current_time, int idle_time);
void populateProcess(Process *processesArray,int processLinesCount, FILE *file, int total_t);
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
    int idle_processes = 0;
    int executed = 0;
    Process *lastExecuted = NULL;
    while (time <= total_time) {
        idle_processes = 0;
    // printf("\n TEMPO=>%d\t", time);
        
        
        for(int i=0; i < p_lines; i++){
            if(processes[i].remaining_burst == processes[i].last_remaining_burst){
                idle_processes++;
                printf("o idle é =>%d e tem %d processos no array", idle_processes, p_lines);
            }
            if(time == processes[i].arriving_time){
                processes[i].remaining_burst = processes[i].CPU_burst;
            }
    // printf("%s burst=> %d\t", processes[i].name,processes[i].remaining_burst);
            if(time == total_time){
                if(processes[i].remaining_burst > 0 && processes[i].arriving_time <= total_time){
                    processes[i].feedback = 'K';
                    processes[i].killed++;
                    if(processes[i].CPU_burst > processes[i].remaining_burst){
                    printf("\n[%s] for %d units - %c\n", processes[i].name, processes[i].running_for, processes[i].feedback);
                    }
                    processes[i].is_running = 0;
                    processes[i].running_for = 0;
                    continue;
                }
            }
            
            
            if(lastExecuted != NULL && time == processes[i].arriving_time){
                if(strcmp(lastExecuted->name, processes[i].name) != 0 && time != total_time && lastExecuted->remaining_burst > 0 && lastExecuted->is_running == 1){
                    lastExecuted->is_running = 0;
                    lastExecuted->feedback = 'H';
                    processes[i].remaining_burst = processes[i].CPU_burst;
                    printf("\n[%s] for %d units - %c\n", lastExecuted->name, lastExecuted->running_for, lastExecuted->feedback);
                    executeProcess(&processes[i], time, count_idle);
                    lastExecuted->running_for = 0;
                    processes[i].running_for = 1;
                    lastExecuted = &processes[i];
                    executed = 1;
                    break;
                }
            }else if(time == processes[i].next_arriving_time && processes[i].remaining_burst > 0 && time != total_time){
                processes[i].feedback = 'L';
                processes[i].lost++;
                processes[i].arriving_time += processes[i].period;
                processes[i].next_arriving_time += processes[i].period;
                processes[i].is_running = 0;
                processes[i].remaining_burst = processes[i].CPU_burst;
                printf("\n[%s] for %d units - %c\n", processes[i].name, processes[i].running_for, processes[i].feedback);
                executeProcess(&processes[i], time, count_idle);
                lastExecuted = &processes[i];
                executed = 1;
                processes[i].running_for = 1;
                break;
            }
            
            if(processes[i].remaining_burst > 0 && time >= processes[i].arriving_time){
                executeProcess(&processes[i], time, count_idle);
                if(count_idle > 0){
                    printf("idle for %d\n", count_idle);
                    count_idle = 0;
                }
                lastExecuted = &processes[i];
                executed = 1;
                break;
            }else if(processes[i].remaining_burst == 0 && time > processes[i].arriving_time && time < processes[i].next_arriving_time && executed == 1){
                
                processes[i].feedback = 'F';
                processes[i].is_running = 0;
                processes[i].completed++;
                if(processes[i].next_arriving_time + processes[i].period <= total_time){
                    processes[i].next_arriving_time += processes[i].period;
                }
                processes[i].arriving_time += processes[i].period;
                printf("\n[%s] for %d units - %c\n", processes[i].name, processes[i].running_for, processes[i].feedback);
                executeProcess(&processes[i], time, count_idle);
                lastExecuted->remaining_burst = lastExecuted->CPU_burst;
                processes[i].running_for = 0;
                lastExecuted = &processes[i];
                executed = 1;

            }
            // else if(executed == 0){
            //     if(time < processes[i].next_arriving_time && time >= processes[i].arriving_time){
            //         printf("idle for %d\n", count_idle);
            //         count_idle = 0;
            //         break;
            //     }
            // }
            

            
        }
        for(int i=0; i < p_lines; i++){
            
        }
        if(idle_processes == p_lines){
            count_idle++;
            printf("o Count idle é %d", count_idle);
        }

        // puts(lastExecuted->name);
        // if(time == 25){
        // //     printf("O next arriving time de %s é %d e o arriving time é %d",processes[0].name, processes[0].next_arriving_time, processes[0].arriving_time );
        //     printf("O next arriving time de %s é %d e o arriving time é %d, já o remaining burst é %d",processes[1].name, processes[1].next_arriving_time, processes[1].arriving_time, processes[1].remaining_burst );
        // }
        time++;
    }
    printf("o idle é %d", count_idle);
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
        processesArray[i].is_running = 0;
        processesArray[i].last_remaining_burst = processesArray[i].remaining_burst;
        processesArray[i].next_arriving_time = processesArray[i].period;
        
    }
}

void edf(int total_time, Process *processes, int p_lines){}

void executeProcess(Process *process, int current_time, int idle_time) {

    if (process->remaining_burst > 0) {
        // if(idle_time > 0){
        //     printf("idle for %d\n", idle_time);
        // }
        process->last_remaining_burst = process->remaining_burst;
        process->remaining_burst--;
        process->running_for++;
        process->is_running = 1;
    }
    return;
}


int rateSort(const void *a, const void *b) {
    Process *processA = (Process *)a;
    Process *processB = (Process *)b;
    return processA->period - processB->period;
}