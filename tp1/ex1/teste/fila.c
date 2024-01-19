#include "fila.h"

int comeco;   //Começo da fila
int tamanho;  //Tamanho da fila (número de elementos)
int max_fila; //Tamanho máximo que a fila pode alcançar
fractal_param_t *fila; //Array da fila circular das tarefas

//Inserir tarefa na fila
int inserir_tarefa(fractal_param_t tarefa) {
	if(tamanho == max_fila) //Se fila está cheia
    	return -1;

	fila[((comeco + tamanho) % max_fila)] = tarefa; //Comeco do array + tamanho mod tamanho máximo da fila é
													 //o índice a ser inserido o próximo elemento
   tamanho ++; //Mais um elemento na fila

	return 0;
}

//Remove o primeiro elemento da fila (a fila é FIFO)
int remover_tarefa() {
	if( tamanho == 0 ) //Se fila está vazia
    	return -1;
   
   comeco ++; //Aumentando o começo da fila em 1 e
	tamanho --; //diminuindo o número de elementos em 1 apaga-se o primeiro elemento

	return 0;
}

//Imprime a fila
void imprimir_fila() {
   for(int i = 0; i < max_fila; i++) {
      printf("fila[%i] = left: %i low: %i\n", i, fila[i].left, fila[i].low);
      printf("           ires: %i jres: %i\n", fila[i].ires, fila[i].jres);
      printf("           xmin: %f jres: %f\n", fila[i].xmin, fila[i].ymin);
      printf("           xmax: %f jres: %f\n", fila[i].xmax, fila[i].ymax);
   }
}
