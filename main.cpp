#include <iostream>
#include <pthread.h>
#include <unistd.h>
#include <cstdlib>
#include <ctime>

using namespace std;

const int numOfPhilosophers = 5;

enum { thinking, eating };

int state[numOfPhilosophers];

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

    cout << "Forks #" << leftFork << " and #" << rightFork
         << " with" << philosopherNumber << endl;
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
        cout << "Philosopher #" << philosopherNumber
             << " took " << thinkTime << "ms thinking" << endl;
        usleep(thinkTime * 1000);

        pickup_forks(philosopherNumber);

        int eatTime = (rand() % 3 + 1) * 1000;
        cout << "Philosopher #" << philosopherNumber
             << " took " << eatTime << "ms eating" << endl;

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
    for (int i = 0; i < numOfPhilosophers; i++)
        {

    }

    return 0;
}