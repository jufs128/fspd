// C implementation for mandelbrot set fractals using libgraph, a simple
// library similar to the old Turbo C graphics library.
/*****************************************************************
 * ATENCAO: a biblioteca libgraph nao funciona com pthreads, entao
 * a solucao do exercicio nao deve chamar as funcoes graficas 
 * NADA SERA EXIBIDO, INFELIZMENTE. :-(
 *****************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>

#define MAXX 640
#define MAXY 480
#define MAXITER 32768
#define NUM_THREADS 2
#define QUEUE_SIZE (4*NUM_THREADS)

FILE* input; // descriptor for the list of tiles (cannot be stdin)
int   color_pick = 5; // decide how to choose the color palette

// params for each call to the fractal function
typedef struct {
	int left; int low;  // lower left corner in the screen
	int ires; int jres; // resolution in pixels of the area to compute
	double xmin; double ymin;   // lower left corner in domain (x,y)
	double xmax; double ymax;   // upper right corner in domain (x,y)
} fractal_param_t;

fractal_param_t EMPTY = {0, 0, 0, 0, 0.0, 0.0, 0.0, 0.0};
fractal_param_t EOW = {-1, -1, -1, -1, -1.0, -1.0, -1.0};

int is_eow(fractal_param_t fractal) {
    return fractal.left == EOW.left && fractal.low == EOW.low &&
	   fractal.ires == EOW.ires && fractal.jres == EOW.jres &&
	   fractal.xmin == EOW.xmin && fractal.ymin == EOW.ymin &&
           fractal.xmax == EOW.xmax && fractal.ymax == EOW.ymax;
}

typedef struct {
    fractal_param_t array[QUEUE_SIZE];
    unsigned begin;
    unsigned end;
    unsigned size;
} circular_queue;

void create_queue(circular_queue *queue) {	
    queue->begin = 0;
    queue->end = 0;
    queue->size = 0;
}

int push(circular_queue *queue, fractal_param_t item) {
    if (queue->size == QUEUE_SIZE)
	return 1;
    
    queue->array[queue->end] = item;
    queue->end = (queue->end + 1) % (QUEUE_SIZE);
    queue->size++;

    return 0;
}

int pop(circular_queue *queue, fractal_param_t *item) {
    if (queue->size == 0)
	return 1;
    
    *item = queue->array[queue->begin];
    
    queue->begin = (queue->begin + 1) % (QUEUE_SIZE);
    queue->size--;

    return 0;
}

// Fila circular que será utilizada pelas threads
circular_queue QUEUE;

// Variáveis de sincronização para controlar acesso à mesma
pthread_mutex_t queue_lock;
pthread_cond_t cond_push_queue;
pthread_cond_t cond_queue_full;


/****************************************************************
 * Nesta versao, o programa principal le diretamente da entrada
 * a descricao de cada quadrado que deve ser calculado; no EX1,
 * uma thread produtora deve ler essas descricoes sob demanda, 
 * para manter uma fila de trabalho abastecida para as threads
 * trabalhadoras.
 ****************************************************************/
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

/****************************************************************
 * A funcao a seguir faz a geracao dos blocos de imagem dentro
 * da area que vai ser trabalhada pelo programa. Para a versao
 * paralela, nao importa quais as dimensoes totais da area, basta
 * manter um controle de quais blocos estao sendo processados
 * a cada momento, para manter as restricoes desritas no enunciado.
 ****************************************************************/
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
				color = ( k >> color_pick ) %16;
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

void *worker_thread(void *data) {
    while (1) {
	pthread_mutex_lock(&queue_lock);
	
	fractal_param_t param;
	// Espera fila encher caso esteja vazia
	while (0 != pop(&QUEUE, &param)) {
	    pthread_cond_wait(&cond_queue_full, &queue_lock);	    
	}
	
	pthread_mutex_unlock(&queue_lock);
	if (QUEUE.size < NUM_THREADS)
	    pthread_cond_signal(&cond_push_queue);
	    
	if (is_eow(param)) {
	    pthread_exit(EXIT_SUCCESS);
	}

	printf("Fractal\n");
    }
    pthread_exit(EXIT_SUCCESS);
}

void *master_thread(void *) {
    pthread_t workers[NUM_THREADS];
    for (unsigned i = 0; i < NUM_THREADS; ++i) {
	pthread_create(&workers[i], NULL, &worker_thread, NULL);
    }

    int is_eof = 0;
    unsigned eof_to_add = NUM_THREADS;
    while (1) {
        pthread_mutex_lock(&queue_lock);    
	// Espera ter menos trabalho que o número de threads
	while (QUEUE.size > NUM_THREADS) {
	    pthread_cond_wait(&cond_push_queue, &queue_lock);
	}

	// Satisfeita a condição, preencher a fila o máximo possível
	fractal_param_t param;
	while (QUEUE.size < QUEUE_SIZE) {
	    // Caso seja fim de arquivo, indica EOW para todas threads
	    if (EOF == input_params(&param) && eof_to_add > 0) {
		for (unsigned i = 0; i < eof_to_add; ++i) {
		    if (0 == push(&QUEUE, EOW))
			eof_to_add--;
		}
		is_eof = 1;
		break;
	    } else {
		push(&QUEUE, param);
	    }
	}
	
	pthread_cond_signal(&cond_queue_full);
	pthread_mutex_unlock(&queue_lock);
	
	if (is_eof == 1)
	  break;	
    }
    
    for (unsigned i = 0; i < NUM_THREADS; ++i) {
	pthread_join(workers[i], NULL);
    }
    
}

int main ( int argc, char* argv[] )
{
	int i,j,k;
	fractal_param_t p;

	if ((argc!=2)&&(argc!=3)) {
		fprintf(stderr,"usage %s filename [color_pick]\n", argv[0] );
		exit(-1);
	} 
	if (argc==3) {
		color_pick = atoi(argv[2]);
	} 
	if ((input=fopen(argv[1],"r"))==NULL) {
		perror("fdopen");
		exit(-1);
	}

	create_queue(&QUEUE); 
	pthread_mutex_init(&queue_lock, NULL);
	pthread_cond_init(&cond_push_queue, NULL);
	pthread_cond_init(&cond_queue_full, NULL);
	
	pthread_t master;
	
	pthread_create(&master, NULL, &master_thread, NULL);

	pthread_join(master, NULL);
	
	return 0;
}

