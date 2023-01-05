#pragma once
#include <string>
#include <vector>
#include <random>
#include <cmath>

struct Location
{
	std::string mName;
	double mLatitude = 0.0;
	double mLongitude = 0.0;
};

struct Population
{
	std::vector<std::vector<int>> mMembers;
};

std::vector<Location> ReadLocations(const std::string& fileName);
std::vector<std::string> SplitStr(const std::string& str);
Population GenerateRandomPopulation(int popSize, int numberOfLocations, std::mt19937& seed);
std::vector<int> GenerateRandomVector(std::mt19937& seed, int numberOfLocations);

std::vector<std::pair<double, double>> GetLatsAndLongs(std::vector<int>& randomizedTrip, std::vector<Location>& locations);
double CalculateHaversineDistance(double lon1, double lon2, double lat1, double lat2);
std::vector<double> CalculateDistance(std::vector<std::pair<double,double>>& latsAndLongs);
double CalculateFitness(std::vector<double>& distances);
std::vector<std::pair<int, double>> CalculatePopFitness(std::vector<std::vector<int>>& population, std::vector<Location>& locations);

bool SortBySecondItem(const std::pair<int, double>& a, const std::pair<int, double>& b);
bool SortByFirstItem(const std::pair<int, double>& a, const std::pair<int, double>& b);
std::vector<std::pair<int, double>> SortByFitness(std::vector<std::pair<int, double>>& vectorToSort);
std::vector<std::pair<int, double>> ReorderByIndex(std::vector<std::pair<int, double>>& originalVector);
std::vector<double> GenerateProbability(int popSize,  std::vector<std::pair<int, double>>& sortedPopulation);

int SelectParentIndex(const std::vector<double>& probabilites, double random);
std::pair<int, int> SelectParents(const std::vector<double>& probabilites, std::mt19937& random);
std::vector<std::pair<int,int>> GeneratePairs(const std::vector<double>& probabilities, std::mt19937& random);

std::vector<int> GetVector(Population& population, int desiredIndex, int numLocations);
std::vector<int> CreateChild(int crossoverIndex, std::vector<int> parent1, std::vector<int> parent2);
std::vector<int> GenerateCrossover(Population& population, std::pair<int, int>& parents, std::mt19937& random, int numLocations, double mutationChance);
Population CrossoverPopulation(Population& population, std::vector<std::pair<int,int>> pairs, std::mt19937& random, int numLocations, double mutationChance);
