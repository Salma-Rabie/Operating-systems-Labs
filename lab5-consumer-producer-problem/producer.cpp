#include <iostream>
#include <cstring>     // strcpy, memset
#include <cstdlib>     // exit()
#include <unistd.h>    // sleep(), fork(), getpid()
#include <sys/ipc.h>   // ftok()
#include <sys/shm.h>   // shmget(), shmat()
#include <sys/sem.h>   // semget(), semop(), semctl()
#include <ctime>       // rand(), srand(), time()
#include <errno.h>     // error printing
#include <sys/types.h> // for completeness
#include <random>
using namespace std;

#define SHM_KEY 0x1234
#define SEM_MUTEX 0x2345
#define SEM_FULL 0x3456
#define SEM_EMPTY 0x4567

struct bufferEntry
{
    char commodity[11];
    double price;
};

struct memory
{
    int out, in;
    int bufferSize;
    bufferEntry buffer[1];
};

string timestamp()
{
    struct timespec ts;
    clock_gettime(CLOCK_REALTIME, &ts);

    // Convert seconds to calendar date
    struct tm localTime;
    localtime_r(&ts.tv_sec, &localTime);

    // Format the time into a string
    char buffer[64];
    int milliseconds = ts.tv_nsec / 1000000;

    snprintf(buffer, sizeof(buffer),
             "[%02d/%02d/%04d %02d:%02d:%02d.%03d]",
             localTime.tm_mon + 1,
             localTime.tm_mday,
             localTime.tm_year + 1900,
             localTime.tm_hour,
             localTime.tm_min,
             localTime.tm_sec,
             milliseconds);

    return string(buffer);
}

int main(int argCount, char *argValues[])
{
    if (argCount != 6)
    {
        cerr << "Usage: ./producer <Commodity> <Mean> <StdDev> <Sleep(ms)> <BufferSize>\n";
        return 1;
    }

    string commodity = argValues[1];
    double mean = atof(argValues[2]);
    double stddev = atof(argValues[3]);
    int sleepTime = atoi(argValues[4]);
    int bufferSize = atoi(argValues[5]);

    bool found = false;
    vector<string> allowed = {
        "ALUMINIUM", "COPPER", "COTTON", "CRUDEOIL", "GOLD",
        "LEAD", "MENTHAOIL", "NATURALGAS", "NICKEL", "SILVER", "ZINC"};

    for (string c : allowed)
    {
        if (commodity == c)
        {
            found = true;
            break;
        }
    }

    if (!found)
    {
        cerr << timestamp() << " ERROR: Invalid commodity '" << commodity << "'.\n";
        cerr << " Allowed commodities are:\n";
        for (string c : allowed)
            cerr << " - " << c << "\n";
        return 1;
    }

    int count = 0;
    FILE *fp = popen("pgrep -c producer", "r");
    if (fp)
    {
        fscanf(fp, "%d", &count);
        pclose(fp);
    }

    if (count > 20)
    {
        cerr << timestamp() << " ERROR: Maximum producers limit (20) reached.\n";
        return 1;
    }

    default_random_engine generator(time(NULL));
    normal_distribution<double> distribution(mean, stddev);

    //  Get (attach to) the shared memory created by the consumer
    int shmid = shmget(SHM_KEY, 0, 0666); // Using size = 0 tells the OS: Find the segment with that key; I don’t care about size
    if (shmid < 0)
    {
        cerr << timestamp() << " ERROR: Shared memory not found.\n";
        return 1;
    }

    // Attach the shared memory to our address space
    memory *mem = (memory *)shmat(shmid, NULL, 0);
    if (mem == (void *)-1) // If shmat returned -1 this means that the shared memory failed
    {
        cerr << timestamp() << " ERROR: Failed to attach shared memory.\n";
        return 1;
    }

    // Validate buffer size
    if (mem->bufferSize != bufferSize)
    {
        cerr << timestamp()
             << " ERROR: Producer buffer size (" << bufferSize << ") does not match consumer buffer size (" << mem->bufferSize << ").\n";
        return 1;
    }

    // Connect to the semaphores (also created by the consumer)
    int semMutex = semget(SEM_MUTEX, 1, 0666);
    int semFull = semget(SEM_FULL, 1, 0666);
    int semEmpty = semget(SEM_EMPTY, 1, 0666);

    if (semMutex < 0 || semFull < 0 || semEmpty < 0)
    {
        cerr << timestamp() << " ERROR: Failed to connect to semaphores.\n";
        return 1;
    }

    // Define standard P and V operations
    sembuf P = {0, -1, 0}; // P(): decrement
    sembuf V = {0, 1, 0};  // V(): increment

    while (true)
    {
        double price = distribution(generator);
        cerr << timestamp() << " " << commodity << ": generating a new value " << price << "\n";
        semop(semEmpty, &P, 1);
        cerr << timestamp() << " " << commodity << ": trying to get mutex on shared buffer\n";
        semop(semMutex, &P, 1);

        strncpy(mem->buffer[mem->in].commodity, commodity.c_str(), 10);
        mem->buffer[mem->in].commodity[10] = '\0';
        mem->buffer[mem->in].price = price;
        cerr << timestamp() << " " << commodity << ": placing " << price << " on shared buffer at index\n";
        mem->in = (mem->in + 1) % bufferSize;

        semop(semMutex, &V, 1);
        semop(semFull, &V, 1);
        cerr << timestamp() << " " << commodity << ": slepping for " << sleepTime << "ms\n";
        usleep(sleepTime * 1000); // usleep accepts arguments in microseconds and we need sleep time in milliseconds so *1000
    }
}