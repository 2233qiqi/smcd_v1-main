#include "SingleParticleSD.hh"
#include "G4Step.hh"
#include "G4TouchableHistory.hh"
#include "G4RunManager.hh"
#include "G4SystemOfUnits.hh"
#include "G4Threading.hh"

#include "TFile.h"
#include "TGraphErrors.h"
#include "TCanvas.h"
#include <cmath>

SingleParticleSD::SingleParticleSD(const G4String &name, G4int nbins, G4double maxDist, const G4String &filename)
    : G4VSensitiveDetector(name),
      fNbins(nbins),
      fMaxDist(maxDist),
      fBinWidth(maxDist > 0 ? maxDist / nbins : 0.0),
      fOutputFilename(filename),
      fRunEdepSum(nbins, 0.0),
      fRunEdepSum2(nbins, 0.0),
      fRunHitCounts(nbins, 0),
      fEventEdepSum(nbins, 0.0),
      fEventEdepSum2(nbins, 0.0),
      fEventHitCounts(nbins, 0),
      fMeanEdep(nbins, 0.0),
      fStdError(nbins, 0.0)
{

    G4cout << ">>> [DEBUG] SD anme: " << name << " constructed. "
           << "MaxDist: " << fMaxDist / um << " um, "
           << "BinWidth: " << fBinWidth / um << " um" << G4endl;
}

SingleParticleSD::~SingleParticleSD()
{
    if (G4Threading::IsMasterThread())
    {

        CalculateStatistics();

        PrintResultsToTerminal();

        G4cout << "\n>>> Saving results to " << fOutputFilename << "..." << G4endl;
        SaveToROOT();
    }
}

void SingleParticleSD::Initialize(G4HCofThisEvent *)
{
    ClearEventData();
}

void SingleParticleSD::ClearEventData()
{
    std::fill(fEventEdepSum.begin(), fEventEdepSum.end(), 0.0);
    std::fill(fEventEdepSum2.begin(), fEventEdepSum2.end(), 0.0);
    std::fill(fEventHitCounts.begin(), fEventHitCounts.end(), 0);
}

G4bool SingleParticleSD::ProcessHits(G4Step *step, G4TouchableHistory *)
{

    G4double edep = step->GetTotalEnergyDeposit();
    if (edep <= 0.0)
    {
        return false;
    }

    G4ThreeVector pos = step->GetPreStepPoint()->GetPosition();
    G4double dist = pos.mag();
    G4int bin_idx = static_cast<int>(dist / fBinWidth);

    if (dist < fMaxDist && bin_idx >= 0 && bin_idx < fNbins)
    {
        fEventEdepSum[bin_idx] += edep;
        fEventEdepSum2[bin_idx] += edep * edep;
        fEventHitCounts[bin_idx]++;
    }

    return true;
}

void SingleParticleSD::EndOfEvent(G4HCofThisEvent *)
{

    std::lock_guard<std::mutex> lock(fMutex);
    for (G4int i = 0; i < fNbins; ++i)
    {
        fRunEdepSum[i] += fEventEdepSum[i];
        fRunEdepSum2[i] += fEventEdepSum2[i];
        fRunHitCounts[i] += fEventHitCounts[i];
    }
    fEventCount++;
}

void SingleParticleSD::CalculateStatistics()
{
    fTotalEdepAllEvents = 0.0;
    for (G4int i = 0; i < fNbins; ++i)
    {
        if (fRunHitCounts[i] > 0)
        {
            fMeanEdep[i] = fRunEdepSum[i] / fRunHitCounts[i];
            if (fRunHitCounts[i] > 1)
            {
                G4double variance = (fRunEdepSum2[i] - fRunEdepSum[i] * fMeanEdep[i]) / (fRunHitCounts[i] - 1);
                fStdError[i] = std::sqrt(std::abs(variance) / fRunHitCounts[i]);
            }
        }
        fTotalEdepAllEvents += fRunEdepSum[i];
    }
}

void SingleParticleSD::SaveToROOT()
{
    TFile file(fOutputFilename.c_str(), "RECREATE");
    if (!file.IsOpen())
    {
        G4cerr << "错误:无法打开ROOT文件 " << fOutputFilename << G4endl;
        return;
    }

    std::vector<G4double> x(fNbins), y(fNbins), ex(fNbins), ey(fNbins);
    for (G4int i = 0; i < fNbins; ++i)
    {
        x[i] = (i + 0.5) * fBinWidth / um;
        ex[i] = (fBinWidth / 2.0) / um;
        y[i] = fMeanEdep[i] / keV;
        ey[i] = fStdError[i] / keV;
    }

    TGraphErrors *graph = new TGraphErrors(fNbins, x.data(), y.data(), ex.data(), ey.data());
    graph->SetName("EnergyDepositGraph");
    graph->SetTitle("Mean Energy Deposit vs. Distance;Distance from Origin [um];Mean Energy Deposit per Hit [keV]");
    graph->SetMarkerStyle(20);
    graph->SetMarkerColor(kBlue);
    graph->SetLineColor(kBlue);
    graph->Write();

    TCanvas canvas("canvas", "Energy Deposit", 800, 600);
    graph->Draw("AP");
    canvas.SaveAs("Single-energy-deposit.png");
    file.Close();
    G4cout << "successfully store" << G4endl;
}

void SingleParticleSD::PrintResultsToTerminal()
{
    G4cout << "\n======================== FINAL RESULTS (Summary) ========================" << G4endl;
    G4cout << "Total energy deposited across all events: " << fTotalEdepAllEvents / keV << " keV" << G4endl;
    G4cout << "\nDetailed data per bin:" << G4endl;
    G4cout << "---------------------------------------------------------------" << G4endl;
    G4cout << std::setw(5) << "Bin" << " |"
           << std::setw(15) << "Position (um)" << " |"
           << std::setw(15) << "Mean Edep (keV)" << " |"
           << std::setw(15) << "Std Error (keV)" << " |"
           << std::setw(10) << "Hits" << G4endl;
    G4cout << "---------------------------------------------------------------" << G4endl;

    for (G4int i = 0; i < fNbins; ++i)
    {
        if (fRunHitCounts[i] > 0)
        {
            G4double x = (i + 0.5) * fBinWidth / um;
            G4double y = fMeanEdep[i] / keV;
            G4double ey = fStdError[i] / keV;

            G4cout << std::setw(5) << i << " |"
                   << std::setw(15) << x << " |"
                   << std::setw(15) << y << " |"
                   << std::setw(15) << ey << " |"
                   << std::setw(10) << fRunHitCounts[i] << G4endl;
        }
    }
    G4cout << "---------------------------------------------------------------" << G4endl;
}
