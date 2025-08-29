#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#define MAX_PROCESSES 100

// Structure to represent a process
typedef struct {
    int processId;
    int arrivalTime;
    int burstTime;
    int priority;
    int waitingTime;
    int completionTime;
    int turnaroundTime;
} ProcessInfo;

// Function prototypes
void firstComeFirstServe(ProcessInfo processArray[], int numberOfProcesses);
void shortestJobFirst(ProcessInfo processArray[], int numberOfProcesses);
void roundRobinScheduling(ProcessInfo processArray[], int numberOfProcesses, int timeQuantum);
void priorityScheduling(ProcessInfo processArray[], int numberOfProcesses);
void displayGanttChart(ProcessInfo processArray[], int numberOfProcesses);
void displayProcessTable(ProcessInfo processArray[], int numberOfProcesses, char algorithmName[]);
float calculateAverageWaitingTime(ProcessInfo processArray[], int numberOfProcesses);
void sortProcessesByBurstTime(ProcessInfo processArray[], int numberOfProcesses);
void sortProcessesByPriority(ProcessInfo processArray[], int numberOfProcesses);
void copyProcessArray(ProcessInfo source[], ProcessInfo destination[], int numberOfProcesses);

int main() {
    int numberOfProcesses, userChoice, timeQuantum;
    ProcessInfo originalProcesses[MAX_PROCESSES];
    ProcessInfo workingProcesses[MAX_PROCESSES];

    // Display welcome message
    printf("=======================================================\n");
    printf("        CPU Scheduling Algorithm Simulator\n");
    printf("=======================================================\n\n");

    // Get number of processes from user
    printf("Enter the number of processes (max %d): ", MAX_PROCESSES);
    scanf("%d", &numberOfProcesses);

    if (numberOfProcesses <= 0 || numberOfProcesses > MAX_PROCESSES) {
        printf("Invalid number of processes! Please enter a value between 1 and %d.\n", MAX_PROCESSES);
        return 1;
    }

    // Input process details
    printf("\nEnter process details:\n");
    printf("Format: [Arrival Time] [Burst Time] [Priority]\n");
    printf("-----------------------------------------------\n");
    
    for (int processIndex = 0; processIndex < numberOfProcesses; processIndex++) {
        printf("Process P%d: ", processIndex + 1);
        scanf("%d %d %d", 
              &originalProcesses[processIndex].arrivalTime,
              &originalProcesses[processIndex].burstTime,
              &originalProcesses[processIndex].priority);
        
        originalProcesses[processIndex].processId = processIndex + 1;
        originalProcesses[processIndex].waitingTime = 0;
        originalProcesses[processIndex].completionTime = 0;
        originalProcesses[processIndex].turnaroundTime = 0;
    }

    // Main menu loop
    while (true) {
        printf("\n=======================================================\n");
        printf("           Select a Scheduling Algorithm\n");
        printf("=======================================================\n");
        printf("1. First Come First Serve (FCFS)\n");
        printf("2. Shortest Job First (SJF)\n");
        printf("3. Round Robin (RR)\n");
        printf("4. Priority Scheduling\n");
        printf("5. Exit Program\n");
        printf("=======================================================\n");
        printf("Enter your choice (1-5): ");
        scanf("%d", &userChoice);

        // Create a working copy of processes for each algorithm
        copyProcessArray(originalProcesses, workingProcesses, numberOfProcesses);

        switch (userChoice) {
            case 1:
                firstComeFirstServe(workingProcesses, numberOfProcesses);
                break;
            case 2:
                shortestJobFirst(workingProcesses, numberOfProcesses);
                break;
            case 3:
                printf("Enter time quantum: ");
                scanf("%d", &timeQuantum);
                if (timeQuantum <= 0) {
                    printf("Time quantum must be positive!\n");
                    break;
                }
                roundRobinScheduling(workingProcesses, numberOfProcesses, timeQuantum);
                break;
            case 4:
                priorityScheduling(workingProcesses, numberOfProcesses);
                break;
            case 5:
                printf("\nThank you for using CPU Scheduling Simulator!\n");
                exit(0);
            default:
                printf("Invalid choice! Please enter a number between 1-5.\n");
        }
    }

    return 0;
}

// Copy process array to preserve original data
void copyProcessArray(ProcessInfo source[], ProcessInfo destination[], int numberOfProcesses) {
    for (int i = 0; i < numberOfProcesses; i++) {
        destination[i] = source[i];
    }
}

// First Come First Serve Scheduling Algorithm
void firstComeFirstServe(ProcessInfo processArray[], int numberOfProcesses) {
    printf("\n--- First Come First Serve (FCFS) Scheduling ---\n");
    
    int currentTime = 0;
    
    // Calculate waiting time and completion time
    for (int processIndex = 0; processIndex < numberOfProcesses; processIndex++) {
        if (currentTime < processArray[processIndex].arrivalTime) {
            currentTime = processArray[processIndex].arrivalTime;
        }
        
        processArray[processIndex].waitingTime = currentTime - processArray[processIndex].arrivalTime;
        currentTime += processArray[processIndex].burstTime;
        processArray[processIndex].completionTime = currentTime;
        processArray[processIndex].turnaroundTime = 
            processArray[processIndex].completionTime - processArray[processIndex].arrivalTime;
    }
    
    displayProcessTable(processArray, numberOfProcesses, "FCFS");
}

// Shortest Job First Scheduling Algorithm
void shortestJobFirst(ProcessInfo processArray[], int numberOfProcesses) {
    printf("\n--- Shortest Job First (SJF) Scheduling ---\n");
    
    // Sort processes by burst time
    sortProcessesByBurstTime(processArray, numberOfProcesses);
    
    int currentTime = 0;
    
    // Calculate waiting time and completion time
    for (int processIndex = 0; processIndex < numberOfProcesses; processIndex++) {
        if (currentTime < processArray[processIndex].arrivalTime) {
            currentTime = processArray[processIndex].arrivalTime;
        }
        
        processArray[processIndex].waitingTime = currentTime - processArray[processIndex].arrivalTime;
        currentTime += processArray[processIndex].burstTime;
        processArray[processIndex].completionTime = currentTime;
        processArray[processIndex].turnaroundTime = 
            processArray[processIndex].completionTime - processArray[processIndex].arrivalTime;
    }
    
    displayProcessTable(processArray, numberOfProcesses, "SJF");
}

// Round Robin Scheduling Algorithm
void roundRobinScheduling(ProcessInfo processArray[], int numberOfProcesses, int timeQuantum) {
    printf("\n--- Round Robin Scheduling (Quantum = %d) ---\n", timeQuantum);
    
    int remainingBurstTime[numberOfProcesses];
    int currentTime = 0;
    
    // Initialize remaining burst times
    for (int processIndex = 0; processIndex < numberOfProcesses; processIndex++) {
        remainingBurstTime[processIndex] = processArray[processIndex].burstTime;
        processArray[processIndex].waitingTime = 0;
    }
    
    // Process execution loop
    bool allProcessesCompleted = false;
    while (!allProcessesCompleted) {
        allProcessesCompleted = true;
        
        for (int processIndex = 0; processIndex < numberOfProcesses; processIndex++) {
            if (remainingBurstTime[processIndex] > 0) {
                allProcessesCompleted = false;
                
                if (remainingBurstTime[processIndex] > timeQuantum) {
                    // Process runs for full quantum
                    currentTime += timeQuantum;
                    remainingBurstTime[processIndex] -= timeQuantum;
                } else {
                    // Process completes
                    currentTime += remainingBurstTime[processIndex];
                    processArray[processIndex].completionTime = currentTime;
                    processArray[processIndex].waitingTime = 
                        currentTime - processArray[processIndex].burstTime - processArray[processIndex].arrivalTime;
                    processArray[processIndex].turnaroundTime = 
                        processArray[processIndex].completionTime - processArray[processIndex].arrivalTime;
                    remainingBurstTime[processIndex] = 0;
                }
            }
        }
    }
    
    displayProcessTable(processArray, numberOfProcesses, "Round Robin");
}

// Priority Scheduling Algorithm
void priorityScheduling(ProcessInfo processArray[], int numberOfProcesses) {
    printf("\n--- Priority Scheduling ---\n");
    
    // Sort processes by priority (lower number = higher priority)
    sortProcessesByPriority(processArray, numberOfProcesses);
    
    int currentTime = 0;
    
    // Calculate waiting time and completion time
    for (int processIndex = 0; processIndex < numberOfProcesses; processIndex++) {
        if (currentTime < processArray[processIndex].arrivalTime) {
            currentTime = processArray[processIndex].arrivalTime;
        }
        
        processArray[processIndex].waitingTime = currentTime - processArray[processIndex].arrivalTime;
        currentTime += processArray[processIndex].burstTime;
        processArray[processIndex].completionTime = currentTime;
        processArray[processIndex].turnaroundTime = 
            processArray[processIndex].completionTime - processArray[processIndex].arrivalTime;
    }
    
    displayProcessTable(processArray, numberOfProcesses, "Priority");
}

// Sort processes by burst time (ascending order)
void sortProcessesByBurstTime(ProcessInfo processArray[], int numberOfProcesses) {
    for (int i = 0; i < numberOfProcesses - 1; i++) {
        for (int j = i + 1; j < numberOfProcesses; j++) {
            if (processArray[i].burstTime > processArray[j].burstTime) {
                ProcessInfo temp = processArray[i];
                processArray[i] = processArray[j];
                processArray[j] = temp;
            }
        }
    }
}

// Sort processes by priority (ascending order - lower number = higher priority)
void sortProcessesByPriority(ProcessInfo processArray[], int numberOfProcesses) {
    for (int i = 0; i < numberOfProcesses - 1; i++) {
        for (int j = i + 1; j < numberOfProcesses; j++) {
            if (processArray[i].priority > processArray[j].priority) {
                ProcessInfo temp = processArray[i];
                processArray[i] = processArray[j];
                processArray[j] = temp;
            }
        }
    }
}

// Display process scheduling results in a table format
void displayProcessTable(ProcessInfo processArray[], int numberOfProcesses, char algorithmName[]) {
    printf("\n%s Scheduling Results:\n", algorithmName);
    printf("=========================================================================\n");
    printf("Process\tArrival\tBurst\tPriority\tWaiting\tTurnaround\tCompletion\n");
    printf("=========================================================================\n");
    
    for (int processIndex = 0; processIndex < numberOfProcesses; processIndex++) {
        printf("P%d\t%d\t%d\t%d\t\t%d\t%d\t\t%d\n",
               processArray[processIndex].processId,
               processArray[processIndex].arrivalTime,
               processArray[processIndex].burstTime,
               processArray[processIndex].priority,
               processArray[processIndex].waitingTime,
               processArray[processIndex].turnaroundTime,
               processArray[processIndex].completionTime);
    }
    
    printf("=========================================================================\n");
    
    float averageWaitingTime = calculateAverageWaitingTime(processArray, numberOfProcesses);
    float averageTurnaroundTime = 0.0;
    
    for (int i = 0; i < numberOfProcesses; i++) {
        averageTurnaroundTime += processArray[i].turnaroundTime;
    }
    averageTurnaroundTime /= numberOfProcesses;
    
    printf("Average Waiting Time: %.2f units\n", averageWaitingTime);
    printf("Average Turnaround Time: %.2f units\n", averageTurnaroundTime);
    
    displayGanttChart(processArray, numberOfProcesses);
}

// Display Gantt Chart representation
void displayGanttChart(ProcessInfo processArray[], int numberOfProcesses) {
    printf("\nGantt Chart:\n");
    printf("============\n");
    
    // Print process boxes
    for (int processIndex = 0; processIndex < numberOfProcesses; processIndex++) {
        printf("| P%d ", processArray[processIndex].processId);
    }
    printf("|\n");
    
    // Print timeline
    int currentTime = 0;
    printf("0");
    for (int processIndex = 0; processIndex < numberOfProcesses; processIndex++) {
        currentTime += processArray[processIndex].burstTime;
        printf("   %d", currentTime);
    }
    printf("\n");
}

// Calculate average waiting time
float calculateAverageWaitingTime(ProcessInfo processArray[], int numberOfProcesses) {
    float totalWaitingTime = 0.0;
    
    for (int processIndex = 0; processIndex < numberOfProcesses; processIndex++) {
        totalWaitingTime += processArray[processIndex].waitingTime;
    }
    
    return totalWaitingTime / numberOfProcesses;
}