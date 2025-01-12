#include <stdio.h>
#include <stdlib.h>

// Funktion, um das optimale Golomb-Linial zu berechnen
void golomb_berechnen(int laenge, int* golomb) {
    int i, j;
    golomb[0] = 1; // Anfangswert für die Golomb-Linie

    for (i = 1; i < laenge; i++) {
        int min = golomb[i - 1] + 1;
        int max = golomb[i - 1] * 2;

        for (j = min; j <= max; j++) {
            int sum = 0;
            for (int k = 0; k < i; k++) {
                if (golomb[k] <= j) {
                    sum += golomb[k];
                }
            }
            if (sum == j) {
                golomb[i] = j;
                break;
            }
        }
    }
}

int main() {
    int laenge = 11; // Länge des Golomb-Linials
    int* golomb = (int*)malloc(laenge * sizeof(int));

    golomb_berechnen(laenge, golomb);

    printf("Optimales Golomb-Linial für Länge %d:\n", laenge);
    for (int i = 0; i < laenge; i++) {
        printf("%d ", golomb[i]);
    }
    printf("\n");

    free(golomb);
    return 0;
}