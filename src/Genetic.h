#include <iostream>
#include <stdio.h>
#include <algorithm>
#include <fstream>
#include <float.h>
#include <string>
#include "menu.h"
#include "Time.h"
#include "Node.h"
#include <vector>

#ifndef GENETIC_H
#define GENETIC_H

class Genetic{

public:
	//Default constructor
	Genetic();
	void setSettingsGenetic(int a, int  b, double c, int d, int e, int f, int g, int h, int i);
	int GeneticMechanism(int a, int **TSPMatrix, vector<unsigned>& islandsBest);
    void saveToFileGenetic(string a, double b, int c, vector<unsigned>& islandsBest, int e, int f, double g, int h, int i, int j, int k, int l, int m);
    double getTime(Time czas, int odp);

#pragma once

private:
	//Member variables
	int optMin;
	int amountRandomNodes;
	vector < unsigned >bestRoute;
	vector < unsigned > currentRoute;
	int **matrix;
	int matrixSize;
	int timeGenetic;
	int iterations;
	double time;
	int populationSize;
	double mutationProb;
	int crossoverType;
	int selectionType;
	int mutationType;
	int elitismNumber;
	int memeticType;

	void launchIslands(int a, int **TSPMatrix, vector < vector<unsigned>>& best, vector<unsigned>& islandsBest);
	void islandExchange(vector <vector<unsigned>>&population, vector < vector<unsigned>> best, int islandId);
	void GeneticEngine(int a, int **TSPMatrix, int islandId, vector < vector<unsigned>>& best, vector<unsigned>& islandBest);
	int costXY(int a, int b);
	void generateInitialPopulation(vector <vector <unsigned>>& pop, vector <double>&fitnesses, Time onboardClock);
	void sortVector(vector <vector<unsigned>>&vect);
	void evaluatePopulation(vector <vector <unsigned>> popul, vector <double> &fitnesses);
	void overwritePopulation(vector <vector<unsigned>>&population, vector <vector<unsigned>>popul);
	void mutation(vector <unsigned>& ind);
	void memeticImprovement(vector <unsigned>& ind);
	void doCrossover(vector <unsigned> parent1, vector <unsigned> parent2, vector <unsigned>&offspring1, vector <unsigned>&offspring2);
	void doSelection(vector <unsigned>& parent1, vector <unsigned>& parent2, vector <vector <unsigned>> population, vector <double>& fitnesses);
	void displayBestRoute();
	void PartiallyMappedCO(vector <unsigned> parent1, vector <unsigned> parent2, vector <unsigned>& offspring1, vector <unsigned>& offspring2);
	void OrderCO(vector <unsigned> parent1, vector <unsigned> parent2, vector <unsigned>& offspring1, vector <unsigned>& offspring2);
	void CycleCO(vector <unsigned> parent1, vector <unsigned> parent2, vector <unsigned>& offspring1, vector <unsigned>& offspring2);
	void CycleCO2(vector <unsigned> parent1, vector <unsigned> parent2, vector <unsigned>& offspring1, vector <unsigned>& offspring2);
	void TwoPointCO(vector <unsigned> parent1, vector <unsigned> parent2, vector <unsigned>& offspring1, vector <unsigned>& offspring2);
	void SequentialCO(vector <unsigned> parent1, vector <unsigned> parent2, vector <unsigned>& offspring);
	void EnhancedSequentialCO(vector <unsigned> parent1, vector <unsigned> parent2, vector <unsigned>& offspring);
	vector <unsigned> tournamentSelection(vector <vector <unsigned>> pop);
    void copyMatrix(int **macierz);
	int rouletteWheelSelection(vector <double> fit);
	int rankSelection(vector <double> fitnesses);
    int getBestNeighborhoodInsert(int &bestI, int &bestJ, vector <unsigned> currentRoute);
	int getBestNeighborhoodReverse(int &bestI, int &bestJ, vector <unsigned> currentRoute);
	int getBestNeighborhoodSwap(int &bestI, int &bestJ, vector <unsigned> currentRoute);
	void reverseVector(int a, int b, vector <unsigned>& currentRoute);
	void insertVector(int a, int b, vector <unsigned>& currentRoute);
	void swapVector(int a, int b, vector <unsigned>& currentRoute);
    void calculateInsert(int i, int j, int &balance, vector <unsigned> currentRoute);
	void calculateReverse(int i, int j, int &balance, vector <unsigned> currentRoute);
	void calculateSwap(int i, int j, int &balance, vector <unsigned> currentRoute);
    int calculateCost(vector <unsigned> a);
    int getInitialReduction(vector <unsigned>& route);
	int getInitialGreedy(vector < unsigned >&route);
	int getInitialGreedyAndRandom(vector < unsigned >&route);
	int getInitialReductionAndRandom(vector < unsigned >&route);
    int reduceMatrix(int **matrix, int size);
    void suitableRowColToInf(int **matrix, int row, int col, int size);


};
#endif