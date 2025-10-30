#ifndef Functions_H
#define Functions_H
#include "Constants.h"

//Combat formulas
inline bool ArmourToRangedNullification(const int rangedPower, const int armourPower) {
    bool result = false;
    if (armourToRangedNullifierPossible + (rangedPower - armourToRangedNullifierMinimumRPower) /
        armourToRangedNullifierMInFormula > armourPower) {
        result = true;
    }
    return result;
}

inline bool RangedToMeleeInstantWin(const int rangedPower, const int meleePower, const int otherRangedPower) {
    bool result = false;
    if (rangedPower > meleePower * rangedToMeleeOverpowerRequiredMultiplier + rangedToMeleeOverpowerFixedAddition &&
        rangedPower >= otherRangedPower) {
        result = true;
    }
    return result;
}

inline bool RangedToMeleeDebuff(const int rangedPower, const int meleePower) {
    bool result = false;
    if (rangedPower > meleePower * rangedToMeleeDebuffRequiredMultiplier + rangedToMeleeDebuffFixedAddition) {
        result = true;
    }
    return result;
}

/*inline bool RangedToMeleeDebuff(const int rangedPower, const int meleePower) {
    bool result = false;
    if (rangedPower > (meleePower))

    return result;
}*/

//Function to display FTXUI coloured text
inline void OutputFTXUIText(const std::string &textToOutput, ftxui::Color textColor) {
    using namespace ftxui;
    auto document = paragraph(textToOutput) | color(textColor);
    auto screen = Screen::Create(Dimension::Fit(document, true));
    Render(screen, document);
    screen.Print();
    //This is basically a way to output either a newline or a space after the beautified text.
    //As a consequence, all passed strings should end in one of those :)
    if (textToOutput[textToOutput.size() - 2] == '\n') {
        std::cout << '\n'; //for the cases where I want 2 newlines
    }
    std::cout << textToOutput[textToOutput.size() - 1];
}

//Function to sanitize user input a tad better
inline void sanitizeInputMore(auto thingToRead) {
    while (std::cin.fail()) {
        std::cin.clear();
        std::cin.ignore();
        std::cin >> thingToRead;
    }
}

#endif
