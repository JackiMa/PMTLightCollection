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
/// \file LightCollection/src/LightCollectionStackingAction.cc
/// \brief Implementation of the LightCollectionStackingAction class
//
//
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#include "LightCollectionStackingAction.hh"
#include "LightCollectionRun.hh"
#include "G4ios.hh"
#include "G4OpticalPhoton.hh"
#include "G4RunManager.hh"
#include "G4Track.hh"
#include "G4VProcess.hh"
#include "G4UnitsTable.hh"
#include "G4SystemOfUnits.hh"
#include "G4AnalysisManager.hh"


#include "utilities.hh"
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

LightCollectionStackingAction::LightCollectionStackingAction()
    : G4UserStackingAction(), fScintillationPhotonCount(0)
{
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
LightCollectionStackingAction::~LightCollectionStackingAction() {}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
G4ClassificationOfNewTrack LightCollectionStackingAction::ClassifyNewTrack(
    const G4Track *aTrack)
{
  if (aTrack->GetDefinition() == G4OpticalPhoton::OpticalPhotonDefinition())
  { // particle is optical photon
    if (aTrack->GetParentID() > 0)
    { // particle is secondary
      if (aTrack->GetCreatorProcess()->GetProcessName() == "Scintillation")
      {
        ++fScintillationPhotonCount;
        G4double wavelength = (1.239841939 * keV) / aTrack->GetKineticEnergy();
        fScintillationWavelengths.push_back(wavelength);
        myPrint(lv, "Scintillation photon detected with wavelength: " + std::to_string(wavelength / nm) + " nm & kineticE = "+std::to_string(aTrack->GetKineticEnergy()/eV)+" eV\n");
      }
      else if (aTrack->GetCreatorProcess()->GetProcessName() == "Cerenkov")
      {
        return fKill; // kill the particle if it is created by Cerenkov process
      }
    }
  }
  return fUrgent;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
void LightCollectionStackingAction::NewStage()
{
  // 当前阶段（堆栈）处理完后执行
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
void LightCollectionStackingAction::PrepareNewEvent()
{
  fScintillationPhotonCount = 0;
}
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
G4int LightCollectionStackingAction::GetScintillationPhotonCount() const {
    return fScintillationPhotonCount;
}

std::vector<G4double> LightCollectionStackingAction::GetScintillationWavelengths() const {
    return fScintillationWavelengths;
}