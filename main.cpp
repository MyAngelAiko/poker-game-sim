#include <iostream>
#include <vector>
#include <string>

class deck {
private:
    std::pmr::vector<std::string> cards;
public:
    deck() {
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


};

int main() {
    deck deck;
    deck.printDeck();
}
