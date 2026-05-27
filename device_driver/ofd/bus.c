#include <stdio.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>

int waiting = 0, boarded = 0;
int capacity, total;
sem_t mutex, bus, go;
void *civilian(void *arg)
{
    int id = *(int *)arg;
    sem_wait(&mutex);
    waiting++;
    printf("C%d waiting\n", id);
    sem_post(&mutex);
    sem_wait(&bus);
    printf("C%d boarded\n", id);
    sem_wait(&mutex);
    boarded++;
    waiting--;
    sem_post(&mutex);
    if (boarded == capacity || waiting == 0)
        sem_post(&go);
    else
        sem_post(&bus);
    return NULL;
}
void *bus_driver(void *arg)
{
    while (1)
    {
        sleep(1); 
        sem_wait(&mutex);
        if (waiting == 0)
        {
            sem_post(&mutex);
            break; 
        }
        sem_post(&mutex);
        printf("\nBus arrived\n");
        boarded = 0;
        sem_post(&bus);
        sem_wait(&go);
        printf("Bus leaving with %d passengers\n", boarded);
    }
    return NULL;
}
int main()
{
    printf("Enter number of civilians: ");
    scanf("%d", &total);
    printf("Enter bus capacity: ");
    scanf("%d", &capacity);
    pthread_t c[total], b;
    int id[total];
    sem_init(&mutex, 0, 1);
    sem_init(&bus, 0, 0);
    sem_init(&go, 0, 0);
    for (int i = 0; i < total; i++)
    {
        id[i] = i + 1;
        pthread_create(&c[i], NULL, civilian, &id[i]);
    }
    pthread_create(&b, NULL, bus_driver, NULL);
    for (int i = 0; i < total; i++)
        pthread_join(c[i], NULL);
    pthread_join(b, NULL);
    sem_destroy(&mutex);
    sem_destroy(&bus);
    sem_destroy(&go);
    return 0;
}
