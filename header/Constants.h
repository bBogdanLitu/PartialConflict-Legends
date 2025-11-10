#ifndef Constants_H
#define Constants_H
#include <string>
#include <vector>

//Usual colours
#define gameAnnouncementsColor ftxui::Color::Gold1
#define userInputExpectedColor ftxui::Color::RoyalBlue1
#define importantGameInformationColor ftxui::Color::Red
#define storyRelatedTextColor ftxui::Color::MediumVioletRed
#define enemyRelatedTextColor ftxui::Color::DarkOrange
#define allyRelatedTextColor ftxui::Color::DarkCyan
#define generalFightDefenderInfoColor ftxui::Color::SpringGreen4
#define generalFightAttackerInfoColor ftxui::Color::IndianRed
#define generalFightDefenderWinColor ftxui::Color::SpringGreen4
#define generalFightAttackerWinColor ftxui::Color::OrangeRed1
#define beautifulBlue ftxui::Color::SkyBlue3

constexpr int warlordMinimumGenerals = 66;
constexpr int emperorMinimumGenerals = 30;
//How much the overall is divided by when supporting an army / being attacked
constexpr int garrisonOverallBoostContribution = 4;
constexpr int armyGeneralsMaximumIndex = 2;
//For combat bonuses
constexpr int armourToRangedNullifierCertain = 1000;
constexpr int armourToRangedNullifierPossible = 416;
constexpr int armourToRangedNullifierMinimumRPower = 432;
constexpr int rPowerMaximumValue = 1600;
constexpr float armourToRangedNullifierMInFormula =
        (armourToRangedNullifierCertain - armourToRangedNullifierPossible) / static_cast<float>(
            rPowerMaximumValue - armourToRangedNullifierMinimumRPower);
constexpr int armourMaximumForBonusMultiplier = 200;
constexpr float meleeBonusMultiplierForLowArmour = 1.1f;
constexpr float rangedBonusMultiplierForLowArmour = 1.1f;
constexpr int rangedToMeleeOverpowerFixedAdditionBase = 100;
constexpr int rangedToMeleeOverpowerRequiredMultiplier = 2;
constexpr int rangedToMeleeOverpowerFixedAddition =
                rangedToMeleeOverpowerFixedAdditionBase * rangedToMeleeOverpowerRequiredMultiplier;
constexpr int rangedToMeleeDebuffFixedAdditionBase = 100;
constexpr float rangedToMeleeDebuffRequiredMultiplier = 1.5;
constexpr int rangedToMeleeDebuffFixedAddition =
        rangedToMeleeOverpowerFixedAdditionBase * rangedToMeleeOverpowerRequiredMultiplier;
constexpr float rangedToMeleeDebuffMultiplier = 0.9f;
constexpr float captainInitialHandicapMultiplier = 0.7f;
constexpr float captainFirstUpgradeHandicapMultiplier = 0.8f;
constexpr float captainSecondUpgradeHandicapMultiplier = 0.9f;
const inline std::string welcomeText =
        "Welcome to Partial Conflict: Legends!\nThe game will commence after a few initial choices\n";
const inline std::string balanceCheckText = "Would you like to see General types? (1/0)\n";
const inline std::string enterToContinueText = "\nHit ENTER when you want to continue!\n";
const inline std::string ftxuiConfirmationText =
        "\nDo you want to use an interactive console through FTXUI? (1 - recommended / 0 - not recommended)\n";
const inline std::string warlordCountWarningText =
        "Your generals.json doesn't meet the expectations: 66 Generals for Warlords (type 3)";
const inline std::string emperorCountWarningText =
        "Your generals.json doesn't meet the expectations: 30 Generals for the Emperor (type 4)";
const inline std::string beginningGeneralText =
        "You will now be prompted to choose your starting General from the following table:\n";
const inline std::string starterPreChoiceText =
        "Type the index of the wanted General (some might have hidden bonuses!): ";
const inline std::string starterPostChoiceText =
                "This is your first settlement. Defend it well!\n";
const inline std::string starterPreTutorial =
                "The game will now begin. Good luck!\n\n";
const inline std::string tutorialFirstDefenceText =
                "Your uprising was noticed by the nearby warlord! Resist his attack and prove your worth.\n\n";
const inline std::string tutorialFirstDefenceEndText =
        "The warlord has crushed your dreams... You must rise again and never give up!\n\n";
const inline std::string settlementBesiegeFailedText =
        " was defended!\n\n";
const inline std::string settlementBesiegeSuccessText =
        " was captured!\n\n";
const inline std::string incomingAttackText =
                "Prepare yourself! An enemy army is approaching:\n";
const inline std::string settlementStationedArmyText =
        "\nThis is your stationed army:\n";
const inline std::string settlementNoStationedArmyText =
                "The battle will be led by your garrisoned troops...\n";
const inline std::string chooseBattleOrderText =
                "\nEnter indexes from your army according to the order you want the generals to fight in:\n";
const inline std::string settlementFightRemainingAttackersText =
                "There are still attackers remaining. Defenders will attempt to defeat them.\n";
const inline std::string settlementFightNoRemainingAttackersText =
                "The defenders have defeated the attackers from the first attempt.\n";
const inline std::string settlementFightNoRemainingDefendersText =
                "The attackers have defeated the defenders from the first attempt.\n";
const inline std::vector<std::string> startingGeneralTableHeaders =
{
    "Index", "Full name", "Type", "Rarity", "Melee", "Ranged", "Armour", "Strength", "Accuracy", "Dexterity", "Overall"
};
const inline std::vector<std::string> settlementTableHeaders =
{
    "Index", "Name", "Owned by", "Garrison OVR", "Has Army", "Control Points #", "Neighbours"
};
const inline std::vector<std::string> armyTableHeaders =
{
    "Index", "Full name", "Type", "Rarity", "Melee", "Ranged", "Armour", "Strength", "Accuracy", "Dexterity", "Overall"
};
const inline std::vector<std::string> generalFightTableHeaders =
{
    "Name", "Melee Power (Eff)", "Ranged Power (Eff)", "Armour Power (Eff)"
};

#endif
