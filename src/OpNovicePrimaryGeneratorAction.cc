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
/// \file OpNovice/src/OpNovicePrimaryGeneratorAction.cc
/// \brief Implementation of the OpNovicePrimaryGeneratorAction class
//
//
//
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#include "OpNovicePrimaryGeneratorAction.hh"
#include "OpNovicePrimaryGeneratorMessenger.hh"
#include "G4Event.hh"
#include "G4ParticleDefinition.hh"
#include "G4ParticleGun.hh"
#include "G4ParticleTable.hh"
#include "G4SystemOfUnits.hh"
#include "Randomize.hh"

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
OpNovicePrimaryGeneratorAction::OpNovicePrimaryGeneratorAction()
  : G4VUserPrimaryGeneratorAction()
  , fParticleGun(nullptr)
{
  G4int n_particle = 1;
  fParticleGun     = new G4ParticleGun(n_particle);
  // create a messenger for this class
  fGunMessenger = new OpNovicePrimaryGeneratorMessenger(this);
  // default kinematic
  //

  G4ParticleDefinition* particle = G4ParticleTable::GetParticleTable()->FindParticle("e-");
  fParticleGun->SetParticleDefinition(particle);

    // 设置粒子发射位置
  fParticleGun->SetParticlePosition(G4ThreeVector(0., 0., -1.45*cm));

  G4ThreeVector direction(0.4, 0.1, 0.8);
  fParticleGun->SetParticleMomentumDirection(direction);

  // 随机设置粒子能量
  G4double energy = 1*keV;
  fParticleGun->SetParticleEnergy(energy);

}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
OpNovicePrimaryGeneratorAction::~OpNovicePrimaryGeneratorAction()
{
  delete fParticleGun;
  delete fGunMessenger;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
void OpNovicePrimaryGeneratorAction::GeneratePrimaries(G4Event* anEvent)
{
  // 设置粒子类型为电子


  // // 设置粒子发射位置
  // fParticleGun->SetParticlePosition(G4ThreeVector(0., 0., -2*cm+7.*mm));

  // // 随机设置粒子发射方向
  // G4double phi = G4UniformRand() * 2 * M_PI;
  // G4double costheta = 2 * G4UniformRand() - 1.;
  // G4double sintheta = std::sqrt(1. - costheta * costheta);
  // G4ThreeVector direction(sintheta * std::cos(phi), sintheta * std::sin(phi), costheta);
  // fParticleGun->SetParticleMomentumDirection(direction);

  // // 随机设置粒子能量
  // G4double energy = 1 * keV + G4UniformRand() * (1000 * keV - 1 * keV);
  // fParticleGun->SetParticleEnergy(energy);





  // 生成粒子
  fParticleGun->GeneratePrimaryVertex(anEvent);
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
void OpNovicePrimaryGeneratorAction::SetOptPhotonPolar()
{
  G4double angle = G4UniformRand() * 360.0 * deg;
  SetOptPhotonPolar(angle);
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
void OpNovicePrimaryGeneratorAction::SetOptPhotonPolar(G4double angle)
{
  if(fParticleGun->GetParticleDefinition()->GetParticleName() !=
     "opticalphoton")
  {
    G4ExceptionDescription ed;
    ed << "Warning: the particleGun is not an opticalphoton";
    G4Exception("OpNovicePrimaryGeneratorAction::SetOptPhotonPolar()",
                "OpNovice_010", JustWarning, ed);
    return;
  }

  G4ThreeVector normal(1., 0., 0.);
  G4ThreeVector kphoton = fParticleGun->GetParticleMomentumDirection();
  G4ThreeVector product = normal.cross(kphoton);
  G4double modul2       = product * product;

  G4ThreeVector e_perpend(0., 0., 1.);
  if(modul2 > 0.)
    e_perpend = (1. / std::sqrt(modul2)) * product;
  G4ThreeVector e_paralle = e_perpend.cross(kphoton);

  G4ThreeVector polar =
    std::cos(angle) * e_paralle + std::sin(angle) * e_perpend;
  fParticleGun->SetParticlePolarization(polar);
}
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
