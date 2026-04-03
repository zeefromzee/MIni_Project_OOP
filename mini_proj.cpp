#include <iostream>
#include <vector>
#include <string>
#include <cstdlib>
#include <ctime>

using namespace std;

// Item class - represents tradeable goods
class Item {
private:
    string name;
    int quantity;
    int basePrice;

public:
    Item(string n, int p) : name(n), quantity(0), basePrice(p) {}

    string getName() const { return name; }
    int getQuantity() const { return quantity; }
    int getBasePrice() const { return basePrice; }

    void setQuantity(int q) { quantity = q; }
    void addQuantity(int q) { quantity += q; }
    void removeQuantity(int q) { quantity -= q; }
};

// Station class - represents space stations
class Station {
private:
    string name;
    string location;
    vector<Item> market;
    vector<int> prices; // current prices for each item

public:
    Station(string n, string loc) : name(n), location(loc) {}

    string getName() const { return name; }
    string getLocation() const { return location; }
    vector<Item>& getMarket() { return market; }
    vector<int>& getPrices() { return prices; }

    void addItem(Item item) {
        market.push_back(item);
        prices.push_back(item.getBasePrice());
    }

    void fluctuatePrices() {
        for (int i = 0; i < prices.size(); i++) {
            int change = (rand() % 21) - 10; // -10 to +10
            prices[i] = prices[i] + (prices[i] * change / 100);
            if (prices[i] < 10) prices[i] = 10; // minimum price
        }
    }

    int getPrice(int itemIndex) const {
        return prices[itemIndex];
    }
};

// Ship class - represents player's vessel
class Ship {
private:
    vector<pair<int, int>> cargo; // (itemIndex, quantity)
    int credits;
    int cargoCapacity;
    int currentStationIndex;
    int totalProfit;

public:
    Ship() : credits(1000), cargoCapacity(20), currentStationIndex(0), totalProfit(0) {}

    int getCredits() const { return credits; }
    int getCargoCapacity() const { return cargoCapacity; }
    int getCurrentStationIndex() const { return currentStationIndex; }
    int getTotalProfit() const { return totalProfit; }
    vector<pair<int, int>>& getCargo() { return cargo; }

    void setCredits(int c) { credits = c; }
    void addCredits(int c) { credits += c; totalProfit += c; }
    void subtractCredits(int c) { credits -= c; }
    void setCurrentStation(int s) { currentStationIndex = s; }

    int getUsedCapacity() const {
        int used = 0;
        for (auto& item : cargo) {
            used += item.second;
        }
        return used;
    }

    bool canAddCargo(int amount) const {
        return (getUsedCapacity() + amount) <= cargoCapacity;
    }

    void addToCargo(int itemIndex, int quantity) {
        for (auto& item : cargo) {
            if (item.first == itemIndex) {
                item.second += quantity;
                return;
            }
        }
        cargo.push_back({itemIndex, quantity});
    }

    void removeFromCargo(int itemIndex, int quantity) {
        for (auto it = cargo.begin(); it != cargo.end(); it++) {
            if (it->first == itemIndex) {
                it->second -= quantity;
                if (it->second <= 0) {
                    cargo.erase(it);
                }
                return;
            }
        }
    }

    int getCargoQuantity(int itemIndex) const {
        for (const auto& item : cargo) {
            if (item.first == itemIndex) {
                return item.second;
            }
        }
        return 0;
    }
};

// Game class - main game controller
class Game {
private:
    vector<Station> stations;
    Ship ship;
    vector<Item> itemTemplates;
    bool gameRunning;
    int targetProfit;

public:
    Game() : gameRunning(true), targetProfit(5000) {
        srand(time(0));
        initializeGame();
    }

    void initializeGame() {
        // Create items
        itemTemplates.push_back(Item("Water", 100));
        itemTemplates.push_back(Item("Metals", 150));
        itemTemplates.push_back(Item("Energy Cells", 200));
        itemTemplates.push_back(Item("Luxury Goods", 250));

        // Create stations
        Station earth("Earth", "Solar System - Origin");
        Station mars("Mars", "Solar System - The Red Planet");
        Station titan("Titan", "Saturn System - Methane Lakes");
        Station europa("Europa", "Jupiter System - Ice Moon");
        Station venus("Venus", "Solar System - Hell Planet");
        Station pluto("Pluto", "Solar System - The Lost Planet");

        // Add items to stations with different base prices
        for (auto& item : itemTemplates) {
            earth.addItem(item);
            mars.addItem(item);
            titan.addItem(item);
            europa.addItem(item);
            venus.addItem(item);
            pluto.addItem(item);
        }

        // Adjust base prices slightly per station for variety
        stations.push_back(earth);
        stations.push_back(mars);
        stations.push_back(titan);
        stations.push_back(europa);
        stations.push_back(venus);
        stations.push_back(pluto);
    }

    void displayWelcome() {
        cout << "\n========== SPACE TRADING SIMULATOR ==========\n";
        cout << "Welcome, Captain!\n";
        cout << "You start with 1000 credits and a cargo hold that can fit 20 units.\n";
        cout << "Travel between space stations, buy low, sell high!\n";
        cout << "Goal: Accumulate " << targetProfit << " credits through smart trading.\n";
        cout << "=========================================\n\n";
    }

    void displayStationInfo() {
        Station& currentStation = stations[ship.getCurrentStationIndex()];
        cout << "\n--- At Station: " << currentStation.getName() << " ---\n";
        cout << "Location: " << currentStation.getLocation() << "\n";
        cout << "Credits: " << ship.getCredits() << " | Cargo: " << ship.getUsedCapacity() 
             << "/" << ship.getCargoCapacity() << " units\n";
        cout << "Profit so far: " << ship.getTotalProfit() << "\n\n";
    }

    void displayMarket() {
        Station& currentStation = stations[ship.getCurrentStationIndex()];
        cout << "Available items at " << currentStation.getName() << ":\n";
        for (int i = 0; i < currentStation.getMarket().size(); i++) {
            cout << i + 1 << ". " << itemTemplates[i].getName() 
                 << " - " << currentStation.getPrice(i) << " credits/unit";
            int owned = ship.getCargoQuantity(i);
            if (owned > 0) {
                cout << " (You own: " << owned << ")";
            }
            cout << "\n";
        }
        cout << "\n";
    }

    void displayMenu() {
        cout << "What do you do?\n";
        cout << "1. Buy item\n";
        cout << "2. Sell item\n";
        cout << "3. View inventory\n";
        cout << "4. Travel to next station\n";
        cout << "5. Check stats\n";
        cout << "6. Leave\n";
        cout << "Choice: ";
    }

    void buyItem() {
        displayMarket();
        cout << "Which item to buy? (Enter number, 0 to cancel): ";
        int itemChoice;
        cin >> itemChoice;

        if (itemChoice < 1 || itemChoice > itemTemplates.size()) {
            cout << "Invalid choice.\n";
            return;
        }

        cout << "How many units? ";
        int quantity;
        cin >> quantity;

        if (quantity <= 0) {
            cout << "Invalid quantity.\n";
            return;
        }

        Station& currentStation = stations[ship.getCurrentStationIndex()];
        int price = currentStation.getPrice(itemChoice - 1);
        int totalCost = price * quantity;

        if (totalCost > ship.getCredits()) {
            cout << "Not enough credits! You need " << totalCost << " but only have " 
                 << ship.getCredits() << ".\n";
            return;
        }

        if (!ship.canAddCargo(quantity)) {
            cout << "Not enough cargo space! You need " << quantity << " units but only have " 
                 << (ship.getCargoCapacity() - ship.getUsedCapacity()) << " free.\n";
            return;
        }

        ship.subtractCredits(totalCost);
        ship.addToCargo(itemChoice - 1, quantity);
        cout << "Bought " << quantity << " " << itemTemplates[itemChoice - 1].getName() 
             << " for " << totalCost << " credits.\n";
    }

    void sellItem() {
        if (ship.getCargo().empty()) {
            cout << "Your cargo hold is empty!\n";
            return;
        }

        cout << "Your inventory:\n";
        int counter = 1;
        for (auto& cargoItem : ship.getCargo()) {
            cout << counter << ". " << itemTemplates[cargoItem.first].getName() 
                 << " (x" << cargoItem.second << ")\n";
            counter++;
        }

        cout << "What to sell? (Enter number, 0 to cancel): ";
        int itemChoice;
        cin >> itemChoice;

        if (itemChoice < 1 || itemChoice > ship.getCargo().size()) {
            cout << "Invalid choice.\n";
            return;
        }

        cout << "How many units? ";
        int quantity;
        cin >> quantity;

        if (quantity <= 0) {
            cout << "Invalid quantity.\n";
            return;
        }

        int cargoItemIndex = ship.getCargo()[itemChoice - 1].first;
        if (quantity > ship.getCargoQuantity(cargoItemIndex)) {
            cout << "You don't have that many!\n";
            return;
        }

        Station& currentStation = stations[ship.getCurrentStationIndex()];
        int price = currentStation.getPrice(cargoItemIndex);
        int totalRevenue = price * quantity;

        ship.addCredits(totalRevenue);
        ship.removeFromCargo(cargoItemIndex, quantity);
        cout << "Sold " << quantity << " " << itemTemplates[cargoItemIndex].getName() 
             << " for " << totalRevenue << " credits.\n";
    }

    void viewInventory() {
        if (ship.getCargo().empty()) {
            cout << "Your cargo hold is empty.\n";
            return;
        }

        cout << "\nYour Inventory:\n";
        for (auto& cargoItem : ship.getCargo()) {
            cout << "- " << itemTemplates[cargoItem.first].getName() 
                 << " x" << cargoItem.second << "\n";
        }
        cout << "\n";
    }

    void checkStats() {
        cout << "\n========== YOUR STATS ==========\n";
        cout << "Credits: " << ship.getCredits() << "\n";
        cout << "Total Profit: " << ship.getTotalProfit() << "\n";
        cout << "Current Station: " << stations[ship.getCurrentStationIndex()].getName() << "\n";
        cout << "Cargo Used: " << ship.getUsedCapacity() << "/" << ship.getCargoCapacity() << "\n";
        cout << "Progress to Goal: " << ship.getTotalProfit() << "/" << targetProfit << "\n";
        cout << "================================\n\n";
    }

    void travel() {
        int currentIndex = ship.getCurrentStationIndex();
        int nextIndex = (currentIndex + 1) % stations.size();

        cout << "\nTraveling from " << stations[currentIndex].getName() 
             << " to " << stations[nextIndex].getName() << "...\n";
        cout << "Journey complete!\n\n";

        ship.setCurrentStation(nextIndex);
        stations[nextIndex].fluctuatePrices();
    }

    void checkWinCondition() {
        if (ship.getTotalProfit() >= targetProfit) {
            cout << "\n========== YOU WIN! ==========\n";
            cout << "Congratulations, Captain!\n";
            cout << "You've accumulated " << ship.getTotalProfit() << " credits!\n";
            cout << "You've mastered the art of space trading.\n";
            cout << "==============================\n\n";
            gameRunning = false;
        }
    }

    void run() {
        displayWelcome();

        while (gameRunning) {
            displayStationInfo();
            displayMenu();

            int choice;
            cin >> choice;

            switch (choice) {
                case 1:
                    buyItem();
                    break;
                case 2:
                    sellItem();
                    break;
                case 3:
                    viewInventory();
                    break;
                case 4:
                    travel();
                    break;
                case 5:
                    checkStats();
                    break;
                case 6:
                    break;
                default:
                    cout << "Invalid choice!\n";
            }

            checkWinCondition();
        }
    }
};

int main() {
    Game game;
    game.run();
    return 0;
}