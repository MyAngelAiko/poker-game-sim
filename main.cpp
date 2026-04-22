#include <iostream>
#include <vector>
#include <string>
#include <random>
#include <algorithm>
#include <filesystem>
#include <ranges>
#include <bits/std_thread.h>

enum class Suit {
    Hearts,
    Clubs,
    Diamonds,
    Spades
};

enum class Rank {
    Two, Three, Four, Five, Six, Seven, Eight, Nine, Ten, Jack, Queen, King, Ace
};

struct Card {
    Suit suit;
    Rank rank;
};

std::string suitToString(Suit suit) {
    switch (suit) {
        case Suit::Hearts: return "Hearts";
        case Suit::Clubs: return "Clubs";
        case Suit::Diamonds: return "Diamonds";
        case Suit::Spades: return "Spades";
    }
    return "error";
}

std::string rankToString(Rank rank) {
    switch (rank) {
        case Rank::Two: return "Two";
        case Rank::Three: return "Three";
        case Rank::Four: return "Four";
        case Rank::Five: return "Five";
        case Rank::Six: return "Six";
        case Rank::Seven: return "Seven";
        case Rank::Eight: return "Eight";
        case Rank::Nine: return "Nine";
        case Rank::Ten: return "Ten";
        case Rank::Jack: return "Jack";
        case Rank::Queen: return "Queen";
        case Rank::King: return "King";
        case Rank::Ace: return "Ace";
    }
    return "error";
}

class Deck {
private:
    std::pmr::vector<Card> cards;
    std::pmr::vector<Card> playerCards;
    std::pmr::vector<Card> flopCards;
    bool pair = false;
    std::string pairedRank = {""};
public:
    Deck() {
        for (int i = 0; i < 4; i++) {
            for (int j = 0; j < 13; j++) {
                cards.push_back(Card{
                    static_cast<Suit>(i),
                    static_cast<Rank>(j)
                });
            }
        }
    }

    void printDeck() const {
        for (int i = 0; i < cards.size(); i++) {
            std::cout << rankToString(cards[i].rank) << " of " << suitToString(cards[i].suit) << std::endl;
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

    Card dealCard() {
        Card top = cards.back();
        cards.pop_back();
        return top;
    }

    void dealCards() {
        playerCards.push_back(dealCard());
        playerCards.push_back(dealCard());
        std::cout << "Your cards are: \n";
        for (const auto& card : playerCards) {
            std::cout << rankToString(card.rank) << " of " << suitToString(card.suit) << std::endl;
        }
    }

    void flop() {
        for (int i = 0; i < 5; i++) {
            flopCards.push_back(dealCard());
        }
        std::cout << "Flop cards are: \n";
        for (const auto& card : flopCards) {
            std::cout << rankToString(card.rank) << " of " << suitToString(card.suit) << std::endl;
        }
    }

    void compareToFlop() {

        std::vector<Card> allCards;
        for (int i = 0; i < 5; i++) {
            allCards.push_back(playerCards[i]);
        }
        for (int j = 0; j < 2; j++) {
            allCards.push_back(flopCards[j]);
        }
        for (int x = 0; x < allCards.size() - 1; x++) {
            for (int y = 0; y < allCards.size() - x - 1; y++) {
                if (allCards[y].rank > allCards[y + 1].rank) {
                    std::swap(allCards[y], allCards[y + 1]);
                }
            }
        }

        for (int z = 0; z < allCards.size() - 1; z++) {
            std::cout << rankToString(allCards[z].rank) << " of " << suitToString(allCards[z].suit) << std::endl ;
        }


        for (const auto& playerCard : playerCards) {
            for (const auto& flopCard : flopCards) {
                if (playerCard.rank == flopCard.rank) {
                    pairedRank = rankToString(playerCard.rank);
                    pair = true;
                    return;
                }
            }
        }
    }

    void finalResult() const {
        if (pair == true) {
            std::cout << "You have a matching pair of " << pairedRank << "s" << std::endl;
        }
    }
};

int main() {
    Deck myDeck;
    myDeck.shuffleDeck();
    myDeck.dealCards();
    myDeck.flop();
    myDeck.compareToFlop();
    myDeck.finalResult();
    return 0;
}
