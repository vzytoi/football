#include <cstdlib>
#include <fstream>
#include <iostream>
#include <tuple>
#include <utility>
#include <vector>

#include "Chromosome.hpp"
#include "Game.hpp"
#include "Population.hpp"
#include "config.h"
#include "util.hpp"

Population::Population(int size) {
    this->size = size;
    this->pop = new Chromosome *[size];

    for (int i = 0; i < size; i++) {
        this->pop[i] = new Chromosome();
        this->pop[i]->initialize();
    }
}

Population::~Population() {
    for (int i = 0; i < size; i++) {
        delete this->pop[i];
    }

    delete[] this->pop;
}

gameStatistics Population::next(bool save) {
    Chromosome **nxt = new Chromosome *[this->size];
    int count = 0;
    int tourn_size = previous_power_2(this->size);

    gameStatistics tourn_stats = {.totalCollisions = 0,
                                  .totalGoals = 0,
                                  .scoreMean = 0,
                                  .goalsMean = 0,
                                  .collisionsMean = 0};
    while (count < this->size) {
        if (likelyness(1 - CROSSOVER_PROBABILITY)) {
            nxt[count] = cloneChromosome(this->pop[rand() % this->size]);

        } else {
            auto outcome = this->tournament(tourn_size, save);
            nxt[count] =
                crossover(*std::get<0>(outcome), *std::get<1>(outcome));
            auto tournResult = std::get<2>(outcome);

            tourn_stats.totalCollisions += tournResult.totalCollisions;
            tourn_stats.totalGoals += tournResult.totalGoals;

            tourn_stats.collisionsMean =
                (double)(count) / count * tourn_stats.collisionsMean +
                tournResult.collisionsMean / (count + 1);
            tourn_stats.goalsMean =
                (double)(count) / count * tourn_stats.goalsMean +
                tournResult.goalsMean / (count + 1);
            tourn_stats.scoreMean =
                (double)(count) / count * tourn_stats.scoreMean +
                tournResult.scoreMean / (count + 1);
        }

        if (likelyness(MUTATION_PROBABILITY)) {
            nxt[count] = mutate(nxt[count]);
        }

        count++;
    }

    for (int i = 0; i < this->size; i++) {
        delete this->pop[i];
        this->pop[i] = nxt[i];
    }

    delete[] nxt;
    return tourn_stats;
}

std::tuple<Chromosome *, Chromosome *, gameStatistics>
Population::tournament(int tournament_size, bool save) {
    std::vector<Chromosome *> contestants(tournament_size);
    for (int i = 0; i < tournament_size; i++) {
        contestants[i] = this->pop[rand() % this->size];
    }

    gameStatistics gameStats = {.totalCollisions = 0,
                                .totalGoals = 0,
                                .scoreMean = 0,
                                .goalsMean = 0,
                                .collisionsMean = 0};
    unsigned int gameN = 1;

    while (tournament_size > 2) {
        int pool_size = tournament_size / 2;
        std::vector<Chromosome *> pool(pool_size);

        for (int i = 0; i < pool_size; i++) {
            auto matchResult =
                play_match(contestants[i * 2], contestants[i * 2 + 1], save);
            if (matchResult.score > 0) {
                pool[i] = contestants[i * 2];
            } else {
                pool[i] = contestants[i * 2 + 1];
            }

            // statistiques
            gameStats.totalCollisions += matchResult.collisions;
            gameStats.totalGoals += matchResult.goals;
            gameStats.collisionsMean =
                (double)(gameN - 1) / gameN * gameStats.collisionsMean +
                (double)matchResult.collisions / gameN;
            gameStats.goalsMean =
                (double)(gameN - 1) / gameN * gameStats.goalsMean +
                (double)matchResult.goals / gameN;
            gameStats.scoreMean =
                (double)(gameN - 1) / gameN * gameStats.scoreMean +
                matchResult.score / gameN;
            gameN += 1;
        }

        tournament_size = pool_size;
        contestants = pool;
    }

    return std::make_tuple(contestants[0], contestants[1], gameStats);
}

Chromosome **getChromosomeFromPopulations(Population **pop, unsigned int i) {
    int ich = 0;
    while (i >= pop[ich]->size) {
        i -= pop[ich]->size;
        ich += 1;
    }
    return &pop[ich]->pop[i];
};

void shufflePopulations(Population **pop, unsigned int numberOfPop) {
    int popTotale = 0;
    for (int i = 0; i < numberOfPop; i++) {
        popTotale += pop[i]->size;
    }
    for (int i = popTotale - 1; i >= 0; i--) {
        int swapIndice = rand() % (i + 1);
        auto pc1 = getChromosomeFromPopulations(pop, i);
        auto pc2 = getChromosomeFromPopulations(pop, swapIndice);
        auto tmp = *pc1;
        *pc1 = *pc2;
        *pc2 = tmp;
    }
}

Chromosome *cloneChromosome(Chromosome *original) {
    auto clone = new Chromosome();

    for (int i = 0; i < EQUIPE_SIZE; i++) {
        for (int j = 0; j < NETWORK_SIZE - 1; j++) {
            for (int k = 0; k < original->matrix[i][j]->ligne; k++) {
                for (int l = 0; l < original->matrix[i][j]->col; l++) {
                    clone->matrix[i][j]->set(k, l,
                                             original->matrix[i][j]->get(k, l));
                }
            }
        }
    }

    for (int i = 0; i < DIDIER_NETWORK_SIZE - 1; i++) {
        for (int k = 0; k < original->didier[i]->ligne; k++) {
            for (int l = 0; l < original->didier[i]->col; l++) {
                clone->didier[i]->set(k, l, original->didier[i]->get(k, l));
            }
        }
    }

    return clone;
}

Population *joinPopulation(Population **p, int n) {
    int c = 0;
    for (int i = 0; i < n; i++) {
        c += p[i]->size;
    }

    auto np = new Population(c);
    int k = 0;

    for (int i = 0; i < n; i++) {
        for (int j = 0; j < p[i]->size; j++) {
            delete np->pop[k];
            np->pop[k++] = cloneChromosome(p[i]->pop[j]);
        }
    }

    return np;
}

void Population::write(std::ofstream &file) {
    file.write((char *)&this->size, sizeof(this->size));

    for (int i = 0; i < this->size; i++) {
        this->pop[i]->write(file);
    }
}

Population *Population::read(std::ifstream &file) {
    int size;
    file.read((char *)&size, sizeof(size));

    Population *c = new Population(size);

    for (int i = 0; i < c->size; i++) {
        delete c->pop[i];
        c->pop[i] = Chromosome::read(file);
    }

    return c;
};
