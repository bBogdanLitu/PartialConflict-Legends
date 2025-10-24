#ifndef Constants_H
#define Constants_H
#include <string>

constexpr int warlordMinimumGenerals = 66;
constexpr int emperorMinimumGenerals = 30;
//How much the overall is divided by when supporting an army / being attacked
constexpr int garrisonOverallBoostContribution = 4;
constexpr int armyGeneralsMaximumIndex = 2;
const inline std::string welcomeText =
        "Welcome to Partial Conflict: Legends!\nThe game will commence after a few initial choices\n";
const inline std::string balanceCheckText = "Would you like to see General types? (1/0)\n";
const inline std::string enterToContinueText = "\nHit ENTER when you want to continue!\n";
const inline std::string warlordCountWarningText =
        "Your generals.json doesn't meet the expectations: 66 Generals for Warlords (type 3)";
const inline std::string emperorCountWarningText =
        "Your generals.json doesn't meet the expectations: 30 Generals for the Emperor (type 4)";
const inline std::string beginningGeneralText =
                "You will now be prompted to choose your starting General from the following list:\n";
const inline std::string starterPreChoiceText =
                "Type the number of the wanted General (some might have hidden bonuses!):\n";
const inline std::string starterPostChoiceText =
                "This is your first settlement. Defend it well!\n";
const inline std::string starterPreTutorial =
                "The game will now begin. Good luck!\n\n";
const inline std::string tutorialFirstDefenceText =
                "Your uprising was noticed by the nearby warlord! Resist his attack and prove your worth.\n\n";
const inline std::string settlementBesiegeFailedText =
                " was defended!\n";
const inline std::string settlementBesiegeSuccessText =
                " was captured!\n";
const inline std::string incomingAttackText =
                "Prepare yourself! An enemy army is approaching:\n";
const inline std::string settlementStationedArmyText =
                "\nThis is your stationed army:\n";
const inline std::string settlementNoStationedArmyText =
                "The battle will be led by your garrisoned troops...\n";
const inline std::string chooseBattleOrderText =
    "\nEnter indexes from your army according to the order you want the generals to fight in:\n";


#endif
