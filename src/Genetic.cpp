#include <iostream>
#include <stdio.h>
#include <algorithm>
#include <fstream>
#include <float.h>
#include <limits.h>
#include <string>
#include "menu.h"
#include "Time.h"
#include "Node.h"
#include <vector>
#include "Genetic.h"
#include <ctime>
#include <random>
#include <thread>
#include <queue>
#include <future>
using namespace std;

std::mutex muA;
std::mutex muB;

Genetic::Genetic() {
}
void Genetic::setSettingsGenetic(int a, int  b, double c, int d, int e, int f, int g, int h, int i) {
	populationSize = a;
	amountRandomNodes = b; // amountRandomNodes wykorzystane w metodzie z LocalSearch.cpp
	mutationProb = c;
	crossoverType = d;
	selectionType = e;
	mutationType = f;
	elitismNumber = g;
	memeticType = h;
	timeGenetic = i;
}

int Genetic::costXY(int a, int b) {
	return matrix[a][b];
}

int Genetic::GeneticMechanism(int matrixSize, int **TSPMatrix, vector<unsigned>& islandIterations) {

	int islandsAmount = 4;
	int result = INT_MAX;
	vector < vector<unsigned>> best;
	vector <unsigned> offspring1(matrixSize + 2, 0);
	for (int i = 0; i < islandsAmount; i++) {
		best.push_back(offspring1);
		islandIterations.push_back(0);
	}
	
	launchIslands(matrixSize, TSPMatrix, best, islandIterations); // w przypadku wyspowego setSettingsGenetic w mainie zbedne
	//GeneticEngine(a, TSPMatrix, 0, best, islandIterations); 

	for (int i = 0; i < islandsAmount; i++) {
		if (best.at(i).at(matrixSize + 1) < result)
			result=best.at(i).at(matrixSize  + 1);
	}

	return result;
}

void Genetic::launchIslands(int matrixSize, int **TSPMatrix, vector < vector<unsigned>>& best, vector<unsigned>& islandIterations) {
	Genetic island1;
	island1.setSettingsGenetic(150, 2, 0.15, 7, 1, 1, 5, 1, timeGenetic);
	auto a1 = std::async(std::launch::async, &Genetic::GeneticEngine, island1, matrixSize, TSPMatrix, 0, std::ref(best), std::ref(islandIterations));
	//populationSize[a], amountRandomNodes[b], mutationProb[d], crossoverType[h], selectionType[e], mutationType[f], elitismNumber[c], memeticType[g], timeGenetic[i])

	Genetic island2;
	island2.setSettingsGenetic(200, 2, 0.10, 2, 1, 1, 5, 1, timeGenetic);
	auto a2 = std::async(std::launch::async, &Genetic::GeneticEngine, island2, matrixSize, TSPMatrix, 1, std::ref(best), std::ref(islandIterations));

	Genetic island3;
	island3.setSettingsGenetic(150, 3, 0.10, 6, 1, 1, 10, 1, timeGenetic);
	auto a3 = std::async(std::launch::async, &Genetic::GeneticEngine, island3, matrixSize, TSPMatrix, 2, std::ref(best), std::ref(islandIterations));

	Genetic island4;
	island4.setSettingsGenetic(200, 4, 0.05, 6, 2, 1, 10, 1, timeGenetic);
	auto a4 = std::async(std::launch::async, &Genetic::GeneticEngine, island4, matrixSize, TSPMatrix, 3, std::ref(best), std::ref(islandIterations));

}

void Genetic::GeneticEngine(int size, int **TSPMatrix, int islandId, vector < vector<unsigned>>& best, vector<unsigned>& islandIterations) {

	//----wczytanie instancji
	matrixSize = size;
	matrixSize = size;
	matrix = new int *[matrixSize];
	for (int i = 0; i < matrixSize; i++) {
		matrix[i] = new int[matrixSize];
		for (int j = 0; j < matrixSize; j++)
			matrix[i][j] = TSPMatrix[i][j];
	}

	vector < vector <unsigned>> population;
	vector <unsigned> parent1, parent2, offspring1(matrixSize + 2, 0), offspring2(matrixSize + 2, 0);
	vector <double> fitnesses(populationSize, 0);
	bool stop = false;
	Time onboardClock;
	
	onboardClock.start();
	generateInitialPopulation(population, fitnesses, onboardClock);
	best.at(islandId) = population.at(0);
	onboardClock.stop();
	//-----sprawdzenie czy uplynal czas------
	if (onboardClock.read() > timeGenetic) {
		stop = true;
	}

	
	for (int j = 0; stop==false; j++) {
		vector < vector <unsigned>> newPopulation;

		while (newPopulation.size() != populationSize) {
			doSelection(parent1, parent2, population, fitnesses);
			doCrossover(parent1, parent2, offspring1, offspring2);
			mutation(offspring1);
			memeticImprovement(offspring1);
			newPopulation.push_back(offspring1);
			islandIterations.at(islandId) = j;

			//----crossovery o id 6 i 7 generuja tylko 1 potomka
			if (crossoverType != 6 && crossoverType != 7) {
				mutation(offspring2);
				memeticImprovement(offspring2);
				newPopulation.push_back(offspring2);
				islandIterations.at(islandId) = j;
			}
		}

		sortVector(newPopulation);

		 if (j != 0 && j%3==0) 
			islandExchange(population, best, islandId);

		overwritePopulation(population, newPopulation);
		sortVector(population);
		evaluatePopulation(population, fitnesses);	
		best.at(islandId)=population.at(0);

		onboardClock.stop();
		//-----sprawdzenie czy uplynal czas------
		if (onboardClock.read() > timeGenetic) {
			stop = true;
		}
	}

}

vector <unsigned> Genetic::tournamentSelection(vector <vector <unsigned>> pop) {
	vector <unsigned> best;
	vector <unsigned> ind;

	random_device randomSrc;
	default_random_engine randomGen(randomSrc());
	uniform_int_distribution<> indRand(0, pop.size() - 1);

	int k = 2;
	for (int i = 1; i <= k; i++) {
		if (i==1)
		best = pop.at(indRand(randomGen));

		else {
			ind = pop.at(indRand(randomGen));
			if (ind.at(matrixSize + 1) < best.at(matrixSize+1))
				best = ind;
		}
	}

	return best;
}

int Genetic::rouletteWheelSelection(vector <double> fitnesses) {

	double number, totalFitness = 0.0, offset = 0.0;
	int pick = 0;

	static_cast<double>(number = (double)rand() / (double)RAND_MAX);
	//cout << number << endl;

	for (int i = 0; i < fitnesses.size(); i++) {
		totalFitness += fitnesses.at(i);
	}
	for (int i = 0; i < fitnesses.size(); i++) {
		fitnesses.at(i) = fitnesses.at(i) / totalFitness;
	}

	for (int i = 0; i < fitnesses.size(); i++) {
		offset += fitnesses.at(i);
		if (number < offset) {
			pick = i;
			break;
		}
	}
	return pick;
}

int Genetic::rankSelection(vector <double> fitnesses) {

	double number, totalProb = 0.0, offset = 0.0;
	double helpRank = populationSize;
	int pick = 0;

	static_cast<double>(number = (double)rand() / (double)RAND_MAX);
	//cout << number << endl;

	for (int i = 0; i < fitnesses.size(); i++) {
		fitnesses.at(i) = helpRank / (populationSize*(populationSize - 1));
		helpRank = helpRank - 1;
	}

	for (int i = 0; i < fitnesses.size(); i++) {
		totalProb += fitnesses.at(i);
	}
	for (int i = 0; i < fitnesses.size(); i++) {
		fitnesses.at(i) = fitnesses.at(1) / totalProb;
	}

	for (int i = 0; i < fitnesses.size(); i++) {
		offset += fitnesses.at(i);
		if (number < offset) {
			pick = i;
			break;
		}
	}
	return pick;
}

void Genetic::overwritePopulation(vector <vector<unsigned>>&population, vector <vector<unsigned>>popul) {
	for (int i = elitismNumber; i < population.size(); i++)
		population.at(i) = popul.at(i - elitismNumber);
}

void Genetic::sortVector(vector <vector<unsigned>>&vect) {
	std::sort(vect.begin(), vect.end(),
		[&](const std::vector<unsigned>& a, const std::vector<unsigned>& b) {
		return a.at(matrixSize + 1) < b.at(matrixSize + 1);
	});
}

void Genetic::evaluatePopulation(vector <vector <unsigned>> popul, vector <double> &fitnesses) {
	double fitness, divider = 1;

	for (int i = 0; i < populationSize; i++) {
		static_cast<float>(fitness = divider / popul.at(i).at(matrixSize + 1));
		fitnesses.at(i) = fitness;
	}
}
void Genetic::mutation(vector <unsigned>& ind) {
	random_device randomSrc;
	default_random_engine randomGen(randomSrc());
	uniform_int_distribution<> nodeRand(1, matrixSize - 1);
	int bestI = 0;
	int bestJ = 0;
	int bestBalance = INT_MAX;


	int i, j, balance = 0;
	if (static_cast<float>(rand()) / RAND_MAX < mutationProb) {
		if (mutationType == 0) {
			for (int k = 0; k < 2; k++) { //po 2 losowania, wybierane lepsze
				do {
					i = nodeRand(randomGen);
					j = nodeRand(randomGen);
				} while (i == j || j < i);

				calculateSwap(i, j, balance, ind);
				if (balance < bestBalance) {
					bestI = i;
					bestJ = j;
					bestBalance = balance;
				}
			}
			swapVector(bestI, bestJ, ind);
		}

		else if (mutationType == 2) {
			for (int k = 0; k < 2; k++) {
				do {
					i = nodeRand(randomGen);
					j = nodeRand(randomGen);
				} while (i == j || j < i);

				calculateReverse(i, j, balance, ind);
				if (balance < bestBalance) {
					bestI = i;
					bestJ = j;
					bestBalance = balance;
				}
			}

			reverseVector(bestI, bestJ, ind);
		}


		else if (mutationType == 1) {
			for (int k = 0; k < 2; k++) {
				do {
					i = nodeRand(randomGen);
					j = nodeRand(randomGen);
				} while (i == j - 1 || i == j || i == j + 1);

				calculateInsert(i, j, balance, ind);
				if (balance < bestBalance) {
					bestI = i;
					bestJ = j;
					bestBalance = balance;
				}
			}
			insertVector(bestI, bestJ, ind);
		}

		ind.at(matrixSize+1) += bestBalance;
	}
}

void Genetic::islandExchange(vector <vector<unsigned>>&population, vector < vector<unsigned>> best, int islandId) {
	bool cont = true;
	for (int i = 0; i < best.size(); i++)
		if (best.at(i).at(matrixSize + 1) == 0)
			cont = false;

	for (int i = 0; i < best.size(); i++) {
		for (int j = 0; j < best.size(); j++) {
			if (i != j && cont == true) {
				int helper = 0;
				if (j > islandId) helper = 1;
				population.at(elitismNumber -best.size()+2+j-helper) = best.at(j);
			}
		}
	}
}

void Genetic::memeticImprovement(vector <unsigned>& ind) {
	int bestBalance, bestI = 0, bestJ = 0;
	//z/*
	if (memeticType == 0) {
		bestBalance = getBestNeighborhoodSwap(bestI, bestJ, ind);
		// if(bestBalance < 0) {
			swapVector(bestI, bestJ, ind);
			ind.at(matrixSize + 1) += bestBalance;
		//}
	}

	if (memeticType == 1) {
		bestBalance = getBestNeighborhoodInsert(bestI, bestJ, ind);
		// if(bestBalance < 0) {
			insertVector(bestI, bestJ	, ind);
			ind.at(matrixSize + 1) += bestBalance;
		//
	}

	if (memeticType == 2) {
		bestBalance = getBestNeighborhoodReverse(bestI, bestJ, ind);
		// if(bestBalance < 0) {
			reverseVector(bestI, bestJ, ind);
			ind.at(matrixSize + 1) += bestBalance;
		//
	}
}

void Genetic::generateInitialPopulation(vector <vector <unsigned>>& pop, vector <double>& fitnesses, Time onboardClock) {
	//dla instancji wiekszych od 100 lepiej 1 greedy, (n-1) greedyRandom, bez czasochlonnego reduction!
	/*
	for (int i = 2; i < populationSize / 2 + 1; i++) {
		vector < unsigned > route;
		route.push_back(getInitialReductionAndRandom(route)); // (n-2)/2 osobnikow losowo redukcyjnym
		pop.push_back(route);
		onboardClock.stop();
		if (onboardClock.read() > timeGenetic) {
			return;
		}
	}
	*/
	//for (int i = populationSize / 2 + 1; i < populationSize; i++) {
	for (int i = 1; i < populationSize; i++) {
		vector < unsigned > route;
		route.push_back(getInitialGreedyAndRandom(route)); // (n-2)/2 osobnikow losowo zachlannym
		pop.push_back(route);

		onboardClock.stop();
		if (onboardClock.read() > timeGenetic) {
			return;
		}
	}

	vector < unsigned > route;
	//route.push_back(getInitialReduction(route)); // 1 osobnik redukcyjnym
	//pop.push_back(route);

	onboardClock.stop();
	if (onboardClock.read() > timeGenetic) {
		return;
	}

	route.clear();
	route.push_back(getInitialGreedy(route)); // 1 osobnik zachlannym
	pop.push_back(route);

	onboardClock.stop();
	if (onboardClock.read() > timeGenetic) {
		return;
	}
	
	sortVector(pop);

	//--------------Wygenerowanie wartosci funkcji zdatnoci (fitness function)-----------------
	evaluatePopulation(pop, fitnesses);
}

void Genetic::doCrossover(vector <unsigned> parent1, vector <unsigned> parent2, vector <unsigned>&offspring1, vector <unsigned>&offspring2){
	switch (crossoverType) {
		case 1: 
			PartiallyMappedCO(parent1, parent2, offspring1, offspring2); // jest gituwa
			break;

		case 2: 
			OrderCO(parent1, parent2, offspring1, offspring2); // jest gituwa
			break;

		case 3: 
			CycleCO(parent1, parent2, offspring1, offspring2); //jest gituwa
			break;

		case 4: 
			CycleCO2(parent1, parent2, offspring1, offspring2); //jest gituwa
			break;

		case 5:
			TwoPointCO(parent1, parent2, offspring1, offspring2); //jest gituwa
			break;

		case 6:
			SequentialCO(parent1, parent2, offspring1); //jest gituwa
			break;

		case 7:
			EnhancedSequentialCO(parent1, parent2, offspring1); //jest gituwa
			break;
	}
}


void Genetic::doSelection(vector <unsigned>& parent1, vector <unsigned>& parent2, vector <vector <unsigned>> population, vector <double>& fitnesses) {
	switch (selectionType) {
	case 1:
		parent1 = tournamentSelection(population);
		parent2 = tournamentSelection(population);
		break;

	case 2:
		parent1 = population.at(rankSelection(fitnesses));
		parent2 = population.at(rankSelection(fitnesses));
		break;

	case 3:
		parent1 = population.at(rouletteWheelSelection(fitnesses));
		parent2 = population.at(rouletteWheelSelection(fitnesses));
		break;
	}


}

void Genetic::displayBestRoute() {
	for (int i = 0; i < bestRoute.size(); i++)
		cout << bestRoute.at(i) << "-";
	cout << endl;

}

void Genetic::PartiallyMappedCO(vector <unsigned> parent1, vector <unsigned> parent2, vector <unsigned>& offspring1, vector <unsigned>& offspring2) {
	random_device randomSrc;
	default_random_engine randomGen(randomSrc());
	uniform_int_distribution<> nodeRand(1, matrixSize - 1);

	vector <unsigned> visitedOffspring1(matrixSize, 0);
	vector <unsigned> visitedOffspring2(matrixSize, 0);

	int a, b, balance = 0;

	do {
		a = nodeRand(randomGen);
		b = nodeRand(randomGen);
	} while (a == b || a > b);

	//cout << "A: " << a << endl;
	//cout << "B: " << b << endl;


	for (int i = a; i < b; i++) {
		offspring1.at(i)=parent2.at(i);
		offspring2.at(i)=parent1.at(i);
		visitedOffspring1.at(parent2.at(i)) = 1;
		visitedOffspring2.at(parent1.at(i)) = 1;
	}

	for (int i = a-1; i >=0; i--) {
		if (visitedOffspring1.at(parent1.at(i)) != 1) {
			offspring1.at(i)=parent1.at(i);
			if (parent1.at(i) != 0)
				visitedOffspring1.at(parent1.at(i)) = 1;
		}
		else
			offspring1.at(i) = 1000;

		if (visitedOffspring2.at(parent2.at(i)) != 1) {
			offspring2.at(i) = parent2.at(i);
			if (parent2.at(i) != 0)
				visitedOffspring2.at(parent2.at(i)) = 1;
		}
		else
			offspring2.at(i) = 1000;
	}


	for (int i = b; i <= matrixSize; i++) {
		if (visitedOffspring1.at(parent1.at(i)) != 1) {
			offspring1.at(i)=parent1.at(i);
			visitedOffspring1.at(parent1.at(i)) = 1;
		}
		else
			offspring1.at(i)=1000;

		if (visitedOffspring2.at(parent2.at(i))!= 1) {
			offspring2.at(i) = parent2.at(i);
			visitedOffspring2.at(parent2.at(i)) = 1;
		}
		else
			offspring2.at(i) = 1000;
	}

	int help = 0;
	bool cont = true;

	for (int i = 0; i < matrixSize; i++) {
		if (offspring1.at(i) == 1000) {
			help = i;
			while (cont == true) {
				for (int j=0; j < matrixSize; j++) {
					if (parent2.at(j) == parent1.at(help)) {
						if (visitedOffspring1.at(parent1.at(j)) != 1) {
							offspring1.at(i) = parent1.at(j);
							cont = false;
							break;
						}
						else {
							help = j;
						}
					}
				}
			}
			cont = true;
		}

		if (offspring2.at(i) == 1000) {
			help = i;
			while (cont == true) {
				for (int j = 0; j < matrixSize; j++) {
					if (parent1.at(j) == parent2.at(help)) {
						if (visitedOffspring2.at(parent2.at(j)) != 1) {
							offspring2.at(i) = parent2.at(j);
							cont = false;
							break;
						}
						else {
							help = j;
						}
					}
				}
			}
			cont = true;
		}

	}
	offspring1.at(matrixSize+1)=calculateCost(offspring1);
	offspring2.at(matrixSize+1)=calculateCost(offspring2);


}

void Genetic::OrderCO(vector <unsigned> parent1, vector <unsigned> parent2, vector <unsigned>& offspring1, vector <unsigned>& offspring2) {
	random_device randomSrc;
	default_random_engine randomGen(randomSrc());
	uniform_int_distribution<> nodeRand(1, matrixSize - 1);

	vector <unsigned> visitedOffspring1(matrixSize, 0);
	vector <unsigned> visitedOffspring2(matrixSize, 0);

	int a, b, balance = 0;


	do {
		a = nodeRand(randomGen);
		b = nodeRand(randomGen);
	} while (a == b || a > b);

	//cout << "A: " << a << endl;
	//cout << "B: " << b << endl;


	for (int i = a; i < b; i++) {
		offspring1.at(i)=parent1.at(i);
		offspring2.at(i) = parent2.at(i);
		visitedOffspring1.at(parent1.at(i)) = 1;
		visitedOffspring2.at(parent2.at(i)) = 1;
	}


	int omitted = 0, omitted2 = 0;

	for (int i = b; i < matrixSize; i++) {
		if (visitedOffspring1.at(parent2.at(i)) != 1) {
			offspring1.at(i- omitted) = parent2.at(i);
			visitedOffspring1.at(parent2.at(i)) = 1;
		}
		else
			omitted++;

		if (visitedOffspring2.at(parent1.at(i)) != 1) {
			offspring2.at(i - omitted2) = parent1.at(i);
			visitedOffspring2.at(parent1.at(i)) = 1;
		}
		else
			omitted2++;

	}

	int helpOmitted1 = omitted, helpOmitted2 = omitted2;

	for (int i = 1; i < b; i++) {

		if (visitedOffspring1.at(parent2.at(i)) != 1) {
			if (omitted != 0) {
				offspring1.at(matrixSize-omitted)=parent2.at(i);
				visitedOffspring1.at(parent2.at(i)) = 1;
				omitted--;
			}

			else {
				offspring1.at(i- helpOmitted1) = parent2.at(i);
				visitedOffspring1.at(parent2.at(i)) = 1;
			}
		}

		else {
			helpOmitted1++;
		}


		if (visitedOffspring2.at(parent1.at(i)) != 1) {
			if (omitted2 != 0) {
				offspring2.at(matrixSize - omitted2) = parent1.at(i);
				visitedOffspring2.at(parent1.at(i)) = 1;
				omitted2--;
			}

			else {
				offspring2.at(i - helpOmitted2) = parent1.at(i);
				visitedOffspring2.at(parent1.at(i)) = 1;
			}
		}

		else {
			helpOmitted2++;
		}
		
	}

	offspring1.at(matrixSize+1)=calculateCost(offspring1);
	offspring2.at(matrixSize + 1) = calculateCost(offspring2);

}


void Genetic::CycleCO(vector <unsigned> parent1, vector <unsigned> parent2, vector <unsigned>& offspring1, vector <unsigned>& offspring2) {

	int omitted = 0, inserted = 0;
	vector <unsigned> visitedOffspring1(matrixSize, 0);
	vector <unsigned> visitedOffspring2(matrixSize, 0);
	vector <unsigned> indexParent1(matrixSize, 0);
	vector <unsigned> indexParent2(matrixSize, 0);

	for (int i = 0; i < matrixSize; i++) {
		indexParent1.at(parent1.at(i)) = i;
		indexParent2.at(parent2.at(i)) = i;
	}

	for (int i = 0; i < offspring1.size(); i++) {
		offspring1.at(i) = 0;
		offspring2.at(i) = 0;
	}


	offspring1.at(1) = parent1.at(1);
	offspring2.at(1) = parent2.at(1);
	visitedOffspring1.at(parent1.at(1)) = 1;
	visitedOffspring2.at(parent2.at(1)) = 1;

	int oldIndex1 = 1, oldIndex2 = 1;

	for (int i = 1; i < matrixSize; i++) {
		if (visitedOffspring1.at(parent2.at(oldIndex1)) != 1) {
			offspring1.at(indexParent1.at(parent2.at(oldIndex1))) = parent2.at(oldIndex1);
			visitedOffspring1.at(parent2.at(oldIndex1)) = 1;
			oldIndex1 = indexParent1.at(parent2.at(oldIndex1));
		}

		if (visitedOffspring2.at(parent1.at(oldIndex2)) != 1) {
			offspring2.at(indexParent2.at(parent1.at(oldIndex2))) = parent1.at(oldIndex2);
			visitedOffspring2.at(parent1.at(oldIndex2)) = 1;
			oldIndex2 = indexParent2.at(parent1.at(oldIndex2));
		}

	}


	for (int i = 1; i < matrixSize; i++) {
		if (offspring1.at(i) == 0) {
			offspring1.at(i) = parent2.at(i);
		}

		if (offspring2.at(i) == 0) {
			offspring2.at(i) = parent1.at(i);
		}
	}

	offspring1.at(matrixSize + 1) = calculateCost(offspring1);
	offspring2.at(matrixSize + 1) = calculateCost(offspring2);

}

void Genetic::CycleCO2(vector <unsigned> parent1, vector <unsigned> parent2, vector <unsigned>& offspring1, vector <unsigned>& offspring2) {

	int omitted = 0, inserted = 0;
	vector <unsigned> visitedOffspring1(matrixSize, 0);
	vector <unsigned> visitedOffspring2(matrixSize, 0);
	vector <unsigned> indexParent1(matrixSize, 0);
	vector <unsigned> indexParent2(matrixSize, 0);

	for (int i = 0; i < matrixSize; i++) {
		indexParent1.at(parent1.at(i)) = i;
		indexParent2.at(parent2.at(i)) = i;
	}

	offspring1.at(1) = parent2.at(1);
	visitedOffspring1.at(parent2.at(1)) = 1;

	int oldIndex1 = 1, oldIndex2 = 1;
	for (int i = 1; i < matrixSize; i++) {
		if (visitedOffspring2.at(parent2.at(indexParent1.at(parent2.at(indexParent1.at(offspring1.at(i)))))) != 1) {
			offspring2.at(i) = parent2.at(indexParent1.at(parent2.at(indexParent1.at(offspring1.at(i)))));
			visitedOffspring2.at(parent2.at(indexParent1.at(parent2.at(indexParent1.at(offspring1.at(i)))))) = 1;
		}

		if (i < matrixSize - 1) {
			if (visitedOffspring1.at(parent2.at(indexParent1.at(offspring2.at(i)))) != 1) {
				offspring1.at(i + 1) = parent2.at(indexParent1.at(offspring2.at(i)));
				visitedOffspring1.at(parent2.at(indexParent1.at(offspring2.at(i)))) = 1;
			}
			else {
				for (int j = 1; j < matrixSize; j++) {
					if (visitedOffspring1.at(parent2.at(j)) != 1) {
						if (visitedOffspring1.at(parent2.at(j))!= 1) {
							offspring1.at(i + 1) = parent2.at(j);
							visitedOffspring1.at(parent2.at(j)) = 1;
							break;
						}
					}
				}
			}
		}
	}

	offspring1.at(matrixSize + 1) = calculateCost(offspring1);
	offspring2.at(matrixSize + 1) = calculateCost(offspring2);

}

void Genetic::TwoPointCO(vector <unsigned> parent1, vector <unsigned> parent2, vector <unsigned>& offspring1, vector <unsigned>& offspring2) {
	random_device randomSrc;
	default_random_engine randomGen(randomSrc());
	uniform_int_distribution<> nodeRand(1, matrixSize - 1);

	vector <unsigned> visitedOffspring1(matrixSize, 0);
	vector <unsigned> visitedOffspring2(matrixSize, 0);

	int a, b, balance = 0;

	do {
		a = nodeRand(randomGen);
		b = nodeRand(randomGen);
	} while (a == b || a > b);

	
	for (int i = 0; i < offspring1.size(); i++) {
		offspring1.at(i) = 0;
		offspring2.at(i) = 0;
	}


	//cout << "A: " << a << endl;
	//cout << "B: " << b << endl;

	for (int i = 1; i < a; i++) {
		offspring1.at(i) = parent1.at(i);
		offspring2.at(i) = parent2.at(i);
		visitedOffspring1.at(parent1.at(i)) = 1;
		visitedOffspring2.at(parent2.at(i)) = 1;
	}

	for (int i = b; i < matrixSize; i++) {
		offspring1.at(i) = parent1.at(i);
		offspring2.at(i) = parent2.at(i);
		visitedOffspring1.at(parent1.at(i)) = 1;
		visitedOffspring2.at(parent2.at(i)) = 1;
	}

	bool cont = true;
	int omitted1 = 0, omitted2 = 0;

	for (int i = a; i < b; i++) {
		if (visitedOffspring1.at(parent2.at(i)) != 1) {
			offspring1.at(i) = parent2.at(i);
			visitedOffspring1.at(parent2.at(i)) = 1;
		}

		if (visitedOffspring2.at(parent1.at(i)) != 1) {
			offspring2.at(i) = parent1.at(i);
			visitedOffspring2.at(parent1.at(i)) = 1;
		}
	}

	for (int i = a; i < b; i++) {

		if(offspring1.at(i)==0){
			for (int j = 1; j < matrixSize; j++) {
				if (visitedOffspring1.at(parent2.at(j)) != 1) {
					offspring1.at(i) = parent2.at(j);
					visitedOffspring1.at(parent2.at(j)) = 1;
					cont = false;
					break;
				}
			}

		}
		cont = true;

		if (offspring2.at(i) == 0) {
			for (int j = 1; j < matrixSize; j++) {
				if (visitedOffspring2.at(parent1.at(j)) != 1) {
					offspring2.at(i) = parent1.at(j);
					visitedOffspring2.at(parent1.at(j)) = 1;
					cont = false;
					break;
				}
			}

		}
	}

	offspring1.at(matrixSize + 1) = calculateCost(offspring1);
	offspring2.at(matrixSize + 1) = calculateCost(offspring2);



}

void Genetic::SequentialCO(vector <unsigned> parent1, vector <unsigned> parent2, vector <unsigned>& offspring) {

	int omitted = 0, inserted = 0;
	vector <unsigned> visitedOffspring(matrixSize, 0);
	vector <unsigned> indexParent1(matrixSize, 0);
	vector <unsigned> indexParent2(matrixSize, 0);

	for (int i = 0; i < matrixSize; i++) {
		indexParent1.at(parent1.at(i)) = i;
		indexParent2.at(parent2.at(i)) = i;
	}
	offspring.at(0) = 0;

	int node1, node2;
	if (costXY(0, parent1.at(1)) < costXY(0, parent2.at(1))) {
		offspring.at(1) = parent1.at(1);
		visitedOffspring.at(parent1.at(1)) = 1;
	}
	else {
		offspring.at(1) = parent2.at(1);
		visitedOffspring.at(parent2.at(1)) = 1;
	}

	for (int i = 2; i < matrixSize; i++) {
		if (indexParent1.at(offspring.at(i - 1)) + 1 <= matrixSize - 1 && visitedOffspring.at(parent1.at(indexParent1.at(offspring.at(i - 1)) + 1)) != 1) {
			node1 = parent1.at(indexParent1.at(offspring.at(i - 1)) + 1);
		}

		else {
			for (int j = 1; j < matrixSize; j++) 
				if (visitedOffspring.at(j) != 1) {
					node1 = j;
					break;
				}
		}
		if (indexParent2.at(offspring.at(i - 1)) + 1 <= matrixSize - 1 && visitedOffspring.at(parent2.at(indexParent2.at(offspring.at(i - 1)) + 1))!=1) {
			node2 = parent2.at(indexParent2.at(offspring.at(i - 1)) + 1);
		}
		else {
			for (int j = 1; j < matrixSize; j++)
				if (visitedOffspring.at(j) != 1) {
					node2 = j;
					break;
				}
		}
		//cout << node1 << endl;
		//cout << node2 << endl;


		if (costXY(offspring.at(i - 1), node1) < costXY(offspring.at(i - 1), node2)) {
			offspring.at(i) = node1;
			visitedOffspring.at(node1) = 1;
		}

		else {
			offspring.at(i) = node2;
			visitedOffspring.at(node2) = 1;
		}
	}

	offspring.at(matrixSize + 1) = calculateCost(offspring);
	

}

void Genetic::EnhancedSequentialCO(vector <unsigned> parent1, vector <unsigned> parent2, vector <unsigned>& offspring) {

	int omitted = 0, inserted = 0;
	vector <unsigned> visitedOffspring(matrixSize, 0);
	vector <unsigned> indexParent1(matrixSize, 0);
	vector <unsigned> indexParent2(matrixSize, 0);

	for (int i = 0; i < matrixSize; i++) {
		indexParent1.at(parent1.at(i)) = i;
		indexParent2.at(parent2.at(i)) = i;
	}

	offspring.at(0) = 0;

	int node1, node2, minimum1 = INT_MAX, minimum2 = INT_MAX;

	for (int i = 0; i < matrixSize; i++) {
		if (visitedOffspring.at(i) != 1 && costXY(parent1.at(1), i)<minimum1)
			minimum1 = costXY(parent1.at(1), i);

		if (visitedOffspring.at(i) != 1 && costXY(parent2.at(1), i) < minimum2)
			minimum2 = costXY(parent2.at(1), i);
	}

	if (costXY(0, parent1.at(1))+minimum1 < costXY(0, parent2.at(1))+minimum2) {
		offspring.at(1) = parent1.at(1);
		visitedOffspring.at(parent1.at(1)) = 1;
	}
	else {
		offspring.at(1) = parent2.at(1);
		visitedOffspring.at(parent2.at(1)) = 1;
	}

	for (int i = 2; i < matrixSize; i++) {
		if (indexParent1.at(offspring.at(i - 1)) + 1 <= matrixSize - 1 && visitedOffspring.at(parent1.at(indexParent1.at(offspring.at(i - 1)) + 1)) != 1) {
			node1 = parent1.at(indexParent1.at(offspring.at(i - 1)) + 1);
		}

		else {
			for (int j = 1; j < matrixSize; j++)
				if (visitedOffspring.at(j) != 1) {
					node1 = j;
					break;
				}
		}
		if (indexParent2.at(offspring.at(i - 1)) + 1 <= matrixSize - 1 && visitedOffspring.at(parent2.at(indexParent2.at(offspring.at(i - 1)) + 1)) != 1) {
			node2 = parent2.at(indexParent2.at(offspring.at(i - 1)) + 1);
		}
		else {
			for (int j = 1; j < matrixSize; j++)
				if (visitedOffspring.at(j) != 1) {
					node2 = j;
					break;
				}
		}

		minimum1 = INT_MAX, minimum2 = INT_MAX;

		for (int j = 0; j < matrixSize; j++) {
			if (visitedOffspring.at(j) != 1 && costXY(parent1.at(1), j) < minimum1)
				minimum1 = costXY(node1, j);

			if (visitedOffspring.at(j) != 1 && costXY(parent2.at(1), j) < minimum2)
				minimum2 = costXY(node2, j);
		}

		if (costXY(offspring.at(i - 1), node1)+minimum1 < costXY(offspring.at(i - 1), node2)+minimum2) {
			offspring.at(i) = node1;
			visitedOffspring.at(node1) = 1;
		}

		else {
			offspring.at(i) = node2;
			visitedOffspring.at(node2) = 1;
		}
	}

	offspring.at(matrixSize + 1) = calculateCost(offspring);

	//showVector(offspring);
}

int Genetic::getBestNeighborhoodInsert(int &bestI, int &bestJ, vector <unsigned> currentRoute) {
	int bestBalance = INT_MAX;
	int balance;
	bool ifTabu;
	bestI = 0;
	bestJ = 0;
	for (int i = 1; i < matrixSize - 1; i++) {
		for (int j = 1; j <= matrixSize; j++) {

			if (i != j - 1 && i != j && i != j + 1) {

				calculateInsert(i, j, balance, currentRoute);

				if (balance < bestBalance) {
					bestBalance = balance;
					bestI = i;
					bestJ = j;
				}
			}
		}
	}

	return bestBalance;
}

void Genetic::insertVector(int a, int b, vector <unsigned>& currentRoute) {
	currentRoute.insert(currentRoute.begin() + b, currentRoute.at(a));
	if (b > a)
		currentRoute.erase(currentRoute.begin() + a);
	else
		currentRoute.erase(currentRoute.begin() + a + 1);
}

void Genetic::calculateInsert(int i, int j, int &balance, vector <unsigned> currentRoute) {
	balance = 0 - matrix[currentRoute.at(i)][currentRoute.at(i + 1)];
	balance = balance - matrix[currentRoute.at(j - 1)][currentRoute.at(j)];
	balance = balance - matrix[currentRoute.at(i - 1)][currentRoute.at(i)];
	balance = balance + matrix[currentRoute.at(i - 1)][currentRoute.at(i + 1)];
	balance = balance + matrix[currentRoute.at(j - 1)][currentRoute.at(i)];
	balance = balance + matrix[currentRoute.at(i)][currentRoute.at(j)];

}

int Genetic::getBestNeighborhoodReverse(int &bestI, int &bestJ, vector <unsigned> currentRoute) {
	int bestBalance = INT_MAX;
	int balance;
	bool ifTabu;
	bestI = 0;
	bestJ = 0;

	for (int i = 1; i < matrixSize - 1; i++) {
		for (int j = i + 1; j < matrixSize; j++) {

			calculateReverse(i, j, balance,currentRoute);

			ifTabu = false;

			if (balance < bestBalance) {
				bestBalance = balance;
				bestI = i;
				bestJ = j;
			}

		}
	}

	return bestBalance;
}

void Genetic::reverseVector(int a, int b, vector <unsigned>& currentRoute) {
	reverse(currentRoute.begin() + a, currentRoute.begin() + b + 1);
}

void Genetic::calculateReverse(int i, int j, int &balance, vector <unsigned> currentRoute) {

	balance = 0 - matrix[currentRoute.at(i - 1)][currentRoute.at(i)] - matrix[currentRoute.at(j)][currentRoute.at(j + 1)];
	balance = balance + matrix[currentRoute.at(i - 1)][currentRoute.at(j)] + matrix[currentRoute.at(i)][currentRoute.at(j + 1)];

	for (int k = i; k < j; k++)
		balance = balance - matrix[currentRoute.at(k)][currentRoute.at(k + 1)] + matrix[currentRoute.at(k + 1)][currentRoute.at(k)];

}


int Genetic::getBestNeighborhoodSwap(int &bestI, int &bestJ, vector <unsigned> currentRoute) {
	int bestBalance = INT_MAX;
	int balance;
	bestI = 0;
	bestJ = 0;


	for (int i = 1; i < matrixSize - 1; i++) {
		for (int j = i + 1; j < matrixSize; j++) {

			calculateSwap(i, j, balance, currentRoute);
			if (balance < bestBalance) {
				bestBalance = balance;
				bestI = i;
				bestJ = j;
			}

		}
	}

	return bestBalance;
}

void Genetic::swapVector(int a, int b, vector <unsigned>& currentRoute) {
	unsigned buffer = currentRoute.at(b);
	currentRoute.at(b) = currentRoute.at(a);
	currentRoute.at(a) = buffer;
}

void Genetic::calculateSwap(int i, int j, int &balance, vector <unsigned> currentRoute) {
	if (i + 1 == j) {
		balance = 0 - matrix[currentRoute.at(i - 1)][currentRoute.at(i)];
		balance = balance - matrix[currentRoute.at(i)][currentRoute.at(j)];
		balance = balance - matrix[currentRoute.at(j)][currentRoute.at(j + 1)];
		balance = balance + matrix[currentRoute.at(i - 1)][currentRoute.at(j)];
		balance = balance + matrix[currentRoute.at(j)][currentRoute.at(i)];
		balance = balance + matrix[currentRoute.at(i)][currentRoute.at(j + 1)];
	}
	else {
		balance = 0 - matrix[currentRoute.at(i - 1)][currentRoute.at(i)];
		balance = balance - matrix[currentRoute.at(i)][currentRoute.at(i + 1)];
		balance = balance - matrix[currentRoute.at(j - 1)][currentRoute.at(j)];
		balance = balance - matrix[currentRoute.at(j)][currentRoute.at(j + 1)];
		balance = balance + matrix[currentRoute.at(i - 1)][currentRoute.at(j)];
		balance = balance + matrix[currentRoute.at(j)][currentRoute.at(i + 1)];
		balance = balance + matrix[currentRoute.at(j - 1)][currentRoute.at(i)];
		balance = balance + matrix[currentRoute.at(i)][currentRoute.at(j + 1)];
	}

}

int Genetic::calculateCost(vector <unsigned> a) {

	int sum = 0;
	int i, j;
	for (int iter = 0; iter < matrixSize; iter++) {
		i = a.at(iter);
		j = a.at(iter + 1);
		//cout << "I= " << i << " j = " << j << endl;
		sum += matrix[i][j];
		//cout << matrix[i][j] << " ";
	}
	return sum;
}

int Genetic::getInitialReduction(vector <unsigned>& route) {
	int localMin = 0;
	int **macierz = new int *[matrixSize]; //macierz do operacji
	int **mainMacierz = new int *[matrixSize]; //ta sama macierz, ale do odtwarzania
	for (int i = 0; i < matrixSize; i++) {
		macierz[i] = new int[matrixSize];
		mainMacierz[i] = new int[matrixSize];
	}
	copyMatrix(macierz);

	int bestMin = 0, tempMin = 0, savedBestCol = 0;
	int *routeTab = new int[matrixSize];
	int *visitedTab = new int[matrixSize];
	for (int i = 0; i < matrixSize; i++) {
		visitedTab[i] = 0;
		routeTab[i] = 0;
	}

	for (int i = 0; i < matrixSize - 1; i++) {
		bestMin = INT_MAX;
		if (i == 0) {
			for (int k = 0; k < matrixSize; k++)
				for (int l = 0; l < matrixSize; l++)
					mainMacierz[k][l] = macierz[k][l];
		}
		for (int j = 1; j < matrixSize; j++) {
			if (visitedTab[j] != -1) {
				suitableRowColToInf(macierz, routeTab[i], j, matrixSize); //ustawienie wartosci w odpowiednim wierszu i kolumnie na -1
				tempMin = reduceMatrix(macierz, matrixSize); //zredukowanie macierzy
				tempMin += mainMacierz[routeTab[i]][j]; //dodanie przejscia

				if (tempMin < bestMin) { //jako ze wszyscy synowie maja ten sam lower bound ojca to nie jest tu uwzgledniany
					bestMin = tempMin; //najlepszy lower bound (bez uwzglednienia lower bound ojca) jest zapisywany do bestMin
					savedBestCol = j; //id wierzcholka z bestMin jest zapisywane do savedBestCol
				}

				for (int k = 0; k < matrixSize; k++)
					for (int l = 0; l < matrixSize; l++)
						macierz[k][l] = mainMacierz[k][l];


			}
		}
		visitedTab[savedBestCol] = -1; //w tabeli wizyt oznaczamy wierzcholek o id savedBestCol na odwiedzony
		routeTab[i + 1] = savedBestCol; //w tabeli drogi komorka o nastepnym indeksie ma id wierzcholka


		localMin = bestMin + localMin; // ustalenie lower bound
		suitableRowColToInf(macierz, routeTab[i], savedBestCol, matrixSize);
		reduceMatrix(macierz, matrixSize);
		for (int k = 0; k < matrixSize; k++)
			for (int l = 0; l < matrixSize; l++)
				mainMacierz[k][l] = macierz[k][l];

	}
	for (int i = 0; i < matrixSize; i++)
		route.push_back(routeTab[i]);

	route.push_back(0);
	delete[]routeTab;
	delete[]visitedTab;

	for (int i = 0; i < matrixSize; i++) {
		delete[]macierz[i];
		delete[]mainMacierz[i];
	}
	delete[]macierz;
	delete[]mainMacierz;

	return localMin;
}

int Genetic::getInitialReductionAndRandom(vector < unsigned >&bestTab) {
	random_device randomSrc;
	default_random_engine randomGen(randomSrc());
	uniform_int_distribution<> nodeRand(1, matrixSize - 1);
	int localMin = 0;
	int randomNode;
	int **macierz = new int *[matrixSize]; //macierz do operacji
	int **mainMacierz = new int *[matrixSize]; //ta sama macierz, ale do odtwarzania
	for (int i = 0; i < matrixSize; i++) {
		macierz[i] = new int[matrixSize];
		mainMacierz[i] = new int[matrixSize];
	}
	copyMatrix(macierz);
	bool ifVisited;
	int bestMin = 0, tempMin = 0, savedBestCol = 0;
	int *visitedTab = new int[matrixSize];
	for (int i = 0; i < matrixSize; i++) {
		visitedTab[i] = 0;
	}
	bestTab.push_back(0);

	for (int i = 0; i < amountRandomNodes; i++) {
		ifVisited = false;
		while (ifVisited == false) {
			randomNode = nodeRand(randomGen);
			ifVisited = true;
			if (visitedTab[randomNode] == -1) {
				ifVisited = false;
			}
		}
		visitedTab[randomNode] = -1;
		bestTab.push_back(randomNode);
		localMin = localMin + macierz[bestTab.at(i)][randomNode];
		suitableRowColToInf(macierz, bestTab.at(i), randomNode, matrixSize);

	}

	for (int i = amountRandomNodes; i < matrixSize - 1; i++) {
		bestMin = INT_MAX;
		if (i == amountRandomNodes) {
			for (int k = 0; k < matrixSize; k++)
				for (int l = 0; l < matrixSize; l++)
					mainMacierz[k][l] = macierz[k][l];
		}
		for (int j = 1; j < matrixSize; j++) {
			if (visitedTab[j] != -1) {
				suitableRowColToInf(macierz, bestTab.at(i), j, matrixSize); //ustawienie wartosci w odpowiednim wierszu i kolumnie na -1
				tempMin = reduceMatrix(macierz, matrixSize); //zredukowanie macierzy
				tempMin += mainMacierz[bestTab.at(i)][j]; //dodanie przejscia

				if (tempMin < bestMin) { //jako ze wszyscy synowie maja ten sam lower bound ojca to nie jest tu uwzgledniany
					bestMin = tempMin; //najlepszy lower bound (bez uwzglednienia lower bound ojca) jest zapisywany do bestMin
					savedBestCol = j; //id wierzcholka z bestMin jest zapisywane do savedBestCol
				}

				for (int k = 0; k < matrixSize; k++)
					for (int l = 0; l < matrixSize; l++)
						macierz[k][l] = mainMacierz[k][l];


			}
		}
		visitedTab[savedBestCol] = -1; //w tabeli wizyt oznaczamy wierzcholek o id savedBestCol na odwiedzony

		bestTab.push_back(savedBestCol);

		localMin = bestMin + localMin; // ustalenie lower bound
		suitableRowColToInf(macierz, bestTab.at(i), savedBestCol, matrixSize);
		reduceMatrix(macierz, matrixSize);
		for (int k = 0; k < matrixSize; k++)
			for (int l = 0; l < matrixSize; l++)
				mainMacierz[k][l] = macierz[k][l];

	}

	bestTab.push_back(0);

	delete[]visitedTab;

	for (int i = 0; i < matrixSize; i++) {
		delete[]macierz[i];
		delete[]mainMacierz[i];
	}
	delete[]macierz;
	delete[]mainMacierz;

	return localMin;
}

int Genetic::getInitialGreedy(vector < unsigned >&bestTab) {
	int localMin = 0;
	int bestMin, tempBest = 0, oldTempBest = 0;
	int *visitedTab = new int[matrixSize];
	for (int i = 0; i < matrixSize; i++) {
		visitedTab[i] = 0;
	}
	int **macierz = new int *[matrixSize];
	for (int i = 0; i < matrixSize; i++) {
		macierz[i] = new int[matrixSize];
	}

	copyMatrix(macierz);
	bool ifVisited;
	for (int i = 0; i < matrixSize; i++) {
		bestMin = INT_MAX;
		oldTempBest = tempBest;
		for (int j = 0; j < matrixSize; j++) {
			ifVisited = true;
			if (j != oldTempBest) {
				for (int k = 0; k <= i; k++) {
					if (j == visitedTab[k]) {
						ifVisited = false;
					}
				}
				if (macierz[oldTempBest][j] < bestMin && ifVisited == true) {
					bestMin = macierz[oldTempBest][j];
					tempBest = j;
				}
			}
		}
		if (i < matrixSize - 1)
			localMin = localMin + bestMin;
		else
			localMin = localMin + macierz[oldTempBest][0];

		bestTab.push_back(oldTempBest);
		visitedTab[i] = tempBest;
	}
	bestTab.push_back(0);

	for (int i = 0; i < matrixSize; i++) {
		delete[]macierz[i];
	}
	delete[]macierz;
	delete[]visitedTab;

	return localMin;

}

int Genetic::getInitialGreedyAndRandom(vector < unsigned >&bestTab) {

	random_device randomSrc;
	default_random_engine randomGen(randomSrc());
	uniform_int_distribution<> nodeRand(0, matrixSize - 1);

	int bestMin, tempBest = 0, oldTempBest = 0;
	int localMin = 0;
	int *visitedTab = new int[matrixSize];
	for (int i = 0; i < matrixSize; i++) {
		visitedTab[i] = 0;
	}
	int **macierz = new int *[matrixSize];
	for (int i = 0; i < matrixSize; i++) {
		macierz[i] = new int[matrixSize];
	}

	copyMatrix(macierz);
	bool ifVisited;
	int randomNode;
	int remainingNodes = amountRandomNodes;

	for (int i = 0; i < matrixSize; i++) {
		bestMin = INT_MAX;
		oldTempBest = tempBest;
		if (remainingNodes != 0) {
			ifVisited = false;
			while (ifVisited == false) {
				randomNode = nodeRand(randomGen);
				ifVisited = true;
				for (int k = 0; k <= i; k++) {
					if (randomNode == visitedTab[k]) {
						ifVisited = false;
					}
				}
			}
			tempBest = randomNode;
			bestMin = macierz[oldTempBest][randomNode];
			remainingNodes--;

		}
		else {
			for (int j = 0; j < matrixSize; j++) {
				ifVisited = true;
				if (j != oldTempBest) {
					for (int k = 0; k <= i; k++) {
						if (j == visitedTab[k]) {
							ifVisited = false;
						}
					}
					if (macierz[oldTempBest][j] < bestMin && ifVisited == true) {
						bestMin = macierz[oldTempBest][j];
						tempBest = j;
					}
				}
			}
		}
		if (i < matrixSize - 1)
			localMin = localMin + bestMin;
		else
			localMin = localMin + macierz[oldTempBest][0];

		bestTab.push_back(oldTempBest);
		visitedTab[i] = tempBest;
	}
	bestTab.push_back(0);
	//cout << "Greedy min: " << helpMin << endl;
	for (int i = 0; i < matrixSize; i++) {
		delete[]macierz[i];
	}
	delete[]macierz;

	return localMin;
}

void Genetic::copyMatrix(int **macierz) {
	for (int i = 0; i < matrixSize; i++)
		for (int j = 0; j < matrixSize; j++)
			macierz[i][j] = matrix[i][j];
}

int Genetic::reduceMatrix(int **matrix, int size) {

	int *row, *col;
	int min = INT_MAX;
	bool done = false;
	row = new int[size];
	col = new int[size];

	for (int i = 0; i < size; i++) {
		row[i] = min;
		done = false;
		for (int j = 0; j < size; j++) {
			if (matrix[i][j] != -1 && matrix[i][j] < row[i]) {
				row[i] = matrix[i][j];
				done = true;
			}
			if ((j == (size - 1)) && !done) {
				row[i] = 0;
			}
		}
	}
	for (int i = 0; i < size; i++) {
		for (int j = 0; j < size; j++) {
			if (matrix[i][j] != -1) {
				matrix[i][j] = matrix[i][j] - row[i];
			}
		}
	}
	for (int i = 0; i < size; i++) {
		col[i] = min;
		done = false;
		for (int j = 0; j < size; j++) {
			if (matrix[j][i] != -1 && matrix[j][i] < col[i]) {
				col[i] = matrix[j][i];
				done = true;
			}
			if ((j == (size - 1)) && !done) {
				col[i] = 0;
			}
		}
	}

	for (int i = 0; i < size; i++) {
		for (int j = 0; j < size; j++) {
			if (matrix[j][i] != -1) {
				matrix[j][i] = matrix[j][i] - col[i];
			}
		}
	}
	min = 0;

	for (int i = 0; i < size; i++) {
		min = min + row[i] + col[i];
	}
	delete[] row;
	delete[] col;
	return min;
}

void Genetic::suitableRowColToInf(int **matrix, int row, int col, int size) {
	for (int i = 0; i < size; i++) {
		matrix[row][i] = -1;
		matrix[i][col] = -1;
	}
	matrix[col][0] = -1;
}

void Genetic::saveToFileGenetic(string a, double b, int c, vector<unsigned>& islandsBest, int e, int f, double g, int h, int i, int j, int k, int l, int m) {
	ofstream plik;
	plik.open("results/wynikiTestyGenetic.csv", std::ios_base::app);
	plik << a << ";" << b << ";" << "ms" << ";" << c << ";";
	for (int i = 0; i < islandsBest.size(); i++) {
		plik << islandsBest.at(i) << ";";
	}
	plik << e << ";" << f << ";" << g << ";" << h << ";" << i << ";" << j << ";" << k << ";" << l << ";" << m << ";";
	plik << endl;
	plik.close();
}

double Genetic::getTime(Time czas, int odp) {
	double czasSek;

	switch (odp) {
	case 1:
		czasSek = czas.czasWykonaniaMili();
		break;
	case 2:
		czasSek = czas.czasWykonania();
		break;
	case 3:
		czasSek = czas.czasWykonaniaNano();
		break;
	case 4:
		czasSek = czas.czasWykonaniaSek();
		break;
	}
	return czasSek;
}