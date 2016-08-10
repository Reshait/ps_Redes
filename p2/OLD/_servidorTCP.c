#include "funciones.h"
  int contadorUsuariosLogueados = 0;
/*
 * El servidor ofrece el servicio de un chat
 */

int main ( )
{

    /*----------------------------------------------------
        Descriptor del socket y buffer de datos
    -----------------------------------------------------*/
    system("clear");

    int sd, new_sd, salida, i, k, numClientes=0, t, j, recibidos;
    int m,r,c, existe,m2,z;

    struct sockaddr_in sockname, from;
    char buffer[MSG_SIZE], aux[30];
    socklen_t from_len;
    fd_set readfds, auxfds;
    int arrayClientes[MAX_CLIENTS];
    char identificador[MSG_SIZE],COLOR[20];

    //Vector de clientes
    Cliente clientesSistema[MAX_CLIENTS];

    //Vector de partidas
    Partida P[MAX_PARTIDAS];

    for(r = 0; r < MAX_PARTIDAS; r++)
      P[r].numJugadores = 0;

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


    /*-----------------------------------------------------------------------
        El servidor acepta una petición
    ------------------------------------------------------------------------ */
        while(1){

            //Esperamos recibir mensajes de los clientes (nuevas conexiones o mensajes de los clientes ya estados)

            auxfds = readfds;

            salida = select(FD_SETSIZE,&auxfds,NULL,NULL,NULL);

            if(salida > 0){


                for(i=0; i<FD_SETSIZE; i++){

                    //Buscamos el socket por el que se ha establecido la comunicación
                    if(FD_ISSET(i, &auxfds))
                    {

                        //Si es un cliente nuevo
                        if( i == sd)
                        {

                            if((new_sd = accept(sd, (struct sockaddr *)&from, &from_len)) == -1)
                            {
                                perror("-Err. Error aceptando peticiones");
                            }
                            else
                            {
                                if(numClientes < MAX_CLIENTS)
                                {

                                    FD_SET(new_sd,&readfds);

                                    sprintf(buffer, "+OK. Usuario estado\n");
                                    clientesSistema[numClientes].nombre[0]='\0';
                                    clientesSistema[numClientes].estado=0;//Igualo a 0 para controlar que el usuario esta o no en el sistema
                                    clientesSistema[numClientes].numSock=new_sd;//Asigno el numero de socket que tiene el cliente

                                    printf("El usuario %d se ha conectado.\n",clientesSistema[numClientes].numSock);

                                    bzero(buffer,sizeof(buffer));
                                    sprintf(buffer, "\t\t\t+----------------------------+\n\t\t\t| Bienvenido a LA CASA GANA: |\n\t\t\t+----------------------------+\nPara empezar a jugar al juego del bingo 90 bolas debe realizar lo siguiente:");                                    send(clientesSistema[numClientes].numSock,buffer,strlen(buffer),0);

                                    bzero(buffer,sizeof(buffer));
                                    sprintf(buffer, "\t1.-Si NO está registrado debe hacerlo con el comando: REGISTRO -u <nombreDeUsuario> -p <password>\n\t2.-Si está registrado, debe loguearse con el comando USUARIO <nombreDeUsuario>");
                                    send(clientesSistema[numClientes].numSock,buffer,strlen(buffer),0);

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

                            //Mensajes
                            if(strcmp(buffer,"SALIR\n") ==0)
                            {

                                for(k=0;k<numClientes;k++)
                                {
                                    bzero(buffer,sizeof(buffer));
                                    sprintf(buffer,"-ERR. Desconexión del servidor\n");
                                    send(clientesSistema[k].numSock, buffer,strlen(buffer),0);

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
                                        {
                                            if(clientesSistema[i-4].numero_partida==clientesSistema[t].numero_partida)
                                            {
                                                clientesSistema[t].estado=2;
                                            }

                                        }
                                       // partidas[clientesSistema[i-4].numero_partida].status=0;
                                    }

                                    clientesSistema[j].nombre[0]='\0';
                                    clientesSistema[j].estado=-1;
                                    clientesSistema[j].numSock=-1;

                                    numClientes--;

                                    bzero(buffer,sizeof(buffer));
                                    sprintf(buffer,"Desconexión del cliente: %d\n",i);

                                }

                                //Controlar el nombre de usuario introducido por el cliente
                                else if(strncmp(buffer,"USUARIO ",8) == 0)
                                {
                                  //sscanf(buffer,"USUARIO %s", clientesSistema[i-4].nombre);
                                  sscanf(buffer,"USUARIO %s", aux);

                                  //Busco si el usuario esta ACEPTADO o VALIDADO en el sistema
                                  existe=0;
                                  for(r=0; r<numClientes;r++)
                                  {
                                      //if(strcmp(clientesSistema[r].nombre,clientesSistema[i-4].nombre)==0 || clientesSistema[r].estado==1)
                                      if(strcmp(clientesSistema[r].nombre,aux)==0 || clientesSistema[r].estado==1)
                                      {
                                          strcpy(clientesSistema[i-4].nombre, aux);
                                          printf("Nombres de usuario %d: %s\n",r, clientesSistema[r].nombre);
                                          existe=1;
                                          break;
                                      }
                                  }

                                  //Si el usuario NO está validado o aceptado y existe en la base de datos
                                  if(existe == 0){
                                    //if(encontradoUsuario(clientesSistema[i-4].nombre)){
                                    if(encontradoUsuario(aux)){
                                      bzero(buffer,sizeof(buffer));
                                      sprintf(buffer,"Nombre de usuario asignado: %s\nPara completar el acceso introduzca la contraseña de la siguiente manera\nPASSWORD <nombreDeContraseña>\n", clientesSistema[i-4].nombre);
                                      send(clientesSistema[i-4].numSock, buffer,strlen(buffer),0);
                                      sscanf(buffer,"PASSWORD %s", clientesSistema[i-4].pass);

                                      //Si corresponde el usuario introducido con la contraseña
                                      if(encontradaPassword(clientesSistema[i-4].nombre, clientesSistema[i-4].pass)){
                                        contadorUsuariosLogueados++;
                                        bzero(buffer,sizeof(buffer));
                                        sprintf(buffer,"Usuario Validado correctamente.\n");
                                        send(clientesSistema[i-4].numSock, buffer,strlen(buffer),0);
                                        clientesSistema[i-4].estado = 2; //usuario validado en el sistema
                                      }
                                      //si no corresponde el usuario introducido con la contraseña
                                      else{
                                        bzero(buffer,sizeof(buffer));
                                        sprintf(buffer,"La combinación de nombre y password no coincide, por favor inténtelo de nuevo.\n");
                                        send(clientesSistema[i-4].numSock, buffer,strlen(buffer),0);
                                      }
                                    }
                                    else{
                                      bzero(buffer,sizeof(buffer));
                                      sprintf(buffer,"Nombre de usuario inexistente, inténtelo de nuevo.\n");
                                      send(clientesSistema[i-4].numSock, buffer,strlen(buffer),0);
                                    }
                                  }
                                  else{
                                    bzero(buffer,sizeof(buffer));
                                    sprintf(buffer,"El usuario YA está validado en el sistema\n");
                                    send(clientesSistema[i-4].numSock, buffer,strlen(buffer),0);
                                  }

                                  /*
                                    sscanf(buffer,"USUARIO %s", clientesSistema[i-4].nombre);
                                    //Busco en primer lugar si el usuario esta ACEPTADO o VALIDADO en el sistema
                                    existe=0;
                                    for(r=0; r<numClientes;r++)
                                    {
                                        if(strcmp(clientesSistema[r].nombre,clientesSistema[i-4].nombre)==0 || clientesSistema[r].estado==1)
                                        {
                                            existe=1;
                                            break;
                                        }
                                    }

                                    //Si no esta ni ACEPTADO ni VALIDADO busco si existe en el fichero de usuarios registrados
                                    if(existe==0)
                                    {
                                        //if(encontradoUsuario(aux))
                                        if(encontradoUsuario(clientesSistema[i-4].nombre))
                                        {
                                            clientesSistema[i-4].estado=1;
                                            //strcpy(clientesSistema[i-4].nombre,aux);
                                            bzero(buffer,sizeof(buffer));
                                            sprintf(buffer,"+OK. Usuario aceptado\n");
                                            send(clientesSistema[i-4].numSock, buffer,strlen(buffer),0);
                                        }
                                        else
                                        {
                                            bzero(buffer,sizeof(buffer));
                                            sprintf(buffer,"-ERR. Usuario no existe \n");
                                            send(clientesSistema[i-4].numSock, buffer,strlen(buffer),0);
                                        }

                                    }
                                    //En caso de que ya este VALIDADO mostramos mensaje
                                    if (existe>=1)
                                    {
                                        bzero(buffer,sizeof(buffer));
                                        sprintf(buffer,"-ERR. Ya estas validado\n");
                                        send(clientesSistema[i-4].numSock, buffer,strlen(buffer),0);
                                    }
                                    bzero(buffer,sizeof(buffer));
                                    */
                                }

                                else if(strncmp(buffer,"PASSWORD ",9) == 0)
                                {
                                    sscanf(buffer,"PASSWORD %s", clientesSistema[i-4].pass);
                                    //Comprobamos que la contraseña sea valida
                                    if(encontradaPassword(clientesSistema[i-4].nombre,clientesSistema[i-4].pass) && clientesSistema[i-4].estado==1)
                                    {
                                        bzero(buffer,sizeof(buffer));
                                        sprintf(buffer,"+OK. Contraseña válida.\n");
                                        send(clientesSistema[i-4].numSock, buffer,strlen(buffer),0);
                                        clientesSistema[i-4].estado=2;
                                        printf("Usuario %s se ha conectado\n", clientesSistema[i-4].nombre);
                                    }
                                    else
                                    {
                                        bzero(buffer,sizeof(buffer));
                                        sprintf(buffer,"-ERR. Contraseña incorrecta!\n");
                                        send(clientesSistema[i-4].numSock, buffer,strlen(buffer),0);
                                    }

                                    bzero(buffer,sizeof(buffer));
                                }

                                else if(strncmp(buffer,"REGISTRO ",9) == 0)
                                {
                                  sscanf(buffer,"REGISTRO -u %s -p %s", clientesSistema[i-4].nombre, clientesSistema[i-4].pass);

                                  printf("Registrados correctamente --> USUARIO: %s | CONTRASEÑA %s\n", clientesSistema[i-4].nombre, clientesSistema[i-4].pass);

                                  if(encontradoUsuario(clientesSistema[i-4].nombre))
                                    puts("El usuario ya está registrado");
                                  else
                                    registrarUsuario(clientesSistema[i-4].nombre, clientesSistema[i-4].pass);
                                }

                                else if(strcmp(buffer,"INICIAR-PARTIDA\n") == 0)
                                {
                                  //INICIANDO PARTIDA: *falta por hacer Asignarle mesa-Partida
                                  bzero(buffer,sizeof(buffer));
                                  sprintf(buffer,"Iniciando Partida\n");
                                  send(clientesSistema[i-4].numSock, buffer,strlen(buffer),0);
                                  //Asignación de mesa.
                                  for(r = 0; r < MAX_PARTIDAS; r++){
                                    if(P[r].status == 0){
                                      P[r].jugadores[P[r].numJugadores] = clientesSistema[i-4];
                                      P[r].numJugadores++;
                                      if(P[r].numJugadores == 3)
                                        P[r].status = 2;
                                      break;
                                    }
                                  }

                                  if(P[r].numJugadores < 3){
                                    bzero(buffer,sizeof(buffer));
                                    sprintf(buffer,"Se le ha asignado la mesa número %d\nPara comenzar la partida, su mesa está a la espera de %d jugadores\n", r+1, 3-r);
                                    send(clientesSistema[i-4].numSock, buffer,strlen(buffer),0);
                                  }
                                  else{//Como hacer para mandar el mensaje a todos los de la mesa???
                                    bzero(buffer,sizeof(buffer));
                                    sprintf(buffer,"Iniciando Partida\n");
                                    send(clientesSistema[i-4].numSock, buffer,strlen(buffer),0);
                                  }


                                }

                                else
                                {
                                    sprintf(identificador,"%d: %s",i-4,buffer);
                                    bzero(buffer,sizeof(buffer));
                                    sprintf(buffer,"-ERR. Comando incorrecto\n");
                                    send(clientesSistema[i-4].numSock, buffer,strlen(buffer),0);
                                    strcpy(buffer,identificador);

                                }


                            }

                        }

                    }

                }

            }

        }

        close(sd);
}
