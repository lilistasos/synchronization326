//All libraries + namespace standard
#include <iostream>
#include <pthread.h>
#include <unistd.h>
#include <cstdlib>
#include <ctime>

using namespace std;

//Amount of people given the rules.
const int numOfPhilosophers = 5;

//Two states the philosophers alternate between
enum { thinking, eating };

int state[numOfPhilosophers];

//Mutex and conditional variables for synchronization
//mtx manages critical sections.
//cond_var for the philosophers waiting for forks.
//printMutex to maintain a concise output.
pthread_mutex_t mtx;
pthread_cond_t cond_var[numOfPhilosophers];
pthread_mutex_t printMutex;

//Gets the index of the philosopher to the left of the current.
int leftPhilosopher(int philosopherNumber)
{
    return (philosopherNumber + numOfPhilosophers -1) % numOfPhilosophers;
}

//Gets the index of the philosopher to the right of the current.
int rightPhilosopher(int philosopherNumber)
{
    return (philosopherNumber + 1) % numOfPhilosophers;
}

//Attempts to pick up the fork and eat.
//A philosopher must wait until both sides forks are
//available, using functions to synchronize this access.
void pickup_forks (int philosopherNumber)
{
    pthread_mutex_lock(&mtx);

    //Waits until both sides are not eating
    while (state[leftPhilosopher(philosopherNumber)] == eating ||
        state[rightPhilosopher(philosopherNumber)] == eating)
        {
            pthread_cond_wait(&cond_var[philosopherNumber], &mtx);
        }

    //Can now eat
    state[philosopherNumber] = eating;

    //identifies the left and right forks
    int leftFork =  philosopherNumber;
    int rightFork = (philosopherNumber + 1) % numOfPhilosophers;

    //Uses printMutext to prevent corrupted outputs.
    pthread_mutex_lock(&printMutex);
    cout << "Forks are with Philosopher #" << philosopherNumber << "\n";
    cout << "Fork #" << leftFork << " is with " << philosopherNumber << "\n";
    cout << "Fork #" << rightFork << " is with " << philosopherNumber << "\n";
    pthread_mutex_unlock(&printMutex);

    pthread_mutex_unlock(&mtx);
}

//Returns the forks after eating and tells the
//waiting philosophers.
void return_forks (int philosopherNumber)
{
    pthread_mutex_lock(&mtx);

    //Can now think
    state[philosopherNumber] = thinking;

    //the left and right forks may be ready.
    pthread_cond_signal(&cond_var[leftPhilosopher(philosopherNumber)]);
    pthread_cond_signal(&cond_var[rightPhilosopher(philosopherNumber)]);

    pthread_mutex_unlock(&mtx);
}

//Function to help each philosopher alternate between
//thinking and eating with random durations. Uses
// usleep for a small delay to prevent bad outputs.
void* philosopher(void* num)
{
    int philosopherNumber = *(int*)num;

    while (true)
    {
        //random thinking time
        int thinkTime = (rand() % 3 + 1) * 1000;

        pthread_mutex_lock(&printMutex);
        cout << "Philosopher #" << philosopherNumber
             <<" took " << thinkTime << "ms thinking\n";
        pthread_mutex_unlock(&printMutex);

        usleep(thinkTime * 1000);

        //calls the function to pickup and eat
        pickup_forks(philosopherNumber);

        //random eating time
        int eatTime = (rand() % 3 + 1) * 1000;

        pthread_mutex_lock(&printMutex);
        cout << "Philosopher #" << philosopherNumber
             << " took " << eatTime << "ms eating\n";
        pthread_mutex_unlock(&printMutex);

        usleep(eatTime * 1000);

        //calls the function to return the forks after eating.
        return_forks(philosopherNumber);
    }

    return NULL;
}

//Main function to manage threads and use the philosopher
int main ()
{
    //random number generator
    srand(time(0));

    //Thread IDs for philosophers
    //Stores their numbers
    pthread_t threadID[numOfPhilosophers];
    int philosophersNumber[numOfPhilosophers];

    //Mutex initializers
    pthread_mutex_init(&mtx, NULL);
    pthread_mutex_init(&printMutex, NULL);

    //Initialize both condition variables and philosopher states.
    for (int i = 0; i < numOfPhilosophers; i++)
    {
        pthread_cond_init(&cond_var[i], NULL);
        state[i] = thinking;
        philosophersNumber[i] = i;
    }

    //Creates threads for philosophers
    for (int i = 0; i < numOfPhilosophers; i++)
    {
        pthread_create(&threadID[i], NULL, philosopher, &philosophersNumber[i]);
    }

    //waits for all threads to finish (won't, just in case)
    for (int i = 0; i < numOfPhilosophers; i++)
    {
        pthread_join(threadID[i], NULL);
    }

    //destroys the mutexes and conditionals.
    pthread_mutex_destroy(&mtx);
    pthread_mutex_destroy(&printMutex);

    for (int i = 0; i < numOfPhilosophers; i++)
    {
        pthread_cond_destroy(&cond_var[i]);
    }

    return 0;
}