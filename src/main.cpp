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
#include "cstring"
#include "Time.h"
#include "Genetic.h"
using namespace std;

void menu(string filename, int iterationsAmount, int iterationTime, bool exchange, int threadsAmount){
    srand(time(NULL));
	string instanceName;
	int answer = 0, matrixSize, result;
	int **TSPMatrix = 0;
	Node start;
	Time time;	

	start.loadInfoGiven(filename);
	matrixSize = start.getStartSize();
	instanceName = start.getInstanceName();
	TSPMatrix = new int *[matrixSize];
	
	for (int i = 0; i < matrixSize; i++)
		TSPMatrix[i] = new int[matrixSize];
	start.copyMatrix(TSPMatrix);

	for (int repeat = 0; repeat < iterationsAmount; repeat++) {
		Genetic gen;
		vector<unsigned> iterations;
		time.startCounting();
		
    	result = gen.GeneticMechanism(matrixSize, TSPMatrix, iterations, iterationTime, exchange, threadsAmount);
		time.stopCounting();
	   	gen.saveToFileGenetic(instanceName, gen.getTime(time, 1), result, iterations);
		cout << "Best = " << result << endl;
    }
			
	cout << "\nTests have been executed" << endl << endl;
}

int main(int, char*argv[]) {
	cout << "-----------------------------------------------------------" << endl;
	cout << "--------------ISLAND MODEL MEMETIC ALGORITHM---------------" << endl;
	cout << "-----------------------Radoslaw Lis------------------------" << endl;
	cout << "-----------------------------------------------------------\n" << endl;

	bool exchange = argv[4] && strcmp(argv[4], "true")==0;
	
	menu(argv[1], stoi(argv[2]), stoi(argv[3]), exchange, stoi(argv[5]));
}
