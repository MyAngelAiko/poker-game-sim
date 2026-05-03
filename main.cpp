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
    std::vector<std::vector<Card>> players;
    std::pmr::vector<Card> cards;
    std::pmr::vector<Card> playerCards;
    std::pmr::vector<Card> flopCards;
    std::vector<Card> allCards;
    std::vector<int> handStrength;
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
        for (int round = 0; round < 2; round++) {
            for (auto& player: players) {
                player.push_back(dealCard());
            }
        }

        for (const auto& card: players[0]) {
            playerCards.push_back(card);
        }

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

            int strength = finalResult();
            handStrength.push_back(strength);
            std::cout <<  "\nPlayer " << p + 1 << ": ";
            printResult(strength);
          }

      }

    int finalResult() const {
        if (fourOfAKind) return 8;
        else if (threeOfAKind && pair) return 7;
        else if (flush) return 6;
        else if (straight) return 5;
        else if (threeOfAKind) return 4;
        else if (pairCount >= 2) return 3;
        else if (pair) return 2;
        else return 1;
    }

    void printResult(int strength) const {
        switch (strength) {
            case 8:
                std::cout << "Four of a kind";
                break;
            case 7:
                std::cout << "Full house";
                break;
            case 6:
                std::cout << "Flush";
                break;
            case 5:
                std::cout << "Straight";
                break;
            case 4:
                std::cout << "Three of a kind";
                break;
            case 3:
                std::cout << "Two Pair";
                break;
            case 2:
                std::cout << "Pair";
                break;
            case 1:
                std::cout << "High Card";
                break;
        }
    }
};

int main() {
    std::cout << "How many players are there?";
    int players;
    std::cin >> players;
    Deck myDeck(players);
    myDeck.shuffleDeck();
    myDeck.dealCards();
    myDeck.flop();
    myDeck.compareToFlop();
    return 0;
}
