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
/// \file optical/LightCollection/include/LightCollectionEventAction.hh
/// \brief Definition of the LightCollectionEventAction class
//
//
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#ifndef LightCollectionEventAction_h
#define LightCollectionEventAction_h 1

#include "globals.hh"
#include "G4UserEventAction.hh"

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

class LightCollectionEventAction : public G4UserEventAction
{
 public:
  LightCollectionEventAction();
  ~LightCollectionEventAction();

  void BeginOfEventAction(const G4Event*) override;
  void EndOfEventAction(const G4Event*) override;

  void Addcrystal() { fcrystalCounts = fcrystalCounts+1; }
  void AddSD2() { fSD2_Counts = fSD2_Counts+1; }

  void AddProtonDose(G4double dose);
  void AddElectronDose(G4double dose);
  void AddGammaDose(G4double dose);
  void AddShieldedProtonDose(G4double dose);
  void AddShieldedElectronDose(G4double dose);
  void AddShieldedGammaDose(G4double dose);
  void AddCrystalDose(G4double dose);

 private:
  G4int fcrystalCounts;
  G4int fSD2_Counts;
  G4double fProtonDose;
  G4double fElectronDose;
  G4double fGammaDose;
  G4double fShieldedProtonDose;
  G4double fShieldedElectronDose;
  G4double fShieldedGammaDose;
  G4double fCrystalDose;
};
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
#endif
