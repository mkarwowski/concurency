#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>

int collect_data_from_user(long int **return_value);
int save_data_to_file(const char *filename, long int data);
int get_results(const char *filename, long int **result);
int clear_results_file(const char *filename);

int main() {
    // inicjalizacja zmiennych
    long int *user_data = NULL, *result = NULL;
    int exit_code = 0;
    
    // pobierz dane od użytkownika
    printf("Collecting data from user ... ");
    if ( collect_data_from_user(&user_data) )
        goto error;
    
    // zapisz dane użytkownika do pliku "data"
    printf("Saving data to file ... ");
    if ( save_data_to_file("data", *user_data) )
        goto error;
    printf("SUCCESS!\n");
    
    // pobierz wyniki z pliku "results"
    printf("Awaiting results ... ");
    if ( get_results("results", &result) )
        goto error;

    // jeśli wyniki są null-pointerem to wystąpił błąd
    if ( !result )
        goto error;
    
    // wyświetl wynik z serwera
    printf("RESULT :  %ld\n", *result);

    // wyczyść plik z wynikami
    printf("Clearing results file ... ");
    if ( clear_results_file("results") )
        goto error;
    printf("SUCCESS!\n");
    goto exit;

    // ścieżka w przypadku błędu
    error:
        printf("FAILED!\n");
        exit_code = 1;

    // czyste wyjście z programu
    exit:
        if (user_data)
            free(user_data);
        if (result)
            free(result);
        return exit_code;
}

/**
 * Funkcja pobierająca dane od użytkownika
 * Pobiera adres wskaźnika do umieszczenia danych
 * Zwraca 0 lub 1 w zależności od wystąpienia błędu
 */
int collect_data_from_user(long int **return_value)
{
    // alokacja przestrzeni dla danych
    printf("\nAllocating space for user data ... ");

    if (!return_value)
        return 1;
    
    *return_value = (long int *)malloc(sizeof(long int));
    
    if ( *return_value == NULL )
        return 1;
    printf("SUCCESS!\n");

    // pobranie danych od użytkownika
    printf("Please input a integer value :  ");
    scanf("%ld", *return_value);
    return 0;
}

/**
 * Funkcja zapisująca dane do pliku
 * Pobiera nazwę pliku i dane
 * Zwraca 0 lub 1 w zależności od wystąpienia błędu
 */
int save_data_to_file(const char *filename, long int data)
{
    FILE *file = NULL;

    if ( !(file = fopen(filename, "w")) )
        return 1;
    
    if ( fprintf(file, "%ld", data) < 0 ) {
        fclose(file);
        return 1;
    }

    fclose(file);
    return 0;
}

/**
 * Funkcja pobierająca wyniki z serwera
 * Pobiera nazwę pliku oraz adres wskaźnika do przechowania danych
 * Zwraca 0 lub 1 w zależności od wystąpienia błędu
 */
int get_results(const char *filename, long int **result)
{
    FILE *file = NULL;
    time_t start;
    int c;

    if ( !result )
        return 1;

    start = time(NULL);
    while(1) {
        if ( !(file = fopen(filename, "r")) )
            return 1;
        
        // pętla trwa max 30s
        if ( difftime(time(NULL), start) >= 30.0 ) {
            fclose(file);
            return 1;
        }
        
        c = fgetc(file);
        
        if (c != EOF)
            break;
        
        ungetc(c, file);
        fclose(file);
    }
    ungetc(c, file);
    printf("(%.2fs)\n", difftime(time(NULL), start));

    *result = (long int *)malloc(sizeof(long int));
    
    if ( !fscanf(file, "%ld", *result) ) {
        fclose(file);
        return 1;
    }
    
    fclose(file);
    return 0;
}

/**
 * Funkcja czyszcząca plik z danymi od serwera
 * Pobiera nazwę pliku
 * Zwraca 0 lub 1 w zależności od wystąpienia błędu
 */
int clear_results_file(const char *filename)
{
    FILE *file = NULL;

    if ( !(file = fopen(filename, "w")) )
        return 1;

    fclose(file);
    return 0;
}