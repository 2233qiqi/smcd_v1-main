#include "PrimaryGenerator.hh"
#include "G4GeneralParticleSource.hh"
#include "G4ParticleTable.hh"
#include "G4SystemOfUnits.hh"

PrimaryGenerator::PrimaryGenerator() : G4VUserPrimaryGeneratorAction()
{

    fGPS = new G4GeneralParticleSource();
}

void PrimaryGenerator::GeneratePrimaries(G4Event *evt)
{
    fGPS->GeneratePrimaryVertex(evt);
}

PrimaryGenerator::~PrimaryGenerator()
{
    delete fGPS;
}

/*PrimaryGenerator::PrimaryGenerator() : G4VUserPrimaryGeneratorAction()
{

    fGPS = new G4GeneralParticleSource();

    G4ParticleTable *particleTable = G4ParticleTable::GetParticleTable();
    G4ParticleDefinition *electron = particleTable->FindParticle("e-");

    fGPS->GetCurrentSource()->SetParticleDefinition(electron);

    fGPS->GetCurrentSource()->GetAngDist()->SetAngDistType("planar");
    fGPS->GetCurrentSource()->GetAngDist()->SetParticleMomentumDirection(G4ThreeVector(0, 0, 1));
    fGPS->GetCurrentSource()->GetPosDist()->SetPosDisType("Point");
    fGPS->GetCurrentSource()->GetPosDist()->SetCentreCoords(G4ThreeVector(0, 0, 0));
    fGPS->GetCurrentSource()->GetEneDist()->SetEnergyDisType("Mono");
}*/

/*PrimaryGenerator::PrimaryGenerator() : G4VUserPrimaryGeneratorAction()
{

    particleGun = new G4ParticleGun(1);

    G4String particle_name = "e-";
    particleDefinition = G4ParticleTable::GetParticleTable()->FindParticle(particle_name);
    particleGun->SetParticleDefinition(particleDefinition);

    G4double particle_E = 17.63 * keV;
    particleGun->SetParticleEnergy(particle_E);

    G4ThreeVector Gunpos(0., 0., 0.);
    particleGun->SetParticlePosition(Gunpos);

    G4ThreeVector Gunp(0., 0., 1.);
    particleGun->SetParticleMomentumDirection(Gunp);
}

void PrimaryGenerator::GeneratePrimaries(G4Event *evt)
{

    particleGun->GeneratePrimaryVertex(evt);
}

PrimaryGenerator::~PrimaryGenerator()
{
    delete particleGun;
}*/