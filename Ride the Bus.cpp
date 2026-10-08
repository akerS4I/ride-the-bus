#include <iostream>
#include <vector>
#include <string>
#include <algorithm>
#include <random>
#include <conio.h>
#include <windows.h>

using namespace std;

constexpr const char* RESET = "\033[0m";
constexpr const char* RED = "\033[31m";
constexpr const char* GREEN = "\033[32m";
constexpr const char* YELLOW = "\033[33m";
constexpr const char* CYAN = "\033[36m";
constexpr const char* BOLD = "\033[1m";
constexpr const char* DIM = "\033[2m";

enum class Suit {
    Hearts,
    Diamonds,
    Clubs,
    Spades
};

struct Card {
    int value;
    Suit suit;

    string valueString() const {
        if (value <= 10)
            return to_string(value);

        switch (value) {
        case 11: return "J";
        case 12: return "Q";
        case 13: return "K";
        case 14: return "A";
        default: return "?";
        }
    }

    string suitString() const {
        switch (suit) {
        case Suit::Hearts:   return "Hearts";
        case Suit::Diamonds: return "Diamonds";
        case Suit::Clubs:    return "Clubs";
        case Suit::Spades:   return "Spades";
        default:             return "";
        }
    }

    bool isRed() const {
        return suit == Suit::Hearts || suit == Suit::Diamonds;
    }

    void print() const {
        const char* color = isRed() ? RED : RESET;

        cout << color
            << valueString() << " of " << suitString()
            << RESET;
    }
};

vector<Card> createDeck() {
    vector<Card> deck;

    for (int suit = 0; suit < 4; suit++) {
        for (int value = 2; value <= 14; value++) {
            deck.push_back({
                value,
                static_cast<Suit>(suit)
                });
        }
    }

    return deck;
}

void enableAnsiColors() {
    HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);

    DWORD dwMode = 0;

    if (GetConsoleMode(hOut, &dwMode)) {
        dwMode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;
        SetConsoleMode(hOut, dwMode);
    }
}

void clearScreen() {
    cout << "\033[2J\033[H";
}

void printHeader(int attempts, int wins) {
    cout << CYAN << BOLD;
    cout << "============================================\n";
    cout << "                 RIDE THE BUS\n";
    cout << "============================================\n";
    cout << RESET;

    cout << DIM
        << "Attempt: " << attempts
        << "    Wins: " << wins
        << RESET << "\n\n";
}

int selectOption(const vector<string>& options) {
    int selected = 0;

    while (true) {
        // Move to the beginning of the menu.
        // We only redraw the menu itself.
        cout << "\r";

        for (int i = 0; i < static_cast<int>(options.size()); i++) {
            cout << "\033[2K\r";

            if (i == selected) {
                cout << "  " << CYAN << BOLD << "> "
                    << options[i] << RESET;
            }
            else {
                cout << "    " << options[i];
            }

            cout << "\n";
        }

        // Move cursor back to the first menu line.
        cout << "\033[" << options.size() << "A";

        int key = _getch();

        // Arrow keys return 224 or 0 first.
        if (key == 224 || key == 0) {
            key = _getch();

            if (key == 72) { // Up
                selected--;

                if (selected < 0)
                    selected = static_cast<int>(options.size()) - 1;
            }
            else if (key == 80) { // Down
                selected++;

                if (selected >= static_cast<int>(options.size()))
                    selected = 0;
            }
        }
        else if (key == 13) { // Enter
            // Move cursor below the menu.
            cout << "\033[" << options.size() << "B";
            return selected;
        }
    }
}

void waitForEnter() {
    cout << "\n" << DIM << "Press ENTER to continue..." << RESET;
    while (_getch() != 13) {
    }
}

void printStage(int number, const string& name) {
    cout << CYAN << BOLD
        << "[" << number << "/4] " << name
        << RESET << "\n\n";
}

void printLoss(const string& reason) {
    cout << "\n" << YELLOW << BOLD
        << "YOU LOST!\n"
        << RESET;

    cout << reason << "\n";
}

int main() {
    enableAnsiColors();

    random_device rd;
    mt19937 rng(rd());

    int attempts = 0;
    int wins = 0;

    bool playAgain = true;

    while (playAgain) {
        attempts++;

        vector<Card> deck = createDeck();
        shuffle(deck.begin(), deck.end(), rng);

        int currentCard = 0;

        clearScreen();
        printHeader(attempts, wins);

        // color
        printStage(1, "COLOR");

        cout << "Guess the color of the first card:\n\n";

        int colorChoice = selectOption({
            "Red",
            "Black"
            });

        Card first = deck[currentCard++];

        cout << "\n\nFirst card: ";
        first.print();
        cout << "\n";

        bool correctColor =
            (colorChoice == 0 && first.isRed()) ||
            (colorChoice == 1 && !first.isRed());

        if (!correctColor) {
            string guessedColor = colorChoice == 0 ? "Red" : "Black";
            string actualColor = first.isRed() ? "Red" : "Black";

            printLoss(
                "You guessed " + guessedColor +
                ", but the card was " + actualColor + "."
            );

            waitForEnter();
            continue;
        }

        cout << GREEN << "Correct!\n" << RESET;
        waitForEnter();

        // higher / lower
        clearScreen();
        printHeader(attempts, wins);

        printStage(2, "HIGHER / LOWER");

        cout << "First card: ";
        first.print();
        cout << "\n\n";

        cout << "Is the second card higher or lower than the first?\n\n";

        int higherLowerChoice = selectOption({
            "Higher",
            "Lower"
            });

        Card second = deck[currentCard++];

        cout << "\n\nSecond card: ";
        second.print();
        cout << "\n";

        if (first.value == second.value) {
            printLoss(
                "Both cards have the same value (" +
                first.valueString() + ")."
            );

            waitForEnter();
            continue;
        }

        bool correctHigherLower =
            (higherLowerChoice == 0 && second.value > first.value) ||
            (higherLowerChoice == 1 && second.value < first.value);

        if (!correctHigherLower) {
            string guessed =
                higherLowerChoice == 0 ? "Higher" : "Lower";

            string actual =
                second.value > first.value ? "higher" : "lower";

            printLoss(
                "You guessed " + guessed +
                ", but the second card was actually " + actual +
                " than the first card."
            );

            waitForEnter();
            continue;
        }

        cout << GREEN << "Correct!\n" << RESET;
        waitForEnter();

        // inside / outside
        clearScreen();
        printHeader(attempts, wins);

        printStage(3, "INSIDE / OUTSIDE");

        cout << "First card:  ";
        first.print();

        cout << "\nSecond card: ";
        second.print();

        int low = min(first.value, second.value);
        int high = max(first.value, second.value);

        cout << "\n\n";

        cout << "Is the third card inside or outside "
            << "the range between them?\n\n";

        int insideOutsideChoice = selectOption({
            "Inside",
            "Outside"
            });

        Card third = deck[currentCard++];

        cout << "\n\nThird card: ";
        third.print();
        cout << "\n";

        if (third.value == low || third.value == high) {
            printLoss(
                "The third card matched one of the boundary values. "
                "It was neither strictly inside nor outside the range."
            );

            waitForEnter();
            continue;
        }

        bool isInside =
            third.value > low && third.value < high;

        bool correctInsideOutside =
            (insideOutsideChoice == 0 && isInside) ||
            (insideOutsideChoice == 1 && !isInside);

        if (!correctInsideOutside) {
            string guessed =
                insideOutsideChoice == 0 ? "Inside" : "Outside";

            string actual =
                isInside ? "Inside" : "Outside";

            printLoss(
                "You guessed " + guessed +
                ", but the card was actually " + actual + "."
            );

            waitForEnter();
            continue;
        }

        cout << GREEN << "Correct!\n" << RESET;
        waitForEnter();

        // suit
        clearScreen();
        printHeader(attempts, wins);

        printStage(4, "SUIT");

        cout << "Guess the suit of the fourth card:\n\n";

        int suitChoice = selectOption({
            "Hearts",
            "Diamonds",
            "Clubs",
            "Spades"
            });

        Card fourth = deck[currentCard++];

        cout << "\n\nFourth card: ";
        fourth.print();
        cout << "\n";

        bool correctSuit =
            static_cast<int>(fourth.suit) == suitChoice;

        if (!correctSuit) {
            string guessedSuit;

            switch (suitChoice) {
            case 0: guessedSuit = "Hearts"; break;
            case 1: guessedSuit = "Diamonds"; break;
            case 2: guessedSuit = "Clubs"; break;
            case 3: guessedSuit = "Spades"; break;
            }

            printLoss(
                "You guessed " + guessedSuit +
                ", but the card was " +
                fourth.suitString() + "."
            );

            waitForEnter();
            continue;
        }

        wins++;

        clearScreen();
        printHeader(attempts, wins);

        cout << GREEN << BOLD;
        cout << "============================================\n";
        cout << "           YOU RODE THE BUS!\n";
        cout << "============================================\n";
        cout << RESET << "\n";

        cout << "All four guesses were correct.\n";
        cout << "You successfully completed the round!\n";

        cout << "\n";

        int playChoice = selectOption({
            "Play again",
            "Quit"
            });

        playAgain = (playChoice == 0);
    }

    clearScreen();

    cout << CYAN << BOLD;
    cout << "============================================\n";
    cout << "                 GOODBYE!\n";
    cout << "============================================\n";
    cout << RESET << "\n";

    cout << "Attempts: " << attempts << "\n";
    cout << "Wins:     " << wins << "\n";

    return 0;
}
