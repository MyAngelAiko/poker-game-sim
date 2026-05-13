#include <iostream>
#include <vector>
#include <string>
#include <random>
#include <algorithm>
#include <filesystem>
#include <ranges>
#include <bits/atomic_lockfree_defines.h>
#include <bits/std_thread.h>
#include <string>

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
    std::vector<std::vector<Card>> players;
    std::pmr::vector<Card> cards;
    std::pmr::vector<Card> playerCards;
    std::pmr::vector<Card> flopCards;
    std::vector<Card> allCards;
    std::vector<long long> handStrength;
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
    int winner = {0};
    int playerWins = {0};
    int totalRuns = {0};
public:
    Deck(int numPlayers) {
        players.resize(numPlayers);

        for (int i = 0; i < 4; i++) {
            for (int j = 0; j < 13; j++) {
                cards.push_back(Card{
                    static_cast<Suit>(i),
                    static_cast<Rank>(j + 1)
                });
            }
        }
    }

    Card parseCard(const std::string& input) {
        char r = input[0];
        char s = input[1];

        Rank rank;
        Suit suit;

        if (r == '2') rank = Rank::Two;
        else if (r == '3') rank = Rank::Three;
        else if (r == '4') rank = Rank::Four;
        else if (r == '5') rank = Rank::Five;
        else if (r == '6') rank = Rank::Six;
        else if (r == '7') rank = Rank::Seven;
        else if (r == '8') rank = Rank::Eight;
        else if (r == '9') rank = Rank::Nine;
        else if (r == 'T') rank = Rank::Ten;
        else if (r == 'J') rank = Rank::Jack;
        else if (r == 'Q') rank = Rank::Queen;
        else if (r == 'K') rank = Rank::King;
        else if (r == 'A') rank = Rank::Ace;

        if (s == 'H') suit = Suit::Hearts;
        else if (s == 'C') suit = Suit::Clubs;
        else if (s == 'D') suit = Suit::Diamonds;
        else if (s == 'S') suit = Suit::Spades;

        return Card{ suit, rank };
    }

    void removeCardFromDeck(const Card& card) {
        for (int i = 0; i < cards.size(); i++) {
            if (cards[i].rank == card.rank && cards[i].suit == card.suit) {
                cards.erase(cards.begin() + i);
                return;
            }
        }
    }

    void inputMyCards() {
        std::cout << "Enter your two cards (eg: AH or 9D): ";
        std::string c1, c2;
        std::cin >> c1 >> c2;
        Card card1 = parseCard(c1);
        Card card2 = parseCard(c2);

        players[0].push_back(card1);
        players[0].push_back(card2);
        removeCardFromDeck(card1);
        removeCardFromDeck(card2);

        for (const auto& card: players[0]) {
            playerCards.push_back(card);
        }
    }

    void inputFlop() {
        std::cout << "Enter the flop cards! (eg: AH or 9D)";

        for (int i = 0; i < 5; i++) {
            std::string input;
            std::cin >> input;
            flopCards.push_back(parseCard(input));
            Card card = parseCard(input);
            removeCardFromDeck(card);
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

    void dealCards(bool showOutput = true) {
        for (auto& player: players) {
            player.clear();
        }

        if (playerCards.empty() != true) {
            for (int i = 0; i < 2; i++) {
                players[0].push_back(playerCards[i]);
            }
        }

        if (players[0].size() >= 2) {
            for (int round = 0; round < 2; round++) {
                for (int i = 1; i < players.size(); i++) {
                    players[i].push_back(dealCard());
                }
            }
        }
        else {
            for (int round = 0; round < 2; round++) {
                for (auto& player : players) {
                    player.push_back(dealCard());
                }
            }
        }
        if (showOutput) {
            std::cout << "Your cards are: \n";
            for (const auto& card : playerCards) {
                std::cout << rankToString(card.rank) << " of " << suitToString(card.suit) << std::endl;
            }
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

    void compareToFlop(bool showOutput = true) {
        winner = {0};
        handStrength.clear();
        for (int p = 0; p < players.size(); p++) {
            pair = false;
            pairedRank = {""};
            threeOfAKind = false;
            threeOfAKindRank = {""};
            fourOfAKind = false;
            fourOfAKindRank = {""};
            straight = false;
            straightRank = {""};
            flush = false;
            highcardRank = {""};
            pairCount = {0};

            allCards.clear();

            for (const auto& card : players[p]) {
                allCards.push_back(card);
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

            long long strength = finalResult();
            handStrength.push_back(strength);
            if (showOutput) {
                std::cout <<  "\nPlayer " << p + 1 << ": ";
                printResult(strength);
                std::cout << " with a ";
                for (const auto& card : players[p]) {
                    std::cout << rankToString(card.rank) << " of " << suitToString(card.suit) << " and ";
                }
            }
        }

        int best = 0;

        for (int i = 0; i < handStrength.size(); i++) {
            if (handStrength[i] > handStrength[winner]) {
                winner = i;
            }
        }
        winner++;
        if (showOutput) {
            std::cout << "\nThe winner is player " << winner << " with a ";
            printResult(handStrength[winner - 1]);
        }
    }

    void tally() {
        if (winner == 1) {
            playerWins++;
            totalRuns++;
        }
        else {
            totalRuns++;
        }
    }

    void winChance() {
        double winPercentage = ((double) playerWins / (double) totalRuns) * 100;
        std::cout << "Your win percentage is: " << winPercentage << "%" << std::endl;
    }

    long long finalResult() const {
        long long value = 0;
        if (fourOfAKind) value = 8;
        else if (threeOfAKind && pair) value = 7;
        else if (flush) value = 6;
        else if (straight) value = 5;
        else if (threeOfAKind) value = 4;
        else if (pairCount >= 2) value = 3;
        else if (pair) value = 2;
        else value = 1;

        value *= 1000000000;
        long long multiplier = 1000000;

        for (int i = allCards.size() - 1; i >= 0; i--) {
            value += static_cast<int>(allCards[i].rank) * multiplier;
            multiplier /= 10;
            if (multiplier == 0) break;
        }

        return value;
    }

    void printResult(long long strength) const {
        if (strength > 8000000000) {
            std::cout <<  "four of a kind";
        }
        else if (strength > 7000000000) {
            std::cout << "full house";
        }
        else if (strength > 6000000000) {
            std::cout << "flush";
        }
        else if (strength > 5000000000) {
            std::cout << "straight";
        }
        else if (strength > 4000000000) {
            std::cout << "three of a kind";
        }
        else if (strength > 3000000000) {
            std::cout << "two pair";
        }
        else if (strength > 2000000000) {
            std::cout << "pair";
        }
        else {
            std::cout << "high card";
        }
    }

    void resetDeck() {
        cards.clear();

        for (int i = 0; i < 4; i++) {
            for (int j = 0; j < 13; j++) {
                cards.push_back(Card{
                    static_cast<Suit>(i),
                    static_cast<Rank>(j + 1)
                });
            }
        }

        for (const auto& card : playerCards) {
            removeCardFromDeck(card);
        }

        for (const auto& card : flopCards) {
            removeCardFromDeck(card);
        }
    }
};

int main() {
    std::cout << "How many players are there?";
    int players;
    std::cin >> players;
    Deck myDeck(players);
    std::cout << "Do you want to run ts as regular or a simulation?";
    std::pmr::string modeInput;
    std::cin >> modeInput;
    if (modeInput == "regular") {
        myDeck.shuffleDeck();
        myDeck.dealCards();
        myDeck.flop();
        myDeck.compareToFlop();
        return 0;
    }
    else if (modeInput == "simulation") {
        myDeck.inputMyCards();
        myDeck.inputFlop();
        for (int i = 0; i < 100; i++) {
            myDeck.resetDeck();
            myDeck.shuffleDeck();
            myDeck.dealCards(false);
            myDeck.compareToFlop(false);
            myDeck.tally();
        }
        myDeck.winChance();
        return 0;
    }
    else {
        std::cout << "Invalid input";
    }

}
