#include <iostream> // For printing to the screen and getting user input
#include <string> // For handling text like city names
#include <unordered_map> // A fancy dictionary to store our cities
#include <vector> // Lists that can grow, used for road connections
#include <fstream> // For saving and loading files
#include <sstream> // For splitting text in files
#include <algorithm> // For some text cleanup tricks
#include <limits> // To set limits on numbers
#include <climits> // More number limits
#include <iomanip> // For making our output look neat

using namespace std; // Makes it easier to use standard C++ stuff without extra typing

// Yo, this function makes sure we get a proper number from the user, like picking a menu option!
int readInt(const string &prompt, int minVal = INT_MIN, int maxVal = INT_MAX) {
    while (true) { // Keep asking until we get it right
        cout << prompt; // Show the question, like "Choose: "
        string line;
        if (!getline(cin, line)) { // Grab what the user types
            cin.clear(); // Clear any input errors
            continue; // Try again
        }
        try {
            int val = stoi(line); // Turn text into a number
            if (val < minVal || val > maxVal) { // Check if it's in the allowed range
                cout << "Please enter a number";
                if (minVal != INT_MIN && maxVal != INT_MAX)
                    cout << " between " << minVal << " and " << maxVal; // Be specific if there's a range
                cout << ".\n";
                continue; // Nope, try again!
            }
            return val; // Sweet, we got a good number!
        }
        catch (...) {
            cout << "Invalid input. Please enter a valid number.\n"; // No letters or weird stuff, please!
        }
    }
}

// This grabs text input, like a city name, and makes sure it’s not blank or messy
string readString(const string &prompt) {
    string input;
    while (true) {
        cout << prompt; // Ask for input, like "City name: "
        getline(cin, input); // Get the whole line they type
        // Clean up extra spaces at the start or end, like trimming a haircut
        input.erase(input.begin(), find_if(input.begin(), input.end(), [](unsigned char c) { return !isspace(c); }));
        input.erase(find_if(input.rbegin(), input.rend(), [](unsigned char c) { return !isspace(c); }).base(), input.end());
        if (input.empty()) { // Nothing typed? Not cool!
            cout << "Input cannot be empty.\n";
        } else if (input.find(',') != string::npos) { // Commas mess up our files
            cout << "Input cannot contain commas.\n";
        } else {
            return input; // Nice, we got a clean name!
        }
    }
}

// A simple way to store info about a city, like its ID and name
struct City {
    int index; // Unique number, like 1 for Kigali
    string name; // The city’s name, like "Huye"
};

// The big boss class that runs our Rwanda road show!
class CityGraph {
private:
    unordered_map<int, City> cities; // Our city list, like a phonebook: ID -> city info
    vector<vector<bool>> roads; // A grid to track which cities have roads (true/false)
    vector<vector<double>> budgets; // Another grid for road budgets in billions of RWF
    int nextIndex; // Keeps track of the next city ID, like a ticket number

public:
    // Setting up the app, like opening a new shop in Kigali
    CityGraph() : nextIndex(8) {
        // Start with 7 awesome Rwandan cities
        cities[1] = {1, "Kigali"};
        cities[2] = {2, "Huye"};
        cities[3] = {3, "Muhanga"};
        cities[4] = {4, "Musanze"};
        cities[5] = {5, "Nyagatare"};
        cities[6] = {6, "Rubavu"};
        cities[7] = {7, "Rusizi"};
        // Get our road and budget grids ready
        resizeMatrices();
        // Load any saved data, like picking up where we left off
        loadData();
    }

    // Clean up when we’re done, like locking the shop
    ~CityGraph() {
        saveData(); // Save everything before we go
    }

    // Make our road and budget grids big enough for new cities
    void resizeMatrices() {
        int n = max(nextIndex, 8); // At least 8 to handle our starting cities
        roads.resize(n, vector<bool>(n, false)); // No roads yet, all false
        budgets.resize(n, vector<double>(n, 0.0)); // No budgets yet, all 0
    }

    // Add new cities, like expanding Rwanda’s map!
    void addCities() {
        int numCities = readInt("Number of cities to add: ", 1); // How many cities we adding?
        for (int i = 0; i < numCities; ++i) {
            cout << "\nAdding city " << (i + 1) << " of " << numCities << "\n";
            string name = readString("City name: "); // Get the city name
            // Check if this name’s already taken
            bool exists = false;
            for (const auto& kv : cities) {
                if (kv.second.name == name) {
                    cout << "Error: City named '" << name << "' already exists!\n"; // No duplicates!
                    exists = true;
                    --i; // Try this one again
                    break;
                }
            }
            if (!exists) {
                cities[nextIndex] = {nextIndex, name}; // Add the new city
                cout << "Added city '" << name << "' with index " << nextIndex << "\n"; // Woohoo!
                nextIndex++; // Next ID, please!
                resizeMatrices(); // Make room in our grids
                saveData(); // Save to file so we don’t lose it
            }
        }
    }

    // Connect two cities with a road, like building a new highway
    void addRoad() {
        int city1 = readInt("Enter first city index: ", 1, nextIndex - 1); // Pick city 1
        int city2 = readInt("Enter second city index: ", 1, nextIndex - 1); // Pick city 2
        if (cities.find(city1) == cities.end() || cities.find(city2) == cities.end()) {
            cout << "Error: One or both cities do not exist!\n"; // Gotta pick real cities!
            return;
        }
        if (city1 == city2) {
            cout << "Error: Cannot add a road from a city to itself!\n"; // No looping roads!
            return;
        }
        if (roads[city1][city2]) {
            cout << "Error: Road between " << cities[city1].name << " and " << cities[city2].name << " already exists!\n";
            return; // No double roads!
        }
        roads[city1][city2] = roads[city2][city1] = true; // Build that road both ways
        cout << "Added road between " << cities[city1].name << " (" << city1 << ") and "
             << cities[city2].name << " (" << city2 << ")\n"; // Road’s ready!
        saveData(); // Save it to file
    }

    // Set a budget for a road, like funding a new Kigali-Muhanga route
    void addBudget() {
        int city1 = readInt("Enter first city index: ", 1, nextIndex - 1);
        int city2 = readInt("Enter second city index: ", 1, nextIndex - 1);
        if (cities.find(city1) == cities.end() || cities.find(city2) == cities.end()) {
            cout << "Error: One or both cities do not exist!\n"; // Need real cities
            return;
        }
        if (!roads[city1][city2]) {
            cout << "Error: No road exists between " << cities[city1].name << " and "
                 << cities[city2].name << "!\n"; // Build the road first!
            return;
        }
        double budget = readInt("Enter budget (billions of RWF): ", 0) / 1.0; // Get budget in billions
        budgets[city1][city2] = budgets[city2][city1] = budget; // Set budget both ways
        cout << "Assigned budget of " << budget << " billion RWF to road between "
             << cities[city1].name << " and " << cities[city2].name << "\n"; // Money allocated!
        saveData(); // Save to file
    }

    // Rename a city, like changing “Huye” to “Gisagara”
    void editCity() {
        int index = readInt("Enter city index to edit: ", 1, nextIndex - 1);
        if (cities.find(index) == cities.end()) {
            cout << "Error: City with index " << index << " does not exist!\n"; // Wrong ID!
            return;
        }
        string newName = readString("Enter new city name: "); // Get new name
        // Check if new name is taken
        for (const auto& kv : cities) {
            if (kv.second.name == newName && kv.first != index) {
                cout << "Error: City named '" << newName << "' already exists!\n"; // No duplicates!
                return;
            }
        }
        string oldName = cities[index].name;
        cities[index].name = newName; // Update the name
        cout << "Changed city " << index << " from '" << oldName << "' to '" << newName << "'\n"; // Done!
        saveData(); // Save the change
    }

    // Find a city by name, like looking up “Musanze”
    void searchCity() {
        string name = readString("Enter city name to search: ");
        bool found = false;
        for (const auto& kv : cities) {
            if (kv.second.name == name) {
                cout << "Found: Index " << kv.first << ", Name: " << kv.second.name << "\n"; // Got it!
                found = true;
                break;
            }
        }
        if (!found) {
            cout << "Error: City named '" << name << "' not found!\n"; // Oops, not there!
        }
    }

    void searchByIndex() {
        int index = readInt("Enter city index to search: ", 1, nextIndex - 1); // Ask for the city’s ID
        if (cities.find(index) != cities.end()) {
            cout << "Found: Index " << index << ", Name: " << cities[index].name << "\n"; // Sweet, we found it!
        } else {
            cout << "Error: City with index " << index << " not found!\n"; // Oops, no city with that ID!
        }
    }

    // Show all cities, like a tour guide listing hot spots
    void displayCities() {
        cout << "\n--- Cities ---\n";
        cout << "Index | City Name\n";
        cout << "------|----------\n";
        for (const auto &kv: cities) {
            cout << setw(5) << kv.first << " | " << kv.second.name << "\n"; // Neat table of cities
        }
    }

    // Show all roads, like mapping out Rwanda’s highways
    void displayRoads() {
        displayCities(); // First, show all cities like a tour guide’s list
        // cout << "\n--- Roads ---\n";
        // cout << "Road | Budget (billions RWF)\n";
        // cout << "-----|----------------------\n";
        // for (int i = 1; i < nextIndex; ++i) {
        //     for (int j = i + 1; j < nextIndex; ++j) {
        //         if (roads[i][j] && cities.find(i) != cities.end() && cities.find(j) != cities.end()) {
        //             cout << cities[i].name << "-" << cities[j].name << " | "
        //                  << fixed << setprecision(2) << budgets[i][j] << "\n"; // List roads and budgets
        //         }
        //     }
        // }

        // Show a grid of which cities are connected
        cout << "\nRoad Adjacency Matrix (1 = road exists, 0 = no road):\n";
        cout << "   ";
        for (int i = 1; i < nextIndex; ++i) {
            if (cities.find(i) != cities.end()) {
                cout << setw(5) << i; // City IDs as headers
            }
        }
        cout << "\n";
        for (int i = 1; i < nextIndex; ++i) {
            if (cities.find(i) != cities.end()) {
                cout << setw(2) << i << ":";
                for (int j = 1; j < nextIndex; ++j) {
                    if (cities.find(j) != cities.end()) {
                        cout << setw(5) << roads[i][j]; // 1 or 0 for roads
                    }
                }
                cout << "\n";
            }
        }
    }

    // Show everything, like a full Rwanda road trip overview!
    void displayData() {
        cout << "\n--- Recorded Data ---\n";
        displayCities(); // List all cities
        // Show road grid
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
        // Show budget grid
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

    // Save our work to files, like storing a map in a drawer
    void saveData() {
        // Save cities to cities.txt
        ofstream cityFile("cities.txt");
        cityFile << "index,city_name\n"; // Header for the file
        for (const auto& kv : cities) {
            cityFile << kv.first << "," << kv.second.name << "\n"; // Write each city
        }
        cityFile.close();
        // Save roads to roads.txt
        ofstream roadFile("roads.txt");
        roadFile << "road,budget\n"; // Header
        for (int i = 1; i < nextIndex; ++i) {
            for (int j = i + 1; j < nextIndex; ++j) {
                if (roads[i][j] && cities.find(i) != cities.end() && cities.find(j) != cities.end()) {
                    roadFile << i << "-" << j << "," << fixed << setprecision(2) << budgets[i][j] << "\n"; // Write each road
                }
            }
        }
        roadFile.close();
    }

    // Load our saved map from files, like opening that drawer
    void loadData() {
        // Load cities
        ifstream cityFile("cities.txt");
        if (cityFile) {
            string line;
            getline(cityFile, line); // Skip the header
            while (getline(cityFile, line)) {
                stringstream ss(line);
                string indexStr, name;
                getline(ss, indexStr, ',');
                getline(ss, name, ',');
                try {
                    int index = stoi(indexStr); // Get the city ID
                    cities[index] = {index, name}; // Add to our list
                    nextIndex = max(nextIndex, index + 1); // Update next ID
                } catch (...) {
                    cout << "Error parsing city: " << line << "\n"; // Oops, bad data!
                }
            }
            cityFile.close();
        }
        resizeMatrices(); // Get our grids ready
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
                size_t dash = road.find('-'); // Look for city1-city2 format
                if (dash == string::npos) continue;
                try {
                    int city1 = stoi(road.substr(0, dash));
                    int city2 = stoi(road.substr(dash + 1));
                    double budget = stod(budgetStr);
                    if (cities.find(city1) != cities.end() && cities.find(city2) != cities.end()) {
                        roads[city1][city2] = roads[city2][city1] = true; // Set road
                        budgets[city1][city2] = budgets[city2][city1] = budget; // Set budget
                    }
                } catch (...) {
                    cout << "Error parsing road: " << line << "\n"; // Bad road data!
                }
            }
            roadFile.close();
        }
    }

    // Show a friendly guide, like a tour guide for our app
    void displayHelp() {
    cout << "\n=== Welcome to the Rwanda Road Network Manager! ===\n";
    cout << "This app helps you keep track of Rwanda’s awesome cities and the roads connecting them, like a super cool map for planning!\n";
    cout << "\nWhat Can You Do?\n";
    cout << "- Add Cities: Pop new cities onto the map, like adding a new favorite spot.\n";
    cout << "- Connect Cities: Build roads between cities, like linking Kigali to Huye.\n";
    cout << "- Set Budgets: Plan how much cash (in billions of RWF) to spend on roads.\n";
    cout << "- Save Everything: Your work is saved to files so you can pick up later.\n";
    cout << "- Auto IDs: Cities get numbered automatically (1, 2, 3...), no stress!\n";

    cout << "\nMenu Options (Pick a Number!):\n";
    cout << "1. Add new city(ies): Add one or more new cities to the map.\n";
    cout << "2. Add roads between cities: Connect two cities with a road.\n";
    cout << "3. Add the budget for roads: Set money for a road, like 28.6 billion RWF.\n";
    cout << "4. Edit city: Change a city’s name, like renaming Huye to something else.\n";
    cout << "5. Search for a city using its index: Look up a city by its ID number.\n";
    cout << "6. Display cities: See all cities and their IDs in reverse order (newest first).\n";
    cout << "7. Display cities: See all cities and their IDs in reverse order, like a quick tour guide list.\n";
    cout << "8. Display recorded data: See everything—cities, roads, and budgets.\n";
    cout << "9. Help: Show this friendly guide.\n";
    cout << "10. Exit: Save your work and head out.\n";

    cout << "\nTips to Rock This App:\n";
    cout << "- City IDs are given automatically (1 for Kigali, 2 for Huye, etc.).\n";
    cout << "- Budgets are in billions of RWF, so type a number like 28 for 28 billion.\n";
    cout << "- Don’t use commas in city names—they mess with our files!\n";
    cout << "- Everything saves automatically to files, so no worries about losing work.\n";

    cout << "\nWhere’s the Data Kept?\n";
    cout << "- cities.txt: Lists all cities with their IDs.\n";
    cout << "- roads.txt: Shows which cities are connected and their budgets.\n";

};

// The main stage where the app runs, like the control center in Kigali
int main() {
    CityGraph graph; // Create our road network manager
    while (true) { // Keep the app running until the user says bye
        cout << "\n=== City Connection System ===\n" // Show the menu, like a restaurant list
             << "1. Add new city(ies)\n"
             << "2. Add roads between cities\n"
             << "3. Add the budget for roads\n"
             << "4. Edit city\n"
             << "5. Search for a city using its index\n"
             << "6. Display cities\n"
             << "7. Display roads\n"
             << "8. Display recorded data on console\n"
             << "9. Help\n"
             << "10. Exit\n";
        int choice = readInt("Choose: ", 1, 10); // Get the user’s pick (1–10)

        if (choice == 10) break; // Time to exit? Peace out!

        switch (choice) { // Do what the user picked
            case 1:
                graph.addCities(); // Add some new cities
                break;
            case 2:
                graph.addRoad(); // Build a new road
                break;
            case 3:
                graph.addBudget(); // Set some cash for a road
                break;
            case 4:
                graph.editCity(); // Rename a city
                break;
            case 5:
                graph.searchByIndex(); // Find a city
                break;
            case 6:
                graph.displayCities(); // Show all cities
                break;
            case 7:
                graph.displayRoads(); // Show all roads
                break;
            case 8:
                graph.displayData(); // Show everything
                break;
            case 9:
                graph.displayHelp(); // Show the guide
                break;
        }
    }
    return 0; // All done, app closes!
}