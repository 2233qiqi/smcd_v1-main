#include "RadialEnergyDepositSD.hh"
#include "G4Step.hh"
#include "G4SystemOfUnits.hh"
#include "TFile.h"
#include "TH1D.h"
#include <algorithm>

RadialEnergyDepositSD::RadialEnergyDepositSD(const G4String& name, 
                                           G4int nbins, 
                                           G4double rmax)
    : G4VSensitiveDetector(name),
      fNbins(nbins),
      fRmax(rmax),
      fBinWidth(rmax/nbins),
      fEnergyDeposit(nbins, 0.0),
      fNormalizedEdep(nbins, 0.0)
{
}

RadialEnergyDepositSD::~RadialEnergyDepositSD()
{
    // 自动保存最终结果
    SaveToROOT("radial_edep_final.root");
}

void RadialEnergyDepositSD::Initialize(G4HCofThisEvent*)
{
    std::fill(fEnergyDeposit.begin(), fEnergyDeposit.end(), 0.0);
}

G4bool RadialEnergyDepositSD::ProcessHits(G4Step* step, G4TouchableHistory*)
{
    G4double edep = step->GetTotalEnergyDeposit();
    if (edep == 0.) return false;
    
    G4ThreeVector pos = step->GetPreStepPoint()->GetPosition();
    G4double r = pos.perp();
    
    if (r < fRmax) {
        G4int bin = static_cast<G4int>(r / fBinWidth);
        if (bin >= fNbins) bin = fNbins - 1;
        fEnergyDeposit[bin] += edep;
    }
    
    return true;
}

void RadialEnergyDepositSD::EndOfEvent(G4HCofThisEvent*)
{
    // 可在此处添加每事件结束时的处理
}

void RadialEnergyDepositSD::NormalizePerEvent(G4int totalEvents)
{
    if(totalEvents <= 0) return;
    
    for(G4int i=0; i<fNbins; ++i) {
        fNormalizedEdep[i] = fEnergyDeposit[i] / totalEvents;
    }
}

void RadialEnergyDepositSD::NormalizePerPrimaryEnergy(G4double primaryEnergy)
{
    if(primaryEnergy <= 0) return;
    
    G4double totalEdep = 0.0;
    for(const auto& edep : fEnergyDeposit) {
        totalEdep += edep;
    }
    
    if(totalEdep <= 0) return;
    
    for(G4int i=0; i<fNbins; ++i) {
        fNormalizedEdep[i] = fEnergyDeposit[i] / (primaryEnergy * (fEnergyDeposit[i]/totalEdep));
    }
}

void RadialEnergyDepositSD::SaveToROOT(const G4String& filename)
{
    TFile file(filename.c_str(), "RECREATE");
    
    // 原始能量沉积分布
    TH1D* histRaw = new TH1D("radial_edep_raw", 
                            "Raw Radial Energy Deposit;Radius [mm];Energy Deposit [MeV]", 
                            fNbins, 0, fRmax);
    
    // 归一化能量沉积分布
    TH1D* histNorm = new TH1D("radial_edep_norm", 
                             "Normalized Radial Energy Deposit;Radius [mm];Normalized Energy Deposit", 
                             fNbins, 0, fRmax);
    
    for(G4int i=0; i<fNbins; ++i) {
        histRaw->SetBinContent(i+1, fEnergyDeposit[i]/MeV);
        histNorm->SetBinContent(i+1, fNormalizedEdep[i]);
    }
    
    histRaw->Write();
    histNorm->Write();
    file.Close();
}