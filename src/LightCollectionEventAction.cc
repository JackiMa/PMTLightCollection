//
// ********************************************************************
// * License and Disclaimer                                           *
// *                                                                  *
// * The  Geant4 software  is  copyright of the Copyright Holders  of *
// * the Geant4 Collaboration.  It is provided  under  the terms  and *
// * conditions of the Geant4 Software License,  included in the file *
// * LICENSE and available at  http://cern.ch/geant4/license .  These *
// * include a list of copyright holders.                             *
// *                                                                  *
// * Neither the authors of this software system, nor their employing *
// * institutes,nor the agencies providing financial support for this *
// * work  make  any representation or  warranty, express or implied, *
// * regarding  this  software system or assume any liability for its *
// * use.  Please see the license in the file  LICENSE  and URL above *
// * for the full disclaimer and the limitation of liability.         *
// *                                                                  *
// * This  code  implementation is the result of  the  scientific and *
// * technical work of the GEANT4 collaboration.                      *
// * By using,  copying,  modifying or  distributing the software (or *
// * any work based  on the software)  you  agree  to acknowledge its *
// * use  in  resulting  scientific  publications,  and indicate your *
// * acceptance of all terms of the Geant4 Software license.          *
// ********************************************************************
//
/// \file optical/LightCollection/src/LightCollectionEventAction.cc
/// \brief Implementation of the LightCollectionEventAction class
//
//
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#include "LightCollectionEventAction.hh"
#include "LightCollectionRun.hh"
#include "LightCollectionStackingAction.hh"
#include "G4Event.hh"
#include "G4RunManager.hh"

#include "G4Threading.hh"
#include "G4AnalysisManager.hh"

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
LightCollectionEventAction::LightCollectionEventAction()
    : G4UserEventAction(),
      fProtonDose(0.0),
      fElectronDose(0.0),
      fGammaDose(0.0),
      fShieldedProtonDose(0.0),
      fShieldedElectronDose(0.0),
      fShieldedGammaDose(0.0),
      fCrystalDose(0.0)
{
  fcrystalCounts = 0;
  fSD2_Counts = 0;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
LightCollectionEventAction::~LightCollectionEventAction() {}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
void LightCollectionEventAction::BeginOfEventAction(const G4Event *)
{
  fcrystalCounts = 0;
  fSD2_Counts = 0;

  fProtonDose = 0.0;
  fElectronDose = 0.0;
  fGammaDose = 0.0;
  fShieldedProtonDose = 0.0;
  fShieldedElectronDose = 0.0;
  fShieldedGammaDose = 0.0;
  fCrystalDose = 0.0;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
void LightCollectionEventAction::EndOfEventAction(const G4Event *event)
{
    // Print per event (modulo n)
    // 判断是否是主进程，在主进程中打印进度
    if (G4Threading::IsMasterThread()) {
      auto eventID = event->GetEventID();
      auto totalEvents = G4RunManager::GetRunManager()->GetCurrentRun()->GetNumberOfEventToBeProcessed();
      auto printModulo = totalEvents / 100; // 每1%的事件数
      if ((printModulo > 0) && (eventID % printModulo == 0))
      {
          G4cout << "---> End of event: " << eventID << ", " << (eventID / printModulo) << "% completed" << std::endl;
      }
    }


  LightCollectionRun *run = static_cast<LightCollectionRun *>(
      G4RunManager::GetRunManager()->GetNonConstCurrentRun());
  run->Addcrystal(fcrystalCounts);
  run->AddSD2(fSD2_Counts);

    run->AddProtonDose(fProtonDose);
    run->AddElectronDose(fElectronDose);
    run->AddGammaDose(fGammaDose);
    run->AddShieldedProtonDose(fShieldedProtonDose);
    run->AddShieldedElectronDose(fShieldedElectronDose);
    run->AddShieldedGammaDose(fShieldedGammaDose);
    run->AddCrystalDose(fCrystalDose);

  G4int sc_PhotonCount_ID, sc_Wavelengths_ID;
  G4AnalysisManager *analysisManager = G4AnalysisManager::Instance();
  sc_PhotonCount_ID = 0; // 假设 ntuple ID 0 对应 NPhotonGenerated
  sc_Wavelengths_ID = analysisManager->GetH1Id("ScintillationWavelength");
    auto stackingAction = static_cast<const LightCollectionStackingAction*>(
        G4RunManager::GetRunManager()->GetUserStackingAction());

    // 获取闪烁光的光子数和波长
    G4int scintillationPhotonCount = stackingAction->GetScintillationPhotonCount();
    const auto& scintillationWavelengths = stackingAction->GetScintillationWavelengths();

    // 填充闪烁光的光子数
    // analysisManager->FillNtupleDColumn(sc_PhotonCount_ID, scintillationPhotonCount);

    // 填充闪烁光的波长
    for (auto wavelength : scintillationWavelengths) {
        analysisManager->FillH1(sc_Wavelengths_ID, wavelength); // 假设直方图 ID 0 对应 ScintillationWavelength
    }
}
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void LightCollectionEventAction::AddProtonDose(G4double dose)
{
    fProtonDose += dose;
}

void LightCollectionEventAction::AddElectronDose(G4double dose)
{
    fElectronDose += dose;
}

void LightCollectionEventAction::AddGammaDose(G4double dose)
{
    fGammaDose += dose;
}

void LightCollectionEventAction::AddShieldedProtonDose(G4double dose)
{
    fShieldedProtonDose += dose;
}

void LightCollectionEventAction::AddShieldedElectronDose(G4double dose)
{
    fShieldedElectronDose += dose;
}

void LightCollectionEventAction::AddShieldedGammaDose(G4double dose)
{
    fShieldedGammaDose += dose;
}

void LightCollectionEventAction::AddCrystalDose(G4double dose)
{
    fCrystalDose += dose;
}