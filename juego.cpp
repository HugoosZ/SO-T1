#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/wait.h>
#include <cstdlib>  // Para rand() y srand()
#include <iostream>
#include <sys/ipc.h>
#include <sys/shm.h> 
#include <vector> 
#include <climits>  // Para acceder a INT_MAX


#define FIFO_NAME "fifo_comunicacion"  // Nombre compartido del FIFO

using namespace std;

int main(){
    cout << "[Juego] Iniciando proceso principal." << endl;

    int fd,sumt= 0;
    int shmid = shmget(IPC_PRIVATE, sizeof(int) * 2, IPC_CREAT | 0666);
    if (shmid == -1) {
        perror("Error creando segmento de memoria compartida");
        exit(1);
    }

    // Adjuntar el segmento de memoria compartida
    int *base = (int*) shmat(shmid, NULL, 0);
    if (base == (void*) -1) {
        perror("Error adjuntando segmento de memoria compartida");
        exit(1);
    }

    // Definir las dos variables en distintas posiciones dentro del segmento
    int *time = base;          // La primera variable apunta al inicio del segmento
    *time = rand() % 4 + 1;
    int *sumacumulada = base + 1;  
    *sumacumulada = 0; 
    int *sincronizadorpid = base + 2;  
    pid_t *sincronizador = (pid_t*) sincronizadorpid;
    int *sincronizadorbool = base + 3;
    *sincronizadorbool = 0;
    int *expulsado = base + 4;  
    *expulsado = 0;
    int *sumacumuladaux = base + 5;  
    *sumacumuladaux = 0;  
    int *liberador = base + 6;  

    int ganadores = 0;
    int n = 0;
    int contadordeexpulsados = 0;

    cout<<"El tiempo de la cancion es" <<*time<<endl;

    cout << "[Juego] Ingrese el número de jugadores: ";

    cin >> n;
    if(n < 2){
        cout << "[Juego] Deben haber al menos 2 jugadores." << endl;
        cin >> n;
    }
    
    cout<<"La cantidad de jugadores es "<<n<<endl;
    fd = open(FIFO_NAME, O_WRONLY);
    cout<<"[Juego] apertura de fifo con valor" <<fd<<endl;
    write(fd, &n, sizeof(n));
    cout << "[Juego] Enviando cantidad de jugadores: " << n << " a observador." << endl;
    close(fd);
    for(int i = 1; i <= n; i++){ //Sumatoria
        sumt += i;
    }
    cout<<"La suma es "<<sumt<<endl;

    if (access(FIFO_NAME, F_OK) != 0) { 
        if (mkfifo(FIFO_NAME, 0666) == -1) {
            perror("mkfifo");
            exit(-1);
        }
        cout << "[Juego] FIFO creado: " << FIFO_NAME << endl;
    } else {
        cout << "[Juego] FIFO ya existe, omitiendo la creación." << endl;
    }

    pid_t pid;
    int pos = 0;
    int expulsados[n+1];
    for(int i = 0; i <= n; i++){
        expulsados[i] = 0;
    }



    for (int i = 1; i <= n; i++) {
        if ((pid = fork()) == -1) {
            perror("fork");
            exit(-1);
        }
        else if (pid == 0) {
            pos = i;
            break;
        }
    }





    *sincronizador = getpid();
    int posaux = pos;
    
    while(*sincronizadorbool == 0){
        sleep(1);
        if(*sincronizadorpid == getpid()){
            for(int i = 1; i <= n; i++){
            }
            *sincronizadorbool = 1;
        }
    }
    
    cout << "Sali del while con pos = " << pos << endl;

        

    int sillas = n-1;
    *liberador = 0;

    if(pos == 0){
        while(*liberador==0){

        }
        cout<<"Termino el padre"<<endl;
        exit(0);
    }
    else{
    
        
        while (sillas > 0 ) {
            
            *sincronizador = getpid();

            sumt = 0;

            *sumacumuladaux = 0;  

            fd = open(FIFO_NAME, O_WRONLY);
            
            cout<<"apertura de fifo de votacion con valor" <<fd<<endl;

            int vote = rand() % n + 1;

            while(true){
                if(expulsados[vote] == 0 ){
                    break;
                }
                else{
                    vote = rand() % n + 1;
                    if(expulsados[vote] == 0){
                        break;
                    }
                }
            }
            
            cout << "[Juego] voto: " << vote << " de jugador " << pos<< "cuando quedan "<<sillas <<" sillas"<< endl;

            write(fd, &vote, sizeof(vote));
            sleep(5);
            close(fd);
            for(int i=1;i<=n;i++){ //se puede quitar por el tema de las sillas!
                if(expulsados[i]==0){   
                    ganadores++;
                }
            }
            int exp = 0;
            fd = open(FIFO_NAME, O_RDONLY);
            ssize_t bytes_read = read(fd, &exp, sizeof(exp));
            close(fd);
            cout<<"El valor de exp es "<<exp<<" En el proceso " << pos<<endl;
            if(exp > 0){
                *expulsado = exp;
                *sumacumulada = *expulsado*ganadores; 
            }
            cout<<"El expulsado es "<<*expulsado<<" En el proceso " << pos<<endl;
            
            while(*expulsado == 0){


            }
            sleep(5);
            sumt=*expulsado;
            cout<<"La sumt es "<<sumt<<endl;

            
            cout << "[Juego] Recibido jugador a eliminar: " << *expulsado << "cuando quedan "<<sillas <<" sillas"<<endl;
            *sincronizador = getpid();
            sleep(5);
            if (pos == *expulsado) {
                cout << "[Juego] Proceso en posición " << pos << " fue eliminado!!!!!!!!!!!!!!" << endl;
                execlp("./amurra_y_reclama", "", NULL);
            }   
            else {
                *sumacumuladaux += sumt;
                cout<<"La sumacumuladaux es "<<*sumacumuladaux<<endl;
                
                expulsados[*expulsado] = 1;
                for(int i=1;i<=n;i++){
                    if(expulsados[i]==1){
                        contadordeexpulsados++;
                    }
                }
                cout<<"contadordeexpulsados vale "<<contadordeexpulsados<<endl;

                while (*sumacumulada-*expulsado != *sumacumuladaux) {
                    sleep(1);
                }
            }
            cout<<"Proceso"<<pos<<"pasó el while"<<endl;

            expulsados[*expulsado] = 1;
            sleep(5);
            for(int i = 1; i <= n; i++){
                cout<<"La pos "<<i<<" vale "<<expulsados[i]<<endl;
            }
            exp=0;
            sumt = sumt - exp;
            sillas--;
            *sumacumulada = 0;
            while(*sincronizadorbool == 0){
                sleep(1);
                if(*sincronizadorpid == getpid()){
                    for(int i = 1; i <= n; i++){
                    }
                    *sincronizadorbool = 1;
                }
            }
            ganadores = 0;
            contadordeexpulsados = 0;

        }
    }
    
    *liberador = 1;
    if(pos == 0){
        sleep(1);
        cout<<"Termino el padre"<<endl;
        exit(0);
    }
    else{
        cout << "Proceso en posición " << pos << " GANOooooOOOOO!." << endl;
        exit(0);
        
    }
    unlink(FIFO_NAME);

    
    return(0);
}