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
/// \file OpNovice/include/OpNoviceDetectorConstruction.hh
/// \brief Definition of the OpNoviceDetectorConstruction class
//
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#ifndef OpNoviceDetectorConstruction_h
#define OpNoviceDetectorConstruction_h 1

#include "globals.hh"
#include "G4VUserDetectorConstruction.hh"
#include "G4Material.hh"
#include "G4OpticalSurface.hh"
#include "MyPhysicalVolume.hh"
#include "G4GenericMessenger.hh"
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

class OpNoviceDetectorMessenger;

class OpNoviceDetectorConstruction : public G4VUserDetectorConstruction
{
 public:
  OpNoviceDetectorConstruction();
  ~OpNoviceDetectorConstruction();

  G4VPhysicalVolume* Construct() override;
  void SetDumpGdml(G4bool);
  G4bool IsDumpGdml() const;
  void SetVerbose(G4bool verbose);
  G4bool IsVerbose() const;
  void SetDumpGdmlFile(G4String);
  G4String GetDumpGdmlFile() const;

  MyPhysicalVolume* GetMyVolume(G4String volumeName) const;

 private:
  void PrintError(G4String);
  G4Material* matter_construct_water();

  std::map<G4String, MyPhysicalVolume*> fVolumeMap; // 维护需要别处引用的Solid

  OpNoviceDetectorMessenger* fDetectorMessenger;
  G4String fDumpGdmlFileName;

  G4bool fVerbose;
  G4bool fDumpGdml;

// 反光罩的形式，立方体、圆柱体、半球体
std::map<std::string, int> reflectorShapeMap = {
    {"NONE", -1},
    {"CUBE", 1},
    {"CYLINDER", 2},
    {"HEMISPHERE", 3}
};
std::string reflectorShape;

// 反光罩的材料
std::map<std::string, int> reflectorMaterialMap = {
    {"NO_MATERIAL", -1},
    {"TEFLON", 1},
    {"TiO2", 2},
    {"ESR", 3}
};
std::string reflectorMaterial;


  G4GenericMessenger* messenger; // 用于与mac文件进行交互

  bool useGrease;
  bool isAirGap;
  G4double scintillatorX;
  G4double scintillatorY;
  G4double scintillatorZ;
  G4ThreeVector scintillatorSize;
  G4ThreeVector reflectorSize;

  public:
  // 适用mac文件进行控制
  void SetReflectorShape(G4String reflectorShape);
  void SetReflectorMaterial(G4String reflectorMaterial);
  void SetScintillatorSize(G4ThreeVector size);
  void SetReflectorSize(G4ThreeVector size);
  void UseGrease(G4bool use);
};

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#endif /*OpNoviceDetectorConstruction_h*/
