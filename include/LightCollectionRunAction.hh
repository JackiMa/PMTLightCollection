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
/// \file LightCollection/include/LightCollectionRunAction.hh
/// \brief Definition of the LightCollectionRunAction class
//
//
//
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#ifndef LightCollectionRunAction_h
#define LightCollectionRunAction_h 1

#include "globals.hh"
#include "G4UserRunAction.hh"
#include "G4Accumulable.hh"
#include <fstream>


#include "G4RootAnalysisManager.hh"

class LightCollectionPrimaryGeneratorAction;
class LightCollectionRun;

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

class G4Run;

class LightCollectionRunAction : public G4UserRunAction
{
 public:
  LightCollectionRunAction(LightCollectionPrimaryGeneratorAction* = nullptr);
  ~LightCollectionRunAction();

  G4Run* GenerateRun() override;
  void BeginOfRunAction(const G4Run*) override;
  void EndOfRunAction(const G4Run*) override;

  G4RootAnalysisManager* analysisManager;

 private:
  LightCollectionRun* fRun;
  LightCollectionPrimaryGeneratorAction* fPrimary;

  std::ofstream outputFile;

  bool fileExists(const std::string& fileName);
  G4String getNewfileName(G4String baseFileName = "LightCollection");

  G4Accumulable<double> fProtonDose;
  G4Accumulable<double> fElectronDose;
  G4Accumulable<double> fGammaDose;
  G4Accumulable<double> fShieldedProtonDose;
  G4Accumulable<double> fShieldedElectronDose;
  G4Accumulable<double> fShieldedGammaDose;
  G4Accumulable<double> fCrystalDose;



};


//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
#endif
