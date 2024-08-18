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
/// \file LightCollection/include/LightCollectionDetectorConstruction.hh
/// \brief Definition of the LightCollectionDetectorConstruction class
//
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#ifndef LightCollectionDetectorConstruction_h
#define LightCollectionDetectorConstruction_h 1

#include "globals.hh"
#include "G4VUserDetectorConstruction.hh"
#include "G4Material.hh"
#include "G4OpticalSurface.hh"
#include "MyPhysicalVolume.hh"
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

class LightCollectionDetectorMessenger;
class G4GlobalMagFieldMessenger;

class LightCollectionDetectorConstruction : public G4VUserDetectorConstruction
{
 public:
  LightCollectionDetectorConstruction();
  ~LightCollectionDetectorConstruction();

  G4VPhysicalVolume* Construct() override;
  void ConstructSDandField() override;

  void SetDumpGdml(G4bool);
  G4bool IsDumpGdml() const;
  void SetVerbose(G4bool verbose);
  G4bool IsVerbose() const;
  void SetDumpGdmlFile(G4String);
  G4String GetDumpGdmlFile() const;

  MyPhysicalVolume* GetMyVolume(G4String volumeName) const;

  

 private:
  void PrintError(G4String);

  std::map<G4String, MyPhysicalVolume*> fVolumeMap; // 维护需要别处引用的Solid

  LightCollectionDetectorMessenger* fDetectorMessenger;
  G4String fDumpGdmlFileName;

  G4bool fVerbose;
  G4bool fDumpGdml;

};

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
#endif /*LightCollectionDetectorConstruction_h*/
