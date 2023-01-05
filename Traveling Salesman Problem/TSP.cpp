#include "TSP.h"
#include <fstream>
#include <algorithm>
#include <cmath>
#include <iterator>
#include <numeric>
#include <iostream>

std::vector<Location> ReadLocations(const std::string& fileName){
    std::ifstream fileInput(fileName);
    std::vector<std::string> locationData;
    std::vector<Location> locations;
    Location newLocation;
    
    if(fileInput.is_open()){
        while(!fileInput.eof()){
            locationData.clear();
            std::string line;
            std::getline(fileInput, line);
            locationData = SplitStr(line);
            newLocation.mName = locationData[0];
            newLocation.mLatitude = std::stod(locationData[1]);
            newLocation.mLongitude = std::stod(locationData[2]);
            locations.emplace_back(newLocation);
        }
    }
    fileInput.close();
    
    return locations;
}

std::vector<std::string> SplitStr(const std::string& str){
    const char delim = ',';
    std::vector<std::string> retVal;
    
    size_t start = 0;
    size_t delimLoc = str.find_first_of(delim, start);
    while(delimLoc != std::string::npos){
        retVal.emplace_back(str.substr(start, delimLoc - start));
        start = delimLoc + 1;
        delimLoc = str.find_first_of(delim, start);
    }
    
    retVal.emplace_back(str.substr(start));
    return retVal;
}

Population GenerateRandomPopulation(int popSize, int numberOfLocations, std::mt19937& seed){
    Population initialPopulation;
    initialPopulation.mMembers.resize(popSize);
    std::generate(initialPopulation.mMembers.begin(), initialPopulation.mMembers.end(),[&seed, &numberOfLocations]{
        return GenerateRandomVector(seed, numberOfLocations);
    });
    return initialPopulation;
}

std::vector<int> GenerateRandomVector(std::mt19937& seed, int numberOfLocations){
    std::vector<int> randomVector(numberOfLocations);
    int i = 0;
    std::generate(randomVector.begin(), randomVector.end(), [&i]{
        return i++;
    });
    std::shuffle(randomVector.begin() + 1, randomVector.end(), seed);
    return randomVector;
}


std::vector<std::pair<double, double>> GetLatsAndLongs(std::vector<int>& randomizedTrip, std::vector<Location>& locations){
    std::vector<std::pair<double, double>> latsAndLongs(locations.size());
    int i = 0;
    std::generate(latsAndLongs.begin(), latsAndLongs.end(), [&i, &randomizedTrip, &locations]{
        std::pair<double, double> temp = std::make_pair(locations[randomizedTrip[i]].mLatitude, locations[randomizedTrip[i]].mLongitude);
        i++;
        return temp;
    });
    return latsAndLongs;
}

double CalculateHaversineDistance(double lon1, double lon2, double lat1, double lat2){
    double degToRad =  0.0174533;
    double dlon = (lon2 - lon1)*degToRad;
    double dlat = (lat2 - lat1)*degToRad;
    double a = (sin(dlat/2)*sin(dlat/2))+cos(lat1*degToRad)*cos(lat2*degToRad)*sin(dlon/2)*sin(dlon/2);
    double c = 2 * atan2(sqrt(a), sqrt(1-a));
    double distance = 3961 * c;
    return distance;
}

std::vector<double> CalculateDistance(std::vector<std::pair<double,double>>& latsAndLongs){
    std::vector<double> distances(latsAndLongs.size());
    int i = 0;
    double temp = 0;
    std::generate(distances.begin(), distances.end(), [&i, &latsAndLongs, &distances, &temp]{
        if(i<distances.size()-1){
            temp = CalculateHaversineDistance(latsAndLongs[i].second, latsAndLongs[i+1].second, latsAndLongs[i].first, latsAndLongs[i+1].first);
        }
        else{
            temp = CalculateHaversineDistance(latsAndLongs[0].second, latsAndLongs[i].second, latsAndLongs[0].first, latsAndLongs[i].first);
        }
        i++;
        return temp;
    });
    return distances;
}

double CalculateFitness(std::vector<double>& distances){
    return std::accumulate(distances.begin(), distances.end(), 0.0, [](const double& a, const double& b){
        return a+b;
    });
}

std::vector<std::pair<int, double>> CalculatePopFitness(std::vector<std::vector<int>>& population, std::vector<Location>& locations){
    std::vector<std::pair<int, double>> output(population.size());
    std::vector<std::vector<std::pair<double, double>>> latsAndLongs(population.size());
    std::vector<std::vector<double>> distances(population.size());
    int i = 0;
    std::generate(latsAndLongs.begin(), latsAndLongs.end(), [&population, &i, &locations]{
        std::vector<std::pair<double, double>> temp = GetLatsAndLongs(population[i], locations);
        i++;
        return temp;
    });
    i = 0;
    std::generate(distances.begin(), distances.end(), [&i, &latsAndLongs]{
        std::vector<double> temp = CalculateDistance(latsAndLongs[i]);
        i++;
        return temp;
    });
    i = 0;
    std::generate(output.begin(), output.end(), [&i, &distances]{
        double temp = CalculateFitness(distances[i]);
        std::pair<int, double> result = std::make_pair(i, temp);
        i++;
        return result;
    });
    return output;
}

bool SortBySecondItem(const std::pair<int, double>& a, const std::pair<int, double>& b){
    return (a.second < b.second);
}

bool SortByFirstItem(const std::pair<int, double>& a, const std::pair<int, double>& b){
    return (a.first < b.first);
}

std::vector<std::pair<int, double>> SortByFitness(std::vector<std::pair<int, double>>& vectorToSort){
    std::sort(vectorToSort.begin(), vectorToSort.end(), SortBySecondItem);
    return vectorToSort;
}

std::vector<std::pair<int, double>> ReorderByIndex(std::vector<std::pair<int, double>>& originalVector){
    std::vector<std::pair<int, double>> newVector;
    std::copy_n(originalVector.begin(), originalVector.size(), std::back_inserter(newVector));
    std::sort(newVector.begin(), newVector.end(), SortByFirstItem);
    return newVector;
}

std::vector<double> GenerateProbability(int popSize,  std::vector<std::pair<int, double>>& sortedPopulation){
    std::vector<double> probabilities(popSize);
    std::generate(probabilities.begin(), probabilities.end(), [&popSize]{
        return 1.0/popSize;
    });
    probabilities[sortedPopulation[0].first] *= 6;
    probabilities[sortedPopulation[1].first] *= 6;
    int i = -1;
    std::generate(probabilities.begin(), probabilities.end(), [&i, &probabilities, &sortedPopulation]{
        i++;
        if(i>=2 && i<=(probabilities.size()/2)-1){
            probabilities[sortedPopulation[i].first] *= 3;
        }
        return probabilities[i];
        
    });
    double sum = std::accumulate(probabilities.begin(), probabilities.end(), 0.0, [](const double& a, const double& b){
        return a+b;
    });
    std::vector<double> normalizedProbabilities;
    std::transform(probabilities.begin(), probabilities.end(), std::back_inserter(normalizedProbabilities), [&sum](const double& a){
        return a/sum;
    });
    return normalizedProbabilities;
}


int SelectParentIndex(const std::vector<double>& probabilites, double random){
    double sum = 0.0;
    int i = 0;
    for(i = 0; i<probabilites.size(); i++){
        sum += probabilites[i];
        if(sum>=random){
            return i;
        }
    }
    return i;
}

std::pair<int, int> SelectParents(const std::vector<double>& probabilites, std::mt19937& random){
    std::uniform_real_distribution<double> distribution(0,1);
    double random1 = distribution(random);
    double random2 = distribution(random);
    int parent1 = SelectParentIndex(probabilites, random1);
    int parent2 = SelectParentIndex(probabilites, random2);
    std::pair<int, int> parents = std::make_pair(parent1, parent2);
    return parents;
}

std::vector<std::pair<int,int>> GeneratePairs(const std::vector<double>& probabilities, std::mt19937& random){
    std::vector<std::pair<int, int>> parentsVector(probabilities.size());
    std::generate(parentsVector.begin(), parentsVector.end(), [&probabilities, &random]{
        return SelectParents(probabilities, random);
    });
    return parentsVector;
}

std::vector<int> GetVector(Population& population, int desiredIndex, int numLocations){
    std::vector<int> output(numLocations);
    int i = 0;
    std::generate(output.begin(), output.end(), [&population, &desiredIndex, &i]{
        int temp = population.mMembers[desiredIndex][i];
        i++;
        return temp;
    });
    return output;
}


std::vector<int> CreateChild(int crossoverIndex, std::vector<int> parent1, std::vector<int> parent2){
    std::vector<int> child;
    std::copy_n(parent1.begin(), crossoverIndex+1, std::back_inserter(child));
    std::copy_if(parent2.begin(), parent2.end(), std::back_inserter(child), [&child](int a){
        return std::find(child.begin(), child.end(), a) == child.end();
    });
    return child;
}

std::vector<int> GenerateCrossover(Population& population, std::pair<int, int>& parents, std::mt19937& random, int numLocations, double mutationChance){
    std::uniform_int_distribution<int> crossoverDistribution(1, numLocations-2);
    int crossoverIndex = crossoverDistribution(random);
    std::uniform_int_distribution<int> parentSelection(0,1);
    int selectedParent = parentSelection(random);
    std::vector<int> parentA = GetVector(population, parents.first, numLocations);
    std::vector<int> parentB = GetVector(population, parents.second, numLocations);
    std::vector<int> child;
    if(selectedParent == 0){
        child = CreateChild(crossoverIndex, parentB, parentA);
    }
    else{
        child = CreateChild(crossoverIndex, parentA, parentB);
    }
    std::uniform_real_distribution<double> mutationDistribution(0, 1);
    double mutationComparator = mutationDistribution(random);
    if(mutationComparator <= mutationChance){
        std::uniform_int_distribution<int> mutator(1, child.size()-1);
        int firstIndex = mutator(random);
        int secondIndex = mutator(random);
        std::swap(child[firstIndex], child[secondIndex]);
    }
    return child;
}

Population CrossoverPopulation(Population& population, std::vector<std::pair<int,int>> pairs, std::mt19937& random, int numLocations, double mutationChance){
    std::vector<std::vector<int>> newPopulationMembers(population.mMembers.size());
    Population newPopulation;
    int i = 0;
    std::generate(newPopulationMembers.begin(), newPopulationMembers.end(), [&population, &pairs, &random, &numLocations, &mutationChance, &i]{
        std::vector<int> temp = GenerateCrossover(population, pairs[i], random, numLocations, mutationChance);
        i++;
        return temp;
    });
    newPopulation.mMembers = newPopulationMembers;
    return newPopulation;
}
