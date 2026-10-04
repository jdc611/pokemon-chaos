#include "global.h"
#include "chaos_quiz.h"
#include "event_data.h"
#include "random.h"
#include "string_util.h"

struct ChaosQuizQuestion
{
    const u8 *question;
    const u8 *answers[3];
    u8 correctAnswer;
};

#define QUESTION(q, a, b, c, correct) {COMPOUND_STRING(q), {COMPOUND_STRING(a), COMPOUND_STRING(b), COMPOUND_STRING(c)}, correct}

// Draft bank: each attempt samples one question from each increasing tier.
// These tiers are deliberately not announced to the player.
static const struct ChaosQuizQuestion sChaosQuizQuestions[3][8] =
{
    {
        QUESTION("FIRE moves are super effective\nagainst which type?", "GRASS", "WATER", "ROCK", 0),
        QUESTION("Which type is immune to\nELECTRIC moves?", "FLYING", "GROUND", "WATER", 1),
        QUESTION("GHOST moves cannot normally\nhit which type?", "NORMAL", "PSYCHIC", "GHOST", 0),
        QUESTION("Which type is immune to\nPOISON moves?", "GRASS", "FAIRY", "STEEL", 2),
        QUESTION("Which item restores a move's\nPP?", "POTION", "ETHER", "REPEL", 1),
        QUESTION("PARALYSIS normally does what\nto a POKéMON's SPEED?", "Lowers it", "Raises it", "No change", 0),
        QUESTION("Matching a move to the user's\ntype gives which damage bonus?", "Critical hit", "Accuracy bonus", "STAB", 2),
        QUESTION("Which type of move is super\neffective against STEEL?", "FAIRY", "FIGHTING", "GRASS", 1),
    },
    {
        QUESTION("Which generation introduced the\nphysical/special move split?", "GEN II", "GEN III", "GEN IV", 2),
        QUESTION("Before that split, what decided\nphysical or special damage?", "Move type", "User's NATURE", "Move accuracy", 0),
        QUESTION("Which generation introduced\nABILITIES?", "GEN II", "GEN III", "GEN IV", 1),
        QUESTION("Which generation introduced\nNATURES?", "GEN III", "GEN IV", "GEN V", 0),
        QUESTION("Which generation introduced\nHidden Abilities?", "GEN III", "GEN IV", "GEN V", 2),
        QUESTION("Which GEN I stat handled both\nspecial attack and defense?", "SPEED", "SPECIAL", "ATTACK", 1),
        QUESTION("LEVITATE normally grants\nimmunity to which move type?", "ELECTRIC", "FLYING", "GROUND", 2),
        QUESTION("INTIMIDATE normally lowers\nwhich opponent stat?", "ATTACK", "DEFENSE", "SP. ATK", 0),
    },
    {
        QUESTION("Without other bonuses, how much\ndamage does standard STAB add?", "1.25x", "1.5x", "2x", 1),
        QUESTION("What is the maximum positive\nATTACK stat stage?", "+4", "+6", "+8", 1),
        QUESTION("An ATTACK boost of two stages\nuses which normal multiplier?", "1.5x", "2x", "3x", 1),
        QUESTION("A physical critical hit ignores\nwhich attacker's stat change?", "Lowered ATTACK", "Raised ATTACK", "Both", 0),
        QUESTION("Which ABILITY lets GROUND moves\nhit a foe with LEVITATE?", "STURDY", "PRESSURE", "MOLD BREAKER", 2),
        QUESTION("With only -1 ACCURACY, a 100%\naccurate move has what accuracy?", "50%", "75%", "90%", 1),
        QUESTION("With no other effects, which\nmove acts before TACKLE?", "GROWL", "QUICK ATTACK", "SWIFT", 1),
        QUESTION("A WATER move hits a ROCK/GROUND\nfoe for which type multiplier?", "2x", "4x", "0.5x", 1),
    },
};

#undef QUESTION

static u8 sPreviousQuizQuestions[ARRAY_COUNT(sChaosQuizQuestions)];
static u8 sPreviousQuizQuestionMask;

void ChaosQuizPrepareQuestion(void)
{
    u32 tier = gSpecialVar_0x8004;
    u32 index, rotation;
    u8 *const answerBuffers[] = {gStringVar1, gStringVar2, gStringVar3};
    const struct ChaosQuizQuestion *question;

    if (tier >= ARRAY_COUNT(sChaosQuizQuestions))
        tier = 0;

    // Bounded sampling excludes the last question seen in this tier, so a
    // retry rerolls its questions without an unbounded rejection loop.
    if (sPreviousQuizQuestionMask & (1 << tier))
    {
        index = RandomUniform(RNG_RANDOM_FROM_LIST, 0, ARRAY_COUNT(sChaosQuizQuestions[tier]) - 2);
        if (index >= sPreviousQuizQuestions[tier])
            index++;
    }
    else
        index = RandomUniform(RNG_RANDOM_FROM_LIST, 0, ARRAY_COUNT(sChaosQuizQuestions[tier]) - 1);

    sPreviousQuizQuestions[tier] = index;
    sPreviousQuizQuestionMask |= 1 << tier;
    question = &sChaosQuizQuestions[tier][index];
    rotation = RandomUniform(RNG_RANDOM_FROM_LIST, 0, ARRAY_COUNT(question->answers) - 1);
    StringCopy(gStringVar4, question->question);
    for (u32 i = 0; i < ARRAY_COUNT(answerBuffers); i++)
        StringCopy(answerBuffers[i], question->answers[(i + rotation) % ARRAY_COUNT(question->answers)]);
    gSpecialVar_0x8005 = (question->correctAnswer + ARRAY_COUNT(question->answers) - rotation) % ARRAY_COUNT(question->answers);
    gSpecialVar_0x8006 = tier * ARRAY_COUNT(sChaosQuizQuestions[tier]) + index;
}
