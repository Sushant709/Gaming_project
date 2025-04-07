#include <iostream>
#include <cstdlib>
#include <ctime>
#include <chrono>
#include <thread>
#include <random>
#include <string>
using namespace std;
const int WIDTH = 20;
const int HEIGHT = 20;
int getRandomNumber(int min, int max) {
    random_device rd;
    mt19937 gen(rd());
    uniform_int_distribution<> distrib(min, max);
    return distrib(gen);
}
void displayGameState(const string& location, int playerHealth, int enemiesRemaining) {
    cout << "\n--- Current State ---\n"
         << "Location: " << location << endl
         << "Player Health: " << playerHealth << endl
         << "Enemies Remaining: " << enemiesRemaining << endl
         << "-----------------------\n";
}
void combat(int& playerHealth, int& enemiesRemaining) {
    cout << "\n--- Combat Encounter! ---\n";
    while (playerHealth > 0 && enemiesRemaining > 0) {
        int playerDamage = getRandomNumber(5, 15);
        cout << "You attack and deal " << playerDamage << " damage!\n";
        enemiesRemaining -= playerDamage;
        enemiesRemaining = max(0, enemiesRemaining);
        if (enemiesRemaining <= 0) {
            cout << "You defeated all enemies!\n";
            return;
        }
        int enemyDamage = getRandomNumber(3, 10);
        cout << "The enemy attacks and deals " << enemyDamage << " damage!\n";
        playerHealth -= enemyDamage;
        playerHealth = max(0, playerHealth);
        if (playerHealth <= 0) {
            cout << "You were defeated!\n";
            return;
        }
        cout << "Player Health: " << playerHealth << ", Enemies Remaining: " << enemiesRemaining << endl;
    }
}
int main() {
    cout << "Select a game:\n"
         << "1. Car Avoidance Game\n"
         << "2. Text Adventure Game\n"
         << "Enter your choice (1 or 2): ";
    int gameChoice;
    cin >> gameChoice;
    if (gameChoice == 1) {
        srand(time(0));
        int carPos = WIDTH / 2, obstacleX, obstacleY = 0, score = 0;
        bool gameOver = false;
        while (!gameOver) {
            system("clear");
            for (int y = 0; y < HEIGHT; y++) {
                for (int x = 0; x < WIDTH; x++) {
                    cout << ((y == HEIGHT - 1 && x == carPos) ? "A" : (y == obstacleY && x == obstacleX) ? "#" : " ");
                }
                cout << endl;
            }
            cout << "\nScore: " << score << endl;
            char ch = 0;
            auto start = chrono::high_resolution_clock::now();
            while (chrono::duration_cast<chrono::milliseconds>(chrono::high_resolution_clock::now() - start).count() < 100) {
            }
            obstacleY++;
            if (obstacleY >= HEIGHT) {
                obstacleY = 0;
                obstacleX = rand() % WIDTH;
                score++;
            }
            if (obstacleY == HEIGHT - 1 && obstacleX == carPos) gameOver = true;
            this_thread::sleep_for(chrono::milliseconds(100));
        }
        system("clear");
        cout << "\n💥 Game Over! Final Score: " << score << "\n";
    } else if (gameChoice == 2) {
        string currentLocation = "Forest Entrance";
        int playerHealth = 100;
        int enemiesRemaining = getRandomNumber(5, 10);
        cout << "Welcome to the Text Adventure!\n";
        while (playerHealth > 0) {
            displayGameState(currentLocation, playerHealth, enemiesRemaining);
            cout << "What do you want to do?\n"
                 << "1. Explore further\n"
                 << "2. Fight enemies\n"
                 << "3. Check inventory\n"
                 << "4. Quit\n";
            int choice;
            cin >> choice;
            switch (choice) {
                case 1:
                    currentLocation = "Deeper into the Forest";
                    cout << "You venture deeper into the forest.\n";
                    break;
                case 2:
                    combat(playerHealth, enemiesRemaining);
                    if (playerHealth > 0 && enemiesRemaining <= 0) {
                        enemiesRemaining = getRandomNumber(3, 8);
                    }
                    break;
                case 3:
                    cout << "Inventory is currently empty (feature not implemented).\n";
                    break;
                case 4:
                    cout << "Thanks for playing!\n";
                    return 0;
                default:
                    cout << "Invalid choice.\n";
            }
        }
        cout << "Game Over.\n";
    } else {
        cout << "Invalid game choice.\n";
    }
    return 0;
}