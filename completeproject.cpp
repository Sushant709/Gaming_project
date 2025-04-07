#include <iostream>
#include <vector>
#include <map>
#include <random>
#include <limits>
#include <cstdlib>
#include <ctime>
#ifdef _WIN32
#include <conio.h>
#include <windows.h>
#else
#include <termios.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#endif
using namespace std;
random_device rd;
mt19937 gen(rd());
int getRandomNumber(int min, int max) {
    uniform_int_distribution<> distrib(min, max);
    return distrib(gen);
}
void clearScreen() {
#ifdef _WIN32
    system("cls");
#else
    cout << "\033[2J\033[1;1H";
#endif
}
void pauseGame() {
    cout << "\nPress Enter to continue...";
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
    cin.get();
}
const int WIDTH = 20;
const int HEIGHT = 20;

void carGameSetup(int& carPos, int& obstacleX, int& obstacleY, int& score, bool& gameOver) {
    carPos = WIDTH / 2;
    obstacleX = getRandomNumber(0, WIDTH - 1);
    obstacleY = 0;
    score = 0;
    gameOver = false;
}
void carGameDraw(int carPos, int obstacleX, int obstacleY, int score) {
#ifdef _WIN32
    system("cls");
#else
    cout << "\033[2J\033[1;1H"; // Clear screen for Unix-like systems
#endif
    for (int y = 0; y < HEIGHT; y++) {
        for (int x = 0; x < WIDTH; x++) {
            if (y == HEIGHT - 1 && x == carPos)
                cout << "A"; // Player car
            else if (y == obstacleY && x == obstacleX)
                cout << "#"; // Obstacle
            else
                cout << " ";
        }
        cout << endl;
    }
    cout << "\nScore: " << score << endl;
}
#ifdef _WIN32
void carGameInput(int& carPos) {
    if (_kbhit()) {
        char ch = _getch();
        if (ch == 'a' || ch == 'A') {
            if (carPos > 0)
                carPos--;
        }
        if (ch == 'd' || ch == 'D') {
            if (carPos < WIDTH - 1)
                carPos++;
        }
    }
}
void sleepForMilliseconds(int milliseconds) {
    Sleep(milliseconds);
}
#else
int kbhit() {
    struct termios oldt, newt;
    int ch;
    int oldf;
    tcgetattr(STDIN_FILENO, &oldt);
    newt = oldt;
    newt.c_lflag &= ~(ICANON | ECHO);
    tcsetattr(STDIN_FILENO, TCSANOW, &newt);
    oldf = fcntl(STDIN_FILENO, F_GETFL, 0);
    fcntl(STDIN_FILENO, F_SETFL, oldf | O_NONBLOCK);
    ch = getchar();
    tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
    fcntl(STDIN_FILENO, F_SETFL, oldf);
    if (ch != EOF) {
        ungetc(ch, stdin);
        return 1;
    }
    return 0;
}
char getch() {
    char buf = 0;
    struct termios old = {0};
    if (tcgetattr(0, &old) < 0)
        perror("tcsetattr()");
    old.c_lflag &= ~ICANON;
    old.c_lflag &= ~ECHO;
    old.c_cc[VMIN] = 1;
    old.c_cc[VTIME] = 0;
    if (tcsetattr(0, TCSANOW, &old) < 0)
        perror("tcsetattr ICANON");
    if (read(0, &buf, 1) < 0)
        perror("read()");
    old.c_lflag |= ICANON;
    old.c_lflag |= ECHO;
    if (tcsetattr(0, TCSADRAIN, &old) < 0)
        perror("tcsetattr ~ICANON");
    return buf;
}
void carGameInput(int& carPos) {
    if (kbhit()) {
        char ch = getch();
        if (ch == 'a' || ch == 'A') {
            if (carPos > 0)
                carPos--;
        }
        if (ch == 'd' || ch == 'D') {
            if (carPos < WIDTH - 1)
                carPos++;
        }
    }
}
void sleepForMilliseconds(int milliseconds) {
    usleep(milliseconds * 1000); // usleep takes microseconds
}
#endif
void carGameLogic(int& carPos, int& obstacleX, int& obstacleY, int& score, bool& gameOver) {
    obstacleY++;
    if (obstacleY >= HEIGHT) {
        obstacleY = 0;
        obstacleX = getRandomNumber(0, WIDTH - 1);
        score++;
    }
    if (obstacleY == HEIGHT - 1 && obstacleX == carPos) {
        gameOver = true;
    }
}
void playCarGame() {
    int carPos, obstacleX, obstacleY, score;
    bool gameOver;
    carGameSetup(carPos, obstacleX, obstacleY, score, gameOver);
    while (!gameOver) {
        carGameDraw(carPos, obstacleX, obstacleY, score);
        carGameInput(carPos);
        carGameLogic(carPos, obstacleX, obstacleY, score, gameOver);
        sleepForMilliseconds(100); // Game speed
    }
#ifdef _WIN32
    system("cls");
    system("pause");
#else
    cout << "\033[2J\033[1;1H";
    cout << "\n💥 Game Over! Final Score: " << score << "\n";
    cout << "\nPress Enter to continue...\n";
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
    cin.get();
#endif
}
struct Item {
    string name;
    string type;  // "food" or "weapon"
    int value;    // health points or damage boost
};
void displayGameState(const string& location, int playerHealth, int enemiesRemaining, const map<string, int>& inventory) {
    clearScreen();
    cout << "================================\n";
    cout << "Location: " << location << "\n";
    cout << "Health: " << playerHealth << "/100\n";
    cout << "Enemies: " << enemiesRemaining << "\n";
    cout << "================================\n";
}
void displayInventory(const map<string, int>& inventory) {
    cout << "\n--- INVENTORY ---\n";
    if (inventory.empty()) {
        cout << "Empty\n";
    } else {
        for (const auto& item : inventory) {
            cout << "- " << item.first << " x" << item.second << "\n";
        }
    }
    cout << "----------------\n";
}
const vector<Item> gameItems = {
    {"Apple", "food", 15},
    {"Bread", "food", 25},
    {"Health Potion", "food", 40},
    {"Rusty Sword", "weapon", 5},
    {"Iron Sword", "weapon", 10},
    {"Steel Sword", "weapon", 15},
    {"Magic Sword", "weapon", 20}
};
Item getRandomItem() {
    return gameItems[getRandomNumber(0, static_cast<int>(gameItems.size()) - 1)];
}
void useItem(map<string, int>& inventory, int& playerHealth, int& weaponBonus) {
    if (inventory.empty()) {
        cout << "Your inventory is empty!\n";
        return;
    }
    cout << "\nSelect item to use:\n";
    int index = 1;
    vector<string> itemNames;
    for (const auto& item : inventory) {
        cout << index++ << ". " << item.first << " (x" << item.second << ")\n";
        itemNames.push_back(item.first);
    }
    cout << "0. Cancel\n";
    int choice;
    while (true) {
        cout << "Your choice: ";
        cin >> choice;
        if (cin.fail()) {
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            cout << "Invalid input. Please enter a number.\n";
            continue;
        }
        if (choice == 0) return;
        if (choice > 0 && choice <= itemNames.size()) break;
        cout << "Invalid choice. Please try again.\n";
    }
    string selected = itemNames[choice - 1];
    for (const auto& item : gameItems) {
        if (item.name == selected) {
            if (item.type == "food") {
                playerHealth = min(100, playerHealth + item.value);
                cout << "\nYou used " << item.name << " and regained " << item.value << " health!\n";
                cout << "Current health: " << playerHealth << "/100\n";
            } else if (item.type == "weapon") {
                weaponBonus = item.value;
                cout << "\nYou equipped " << item.name << " (+" << item.value << " damage)!\n";
            }
            if (--inventory[selected] <= 0) {
                inventory.erase(selected);
            }
            return;
        }
    }
}
void combat(int& playerHealth, int& enemiesRemaining, map<string, int>& inventory, int& weaponBonus) {
    const int baseDamage = 10;
    int totalDamage = baseDamage + weaponBonus;
    while (playerHealth > 0 && enemiesRemaining > 0) {
        displayGameState("COMBAT", playerHealth, enemiesRemaining, inventory);
        cout << "\nCOMBAT OPTIONS:\n";
        cout << "1. Attack (" << totalDamage << " damage)\n";
        cout << "2. Use item\n";
        cout << "3. Try to flee\n";
        cout << "Choose action: ";
        int choice;
        while (!(cin >> choice) || choice < 1 || choice > 3) {
            cout << "Invalid choice. Enter 1-3: ";
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
        }
        if (choice == 1) {
            cout << "\nYou attack for " << totalDamage << " damage!\n";
            enemiesRemaining--;
            if (enemiesRemaining <= 0) {
                cout << "\nYou defeated all enemies!\n";
                if (getRandomNumber(1, 100) <= 40) {
                    Item loot = getRandomItem();
                    cout << "You found: " << loot.name << "!\n";
                    inventory[loot.name]++;
                }
                return;
            }
            int enemyDamage = getRandomNumber(8, 15);
            cout << "Enemy attacks for " << enemyDamage << " damage!\n";
            playerHealth -= enemyDamage;
            if (playerHealth <= 0) {
                cout << "\nYou have been defeated!\n";
                return;
            }
        } else if (choice == 2) {
            useItem(inventory, playerHealth, weaponBonus);
        } else if (choice == 3) {
            if (getRandomNumber(1, 100) <= 50) {
                cout << "\nYou successfully fled!\n";
                return;
            }
            cout << "\nEscape failed!\n";
            int enemyDamage = getRandomNumber(8, 15);
            cout << "Enemy attacks for " << enemyDamage << " damage!\n";
            playerHealth -= enemyDamage;
            if (playerHealth <= 0) {
                cout << "\nYou have been defeated!\n";
                return;
            }
        }
        pauseGame();
    }
}
void playTextAdventureGame() {
    string location = "Forest Entrance";
    int health = 100;
    int enemies = getRandomNumber(2, 5);
    int weaponBonus = 0;
    map<string, int> inventory;
    inventory["Apple"] = 2;
    inventory["Rusty Sword"] = 1;
    weaponBonus = 5; // Rusty Sword bonus
    clearScreen();
    cout << "=== TEXT ADVENTURE GAME ===\n\n";
    cout << "You wake up at the " << location << ".\n";
    cout << "You spot " << enemies << " enemies nearby.\n";
    cout << "Your inventory contains some basic supplies.\n";
    pauseGame();
    bool gameRunning = true;
    while (gameRunning && health > 0) {
        displayGameState(location, health, enemies, inventory);
        cout << "\nMAIN MENU:\n";
        cout << "1. Explore\n";
        cout << "2. Fight enemies\n";
        cout << "3. Inventory\n";
        cout << "4. Use item\n";
        cout << "5. Quit game\n";
        cout << "Choose action: ";
        int choice;
        while (!(cin >> choice) || choice < 1 || choice > 5) {
            cout << "Invalid choice. Enter 1-5: ";
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
        }
        switch (choice) {
            case 1: // Explore
                if (enemies > 0) {
                    cout << "\nYou can't explore with enemies nearby!\n";
                } else {
                    if (location == "Forest Entrance") {
                        location = "Deep Forest";
                    } else if (location == "Deep Forest") {
                        location = "Ancient Ruins";
                    } else {
                        cout << "\nYou've reached the end of the known world!\n";
                        break;
                    }
                    enemies = getRandomNumber(3, 6);
                    if (getRandomNumber(1, 100) <= 50) {
                        Item found = getRandomItem();
                        cout << "\nYou found: " << found.name << "!\n";
                        inventory[found.name]++;
                    }
                    cout << "\nYou venture into the " << location << ".\n";
                    cout << "There are " << enemies << " enemies here.\n";
                }
                break;
            case 2: // Fight
                if (enemies <= 0) {
                    cout << "\nNo enemies to fight here!\n";
                } else {
                    combat(health, enemies, inventory, weaponBonus);
                }
                break;
            case 3: // Inventory
                displayInventory(inventory);
                break;
            case 4: // Use item
                useItem(inventory, health, weaponBonus);
                break;
            case 5: // Quit
                cout << "\nAre you sure you want to quit? (y/n): ";
                char confirm;
                cin >> confirm;
                if (confirm == 'y' || confirm == 'Y') {
                    gameRunning = false;
                    cout << "\nThanks for playing!\n";
                }
                break;
        }
        if (health > 0 && location == "Ancient Ruins" && enemies == 0) {
            cout << "\nCONGRATULATIONS!\n";
            cout << "You've cleared all areas and won the game!\n";
            gameRunning = false;
        }
        pauseGame();
    }
    if (health <= 0) {
        cout << "\nGAME OVER\n";
        cout << "You were defeated in battle...\n";
    }
}
void showMainMenu() {
    while (true) {
        clearScreen();
        cout << "=== GAME SELECTION ===\n\n";
        cout << "1. Car Dodging Game\n";
        cout << "2. Text Adventure Game\n";
        cout << "3. Exit\n\n";
        cout << "Select game (1-3): ";
        int choice;
        cin >> choice;
        if (cin.fail()) { 
            cout << "Invalid input! Please enter a number.\n";
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            continue;
        }
        switch (choice) {
            case 1:
                playCarGame();
                break;
            case 2:
                playTextAdventureGame();
                break;
            case 3:
                return;
            default:
                cout << "Invalid selection!\n";
                sleepForMilliseconds(1000);
        }
    }
}
int main() {
    showMainMenu();
    cout << "\nThanks for playing!\n";
    return 0;
}