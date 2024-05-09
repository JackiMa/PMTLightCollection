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
/// \file OpNovice/include/OpNoviceRun.hh
/// \brief Definition of the OpNoviceRun class
//
//
//
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#ifndef OpNoviceRun_h
#define OpNoviceRun_h 1

#include "G4Run.hh"

class G4ParticleDefinition;

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

class OpNoviceRun : public G4Run
{
 public:
  OpNoviceRun();
  ~OpNoviceRun();

  void SetPrimary(G4ParticleDefinition* particle, G4double energy);

  void AddCerenkov(G4double n)
  {
    fCerenkovCounter += n;
    fCerenkov2 += n * n;
  };
  void AddScintillation(G4double n)
  {
    fScintillationCounter += n;
    fScintillation2 += n * n;
  };
  void AddPMT(G4double n)
  {
    fPMTCounter += n;
    fPMT2 += n * n;
  };
  void AddSensitiveVolume(G4double n)
  {
    fSVCounter += n;
    fSV2 += n * n;
  };

  void Merge(const G4Run*) override;
  void EndOfRun();

 public:
  G4ParticleDefinition* fParticle;
  G4double fEnergy;

  G4double fCerenkovCounter;
  G4double fCerenkov2;
  G4double fScintillationCounter;
  G4double fScintillation2;
  G4double fPMTCounter;
  G4double fPMT2;
  G4double fSVCounter;
  G4double fSV2;

};
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
#endif
