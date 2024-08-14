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
/// \file LightCollection/src/LightCollectionRun.cc
/// \brief Implementation of the LightCollectionRun class
//
//
//
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#include "LightCollectionRun.hh"
#include "G4ParticleDefinition.hh"
#include "G4Run.hh"
#include "G4UnitsTable.hh"
#include "G4AccumulableManager.hh"


//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

LightCollectionRun::LightCollectionRun()
  : G4Run(),
    fProtonDose(0.0),
    fElectronDose(0.0),
    fGammaDose(0.0),
    fShieldedProtonDose(0.0),
    fShieldedElectronDose(0.0),
    fShieldedGammaDose(0.0),
    fCrystalDose(0.0)
{
  fParticle             = nullptr;
  fEnergy               = -1.;

  fCerenkovCounter      = 0.;
  fCerenkov2            = 0.;
  fScintillationCounter = 0.;
  fScintillation2       = 0.;
  fcrystalCounter           = 0.;
  fcrystal2                 = 0.;
  fSD2_Counter            = 0.;
  fSD2_2                  = 0.;

    G4AccumulableManager* accumulableManager = G4AccumulableManager::Instance();
    accumulableManager->RegisterAccumulable(fProtonDose);
    accumulableManager->RegisterAccumulable(fElectronDose);
    accumulableManager->RegisterAccumulable(fGammaDose);
    accumulableManager->RegisterAccumulable(fShieldedProtonDose);
    accumulableManager->RegisterAccumulable(fShieldedElectronDose);
    accumulableManager->RegisterAccumulable(fShieldedGammaDose);
    accumulableManager->RegisterAccumulable(fCrystalDose);
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

LightCollectionRun::~LightCollectionRun() {}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void LightCollectionRun::SetPrimary(G4ParticleDefinition* particle, G4double energy)
{
  fParticle = particle;
  fEnergy   = energy;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void LightCollectionRun::Merge(const G4Run* aRun)
{
  const LightCollectionRun* localRun = static_cast<const LightCollectionRun*>(aRun);

  fParticle = localRun->fParticle;
  fEnergy   = localRun->fEnergy;

  fCerenkovCounter += localRun->fCerenkovCounter;
  fCerenkov2 += localRun->fCerenkov2;
  fScintillationCounter += localRun->fScintillationCounter;
  fScintillation2 += localRun->fScintillation2;
  fcrystalCounter += localRun->fcrystalCounter;
  fcrystal2 += localRun->fcrystal2;
  fSD2_Counter += localRun->fSD2_Counter;
  fSD2_2 += localRun->fSD2_2;

    AddProtonDose(localRun->fProtonDose.GetValue());
    AddElectronDose(localRun->fElectronDose.GetValue());
    AddGammaDose(localRun->fGammaDose.GetValue());
    AddShieldedProtonDose(localRun->fShieldedProtonDose.GetValue());
    AddShieldedElectronDose(localRun->fShieldedElectronDose.GetValue());
    AddShieldedGammaDose(localRun->fShieldedGammaDose.GetValue());
    AddCrystalDose(localRun->fCrystalDose.GetValue());
    

  G4Run::Merge(aRun);
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
void LightCollectionRun::EndOfRun()
{

}


void LightCollectionRun::RecordEvent(const G4Event* anEvent)
{
    // 记录事件数据
}

void LightCollectionRun::AddProtonDose(G4double dose)
{
    fProtonDose += dose;
}

void LightCollectionRun::AddElectronDose(G4double dose)
{
    fElectronDose += dose;
}

void LightCollectionRun::AddGammaDose(G4double dose)
{
    fGammaDose += dose;
}

void LightCollectionRun::AddShieldedProtonDose(G4double dose)
{
    fShieldedProtonDose += dose;
}

void LightCollectionRun::AddShieldedElectronDose(G4double dose)
{
    fShieldedElectronDose += dose;
}

void LightCollectionRun::AddShieldedGammaDose(G4double dose)
{
    fShieldedGammaDose += dose;
}

void LightCollectionRun::AddCrystalDose(G4double dose)
{
    fCrystalDose += dose;
}

G4double LightCollectionRun::GetProtonDose() const
{
    return fProtonDose.GetValue();
}

G4double LightCollectionRun::GetElectronDose() const
{
    return fElectronDose.GetValue();
}

G4double LightCollectionRun::GetGammaDose() const
{
    return fGammaDose.GetValue();
}

G4double LightCollectionRun::GetShieldedProtonDose() const
{
    return fShieldedProtonDose.GetValue();
}

G4double LightCollectionRun::GetShieldedElectronDose() const
{
    return fShieldedElectronDose.GetValue();
}

G4double LightCollectionRun::GetShieldedGammaDose() const
{
    return fShieldedGammaDose.GetValue();
}

G4double LightCollectionRun::GetCrystalDose() const
{
    return fCrystalDose.GetValue();
}