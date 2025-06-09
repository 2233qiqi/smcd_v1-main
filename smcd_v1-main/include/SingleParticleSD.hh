#ifndef SINGLE_PARTICLE_SD_HH
#define SINGLE_PARTICLE_SD_HH

#include "G4VSensitiveDetector.hh"
#include <vector>
#include <map>
#include <mutex>

class G4Step;
class G4TouchableHistory;
class G4Track;

class SingleParticleSD : public G4VSensitiveDetector
{
public:
    SingleParticleSD(G4String name, G4int nbins, G4double maxDist);
    virtual ~SingleParticleSD();

    virtual void Initialize(G4HCofThisEvent *hce) override;
    virtual G4bool ProcessHits(G4Step *step, G4TouchableHistory *history) override;
    virtual void EndOfEvent(G4HCofThisEvent *hce) override;

    G4double CalculateAntiScatter(const std::map<G4int, G4double> &initialEnergies);

    void CalculateStatistics();
    void SaveToROOT(const G4String &filename);
    void ClearTrackData();

private:
    const G4int fNbins;
    const G4double fMaxDist;
    const G4double fBinWidth;

    std::mutex fMutex;
    std::mutex fTrackMutex;

    // Per-bin data
    std::vector<G4double> fEdepSum;
    std::vector<G4double> fEdepSum2;
    std::vector<G4int> fHitCounts;

    // Results
    std::vector<G4double> fMeanEdep;
    std::vector<G4double> fStdError;

    // 计算总的能量沉积
    G4double fTotalEdepAllEvents = 0.0;

    G4int fEventCount = 0;
};

#endif // SINGLE_PARTICLE_SD_HH