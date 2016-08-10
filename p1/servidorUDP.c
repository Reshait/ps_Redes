#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>

/*
 * El servidor ofrece el servicio de incrementar un n\ufffdmero recibido de un cliente
 */

main ()
{
	/*----------------------------------------------------
		Descriptor del socket y buffer de datos
	-----------------------------------------------------*/
	int Socket_Servidor;
	struct sockaddr_in Servidor;
	int contador = 0;
  char horaFormateada[80];
  char Datos[80] = "XX";
  time_t tiempo;
  struct tm * stTm;

  tiempo = time(NULL);
  stTm = localtime(&tiempo);


	/* -----------------------------------------------------
   		Informaci\ufffdn del Cliente
   	-----------------------------------------------------*/
   struct sockaddr_in Cliente;
	socklen_t Longitud_Cliente;

  puts("Servidor Iniciado correctamente.");
	/* --------------------------------------------------
		Se abre el socket Servidor
	---------------------------------------------------*/
	Socket_Servidor = socket (AF_INET, SOCK_DGRAM, 0);
	if (Socket_Servidor == -1)
	{
		printf ("No se puede abrir socket servidor\n");
		exit (-1);
	}

	/* ------------------------------------------------------------------
		Se rellenan los campos de la estructura servidor, necesaria
		para la llamada a la funcion bind().
	-------------------------------------------------------------------*/
	Servidor.sin_family = AF_INET;
	Servidor.sin_port = htons(2000);
	Servidor.sin_addr.s_addr = htonl(INADDR_ANY);

	if (bind (Socket_Servidor, (struct sockaddr *)&Servidor, sizeof (Servidor)) == -1)
	{
		close (Socket_Servidor);
		exit (-1);
	}


  	/*---------------------------------------------------------------------
		Del cliente s\ufffdlo necesitamos el tama\ufffdo de su estructura, el
		resto de informaci\ufffdn (familia, puerto, ip), la obtendremos
		nos la proporcionar\ufffd el propio m\ufffdtodo recvfrom, cuando
		recibamos la llamada de un cliente.
   ----------------------------------------------------------------------*/
	Longitud_Cliente = sizeof (Cliente);


	/*-----------------------------------------------------------------------
		El servidor espera continuamente los mensajes de los clientes
	------------------------------------------------------------------------ */
	while (1)
	{
    /* -----------------------------------------------------------------
			Esperamos la llamada de alg\ufffdn cliente
		-------------------------------------------------------------------*/
    recvfrom (Socket_Servidor, &Datos, sizeof(Datos), 0, (struct sockaddr *) &Cliente, &Longitud_Cliente);
		/* -----------------------------------------------------------------
			Comprobamos si hemos recibido alguna informaci\ufffdn
		-------------------------------------------------------------------*/
		if (strcmp(Datos, "DAY") == 0){
        strftime(horaFormateada,80,"%A, %d de %B de %Y", stTm);
        printf ("Recibido %s, enviando la Fecha: %s\n", Datos, horaFormateada);
		}
    else if(strcmp(Datos, "TIME") == 0){
        strftime(horaFormateada,80,"%H:%M:%S", stTm);
        printf ("Recibido %s, enviando la Hora: %s\n", Datos, horaFormateada);
    }
    else if(strcmp(Datos, "DAYTIME") == 0){
        strftime(horaFormateada,80,"%A, %d de %B de %Y, %H:%M:%S", stTm);
        printf ("Recibido %s, enviando la Fecha y Hora: %s\n", Datos, horaFormateada);
    }
    else{
      puts("Error al recibir el mensaje.\n");
      strcpy(horaFormateada, "Error de recepción");
    }

    sendto (Socket_Servidor, &horaFormateada, sizeof(horaFormateada), 0,  (struct sockaddr *) &Cliente, Longitud_Cliente);

  }

	 close(Socket_Servidor);
}
