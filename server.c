#include <stdio.h>
#include <stdlib.h>
#include <math.h>

int ensure_files_exist(const char *input, const char *output);
int observe_file(const char *filename);
int get_data(const char *filename, long int **data);
int clean_data_file(const char *filename);
int operation(long int *data, long int **result);
int save_results(const char *filename, long int result);
long int polynomial(long int x);

int main() {
    // Zapewnienie istnienia plików wejścia i wyjścia dla serwera
    printf("Ensuring in/out files existence...\n");
    if (ensure_files_exist("data", "results")) {
        printf("FAILED!\n");
        return 1;
    }

    // Pętla "nasłuchwiania"
    printf("Listening... \n");
    while (1)
    {
        // jeśli w pliku pojawiły się dane
        if ( !observe_file("data") )
        {
            // inicjalizacja zmiennych
            long int *data = NULL, *result = NULL;

            // pobierz dane z pliku
            if ( get_data("data", &data) )
            {
                printf("Failed to retrieve data from file.\n");
                goto error;
            }
            printf("Retrieved data: %ld\n", *data);

            // wyczyść plik z danymi
            if ( clean_data_file("data") )
            {
                printf("Failed to clean data file.\n");
                goto error;
            }
            printf("Cleaned data file.\n");

            // wykonaj operację na danych
            if ( operation(data, &result) )
            {
                printf("Failed to execute the operation.\n");
                goto error;
            }
            printf("Operation executed.\n");

            // zapisz dane do pliku
            if ( save_results("results", *result) )
            {
                printf("Failed to save results.\n");
                goto error;
            }
            printf("Results saved.\nListening... \n");

            error:
                if (data)
                    free(data);
                if (result)
                    free(result);
        }
    }
    return 0;
}


/**
 * Funkcja zapewniająca istnienie plików IN/OUT
 * Pobiera nazwy plików
 * Zwraca 0 lub 1 w zależności od wystąpienia błędów
 */
int ensure_files_exist(const char *input, const char *output)
{
    FILE *in, *out;
    
    if ( !(in = fopen(input, "r")) ) {
        printf("Input file did not exist, creating... ");
        if ( !(in = fopen(input, "w")) ) {
            return 1;
        }
        printf("SUCCESS!\n");
    } else {
        printf("Input file exists.\n");
    }

    if ( !(out = fopen(output, "r")) ) {
        printf("Output file did not exist, creating... ");
        if ( !(out = fopen(output, "w")) ) {
            return 1;
        }
        printf("SUCCESS!\n");
    } else {
        printf("Output file exists.\n");
    }

    fclose(in);
    fclose(out);
    return 0;
}

/**
 * Funkcja "obserwująca" plik wejścia
 * Pobiera nazwę pliku
 * Zwraca 0 jeśli w pliku pojawiły się dane
 * W przeciwnym przypadku zwraca 1
 * W przypadku błędu również zwraca 1
 */
int observe_file(const char *filename)
{
    FILE *file = NULL;
    int c, return_value = 1;

    if ( !(file = fopen(filename, "r")) )
        return return_value;
    
    c = fgetc(file);
    
    if (c != EOF)
        return_value = 0;
    
    ungetc(c, file);
    fclose(file);
    return return_value;
}

/**
 * Funkcja pobierająca dane z pliku
 * Pobiera nazwę pliku oraz adres wskaźnika do przekazania danych
 * Zwraca 0 lub 1 w zależności od wystąpienia błędu
 */
int get_data(const char *filename, long int **data)
{
    FILE *file = NULL;

    if ( !(file = fopen(filename, "r")) )
        return 1;
    
    if ( !data )
        return 1;
    
    *data = (long int *)malloc(sizeof(long int));

    if ( !fscanf(file, "%ld", *data) ) {
        fclose(file);
        return 1;
    }

    fclose(file);
    return 0;
}

/**
 * Funkcja czyszcząca plik wejścia
 * Pobiera nazwę pliku
 * Zwraca 0 lub 1 w zależności od wystąpienia błędu
 */
int clean_data_file(const char *filename)
{
    FILE *file = NULL;

    if ( !(file = fopen(filename, "w")) )
        return 1;
    
    fclose(file);
    return 0;
}

/**
 * Funkcja wykonująca operację serwera z wykorzystaniem pobranych danych
 * Pobiera wskaźnik na dane oraz adres wskaźnika dla wyniku
 * Zwraca 0 lub 1 w zależności od wystąpienia błędu
 */
int operation(long int *data, long int **result)
{
    if ( !data || !result )
        return 1;
    
    *result = (long int *)malloc(sizeof(long int));

    **result = polynomial(*data);
    return 0;
}

/**
 * Funkcja zapisująca dane do pliku wyjścia
 * Pobiera nazwę pliku oraz dane do zapisania
 * Zwraca 0 lub 1 w zależności od wystąpienia błędu
 */
int save_results(const char *filename, long int result)
{
    FILE *file = NULL;

    if ( !(file = fopen(filename, "w")) )
        return 1;

    if ( fprintf(file, "%ld", result) < 0 ) {
        fclose(file);
        return 1;
    }

    fclose(file);
    return 0;
}

/**
 * Funkcja obliczająca wielomian
 * x^5 + x^3 - 2x + 11
 * dla podanego x
 */
long int polynomial(long int x)
{
    return (long int) pow(x, 5) + (long int) pow(x, 3) - 2 * x + 11;
}