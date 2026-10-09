#include "global.h"
#include "test/battle.h"
#include "chaos_abilities.h"
#include "battle_util.h"
#include "constants/weather.h"

SINGLE_BATTLE_TEST("Chaos V3 Zen Mode uses temporary actual battle stats on a non-original species")
{
    GIVEN { PLAYER(SPECIES_PIKACHU) { Ability(ABILITY_ZEN_MODE); HP(50); MaxHP(100); } OPPONENT(SPECIES_WOBBUFFET); }
    WHEN { TURN { MOVE(player, MOVE_CELEBRATE); MOVE(opponent, MOVE_CELEBRATE); } }
    THEN {
        EXPECT_EQ(ChaosAbilityStat(B_BATTLER_0, ABILITY_ZEN_MODE, STAT_SPATK, 100), 150);
        EXPECT_EQ(ChaosAbilityStat(B_BATTLER_0, ABILITY_ZEN_MODE, STAT_SPDEF, 100), 150);
        EXPECT_EQ(player->species, SPECIES_PIKACHU);
        EXPECT_EQ(player->maxHP, 100);
    }
}

SINGLE_BATTLE_TEST("Chaos V3 Disguise blocks an eligible first hit completely without changing species")
{
    GIVEN { PLAYER(SPECIES_PIKACHU) { Ability(ABILITY_DISGUISE); HP(100); MaxHP(100); } OPPONENT(SPECIES_WOBBUFFET); }
    WHEN { TURN { MOVE(player, MOVE_CELEBRATE); MOVE(opponent, MOVE_SCRATCH); } }
    THEN { EXPECT_EQ(player->hp, 100); EXPECT_EQ(player->species, SPECIES_PIKACHU); EXPECT(GetBattlerPartyState(B_BATTLER_0)->chaosDisguiseBroken); }
}

SINGLE_BATTLE_TEST("Chaos V3 Ice Face blocks a physical first hit on a non-original species")
{
    GIVEN { PLAYER(SPECIES_PIKACHU) { Ability(ABILITY_ICE_FACE); HP(100); MaxHP(100); } OPPONENT(SPECIES_WOBBUFFET); }
    WHEN { TURN { MOVE(player, MOVE_CELEBRATE); MOVE(opponent, MOVE_SCRATCH); } }
    THEN { EXPECT_EQ(player->hp, 100); EXPECT_EQ(player->species, SPECIES_PIKACHU); EXPECT(GetBattlerPartyState(B_BATTLER_0)->chaosIceBroken); }
}

SINGLE_BATTLE_TEST("Chaos V3 Power Construct boosts defenses without increasing HP or changing stored species")
{
    GIVEN { PLAYER(SPECIES_PIKACHU) { Ability(ABILITY_POWER_CONSTRUCT); HP(50); MaxHP(100); } OPPONENT(SPECIES_WOBBUFFET); }
    WHEN { TURN { MOVE(player, MOVE_CELEBRATE); MOVE(opponent, MOVE_CELEBRATE); } }
    THEN {
        EXPECT(GetBattlerPartyState(B_BATTLER_0)->chaosComplete);
        EXPECT_EQ(ChaosAbilityStat(B_BATTLER_0, ABILITY_POWER_CONSTRUCT, STAT_DEF, 100), 150);
        EXPECT_EQ(ChaosAbilityStat(B_BATTLER_0, ABILITY_POWER_CONSTRUCT, STAT_ATK, 100), 120);
        EXPECT_EQ(player->hp, 50); EXPECT_EQ(player->maxHP, 100); EXPECT_EQ(GetMonData(&gParties[B_TRAINER_PLAYER][0], MON_DATA_SPECIES), SPECIES_PIKACHU);
    }
}

SINGLE_BATTLE_TEST("Chaos V3 Stance Change returns to normal after a status move")
{
    GIVEN { PLAYER(SPECIES_PIKACHU) { Ability(ABILITY_STANCE_CHANGE); } OPPONENT(SPECIES_WOBBUFFET); }
    WHEN { TURN { MOVE(player, MOVE_SCRATCH); MOVE(opponent, MOVE_CELEBRATE); } TURN { MOVE(player, MOVE_CELEBRATE); MOVE(opponent, MOVE_CELEBRATE); } }
    THEN { EXPECT(!GetBattlerPartyState(B_BATTLER_0)->chaosBlade); EXPECT_EQ(ChaosAbilityStat(B_BATTLER_0, ABILITY_STANCE_CHANGE, STAT_ATK, 100), 100); EXPECT_EQ(player->statStages[STAT_ATK], DEFAULT_STAT_STAGE); }
}

SINGLE_BATTLE_TEST("Chaos V3 Tera Shift preserves Tera Starstorm as Normal on a non-original species")
{
    GIVEN { PLAYER(SPECIES_PIKACHU) { Ability(ABILITY_TERA_SHIFT); } OPPONENT(SPECIES_GASTLY); }
    WHEN { TURN { MOVE(player, MOVE_TERA_STARSTORM); MOVE(opponent, MOVE_CELEBRATE); } }
    THEN { EXPECT(GetBattlerPartyState(B_BATTLER_0)->chaosTera); EXPECT_EQ(opponent->hp, opponent->maxHP); EXPECT_EQ(player->species, SPECIES_PIKACHU); EXPECT_EQ(ChaosAbilityStat(B_BATTLER_0, ABILITY_TERA_SHIFT, STAT_DEF, 100), 130); }
}

SINGLE_BATTLE_TEST("Chaos V3 Gulp Missile loads Surf and retaliates for one quarter of attacker maximum HP")
{
    s16 retaliation;
    GIVEN { PLAYER(SPECIES_PIKACHU) { Ability(ABILITY_GULP_MISSILE); HP(100); MaxHP(100); Speed(200); } OPPONENT(SPECIES_WOBBUFFET) { HP(1000); MaxHP(1000); Speed(100); } }
    WHEN { TURN { MOVE(player, MOVE_SURF); MOVE(opponent, MOVE_CELEBRATE); } TURN { MOVE(player, MOVE_CELEBRATE); MOVE(opponent, MOVE_SCRATCH); } }
    SCENE { ANIMATION(ANIM_TYPE_MOVE, MOVE_SURF, player); HP_BAR(opponent); ANIMATION(ANIM_TYPE_MOVE, MOVE_SCRATCH, opponent); HP_BAR(player); HP_BAR(opponent, captureDamage: &retaliation); }
    THEN { EXPECT_EQ(retaliation, 250); EXPECT_EQ(opponent->statStages[STAT_DEF], DEFAULT_STAT_STAGE - 1); EXPECT(!GetBattlerPartyState(B_BATTLER_0)->chaosGulpLoaded); }
}

SINGLE_BATTLE_TEST("Chaos V3 Singles Symbiosis transfers the actual bench item and prevents post-battle duplication")
{
    GIVEN { PLAYER(SPECIES_WOBBUFFET) { Ability(ABILITY_UNBURDEN); Item(ITEM_ORAN_BERRY); HP(40); MaxHP(100); } PLAYER(SPECIES_ORANGURU) { Ability(ABILITY_SYMBIOSIS); Item(ITEM_LEFTOVERS); } OPPONENT(SPECIES_WOBBUFFET); }
    WHEN { TURN { MOVE(player, MOVE_CELEBRATE); MOVE(opponent, MOVE_CELEBRATE); } }
    THEN {
        EXPECT_EQ(player->item, ITEM_LEFTOVERS);
        EXPECT_EQ(GetMonData(&gParties[B_TRAINER_PLAYER][1], MON_DATA_HELD_ITEM), ITEM_NONE);
        EXPECT(!gBattleMons[B_BATTLER_0].volatiles.unburdenActive);
        gBattleTypeFlags |= BATTLE_TYPE_TRAINER;
        TryRestoreHeldItems();
        EXPECT_EQ(GetMonData(&gParties[B_TRAINER_PLAYER][0], MON_DATA_HELD_ITEM), ITEM_LEFTOVERS);
        EXPECT_EQ(GetMonData(&gParties[B_TRAINER_PLAYER][1], MON_DATA_HELD_ITEM), ITEM_NONE);
    }
}

DOUBLE_BATTLE_TEST("Chaos V3 Commander boosts an active ally once while holder remains targetable")
{
    GIVEN { PLAYER(SPECIES_PIKACHU) { Ability(ABILITY_COMMANDER); } PLAYER(SPECIES_WOBBUFFET); OPPONENT(SPECIES_WOBBUFFET); OPPONENT(SPECIES_WOBBUFFET); }
    WHEN { TURN { MOVE(playerLeft, MOVE_CELEBRATE); MOVE(playerRight, MOVE_CELEBRATE); MOVE(opponentLeft, MOVE_SCRATCH, target: playerLeft); MOVE(opponentRight, MOVE_CELEBRATE); } }
    THEN { EXPECT_LT(playerLeft->hp, playerLeft->maxHP); for (enum Stat stat=STAT_ATK;stat<=STAT_SPDEF;stat++) EXPECT_EQ(playerRight->statStages[stat], DEFAULT_STAT_STAGE+1); }
}

SINGLE_BATTLE_TEST("Chaos V3 Teraform Zero clears weather on non-original entry and reentry")
{
    GIVEN { PLAYER(SPECIES_PIKACHU) { Ability(ABILITY_TERAFORM_ZERO); Speed(100); } PLAYER(SPECIES_WOBBUFFET) { Speed(50); } OPPONENT(SPECIES_NINETALES) { Ability(ABILITY_DROUGHT); Speed(200); } }
    WHEN { TURN { MOVE(player, MOVE_SUNNY_DAY); MOVE(opponent, MOVE_CELEBRATE); } TURN { SWITCH(player, 1); MOVE(opponent, MOVE_CELEBRATE); } TURN { SWITCH(player, 0); MOVE(opponent, MOVE_CELEBRATE); } }
    THEN { EXPECT_EQ(gBattleWeather, WEATHER_NONE); }
}
