#include <iostream>
#include <vector>
#include <string>
#include <random>
#include <algorithm>
#include <bits/std_thread.h>

class Deck {
private:
    std::pmr::vector<std::string> cards;
public:
    Deck() {
        std::pmr::vector<std::string> suits = {"Hearts", "Clubs", "Diamonds", "Spades"};
        std::pmr::vector<std::string> ranks = {"2", "3", "4", "5", "6", "7", "8", "9", "10","Jack", "Queen", "King", "Ace"};

        for (int i = 0; i < suits.size(); i++) {
            for (int j = 0; j < ranks.size(); j++) {
                cards.push_back(ranks[j] + suits[i]);
            }
        }
    }

    void printDeck() {
        for (int i = 0; i < cards.size(); i++) {
            std::cout << cards[i] << std::endl;
        }
    }

    void shuffleDeck() {
        std::random_device rd;
        std::mt19937 g(rd());

        for (int i = cards.size() - 1; i > 0; i--) {
            std::uniform_int_distribution<int> dist(0, i);
            int j = dist(g);
            std::swap(cards[i], cards[j]);
        }
    }
    //I decided to use a Fisher-Yates shuffle over just randomly picking cards from an unshuffled deck cuz it felt cooler lol


};

int main() {
    Deck myDeck;
    myDeck.shuffleDeck();
    myDeck.printDeck();
}
