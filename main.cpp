#include <iostream>
#include <pthread.h>
#include <unistd.h>
#include <cstdlib>
#include <ctime>

using namespace std;

const int numOfPhilosophers = 5;

enum { thinking, eating };

int state[numOfPhilosophers];

pthread_mutex_t printMutex;
pthread_mutex_t mtx;
pthread_cond_t cond_var[numOfPhilosophers];

int leftPhilosopher(int philosopherNumber)
{
    return (philosopherNumber + numOfPhilosophers -1) % numOfPhilosophers;
}

int rightPhilosopher(int philosopherNumber)
{
    return (philosopherNumber + 1) % numOfPhilosophers;
}

void pickup_forks (int philosopherNumber)
{
    pthread_mutex_lock(&mtx);

    while (state[leftPhilosopher(philosopherNumber)] == eating ||
        state[rightPhilosopher(philosopherNumber)] == eating)
        {
            pthread_cond_wait(&cond_var[philosopherNumber], &mtx);
        }

    state[philosopherNumber] = eating;

    int leftFork =  philosopherNumber;
    int rightFork = (philosopherNumber + 1) % numOfPhilosophers;

    pthread_mutex_lock(&printMutex);

    cout << "Forks are with Philosopher #" << philosopherNumber << "\n";
    cout << "Fork #" << leftFork << " is with " << philosopherNumber << "\n";
    cout << "Fork #" << rightFork << " is with " << philosopherNumber << "\n";

    pthread_mutex_unlock(&printMutex);

    pthread_mutex_unlock(&mtx);
}

void return_forks (int philosopherNumber)
{
    pthread_mutex_lock(&mtx);

    state[philosopherNumber] = thinking;

    pthread_cond_signal(&cond_var[leftPhilosopher(philosopherNumber)]);
    pthread_cond_signal(&cond_var[rightPhilosopher(philosopherNumber)]);

    pthread_mutex_unlock(&mtx);
}

void* philosopher(void* num)
{
    int philosopherNumber = *(int*)num;

    while (true)
    {
        int thinkTime = (rand() % 3 + 1) * 1000;

        pthread_mutex_lock(&printMutex);
        cout << "Philosopher #" << philosopherNumber
             <<" took " << thinkTime << "ms thinking\n";
        pthread_mutex_unlock(&printMutex);

        usleep(thinkTime * 1000);

        pickup_forks(philosopherNumber);

        int eatTime = (rand() % 3 + 1) * 1000;

        pthread_mutex_lock(&printMutex);
        cout << "Philosopher #" << philosopherNumber
             << " took " << eatTime << "ms eating\n";
        pthread_mutex_unlock(&printMutex);

        usleep(eatTime * 1000);

        return_forks(philosopherNumber);
    }

    return NULL;
}

int main ()
{
    srand(time(0));

    pthread_t threadID[numOfPhilosophers];
    int philosophersNumber[numOfPhilosophers];

    pthread_mutex_init(&mtx, NULL);
    pthread_mutex_init(&printMutex, NULL);

    for (int i = 0; i < numOfPhilosophers; i++)
    {
        pthread_cond_init(&cond_var[i], NULL);
        state[i] = thinking;
        philosophersNumber[i] = i;
    }

    for (int i = 0; i < numOfPhilosophers; i++)
    {
        pthread_create(&threadID[i], NULL, philosopher, &philosophersNumber[i]);
    }

    for (int i = 0; i < numOfPhilosophers; i++)
    {
        pthread_join(threadID[i], NULL);
    }

    pthread_mutex_destroy(&mtx);
    pthread_mutex_destroy(&printMutex);

    for (int i = 0; i < numOfPhilosophers; i++)
    {
        pthread_cond_destroy(&cond_var[i]);
    }

    return 0;
}