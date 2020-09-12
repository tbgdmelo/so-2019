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

int pessoas_banheiro=0;
int pessoas_usaram=0;
int limpando=0;

sem_t porta_banheiro; //Controla o banheiro todo
sem_t user; //Controla os usuários
sem_t mutex;

typedef struct _programador {
   int id;
   int tbanheiro;
   int ttrab;
} programador;
//Seta os valores da struct
void informacoes_set_p(programador *prog, int id, int ttrab, int tbanheiro){
    prog->id = id;
    prog->ttrab = ttrab;
    prog->tbanheiro = tbanheiro;
}

typedef struct _limpador {
   int tlimp;
   int tesp;
} limpador;
//Seta os valores da struct
void informacoes_set_l(limpador *limp, int tlimp, int tesp){
    limp->tlimp = tlimp;
    limp->tesp = tesp;
}
void* Programadores(void *arg){
    programador info = *((programador*)arg);
    while(1){
        sem_wait(&user); 
        sem_wait(&mutex); //Limpeza não pode entrar pq tem gente

        if(pessoas_banheiro==0){ //Verfica se está vazio
            if(limpando==1){ //Se tiver alguem limpando, aguarda
                sem_wait(&mutex);
            }
        }
        sem_post(&mutex);

        if(pessoas_banheiro<3){
            sem_post(&user);
            sem_wait(&porta_banheiro);
            sem_wait(&user); 
        }
        pessoas_banheiro++;

        //utiliza o banheiro
        sem_post(&user);
        printf("O programador entrando no banheiro eh: %d\n", info.id);
	    sleep(info.tbanheiro );
        sem_wait(&user);
        pessoas_banheiro--;
        pessoas_usaram++;
        sem_post(&porta_banheiro);
        sem_post(&user);
        printf("O programador %d foi trabalhar\n", info.id);
        sleep(info.ttrab);
    }
}

void* Limpeza(void *arg){
    limpador info = *((limpador*)arg);
    while(1){
        sleep(info.tesp); //Espera o tempo determinado pra limpar
        //Para todo mundo
        sem_wait(&mutex);
        sem_wait(&user);
        sem_wait(&porta_banheiro);

        //Limpa o banheiro
        limpando=1;
        printf("Limpeza entrando no banheiro...\n");
        printf("Ate o momento passaram pelo banheiro: %d\n", pessoas_usaram);
        printf("Limpando o banheiro...\n");
        sleep(info.tlimp);
        pessoas_usaram=0;
        printf("Limpeza saindo do banheiro\n");
        limpando=0;
        sem_post(&mutex);
        sem_post(&user);
        sem_post(&porta_banheiro);

    }
}

int main(int argc, char *argv[]){
    int tbanheiro, ttrab, tlimp, tesp;
    if (argc != 4) {
      fprintf(stderr, "Argumentos: tbanheio, ttrab, tlimp, tesp\n");
      exit(1);
    }
    tbanheiro = atoi(argv[1]);
    ttrab = atoi(argv[2]);
    tlimp = atoi(argv[3]);
    tesp = atoi(argv[4]);
    int i=0;
    pthread_t l;
    pthread_t p[10];

    programador pr; //programador
    limpador li; //limpador


    sem_init(&mutex,0,1);
    sem_init(&porta_banheiro, 0,1);
    sem_init(&user, 0,3);
    while(1){
        informacoes_set_l(&li,tlimp,tesp);
        pthread_create(&l,NULL,Limpeza,(void*)&li);
        for (i=0;i<10;i++){
            informacoes_set_p(&pr,i,ttrab,tbanheiro);
            pthread_create(&p[i],NULL,Programadores,(void*)&pr);
            pthread_join(p[i],NULL);
        }
    }
    

}