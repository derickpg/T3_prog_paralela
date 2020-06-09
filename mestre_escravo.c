#include <stdio.h>
#include <stdlib.h>
#include "mpi.h"


#define COLUNA 1000
#define LINHA 100000

//Função de comparação QS
int compare (const void * a, const void * b) {
   return ( *(int*)a - *(int*)b );
}


// Bubble sort
void bs(int n, int * vetor)
{
    int c=0, d, troca, trocou =1;

    while (c < (n-1) & trocou )
        {
        trocou = 0;
        for (d = 0 ; d < n - c - 1; d++)
            if (vetor[d] > vetor[d+1])
                {
                troca      = vetor[d];
                vetor[d]   = vetor[d+1];
                vetor[d+1] = troca;
                trocou = 1;
                }
        c++;
        }
}

int main(int argc, char* argv[]) {
  int id, n, message, aux;
  int i;
  int j;	
  double ti, tf; // Tempos
  //Dados da Matriz
  //int coluna = 1000; // Quantidade de Vetores
  //int linha  = 100000; // Quantidade de Elementos
  int ordenado[LINHA];
  int ord = 1; // 1 == QS ; 0 == BS
  int v_ok = COLUNA;
  int pos_fim = COLUNA;

  MPI_Status status;

  MPI_Init(&argc , &argv); // funcao que inicializa o MPI, todo o codigo paralelo estah abaixo
  MPI_Comm_rank(MPI_COMM_WORLD, &id); // pega pega o numero do processo atual (rank)
  MPI_Comm_size(MPI_COMM_WORLD, &n);  // pega informacao do numero de processos (quantidade total)
  if ( id == 0 ) {  // MESTRE
      ord = 1;
      printf("\n Começo da Ordenação! Ordenação do tipo: %d",ord);

      int Matriz[COLUNA][LINHA]; 
     // Iniciando a Matriz NÃO DEVE CONTAR COMO TEMPO DE ORDENAÇÃO
     for ( i = 0; i < COLUNA; i++)
	 for ( j = 0; j < LINHA; j++){
	        aux = rand() % 100;
		Matriz[i][j] = aux;
	 }
     
     //printf("\n VETORES PRONTOS \n");

     //DEBUG VETORES PRONTOS

   /*  for ( i = 0; i < COLUNA; i++){
	 for ( j = 0; j < LINHA; j++){
	        printf("%d;", Matriz[i][j]);
	 }
	 printf("\n");
     } */
	ti = MPI_Wtime();
	v_ok = COLUNA;
	pos_fim = COLUNA;
	while(v_ok > 0){
		//printf("\n VOU PARA O WHILE v_ok = %d \n",v_ok);
		//COMEÇA O TEMPO DE ORDENAÇÃO
	     	// Envio das mensagens para os escravos
	     	for ( i=1 ; i < n && v_ok > 0; i++) { // PARA CADA ESCRAVO
		 	MPI_Send(&Matriz[v_ok - 1], LINHA, MPI_INT, i, 1, MPI_COMM_WORLD);
			v_ok = v_ok -1;
	     	}
	     	// Recebimento das respostas
	     	for ( i=1 ; i < n && pos_fim > 0;  i++) {
			 MPI_Recv(Matriz[pos_fim - 1], LINHA, MPI_INT, MPI_ANY_SOURCE, 1, MPI_COMM_WORLD, &status);
			 pos_fim = pos_fim - 1;
			 //printf("\nMestre recebeu do escravo %d e o i = %d", status.MPI_SOURCE, i);
	     	}
	}
	

	// MSG para matar os escravos
 	i = 1;
	for ( i=1 ; i < n ; i++) { // PARA CADA ESCRAVO
		//printf("\n VOU MATAR OS PROCESSOS! %d \n", i);
		MPI_Send(0, 0, MPI_INT, i, 2, MPI_COMM_WORLD);
  	}
	
  }
  else {  // ESCRAVO
	int recebido[LINHA];
	while(1){
	     // Recebe mensage
	     MPI_Recv(&recebido, LINHA, MPI_INT, 0, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
	     if(status.MPI_TAG == 2){
			  //printf("\n SOU O PROCESSO %d e estou saindo! \n",id);
			  MPI_Finalize();
  			  return 0;		
	     }
	     // Escolhe método de ordenação
	     if(ord == 0){ //Se for BS
		 bs(LINHA,recebido);
	     }else{ // se for QS
		 //printf("\n Escravo %d está ordenando", id);
		 qsort(recebido, LINHA, sizeof(int), compare);
	     }
	     // Retorna resposta para o mestres
	     MPI_Send(&recebido, LINHA, MPI_INT, 0, 1, MPI_COMM_WORLD);
	}
  }
  tf = MPI_Wtime();
  double total_time;
  total_time = tf - ti;
  printf("\n TEMPO TOTAL = %f \n", total_time);
  MPI_Finalize();
  return 0;
}
