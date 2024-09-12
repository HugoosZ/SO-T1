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
    srand(static_cast<unsigned int>(std::time(nullptr)));

    cout << "[Observador] Iniciando proceso observador." << endl;
    if (access(FIFO_NAME, F_OK) != 0) { 
        if (mkfifo(FIFO_NAME, 0666) == -1) {
            perror("mkfifo");
            exit(-1);
        }
        cout << "[Observador] FIFO creado: " << FIFO_NAME << endl;
    } else {
        cout << "[Observador] FIFO ya existe, omitiendo la creación." << endl;
    }

    int fd, exp;
    fd = open(FIFO_NAME, O_RDONLY);
    cout << "[Observador] FIFO abierto para lectura: " << FIFO_NAME << " con valor " << fd << endl;

    int n = 0;
    ssize_t njugadores = read(fd, &n, sizeof(n)); // Lee el número de jugadores
    close(fd);
    cout << "[Observador] Número de jugadores recibido: " << n << endl;

    int votos[n];
    int sillas = n - 1;

    while (sillas > 0) {
        cout << "[Observador] Cantidad de sillas restantes: " << sillas << endl;

        // Inicializar todos los votos en 0
        for (int i = 0; i < n; i++) {
            votos[i] = 0;
        }

        // Leer los votos de los jugadores
        fd = open(FIFO_NAME, O_RDONLY);
        while (true) {
            int voto;
            ssize_t bytes_read = read(fd, &voto, sizeof(voto));
            if (bytes_read == 0) {
                break;
            }
            if (voto > 0) {
                cout << "[Observador] Voto recibido: " << voto << endl;
                votos[voto - 1]++;
            }
        }
        close(fd);

        for (int i = 0; i < n; i++) {
            if (votos[i] > 0) {
                cout << "[Observador] Votos en contra del jugador " << i + 1 << ": " << votos[i] << endl;
            }
        }

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
            int random = rand() % mayores.size();  
            posmayor = mayores[random];
        }

        fd = open(FIFO_NAME, O_WRONLY);
        cout << "[Observador] FIFO abierto para escritura: " << FIFO_NAME << " con valor " << fd << endl;

        write(fd, &posmayor, sizeof(posmayor));
        cout << "[Observador] Jugador con mayor voto (a eliminar): " << posmayor << endl;

        close(fd);

        sillas--;
    }

    cout << "[Observador] Finalizando proceso observador." << endl;
    return 0;
}