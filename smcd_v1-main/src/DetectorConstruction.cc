#include "DetectorConstruction.hh"
#include "RadialEnergyDepositSD.hh"
#include "SingleParticleSD.hh"
#include "G4VisAttributes.hh"
#include "G4Colour.hh"
#include "G4PVReplica.hh"

DetectorConstruction::DetectorConstruction() {
};

DetectorConstruction::~DetectorConstruction() {

};

G4VPhysicalVolume *DetectorConstruction::Construct()
{

    G4bool checkOverlaps = true;

    // 63Ni
    G4Isotope *isoNi63 = new G4Isotope("Ni63", 28, 63, 62.9296 * g / mole);
    G4Isotope *isoNi64 = new G4Isotope("Ni64", 28, 64, 63.92797 * g / mole);
    G4Element *iso_elNi = new G4Element("CustomNickel", "Ni", 2);
    iso_elNi->AddIsotope(isoNi63, 20. * perCent);
    iso_elNi->AddIsotope(isoNi64, 80. * perCent);
    G4Material *MixNi = new G4Material("Ni", 8.9 * g / cm3, 1);
    MixNi->AddElement(iso_elNi, 100. * perCent);

    // SiC
    G4Element *elSi = new G4Element("Silicon", "Si", 14, 28.09 * g / mole);
    G4Element *elC = new G4Element("Carbon", "C", 6, 12.01 * g / mole);
    G4Material *SiC = new G4Material("SiliconCarbide", 3.21 * g / cm3, 2);
    SiC->AddElement(elSi, 1);
    SiC->AddElement(elC, 1);

    // construct
    G4NistManager *nist = G4NistManager::Instance();
    G4Material *worldmat = nist->FindOrBuildMaterial("G4_Galactic");

    // World
    G4double worldX = 20. * um, worldY = 20. * um, worldZ = 20 * um;
    auto *solidWorld = new G4Box("SoildWorld", worldX, worldY, worldZ);
    G4LogicalVolume *logicWorld = new G4LogicalVolume(solidWorld, worldmat, "LogicWorld");
    G4VPhysicalVolume *physWorld = new G4PVPlacement(0, G4ThreeVector(0., 0., 0.), logicWorld, "PhysWorld", 0, false, 0, checkOverlaps);
    G4VisAttributes *worldVisAtt = new G4VisAttributes(G4Colour(1.0, 1.0, 1.0, 0.1));
    worldVisAtt->SetVisibility(true);
    logicWorld->SetVisAttributes(worldVisAtt);

    // SIC
    G4double sicX = 10 * um, sicY = 10. * um, sicZ = 5 * um;
    G4Box *solidSic = new G4Box("SolidSic", sicX, sicY, sicZ);
    G4LogicalVolume *logicSic = new G4LogicalVolume(solidSic, SiC, "LogicSic");
    G4VPhysicalVolume *physSic = new G4PVPlacement(0, G4ThreeVector(0. * um, 0., 5 * um), logicSic, "PhysSic", logicWorld, false, 0, checkOverlaps);

    G4VisAttributes *sicVisAtt = new G4VisAttributes(G4Colour(1.0, 0.0, 0.0, 0.7));
    sicVisAtt->SetVisibility(true);
    logicSic->SetVisAttributes(sicVisAtt);

    // Ni
    /* G4int NumLayersNi63 = 20;
     G4double layerSpacing = 0.25 * um;
     G4double Ni63X = 10. * um, Ni63Y = 10. * um, Ni63Z = 2.5 * um;
     G4double totalNi63Z = NumLayersNi63 * layerSpacing;

     auto *solidNi63 = new G4Box("SolidNi63", Ni63X, Ni63Y, Ni63Z);
     auto *logicalNi63 = new G4LogicalVolume(solidNi63, MixNi, "LogicNi63");
     G4VPhysicalVolume *physNi63 = new G4PVPlacement(0, G4ThreeVector(0., 0., -5 * um), logicalNi63, "PhysNi63", logicWorld, false, 0, checkOverlaps);

     // Ni-layer
     auto *solidNi63layer = new G4Box("SolidNi63", Ni63X, Ni63Y, layerSpacing / 2);
     auto *logicalNi63layer = new G4LogicalVolume(solidNi63layer, Ni63, "LogicaNi63layer");

     new G4PVReplica("Ni63layer", logicalNi63layer, logicalNi63, kZAxis, NumLayersNi63, layerSpacing);

     auto *niVisAtt = new G4VisAttributes(G4Colour(1.0, 0.843, 0.0, 0.8));
     niVisAtt->SetVisibility(true);

     logicalNi63->SetVisAttributes(niVisAtt);
  */
    fScoringVolume = logicSic;

    // new detector
    /*G4double newDetX = 20 * um, newDetY = 20 * um, newSicDetZ = 2 * um;
    G4Box *solidNewDet = new G4Box("SolidNewDet", newDetX, newDetY, newSicDetZ);
    G4LogicalVolume *logicNewDet = new G4LogicalVolume(solidNewDet, SiC, "LogicnewSic");
    G4double newDetPosZ = -15 * um + newSicDetZ;
    G4VPhysicalVolume *physnewSic = new G4PVPlacement(0, G4ThreeVector(0. * um, 0. * um, -6. * um), logicNewDet, "PhysnewSic", logicWorld, false, 0, checkOverlaps);

    G4VisAttributes *newsicVisatt = new G4VisAttributes(G4Colour(1.0, 1.0, 1.0, 0.2));
    newsicVisatt->SetVisibility(true);
    logicNewDet->SetVisAttributes(newsicVisatt);
*/

    G4int nbins = 500;       // 径向分箱数量
    G4double rmax = 10 * um; // 最大径向距离

    // G4VSensitiveDetector *radiaSD = new RadialEnergyDepositSD("RadialEnergyDeposit", nbins, rmax);

    return physWorld;
}

void DetectorConstruction::ConstructSDandField()
{
    G4SDManager *SDman = G4SDManager::GetSDMpointer();

    if (!SDman->FindSensitiveDetector("SingleParticleSD", false))
    {
        G4VSensitiveDetector *radialSD =
            new SingleParticleSD("SingleParticleSD", 100, 10 * um, "particle_results.root");
        SDman->AddNewDetector(radialSD);
        fScoringVolume->SetSensitiveDetector(radialSD);
    }
}
