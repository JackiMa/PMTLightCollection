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
/// \file LightCollection/src/LightCollectionPrimaryGeneratorAction.cc
/// \brief Implementation of the LightCollectionPrimaryGeneratorAction class
//
//
//
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#include "LightCollectionPrimaryGeneratorAction.hh"
#include "LightCollectionPrimaryGeneratorMessenger.hh"

#include "G4Event.hh"
#include "G4ParticleDefinition.hh"
#include "G4ParticleGun.hh"
#include "G4GeneralParticleSource.hh"
#include "G4ParticleTable.hh"
#include "G4SystemOfUnits.hh"
#include "G4Box.hh"
#include "G4Tubs.hh"
#include "G4VPhysicalVolume.hh"
#include "G4LogicalVolume.hh"
#include "G4RunManager.hh"
#include "G4AnalysisManager.hh"

#include "Randomize.hh"

#include "utilities.hh"
#include "MyPhysicalVolume.hh"
#include "config.hh"

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
LightCollectionPrimaryGeneratorAction::LightCollectionPrimaryGeneratorAction(LightCollectionDetectorConstruction* detectorConstruction)
  :G4VUserPrimaryGeneratorAction()
  , fParticleGun(nullptr)
  , useParticleGun(false)
  , fDetectorConstruction(detectorConstruction)
{
    fGunMessenger = new LightCollectionPrimaryGeneratorMessenger(this);

    // Initialize parameters
    G4int n_particle = 1;
    G4ParticleDefinition* particle = G4ParticleTable::GetParticleTable()->FindParticle("gamma");
    G4ThreeVector source_position = G4ThreeVector(0., 0., 16.*mm);
    G4double energy = 0.1*keV;
    
    // ParticleGun
    fParticleGun = new G4ParticleGun(n_particle);
    fParticleGun->SetParticleDefinition(particle);
    fParticleGun->SetParticlePosition(source_position);
    fParticleGun->SetParticleEnergy(energy);

    // GPS
    fGPS = new G4GeneralParticleSource();
    fGPS->SetParticleDefinition(particle);
    fGPS->GetCurrentSource()->GetPosDist()->SetCentreCoords(source_position);
    fGPS->GetCurrentSource()->GetEneDist()->SetMonoEnergy(energy);
    fGPS->GetCurrentSource()->GetAngDist()->SetParticleMomentumDirection(G4ThreeVector(0., 0., 1.));

}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
LightCollectionPrimaryGeneratorAction::~LightCollectionPrimaryGeneratorAction()
{
  delete fParticleGun;
  delete fGPS;
  delete fGunMessenger;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
void LightCollectionPrimaryGeneratorAction::GeneratePrimaries(G4Event* anEvent)
{

  G4RunManager* runManager = G4RunManager::GetRunManager();
  detector = dynamic_cast<const LightCollectionDetectorConstruction*>(runManager->GetUserDetectorConstruction());
  if (!detector) {
    G4ExceptionDescription msg;
    msg << "Detector construction is not found!";
    G4Exception("LightCollectionPrimaryGeneratorAction::GeneratePrimaries()", "LightCollection_001", FatalException, msg);
  }

  InitializeProjectionArea();
  
    // 在投影区域内随机抽样一个点
    G4double x = disX(gen);
    G4double y = disY(gen);

    // 将抽样得到的放射源位置XY填入到H2中
    auto analysisManager = G4AnalysisManager::Instance();
    analysisManager->FillH2(0, x, y);


    MyPhysicalVolume* p_shield = detector->GetMyVolume("shield");
    G4Box* shield_box = dynamic_cast<G4Box*>(p_shield->GetLogicalVolume()->GetSolid());
    z_pos = p_shield->GetAbsolutePosition().z() + shield_box->GetZHalfLength()+5*mm;

    G4ThreeVector sourcePosition(x, y, z_pos);

    // 设置射线的方向
    G4ThreeVector direction(0, 0, -1);

    if (useParticleGun)
    {
    fParticleGun->SetParticlePosition(sourcePosition);
    fParticleGun->SetParticleMomentumDirection(direction);
    fParticleGun->GeneratePrimaryVertex(anEvent);
    }
    else
    {
    fGPS->GetCurrentSource()->GetPosDist()->SetPosDisType("Point");
    fGPS->GetCurrentSource()->GetPosDist()->SetCentreCoords(sourcePosition);
    fGPS->GetCurrentSource()->GetAngDist()->SetParticleMomentumDirection(direction);
    fGPS->GeneratePrimaryVertex(anEvent);
    }


}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
void LightCollectionPrimaryGeneratorAction::SetOptPhotonPolar()
{
  G4double angle = G4UniformRand() * 360.0 * deg;
  SetOptPhotonPolar(angle);
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
void LightCollectionPrimaryGeneratorAction::SetOptPhotonPolar(G4double angle)
{
  if(fParticleGun->GetParticleDefinition()->GetParticleName() !=
     "opticalphoton")
  {
    G4ExceptionDescription ed;
    ed << "Warning: the particleGun is not an opticalphoton";
    G4Exception("LightCollectionPrimaryGeneratorAction::SetOptPhotonPolar()",
                "LightCollection_010", JustWarning, ed);
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


void LightCollectionPrimaryGeneratorAction::InitializeProjectionArea() {

  if (isInitialized) return;
    isInitialized = true;
    // 获取闪烁体的位置和形状
    MyPhysicalVolume* physicalScintillator = detector->GetMyVolume("sc_crystal");
    G4ThreeVector position = physicalScintillator->GetAbsolutePosition();
    G4VSolid* solid = physicalScintillator->GetLogicalVolume()->GetSolid();
    G4RotationMatrix* rotation = physicalScintillator->GetAbsoluteRotation();

    if (G4Box* box = dynamic_cast<G4Box*>(solid)) {
        // 闪烁体是 G4Box
        G4double halfX = box->GetXHalfLength();
        G4double halfY = box->GetYHalfLength();
        G4double halfZ = box->GetZHalfLength();

        // 计算投影区域
        G4ThreeVector corners[8] = {
            G4ThreeVector(-halfX, -halfY, -halfZ),
            G4ThreeVector(halfX, -halfY, -halfZ),
            G4ThreeVector(-halfX, halfY, -halfZ),
            G4ThreeVector(halfX, halfY, -halfZ),
            G4ThreeVector(-halfX, -halfY, halfZ),
            G4ThreeVector(halfX, -halfY, halfZ),
            G4ThreeVector(-halfX, halfY, halfZ),
            G4ThreeVector(halfX, halfY, halfZ)
        };

        minX = DBL_MAX, maxX = -DBL_MAX;
        minY = DBL_MAX, maxY = -DBL_MAX;

        for (auto& corner : corners) {
            if (rotation) {
                corner = (*rotation) * corner;
            }
            corner += position;
            if (corner.x() < minX) minX = corner.x();
            if (corner.x() > maxX) maxX = corner.x();
            if (corner.y() < minY) minY = corner.y();
            if (corner.y() > maxY) maxY = corner.y();
        }

    } else if (G4Tubs* tubs = dynamic_cast<G4Tubs*>(solid)) {
        // 闪烁体是 G4Tubs
        G4double rMax = tubs->GetOuterRadius();
        G4double halfZ = tubs->GetZHalfLength();
        // 默认圆柱体都是竖着的，所以halfZ就是圆柱体的半长度

        // 计算投影区域
        minX = position.x() - halfZ;
        maxX = position.x() + halfZ;
        minY = position.y() - rMax;
        maxY = position.y() + rMax;
    }
  else {
      G4ExceptionDescription msg;
      msg << "Invalid entity type, current entity type is: " << solid->GetEntityType();
      G4Exception("LightCollectionPrimaryGeneratorAction::InitializeProjectionArea()", 
                  "LightCollection_001", FatalException, msg);
  }


// 计算原来的中心点
G4double centerX = 0.5 * (minX + maxX);
G4double centerY = 0.5 * (minY + maxY);

// 计算新的范围
G4double newMinX = centerX - (centerX - minX) * g_source_scale;
G4double newMaxX = centerX + (maxX - centerX) * g_source_scale;
G4double newMinY = centerY - (centerY - minY) * g_source_scale;
G4double newMaxY = centerY + (maxY - centerY) * g_source_scale;

// 创建新的分布
disX = std::uniform_real_distribution<>(newMinX, newMaxX);
disY = std::uniform_real_distribution<>(newMinY, newMaxY);

    G4cout << "Projection area initialized: " << minX << " " << maxX << " " << minY << " " << maxY << G4endl;
}

void LightCollectionPrimaryGeneratorAction::SetUseParticleGun(G4bool useGun)
{
    useParticleGun = useGun;
}