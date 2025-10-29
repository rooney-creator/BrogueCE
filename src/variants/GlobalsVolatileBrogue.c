/*
 *  GlobalsVolatileBrogue.c
 *  Brogue
 *
 *  Variant-specific data for Volatile Brogue. This variant reuses the
 *  baseline Brogue content but dials up high-risk elements like out-of-depth
 *  monsters and swingy status effects.
 */

#include <stdio.h>
#include <string.h>

#include "Rogue.h"
#include "GlobalsBase.h"
#include "Globals.h"
#include "GlobalsBrogue.h"
#include "GlobalsVolatileBrogue.h"

#define VOLATILE_BROGUE_MAJOR 1
#define VOLATILE_BROGUE_MINOR 0
#define VOLATILE_BROGUE_PATCH 0

static const char volatileBrogueRecordingVersionString[] = "VB " STRINGIFY(VOLATILE_BROGUE_MAJOR) "." STRINGIFY(VOLATILE_BROGUE_MINOR) "." STRINGIFY(VOLATILE_BROGUE_PATCH);
static const char volatileBroguePatchPattern[] = "VB " STRINGIFY(VOLATILE_BROGUE_MAJOR) "." STRINGIFY(VOLATILE_BROGUE_MINOR) ".%hu";
static const char volatileBrogueDungeonVersionString[] = "VB 1.0";

#define AMULET_LEVEL            26
#define DEEPEST_LEVEL           40

static char volatileBrogueVersionString[32];
const char *volatileBrogueVersion = volatileBrogueVersionString;

typedef struct mutableGameConstants {
    int majorVersion;
    int minorVersion;
    int patchVersion;

    const char *variantName;

    const char *versionString;
    const char *dungeonVersionString;
    const char *patchVersionPattern;
    const char *recordingVersionString;

    int deepestLevel;
    int amuletLevel;

    int depthAccelerator;
    int minimumAltarLevel;
    int minimumLavaLevel;
    int minimumBrimstoneLevel;
    int mutationsOccurAboveLevel;
    int monsterOutOfDepthChance;

    int extraItemsPerLevel;
    int goldAdjustmentStartDepth;

    int machinesPerLevelSuppressionMultiplier;
    int machinesPerLevelSuppressionOffset;
    int machinesPerLevelIncreaseFactor;
    int maxLevelForBonusMachines;
    int deepestLevelForMachines;

    int playerTransferenceRatio;
    int onHitHallucinateDuration;
    int onHitWeakenDuration;
    int onHitMercyHealPercent;

    int fallDamageMin;
    int fallDamageMax;

    int weaponKillsToAutoID;
    int armorDelayToAutoID;
    int ringDelayToAutoID;

    int numberAutogenerators;
    int numberBoltKinds;
    int numberBlueprints;
    int numberHordes;

    int numberMeteredItems;
    int numberCharmKinds;
    int numberPotionKinds;
    int numberGoodPotionKinds;
    int numberScrollKinds;
    int numberGoodScrollKinds;
    int numberWandKinds;
    int numberGoodWandKinds;

    int numberFeats;
    int companionFeatRequiredXP;

    int mainMenuTitleHeight;
    int mainMenuTitleWidth;
} mutableGameConstants;

_Static_assert(sizeof(mutableGameConstants) == sizeof(gameConstants),
               "mutableGameConstants must mirror gameConstants");

static mutableGameConstants volatileBrogueGameConst;
static volatileClass volatileBrogueClassSelection = VOLATILE_CLASS_NONE;

void setVolatileBrogueClass(volatileClass selection) {
    volatileBrogueClassSelection = selection;
}

volatileClass getVolatileBrogueClass(void) {
    return volatileBrogueClassSelection;
}

void initializeGameVariantVolatileBrogue() {
    // Start with the baseline Brogue setup so we can reuse all the existing
    // tables and pointers.
    initializeGameVariantBrogue();

    // Copy the canonical Brogue constants and then tweak what we need for the
    // volatile experience.
    memcpy(&volatileBrogueGameConst, gameConst, sizeof(volatileBrogueGameConst));

    volatileBrogueGameConst.majorVersion = VOLATILE_BROGUE_MAJOR;
    volatileBrogueGameConst.minorVersion = VOLATILE_BROGUE_MINOR;
    volatileBrogueGameConst.patchVersion = VOLATILE_BROGUE_PATCH;

    snprintf(volatileBrogueVersionString, sizeof(volatileBrogueVersionString),
             "VB %d.%d.%d%s",
             VOLATILE_BROGUE_MAJOR,
             VOLATILE_BROGUE_MINOR,
             VOLATILE_BROGUE_PATCH,
             BROGUE_EXTRA_VERSION);

    volatileBrogueGameConst.versionString = volatileBrogueVersionString;
    volatileBrogueGameConst.dungeonVersionString = volatileBrogueDungeonVersionString;
    volatileBrogueGameConst.patchVersionPattern = volatileBroguePatchPattern;
    volatileBrogueGameConst.recordingVersionString = volatileBrogueRecordingVersionString;

    volatileBrogueGameConst.variantName = "volatileBrogue";

    volatileBrogueGameConst.deepestLevel = DEEPEST_LEVEL;
    volatileBrogueGameConst.amuletLevel = AMULET_LEVEL;

    // Tune a few gameplay knobs to make the dungeon feel unstable.
    volatileBrogueGameConst.minimumAltarLevel = 10;
    volatileBrogueGameConst.minimumLavaLevel = 3;
    volatileBrogueGameConst.minimumBrimstoneLevel = 12;
    volatileBrogueGameConst.mutationsOccurAboveLevel = 6;
    volatileBrogueGameConst.monsterOutOfDepthChance = 25;

    volatileBrogueGameConst.machinesPerLevelSuppressionMultiplier = 3;
    volatileBrogueGameConst.machinesPerLevelSuppressionOffset = 1;
    volatileBrogueGameConst.machinesPerLevelIncreaseFactor = 2;
    volatileBrogueGameConst.maxLevelForBonusMachines = 3;

    volatileBrogueGameConst.extraItemsPerLevel = 1;
    volatileBrogueGameConst.goldAdjustmentStartDepth = 4;

    volatileBrogueGameConst.playerTransferenceRatio = 15;
    volatileBrogueGameConst.onHitHallucinateDuration = 60;
    volatileBrogueGameConst.onHitWeakenDuration = 220;
    volatileBrogueGameConst.onHitMercyHealPercent = 40;

    volatileBrogueGameConst.weaponKillsToAutoID = 10;
    volatileBrogueGameConst.armorDelayToAutoID = 600;
    volatileBrogueGameConst.ringDelayToAutoID = 900;

    volatileBrogueGameConst.fallDamageMin = 8;
    volatileBrogueGameConst.fallDamageMax = 16;

    volatileBrogueGameConst.deepestLevelForMachines = AMULET_LEVEL;
    volatileBrogueGameConst.companionFeatRequiredXP = 8400;

    // Reset any pending class selection until the player picks one.
    setVolatileBrogueClass(VOLATILE_CLASS_NONE);

    // Point the global pointer at our tailored copy.
    gameConst = (const gameConstants *) &volatileBrogueGameConst;
}
