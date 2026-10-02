#include <iostream>
#include <iomanip>
#include <cstring>
#include <unistd.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <ctime>
#include <vector>
#include <map>

using namespace std;

#define SHM_KEY 0x1234
#define SEM_MUTEX 0x2345
#define SEM_FULL 0x3456
#define SEM_EMPTY 0x4567

struct bufferEntry
{
    char commodity[11]; // 10 characters plus a '\0' terminator
    double price;
};

struct memory
{
    int out, in; // indices for consumer (out) and producer (in) positions in the circular buffer
    int bufferSize; 
    bufferEntry buffer[1];
};

vector<string> commodities = {
    "ALUMINIUM", "COPPER", "COTTON", "CRUDEOIL", "GOLD",
    "LEAD", "MENTHAOIL", "NATURALGAS", "NICKEL", "SILVER", "ZINC"};

#define RESET "\033[0m" // RESET returns styling to defaults
#define CYAN "\033[36m"
#define WHITE "\033[37m"
#define GREEN "\033[32m"
#define RED "\033[31m"

int main(int argc, char *argv[])
{
    if (argc != 2)
    {
        cerr << "Usage: ./consumer <BufferSize>\n";
        return 1;
    }

    int bufferSize = atoi(argv[1]);

    int shmid = shmget(SHM_KEY,sizeof(memory) + (bufferSize - 1) * sizeof(bufferEntry), IPC_CREAT | 0666); // requests a shared memory segment

    if (shmid < 0)
    { // shmid is the shared memory ID returned; negative value indicates failure
        cerr << "ERROR creating shared memory\n";
        return 1;
    }

    memory *mem = (memory *)shmat(shmid, NULL, 0); // attaches the segment to the process virtual memory
    if (mem == (void *)-1)
    {
        cerr << "ERROR attaching shared memory\n";
        return 1;
    }

    mem->in = 0;
    mem->out = 0;
    mem->bufferSize = bufferSize;


    int semMutex = semget(SEM_MUTEX, 1, IPC_CREAT | 0666); // 1 means one semaphore
    int semFull = semget(SEM_FULL, 1, IPC_CREAT | 0666);
    int semEmpty = semget(SEM_EMPTY, 1, IPC_CREAT | 0666);

    if (semMutex < 0 || semFull < 0 || semEmpty < 0)
    {
        cerr << "ERROR creating semaphores\n";
        return 1;
    }
    // sets the values of semaphores
    semctl(semMutex, 0, SETVAL, 1);
    semctl(semFull, 0, SETVAL, 0);
    semctl(semEmpty, 0, SETVAL, bufferSize);

    sembuf P = {0, -1, 0}; // will decrement the semaphore and block if value would go negative
    sembuf V = {0, 1, 0};  // increments semaphore

    map<string, vector<double>> history;
    map<string, double> lastPrice;
    map<string, double> lastAvg;


    for (int i = 0; i < commodities.size(); i++)
    {
        string c = commodities[i];
        lastPrice[c] = 0.0;
        lastAvg[c] = 0.0;

    }

    while (true)
    {
        semop(semFull, &P, 1);  // to block the consumer if the buffer is empty
        semop(semMutex, &P, 1); // take the mutex before accessing shared buffer

        bufferEntry entry = mem->buffer[mem->out];
        mem->out = (mem->out + 1) % bufferSize;

        semop(semMutex, &V, 1); // release the mutex
        semop(semEmpty, &V, 1); // increment semEmpty to signal a newly freed empty slot to producers

        string com = entry.commodity;
        double price = entry.price;

        auto &vec = history[com];
        vec.insert(vec.begin(), price); // inserts the new price at the front
        if (vec.size() > 5)
            vec.pop_back(); // keeps only the most recent 5 readings by removing the oldest element at the end

        printf("\e[1;1H\e[2J"); // clear screen and reset cursor

        printf("\033[2;1H");    // move cursor to top-left corner


        cout << CYAN << "+--------------+-------------+-------------+" << RESET << "\n";
        cout << CYAN << "|  Commodity   |    Price    |   AvgPrice  |" << RESET << "\n";
        cout << CYAN << "+--------------+-------------+-------------+" << RESET << "\n";

        for (int i = 0; i < commodities.size(); i++)
        {
            string c = commodities[i];

            double p;
            if (history[c].empty())
            {
                p = 0.0;
            }
            else
            {
                p = history[c][0]; // most recent price because we insert at the begining
            }

            double prev = lastPrice[c];

            // Default color is white with no arrow (in case of no change)
            string priceColor = WHITE;
            string priceArrow = " ";

            if (p > prev)
            {
                priceColor = GREEN;
                priceArrow = GREEN "↑" RESET;
            }
            else if (p < prev)
            {
                priceColor = RED;
                priceArrow = RED "↓" RESET;
            }

            // AvgPrice Calculation
            double a = 0;

            int count = history[c].size(); // how many values exist

            // add all values
            for (int i = 0; i < count; i++)
            {
                a = a + history[c][i];
            }

            // avoid division by zero
            if (count == 0)
            {
                a = 0;
            }
            else
            {
                a = a / count;
            }

            // AvgPrice Previous Value
            double prevAvg = lastAvg[c];

            // Default color is white with no arrow (in case of no change)
            string avgColor = WHITE;
            string avgArrow = " ";

            if (a > prevAvg)
            {
                avgColor = GREEN;
                avgArrow = GREEN "↑" RESET;
            }
            else if (a < prevAvg)
            {
                avgColor = RED;
                avgArrow = RED "↓" RESET;
            }

            // --- Printing row ---
            cout << CYAN << "| " << setw(12) << left << c << RESET << " | ";

            // price
            printf("%s%7.2lf%s", priceColor.c_str(), p, RESET);
            cout << "  " << priceArrow << "  | ";

            // avg price
            printf("%s%7.2lf%s", avgColor.c_str(), a, RESET);
            cout << " " << avgArrow << "   |\n";

            lastPrice[c] = p;
            lastAvg[c] = a;
        }

         cout << CYAN << "+--------------+-------------+-------------+" << RESET << "\n";

        usleep(200000);
    
    }
}
