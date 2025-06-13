#ifndef PRIMARYGENERATOR_HH
#define PRIMARYGENERATOR_HH

#include "G4Event.hh"
#include "G4GeneralParticleSource.hh"
#include "G4ParticleDefinition.hh"
#include "G4ParticleGun.hh"
#include "G4ParticleTable.hh"
#include "G4SystemOfUnits.hh"
#include "G4VUserPrimaryGeneratorAction.hh"
#include "RunAction.hh"
// class RunAction;
class PrimaryGenerator : public G4VUserPrimaryGeneratorAction
{
public:
  PrimaryGenerator();
  virtual ~PrimaryGenerator();

  virtual void GeneratePrimaries(G4Event *);

private:
  G4ParticleGun *particleGun;
  G4ParticleTable *particleTable;
  G4ParticleDefinition *particleDefinition;
  G4GeneralParticleSource *fGPS;
  RunAction *fRunAction;
};
#endif
