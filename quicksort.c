#include <stdio.h>
#include <stdlib.h>

#define LINHAS 1000
#define COLUNAS 100000

int matriz[LINHAS][COLUNAS];

int comparar(const void *a, const void *b) {
	return (*(int *) a - *(int *) b);
}

int main() {
	int i, j;

	for (i = 0; i < LINHAS; i++) {
		for (j = 0; j < COLUNAS; j++) {
			matriz[i][j] = COLUNAS - j;
		}
	}

	for (i = 0; i < LINHAS; i++) {
		qsort(matriz[i], COLUNAS, sizeof(int), comparar);
	}
	return 0;
}
