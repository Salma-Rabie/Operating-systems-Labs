README – Producer–Consumer Commodity Price System


## Project Description

    -This project implements the classic Producer–Consumer problem using:

    -System V Shared Memory

    -System V Semaphores

    -Circular Buffer

    -Multiple Producers (maximum 20)

    -One Consumer

    -Normal distribution for price generation

    -Each producer generates live commodity prices and inserts them into a shared bounded buffer.
    
    -The consumer continuously reads these values and displays them in an updating table.

## How It Works
### 1-Producer

    A producer:

    Validates input arguments.

    Ensures the commodity is one of the allowed list.

    Checks that no more than 20 producers are running.

    Connects to shared memory created by the consumer.

    Connects to the three semaphores (mutex, full, empty).

    Generates prices using: normal_distribution(mean, stddev)

    Inserts prices into the buffer using:

    P(empty)

    P(mutex)

    write into buffer

    V(mutex)

    V(full)

    Logs all actions to stderr with timestamps.


### 2-Consumer

    The consumer:

    Creates the shared memory using the given buffer size.

    Initializes the buffer indices (in, out).

    Initializes semaphores:

    mutex = 1

    full = 0

    empty = bufferSize

    Continuously reads data from the buffer using:

    P(full)

    P(mutex)

    read buffer

    V(mutex)

    V(empty)

    Keeps the last 5 prices for every commodity.

    Prints a live table showing:

    Latest price

    Average of the last 5 prices

    Green ↑ if price increased

    Red ↓ if price decreased

## How to Compile
    g++ -o producer producer.cpp
    g++ -o consumer consumer.cpp

## How to Run
    1. Start the consumer first
    ./consumer 40
    (40 = buffer size)


    2. Start producers
    ./producer GOLD 1800 20 500 40 &
    ./producer COPPER 5000 30 1500 40 &


    The & runs them in the background.


## Stopping Processes

   - List running producers:

        pgrep producer


    Stop one:

        kill <PID>


   - Stop all:

        pkill producer


## Redirecting Logs

   - Producer logs (stderr):

        ./producer GOLD 1800 20 500 40 2> logs.txt


 ## Important Notes

   - The consumer must be started first because it creates the shared memory and semaphores.

   - Producers only attach to existing shared memory; they never create or initialize it.

   - Producer buffer size must match the consumer buffer size.

   - Maximum number of producers allowed: 20.