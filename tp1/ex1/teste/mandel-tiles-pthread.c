#include "fila.h"

#include <math.h>
#include <sys/time.h>
#include <time.h>
#include <unistd.h>
#include <pthread.h>

#define MAXX 640
#define MAXY 480
#define MAXITER 32768

FILE* input; // descriptor for the list of tiles (cannot be stdin)

int num_threads = 20; // Número de threads a serem utilizadas
pthread_mutex_t fila_mut = PTHREAD_MUTEX_INITIALIZER; // Mutex para controlar o acesso à fila

pthread_mutex_t id_mut = PTHREAD_MUTEX_INITIALIZER; //Para dar id às threads
int cont_id = 0;
int total_tarefas = 0; //Total de tarefas
int *num_tarefas; //Número de tarefas por trabalhador
float media_tarefas_trab = 0.0; //Média de tarefas por trabalhador
float desvio_padrao_tarefas = 0.0; //Desvio padrão do tarefas por trabalhador

#define MAXTAREFAS 2048
float tempo_tarefas[MAXTAREFAS]; //Array de tempo por tarefa
int cont_id_tarefa = 0; //Contagem para preencher array
float media_tempo_tarefa = 0.0; //Média de tempo por tarefa
float desvio_padrao_tempo = 0.0; //Desvio padrão do tempo por tarefa

int num_fila_vazia = 0; //Número de vezes que encontrou a fila vazia

//Leitura das tarefas nos arquivos
int input_params( fractal_param_t* p )
{ 
	int n;
	n = fscanf(input,"%d %d %d %d",&(p->left),&(p->low),&(p->ires),&(p->jres));
	if (n == EOF) return n;

	if (n!=4) {
		perror("fscanf(left,low,ires,jres)");
		exit(-1);
	}
	n = fscanf(input,"%lf %lf %lf %lf",
		 &(p->xmin),&(p->ymin),&(p->xmax),&(p->ymax));
	if (n!=4) {
		perror("scanf(xmin,ymin,xmax,ymax)");
		exit(-1);
	}
	return 8;

}

// Function to draw mandelbrot set
void fractal( fractal_param_t* p )
{
	double dx, dy;
	int i, j, k, color;
	double x, y, u, v, u2, v2;

    /*************************************************************
     * a funcao rectangle deve ser removida na versao com pthreads,
     * ja que as bibliotecas sao conflitantes.
     *************************************************************/
	//rectangle(p->left, p->low, p->left+p->ires-1, p->low+p->jres-1);

	dx = ( p->xmax - p->xmin ) / p->ires;
	dy = ( p->ymax - p->ymin ) / p->jres;
	
	// scanning every point in that rectangular area.
	// Each point represents a Complex number (x + yi).
	// Iterate that complex number
	for (j = 0; j < p->jres; j++) {
		for (i = 0; i <= p->ires; i++) {
			x = i * dx + p->xmin; // c_real
			u = u2 = 0; // z_real
			y = j * dy + p->ymin; // c_imaginary
			v = v2 = 0; // z_imaginary

			// Calculate whether c(c_real + c_imaginary) belongs
			// to the Mandelbrot set or not and draw a pixel
			// at coordinates (i, j) accordingly
			// If you reach the Maximum number of iterations
			// and If the distance from the origin is
			// greater than 2 exit the loop
			for (k=0; (k < MAXITER) && ((u2+v2) < 4); ++k) {
				// Calculate Mandelbrot function
				// z = z*z + c where z is a complex number

				// imag = 2*z_real*z_imaginary + c_imaginary
				v = 2 * u * v + y;
				// real = z_real^2 - z_imaginary^2 + c_real
				u  = u2 - v2 + x;
				u2 = u * u;
				v2 = v * v;
			}
			if (k==MAXITER) {
				// converging areas are black
				color = 0;
			} else {
				// graphics mode has only 16 colors;
				// choose a range and recycle!
				color = ( k >> num_threads ) %16;
			}
			// To display the created fractal
			/******************************************************
                         * a funcao putpixel deve ser removida na versao com pthreads,
                         * ja que as bibliotecas sao conflitantes. Infelizmente,
                         * isso significa que sua versao final nao tera uma saida
                         * grafica real.
                         ******************************************************/
			//putpixel(p->left+i, p->low+j, color);
		}
	}
}

//Coloca tarefas nos espaços disponíveis da fila circular
int popula_lista() {
	fractal_param_t p;
    int limite = max_fila - tamanho; //Número de tarefas a serem adicionadas é o máximo de tarefas
                                     //menos o número de elementos já na lista

    for (int i = 0; i < (limite); i++) {
        if (input_params(&p) == EOF) //Se chegou ao fim do arquivo, retorna 1
            return 1;
        inserir_tarefa(p); //Insere tarefa na fila
        total_tarefas++;
    }
    return 0;
}

//Thread do mestre
void *thread_entrada() {
    int eof = 0; //Indicador se chegou no fim do arquivo; 0 se não chegou, 1 se chegou

    //Popula inicialmente
    pthread_mutex_lock(&fila_mut); //Mutex bloqueia a fila enquanto a thread está mexendo nela
    eof = popula_lista();
    pthread_mutex_unlock(&fila_mut);

    //Até chegar ao fim do arquivo
    while(eof == 0) {
        if (tamanho <= num_threads) { //Se o número de tarefas é igual ao número de threads, popula novamente
            pthread_mutex_lock(&fila_mut); //Mutex bloqueia a fila enquanto a thread está mexendo nela
            eof = popula_lista();
            pthread_mutex_unlock(&fila_mut);
        }
    }
    
    fractal_param_t eow;
    eow.left = -1; //EOW possui left = -1, já que os valores corretos vão de 0 a 640
    int eow_restantes = num_threads; //EOW a serem adicionados é o número de threads

    while (eow_restantes > 0) { //Enquanto há EOWs a adicionar
        while((tamanho < max_fila) && (eow_restantes > 0)) { //Enquanto houver espaço e faltar EOWs para adicionar, adiciona tarefa na fila
            pthread_mutex_lock(&fila_mut); //Mutex bloqueia a fila enquanto a thread está mexendo nela
            inserir_tarefa(eow);
            eow_restantes--; //Diminui o número de EOWs a serem adicionados
            pthread_mutex_unlock(&fila_mut);
        }
    }
    
    pthread_exit(EXIT_SUCCESS);
}

//Thread dos trabalhadores
void *thread_calcula() {
    pthread_mutex_lock(&id_mut); //ID para salvar estatísticas nos vetores
    int id = cont_id;
    cont_id++;
    pthread_mutex_unlock(&id_mut);
    num_tarefas[id] = 0;

    int id_tarefa; // Variável com id de cada tarefa no array que guarda tempo de execução
    struct timespec start, stop;

    while(1) {
        clock_gettime(CLOCK_REALTIME, &start);
        pthread_mutex_lock(&fila_mut); //Mutex bloqueia a fila enquanto a thread está mexendo nela
        fractal_param_t tarefa = fila[comeco % max_fila];
        if (remover_tarefa() == -1) { //Se fila está vazia
            num_fila_vazia++;
        }
        id_tarefa = cont_id_tarefa;
        cont_id_tarefa++; //Aumenta contagem de id das tarefas
        pthread_mutex_unlock(&fila_mut);

        if (tarefa.left == -1) { //Se é EOW, sai do loop de calcular 
            clock_gettime( CLOCK_REALTIME, &stop);
            cont_id_tarefa--; //Retira a adição no id se é um EOW
            break;
        } else { //Se não é, calcula
            fractal(&tarefa);
            clock_gettime(CLOCK_REALTIME, &stop);

            num_tarefas[id]++; //Aumenta a quantidade de tarefas da thread
            tempo_tarefas[id_tarefa] = ((stop.tv_sec - start.tv_sec)*1000) + ((stop.tv_nsec - start.tv_nsec)/1000000); //Converte tempo de execução para ms e salva
        }
    }

    pthread_exit(EXIT_SUCCESS);
}

int main(int argc, char* argv[]) {
    
    if ((argc!=2)&&(argc!=3)) {
		fprintf(stderr,"usage %s filename [num_threads]\n", argv[0] );
		exit(-1);
	}
	if (argc==3) {
		num_threads = atoi(argv[2]);
	}
	if ((input=fopen(argv[1],"r"))==NULL) {
		perror("fdopen");
		exit(-1);
	}

    //Inicia fila circular vazia
    comeco = 0;
    tamanho = 0;
    max_fila = num_threads*4; //Tamanho máximo da fila é o número de threads multiplicado por 4
    fila = (fractal_param_t*) malloc(max_fila * sizeof(fractal_param_t));

    //Inicia listas de estatísticas
    num_tarefas = (int*) malloc(num_threads * sizeof(int));

    //Definição das threads
    pthread_t *threads_trabalhadoras = (pthread_t*) malloc(num_threads * sizeof(pthread_t));
    pthread_t thread_mestre;

    //Criação das threads
    pthread_create(&thread_mestre, NULL, thread_entrada, NULL);
    for (int i = 0; i < num_threads; i++) {
        pthread_create(&threads_trabalhadoras[i], NULL, thread_calcula, NULL);
    }

    // Espera cada uma das threads terminarem
    pthread_join(thread_mestre, NULL);
    for (int i = 0; i < num_threads; i++) {
        pthread_join(threads_trabalhadoras[i], NULL);
    }

    //Estatísticas de número de tarefas
    for (int i = 0; i < num_threads; i++) { //Média
        media_tarefas_trab += num_tarefas[i];
    }
    media_tarefas_trab /= num_threads;

    for (int i = 0; i < num_threads; i++) { //Desvio padrão
        desvio_padrao_tarefas += pow((num_tarefas[i] - media_tarefas_trab), 2);
    }
    desvio_padrao_tarefas = sqrt(desvio_padrao_tarefas / num_threads);

    printf("Tarefas: total = %d;  média por trabalhador = %f(%f)\n", total_tarefas, media_tarefas_trab, desvio_padrao_tarefas);

    //Estatísticas de tempo
    for (int i = 0; i < cont_id_tarefa; i++) { //Média
        media_tempo_tarefa += tempo_tarefas[i];
    }
    media_tempo_tarefa /= total_tarefas;

    for (int i = 0; i < cont_id_tarefa; i++) { //Desvio padrão
        desvio_padrao_tempo += pow((tempo_tarefas[i] - media_tempo_tarefa), 2);
    }
    desvio_padrao_tempo = sqrt(desvio_padrao_tempo / total_tarefas);

    printf("Tempo médio por tarefa: %.6f (%.6f) ms\n", media_tempo_tarefa, desvio_padrao_tempo);

    //Estatística do número de vezes que a fila foi encontrada vazia
    printf("Fila estava vazia: %d vezes\n", num_fila_vazia);

    return 0;
}