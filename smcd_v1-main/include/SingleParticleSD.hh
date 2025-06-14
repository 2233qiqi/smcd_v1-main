#ifndef SINGLE_PARTICLE_SD_HH
#define SINGLE_PARTICLE_SD_HH

#include "G4VSensitiveDetector.hh"
#include <vector>
#include <map>
#include <mutex>
#include <string>

class G4Step;
class G4TouchableHistory;
class G4Track;

class SingleParticleSD : public G4VSensitiveDetector
{
public:
    SingleParticleSD(const G4String &name, G4int nbins, G4double maxDist, const G4String &filename);
    virtual ~SingleParticleSD();

    virtual void Initialize(G4HCofThisEvent *hce) override;
    virtual G4bool ProcessHits(G4Step *step, G4TouchableHistory *history) override;
    virtual void EndOfEvent(G4HCofThisEvent *hce) override;

    void CalculateAndRecordAntiScatter();

private:
    void CalculateStatistics();
    void SaveToROOT();
    void ClearEventData();
    void PrintResultsToTerminal();

    const G4int fNbins;
    const G4double fMaxDist;
    const G4double fBinWidth;
    const G4String fOutputFilename;

    std::mutex fMutex;

    std::vector<G4double> fRunEdepSum;
    std::vector<G4double> fRunEdepSum2;
    std::vector<G4long> fRunHitCounts;
    G4double fTotalEdepAllEvents = 0.0;
    G4int fEventCount = 0;

    std::vector<G4double> fEventEdepSum;
    std::vector<G4double> fEventEdepSum2;
    std::vector<G4long> fEventHitCounts;

    std::map<G4int, G4double> fEventInitialEnergies;

    std::vector<G4double> fMeanEdep;
    std::vector<G4double> fStdError;
};

#endif