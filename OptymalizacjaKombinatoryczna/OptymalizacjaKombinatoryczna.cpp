#include <iostream>
#include <vector>
#include <algorithm>
#include <chrono>
#include <random>
#include <fstream>
#include <nlohmann/json.hpp>
using json = nlohmann::json;
using namespace std::chrono;
using namespace std;

void generateRandomInstance(vector<int>& weights, vector<int>& values, vector<vector<int>>& conflicts, int numItems) {
    srand(time(0));

    // Generate random values for weights and values
    for (int i = 0; i < numItems; ++i) {
        weights.push_back(rand() % 20 + 1); // Random weight between 1 and 20
        values.push_back(rand() % 100 + 1); // Random value between 1 and 100
    }

    // Generate random conflicts matrix
    conflicts.resize(numItems);
    for (int i = 0; i < numItems; ++i) {
        int numConflicts = rand() % (numItems / 4); // Random number of conflicts (up to quarter of the items)
        for (int j = 0; j < numConflicts; ++j) {
            int conflictIndex = rand() % numItems; // Random index for conflict
            if (conflictIndex != i) {
                conflicts[i].push_back(conflictIndex);
                conflicts[conflictIndex].push_back(i);
            }
        }
    }
}

bool sortByValuePerWeight(const std::vector<int>& weights, const std::vector<int>& values, int a, int b) {
    return (static_cast<double>(values[a]) / weights[a]) > (static_cast<double>(values[b]) / weights[b]);
}

int knapsackGreedy(int W, const std::vector<int>& weights, const std::vector<int>& values, const std::vector<std::vector<int>>& conflicts) {
    int n = weights.size();
    std::vector<bool> selected(n, false);
    int totalValue = 0;

    // Create a vector of indices to represent items
    std::vector<int> indices(n);
    for (int i = 0; i < n; ++i) {
        indices[i] = i;
    }

    // Sort indices by value-to-weight ratio in descending order
    std::sort(indices.begin(), indices.end(), [&](int a, int b) { return sortByValuePerWeight(weights, values, a, b); });

    for (int i : indices) {
        // Check for conflicts with already selected items
        bool conflict = false;
        for (int conflictIndex : conflicts[i]) {
            if (selected[conflictIndex]) {
                conflict = true;
                break;
            }
        }

        // If no conflict, add the item to the knapsack
        if (!conflict && W >= weights[i]) {
            selected[i] = true;
            totalValue += values[i];
            W -= weights[i];
        }
    }

    return totalValue;
}

int knapsackGreedyRandom(int W, const std::vector<int>& weights, const std::vector<int>& values, const std::vector<std::vector<int>>& conflicts) {
    int n = weights.size();
    std::vector<bool> selected(n, false);
    int totalValue = 0;

    // Create a vector of indices to represent items
    std::vector<int> indices(n);
    for (int i = 0; i < n; ++i) {
        indices[i] = i;
    }

    // Shuffle indices
    auto rng = std::default_random_engine{};
    std::shuffle(std::begin(indices), std::end(indices), rng);

    for (int i : indices) {
        // Check for conflicts with already selected items
        bool conflict = false;
        for (int conflictIndex : conflicts[i]) {
            if (selected[conflictIndex]) {
                conflict = true;
                break;
            }
        }

        // If no conflict, add the item to the knapsack
        if (!conflict && W >= weights[i]) {
            selected[i] = true;
            totalValue += values[i];
            W -= weights[i];
        }
    }

    return totalValue;
}

int knapsackDynamic(int W, const vector<int>& weights, const vector<int>& values, const vector<vector<int>>& conflicts) {
    int n = weights.size();
    vector<vector<int>> dp(n + 1, vector<int>(W + 1, 0));

    for (int i = 1; i <= n; ++i) {
        for (int w = 1; w <= W; ++w) {
            // Check if the current item conflicts with any previously selected items
            bool conflict = false;
            for (int prevItem : conflicts[i - 1]) {
                if (dp[prevItem][w] > 0) {
                    conflict = true;
                    break;
                }
            }

            if (!conflict && weights[i - 1] <= w) {
                dp[i][w] = max(values[i - 1] + dp[i - 1][w - weights[i - 1]], dp[i - 1][w]);
            }
            else {
                dp[i][w] = dp[i - 1][w];
            }
        }
    }

    return dp[n][W];
}

float average(std::vector<int> &v) {
    if (v.empty()) {
        return 0;
    }
    float sum = 0.0;
    for (int i = 0; i < v.size(); i++)
        sum += v[i];
    return sum/v.size();
}

void randomInstances()
{
    int W;
    vector<int> values, weights;
    vector<vector<int>> conflicts;

    vector<int> greedyRandomTimes, dynamicProgrammingTimes, greedyTimes;
    vector<int> PLTgreedyRandomTimes, PLTdynamicProgrammingTimes, PLTgreedyTimes;

    vector<int> numItems = { 100, 200, 300, 400, 500, 600, 700, 800, 900, 1000, 1200, 1400, 1600, 1800, 2000 };

    for (int n = 0; n < numItems.size(); n++)
    {
        greedyRandomTimes.clear(); dynamicProgrammingTimes.clear(); greedyTimes.clear();
        
        W = numItems[n] * 10;

        std::cout << "Number of instances: " << numItems[n] << endl;
        std::cout << "Knapsack Capacity: " << W << endl;
        

        for (int t = 0; t < 100; t++)
        {
            generateRandomInstance(weights, values, conflicts, numItems[n]);

            auto start = high_resolution_clock::now();
            knapsackGreedyRandom(W, weights, values, conflicts);
            auto stop = high_resolution_clock::now();
            auto duration = duration_cast<milliseconds>(stop - start);
            greedyRandomTimes.push_back(duration.count());

            start = high_resolution_clock::now();
            knapsackDynamic(W, weights, values, conflicts);
            stop = high_resolution_clock::now();
            duration = duration_cast<milliseconds>(stop - start);
            dynamicProgrammingTimes.push_back(duration.count());

            start = high_resolution_clock::now();
            knapsackGreedy(W, weights, values, conflicts);
            stop = high_resolution_clock::now();
            duration = duration_cast<milliseconds>(stop - start);
            greedyTimes.push_back(duration.count());

            values.clear(); weights.clear(); conflicts.clear();
        }
        std::cout << "Dynamic Programming average time (milliseconds): " << average(dynamicProgrammingTimes) << endl;
        PLTdynamicProgrammingTimes.push_back(average(dynamicProgrammingTimes));
        std::cout << "Greedy random algorithm average time (milliseconds): " << average(greedyRandomTimes) << endl;
        PLTgreedyRandomTimes.push_back(average(greedyRandomTimes));
        std::cout << "Greedy average time (milliseconds): " << average(greedyTimes) << endl << endl;
        PLTgreedyTimes.push_back(average(greedyTimes));
    }
    cout << endl << endl;
    for (int i = 0; i < PLTgreedyRandomTimes.size(); i++)
        cout << PLTgreedyRandomTimes[i] << endl;
    cout << endl << endl;
    for (int i = 0; i < PLTdynamicProgrammingTimes.size(); i++)
        cout << PLTdynamicProgrammingTimes[i] << endl;
    cout << endl << endl;
    for (int i = 0; i < PLTgreedyTimes.size(); i++)
        cout << PLTgreedyTimes[i] << endl;
}

void jsonInstances()
{
    std::ifstream file("data.json");
    json data;
    file >> data;
    int W = 50, res1, res2, res3;
    vector<int> values, weights, subconflict;
    vector<vector<int>> conflicts;

    for (const auto& entry : data) {

        conflicts.clear(); weights.clear(); values.clear();
        for (const auto& weight : entry["weights"]) {
            weights.push_back(weight);
        }

        for (const auto& value : entry["values"]) {
            values.push_back(value);
        }

        for (const auto& conflict : entry["conflicts"]) {
            for (const auto& element : conflict) {
                subconflict.push_back(element);
            }
            conflicts.push_back(subconflict);
            subconflict.clear();
        }

        cout << "Weights: ";
        for (int weight : weights)
        {
            cout << weight << " ";
        }
        cout << endl;

        cout << "Values: ";
        for (int value : values)
        {
            cout << value << " ";
        }
        cout << endl;

        cout << "Conflicts:" << endl;
        for (int i = 0; i < conflicts.size(); ++i)
        {
            cout << i + 1 << ": ";
            for (int conflict : conflicts[i])
            {
                cout << conflict<< " ";
            }
            cout << endl;
        }

        res1 = knapsackDynamic(W, weights, values, conflicts);

        res2 = knapsackGreedyRandom(W, weights, values, conflicts);

        res3 = knapsackGreedy(W, weights, values, conflicts);

        
        cout << "Dynamic programming best value: " << res1 << endl << "Greedy random best value: " << res2 << endl << "Greedy best value: " << res3 << endl << endl;
    }
}

int main() 
{
    while (true)
    {
        char ch;
        std::cout << "Would you like to:\n1. Get result from a random instances\n2. Get result from a JSON instances\n3. Quit the program\n";
        while(isspace(ch = getchar()));
        switch (ch)
        {
        case '1':
            randomInstances();
            std::cout << endl;
            break;
        case '2':
            jsonInstances();
            std::cout << endl;
            break;
        case '3':
            exit(1);
            break;
        default:
            std::cout << "Wrong key\n";
            break;
        }
        std::cout << endl;
    }
    return 0;
}
