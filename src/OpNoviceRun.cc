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
/// \file OpNovice/src/OpNoviceRun.cc
/// \brief Implementation of the OpNoviceRun class
//
//
//
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#include "OpNoviceRun.hh"
#include "G4ParticleDefinition.hh"
#include "G4Run.hh"
#include "G4UnitsTable.hh"

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

OpNoviceRun::OpNoviceRun()
  : G4Run()
{
  fParticle             = nullptr;
  fEnergy               = -1.;

  fCerenkovCounter      = 0.;
  fCerenkov2            = 0.;
  fScintillationCounter = 0.;
  fScintillation2       = 0.;
  fPMTCounter           = 0.;
  fPMT2                 = 0.;
  fSVCounter            = 0.;
  fSV2                  = 0.;

}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

OpNoviceRun::~OpNoviceRun() {}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void OpNoviceRun::SetPrimary(G4ParticleDefinition* particle, G4double energy)
{
  fParticle = particle;
  fEnergy   = energy;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void OpNoviceRun::Merge(const G4Run* run)
{
  const OpNoviceRun* localRun = static_cast<const OpNoviceRun*>(run);

  fParticle = localRun->fParticle;
  fEnergy   = localRun->fEnergy;

  fCerenkovCounter += localRun->fCerenkovCounter;
  fCerenkov2 += localRun->fCerenkov2;
  fScintillationCounter += localRun->fScintillationCounter;
  fScintillation2 += localRun->fScintillation2;
  fPMTCounter += localRun->fPMTCounter;
  fPMT2 += localRun->fPMT2;
  fSVCounter += localRun->fSVCounter;
  fSV2 += localRun->fSV2;


  G4Run::Merge(run);
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
void OpNoviceRun::EndOfRun()
{

}
