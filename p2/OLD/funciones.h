#ifndef FUNCIONES_H
#define FUNCIONES_H

#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define MSG_SIZE 250
#define MAX_CLIENTS 40
#define NUMPARTIDAS 1
#define MAX_PARTIDAS 10



typedef struct usuario // para trabajar con ficheros
{
        char nick[30];
        char pass[30];
}Usuario;

typedef struct cliente // para trabajar en el servidor
 {
    char nombre[30];
    char pass[30];
    int estado;
    /*
        LOS ESTADOS POSIBLES SON:
                0 - USUARIO INACTIVO EN EL SISTEMA
                1 - USUARIO CONECTADO AL SISTEMA
                2 - USUARIO VALIDADO EN EL SISTEMA
                3 - ESPERANDO PARTIDA
                4 - JUGANDO
    */
    int numSock;
    int numero_partida;
    int carton_jugador[3][9];

 }Cliente;



typedef struct partida
 {
        int status;
         /*
        LOS ESTADOS POSIBLES SON:
                0 - PARTIDA SIN EMPEZAR
                1 - PARTIDA ACTIVA
                2 - PARTIDA COMPLETA, COMIENZA EL JUEGO
        */
        int numJugadores;
        Cliente jugadores[4];

 }Partida;





int encontradoUsuario(char cadena[])
{
        Usuario A;
        int encon=0;
        FILE * fich;
        fich=fopen("usuarios.txt","r");
        if(fich)
        {
                while(fscanf(fich,"%s %s",A.nick,A.pass)==2)
                {
                        if(strcmp(cadena,A.nick)==0)
                                encon = 1;
                }
                fclose(fich);

        }
        else
        {
                printf("ERROR al abrir el fichero.\n");
        }
        return encon;
}

int encontradaPassword(char cadena[],char cadena1[])
{
        Usuario A;
        int encon=0;
        FILE * fich;
        if(fich=fopen("usuarios.txt","r"))
        {
                while(fscanf(fich,"%s %s",A.nick,A.pass)==2)
                {
                        if((strcmp(cadena1,A.pass)==0) && (strcmp(cadena,A.nick)==0))
                                encon = 1;
                }
                fclose(fich);

        }
        else
        {
                printf("ERROR al abrir el fichero.\n");
        }

        return encon;
}

int registrarUsuario(char cadena[], char cadena1[])
{
        FILE *f;
        if(f=fopen("usuarios.txt","a"))
        {
                fprintf(f,"%s %s\n",cadena,cadena1);
                fclose(f);
                return 1;
        }
}

int asignarPartida(Partida bingo[], Cliente quiereJugar)
{
    int partida;

    for(partida=0; partida<NUMPARTIDAS; partida++)
    {
        printf("%d\n", partida);
        if(bingo[partida].numJugadores<4)
        {
            return partida;
        }
    }

    return -1;
}

/*int **inicializarCarton(int aux[][9])
{
    int num, i, fil, col;
    int cont_columna[9];
    int cont_fila[3];

    //Pongo el contador de las filas y las columnas a cero
    for ( fil=0; fil<3; fil ++)
    {
        cont_fila[i]=0;
    }
    for ( col=0; col<9; col ++)
    {
        cont_columna[col]=0;
    }


    for ( fil=0; fil<3; fil ++)
    {
        for ( col=0; col<9; col ++)
        {
            aux[fil][col]=-1;
        }
    }

    srand(time(NULL));

    for ( i=0; i<15; )
    {
        fil=rand()%8;
        col=rand()%2;

        if (aux[fil][col]==-1 && cont_fila[fil]<2 && cont_columna[col]<5 )
        {
            num=((rand()%10)+1)+(col*10);
            aux[fil][col]=num;
            i++;
        }
    }
}

void imprimirCarton(Cliente aux)
{
    int fil, col;

    printf("\n\n");

    for ( fil=0; fil<3; fil ++)
    {
        for ( col=0; col<9; col ++)
        {
            if (aux.carton_jugador[fil][col]==-1)
            {
                printf(" X\n");
            }
            else
            {
                printf(" %d\n", aux.carton_jugador[fil][col]);
            }
        }
        printf("\n");
    }

    printf("\n\n");


}*/


//Cliente clientesSistema[MAX_CLIENTS];

#endif
