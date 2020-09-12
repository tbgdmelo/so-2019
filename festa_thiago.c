/**
 Universidade Federal do Amazonas - Icomp
 Aluno: Thiago Braga de Melo - 21650633 - tbm@icomp.ufam.edu.br
*/

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <semaphore.h>
#include <time.h>

#define NINDIOS 5
#define MULTIPLICADOR 100000000

int caldeirao=0;
static const char * animais[]= {"Onca", "Capivara", "Mico Leao"};

sem_t cacique; //Controlar o cacique
sem_t indio; //Controlar os indios
pthread_mutex_t alimentacao = PTHREAD_MUTEX_INITIALIZER; //Controlar as alterações no caldeirao

//Struct para ir como parametro nas threads dos indios
typedef struct _informacoes {
   int id;
   struct timespec time;
   struct timespec time2;
} informacoes;
//Seta os valores da struct
void informacoes_set(informacoes *info, int id, int tcomer){
    info->id = id;
    info->time.tv_sec = 0;
    info->time.tv_nsec = tcomer * MULTIPLICADOR;
}

void* vaiCacar(void *arg){
    while(1){
        sem_wait(&cacique); //Cacique dorme
        pthread_mutex_lock(&alimentacao); //Cacique trava o caldeirao
        
        printf("Cacique: Uga uga! Acordei!\nCacique foi cacar...\n");
        sleep(3);
        srand(time(NULL));

        int comida=rand()%3;

        if(comida==0){
            caldeirao = 15;
        }
        else if(comida==1){
            caldeirao = 10;
        }
        else{
            caldeirao = 2;
        }

        printf("Cacique: Uga uga! Cacique voltou com %s.\n",animais[comida]);
        sleep(3);
        printf("Cacique: Agora cacique vai dormir.\n");
        sleep(2);
        pthread_mutex_unlock(&alimentacao); //Cacique libera o caldeirao
        sem_post(&indio); //Avisa que já tem comida
    }
}

void* filarBoia(void *arg){
    //int id = *((int*)arg);
    pthread_mutex_lock(&alimentacao); //Pega a trava para se alimentar
    informacoes info = *((informacoes*)arg); //Struct com id e tcomer do indio

    if(caldeirao==0){ //Verifica se tem algo pra comer
        //Chama o cacique
        printf("Indio %d: Ishe, nao tem comida! Indio %d vai chamar cacique.\n", info.id, info.id);
        sleep(3);
        pthread_mutex_unlock(&alimentacao); //Destrava caldeirao pro cacique caçar
        sem_post(&cacique); //Chama o cacique
        sem_wait(&indio); //Espera o cacique voltar
    }

    caldeirao--; //Come

    printf("Indio %d comendo...\n",info.id);
    //sleep(5);
    nanosleep(&info.time, &info.time2);
    nanosleep(&info.time, &info.time2);
    //Dois nanosleep para não ficar muito rapido

    printf("Indio %d: Uga uga! Eu indio %d comi do caldeirao.\n", info.id, info.id);
    sleep(2);
    pthread_mutex_unlock(&alimentacao); //Devolve a trava para os outros comerem
}

int main(int argc, char *argv[]){
    if (argc != 2) {
      fprintf(stderr, "Argumentos: tempoComer (valor de 1 a 9)\n");
      exit(1);
    }
    int tcomer;
    tcomer = atoi(argv[1]);

    //Definindo o tempo de comer
    //struct timespec time, time2;
    //time.tv_sec = 0;
    //time.tv_nsec = tcomer * MULTIPLICADOR;
    //Definindo o tempo de comer

    informacoes info;
    
    int i=0;
    
    pthread_t indigenas[NINDIOS]; //Threads dos indios

    pthread_t t_cacique; //Thread do cacique

    pthread_create(&t_cacique,NULL,vaiCacar,(void*)NULL); //Cria o cacique
    sem_init(&cacique, 0,0); //Inicia o semaforo do cacique para ele dormir logo de inicio
    
    //Preparacao pra ficar mais divertido :)
    printf("A festa na aldeia já vai começar...\n");
    sleep(2);
    printf("Chamando todos os indios!!!\n");
    sleep(4);
    printf("A festa comecou!\n");
    sleep(2);
    //Preparacao pra ficar mais divertido :)

    while(1){
        for(i=0;i < NINDIOS;i++){
            informacoes_set(&info,i,tcomer);
            pthread_create(&indigenas[i],NULL,filarBoia,(void*)&info);
            pthread_join(indigenas[i],NULL);
        }
    }
}