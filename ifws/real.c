#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct Process
{
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
    int originIndex;
    char feedback;
} Process;

FILE *rateFile = NULL;
FILE *edfFile = NULL;
void executeProcess(Process *process, int current_time);
void populateProcess(Process *processesArray, int processLinesCount, FILE *file, int total_t);
void rateMonotonicAlgorithm(int total_time, Process *processes, int p_lines);
void earliestDeadlineFirstAlgorithm(int total_time, Process *processes, int p_lines);
int rateSort(const void *a, const void *b);
int edfSort(const void *a, const void *b);
int originIndexSort(const void *a, const void *b);
void finishingProcess(Process *Process, int total_time, int time, struct Process **lastExecuted, int *executed);
void lostingProcess(Process *Process, int total_time, int time, struct Process **lastExecuted, int *executed);
void holdingProcess(Process *Process, int total_time, int time, struct Process **lastExecuted, int *executed);
void killingProcess(Process *Process, int total_time, int time, struct Process **lastExecuted, int *executed);

int main(int argc, char **argv)
{
    int total_time, p_lines;
    int count = 0;
    char line[10000];

    if (argc < 2)
    {
        printf("Por favor, forneça o nome do arquivo como argumento.\n");
        return 1;
    }

    FILE *ptr_file = fopen(argv[1], "r");
    if (ptr_file == NULL)
    {
        printf("Erro ao abrir o arquivo.\n");
        return 1;
    }

    fscanf(ptr_file, "%d", &total_time);

    while (fgets(line, sizeof(line), ptr_file))
    { // cuidado com o tamanho da linha
        count++;
    }
    rewind(ptr_file);

    p_lines = count - 1;

    Process *processes = malloc((p_lines) * sizeof(Process));
    if (processes == NULL)
    {
        printf("Erro ao alocar memória.\n");
        return 1;
    }
    int lixo;
    fscanf(ptr_file, "%d", &lixo);

    populateProcess(processes, p_lines, ptr_file, total_time);

    if (strcmp(argv[0], "./rate") == 0)
    {
        rateFile = fopen("rate.out", "w");
        fprintf(rateFile, "EXECUTION BY RATE\n");
        fclose(rateFile);

        rateMonotonicAlgorithm(total_time, processes, p_lines);

        qsort(processes, p_lines, sizeof(Process), originIndexSort);
        fopen("rate.out", "a");
        fprintf(rateFile, "\nLOST DEADLINES\n");
        for (int p = 0; p < p_lines; p++)
        {
            fprintf(rateFile, "[%s] %d\n", processes[p].name, processes[p].lost);
        }
        fprintf(rateFile, "\nCOMPLETE EXECUTION\n");
        for (int p = 0; p < p_lines; p++)
        {
            fprintf(rateFile, "[%s] %d\n", processes[p].name, processes[p].completed);
        }
        fprintf(rateFile, "\nKILLED\n");
        for (int p = 0; p < p_lines; p++)
        {
            if (p == p_lines - 1)
            {
                fprintf(rateFile, "[%s] %d", processes[p].name, processes[p].killed);
            }
            else
            {
                fprintf(rateFile, "[%s] %d\n", processes[p].name, processes[p].killed);
            }
        }
        fclose(rateFile);
    }
    else if (strcmp(argv[0], "./edf") == 0)
    {
        edfFile = fopen("edf.out", "w");
        fprintf(edfFile, "EXECUTION BY EDF\n");
        fclose(edfFile);

        earliestDeadlineFirstAlgorithm(total_time, processes, p_lines);

        qsort(processes, p_lines, sizeof(Process), originIndexSort);
        fopen("edf.out", "a");
        fprintf(edfFile, "\nLOST DEADLINES\n");
        for (int p = 0; p < p_lines; p++)
        {
            fprintf(edfFile, "[%s] %d\n", processes[p].name, processes[p].lost);
        }
        fprintf(edfFile, "\nCOMPLETE EXECUTION\n");
        for (int p = 0; p < p_lines; p++)
        {
            fprintf(edfFile, "[%s] %d\n", processes[p].name, processes[p].completed);
        }
        fprintf(edfFile, "\nKILLED\n");
        for (int p = 0; p < p_lines; p++)
        {
            if (p == p_lines - 1)
            {
                fprintf(edfFile, "[%s] %d", processes[p].name, processes[p].killed);
            }
            else
            {
                fprintf(edfFile, "[%s] %d\n", processes[p].name, processes[p].killed);
            }
        }
        fclose(edfFile);
    }

    fclose(ptr_file);
    free(processes);
    return 0;
}

void rateMonotonicAlgorithm(int total_time, Process *processes, int p_lines)
{
    qsort(processes, p_lines, sizeof(Process), rateSort);
    int time = 0;
    int count_idle = 0;
    int idle_processes = 0;
    int executed = 0;
    Process *lastExecuted = NULL;
    int remainBurst = 0;
    while (time <= total_time)
    {
        idle_processes = 0;
        printf("\n TEMPO=>%d\t", time);
        for (int x = 0; x < p_lines; x++)
        {
            printf("%s burst=> %d | arriving time=> %d | NAT=> %d\t", processes[x].name, processes[x].remaining_burst, processes[x].arriving_time, processes[x].next_arriving_time);
        }
        for (int i = 0; i < p_lines; i++)
        {
            // if (time == processes[i].arriving_time)
            // {
            //     processes[i].remaining_burst = processes[i].CPU_burst;
            // }
            if (processes[i].remaining_burst > 0 && time >= processes[i].arriving_time)
            {
                executeProcess(&processes[i], time);
                if (count_idle > 0)
                {
                    fopen("rate.out", "a");
                    fprintf(rateFile, "idle for %d units\n", count_idle);
                    fclose(rateFile);
                    count_idle = 0;
                }
                lastExecuted = &processes[i];
                executed = 1;
                break;
            }
        }

        printf("\nPOS EXECUÇÃO ");
        for (int x = 0; x < p_lines; x++)
        {
            printf(" %s burst=> %d | arriving time=> %d | NAT=> %d\t", processes[x].name, processes[x].remaining_burst, processes[x].arriving_time, processes[x].next_arriving_time);
        }
        for (int i = 0; i < p_lines; i++)
        {
            if (time == total_time)
            {
                if (count_idle > 0)
                {
                    fopen("rate.out", "a");
                    fprintf(rateFile, "idle for %d units\n", count_idle);
                    fclose(rateFile);
                    count_idle = 0;
                }
                if (processes[i].remaining_burst > 0 && processes[i].arriving_time <= total_time)
                {
                    killingProcess(&processes[i], total_time, time, &lastExecuted, &executed);
                }
            }

            if (lastExecuted != NULL && time == processes[i].arriving_time)
            {
                if (strcmp(lastExecuted->name, processes[i].name) != 0 && time != total_time && lastExecuted->remaining_burst > 0 && lastExecuted->is_running == 1)
                {
                    holdingProcess(&processes[i], total_time, time, &lastExecuted, &executed);
                    break;
                }
            }
            if (lastExecuted != NULL && time == processes[i].next_arriving_time && processes[i].remaining_burst > 0 && time != total_time)
            {
                lostingProcess(&processes[i], total_time, time, &lastExecuted, &executed);
                break;
            }

            else if (processes[i].remaining_burst == 0 && time >= processes[i].arriving_time && processes[i].running_for > 0)
            {
                finishingProcess(&processes[i], total_time, time, &lastExecuted, &executed);
                break;
            }
            else if ((time < processes[i].next_arriving_time && time >= processes[i].arriving_time) || time == total_time && count_idle > 0)
            {
                if (count_idle > 0)
                {
                    fopen("rate.out", "a");
                    fprintf(rateFile, "idle for %d units\n", count_idle);
                    fclose(rateFile);
                }
                count_idle = 0;
                break;
            }
        }
        if (executed == 0)
        {
            count_idle++;
        }
        time++;
        printf("\nPOS FLAGS T=%d",time);
        for (int x = 0; x < p_lines; x++)
        {
            printf(" %s burst=> %d | arriving time=> %d | NAT=> %d\t", processes[x].name, processes[x].remaining_burst, processes[x].arriving_time, processes[x].next_arriving_time);
            if (time == processes[x].next_arriving_time)
            {
                processes[x].remaining_burst = processes[x].CPU_burst;
                processes[x].arriving_time = processes[x].next_arriving_time;
                if (processes[x].next_arriving_time + processes[x].period <= total_time)
                {
                    processes[x].next_arriving_time += processes[x].period;
                }
            }
        }
        printf("\nPOS RESET AT E NAT ");
        for (int x = 0; x < p_lines; x++)
        {
            printf(" %s burst=> %d | arriving time=> %d | NAT=> %d\t", processes[x].name, processes[x].remaining_burst, processes[x].arriving_time, processes[x].next_arriving_time);
        }
        printf("IDLE = %d\n", count_idle);
        // printf("EXECUTED = %d", executed);
    }
    // printf("o idle é %d", count_idle);
}

void killingProcess(Process *Process, int total_time, int time, struct Process **lastExecuted, int *executed)
{
    Process->feedback = 'K';
    Process->killed++;
    if (Process->running_for > 0)
    {
        fopen("rate.out", "a");
        fprintf(rateFile, "[%s] for %d units - %c\n", Process->name, Process->running_for, Process->feedback);
        fclose(rateFile);
    }
    Process->is_running = 0;
    Process->running_for = 0;
}
void holdingProcess(Process *Process, int total_time, int time, struct Process **lastExecuted, int *executed)
{
    (*lastExecuted)->is_running = 0;
    (*lastExecuted)->feedback = 'H';
    Process->remaining_burst = Process->CPU_burst;
    if ((*lastExecuted)->running_for > 0)
    {
        fopen("rate.out", "a");
        fprintf(rateFile, "[%s] for %d units - %c\n", (*lastExecuted)->name, (*lastExecuted)->running_for, (*lastExecuted)->feedback);
        fclose(rateFile);
    }
    executeProcess(Process, time);
    (*lastExecuted)->running_for = 0;
    *lastExecuted = Process;
    *executed = 1;
}
void lostingProcess(Process *Process, int total_time, int time, struct Process **lastExecuted, int *executed)
{
    Process->feedback = 'L';
    Process->lost++;
    Process->arriving_time = Process->next_arriving_time;
    if (Process->next_arriving_time + Process->period <= total_time)
    {
        Process->next_arriving_time += Process->period;
    }
    Process->is_running = 0;
    Process->remaining_burst = Process->CPU_burst;
    if (Process->running_for > 0)
    {
        fopen("rate.out", "a");
        fprintf(rateFile, "[%s] for %d units - %c\n", Process->name, Process->running_for, Process->feedback);
        fclose(rateFile);
    }
    executeProcess(Process, time);
    Process->running_for = 0;
    *lastExecuted = Process;
    *executed = 1;
}
void finishingProcess(Process *Process, int total_time, int time, struct Process **lastExecuted, int *executed)
{
    Process->feedback = 'F';
    Process->is_running = 0;
    Process->completed++;
    Process->arriving_time = Process->next_arriving_time;
    if (Process->next_arriving_time + Process->period <= total_time)
    {
        Process->next_arriving_time += Process->period;
    }
    if (Process->CPU_burst > Process->remaining_burst && Process->running_for > 0)
    {
        fopen("rate.out", "a");
        fprintf(rateFile, "[%s] for %d units - %c\n", Process->name, Process->running_for, Process->feedback);
        fclose(rateFile);
    }
    executeProcess(Process, time);
    Process->running_for = 0;
    *lastExecuted = Process;
    *executed = 1;
}

void populateProcess(Process *processesArray, int processLinesCount, FILE *file, int total_t)
{
    for (int i = 0; i < processLinesCount; i++)
    {
        processesArray[i].name = malloc(1000 * sizeof(char));
        if (processesArray[i].name == NULL)
        {
            printf("Erro ao alocar memória para o nome do processo.\n");
        }

        fscanf(file, "%s %d %d", processesArray[i].name, &processesArray[i].period, &processesArray[i].CPU_burst);

        processesArray[i].lost = 0;
        processesArray[i].completed = 0;
        processesArray[i].killed = 0;
        processesArray[i].remaining_burst = processesArray[i].CPU_burst;
        processesArray[i].feedback = 'H';
        processesArray[i].running_for = 0;
        int number_of_arives = (total_t / processesArray[i].period);
        processesArray[i].could_run = number_of_arives;
        processesArray[i].arriving_time = 0;
        processesArray[i].is_running = 0;
        processesArray[i].last_remaining_burst = processesArray[i].remaining_burst;
        processesArray[i].next_arriving_time = processesArray[i].period;
        processesArray[i].originIndex = i;
    }
}

void earliestDeadlineFirstAlgorithm(int total_time, Process *processes, int p_lines)
{
    qsort(processes, p_lines, sizeof(Process), edfSort);

    int time = 0;
    int count_idle = 0;
    int idle_processes = 0;
    int executed = 0;
    Process *lastExecuted = NULL;
    while (time <= total_time)
    {
        qsort(processes, p_lines, sizeof(Process), edfSort);
        idle_processes = 0;
        // printf("\n TEMPO=>%d\t", time);
        for (int i = 0; i < p_lines; i++)
        {
            if (time == processes[i].arriving_time)
            {
                processes[i].remaining_burst = processes[i].CPU_burst;
            }
            // printf("%s burst=> %d | Deadline=> %d | arrive=> %d\t", processes[i].name,processes[i].remaining_burst, processes[i].next_arriving_time, processes[i].arriving_time);
            if (time == total_time)
            {
                if (count_idle > 0)
                {
                    fopen("edf.out", "a");
                    fprintf(edfFile, "idle for %d units\n", count_idle);
                    fclose(edfFile);
                    count_idle = 0;
                }
                if (processes[i].remaining_burst > 0 && processes[i].arriving_time <= total_time)
                {
                    processes[i].feedback = 'K';
                    processes[i].killed++;
                    if (processes[i].CPU_burst > processes[i].remaining_burst)
                    {
                        if (processes[i].running_for > 0)
                        {
                            fopen("edf.out", "a");
                            fprintf(edfFile, "[%s] for %d units - %c\n", processes[i].name, processes[i].running_for, processes[i].feedback);
                            fclose(edfFile);
                        }
                    }
                    processes[i].is_running = 0;
                    processes[i].running_for = 0;
                    continue;
                }
            }

            if (lastExecuted != NULL && time == processes[i].arriving_time)
            {
                if (strcmp(lastExecuted->name, processes[i].name) != 0 && time != total_time && lastExecuted->remaining_burst > 0 && lastExecuted->is_running == 1)
                {
                    lastExecuted->is_running = 0;
                    lastExecuted->feedback = 'H';
                    processes[i].remaining_burst = processes[i].CPU_burst;
                    fopen("edf.out", "a");
                    fprintf(edfFile, "[%s] for %d units - %c\n", lastExecuted->name, lastExecuted->running_for, lastExecuted->feedback);
                    fclose(edfFile);
                    executeProcess(&processes[i], time);
                    lastExecuted->running_for = 0;
                    processes[i].running_for = 1;
                    lastExecuted = &processes[i];
                    executed = 1;
                    break;
                }
            }
            else if (time == processes[i].next_arriving_time && processes[i].remaining_burst > 0 && time != total_time)
            {
                processes[i].feedback = 'L';
                processes[i].lost++;
                processes[i].arriving_time += processes[i].period;
                processes[i].next_arriving_time += processes[i].period;
                processes[i].is_running = 0;
                processes[i].remaining_burst = processes[i].CPU_burst;
                if (processes[i].running_for > 0)
                {
                    fopen("edf.out", "a");
                    fprintf(edfFile, "[%s] for %d units - %c\n", processes[i].name, processes[i].running_for, processes[i].feedback);
                    fclose(edfFile);
                }
                executeProcess(&processes[i], time);
                lastExecuted = &processes[i];
                executed = 1;
                processes[i].running_for = 1;
                break;
            }

            if (processes[i].remaining_burst > 0 && time >= processes[i].arriving_time)
            {
                executeProcess(&processes[i], time);
                if (count_idle > 0)
                {
                    fopen("edf.out", "a");
                    fprintf(edfFile, "idle for %d units\n", count_idle);
                    fclose(edfFile);
                    count_idle = 0;
                }
                lastExecuted = &processes[i];
                executed = 1;
                break;
            }
            else if (processes[i].remaining_burst == 0 && time > processes[i].arriving_time && executed == 1)
            {

                processes[i].feedback = 'F';
                processes[i].is_running = 0;
                processes[i].completed++;
                processes[i].next_arriving_time += processes[i].period;
                processes[i].arriving_time += processes[i].period;
                fopen("edf.out", "a");
                fprintf(edfFile, "[%s] for %d units - %c\n", processes[i].name, processes[i].running_for, processes[i].feedback);
                fclose(edfFile);
                executeProcess(&processes[i], time);
                lastExecuted->remaining_burst = lastExecuted->CPU_burst;
                processes[i].running_for = 0;
                lastExecuted = &processes[i];
                executed = 0;
            }
            else if ((time < processes[i].next_arriving_time && time >= processes[i].arriving_time) || time == total_time && count_idle > 0)
            {
                fopen("edf.out", "a");
                fprintf(edfFile, "idle for %d units\n", count_idle);
                fclose(edfFile);
                count_idle = 0;
                break;
            }
        }
        if (executed == 0)
        {
            count_idle++;
        }

        // printf(" COUNT IDLE = %d", count_idle);
        // printf("EXECUTED = %d", executed);
        time++;
    }
    // printf("o idle é %d", count_idle);
}

void executeProcess(Process *process, int current_time)
{

    if (process->remaining_burst > 0)
    {

        process->last_remaining_burst = process->remaining_burst;
        process->remaining_burst--;
        process->running_for++;
        process->is_running = 1;
    }
    return;
}

int rateSort(const void *a, const void *b)
{
    Process *processA = (Process *)a;
    Process *processB = (Process *)b;
    return processA->period - processB->period;
}

int edfSort(const void *a, const void *b)
{
    Process *processA = (Process *)a;
    Process *processB = (Process *)b;
    return processA->next_arriving_time - processB->next_arriving_time;
}

int originIndexSort(const void *a, const void *b)
{
    Process *processA = (Process *)a;
    Process *processB = (Process *)b;
    return processA->originIndex - processB->originIndex;
}