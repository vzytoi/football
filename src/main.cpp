#include "Matrix.h"
#include "config.h"
#include "train.hpp"

#include <chrono>
#include <cstddef>
#include <cstdlib>
#include <cstring>
#include <ctime>
#include <iostream>
#include <stdexcept>
#include <string>
#include <thread>
#include <unistd.h>

/*

    TODO: on prend à chaque génération un joueur aléatoire et on voit si sa
   valeur décroit. Si c'est le cas alors les neurones finissent par mourir il y
   a donc un problème.
*/

int main(int argc, char *argv[]) {
    srand(time(NULL));

    if (argc < 2) {
        std::cerr
            << "Aucun argument fourni ! " << std::endl
            << "\t train <genN> <popSize> [nThread] : entraine le "
               "réseaux en sauvegardant les populations"
            << std::endl
            << "\t play <popFile> : fait un tournoi au sein d'une "
               "population et sauvegarde un mach entre les meilleurs"
            << std::endl
            << "\t map <popFile> [output] crée un fichier csv mappant une "
               "population"
            << std::endl;
        _exit(EXIT_FAILURE);
    }

    if (strcmp(argv[1], "train") == 0) {
        unsigned int genN = N;
        unsigned int popSize = POPULATION_SIZE;
        unsigned int nThread = std::thread::hardware_concurrency();
        if (argc >= 3) {
            genN = std::stoi(argv[2]);
        }
        if (argc >= 4) {
            popSize = std::stoi(argv[3]);
        }
        if (argc >= 5) {
            nThread = std::stoi(argv[4]);
        }

        // Il doit au moins y avoir 2 individus par thread car la taille
        // minimale d'une population pour organiser un tournois est de 2.
        if (popSize < 2 * nThread) {
            throw std::invalid_argument("Population trop petite.");
        }

        train(genN, popSize, nThread);
    } else if (strcmp(argv[1], "play") == 0) {
        if (argc < 3) {
            throw std::invalid_argument("Missing argument popFile");
        }
        simulateAndSave(argv[2]);
    } else if (strcmp(argv[1], "map") == 0) {
        if (argc < 3) {
            throw std::invalid_argument("Missing argument popFile");
        }
        if (argc > 3) {
            saveMap(argv[2], argv[3]);
        } else {
            saveMap(argv[2]);
        }
    } else if (strcmp(argv[1], "speedtest") == 0) {
        auto start = std::chrono::system_clock::now();
        train(25, 50, 1);
        auto end = std::chrono::system_clock::now();
        auto duration =
            std::chrono::duration_cast<std::chrono::milliseconds>(end - start);

        std::cout << "Time: " << duration.count() / 25. / 50.
                  << " ms par générations par individus par thread"
                  << std::endl;
    }

    _exit(EXIT_SUCCESS);
}
