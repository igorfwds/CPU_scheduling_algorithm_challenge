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
    char feedback;
    int running;
} Process;


void rate(int total_time, Process *processes, int p_lines);
void edf(int total_time, Process *processes, int p_lines);
int rateSort(const void *a, const void *b);
void populateProcess(Process *processesArray,int processLinesCount, FILE *file);


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
    int lixo;
    // fgets(line, sizeof(line), ptr_file);
    fscanf(ptr_file, "%d", &lixo);

    populateProcess(processes,p_lines, ptr_file);
    
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


void rate(int total_time, Process *processes, int p_lines){
    qsort(processes, p_lines, sizeof(Process), rateSort);
    int time = 0;
    int lost = 0;
    int remaining_burst;
    int process = 0;
    int completed = 0;
    int deadline;
    int period;
    for(;process <= p_lines; process++ ){
        remaining_burst = processes[process].CPU_burst;
        deadline = processes[process].deadline;
        period = processes[process].period;
        if(time == total_time){
            break;
        }
        if(remaining_burst == 0){
            remaining_burst = processes[process].CPU_burst;
        }
        
        if(time > deadline){
            deadline += period; 
        }
        while(remaining_burst > 0){
            if(time >= deadline){
                // processes[process].lost++;
                lost++;
                deadline += period;
                break;
            }
    
            time++;
            remaining_burst--;
            // processes[process].remaining_burst--;
            if(remaining_burst == 0){
                // processes[process].completed++;
                completed++;
                processes[process].feedback = 'F';
                break;
            }
        }
        processes[process].lost = lost;
        

        // process++; //logica certa mas lixo de cpu entrando na string do nome do primeiro processo
        time++;
        
    }


    for(int p = 0; p < p_lines; p++){
        printf("[%s] for %d units - %c\n", processes[p].name, (processes[p].CPU_burst - processes[p].remaining_burst), processes[p].feedback);
    }

    
}

void populateProcess(Process *processesArray,int processLinesCount, FILE *file){
    for (int i = 0; i < processLinesCount; i++) {
        processesArray[i].name = malloc(1000 * sizeof(char));
        if (processesArray[i].name == NULL) {
            printf("Erro ao alocar memória para o nome do processo.\n");
            
        }

        fscanf(file, "%s %d %d", processesArray[i].name, &processesArray[i].period, &processesArray[i].CPU_burst);

        processesArray[i].lost = 0;
        processesArray[i].completed = 0;
        processesArray[i].killed = 0;
        processesArray[i].deadline = processesArray[i].period;
        processesArray[i].feedback = 'H';
        processesArray[i].running = 0;
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
