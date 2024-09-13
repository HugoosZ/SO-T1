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

#define FIFO_NAME "fifo_comunicacion"  // Nombre compartido del FIFO

using namespace std;

int main(){
    srand(time(NULL) + getpid());

    if (access(FIFO_NAME, F_OK) != 0) { 
        if (mkfifo(FIFO_NAME, 0666) == -1) {
            perror("mkfifo");
            exit(-1);
        }
        cout << "[Observador] FIFO creado: " << FIFO_NAME << endl;
    } 

    int fd, exp;
    fd = open(FIFO_NAME, O_RDONLY);
    int n = 0;
    ssize_t njugadores = read(fd, &n, sizeof(n)); // Lee el número de jugadores
    close(fd);
    int votos[n];
    int sillas = n - 1;

    while (sillas > 0) {

        for (int i = 0; i < n; i++) {
            votos[i] = 0;
        }
        fd = open(FIFO_NAME, O_RDONLY);
        while (true) {
            int voto;
            ssize_t bytes_read = read(fd, &voto, sizeof(voto));
            if (bytes_read == 0) {
                break;
            }
            if (voto > 0) {
                votos[voto - 1]++;
            }
        }
        close(fd);
        cout<<"-------------------------"<<endl;
        for (int i = 0; i < n; i++) {
            if (votos[i] > 0) {
                cout << "Votos en contra del jugador " << i + 1 << ": " << votos[i] << endl;
            }
        }
        cout<<"-------------------------"<<endl;


        int comparador = 0;
        for (int i = 0; i < n; i++) {
            if (comparador < votos[i]) {
                comparador = votos[i];
            }
        }

        vector<int> mayores;
        for (int i = 0; i < n; i++) {
            if (votos[i] == comparador) {
                mayores.push_back(i + 1);  
            }
        }

        
        int posmayor = mayores[0];
        if (mayores.size() > 1) {
            cout << "Hay un empate, se elegirá a un jugador de manera aleatoria" << endl;
            cout<<"-------------------------"<<endl;

            int random = rand() % mayores.size();  
            posmayor = mayores[random];
        }

        int fd = open(FIFO_NAME, O_WRONLY);
        if (fd == -1) {
            cerr << "Error al abrir FIFO para escribir voto." << endl;
            continue;
        }
        write(fd, &posmayor, sizeof(posmayor));
        cout << "Jugador con mayor voto (a eliminar): " << posmayor << endl;

        close(fd);

        sillas--;
    }
    return 0;
}