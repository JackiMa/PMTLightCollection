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
/// \file LightCollection/include/LightCollectionPrimaryGeneratorAction.hh
/// \brief Definition of the LightCollectionPrimaryGeneratorAction class
//
//
//
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#ifndef LightCollectionPrimaryGeneratorAction_h
#define LightCollectionPrimaryGeneratorAction_h 1

#include "globals.hh"
#include "G4ParticleGun.hh"
#include "G4VUserPrimaryGeneratorAction.hh"
#include "LightCollectionDetectorConstruction.hh"
#include <random>
#include "G4GeneralParticleSource.hh"

class G4Event;
class LightCollectionPrimaryGeneratorMessenger;

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

class LightCollectionPrimaryGeneratorAction : public G4VUserPrimaryGeneratorAction
{
 public:
  LightCollectionPrimaryGeneratorAction(LightCollectionDetectorConstruction* detectorConstruction);
  ~LightCollectionPrimaryGeneratorAction();

  void GeneratePrimaries(G4Event*) override;

  void SetOptPhotonPolar();
  void SetOptPhotonPolar(G4double);

  void InitializeProjectionArea();
  static bool isInitialized;

  G4ParticleGun* GetParticleGun() { return fParticleGun; }
  G4GeneralParticleSource* GetGPS() { return fGPS; }

  void UseParticleGun(G4bool useGun) { useParticleGun = useGun; }
  void SetUseParticleGun(G4bool useGun);
  G4bool GetUseParticleGun() { return useParticleGun; }

 private:
  G4ParticleGun* fParticleGun;
  G4GeneralParticleSource* fGPS; // 使用 GPS
  bool useParticleGun; // 标记使用哪种粒子源

  LightCollectionPrimaryGeneratorMessenger* fGunMessenger;
  const LightCollectionDetectorConstruction* detector;

  LightCollectionDetectorConstruction* fDetectorConstruction;
  

  G4double minX, maxX, minY, maxY, z_pos;
  std::uniform_real_distribution<> disX;
  std::uniform_real_distribution<> disY;
  std::mt19937 gen;

  
};

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#endif /*LightCollectionPrimaryGeneratorAction_h*/
