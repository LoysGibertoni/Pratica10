#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <math.h>

void copiar(char* origem, char* destino)
{
    FILE* arquivo_origem = fopen(origem, "rb+");
    FILE* arquivo_destino = fopen(destino, "wb+");

    fseek(arquivo_origem, 0, SEEK_END);
    long int tamanho = ftell(arquivo_origem);
    fseek(arquivo_origem, 0, SEEK_SET);
    printf("Data size of %ld bytes:\n", tamanho);

    unsigned char byte;
    long int i;
    for(i = 0; i < tamanho; i++)
    {
        fread(&byte, sizeof(unsigned char), 1, arquivo_origem);
        fwrite(&byte, sizeof(unsigned char), 1, arquivo_destino);
    }

    fclose(arquivo_origem);
    fclose(arquivo_destino);
}

void* monitorar(void* argv)
{
    long int tamanho_total;
    long int tamanho_copiado = 0;
    int tempo_total = 0;
    
    FILE* arquivo_origem = fopen(((char**)argv)[1], "rb+");
    fseek(arquivo_origem, 0, SEEK_END);
    tamanho_total = ftell(arquivo_origem);
    fseek(arquivo_origem, 0, SEEK_SET);
    fclose(arquivo_origem);

    while(tamanho_copiado < tamanho_total)
    {
        sleep(1);

        FILE* arquivo_destino = fopen(((char**)argv)[2], "rb+");
        fseek(arquivo_destino, 0, SEEK_END);
        long int tamanho_janela = ftell(arquivo_destino) - tamanho_copiado;
        tamanho_copiado += tamanho_janela;
        fseek(arquivo_destino, 0, SEEK_SET);
        fclose(arquivo_destino);

        int tempo_restante = ceil((tamanho_total - tamanho_copiado) / tamanho_janela);
        if(tempo_restante > 0)
        {
            printf("Time remaining: %d seconds\n", tempo_restante);
        }

        tempo_total++;
    }

    printf("Total time: %d seconds\n", tempo_total);
}

int main(int argc, char* argv[])
{
    pthread_t monitor;
    pthread_create(&monitor, NULL, monitorar, (void*)argv);

    copiar(argv[1], argv[2]);

    pthread_join(monitor, NULL);

    return 0;
}