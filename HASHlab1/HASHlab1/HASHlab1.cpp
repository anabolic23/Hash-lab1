#include <iostream>
#include <string>
#include <random>
#include <algorithm>
#include <iomanip>
#include <unordered_map>
#include "include/lsh.h"

using namespace std;

//Operations with message
string appendNumber(const string& message, int number) {
    return message + to_string(number);
}

string insertRandomSymbols(const string& surname, const string& name, const string& patronymic, int symbolCount) {
    random_device rd;
    mt19937 gen(rd());
    uniform_int_distribution<> dis(33, 126);

    string randomSymbols1;
    string randomSymbols2;
    string randomSymbols3;
    string randomSymbols4;

    for (int i = 0; i < symbolCount; ++i) {
        randomSymbols1 += static_cast<char>(dis(gen));
        randomSymbols2 += static_cast<char>(dis(gen));
        randomSymbols3 += static_cast<char>(dis(gen));
        randomSymbols4 += static_cast<char>(dis(gen));

    }
    return randomSymbols1 + surname + randomSymbols2 + name + randomSymbols3 + patronymic + randomSymbols4;
}

string modifyMessageRandomly(string message) {
    random_device rd;
    mt19937 gen(rd());
    uniform_int_distribution<> dis(0, message.length() - 1);  
    uniform_int_distribution<> charDis(32, 126); 

    int pos = dis(gen);
    char originalChar = message[pos];

    char newChar;
    do {
        newChar = static_cast<char>(charDis(gen));
    } while (newChar == originalChar); 

    message[pos] = newChar;
    return message;
}

//Auxiliary functions for working with hash function
bool compareHashes(const lsh_u8* hash1, const lsh_u8* hash2, size_t length) {
    for (size_t i = 0; i < length; i++) {
        if (hash1[31 - i] != hash2[31 - i]) {
            return false;
        }
    }
    return true;
}

void computeHash(const string& message, lsh_u8* hashResult) {
    lsh_u8* data = reinterpret_cast<lsh_u8*>(const_cast<char*>(message.c_str()));
    lsh_digest(LSH_TYPE_256, data, message.size() * 8, hashResult);
}

void printHash(const lsh_u8* hash) {
    for (int i = 0; i < 32; i++) {
        cout << hex << setfill('0') << setw(2) << (int)hash[i];
    }
    cout << endl;
}

//Attacks
int preimageAttackByNumbers(const string& originalMessage, const lsh_u8* originalHash) {
    int counter = 1;
    string attackMessage;
    lsh_u8 currentHash[32];

    while (true) {
        attackMessage = appendNumber(originalMessage, counter);
        computeHash(attackMessage, currentHash);
        
        /*if (counter < 31) {
            cout << "Iteration " << dec << counter << " | Message: " << attackMessage << " | Hash: ";
            printHash(currentHash);

        }*/
            
        if (compareHashes(originalHash, currentHash, 2)) {
            cout << "Preimage Attack by Numbers Success!" << endl;
            cout << "Original Message: " << attackMessage << endl;  
            cout << "Hash: ";
            printHash(currentHash);
            cout << "Iteration: " << dec << counter << endl;

            /*return attackMessage;*/
            return counter;
        }

        counter++;
    }

}


int preimageAttackByRandomModifications(const string& originalMessage, const lsh_u8* originalHash) {
    int counter = 1;
    string attackMessage = originalMessage;
    lsh_u8 currentHash[32];

    while (true) {
        attackMessage = modifyMessageRandomly(attackMessage);
        computeHash(attackMessage, currentHash);

        /*if (counter < 31) {
            cout << "Iteration " << dec << counter << " | Message: " << attackMessage << " | Hash: ";
            printHash(currentHash);

        }*/

        if (compareHashes(originalHash, currentHash, 2)) {
            cout << "Preimage Attack by Random Modifications Success!" << endl;
            cout << "Original Message: " << attackMessage << endl;
            cout << "Hash: ";
            printHash(currentHash);
            cout << "Iteration: " << dec << counter << endl;

            /*return attackMessage;*/
            return counter;
        }
        counter++;
    }
}

int birthdayAttackByNumbers(const string& message) {
    unordered_map<string, string> hashMap;
    int counter = 1;
    string modifiedMessage;
    lsh_u8 currentHash[32];

    while (true) {
        modifiedMessage = appendNumber(message, counter);
        computeHash(modifiedMessage, currentHash);

        /*if (counter < 31) {
            cout << "Iteration " << dec << counter << " | Message: " << modifiedMessage << " | Hash: ";
            printHash(currentHash);

        }*/

        string hashString(reinterpret_cast<char*>(currentHash), 4);

        if (hashMap.count(hashString)) {
            cout << "Collision found!" << endl;
            cout << "Message 1: " << hashMap[hashString] << endl;
            cout << "Message 2: " << modifiedMessage << endl;
            cout << "Hash: ";
            printHash(currentHash);
            cout << "Iteration: " << dec << counter << endl;

            /*return modifiedMessage;*/
            return counter;
        }

        hashMap[hashString] = modifiedMessage;
        counter++;
    }
}

int birthdayAttackByRandomModifications(const string& message) {
    unordered_map<string, string> hashMap;
    string modifiedMessage = message;
    lsh_u8 currentHash[32];
    int counter = 1;

    while (true) {
        modifiedMessage = modifyMessageRandomly(modifiedMessage);
        computeHash(modifiedMessage, currentHash);

        /*if (counter < 31) {
            cout << "Iteration " << dec << counter << " | Message: " << attackMessage << " | Hash: ";
            printHash(currentHash);

        }*/

        string hashString(reinterpret_cast<char*>(currentHash), 4);

        if (hashMap.count(hashString)) {
            cout << "Collision found!" << endl;
            cout << "Message 1: " << hashMap[hashString] << endl;
            cout << "Message 2: " << modifiedMessage << endl;
            cout << "Hash: ";
            printHash(currentHash);
            cout << "Iteration: " << dec << counter << endl;

            /*return modifiedMessage;*/
            return counter;
        }

        hashMap[hashString] = modifiedMessage;
        counter++;
    }
}

void runAttackExperiment(int attackType, int variant, int runs, const lsh_u8* originalHash) {
    string surname = "lopatetskyi";
    string name = "mykhailo";
    string patronymic = "volodymyrovych";

    string message = surname + " " + name + " " + patronymic;

    vector<int> iterations(runs);
    for (int i = 0; i < runs; ++i) {
        string randomMessage = insertRandomSymbols(surname, name, patronymic, 5);
        int count = 0;

        if (attackType == 1) { // Preimage Attack
            if (variant == 1) {
                count = preimageAttackByNumbers(randomMessage, originalHash);
            }
            else {
                count = preimageAttackByRandomModifications(randomMessage, originalHash);
            }
        }
        else if (attackType == 2) { // Birthday Attack
            if (variant == 1) {
                count = birthdayAttackByNumbers(randomMessage);
            }
            else {
                count = birthdayAttackByRandomModifications(randomMessage);
            }
        }

        iterations[i] = count; 

    }

    double mean = 0, variance = 0;
    for (int it : iterations) {
        mean += it;
    }
    mean /= runs;

    for (int it : iterations) {
        variance += (it - mean) * (it - mean);
    }
    variance /= runs;

    double confidenceInterval = 1.96 * sqrt(variance / runs); 

    cout << "Mean: " << mean << ", Variance: " << variance
        << ", 95% Confidence Interval: ["
        << mean - confidenceInterval << ", " << mean + confidenceInterval << "]" << endl;
}

int main() {
    lsh_u8 originalHash[32];

    runAttackExperiment(1, 1, 100, originalHash); 
    /*runAttackExperiment(1, 2, 100, originalHash); 
    runAttackExperiment(2, 1, 100, originalHash);
    runAttackExperiment(2, 2, 100, originalHash);*/

    return 0;
}


