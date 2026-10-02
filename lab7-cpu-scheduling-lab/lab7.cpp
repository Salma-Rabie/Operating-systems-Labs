#include "lab7.h"
#include <iostream>
#include <iomanip>
#include <map>
#include <queue>
#include <math.h>

using namespace std;

void Scheduler::execute()
{
    for (int i = 0; i < schedulongPoliceis.size(); i++)
    {
        clearTables();
        if (schedulongPoliceis[i].first == 1)
        {
            FCFS();
            if (type == "trace")
                trace(1, -1);
            else
                stats(1, -1);
        }
        else if (schedulongPoliceis[i].first == 2)
        {
            RR(schedulongPoliceis[i].second);
            if (type == "trace")
                trace(2, schedulongPoliceis[i].second);
            else
                stats(2, schedulongPoliceis[i].second);
        }
        else if (schedulongPoliceis[i].first == 3)
        {
            SPN();
            if (type == "trace")
                trace(3, -1);
            else
                stats(3, -1);
        }
        else if (schedulongPoliceis[i].first == 4)
        {
            SRT();
            if (type == "trace")
                trace(4, -1);
            else
                stats(4, -1);
        }
        else if (schedulongPoliceis[i].first == 5)
        {
            HRRN();
            if (type == "trace")
                trace(5, -1);
            else
                stats(5, -1);
        }
        else if (schedulongPoliceis[i].first == 6)
        {
            FB1();
            if (type == "trace")
                trace(6, -1);
            else
                stats(6, -1);
        }
        else if (schedulongPoliceis[i].first == 7)
        {
            FB2i();
            if (type == "trace")
                trace(7, -1);
            else
                stats(7, -1);
        }
        else if (schedulongPoliceis[i].first == 8)
        {
            AGE(schedulongPoliceis[i].second);
            if (type == "trace")
                trace(8, -1);
        }
    }
}
void Scheduler::trace(int policy, int argument)
{
    if (policy == 1)
    {
        cout << "FCFS  ";
        printHeader();
        cout << '\n';
        printTracing();
        printDashes();
        cout << "\n\n";
    }
    else if (policy == 2)
    {
        if (argument > 10)
            cout << "RR-" << argument << " ";
        else
            cout << "RR-" << argument << "  ";
        printHeader();
        cout << '\n';
        printTracing();
        printDashes();
        cout << "\n\n";
    }
    else if (policy == 3)
    {
        cout << "SPN   ";
        printHeader();
        cout << "\n";
        printTracing();
        printDashes();
        cout << "\n\n";
    }
    else if (policy == 4)
    {
        cout << "SRT   ";
        printHeader();
        cout << '\n';
        printTracing();
        printDashes();
        cout << "\n\n";
    }
    else if (policy == 5)
    {
        cout << "HRRN  ";
        printHeader();
        cout << "\n";
        printTracing();
        printDashes();
        cout << "\n\n";
    }
    else if (policy == 6)
    {
        cout << "FB-1  ";
        printHeader();
        cout << "\n";
        printTracing();
        printDashes();
        cout << "\n\n";
    }
    else if (policy == 7)
    {
        cout << "FB-2i ";
        printHeader();
        cout << "\n";
        printTracing();
        printDashes();
        cout << "\n\n";
    }
    else if (policy == 8)
    {
        cout << "Aging ";
        printHeader();
        cout << "\n";
        printTracing();
        printDashes();
        cout << "\n\n";
    }
}
void Scheduler::stats(int policy, int argument)
{
    if (policy == 1)
    {
        cout << "FCFS" << endl;
        printStats();
        cout << '\n';
    }
    else if (policy == 2)
    {
        cout << "RR-" << argument << endl;
        printStats();
        cout << '\n';
    }
    else if (policy == 3)
    {
        cout << "SPN" << endl;
        printStats();
        cout << '\n';
    }
    else if (policy == 4)
    {
        cout << "SRT" << endl;
        printStats();
        cout << '\n';
    }
    else if (policy == 5)
    {
        cout << "HRRN" << endl;
        printStats();
        cout << '\n';
    }
    else if (policy == 6)
    {
        cout << "FB-1" << endl;
        printStats();
        cout << '\n';
    }
    else if (policy == 7)
    {
        cout << "FB-2i" << endl;
        printStats();
        cout << '\n';
    }
}
void Scheduler::FCFS()
{
    int time = 0;
    int finished = 0;
    int current = -1; // index of running process

    while (time < maxSeconds)
    {
        // Add newly arrived processes to ready queue
        for (int i = 0; i < numberOfProcesses; i++)
        {
            if (processes[i].arrivalTime == time)
            {
                readyQueue.push(processes[i]);
            }
        }

        //  If CPU is idle, take next process
        if (!processorBusy && !readyQueue.empty())
        {
            currentProcess = readyQueue.front();
            readyQueue.pop();

            current = currentProcess.id;
            processorBusy = true;
        }

        //  Mark timeline
        for (int i = 0; i < numberOfProcesses; i++)
        {
            // Running
            if (processorBusy && i == current)
            {
                *(processesPrintingArray + i * maxSeconds + time) = '*';
            }
            // Ready
            else if (processes[i].arrivalTime <= time && processes[i].remainingTime > 0)
            {
                *(processesPrintingArray + i * maxSeconds + time) = '.';
            }
        }

        //  Execute running process
        if (processorBusy)
        {
            processes[current].remainingTime--;

            // Finished?
            if (processes[current].remainingTime == 0)
            {
                processorBusy = false;

                processes[current].finishTime = time + 1;
                processes[current].turnAroundTime = processes[current].finishTime - processes[current].arrivalTime;
                processes[current].NormTurnTime = processes[current].turnAroundTime / processes[current].serveTime;

                finished++;
            }
        }

        time++;
    }
}

void Scheduler::RR(int quantum)
{
    int time = 0;
    int current = -1;
    int qremaining = 0;
    int delayedProcess = -1;


    while (time < maxSeconds)
    {
        //  Add arrivals for THIS time
        for (int i = 0; i < numberOfProcesses; i++)
        {
            if (processes[i].arrivalTime == time)
            {
                readyQueue.push(processes[i]);
            }
        }

        if (delayedProcess != -1)
        {
            readyQueue.push(processes[delayedProcess]);
            delayedProcess = -1;
        }

        //  If CPU idle, select process
        if (!processorBusy && !readyQueue.empty())
        {
            currentProcess = readyQueue.front();
            readyQueue.pop();

            current = currentProcess.id;
            processorBusy = true;
            qremaining = quantum;
        }

        //  Mark timeline AFTER selection
        for (int i = 0; i < numberOfProcesses; i++)
        {
            if (processorBusy && i == current)
            {
                *(processesPrintingArray + i * maxSeconds + time) = '*';
            }
            else if (processes[i].arrivalTime <= time && processes[i].remainingTime > 0)
            {
                *(processesPrintingArray + i * maxSeconds + time) = '.';
            }
        }

        //  Execute one time unit
        if (processorBusy)
        {
            processes[current].remainingTime--;
            qremaining--;

            // Finished
            if (processes[current].remainingTime == 0)
            {
                processes[current].finishTime = time + 1;
                processes[current].turnAroundTime = processes[current].finishTime - processes[current].arrivalTime;
                processes[current].NormTurnTime = processes[current].turnAroundTime / processes[current].serveTime;

                processorBusy = false;
                current = -1;
            }
            // Quantum expired
            else if (qremaining == 0)
            {
                delayedProcess = current; // remember old process
                processorBusy = false;
                current = -1;
            }
        }

        time++;
    }
}

void Scheduler::SPN()
{
    int time = 0;
    int current = -1;

    while (time < maxSeconds)
    {
        //  Add arrivals
        for (int i = 0; i < numberOfProcesses; i++)
        {
            if (processes[i].arrivalTime == time)
            {
                // priority = -service time
                readyPriorityQueue.push(make_pair(-processes[i].serveTime, i));
            }
        }

        //  Pick shortest job if CPU idle
        if (!processorBusy && !readyPriorityQueue.empty())
        {
            current = readyPriorityQueue.top().second;
            readyPriorityQueue.pop();
            processorBusy = true;
        }

        //  Mark timeline
        for (int i = 0; i < numberOfProcesses; i++)
        {
            if (processorBusy && i == current)
            {
                *(processesPrintingArray + i * maxSeconds + time) = '*';
            }
            else if (processes[i].arrivalTime <= time && processes[i].remainingTime > 0)
            {
                *(processesPrintingArray + i * maxSeconds + time) = '.';
            }
        }

        //  Execute
        if (processorBusy)
        {
            processes[current].remainingTime--;

            if (processes[current].remainingTime == 0)
            {
                processes[current].finishTime = time + 1;
                processes[current].turnAroundTime = processes[current].finishTime - processes[current].arrivalTime;
                processes[current].NormTurnTime = processes[current].turnAroundTime / processes[current].serveTime;

                processorBusy = false;
            }
        }

        time++;
    }
}

void Scheduler::SRT()
{
    int time = 0;
    int current = -1;

    while (time < maxSeconds)
    {
        // Select process with shortest remaining time
        int selectedprocess = -1;

        for (int i = 0; i < numberOfProcesses; i++)
        {
            if (processes[i].arrivalTime <= time && processes[i].remainingTime > 0)
            {
                if (selectedprocess == -1 || processes[i].remainingTime < processes[selectedprocess].remainingTime ||
                    (processes[i].remainingTime == processes[selectedprocess].remainingTime && processes[i].arrivalTime < processes[selectedprocess].arrivalTime))
                {
                    selectedprocess = i;
                }
            }
        }

        if (selectedprocess != -1)
        {
            current = selectedprocess;
            processorBusy = true;
        }
        else
        {
            processorBusy = false;
        }

        // Mark timeline
        for (int i = 0; i < numberOfProcesses; i++)
        {
            if (processorBusy && i == current)
                *(processesPrintingArray + i * maxSeconds + time) = '*';
            else if (processes[i].arrivalTime <= time && processes[i].remainingTime > 0)
                *(processesPrintingArray + i * maxSeconds + time) = '.';
        }

        // Execute
        if (processorBusy)
        {
            processes[current].remainingTime--;

            if (processes[current].remainingTime == 0)
            {
                processes[current].finishTime = time + 1;
                processes[current].turnAroundTime = processes[current].finishTime - processes[current].arrivalTime;
                processes[current].NormTurnTime = processes[current].turnAroundTime / processes[current].serveTime;

                processorBusy = false;
                current = -1;
            }
        }

        time++;
    }
}


void Scheduler::HRRN()
{
    int time = 0;
    int current = -1;

    while (time < maxSeconds)
    {
        // Update waiting times
        for (int i = 0; i < numberOfProcesses; i++)
        {
            if (processes[i].arrivalTime <= time && processes[i].remainingTime > 0 && i != current)
            {
                processes[i].waitingTime++;
            }
        }

        // Select new process if CPU idle
        if (!processorBusy)
        {
            float bestRatio = -1.0;
            int selectedprocess = -1;

            for (int i = 0; i < numberOfProcesses; i++)
            {
                if (processes[i].arrivalTime <= time && processes[i].remainingTime > 0)
                {
                    float ratio = (processes[i].waitingTime + processes[i].serveTime) / (float)processes[i].serveTime;

                    if (ratio > bestRatio)
                    {
                        bestRatio = ratio;
                        selectedprocess = i;
                    }
                }
            }

            if (selectedprocess != -1)
            {
                current = selectedprocess;
                processorBusy = true;
            }
        }

        // Mark timeline
        for (int i = 0; i < numberOfProcesses; i++)
        {
            if (processorBusy && i == current)
                *(processesPrintingArray + i * maxSeconds + time) = '*';
            else if (processes[i].arrivalTime <= time && processes[i].remainingTime > 0)
                *(processesPrintingArray + i * maxSeconds + time) = '.';
        }

        // Execute
        if (processorBusy)
        {
            processes[current].remainingTime--;

            if (processes[current].remainingTime == 0)
            {
                processes[current].finishTime = time + 1;
                processes[current].turnAroundTime = processes[current].finishTime - processes[current].arrivalTime;
                processes[current].NormTurnTime = processes[current].turnAroundTime / processes[current].serveTime;

                processorBusy = false;
                current = -1;
            }
        }

        time++;
    }
}


void Scheduler::FB1()
{
    FBQueues.clear();
    FBQueues.push_back(queue<Process>());
    
    // Track process completion status
    vector<bool> completed(numberOfProcesses, false);
    int completedCount = 0;
    
    int currentTime = 0;
    processorBusy = false;
    int currentProcessId = -1;  
    
    while (currentTime < maxSeconds && completedCount < numberOfProcesses) {
        // Add arriving processes at current time
        for (int i = 0; i < numberOfProcesses; i++) {
            if (processes[i].arrivalTime == currentTime && !completed[i]) {
                // Check if process is already in a queue (shouldn't happen for arrivals)
                bool alreadyInQueue = false;
                for (size_t q = 0; q < FBQueues.size(); q++) {
                    queue<Process> temp = FBQueues[q];
                    while (!temp.empty()) {
                        if (temp.front().id == i) {
                            alreadyInQueue = true;
                            break;
                        }
                        temp.pop();
                    }
                }
                
                if (!alreadyInQueue && currentProcessId != -1 && currentProcessId == i) {
                    alreadyInQueue = true;
                }
                
                if (!alreadyInQueue) {
                    processes[i].FBLevel = 0;
                    FBQueues[0].push(processes[i]);
                }
            }
        }
        
        // Check if current process finished execution
        if (currentProcessId != -1) {
            if (processes[currentProcessId].remainingTime <= 0) {
                // Process completed
                completed[currentProcessId] = true;
                completedCount++;
                processes[currentProcessId].finishTime = currentTime;
                processes[currentProcessId].turnAroundTime = currentTime - processes[currentProcessId].arrivalTime;
                processes[currentProcessId].NormTurnTime = processes[currentProcessId].turnAroundTime * 1.0 / processes[currentProcessId].serveTime;
                
                // Clear current process
                currentProcessId = -1;
                processorBusy = false;
            }
            else {
                // Count other ready processes
                bool otherExists = false;
                // Check for new arrivals at this time
                for (int i = 0; i < numberOfProcesses; i++) {
                    if (processes[i].arrivalTime == currentTime && !completed[i]) {
                        otherExists = true;
                        break;
                    }
                }
                // Check queues
                for (int i = 0; i < FBQueues.size(); i++) {
                    if (!FBQueues[i].empty()) {
                        otherExists = true;
                        break;
                    }
                }
                
                // Demote ONLY if competition exists (FB-1 rule)
                if (otherExists) {
                    processes[currentProcessId].FBLevel++;
                    
                    // Create new level if needed
                    if (processes[currentProcessId].FBLevel >= FBQueues.size()) {
                        FBQueues.push_back(queue<Process>());
                    }
                }
                
                FBQueues[processes[currentProcessId].FBLevel].push(processes[currentProcessId]);
                
                currentProcessId = -1;
                processorBusy = false;
            }
        }
        
        // If no current process, select next one from highest priority non-empty queue
        if (currentProcessId == -1) {
            for (size_t level = 0; level < FBQueues.size(); level++) {
                // Remove any completed processes from front of queue
                while (!FBQueues[level].empty() && completed[FBQueues[level].front().id]) {
                    FBQueues[level].pop();
                }
                
                if (!FBQueues[level].empty()) {
                    Process selectedProcess = FBQueues[level].front();
                    FBQueues[level].pop();
                    
                    currentProcessId = selectedProcess.id;
                    processorBusy = true;
                    break;
                }
            }
        }
        
        // Execute for 1 time unit if we have a current process
        if (currentProcessId != -1) {
            // Update printing array for current process
            *(processesPrintingArray + currentProcessId * maxSeconds + currentTime) = '*';
            processes[currentProcessId].processState = '*';
            
            // Execute for 1 time unit 
            processes[currentProcessId].remainingTime--;
            
            // Mark other ready processes with '.'
            for (int i = 0; i < numberOfProcesses; i++) {
                if (i != currentProcessId && !completed[i] && 
                    processes[i].arrivalTime <= currentTime &&
                    processes[i].remainingTime > 0) {
                    
                    // Check if process is in ready state (in a queue or about to be added)
                    bool isReady = false;
                    
                    // Check queues
                    for (size_t q = 0; q < FBQueues.size(); q++) {
                        queue<Process> temp = FBQueues[q];
                        while (!temp.empty()) {
                            if (temp.front().id == i) {
                                isReady = true;
                                break;
                            }
                            temp.pop();
                        }
                        if (isReady) break;
                    }
                    
                    if (isReady) {
                        *(processesPrintingArray + i * maxSeconds + currentTime) = '.';
                        processes[i].processState = '.';
                    }
                }
            }
        }
        else {
            // No process running
            for (int i = 0; i < numberOfProcesses; i++) {
                if (!completed[i] && processes[i].arrivalTime <= currentTime &&
                    processes[i].remainingTime > 0) {
                    // These processes are waiting but CPU is idle
                    *(processesPrintingArray + i * maxSeconds + currentTime) = ' ';
                    processes[i].processState = ' ';
                }
            }
        }
        
        currentTime++;
    }
    
    // Handle processes that finished execution
    for (int i = 0; i < numberOfProcesses; i++) {
        if (processes[i].remainingTime <= 0 && processes[i].finishTime == 0) {
            processes[i].finishTime = maxSeconds;
            processes[i].turnAroundTime = processes[i].finishTime - processes[i].arrivalTime;
            processes[i].NormTurnTime = processes[i].turnAroundTime * 1.0 / processes[i].serveTime;
        }
    }
}


void Scheduler::FB2i()
{
    FBQueues.clear();
    FBQueues.push_back(queue<Process>());
    
    // Track process completion status
    vector<bool> completed(numberOfProcesses, false);
    int completedCount = 0;
    
    int currentTime = 0;
    processorBusy = false;
    int currentProcessId = -1;  
    
    
    while (currentTime < maxSeconds && completedCount < numberOfProcesses) {
        // Add arriving processes at current time
        for (int i = 0; i < numberOfProcesses; i++) {
            if (processes[i].arrivalTime == currentTime && !completed[i]) {
                // Check if process is already in a queue (shouldn't happen for arrivals)
                bool alreadyInQueue = false;
                for (size_t q = 0; q < FBQueues.size(); q++) {
                    queue<Process> temp = FBQueues[q];
                    while (!temp.empty()) {
                        if (temp.front().id == i) {
                            alreadyInQueue = true;
                            break;
                        }
                        temp.pop();
                    }
                }
                
                if (!alreadyInQueue && currentProcessId != -1 && currentProcessId == i) {
                    alreadyInQueue = true;
                }
                
                if (!alreadyInQueue) {
                    processes[i].FBLevel = 0;
                    processes[i].q = 1;
                    
                    // Ensure we have enough levels
                    while (FBQueues.size() <= 0) {
                        FBQueues.push_back(queue<Process>());
                    }
                    
                    processes[i].FBLevel = 0;
                    processes[i].q = (int)pow(2, 0); 
                    FBQueues[0].push(processes[i]);
                }
            }
        }
        
        // Check if current process finished quantum or execution
        if (currentProcessId != -1) {
            if (processes[currentProcessId].remainingTime <= 0) {
                // Process completed
                completed[currentProcessId] = true;
                completedCount++;
                processes[currentProcessId].finishTime = currentTime;
                processes[currentProcessId].turnAroundTime = currentTime - processes[currentProcessId].arrivalTime;
                processes[currentProcessId].NormTurnTime = processes[currentProcessId].turnAroundTime * 1.0 / processes[currentProcessId].serveTime;
                
                // Clear current process
                currentProcessId = -1;
                processorBusy = false;
            }
            else if (processes[currentProcessId].q <= 0) {
                // Quantum expired
                int processesInSystem = 0;
                // Count processes in queues
                for (size_t i = 0; i < FBQueues.size(); i++) {
                    processesInSystem += FBQueues[i].size();
                }
                // Add current process if it still has remaining time
                if (processes[currentProcessId].remainingTime > 0) {
                    processesInSystem++;
                }
                
                // Check if there's a new arrival right now
                bool newArrivalNow = false;
                for (int i = 0; i < numberOfProcesses; i++) {
                    if (processes[i].arrivalTime == currentTime && !completed[i]) {
                        newArrivalNow = true;
                        break;
                    }
                }
                
                if (processesInSystem == 1 && !newArrivalNow) {
                    // Only this process in system AND no new arrival now -> stay in same level
                    processes[currentProcessId].q = (int)pow(2, processes[currentProcessId].FBLevel); // Reset quantum
                    
                    // Ensure we have enough levels
                    while (FBQueues.size() <= (size_t)processes[currentProcessId].FBLevel) {
                        FBQueues.push_back(queue<Process>());
                    }
                    
                    FBQueues[processes[currentProcessId].FBLevel].push(processes[currentProcessId]);
                }
                else {
                    // Demote to next level
                    int nextLevel = processes[currentProcessId].FBLevel + 1;
                    processes[currentProcessId].q = (int)pow(2, nextLevel); // Reset quantum for next level
                    
                    // Ensure we have enough levels
                    while (FBQueues.size() <= (size_t)nextLevel) {
                        FBQueues.push_back(queue<Process>());
                    }
                    
                    processes[currentProcessId].FBLevel = nextLevel;
                    FBQueues[nextLevel].push(processes[currentProcessId]);
                }
                
                currentProcessId = -1;
                processorBusy = false;
            }
        }
        
        // If no current process, select next one from highest priority non-empty queue
        if (currentProcessId == -1) {
            for (size_t level = 0; level < FBQueues.size(); level++) {
                // Remove any completed processes from front of queue
                while (!FBQueues[level].empty() && completed[FBQueues[level].front().id]) {
                    FBQueues[level].pop();
                }
                
                if (!FBQueues[level].empty()) {
                    Process selectedProcess = FBQueues[level].front();
                    FBQueues[level].pop();
                    
                    currentProcessId = selectedProcess.id;
                    processorBusy = true;
                    
                    // Update process info (quantum already set when added to queue)
                    break;
                }
            }
        }
        
        // Execute for 1 time unit if we have a current process
        if (currentProcessId != -1) {
            // Update printing array for current process
            *(processesPrintingArray + currentProcessId * maxSeconds + currentTime) = '*';
            processes[currentProcessId].processState = '*';
            
            // Execute
            processes[currentProcessId].remainingTime--;
            processes[currentProcessId].q--;
            
            // Mark other ready processes with '.'
            for (int i = 0; i < numberOfProcesses; i++) {
                if (i != currentProcessId && !completed[i] && 
                    processes[i].arrivalTime <= currentTime &&
                    processes[i].remainingTime > 0) {
                    
                    // Check if process is in ready state (in a queue or about to be added)
                    bool isReady = false;
                    
                    // Check queues
                    for (size_t q = 0; q < FBQueues.size(); q++) {
                        queue<Process> temp = FBQueues[q];
                        while (!temp.empty()) {
                            if (temp.front().id == i) {
                                isReady = true;
                                break;
                            }
                            temp.pop();
                        }
                        if (isReady) break;
                    }
                    
                    if (isReady) {
                        *(processesPrintingArray + i * maxSeconds + currentTime) = '.';
                        processes[i].processState = '.';
                    }
                }
            }
        }
        else {
            // No process running
            for (int i = 0; i < numberOfProcesses; i++) {
                if (!completed[i] && processes[i].arrivalTime <= currentTime &&
                    processes[i].remainingTime > 0) {
                    // These processes are waiting but CPU is idle
                    *(processesPrintingArray + i * maxSeconds + currentTime) = ' ';
                    processes[i].processState = ' ';
                }
            }
        }
        
        currentTime++;
    }
    
    // Handle processes that finished execution
    for (int i = 0; i < numberOfProcesses; i++) {
        if (processes[i].remainingTime <= 0 && processes[i].finishTime == 0) {
            processes[i].finishTime = maxSeconds;
            processes[i].turnAroundTime = processes[i].finishTime - processes[i].arrivalTime;
            processes[i].NormTurnTime = processes[i].turnAroundTime * 1.0 / processes[i].serveTime;
        }
    }
}



void Scheduler::AGE(int quantum)
{
    int time = 0;
    int current = -1;
    int qremaining = 0;
    int delayedProcess = -1;
    
    // Initialize current priority with initial priority
    for (int i = 0; i < numberOfProcesses; i++) {
        processes[i].currentPriority = processes[i].priority;
    }

    while (time < maxSeconds)
    {
        Process justLeftProcess;
        bool processJustLeft = false;
        int justLeftId = -1;
        
        if (delayedProcess != -1) {
            justLeftId = delayedProcess;
            processJustLeft = true;
        } 
        else if (processorBusy && current != -1 && (processes[current].remainingTime == 0 || qremaining == 0)) {
            justLeftId = current;
            processJustLeft = true;
        }

        for (int i = 0; i < numberOfProcesses; i++)
        {
            if (processes[i].arrivalTime == time)
            {
                processes[i].currentPriority = processes[i].priority;
                readyQueue.push(processes[i]);
            }
        }

        if (processJustLeft) {
            processes[justLeftId].currentPriority = processes[justLeftId].priority;
            
            if (processes[justLeftId].remainingTime > 0) {
                readyQueue.push(processes[justLeftId]);
            }
        }

       
        queue<Process> tempQueue;
        while (!readyQueue.empty()) {
            Process p = readyQueue.front();
            readyQueue.pop();
            
            if (p.id != justLeftId) {
                processes[p.id].currentPriority++;
                p.currentPriority = processes[p.id].currentPriority;
            }
            tempQueue.push(p);
        }
        readyQueue = tempQueue;

        if (!processorBusy)
        {
            if (!readyQueue.empty())
            {
               
                int highestPriority = -999999;
                Process highestProcess;
                queue<Process> searchQueue = readyQueue;
                int foundIndex = -1;
                int queuePos = 0;
                
                while (!searchQueue.empty()) {
                    Process p = searchQueue.front();
                    searchQueue.pop();
                    
                    if (processes[p.id].currentPriority > highestPriority) {
                        highestPriority = processes[p.id].currentPriority;
                        highestProcess = p;
                        foundIndex = queuePos;
                    }
                    queuePos++;
                }
                
                queue<Process> newQueue;
                int currentPos = 0;
                while (!readyQueue.empty()) {
                    Process p = readyQueue.front();
                    readyQueue.pop();
                    
                    if (currentPos != foundIndex) {
                        newQueue.push(p);
                    } else {
                        currentProcess = p;
                    }
                    currentPos++;
                }
                readyQueue = newQueue;
                
                current = currentProcess.id;
                processorBusy = true;
                qremaining = quantum;
            }
        }

        // Mark timeline
        for (int i = 0; i < numberOfProcesses; i++)
        {
            if (processorBusy && i == current)
            {
                *(processesPrintingArray + i * maxSeconds + time) = '*';
            }
            else if (processes[i].arrivalTime <= time && processes[i].remainingTime > 0 && i != current)
            {
                bool inReadyQueue = false;
                queue<Process> checkQueue = readyQueue;
                while (!checkQueue.empty()) {
                    if (checkQueue.front().id == i) {
                        inReadyQueue = true;
                        break;
                    }
                    checkQueue.pop();
                }
                
                if (inReadyQueue) {
                    *(processesPrintingArray + i * maxSeconds + time) = '.';
                }
            }
        }

        // Execute running process
        if (processorBusy)
        {
            processes[current].remainingTime--;
            qremaining--;

            if (processes[current].remainingTime == 0)
            {
                processes[current].finishTime = time + 1;
                processes[current].turnAroundTime = processes[current].finishTime - processes[current].arrivalTime;
                processes[current].NormTurnTime = processes[current].turnAroundTime / processes[current].serveTime;
                
                processes[current].currentPriority = processes[current].priority;
                
                processorBusy = false;
                current = -1;
            }
            else if (qremaining == 0)
            {
                delayedProcess = current;
                processorBusy = false;
                current = -1;
            }
        }

        time++;
    }
    
    // Handle any remaining processes that didn't finish
    for (int i = 0; i < numberOfProcesses; i++) {
        if (processes[i].remainingTime > 0) {
            processes[i].finishTime = maxSeconds;
            processes[i].turnAroundTime = processes[i].finishTime - processes[i].arrivalTime;
            processes[i].NormTurnTime = processes[i].turnAroundTime / processes[i].serveTime;
        }
    }
}

void Scheduler::printTracing()
{
    for (int process = 0; process < numberOfProcesses; process++)
    {
        cout << processes[process].processName << "     |";
        for (int time = 0; time < maxSeconds; time++)
            cout << *(processesPrintingArray + process * maxSeconds + time) << '|';
        cout << " \n";
    }
}
void Scheduler::printStats()
{
    float sum, mean, sum2;
    cout << "Process    |";
    for (int i = 0; i < numberOfProcesses; i++)
        cout << "  " << processes[i].processName << "  |";
    cout << endl;
    cout << "Arrival    |";
    for (int i = 0; i < numberOfProcesses; i++)
    {
        if (processes[i].arrivalTime < 10)
            cout << "  " << processes[i].arrivalTime << "  |";
        else
            cout << " " << processes[i].arrivalTime << "  |";
    }
    cout << endl;
    cout << "Service    |";
    for (int i = 0; i < numberOfProcesses; i++)
    {
        if (processes[i].arrivalTime < 10)
            cout << "  " << processes[i].serveTime << "  |";
        else
            cout << " " << processes[i].serveTime << "  |";
    }
    cout << " Mean|" << endl;
    cout << "Finish     |";
    for (int i = 0; i < numberOfProcesses; i++)
    {
        if (processes[i].finishTime >= 10)
            cout << " " << processes[i].finishTime << "  |";
        else
            cout << "  " << processes[i].finishTime << "  |";
    }
    cout << "-----|" << endl;
    cout << "Turnaround |";
    for (int i = 0; i < numberOfProcesses; i++)
    {

        if (processes[i].turnAroundTime >= 10)
            cout << " " << (int)processes[i].turnAroundTime << "  |";
        else
            cout << "  " << (int)processes[i].turnAroundTime << "  |";
        sum += processes[i].turnAroundTime;
    }
    cout << fixed;
    cout << setprecision(2);
    mean = (sum * 1.0) / numberOfProcesses;
    if (mean >= 10)
        cout << mean << "|";
    else
        cout << " " << mean << "|";
    cout << endl;
    cout << "NormTurn   |";
    sum2 = 0;
    for (int i = 0; i < numberOfProcesses; i++)
    {
        if (processes[i].NormTurnTime > 10)
            cout << processes[i].NormTurnTime << "|";
        else
            cout << " " << processes[i].NormTurnTime << "|";
        sum2 += (processes[i].NormTurnTime * 1.0);
    }

    mean = (sum2 * 1.0) / numberOfProcesses;
    if (mean > 10)
        cout << mean << "|";
    else
        cout << " " << mean << "|";
    cout << endl;
}
void Scheduler::clearTables()
{

    for (int i = 0; i < numberOfProcesses; i++)
    {
        for (int j = 0; j < maxSeconds; j++)
            *(processesPrintingArray + i * maxSeconds + j) = ' ';
    }
    for (int i = 0; i < numberOfProcesses; i++)
    {
        processes[i].finishTime = 0;
        processes[i].turnAroundTime = 0;
        processes[i].NormTurnTime = 0;
        processes[i].processState = ' ';
        processes[i].remainingTime = processes[i].serveTime;
    }
    processorBusy = false;
    while (!readyQueue.empty())
        readyQueue.pop();
    while (!readyPriorityQueue.empty())
        readyPriorityQueue.pop();
}
void Scheduler::splitPolicyAndParameter(string str)
{
    string w = "";
    pair<int, int> policies;
    bool parameterExists = false;
    policies.second = -1;
    for (int i = 0; i < str.length(); i++)
    {
        if (str[i] == '-')
        {
            parameterExists = true;
            policies.first = stoi(w);
            w = "";
        }
        else if (str[i] == ',')
        {
            if (parameterExists)
                policies.second = stoi(w);
            else
            {
                policies.first = stoi(w);
                policies.second = -1;
            }
            w = "";
            schedulongPoliceis.push_back(policies);
            parameterExists = false;
        }
        else
            w = w + str[i];
    }
    if (parameterExists)
        policies.second = stoi(w);
    else
        policies.first = stoi(w);
    schedulongPoliceis.push_back(policies);
}
void Scheduler::splitPrcoessAndTimes(string str, int id)
{
    Process process;
    string w = "";
    process.processName = str[0];
    for (int i = 2; i < str.length(); i++)
    {
        if (str[i] == ',')
        {
            process.arrivalTime = stoi(w);
            w = "";
        }
        else
            w = w + str[i];
    }
    processorBusy = false;
    process.processState = ' ';
    if (schedulongPoliceis[0].first == 8)
    {
        process.priority = stoi(w);
        process.currentPriority = stoi(w);
    }
    else
        process.serveTime = stoi(w);
    process.remainingTime = process.serveTime;
    process.waitingTime = 0;
    process.id = id;
    processes.push_back(process);
}
void Scheduler::readFile()
{
    processorBusy = false;
    string temp1, temp2;
    cin >> type;
    cin >> temp1;
    splitPolicyAndParameter(temp1);
    cin >> maxSeconds;
    cin >> numberOfProcesses;

    for (int i = 0; i < numberOfProcesses; i++)
    {
        cin >> temp2;
        splitPrcoessAndTimes(temp2, i);
    }
    processesPrintingArray = new char[numberOfProcesses * maxSeconds];
    currentProcess.processName = 0;
    currentProcess.q = 0;
    clearTables();
}
void Scheduler::printHeader()
{
    for (int i = 0; i < maxSeconds + 1; i++)
        cout << i % 10 << ' ';
    cout << "\n";
    printDashes();
}
void Scheduler::printDashes()
{
    cout << "------------------------------------------------";
}
int main(void)
{
    Scheduler scheduler;
    scheduler.readFile();
    scheduler.execute();
    return 0;
}