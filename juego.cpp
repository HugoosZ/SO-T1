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
    if (system("g++ ./Observador.cpp") != 0) { 
            cerr << "Error al compilar Observador.cpp" << endl;
            return 1;
    }
    //if (system("./Observador &") == -1) { //ejecución observador
      //          cerr << "Error al ejecutar el observador" << endl;
         //       return 1;
    //}
    cout << "[Juego] Iniciando proceso principal." << endl;
    srand(time(NULL) + getpid());
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
    int *t = base;          
    *t = (rand() % 4) + 1;
    int *sumacumulada = base + 1;  
    *sumacumulada = 0; 
    int *sincro = base + 2;  
    *sincro = 0;
    int *sincronizadorbool = base + 3;
    *sincronizadorbool = 0;
    int *expulsado = base + 4;  
    *expulsado = 0;
    int *sumacumuladaux = base + 5;  
    *sumacumuladaux = 0;  
    int *liberador = base + 6;  
    int *votantes = base + 7;  
    *votantes = 0;
    int ganadores = 0;
    int n = 0;
    int contadordeexpulsados = 0;
    int contadordeexpulsadosfinal=0;

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
            srand(time(NULL) + getpid());

            break;
        }
    }




    int posaux = pos;
    

    

        

    int sillas = n-1;
    *liberador = 0;


    
        while (sillas > 0 ) {
            int vote = 0;

            if(pos == 0){
                cout<<"Cantidad de sillas "<<sillas<<endl;
            }

            vote = (rand() % n) + 1;
            if(pos>0){
                fd = open(FIFO_NAME, O_WRONLY);

            while (expulsados[vote] != 0) {
                vote = (rand() % n) + 1;
            }
                
                if(write(fd, &vote, sizeof(vote))){
                    *votantes=*votantes+1;
                    if(*votantes == (sillas+1)){
                        *sincronizadorbool = 1;

                    }
                }
                    cout<<"El voto del proceso "<<pos<<" es "<<vote<<endl;
            }

            while(*sincronizadorbool == 0){
                sleep(1);
            }
            if(pos>0){
                close(fd);
            }

           

            if(pos>0){
                sleep(2);
            }



            int exp = 0;
            if(pos == 0){
                
                fd = open(FIFO_NAME, O_RDONLY); 
                ssize_t bytes_read = read(fd, &exp, sizeof(exp));
                
                close(fd);
                if(exp == 0){
                    cout<<"WELTAAA"<<endl;
                    continue;

                }
                cout << "[Juego] Expulsado: " << exp << endl;
                *expulsado = exp;
            }

            while(*expulsado == 0){
                sleep(1);
            }
            

            if(pos>0){
            cout<<"El expulsado es "<<*expulsado<<" En el proceso " << pos<<endl;

            }
            sleep(1);
            contadordeexpulsadosfinal++;
        
                if (pos == *expulsado && pos > 0) {
                    cout << "[Juego] Proceso en posición " << pos << " fue eliminado!!!!!!!!!!!!!!" << endl;
                    execlp("./amurra_y_reclama", "", NULL);
                }   
                else {
                    expulsados[*expulsado] = 1;
                    if(pos>0){
                        (*sumacumuladaux)++;


                        while(*sumacumuladaux != *votantes-1 ){

                        }
                        sleep(1);
                        *sincro = 1;

                    }
                }
                sleep(5);

                exp=0;
                if(pos==0){
                    while(*sincro==0){
                        if(contadordeexpulsadosfinal == n){
                            break;
                        }
                    }
                    *votantes = 0;
                    *sumacumuladaux = 0;
                    *sincronizadorbool = 0;
                    *sincro=0;
            }
            exp = 0;             
            sillas--;
            ganadores = 0;
            contadordeexpulsados = 0;
            

        }
    
    
    *liberador = 1;
    if(pos == 0){
        wait(NULL);
        cout<<"Termino el padre"<<endl;
        exit(0);
    }
    else{
        cout << "Proceso en posición " << pos << " GANOooooOOOOO!." << endl;
        exit(0);
        
    }
    unlink(FIFO_NAME);
    if (shmdt(base) == -1) {
        perror("Error desmontando segmento de memoria compartida");
        exit(1);
    }
    
    return(0);
}