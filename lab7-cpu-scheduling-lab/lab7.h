#include <string>
#include <vector>
#include <queue>
#include <iomanip>

using namespace std;

class Process
{
public:
    char processName;        // A,B,C...
    char processState;       // ' ' , '.' , '*'
    int arrivalTime;         // when it appears
    int serveTime;           // total CPU needed
    int remainingTime;       // decreases while running
    int finishTime;          // when it finishes
    float turnAroundTime;    // finish - arrival
    float NormTurnTime;      // turnaround / service
    int waitingTime;         // used by HRRN
    float responseRatio;     // HRRN formula
    int FBLevel;             // feedback queue index
    int q;                   // remaining quantum
    int priority;            // aging initial priority
    int currentPriority;     // aging dynamic priority
    int id;                  // index
};

class Scheduler
{
public:
    string type;
    int numberOfProcesses;
    int maxSeconds;
    bool processorBusy;
    Process currentProcess;
    vector<Process> processes;
    vector<pair<int, int>> schedulongPoliceis;
    char *processesPrintingArray;
    queue<Process> readyQueue;
    priority_queue<pair<float, int>> readyPriorityQueue;
    vector<queue<Process>> FBQueues;
    void printTracing();
    void printStats();
    void readFile();
    void clearTables();
    void splitPrcoessAndTimes(string str, int id);
    void splitPolicyAndParameter(string str);
    void printHeader();
    void printDashes();
    void printDetails();
    void execute();
    void trace(int policy, int argument);
    void stats(int policy, int argument);
    void FCFS();
    void RR(int quantum);
    void SPN();
    void SRT();
    void HRRN();
    void FB1();
    void FB2i();
    void AGE(int quantum);
};
