/*
Compilar con:
gcc -framework CoreFoundation x-servidorTCP.c para que funcionen los popups
*/

#include "funciones.h"

int main ( )
{
  
	/*---------------------------------------------------- 
		Descriptor del socket y buffer de datos                
	-----------------------------------------------------*/
	system("clear");

    int sd, new_sd, numPartida=0, numeroPartida, m=0,m2=0, existe=0, m3=0, numClientes = 0, p, salida, i, j, k, recibidos, t, c, r, z, q, _i, _j, i2, i3, q2, numero, it, q3;

	struct sockaddr_in sockname, from;
	char buffer[MSG_SIZE], mensaje[MSG_SIZE], aux[30], aux2[30],aux3[30], nombre[30],contrasena[30],cadena[30],*cadena1;
	socklen_t from_len;
    fd_set readfds, auxfds;
    int arrayClientes[MAX_CLIENTS];
    char identificador[MSG_SIZE],COLOR[20], listaBolas[MSG_SIZE], bola[10], mensajeAux[MSG_SIZE];
   	int CARTON[3][9];
    strcpy(listaBolas, "Números "); 


    for(p = 0; p < NUMPARTIDAS; p++)
    {
        bingo[p].status=0;
        bingo[p].numJugadores=0;
        bingo[p].partida_estado=0;
    }



	/* --------------------------------------------------
		Se abre el socket 
	---------------------------------------------------*/
  	sd = socket (AF_INET, SOCK_STREAM, 0);
	if (sd == -1)
	{
		perror("No se puede abrir el socket cliente\n");
    		exit (1);	
	}


	sockname.sin_family = AF_INET;
	sockname.sin_port = htons(2000);
	sockname.sin_addr.s_addr =  INADDR_ANY;

	if (bind (sd, (struct sockaddr *) &sockname, sizeof (sockname)) == -1)
	{
		perror("Error en la operación bind");
		exit(1);
	}
	

   	/*---------------------------------------------------------------------
		Del las peticiones que vamos a aceptar sólo necesitamos el 
		tamaño de su estructura, el resto de información (familia, puerto, 
		ip), nos la proporcionará el método que recibe las peticiones.
   	----------------------------------------------------------------------*/
	from_len = sizeof (from);


	if(listen(sd,1) == -1){
		perror("Error en la operación de listen");
		exit(1);
	}
    
    //Inicializar los conjuntos fd_set
    FD_ZERO(&readfds);
    FD_ZERO(&auxfds);
    FD_SET(sd,&readfds);
    FD_SET(0,&readfds);

    struct timeval timeout;      
    timeout.tv_sec = 1;
    timeout.tv_usec = 0;


	/*-----------------------------------------------------------------------
		El servidor acepta una petición
	------------------------------------------------------------------------ */
	while(1)
	{

        //Aquí hago el correspondiente reparto de numeros a las partidas iniciadas
        for (it = 0; it < NUMPARTIDAS ; it++)
        {

        	if (bingo[it].status==2)
        	{
                do
                {
                    numero=(rand()%90)+1;
                                        		
                }while(bingo[it].sorteo[numero]!=-1);

                bingo[it].sorteo[numero]=1;

                //Comienza el envío de bolas!!
                sprintf(bola, "%d ", numero);
                strcat(listaBolas, bola);
                sprintf(mensajeAux, "%s \n", listaBolas);

                for (q2 = 0; q2 < MAX_USUARIOS_X_MESA; q2++)
                    enviaMensajeAlUsuario(mensajeAux, bingo[it].jugadores[q2].numSock);
            }
        }   
        sleep(1);


        //Esperamos recibir mensajes de los clientes (nuevas conexiones o mensajes de los clientes ya estados)
     
        auxfds = readfds;

        salida = select(FD_SETSIZE,&auxfds,NULL,NULL,&timeout);

        if(salida > 0)
        {
            for(i=0; i<FD_SETSIZE; i++)
            {
                
                //Buscamos el socket por el que se ha establecido la comunicación
                if(FD_ISSET(i, &auxfds)) 
                {
                    
                    //Si es un cliente nuevo
                    if( i == sd)
                    {

                        
                        if((new_sd = accept(sd, (struct sockaddr *)&from, &from_len)) == -1)
                            perror("-Err. Error aceptando peticiones");

                        else
                        {
                            if(numClientes < MAX_CLIENTS)
                            {
                                
                                FD_SET(new_sd,&readfds);
                            
                                sprintf(buffer, "+OK. Usuario estado\n");
                                clientesSistema[numClientes].estado = -1;//Igualo a -1 para controlar que el usuario esta o no en el sistema
                                clientesSistema[numClientes].numSock = new_sd;//Asigno el numero de socket que tiene el cliente
                                enviaMensajeAlUsuario("\t\t\t+----------------------------+\n\t\t\t| Bienvenido a LA CASA GANA: |\n\t\t\t+----------------------------+\nPara empezar a jugar al juego del bingo 90 bolas debe realizar lo siguiente:\n", clientesSistema[numClientes].numSock);
                                enviaMensajeAlUsuario( "\t1.-Si NO está registrado debe hacerlo con el comando: REGISTRO -u <nombreDeUsuario> -p <password>\n\t2.-Si está registrado, debe loguearse con el comando USUARIO <nombreDeUsuario>\n", clientesSistema[numClientes].numSock);

                                numClientes++;


                                for(j=0; j<(numClientes-1);j++)
                                {
                                    bzero(buffer,sizeof(buffer));
                                    sprintf(buffer, "Nuevo Cliente estado: %d\n",new_sd);
                                    send(clientesSistema[j].numSock,buffer,strlen(buffer),0);
                                }
                            }
                            else
                            {
                                bzero(buffer,sizeof(buffer));
                                strcpy(buffer,"Demasiados clientes estados\n");
                                send(new_sd,buffer,strlen(buffer),0);
                                close(new_sd);
                            }
                            
                        }
                        
                        
                    }

                    else if (i == 0)
                    {
                        //Se ha introducido información de teclado
                        bzero(buffer, sizeof(buffer));
                        fgets(buffer, sizeof(buffer),stdin);
                        
                        //Controlar si se ha introducido "SALIR", cerrando todos los sockets y finalmente saliendo del servidor. (implementar)
                        //Mensajes que se quieran mandar a los clientes (implementar)
                        
                        if(strcmp(buffer,"SALIR\n") ==0)
                        {
                        	
                        	for(k=0;k<numClientes;k++)
                        	{
                                enviaMensajeAlUsuario("-ERR. Desconexiónd el serividor\n", clientesSistema[k].numSock);
                                	
                                close(k);
                                FD_CLR(k,&readfds);
                                	
                        	}
                        	return 0;
                        	
                        }                                                    
                    }

                    else
                    {
                        bzero(buffer,sizeof(buffer));
                        
                        recibidos = recv(i,buffer,sizeof(buffer),0);
                        
                        if(recibidos > 0)
                        {
                            //Controlar que un usuario quiera salir
                            if(strcmp(buffer,"SALIR\n") == 0)
                            {
                                
                                close(i);
                                FD_CLR(i,&readfds);
                               
                                //Re-estructurar el array de clientes
                                for (j = 0; j < numClientes - 1; j++)
                                    if (arrayClientes[j] == i)
                                        break;
                                for (; i < numClientes - 1; i++)
                                    (arrayClientes[i] = arrayClientes[i+1]);

                                if(clientesSistema[i-4].estado>2)
                                {
                                    for(t=0;t<5;t++)
                                        if(clientesSistema[i-4].numero_partida == clientesSistema[t].numero_partida)
                                            clientesSistema[t].estado = 2;   //El cliente pasa a estar sólo logueado                             
                                }

                                clientesSistema[j].nombre[0]='\0';
                                clientesSistema[j].estado=-1;
                                clientesSistema[j].numSock=-1;

                                numClientes--;
                                
                                bzero(buffer,sizeof(buffer));
                                //sprintf(buffer,"Desconexión del cliente: %d\n",i);
                                
                            }

                            //Controlar el nombre de usuario introducido por el cliente
                            else if(strncmp(buffer,"USUARIO ",8) == 0)
                            {
                                //Cojo el nombre escrito tras USUARIO
                                sscanf(buffer,"USUARIO %s\n", aux);

                                //Busco en primer lugar si el usuario esta ACEPTADO o VALIDADO en el sistema
                                existe = 0;
                                for(r = 0; r < numClientes; r++)
                                {
                                    if(strcmp(clientesSistema[r].nombre,aux)==0 || clientesSistema[r].estado==1)
                                    {
                                        existe = 1;
                                        break;
                                    }
                                }

                                //Si el usuario NO está ni ACEPTADO ni VALIDADO busco si existe en el fichero de usuarios registrados
                                if(existe == 0)
                                {
                                    if(encontradoUsuario(aux))
                                    {
                                        clientesSistema[i-4].estado = 1;
                                        strcpy(clientesSistema[i-4].nombre,aux);
                                        enviaMensajeAlUsuario("+OK. Usurio aceptado\n", clientesSistema[i-4].numSock);

                                    }
                                    else
                                        enviaMensajeAlUsuario("-ERR. Usuario no existe\n", clientesSistema[i-4].numSock);
                                    
                                }
                                //En caso de que ya este VALIDADO mostramos mensaje 
                                if (existe>1)                                
                                    enviaMensajeAlUsuario("-ERR. Ya estas logueado\n", clientesSistema[i-4].numSock);

                            }

                            else if(strncmp(buffer,"PASSWORD ",9) == 0)
                            {
                                //Copiamos la contraseña que introducimos detras del comando PASSWORD
                                sscanf(buffer,"PASSWORD %s\n", aux);

                                //Comprobamos que la contraseña sea valida
                                if(buscarPassword(clientesSistema[i-4].nombre,aux) && clientesSistema[i-4].estado == 1)
                                {
                                    clientesSistema[i-4].estado = 2; //estado de cliente pasa a ser logueado en el sistema
                                    printf("Usuario %s se ha conectado\n", clientesSistema[i-4].nombre);
                                    enviaMensajeAlUsuario("+OK. Contraseña válida\n", clientesSistema[i-4].numSock);
                                }
                                else
                                    enviaMensajeAlUsuario("-ERR. Contraseña incorrecta!\n", clientesSistema[i-4].numSock);
                            }

                            else if(strncmp(buffer,"REGISTRO ", 9) == 0)
                            {
                                sscanf(buffer,"REGISTRO -u %s -p %s", clientesSistema[i-4].nombre, clientesSistema[i-4].pass);

                                if(encontradoUsuario(clientesSistema[i-4].nombre))
                                    enviaMensajeAlUsuario("-ERR. El usuario ya está registrado, pruebe otro nick.\n", clientesSistema[i-4].numSock);
                                else{
                                    registrarUsuario(clientesSistema[i-4].nombre, clientesSistema[i-4].pass);
                                    printf("Registrados correctamente --> USUARIO: %s | CONTRASEÑA %s\n", clientesSistema[i-4].nombre, clientesSistema[i-4].pass);
                                    clientesSistema[i-4].estado = 1;
                                    enviaMensajeAlUsuario("+OK. Usted ha sido registrado correctamente\n", clientesSistema[i-4].numSock);
                                    sprintf(mensaje,"+OK. Usted acaba de ser conectado con el usuario recien registrado %s, para terminar correctamente el login, introduzca la contraseña: PASSWORD <contraseña>\n", clientesSistema[i-4].nombre);
                                    enviaMensajeAlUsuario(mensaje, clientesSistema[i-4].numSock);
                                }
                            }

                            //Funcion que asigna a un jugador una partida
                            else if(strcmp(buffer,"INICIAR-PARTIDA\n")==0)
                            {
                                //Comprobamos que el usuario esta validado
                                if(clientesSistema[i-4].estado == 2)
                                {
                                    clientesSistema[i-4].estado = 3; // si está validado, mételo en una mesa (estado 3) y espera empezar partida (eempezar partida --> stado 4)

                                    numeroPartida=asignarPartida(bingo,clientesSistema[i-4]);
                                    
                                    if (numeroPartida != -1) //Mesa a la que le faltan jugadores
                                    {
                                        bingo[numeroPartida].jugadores[bingo[numeroPartida].numJugadores] = clientesSistema[i-4];//meto el jugador en esa mesa
                                        bingo[numeroPartida].numJugadores++;
                                        enviaMensajeAlUsuario("+OK. Petición Recibida. Quedamos a la espera de más jugadores\n", clientesSistema[i-4].numSock);

                                        //Le enseñamos el carton al jugador mientras espera mas jugadores

                                        generaCarton(bingo[numeroPartida].jugadores[bingo[numeroPartida].numJugadores-1].carton_jugador);

                                        if (!pasarCarton(bingo[numeroPartida].jugadores[bingo[numeroPartida].numJugadores-1].carton_jugador))
                                        	printf("ERROR AL ESCRIBIR EN EL FICHERO EL CARTON\n");

                                        sprintf(mensaje, "+OK. Se le ha asignado la mesa %d\n", numeroPartida+1);
                                        enviaMensajeAlUsuario(mensaje, bingo[numeroPartida].jugadores[bingo[numeroPartida].numJugadores-1].numSock);

                                       	sleep(1);

                                        sprintf(mensaje, "+OK. Se le ha sentado en la silla %d\n", bingo[numeroPartida].numJugadores);
                                        enviaMensajeAlUsuario(mensaje, bingo[numeroPartida].jugadores[bingo[numeroPartida].numJugadores-1].numSock);

                                        //Comprobar si es el cuarto jugador
                                        if (bingo[numeroPartida].numJugadores == MAX_USUARIOS_X_MESA)// Máximo de jugadores x mesa
                                        {
                                        	bingo[numeroPartida].status=2;

                                            for (q = 0; q < MAX_USUARIOS_X_MESA; q++)//Maximo de jugadores x Mesa
                                                enviaMensajeAlUsuario("+Ok. Va a comenzar la partida\n", bingo[numeroPartida].jugadores[q].numSock);

                                            printf("Partida %d iniciada por los jugadores:\n", numeroPartida+1);

                                            for(q = 0; q < MAX_USUARIOS_X_MESA; q++)
                                                printf("Usuario en silla %d: - %s\n", q+1,  bingo[numeroPartida].jugadores[q].nombre);

                                            for (i3 = 0; i3 < 90; i3++)
                                            	bingo[numeroPartida].sorteo[i3]=-1;

                                            sleep(1); //se puede quitar
                                            
                                        }
                                    }
                                    
                                    else//Si todas las mesas estan llenas entraría en este else 
                                        enviaMensajeAlUsuario("-ERR. Todas las mesas estan llenas. Intentelo de nuevo mas tarde.\n", clientesSistema[i-4].numSock);
                                }
                                else // si no está logueado mensaje de error	                           
                                    enviaMensajeAlUsuario("-ERR. Debe estar validado\n", clientesSistema[i-4].numSock);
                            }                            
                            
                            else if(strcmp(buffer,"UNA-LINEA\n") == 0)
                            {
	                           	printf("Recibido UNA-LINEA del jugador %s\n", clientesSistema[i-4].nombre);
                                printf("El estado de la partida es %d\n", bingo[numeroPartida].status);
                                
                                for(j = 0; j < NUMPARTIDAS; j++){
                                    for(q = 0; q < bingo[j].numJugadores; q++){
                                        if(bingo[j].jugadores[q].numSock == clientesSistema[i-4].numSock){ 
                                            printf("Recibido UNA-LINEA del jugador %s\n", clientesSistema[i-4].nombre);

                                            // Si tiene una línea y no había salido antes en su mesa ninguna línea.
                                            if(lineasEnCarton(bingo[j].jugadores[q].carton_jugador, bingo[j].sorteo) == 1 && bingo[j].partida_estado == 0){
                                                enviaMensajeAlUsuario("PREMIO - usted ha conseguido el premio de una línea\n", clientesSistema[i-4].numSock);
                                                printf("El jugador %s, ha conseguido el premio de una línea en la mesa %d\n", clientesSistema[i-4].nombre, j+1);
                                                bingo[j].partida_estado = 1; // en esta mesa ya ha salido una línea
                                            }
                                            // Si tiene una línea pero ya había salido una línea en su partida.
                                            else if(lineasEnCarton(bingo[j].jugadores[q].carton_jugador, bingo[j].sorteo) == 1 && bingo[j].partida_estado == 1)
                                                enviaMensajeAlUsuario("El premio de una línea ya ha sido otorgado en su partida\n", clientesSistema[i-4].numSock);
                                            // Si es un maldito mentiroso y no tiene línea.
                                            else
                                                enviaMensajeAlUsuario("Usted NO tiene ninguna línea en su cartón\n", clientesSistema[i-4].numSock);
                                                printf("El jugador %s ha MENTIDO en la mesa %d\n", clientesSistema[i-4].nombre, j+1);
                                        }
                                    }
                                }
                            	
                            }

                            else if (strcmp(buffer, "DOS-LINEA\n")==0)
                            {
                                printf("Recibido DOS-LINEA del jugador %s\n", clientesSistema[i-4].nombre);
                                printf("El estado de la partida es %d\n", bingo[numeroPartida].status);
                                
                                for(j = 0; j < NUMPARTIDAS; j++){
                                    for(q = 0; q < bingo[j].numJugadores; q++){
                                        if(bingo[j].jugadores[q].numSock == clientesSistema[i-4].numSock){ 
                                            printf("Recibido DOS-LINEA del jugador %s\n", clientesSistema[i-4].nombre);

                                            // Si tiene una línea y no había salido antes en su mesa ninguna línea.
                                            if(lineasEnCarton(bingo[j].jugadores[q].carton_jugador, bingo[j].sorteo) == 1 && bingo[j].partida_estado == 1){
                                                enviaMensajeAlUsuario("PREMIO - usted ha conseguido el premio de DOS-LÍNEAS\n", clientesSistema[i-4].numSock);
                                                printf("El jugador %s, ha conseguido el premio de DOS-LÍNEAS en la mesa %d\n", clientesSistema[i-4].nombre, j+1);
                                                bingo[j].partida_estado = 2; // en esta mesa ya ha salido una línea
                                            }
                                            // Si tiene una línea pero ya había salido una línea en su partida.
                                            else if(lineasEnCarton(bingo[j].jugadores[q].carton_jugador, bingo[j].sorteo) == 2 && bingo[j].partida_estado == 2)
                                                enviaMensajeAlUsuario("El premio de 2 líneas ya ha sido otorgado en su partida\n", clientesSistema[i-4].numSock);
                                            // Si es un maldito mentiroso y no tiene línea.
                                            else
                                                enviaMensajeAlUsuario("Usted NO tiene ninguna línea en su cartón\n", clientesSistema[i-4].numSock);
                                                printf("El jugador %s ha MENTIDO en la mesa %d\n", clientesSistema[i-4].nombre, j+1);
                                        }
                                    }
                                }
                            }

                            else if (strcmp(buffer, "BINGO\n")==0)
                            {
                                printf("Recibido BINGO del jugador %s\n", clientesSistema[i-4].nombre);
                                printf("El estado de la partida es %d\n", bingo[numeroPartida].status);
                                
                                for(j = 0; j < NUMPARTIDAS; j++){
                                    for(q = 0; q < bingo[j].numJugadores; q++){
                                        if(bingo[j].jugadores[q].numSock == clientesSistema[i-4].numSock){ 
                                            printf("Recibido DOS-LINEA del jugador %s\n", clientesSistema[i-4].nombre);

                                            // Si tiene una línea y no había salido antes en su mesa ninguna línea.
                                            if(lineasEnCarton(bingo[j].jugadores[q].carton_jugador, bingo[j].sorteo) == 1 && bingo[j].partida_estado == 2){
                                                enviaMensajeAlUsuario("PREMIO - usted ha conseguido el premio de BINGO!!\n", clientesSistema[i-4].numSock);
                                                printf("El jugador %s, ha conseguido el premio de BINGO en la mesa %d\n", clientesSistema[i-4].nombre, j+1);
                                                bingo[j].partida_estado = 1; // en esta mesa ya ha salido una línea
                                                //SACAR A JUGADORES Y DEJAR LA MESA LIBRE.
                                            }
                                            // Si tiene una línea pero ya había salido una línea en su partida.
                                            else if(lineasEnCarton(bingo[j].jugadores[q].carton_jugador, bingo[j].sorteo) == 1 && bingo[j].partida_estado == 1)
                                                enviaMensajeAlUsuario("El premio de 2 líneas ya ha sido otorgado en su partida\n", clientesSistema[i-4].numSock);
                                            // Si es un maldito mentiroso y no tiene línea.
                                            else
                                                enviaMensajeAlUsuario("Usted NO tiene ninguna línea en su cartón\n", clientesSistema[i-4].numSock);
                                                printf("El jugador %s ha MENTIDO en la mesa %d\n", clientesSistema[i-4].nombre, j+1);
                                        }
                                    }
                                }
                            }

                            //else if(strcmp(buffer,"SALIR-PARTIDA")==0)
                            //{
                            	/*
								Buscar en que partida esta el que ha escrito SALIR-PARTIDA
								poner estado del jugador como validado
								Comprobar que no es el unico que queda
								y sacar de la partida
								*/
                            //}


                                
                            else
                            {
                                //sprintf(identificador,"%d: %s",i-4,buffer);
                                enviaMensajeAlUsuario("-ERR. Comando incorrecto\n", clientesSistema[i-4].numSock);
                                //strcpy(buffer,identificador);

                            }

                        }

                    }

                }

            }

	    }

	}

	close(sd);
	
}

