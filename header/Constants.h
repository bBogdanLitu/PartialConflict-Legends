#ifndef Constants_H
#define Constants_H
#include <string>
#include <vector>
#include <memory>

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
#define beautifulGreen ftxui::Color::SpringGreen3Bis
#define beautifulOrange ftxui::Color::DarkOrange
#define susPink ftxui::Color::MistyRose3
#define kaki ftxui::Color::Khaki3
#define weirdPurple ftxui::Color::LightSlateBlue
#define olive ftxui::Color::Gold3Bis
#define grayYellow ftxui::Color::Wheat4
#define honeydew ftxui::Color::Honeydew2
#define backgroundGrey ftxui::Color::Grey7

//Battle constants

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

//Debuffs for captains

constexpr float captainInitialHandicapMultiplier = 0.7f;
constexpr float captainFirstUpgradeHandicapMultiplier = 0.8f;
constexpr float captainSecondUpgradeHandicapMultiplier = 0.9f;

//TEXTS

///non-ftxui mostly / pre-game start
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

///ftxui
const inline std::string gameTitle = "PARTIAL CONFLICT: LEGENDS";

//Pre-Game UI Buttons
const inline std::string startGameButtonText = "Start a New Game";
const inline std::string loadGameButtonText = "Load Progress";

//Unlocking Buttons Announcements
const inline std::string checkSettlementsButtonUnlockText = "You should check out your settlements now!";
const inline std::string checkEnemyIntentsButtonUnlockText = "Now take a look at your enemy's intents!";

//Story
const inline std::string storyBeginning1 = "It can't be...";
const inline std::string storyBeginning2 = "You were supposed to make a difference!";
const inline std::string storyBeginning3 = "I won't allow you to escape this easily.";
const inline std::string storyBeginning4 = "Rise again and show everyone why I chose you!";
const inline std::string checkSettlementsGameLostText = "Wait... There are none! YOU LOST?!";
const inline std::string gameWinText = "You won! (temporarily)";

//Captions / extra information
const inline std::string modifyArmyChooseStartersText = "Choose from this list of starters:";
const inline std::string modifyArmyChooseNonStartersText = "Or from this list of non-starters:";
const inline std::string modifyArmyChooseCaptainText = "Or from this list of captains:";
const inline std::string cancelOptionChosenText = "Operation canceled";
const inline std::string modifyArmyInformWhichText = "You are modifying this army:";
const inline std::string moveArmySettlementNeighbourInformationText = "Neighbours of settlement ";
const inline std::string moveArmyNeighbourChooseWhichText = "Now choose the index of the neighbour you want to move to.";
const inline std::string moveArmyWhereAttackInformationText = "This settlement isn't yours. This action will trigger an attack...";
const inline std::string checkSettlementsCaptionText = "CHECKING SETTLEMENTS";
const inline std::string checkEnemyIntentsCaptionText = "CHECKING ENEMY INTENTS";
const inline std::string moveArmyCaptionText = "MOVING ARMY";
const inline std::string checkSettlementsPlayerSettlementsInformationText = "These are your settlements:";
const inline std::string checkEnemyIntentsTurnsToActInformationText = " intends to act in ";
const inline std::string checkEnemyIntentsAttackImminentText = "You will likely be attacked!";
const inline std::string checkEnemyIntentsSettlementsInformationText = "Owned settlements:";
const inline std::string modifyPlayerArmyOwnedOnesInformationText = "These are your armies:";
const inline std::string moveArmyPossibleSettlementsInformationText = "These are your settlements with armies:";
const inline std::string moveArmySettlementChoiceInformationText = "Enter the index of the settlement whose army you'd like to move.";
const inline std::string currentTurnInformationText = "Current turn: ";

//Successful operations
const inline std::string armyUnitRemovedSuccessText = "You have successfully removed an unit from your army.";
const inline std::string armyUnitAddedSuccessText = "You have successfully modified your army - added ";
const inline std::string modifyArmyDeleteSuccessText = "Army obliterated.";
const inline std::string moveArmyWhereSuccessText = "Army was successfully moved to an allied settlement!";
const inline std::string gameSavedSuccessText = "Game saved.";

//Unsuccessful operations
const inline std::string armyFullSizeWarningText = "Army is full already!";
const inline std::string moveArmyWhereFailText = "Army couldn't be moved!";
const inline std::string moveArmyWhereLackingActionPointsFailText = "Couldn't send army, probably not enough action points!";
const inline std::string moveArmyWhereSettlementHasArmyText = "The chosen settlement already had an army stationed.";
const inline std::string indexInvalidText = "Invalid index";
const inline std::string forgorToDoAllRequiredOperationsErrorText = "There still are things you should do!";

//Game UI Buttons
const inline std::string testButtonText = "Show ALL settlements";
const inline std::string checkSettlementsButtonText = "Check my settlements";
const inline std::string checkEnemyIntentsButtonText = "Check enemy intents";
const inline std::string modifyPlayerArmyButtonText = "Modify an army";
const inline std::string moveArmyButtonText = "Move an army";
const inline std::string nextTurnButtonText = "Next Turn";
const inline std::string saveGameButtonText = "Save Game";
const inline std::string exitButtonText = "Exit";

//Game UI Inputs
const inline std::string starterGeneralInputText = starterPreChoiceText;
const inline std::string modifyArmyCountInputText = "Count of the army you want to modify:";
const inline std::string modifyArmyWhatInputText = "0 - ADD UNIT | 1 - REMOVE UNIT | 2 - DELETE ARMY | 3 - CANCEL";
const inline std::string modifyArmyAddInputText = "Index of the general you'd wish to add:";
const inline std::string modifyArmyRemoveInputText = "Index of the general you want to remove:";
const inline std::string moveArmyWhichSettlementInputText = "Index of the settlement";
const inline std::string moveArmyWhereInputText = "Index of the neighbour:";

//JSON file names
const inline std::string generalsJsonFileName = "generals.json";
const inline std::string settlementsJsonFileName = "settlements.json";
const inline std::string controlPointsJsonFileName = "controlPoints.json";
const inline std::string captainsJsonFileName = "captains.json";
const inline std::string enemiesJsonFileName = "enemies.json";
const inline std::string localLeadersJsonFileName = "localLeaders.json";

const inline std::vector<std::string> generalTableHeaders =
{
    "Index", "Full name", "Type", "Rarity", "Melee", "Ranged", "Armour", "Strength", "Accuracy", "Dexterity", "Overall"
};
const inline std::vector<std::string> settlementTableHeaders =
{
    "Index", "Name", "Owned by", "Garrison OVR", "Has Army", "Control Points #", "Neighbours"
};
const inline std::vector<std::string> armyTableHeaders =
{
    "Index", "AP", "Max AP", "Full name", "Type", "Rarity", "Melee", "Ranged", "Armour", "Strength", "Accuracy",
    "Dexterity", "Overall"
};
const inline std::vector<std::string> generalFightTableHeaders =
{
    "Name", "Melee Power (Eff)", "Ranged Power (Eff)", "Armour Power (Eff)"
};

//template sorting function for any vectors of shared pointers
template<class T>
bool comparatorAsc(std::shared_ptr<T> a, std::shared_ptr<T> b) {
    return *a < *b;
}

template<class T>
std::vector<std::shared_ptr<T> > SortVectorSharedPTR(std::vector<std::shared_ptr<T> > vector) {
    sort(vector.begin(), vector.end(), comparatorAsc<T>);
    return vector;
}


#endif
