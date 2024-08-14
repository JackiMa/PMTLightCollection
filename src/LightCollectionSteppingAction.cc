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
//
/// \file LightCollectionSteppingAction.cc
/// \brief Implementation of the LightCollectionSteppingAction class

#include "LightCollectionSteppingAction.hh"
#include "LightCollectionRun.hh"
#include "G4Event.hh"
#include "G4OpBoundaryProcess.hh"
#include "G4OpticalPhoton.hh"
#include "G4RunManager.hh"
#include "G4Step.hh"
#include "G4Track.hh"

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

LightCollectionSteppingAction::LightCollectionSteppingAction(LightCollectionEventAction* event)
  : G4UserSteppingAction()
  , fEventAction(event)
{}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
LightCollectionSteppingAction::~LightCollectionSteppingAction() {}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
void LightCollectionSteppingAction::UserSteppingAction(const G4Step* step)
{
    static const G4ParticleDefinition* opticalphoton = G4OpticalPhoton::OpticalPhotonDefinition();
    const G4ParticleDefinition* particleDef = step->GetTrack()->GetParticleDefinition();

    // 检查是否为光子
    if (particleDef != opticalphoton) return;

    // 确保步骤的后续点和可接触对象有效
    G4StepPoint* postStepPoint = step->GetPostStepPoint();
    if (!postStepPoint) return;

    G4TouchableHandle touchableHandle = postStepPoint->GetTouchableHandle();
    if (!touchableHandle) return;

    G4VPhysicalVolume* volume = touchableHandle->GetVolume();
    if (!volume) return;

    G4LogicalVolume* logicalVolume = volume->GetLogicalVolume();
    if (!logicalVolume) return;

    G4String post_volumeName = logicalVolume->GetName();

    // 检查光子是否穿过crystal端窗和光阴极到达SensitiveVolume
    if(post_volumeName == "SD2" )
    {
      fEventAction->AddSD2();

        G4ThreeVector photonDirection = postStepPoint->GetMomentumDirection();
        G4ThreeVector normalDirection = touchableHandle->GetSolid()->SurfaceNormal(postStepPoint->GetPosition());

        double dotProduct = photonDirection.dot(normalDirection);

        // 如果点积小于0，表示光子的方向与法线方向成钝角，即光子是进入crystal窗口
        if (dotProduct < 0)
        {
            fEventAction->Addcrystal();
        }
      // 杀掉本次事例
      step->GetTrack()->SetTrackStatus(fStopAndKill);
    }


}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
