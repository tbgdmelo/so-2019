/*
Developed by: Thiago Braga de Melo
ICOMP - Instituto de Computação
UFAM - Universidade Federal do Amazonas
tbm@icomp.ufam.edu.br
Atividade: Proto Terminal - Sistemas Operacionais
*/
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <dirent.h>
#include <sys/wait.h>
#include <fcntl.h>


int main (int argc, char *argv[]){

	char diretorio_atual[256];
	//Pegar o diretorio atual
	getcwd(diretorio_atual, 256);

	char linha[256];
	char *palavra;
	char *argumentos[256];
	char comando[256];

	int i;
	
	while(1){
		//printf("Sou o processo (pid:%d)\n", (int) getpid());
		sleep(1);
		printf("@modoDeus:D~%s # ", diretorio_atual);
		
		scanf("%[^\n]", linha);
		scanf("%*c");
		//Separar as palavras por espaço
		i=0;
		palavra = strtok(linha, " ");
		argumentos[i]=palavra;
		do{
			palavra = strtok(NULL," ");
			if(palavra){	//Se tiver uma palavra
				i++;
				argumentos[i]=palavra; //Armazena cada argumento em uma pos do vetor
			}
		}
		while(palavra); //Enquanto tiver palavra
		i++;
		argumentos[i]=NULL; //Final do vetor



		//Verifica se o comando é o CD
		if (strcmp(argumentos[0],"cd")==0){
			if(chdir (argumentos[1])==-1){ //Se não encontrar o caminho
				printf("Diretório não existe!\n");
			}
			else{
				//Mudar o diretório para o argumento passado atualizando o atual
				getcwd(diretorio_atual, 256);
			} 
		}
		//Verifica se o comando é o PWD
		else if (strcmp(argumentos[0],"pwd")==0){
			printf("O diretório atual é: %s\n", getcwd(diretorio_atual, 256) );
		}
		//É operação com executável
		else{
			//Apenas executar
			if (!argumentos[1]){ 
				//Cria um novo processo
				int processo = fork();
				
				//Falha no Fork
				if(processo < 0){
					fprintf(stderr, "Erro no Fork.\n");
					exit(1);
				}
				else if(processo == 0){
					printf("\nExecutando o programa...\n");
					sleep(1);

					//Concatenar o comando "./" com o nome do programa
					strcat(comando, "./");
					strcat(comando, argumentos[0]);
					argumentos[0] = comando;
					execvp(argumentos[0], argumentos);

				}
				else{
					wait(NULL);
				}
			}
			//Liga a saída do primeiro executável à entrada do segundo via um PIPE
			else if(strcmp(argumentos[1],"|")==0){
				int arq_desc[2];

				//Criar o Pipe
				if(pipe(arq_desc) < 0){
					fprintf(stderr, "Erro no Pipe.\n");
					exit(1);
				}
				//fprintf(stderr, "%d %d\n", arq_desc[0], arq_desc[1]);
				int processo = fork();

				if(processo < 0){
					fprintf(stderr, "Erro no Fork.\n");
					exit(1);
				}
				else if(processo == 0){
					//Configura os descritores
					close(1);
					dup(arq_desc[1]);
					close(arq_desc[1]);
					close(arq_desc[0]);
					//Concatenar o comando "./" com o nome do programa
					strcat(comando, "./");
					strcat(comando, argumentos[0]);
					argumentos[0] = comando;
					execvp(argumentos[0], argumentos);
					//execlp(argumentos[0], argumentos[0],NULL);
				}
				else{
					//Configura os descritores
					close(0);
					dup(arq_desc[0]);
					close(arq_desc[0]);
					close(arq_desc[1]);
					//Concatenar o comando "./" com o nome do programa
					strcat(comando, "./");
					strcat(comando, argumentos[2]);
					argumentos[2] = comando;
					execvp(argumentos[2], argumentos);
					//execlp(argumentos[2], argumentos[2],NULL);
				}
			}
			//Altera a saída do programa para um arquivo
			else if(strcmp(argumentos[1],">")==0){
				int processo = fork();
				if(processo < 0){
					fprintf(stderr, "Erro no Fork.\n");
					exit(1);
				}
				else if(processo == 0){
					close(STDOUT_FILENO);
					//Criar o arquivo para receber a saida
					open(argumentos[2], O_CREAT|O_WRONLY|O_TRUNC, S_IRWXU);
					//Concatenar o comando "./" com o nome do programa
					strcat(comando, "./");
					strcat(comando, argumentos[0]);
					argumentos[0] = comando;
					execvp(argumentos[0], argumentos);
				}
				else{
					wait(NULL);
				}
			}
			//Alterar entrada do programa para um arquivo
			else if(strcmp(argumentos[1],"<")==0){
				int processo = fork();
				if(processo < 0){
					fprintf(stderr, "Erro no Fork.\n");
					exit(1);
				}
				else if(processo == 0){
					//Abre o arquivo e redireciona o padrão para stdin
					freopen(argumentos[2], "r", stdin);
					//Concatenar o comando "./" com o nome do programa
					strcat(comando, "./");
					strcat(comando, argumentos[0]);
					argumentos[0] = comando;
					execvp(argumentos[0], argumentos);
					
				}
				else{
					wait(NULL);
				}
			}
			else{
				printf("Comando indisponível. \nVerifique a sintaxe. :)\n");
			}
		}
	}
}