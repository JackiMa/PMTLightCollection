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
/// \file optical/OpNovice/src/OpNoviceEventAction.cc
/// \brief Implementation of the OpNoviceEventAction class
//
//
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#include "OpNoviceEventAction.hh"
#include "OpNoviceRun.hh"
#include "G4Event.hh"
#include "G4RunManager.hh"

#include "G4AnalysisManager.hh"

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
OpNoviceEventAction::OpNoviceEventAction()
  : G4UserEventAction()
{
  fPMTCounts   = 0;
  fSVCounts    = 0;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
OpNoviceEventAction::~OpNoviceEventAction() {}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
void OpNoviceEventAction::BeginOfEventAction(const G4Event*)
{
  fPMTCounts   = 0;
  fSVCounts    = 0;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
void OpNoviceEventAction::EndOfEventAction(const G4Event*)
{
  OpNoviceRun* run = static_cast<OpNoviceRun*>(
    G4RunManager::GetRunManager()->GetNonConstCurrentRun());
  run->AddPMT(fPMTCounts);
  run->AddSensitiveVolume(fSVCounts);
  
  G4int id;
  G4AnalysisManager* analysisManager = G4AnalysisManager::Instance();
  id = analysisManager->GetH1Id("hPhotonGenerated");
  analysisManager->FillH1(id, fPMTCounts);
  id = analysisManager->GetH1Id("hPhotonDetected");
  analysisManager->FillH1(id, 555);  // 假定hPhoton的ID是0
}
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
