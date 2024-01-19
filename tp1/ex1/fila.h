#ifndef FILA_H
#define FILA_H

#include <stdio.h>
#include <stdlib.h>

// params for each call to the fractal function
typedef struct {
	int left; int low;  // lower left corner in the screen
	int ires; int jres; // resolution in pixels of the area to compute
	double xmin; double ymin;   // lower left corner in domain (x,y)
	double xmax; double ymax;   // upper right corner in domain (x,y)
} fractal_param_t;

//FILA CIRCULAR

//Variáveis da fila
int comeco;   //Começo da fila
int tamanho;  //Tamanho da fila (número de elementos)
int max_fila; //Tamanho máximo que a fila pode alcançar
fractal_param_t *fila; //Array da fila circular das tarefas

//Funções para mexer na fila
int inserir_tarefa(fractal_param_t tarefa);

int remover_tarefa();

void imprimir_fila();

#endif