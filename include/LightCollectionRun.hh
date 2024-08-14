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
/// \file LightCollection/include/LightCollectionRun.hh
/// \brief Definition of the LightCollectionRun class
//
//
//
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#ifndef LightCollectionRun_h
#define LightCollectionRun_h 1

#include "G4Accumulable.hh"  
#include "G4Run.hh"
#include "globals.hh"

class G4ParticleDefinition;

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

class LightCollectionRun : public G4Run
{
 public:
  LightCollectionRun();
  ~LightCollectionRun();

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
  void Addcrystal(G4double n)
  {
    fcrystalCounter += n;
    fcrystal2 += n * n;
  };
  void AddSD2(G4double n)
  {
    fSD2_Counter += n;
    fSD2_2 += n * n;
  };

  virtual void Merge(const G4Run*) override;
  virtual void RecordEvent(const G4Event*) override;
  void EndOfRun();

 public:
  G4ParticleDefinition* fParticle;
  G4double fEnergy;

  G4double fCerenkovCounter;
  G4double fCerenkov2;
  G4double fScintillationCounter;
  G4double fScintillation2;
  G4double fcrystalCounter;
  G4double fcrystal2;
  G4double fSD2_Counter;
  G4double fSD2_2;

    void AddProtonDose(G4double dose);
    void AddElectronDose(G4double dose);
    void AddGammaDose(G4double dose);
    void AddShieldedProtonDose(G4double dose);
    void AddShieldedElectronDose(G4double dose);
    void AddShieldedGammaDose(G4double dose);
    void AddCrystalDose(G4double dose);

    G4double GetProtonDose() const;
    G4double GetElectronDose() const;
    G4double GetGammaDose() const;
    G4double GetShieldedProtonDose() const;
    G4double GetShieldedElectronDose() const;
    G4double GetShieldedGammaDose() const;
    G4double GetCrystalDose() const;

  private:
    G4Accumulable<G4double> fProtonDose;
    G4Accumulable<G4double> fElectronDose;
    G4Accumulable<G4double> fGammaDose;
    G4Accumulable<G4double> fShieldedProtonDose;
    G4Accumulable<G4double> fShieldedElectronDose;
    G4Accumulable<G4double> fShieldedGammaDose;
    G4Accumulable<G4double> fCrystalDose;

};
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
#endif
