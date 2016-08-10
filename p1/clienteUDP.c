#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <stdlib.h>

/* --------------------------------------------------------------------------------------

 Env�a un n�mero aleatorio al servidor, quien el devuelve el n�mero incremetnado

---------------------------------------------------------------------------------------- */

main (int argc, char **argv )
{

		/*----------------------------------------------------
			Descriptor del socket y buffer para datos
		-----------------------------------------------------*/
		int Socket_Cliente;
		int opcion, salida;
		char Datos[80];
    int intentos = 0;
    int recibido = 0;
    int enviado = 0;
    char horaFormateada[80];
		struct timeval timeout;
		fd_set lectura;

		//Inicializar la estructua timeval
		timeout.tv_sec = atoi(argv[2]);
		timeout.tv_usec = 0;

   	/* -----------------------------------------------------
   		Informaci\ufffdn del Servidor
   	-----------------------------------------------------*/
   	struct sockaddr_in Servidor;
   	socklen_t Longitud_Servidor;

    if(argc != 3){
      printf("ERROR. La sintaxis argumental debe ser:\n\t%s númeroIP cantidad_tiempo_espera\nej:\t%s 172.0.0.1 3", argv[0], argv[0]);
      exit(EXIT_FAILURE);
    }

   	/* --------------------------------------------------
			Se abre el socket cliente
		---------------------------------------------------*/
		Socket_Cliente = socket (AF_INET, SOCK_DGRAM, 0);
		if (Socket_Cliente == -1)
		{
			printf ("No se puede abrir el socket cliente\n");
    		exit (-1);
		}

 		/*---------------------------------------------------------------------
			Necesitamos una estructura con la informaci\ufffdn del Servidor
			para poder solicitarle un servicio.
   		----------------------------------------------------------------------*/
		Servidor.sin_family = AF_INET;
		Servidor.sin_port = htons(2000);
		Servidor.sin_addr.s_addr =  inet_addr(argv[1]);
  	Longitud_Servidor = sizeof(Servidor);

  	do{
  	   puts("1.- Para enviar DAY:");
  	   puts("2.- Para enviar TIME:");
  	 	 puts("3.- Para enviar DAYTIME:");
  	 	 scanf("%d", &opcion);
  	}while(opcion < 1 || opcion > 3);

  	if(opcion == 1) sprintf(Datos, "DAY");
  	else if(opcion == 2) sprintf(Datos, "TIME");
  	else sprintf(Datos, "DAYTIME");

   	printf ("Envio %s...\n", Datos);

   	do{
      //envío solicitud y espero 5 sg
      enviado = sendto (Socket_Cliente, Datos, sizeof(Datos), 0, (struct sockaddr *) &Servidor, Longitud_Servidor);
			timeout.tv_sec = atoi(argv[2]);
			timeout.tv_usec = 0;
			FD_ZERO(&lectura);
//			FD_SET(0,&lectura);
			FD_SET(Socket_Cliente, &lectura);

		  //espero respuesta
			salida = select(Socket_Cliente+1,&lectura,NULL,NULL,&timeout);

			if(salida == -1)
				printf("Se ha producido un error en select\n");
			else if(salida == 0)
				printf("Se ha agotado el tiempo\n");
			else
      	recibido = recvfrom (Socket_Cliente, horaFormateada, sizeof(horaFormateada), 0, (struct sockaddr *) &Servidor, &Longitud_Servidor);
      intentos++;// contador para salir del bucle
    }while(recibido < 1 && intentos != 3);
		/*-----------------------------------------------------------------------
			Se env\ufffda mensaje al Servidor
		-----------------------------------------------------------------------*/
   	if (recibido > 0)
    	printf ("Recibo:\t%s\n", horaFormateada);
   	else
    	printf ("Error al leer del servidor\n");

		close(Socket_Cliente);
}
