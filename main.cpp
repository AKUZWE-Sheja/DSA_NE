#include <iostream>
#include <string>
#include <unordered_map>
#include <vector>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <limits>
#include <climits>
#include <iomanip>

using namespace std;

// Utility: Safe Integer Input (reused from previous code)
int readInt(const string &prompt, int minVal = INT_MIN, int maxVal = INT_MAX) {
    while (true) {
        cout << prompt;
        string line;
        if (!getline(cin, line)) {
            cin.clear();
            continue;
        }
        try {
            int val = stoi(line);
            if (val < minVal || val > maxVal) {
                cout << "Please enter a number";
                if (minVal != INT_MIN && maxVal != INT_MAX)
                    cout << " between " << minVal << " and " << maxVal;
                cout << ".\n";
                continue;
            }
            return val;
        }
        catch (...) {
            cout << "Invalid input. Please enter a valid number.\n";
        }
    }
}

// Utility: Safe String Input
string readString(const string &prompt) {
    string input;
    while (true) {
        cout << prompt;
        getline(cin, input);
        // Remove leading/trailing whitespace
        input.erase(input.begin(), find_if(input.begin(), input.end(), [](unsigned char c) { return !isspace(c); }));
        input.erase(find_if(input.rbegin(), input.rend(), [](unsigned char c) { return !isspace(c); }).base(), input.end());
        // Check for empty input or invalid characters (e.g., commas, as they interfere with CSV)
        if (input.empty()) {
            cout << "Input cannot be empty.\n";
        } else if (input.find(',') != string::npos) {
            cout << "Input cannot contain commas.\n";
        } else {
            return input;
        }
    }
}

// City Structure
struct City {
    int index;
    string name;
};

// CityGraph Class
class CityGraph {
private:
    unordered_map<int, City> cities; // index -> City
    vector<vector<bool>> roads;      // Adjacency matrix for roads
    vector<vector<double>> budgets;  // Adjacency matrix for budgets
    int nextIndex;                   // Auto-incrementing index for new cities

public:
    CityGraph() : nextIndex(8) {
        // Initialize with 7 Rwandan cities
        cities[1] = {1, "Kigali"};
        cities[2] = {2, "Huye"};
        cities[3] = {3, "Muhanga"};
        cities[4] = {4, "Musanze"};
        cities[5] = {5, "Nyagatare"};
        cities[6] = {6, "Rubavu"};
        cities[7] = {7, "Rusizi"};
        // Initialize adjacency matrices
        resizeMatrices();
        loadData();
    }

    ~CityGraph() {
        saveData();
    }

    // Resize adjacency matrices when adding cities
    void resizeMatrices() {
        int n = max(nextIndex, 8); // Ensure matrices cover all indices
        roads.resize(n, vector<bool>(n, false));
        budgets.resize(n, vector<double>(n, 0.0));
    }

    // Add new cities
    void addCities() {
        int numCities = readInt("Number of cities to add: ", 1);
        for (int i = 0; i < numCities; ++i) {
            cout << "\nAdding city " << (i + 1) << " of " << numCities << "\n";
            string name = readString("City name: ");
            // Check for duplicate name
            bool exists = false;
            for (const auto& kv : cities) {
                if (kv.second.name == name) {
                    cout << "Error: City named '" << name << "' already exists!\n";
                    exists = true;
                    --i; // Retry this iteration
                    break;
                }
            }
            if (!exists) {
                cities[nextIndex] = {nextIndex, name};
                cout << "Added city '" << name << "' with index " << nextIndex << "\n";
                nextIndex++;
                resizeMatrices();
                saveData();
            }
        }
    }

    // Add road between cities
    void addRoad() {
        int city1 = readInt("Enter first city index: ", 1, nextIndex - 1);
        int city2 = readInt("Enter second city index: ", 1, nextIndex - 1);
        if (cities.find(city1) == cities.end() || cities.find(city2) == cities.end()) {
            cout << "Error: One or both cities do not exist!\n";
            return;
        }
        if (city1 == city2) {
            cout << "Error: Cannot add a road from a city to itself!\n";
            return;
        }
        if (roads[city1][city2]) {
            cout << "Error: Road between " << cities[city1].name << " and " << cities[city2].name << " already exists!\n";
            return;
        }
        roads[city1][city2] = roads[city2][city1] = true;
        cout << "Added road between " << cities[city1].name << " (" << city1 << ") and "
             << cities[city2].name << " (" << city2 << ")\n";
        saveData();
    }

    // Add budget for a road
    void addBudget() {
        int city1 = readInt("Enter first city index: ", 1, nextIndex - 1);
        int city2 = readInt("Enter second city index: ", 1, nextIndex - 1);
        if (cities.find(city1) == cities.end() || cities.find(city2) == cities.end()) {
            cout << "Error: One or both cities do not exist!\n";
            return;
        }
        if (!roads[city1][city2]) {
            cout << "Error: No road exists between " << cities[city1].name << " and "
                 << cities[city2].name << "!\n";
            return;
        }
        double budget = readInt("Enter budget (billions of RWF): ", 0) / 1.0;
        budgets[city1][city2] = budgets[city2][city1] = budget;
        cout << "Assigned budget of " << budget << " billion RWF to road between "
             << cities[city1].name << " and " << cities[city2].name << "\n";
        saveData();
    }

    // Edit city name
    void editCity() {
        int index = readInt("Enter city index to edit: ", 1, nextIndex - 1);
        if (cities.find(index) == cities.end()) {
            cout << "Error: City with index " << index << " does not exist!\n";
            return;
        }
        string newName = readString("Enter new city name: ");
        // Check for duplicate name
        for (const auto& kv : cities) {
            if (kv.second.name == newName && kv.first != index) {
                cout << "Error: City named '" << newName << "' already exists!\n";
                return;
            }
        }
        string oldName = cities[index].name;
        cities[index].name = newName;
        cout << "Changed city " << index << " from '" << oldName << "' to '" << newName << "'\n";
        saveData();
    }

    // Search for a city by name
    void searchCity() {
        string name = readString("Enter city name to search: ");
        bool found = false;
        for (const auto& kv : cities) {
            if (kv.second.name == name) {
                cout << "Found: Index " << kv.first << ", Name: " << kv.second.name << "\n";
                found = true;
                break;
            }
        }
        if (!found) {
            cout << "Error: City named '" << name << "' not found!\n";
        }
    }

    // Display all cities
    void displayCities() {
        cout << "\n--- Cities ---\n";
        cout << "Index | City Name\n";
        cout << "------|----------\n";
        for (const auto& kv : cities) {
            cout << setw(5) << kv.first << " | " << kv.second.name << "\n";
        }
    }

    // Display all roads
    void displayRoads() {
        cout << "\n--- Roads ---\n";
        cout << "Road | Budget (billions RWF)\n";
        cout << "-----|----------------------\n";
        for (int i = 1; i < nextIndex; ++i) {
            for (int j = i + 1; j < nextIndex; ++j) {
                if (roads[i][j] && cities.find(i) != cities.end() && cities.find(j) != cities.end()) {
                    cout << cities[i].name << "-" << cities[j].name << " | "
                         << fixed << setprecision(2) << budgets[i][j] << "\n";
                }
            }
        }
        // Display road adjacency matrix
        cout << "\nRoad Adjacency Matrix (1 = road exists, 0 = no road):\n";
        cout << "   ";
        for (int i = 1; i < nextIndex; ++i) {
            if (cities.find(i) != cities.end()) {
                cout << setw(5) << i;
            }
        }
        cout << "\n";
        for (int i = 1; i < nextIndex; ++i) {
            if (cities.find(i) != cities.end()) {
                cout << setw(2) << i << ":";
                for (int j = 1; j < nextIndex; ++j) {
                    if (cities.find(j) != cities.end()) {
                        cout << setw(5) << roads[i][j];
                    }
                }
                cout << "\n";
            }
        }
    }

    // Display recorded data (adjacency matrices)
    void displayData() {
        cout << "\n--- Recorded Data ---\n";
        // Display cities
        displayCities();
        // Display road adjacency matrix
        cout << "\nRoad Adjacency Matrix (1 = road exists, 0 = no road):\n";
        cout << "   ";
        for (int i = 1; i < nextIndex; ++i) {
            if (cities.find(i) != cities.end()) {
                cout << setw(5) << i;
            }
        }
        cout << "\n";
        for (int i = 1; i < nextIndex; ++i) {
            if (cities.find(i) != cities.end()) {
                cout << setw(2) << i << ":";
                for (int j = 1; j < nextIndex; ++j) {
                    if (cities.find(j) != cities.end()) {
                        cout << setw(5) << roads[i][j];
                    }
                }
                cout << "\n";
            }
        }
        // Display budget adjacency matrix
        cout << "\nBudget Adjacency Matrix (billions RWF):\n";
        cout << "   ";
        for (int i = 1; i < nextIndex; ++i) {
            if (cities.find(i) != cities.end()) {
                cout << setw(8) << i;
            }
        }
        cout << "\n";
        for (int i = 1; i < nextIndex; ++i) {
            if (cities.find(i) != cities.end()) {
                cout << setw(2) << i << ":";
                for (int j = 1; j < nextIndex; ++j) {
                    if (cities.find(j) != cities.end()) {
                        cout << setw(8) << fixed << setprecision(2) << (roads[i][j] ? budgets[i][j] : 0.0);
                    }
                }
                cout << "\n";
            }
        }
    }

    // Save data to files
    void saveData() {
        // Save cities
        ofstream cityFile("cities.txt");
        cityFile << "index,city_name\n";
        for (const auto& kv : cities) {
            cityFile << kv.first << "," << kv.second.name << "\n";
        }
        cityFile.close();
        // Save roads
        ofstream roadFile("roads.txt");
        roadFile << "road,budget\n";
        for (int i = 1; i < nextIndex; ++i) {
            for (int j = i + 1; j < nextIndex; ++j) {
                if (roads[i][j] && cities.find(i) != cities.end() && cities.find(j) != cities.end()) {
                    roadFile << i << "-" << j << "," << fixed << setprecision(2) << budgets[i][j] << "\n";
                }
            }
        }
        roadFile.close();
    }

    // Load data from files
    void loadData() {
        // Load cities
        ifstream cityFile("cities.txt");
        if (cityFile) {
            string line;
            getline(cityFile, line); // Skip header
            while (getline(cityFile, line)) {
                stringstream ss(line);
                string indexStr, name;
                getline(ss, indexStr, ',');
                getline(ss, name, ',');
                try {
                    int index = stoi(indexStr);
                    cities[index] = {index, name};
                    nextIndex = max(nextIndex, index + 1);
                } catch (...) {
                    cout << "Error parsing city: " << line << "\n";
                }
            }
            cityFile.close();
        }
        resizeMatrices();
        // Load roads
        ifstream roadFile("roads.txt");
        if (roadFile) {
            string line;
            getline(roadFile, line); // Skip header
            while (getline(roadFile, line)) {
                stringstream ss(line);
                string road, budgetStr;
                getline(ss, road, ',');
                getline(ss, budgetStr, ',');
                // Parse road (format: index1-index2)
                size_t dash = road.find('-');
                if (dash == string::npos) continue;
                try {
                    int city1 = stoi(road.substr(0, dash));
                    int city2 = stoi(road.substr(dash + 1));
                    double budget = stod(budgetStr);
                    if (cities.find(city1) != cities.end() && cities.find(city2) != cities.end()) {
                        roads[city1][city2] = roads[city2][city1] = true;
                        budgets[city1][city2] = budgets[city2][city1] = budget;
                    }
                } catch (...) {
                    cout << "Error parsing road: " << line << "\n";
                }
            }
            roadFile.close();
        }
    }

    void displayHelp() {
        cout << "This system helps you manage secondary cities and their connecting roads in a Rwanda" << endl;
        cout << "\nFeatures Overview:" << endl;
        cout << "- District Management: Add, view, update, and delete district records" << endl;
        cout << "- Network Management: Create connections between districts with budgets" << endl;
        cout << "- Data Persistence: All data is saved to CSV files automatically" << endl;
        cout << "- Auto ID Generation: District IDs are generated automatically (H1, H2, H3...)" << endl;

        cout << "\nMenu Options Explained:" << endl;
        cout << "1. View districts: Display all registered districts and their details" << endl;
        cout << "2. Add Hospital: Register a new hospital (ID auto-generated)" << endl;
        cout << "3. Update Hospital: Modify existing hospital information" << endl;
        cout << "4. Delete Hospital: Remove a hospital and all its connections" << endl;
        cout << "5. Link districts: Create a connection between two districts with distance" << endl;  // ---------------- neeeds edit
        cout << "6. View Graph: Display the hospital network with all connections" << endl;
        cout << "7. Add Multiple: Add several districts at once" << endl;
        cout << "8. Help: Show this help information" << endl;
        cout << "9. Exit: Save all data and exit the program" << endl;

        cout << "\nTips for Usage:" << endl;
        cout << "- District IDs are auto-generated in format: H1, H2, H3, etc." << endl;
        cout << "- Budgets are positive numbers in billions" << endl;
        cout << "- Data is automatically saved after each operation" << endl;
        cout << "- You can press Enter to keep current values when updating" << endl;

        cout << "\nFile Information:" << endl;
        cout << "- districts.csv: Contains hospital data" << endl;
        cout << "- hospital_connections.csv: Contains connection data" << endl; // ---------needs update
        cout << "- Files are created automatically if they don't exist" << endl;
    }
};

// Main Menu
int main() {
    CityGraph graph;
    while (true) {
        cout << "\n=== City Connection System ===\n"
             << "1. Add new city(ies)\n"
             << "2. Add roads between cities\n"
             << "3. Add the budget for roads\n"
             << "4. Edit city\n"
             << "5. Search for a city using its name\n"
             << "6. Display cities\n"
             << "7. Display roads\n"
             << "8. Display recorded data on console\n"
             << "9. Help\n"
             << "10. Exit\n";
        int choice = readInt("Choose: ", 1, 9);

        if (choice == 10) break;

        switch (choice) {
            case 1:
                graph.addCities();
                break;
            case 2:
                graph.addRoad();
                break;
            case 3:
                graph.addBudget();
                break;
            case 4:
                graph.editCity();
                break;
            case 5:
                graph.searchCity();
                break;
            case 6:
                graph.displayCities();
                break;
            case 7:
                graph.displayRoads();
                break;
            case 8:
                graph.displayData();
                break;
            case 9:
                graph.displayHelp();
                break;
        }
    }
    return 0;
}