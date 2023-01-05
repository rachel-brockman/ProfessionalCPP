#include "SrcMain.h"
#include <iostream>
#include <random>
#include "TSP.h"
#include <fstream>
#include <algorithm>


void ProcessCommandArgs(int argc, const char* argv[])
{
    std::string inputFile = argv[1];
    int popSize = std::stoi(argv[2]);
    int generations = std::stoi(argv[3]);
    double mutationChance = std::stoi(argv[4])/100.0;
    int seed = std::stoi(argv[5]);
    
    std::mt19937 randGen(seed);
    std::vector<Location> locations = ReadLocations(inputFile);
    
    Population initialPopulation = GenerateRandomPopulation(popSize, locations.size(), randGen);
    
    std::vector<std::pair<int, double>> popFitness = CalculatePopFitness(initialPopulation.mMembers, locations);
    
    std::vector<std::pair<int, double>> sortedByFitness = SortByFitness(popFitness);
    
    std::vector<double> probability = GenerateProbability(popSize, sortedByFitness);
    
    std::vector<std::pair<int,int>> pairs = GeneratePairs(probability, randGen);
    
    
    std::ofstream outputFile("log.txt");
    if (outputFile.is_open()){
        //Output initial population + fitness
        outputFile << "INITIAL POPULATION:" << std::endl;
        for(int i = 0; i < initialPopulation.mMembers.size(); i++){
            for(int j = 0; j < initialPopulation.mMembers[i].size(); j++){
                if(j < initialPopulation.mMembers[i].size()-1){
                    outputFile << initialPopulation.mMembers[i][j] << ",";
                }
                else{
                    outputFile << initialPopulation.mMembers[i][j];
                }
            }
            outputFile << std::endl;
        }
        outputFile << "FITNESS:" << std::endl;
        std::vector<std::pair<int, double>> sortedByIndex = ReorderByIndex(sortedByFitness);
        for(int i = 0; i < sortedByIndex.size(); i++){
            outputFile << sortedByIndex[i].first << ":" << sortedByIndex[i].second << std::endl;
        }
        
        Population newPopulation = CrossoverPopulation(initialPopulation, pairs, randGen, locations.size(), mutationChance);
        Population nextGen;
        for(int k = 1; k <= generations; k++){
            
            outputFile << "SELECTED PAIRS:" << std::endl;
            for(int i = 0; i < pairs.size(); i++){
                outputFile << "(" << pairs[i].first << "," << pairs[i].second << ")" << std::endl;
            }
            outputFile << "GENERATION: " << k << std::endl;
            for(int i = 0; i < newPopulation.mMembers.size(); i++){
                for(int j = 0; j < newPopulation.mMembers[i].size(); j++){
                    if(j < newPopulation.mMembers[i].size()-1){
                        outputFile << newPopulation.mMembers[i][j] << ",";
                    }
                    else{
                        outputFile << newPopulation.mMembers[i][j];
                    }
                }
                outputFile << std::endl;
            }
            popFitness = CalculatePopFitness(newPopulation.mMembers, locations);
            
            sortedByFitness = SortByFitness(popFitness);
            
            probability = GenerateProbability(popSize, sortedByFitness);
            
            pairs = GeneratePairs(probability, randGen);
            
            if(k != generations){
                nextGen = CrossoverPopulation(newPopulation, pairs, randGen, locations.size(), mutationChance);
                newPopulation = nextGen;
            }
            outputFile << "FITNESS:" << std::endl;
            sortedByIndex = ReorderByIndex(sortedByFitness);
            for(int i = 0; i < sortedByIndex.size(); i++){
                outputFile << sortedByIndex[i].first << ":" << sortedByIndex[i].second << std::endl;
            }
            if(k == generations){
                outputFile << "SOLUTION:" << std::endl;
                //Maybe revisit this??
                std::for_each(nextGen.mMembers[sortedByFitness[0].first].begin(), nextGen.mMembers[sortedByFitness[0].first].end(), [&outputFile, &locations](int i){
                    outputFile << locations[i].mName << std::endl;
                });
                outputFile << locations[0].mName << std::endl;
                outputFile << "DISTANCE: " << sortedByFitness[0].second << " miles";
            }
        }

    }
}
