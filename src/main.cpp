#include <iostream>
#include <stdio.h>
#include <algorithm>
#include <fstream>
#include <float.h>
#include <string>
#include <cstdio>
#include <ctime>
#include <cstdlib>
#include <vector>
#include <thread>
#include "Node.h"
#include "Time.h"
#include "Genetic.h"
using namespace std;


int main(int, char**) {
    cout << "Hello, world!\n";

    srand(time(NULL));
	string instanceName;
	int answer = 0;
	int matrixSize;
	int **TSPMatrix = 0;
	cout << "-----------------------------------------------------------" << endl;
	cout << "--------------Designing Effective Algorithms---------------" << endl;
	cout << "-----------------------Radoslaw Lis------------------------" << endl;
	cout << "-----------------------------------------------------------\n" << endl;

	Node start;

    string matrixes[1] = { "data65.txt"};
	int populationSize[1] = { 150};
	int amountRandomNodes[1] = { 3 };
	double mutationProb[1] = { 0.10};
	int selectionType[1] = { 1};// 1-TS, 2-RS, 3-RWS
	int mutationType[1] = { 1 };// 2-reverse, 1-insert, 0-swap
	int memeticType[1] = { 1 };// 2-reverse, 1-insert, 0-swap
	int crossoverType[1] = {6}; // 1-PMX, 2-OX, 3-CX, 4-CX2, 5-TPX, 6-SCX, 7-ESCX
	int elitismNumber[1] = { 10 };
	int timeGenetic[1] = {60}; //czasy
	Time time;	
	int result;


	for (int x = 0; x < sizeof(matrixes) / sizeof(*matrixes); x++) {
		Node start;
		start.loadInfoGiven(matrixes[x]);
		matrixSize = start.getStartSize();
		instanceName = start.getInstanceName();
		TSPMatrix = new int *[matrixSize];
		for (int i = 0; i < matrixSize; i++)
			TSPMatrix[i] = new int[matrixSize];
		start.copyMatrix(TSPMatrix);
		for (int a = 0; a < sizeof(populationSize) / sizeof(*populationSize); a++)
			for (int b = 0; b < sizeof(amountRandomNodes) / sizeof(*amountRandomNodes); b++)
				for (int c = 0; c < sizeof(elitismNumber) / sizeof(*elitismNumber); c++)
					for (int d = 0; d < sizeof(mutationProb) / sizeof(*mutationProb); d++)
						for (int e = 0; e < sizeof(selectionType) / sizeof(*selectionType); e++)
							for (int f = 0; f < sizeof(mutationType) / sizeof(*mutationType); f++)
								for (int g = 0; g < sizeof(memeticType) / sizeof(*memeticType); g++)
									for (int h = 0; h < sizeof(crossoverType) / sizeof(*crossoverType); h++)
										for (int i = 0; i < sizeof(timeGenetic) / sizeof(*timeGenetic); i++)
											for (int repeat = 0; repeat < 200; repeat++) {
												Genetic gen;
												gen.setSettingsGenetic(populationSize[a], amountRandomNodes[b], mutationProb[d], crossoverType[h], selectionType[e], mutationType[f], elitismNumber[c],memeticType[g],timeGenetic[i]);
												vector<unsigned> iterations;
												time.czasStart();
    											result = gen.GeneticMechanism(matrixSize, TSPMatrix, iterations);
												time.czasStop();
	    										gen.saveToFileGenetic(instanceName, gen.getTime(time, 1), result, iterations, populationSize[a], amountRandomNodes[b], mutationProb[d], crossoverType[h], selectionType[e], mutationType[f], elitismNumber[c], memeticType[g], timeGenetic[i]);
												cout << "Best = " << result << endl;
											}
    }
			

	cout << endl;
	cout << "Testy wykonane, wyniki zapisane do Output/wynikiTestyGenetic.csv" << endl;
	cout << endl;

}
