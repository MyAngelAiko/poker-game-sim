#include <iostream>
#include <vector>
#include <string>
#include <random>
#include <algorithm>
#include <filesystem>
#include <ranges>
#include <bits/atomic_lockfree_defines.h>
#include <bits/std_thread.h>

enum class Suit {
    Hearts,
    Clubs,
    Diamonds,
    Spades
};

enum class Rank {
    Two = 1, Three = 2, Four = 3, Five = 4, Six = 5, Seven = 6, Eight = 7, Nine = 8, Ten = 9, Jack = 10, Queen = 11, King = 12, Ace = 13
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
    std::vector<Card> allCards;
    bool pair = false;
    std::string pairedRank = {""};
    bool threeOfAKind = false;
    std::string threeOfAKindRank = {""};
    bool fourOfAKind = false;
    std::string fourOfAKindRank = {""};
    bool straight = false;
    std::string straightRank = {""};
    bool flush = false;
    std::string highcardRank = {""};
    int pairCount = {0};
public:
    Deck() {
        for (int i = 0; i < 4; i++) {
            for (int j = 0; j < 13; j++) {
                cards.push_back(Card{
                    static_cast<Suit>(i),
                    static_cast<Rank>(j + 1)
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
        allCards.clear();

        for (int i = 0; i < playerCards.size(); i++) {
            allCards.push_back(playerCards[i]);
        }

        for (int i = 0; i < flopCards.size(); i++) {
            allCards.push_back(flopCards[i]);
        }

        for (int i = 0; i < allCards.size() - 1; i++) {
            for (int j = 0; j < allCards.size() - i - 1; j++) {
                if (allCards[j].suit > allCards[j + 1].suit) {
                    std::swap(allCards[j], allCards[j + 1]);
                }
            }
        }

        int countFlush = 1;
        for (int i = 0; i < allCards.size() - 1; i++) {
            int currentCard = static_cast<int>(allCards[i].suit);
            int nextCard = static_cast<int>(allCards[i + 1].suit);

            if (currentCard == nextCard) {
                countFlush++;
                if (countFlush >= 5) {
                    flush = true;
                    break;
                }
            }
            else {
                countFlush = 1;
            }
        }


        for (int i = 0; i < allCards.size() - 1; i++) {
            for (int j = 0; j < allCards.size() - i - 1; j++) {
                if (allCards[j].rank > allCards[j + 1].rank) {
                    std::swap(allCards[j], allCards[j + 1]);
                }
            }
        }


        for (int i = 0; i < allCards.size() - 1; i++) {
            for (int j = 0; j < allCards.size() - i - 1; j++) {
                if (allCards[j].rank > allCards[j + 1].rank) {
                    std::swap(allCards[j], allCards[j + 1]);
                }
            }
        }

        int count = 1;
        for (int i = 0; i < allCards.size() - 1; i++) {
            int currentCard = static_cast<int>(allCards[i].rank);
            int nextCard = static_cast<int>(allCards[i + 1].rank);

            if (currentCard == nextCard) {
                continue;
            }
            else if (currentCard + 1 == nextCard) {
                count++;
                if (count >= 5) {
                    straight = true;
                    break;
                }
            }
            else {
                count = 1;
            }
        }

        std::vector<int> rankCount(13,0);
        for (const auto& card : allCards) {
            int rank = static_cast<int>(card.rank) - 1;
            rankCount[rank]++;
        }
        for (int i = 12; i >= 0; i--) {
            if (rankCount[i] == 4) {
                fourOfAKind = true;
                fourOfAKindRank = rankToString(static_cast<Rank>(i + 1));
                break;
            }
            else if (rankCount[i] == 3) {
                threeOfAKind = true;
                threeOfAKindRank = rankToString(static_cast<Rank>(i + 1));
            }
            else if (rankCount[i] == 2 && pair == false) {
                pair = true;
                pairedRank = rankToString(static_cast<Rank>(i + 1));
                pairCount++;
            }
            else if (rankCount[i] == 2) {
                pairCount++;
            }
        }
    }

    void finalResult() const {
        if (fourOfAKind == true) {
            std::cout << "You have a four of a kind of " << fourOfAKindRank << "s!";
        }
        else if (threeOfAKind == true && pair == true) {
            std::cout << "You have a full house!";
        }
        else if (flush == true) {
            std::cout << "You have a Flush!";
        }
        else if (straight == true) {
            std::cout << "You have a straight!";
        }
        else if (threeOfAKind == true) {
            std::cout << "You have a Three of a Kind of " << threeOfAKindRank << "s!";
        }
        else if (pairCount >= 2) {
            std::cout << "You have a twopair";
        }
        else if (pair == true) {
            std::cout << "You have a matching pair of " << pairedRank << "s!" << std::endl;
        }
        else {
            std::cout << "You have a high card!";
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
