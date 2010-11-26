#include "Wiki.h"

using namespace BWAPI;

std::string wiki(std::string name, bool makeTitle = false)
{
  char temp[64];
  strcpy(temp, name.c_str());

  bool inword = false;
  size_t tlen = strlen(temp);
  for ( unsigned int i = 0; i < tlen; ++i )
  {
    if ( temp[i] == ' ' )
    {
      inword = false;
      if ( !makeTitle )
        temp[i] = '_';
    }
    else
    {
      if ( inword )
        temp[i] = tolower(temp[i]);
      inword = true;
    }

  }
  return std::string(temp);
}

void printUnitData(FILE *outWiki, UnitType u)
{
  fprintf(outWiki, "=== %s ===\n", maketitle(u));
  if ( !u.isInvincible() )
  {
    fprintf(outWiki, "  * *Max HP*: %d\n", u.maxHitPoints());
    if ( u.maxShields() )
      fprintf(outWiki, "  * *Max Shields*: %d\n", u.maxShields());
    if ( u.maxEnergy() )
      fprintf(outWiki, "  * *Max Energy*: %u\n", u.maxEnergy());
    fprintf(outWiki, "  * *Armor*: %u\n", u.armor());
    fprintf(outWiki, "  * *Unit Size*: %s\n", u.size().getName().c_str());
  }
  if ( u.whatBuilds().second > 0 )
  {
    if ( u.whatBuilds().second == 1 )
    {
      fprintf(outWiki, "  * *Mineral Cost*: %u\n", u.mineralPrice());
      fprintf(outWiki, "  * *Gas Cost*: %u\n", u.gasPrice());
    }
    fprintf(outWiki, "  * *Build Time*: %u\n", u.buildTime());
  }
  if ( u.supplyProvided() )
    fprintf(outWiki, "  * *Supply Provided*: %u\n", u.supplyProvided());
  if ( u.supplyRequired() )
    fprintf(outWiki, "  * *Supply Required*: %u\n", u.supplyRequired());
  if ( u.spaceProvided() )
    fprintf(outWiki, "  * *Space Provided*: %u\n", u.spaceProvided());
  if ( u.spaceRequired() != 255 )
    fprintf(outWiki, "  * *Space Required*: %u\n", u.spaceRequired());
  if ( u.isBuilding() )
    fprintf(outWiki, "  * *Tile Size*: %u x %u\n", u.tileWidth(), u.tileHeight());
  fprintf(outWiki, "  * *Unit Dimensions*: %u x %u\n", u.dimensionLeft() + u.dimensionRight() + 1, u.dimensionUp() + u.dimensionDown() + 1);
  fprintf(outWiki, "  * *Sight Range*: %u (%u)\n", u.sightRange(), u.sightRange()/32);
  if ( u.buildScore() && u.whatBuilds().second > 0 )
    fprintf(outWiki, "  * *Build Score*: %u\n", u.buildScore());
  if ( u.destroyScore() && !u.isInvincible() )
    fprintf(outWiki, "  * *Destroy Score*: %u\n", u.destroyScore());
  if ( u.topSpeed() > 0 )
    fprintf(outWiki, "  * *Top Speed*: %Lf\n", u.topSpeed());
  if ( u.groundWeapon() == u.airWeapon() )
  {
    if ( u.groundWeapon() != WeaponTypes::None )
    {
      fprintf(outWiki, "  * *Weapon*: [WeaponTypes#%s %s]\n", makelink(u.groundWeapon()) );
    }
  }
  else
  {
    if ( u.groundWeapon() != WeaponTypes::None )
      fprintf(outWiki, "  * *Ground Weapon*: [WeaponTypes#%s %s]\n", makelink(u.groundWeapon()) );
    if ( u.airWeapon() != WeaponTypes::None )
      fprintf(outWiki, "  * *Air Weapon*: [WeaponTypes#%s %s]\n", makelink(u.airWeapon()) );
  }
  if ( u.whatBuilds().first != UnitTypes::None )
  {
    if ( u.whatBuilds().second == 1 )
      fprintf(outWiki, "  * *Comes from*: [UnitTypes#%s %s]\n", makelink(u.whatBuilds().first) );
    else if ( u.whatBuilds().second > 1 )
      fprintf(outWiki, "  * *Comes from*: %u [UnitTypes#%s %s]\n", u.whatBuilds().second, makelink(u.whatBuilds().first) );
  }
  if ( !u.abilities().empty() ||
        u.hasPermanentCloak() ||
        u.isDetector()        ||
        u.isFlyingBuilding()  ||
        u.isInvincible()      ||
        u.regeneratesHP())
  {
    fprintf(outWiki, "\n  ==== Abilities ====\n");
    for each ( TechType t in u.abilities() )
      fprintf(outWiki, "    * [TechTypes#%s %s]\n", makelink(t));
    if ( u.hasPermanentCloak() )
      fprintf(outWiki, "    * Permanently Cloaked\n");
    if ( u.isDetector() )
      fprintf(outWiki, "    * Detector\n");
    if ( u.isFlyingBuilding() )
      fprintf(outWiki, "    * Liftoff\n");
    if ( u.isInvincible() )
      fprintf(outWiki, "    * Invincible\n");
    if ( u.regeneratesHP() )
      fprintf(outWiki, "    * Regenerates Hit Points\n");
  }
  if ( (!u.requiredUnits().empty()  ||
        u.requiresCreep()           ||
        u.requiresPsi()             ||
        u.requiredTech() != TechTypes::None) &&
       !u.isSpecialBuilding() )
  {
    fprintf(outWiki, "\n  ==== Requirements ====\n");
    for each ( std::pair<UnitType,int> p in u.requiredUnits() )
    {
      if ( p.second == 1 )
        fprintf(outWiki, "    * [UnitTypes#%s %s]\n", makelink(p.first) );
      else if ( p.second > 1 )
        fprintf(outWiki, "    * %u [UnitTypes#%s %s]\n", p.second, makelink(p.first) );
    }
    if ( u.requiresCreep() )
      fprintf(outWiki, "    * Requires Creep\n");
    if ( u.requiresPsi() )
      fprintf(outWiki, "    * Requires Psi Field\n");
    if ( u.requiredTech() != TechTypes::None )
      fprintf(outWiki, "    * [TechTypes#%s %s]\n", makelink(u.requiredTech()) );
  }
  if ( !u.upgrades().empty() )
  {
    fprintf(outWiki, "\n  ==== Upgrades ====\n");
    for each ( UpgradeType upg in u.upgrades() )
      fprintf(outWiki, "    * [UpgradeTypes#%s %s]\n", makelink(upg) );
  }
  fprintf(outWiki, "\n\n");
}

bool nameSort(const UnitType &u1, const UnitType &u2)
{
  return strcmp(maketitle(u1), maketitle(u2)) < 0;
}

void writeUnitWiki()
{
  FILE *outWiki = fopen("UnitTypes.wiki", "w");
  if ( !outWiki )
    return;

  fprintf(outWiki, "#summary Unit Types in BWAPI\n");
  fprintf(outWiki, "#sidebar TableOfContents\n\n");

  fprintf(outWiki, "This page contains a list of UnitTypes provided by BWAPI.\n\n");

  fprintf(outWiki, "<wiki:toc max_depth=\"3\" />\n\n");

  fprintf(outWiki, "\n= Terran =\n");
  fprintf(outWiki, "\n== Terran Ground Units ==\n");

  std::vector<UnitType> unitTypes;
  for each ( UnitType u in UnitTypes::allUnitTypes() )
    unitTypes.push_back(u);

  std::sort(unitTypes.begin(), unitTypes.end(), nameSort);

  for each ( UnitType u in unitTypes )
    if ( u.getRace() == Races::Terran && !u.isHero() && !u.isBuilding() && !u.isSpell() && !u.isFlyer() )
      printUnitData(outWiki, u);

  fprintf(outWiki, "\n== Terran Air Units ==\n");
  for each ( UnitType u in unitTypes )
    if ( u.getRace() == Races::Terran && !u.isHero() && !u.isBuilding() && !u.isSpell() && u.isFlyer() )
      printUnitData(outWiki, u);

  fprintf(outWiki, "\n== Terran Heroes ==\n");
  for each ( UnitType u in unitTypes )
    if ( u.getRace() == Races::Terran && u.isHero() && !u.isBuilding() && !u.isSpell() )
      printUnitData(outWiki, u);

  fprintf(outWiki, "\n== Terran Buildings ==\n");
  for each ( UnitType u in unitTypes )
    if ( u.getRace() == Races::Terran && u.isBuilding() && !u.isSpecialBuilding() && !u.isAddon() )
      printUnitData(outWiki, u);

  fprintf(outWiki, "\n== Terran Addons ==\n");
  for each ( UnitType u in unitTypes )
    if ( u.getRace() == Races::Terran && u.isBuilding() && !u.isSpecialBuilding() && u.isAddon() )
      printUnitData(outWiki, u);

  fprintf(outWiki, "\n== Terran Special Buildings ==\n");
  for each ( UnitType u in unitTypes )
    if ( u.getRace() == Races::Terran && u.isSpecialBuilding() && !u.isBeacon() && !u.isFlagBeacon() )
      printUnitData(outWiki, u);


  fprintf(outWiki, "\n= Protoss =\n");
  fprintf(outWiki, "\n== Protoss Ground Units ==\n");
  for each ( UnitType u in unitTypes )
    if ( u.getRace() == Races::Protoss && !u.isHero() && !u.isBuilding() && !u.isSpell() && !u.isFlyer() )
      printUnitData(outWiki, u);

  fprintf(outWiki, "\n== Protoss Air Units ==\n");
  for each ( UnitType u in unitTypes )
    if ( u.getRace() == Races::Protoss && !u.isHero() && !u.isBuilding() && !u.isSpell() && u.isFlyer() )
      printUnitData(outWiki, u);

  fprintf(outWiki, "\n== Protoss Heroes ==\n");
  for each ( UnitType u in unitTypes )
    if ( u.getRace() == Races::Protoss && u.isHero() && !u.isBuilding() && !u.isSpell() )
      printUnitData(outWiki, u);

  fprintf(outWiki, "\n== Protoss Buildings ==\n");
  for each ( UnitType u in unitTypes )
    if ( u.getRace() == Races::Protoss && u.isBuilding() && !u.isSpecialBuilding() )
      printUnitData(outWiki, u);

  fprintf(outWiki, "\n== Protoss Special Buildings ==\n");
  for each ( UnitType u in unitTypes )
    if ( u.getRace() == Races::Protoss && u.isSpecialBuilding() && !u.isBeacon() && !u.isFlagBeacon() )
      printUnitData(outWiki, u);


  fprintf(outWiki, "\n= Zerg =\n");
  fprintf(outWiki, "\n== Zerg Ground Units ==\n");
  for each ( UnitType u in unitTypes )
    if ( u.getRace() == Races::Zerg && !u.isHero() && !u.isBuilding() && !u.isSpell() && !u.isFlyer() )
      printUnitData(outWiki, u);

  fprintf(outWiki, "\n== Zerg Air Units ==\n");
  for each ( UnitType u in unitTypes )
    if ( u.getRace() == Races::Zerg && !u.isHero() && !u.isBuilding() && !u.isSpell() && u.isFlyer() )
      printUnitData(outWiki, u);

  fprintf(outWiki, "\n== Zerg Heroes ==\n");
  for each ( UnitType u in unitTypes )
    if ( u.getRace() == Races::Zerg && u.isHero() && !u.isBuilding() && !u.isSpell() )
      printUnitData(outWiki, u);

  fprintf(outWiki, "\n== Zerg Buildings ==\n");
  for each ( UnitType u in unitTypes )
    if ( u.getRace() == Races::Zerg && u.isBuilding() && !u.isSpecialBuilding() )
      printUnitData(outWiki, u);

  fprintf(outWiki, "\n== Zerg Special Buildings ==\n");
  for each ( UnitType u in unitTypes )
    if ( u.getRace() == Races::Zerg && u.isSpecialBuilding() && !u.isBeacon() && !u.isFlagBeacon() )
      printUnitData(outWiki, u);

  fprintf(outWiki, "\n= Other =\n");
  fprintf(outWiki, "\n== Critters ==\n");
  for each ( UnitType u in unitTypes )
    if ( u.getRace() == Races::Other && !u.isBuilding() && !u.isPowerup() && !u.isResourceContainer() )
      printUnitData(outWiki, u);

  fprintf(outWiki, "\n== Resources ==\n");
  for each ( UnitType u in unitTypes )
    if ( u.isResourceContainer() && !u.isRefinery() )
      printUnitData(outWiki, u);

  fprintf(outWiki, "\n== Spells ==\n");
  for each ( UnitType u in unitTypes )
    if ( u.isSpell() )
      printUnitData(outWiki, u);

  fprintf(outWiki, "\n== Beacons ==\n");
  for each ( UnitType u in unitTypes )
    if ( u.isBeacon() || u.isFlagBeacon() )
      printUnitData(outWiki, u);

  fprintf(outWiki, "\n== Powerups ==\n");
  for each ( UnitType u in unitTypes )
    if ( u.isPowerup() )
      printUnitData(outWiki, u);

  fprintf(outWiki, "\n== Misc ==\n");
  printUnitData(outWiki, UnitTypes::None);
  printUnitData(outWiki, UnitTypes::Unknown);

  fclose(outWiki);
}