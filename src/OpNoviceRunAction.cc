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
/// \file OpNovice/src/OpNoviceRunAction.cc
/// \brief Implementation of the OpNoviceRunAction class
//
//
//
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
#include "OpNoviceRunAction.hh"
#include "OpNovicePrimaryGeneratorAction.hh"
#include "OpNoviceRun.hh"
#include "G4ParticleDefinition.hh"
#include "G4Run.hh"
#include <fstream>
#include <mutex>
#include "G4UnitsTable.hh"
#include "G4SystemOfUnits.hh"

#include "G4RootAnalysisManager.hh"

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
OpNoviceRunAction::OpNoviceRunAction(OpNovicePrimaryGeneratorAction* prim)
  : G4UserRunAction()
  , fRun(nullptr)
  , fPrimary(prim)
{
  
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
OpNoviceRunAction::~OpNoviceRunAction() {}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
G4Run* OpNoviceRunAction::GenerateRun()
{
  fRun = new OpNoviceRun();
  return fRun;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
void OpNoviceRunAction::BeginOfRunAction(const G4Run*)
{
  if(fPrimary)
  {
    G4ParticleDefinition* particle =
      fPrimary->GetParticleGun()->GetParticleDefinition();
    G4double energy = fPrimary->GetParticleGun()->GetParticleEnergy();
    fRun->SetPrimary(particle, energy);
  }

  G4double gamma_eng = 511*keV; // 入射光子的能量
  G4double LY = 40 / keV;       // 光产额 

  // Create analysis manager in the master run
  // Initialize the analysis manager and create a histogram
  if (isMaster) {
    analysisManager = G4RootAnalysisManager::Instance();
    analysisManager->OpenFile("output.root");  // 打开一个文件
    G4double xmax = gamma_eng * LY * 1.2; // 根据入射能量和光产额设置直方图的上限 
    analysisManager->CreateH1("hPhotonGenerated","Spectrum of Generated Scintillation Photons", 100, 0, xmax); // 创建一个直方图
    analysisManager->CreateH1("hPhotonDetected","Spectrum of Detected Photons", 100, 0, xmax); // 创建一个直方图
}
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
void OpNoviceRunAction::EndOfRunAction(const G4Run* g4run)
{
  OpNoviceRun* run = dynamic_cast<OpNoviceRun*>(const_cast<G4Run*>(g4run));
  if (!run) {
        G4cerr << "Error: could not cast G4Run to OpNoviceRun." << G4endl;
        return;
    }

  if(isMaster){
    fRun->EndOfRun();
  

  // Fill the histogram and write it to a file
  analysisManager->Write();
  analysisManager->CloseFile();


  G4int numberOfEvent = run->GetNumberOfEvent();
  if(numberOfEvent == 0)
    return;
  G4double TotNbofEvents = G4double(numberOfEvent);

  run->fCerenkovCounter /= TotNbofEvents;
  run->fCerenkov2 /= TotNbofEvents;
  G4double rmsCerenkov = run->fCerenkov2 - run->fCerenkovCounter * run->fCerenkovCounter;
  if(rmsCerenkov > 0.)
    rmsCerenkov = std::sqrt(rmsCerenkov);
  else
    rmsCerenkov = 0.;

  run->fScintillationCounter /= TotNbofEvents;
  run->fScintillation2 /= TotNbofEvents;
  G4double rmsScint =
    run->fScintillation2 - run->fScintillationCounter * run->fScintillationCounter;
  if(rmsScint > 0.)
    rmsScint = std::sqrt(rmsScint);
  else
    rmsScint = 0.;

  run->fPMTCounter /= TotNbofEvents;
  run->fPMT2 /= TotNbofEvents;
  G4double rmsPMT = run->fPMT2 - run->fPMTCounter * run->fPMTCounter;
  if(rmsPMT > 0.)
    rmsPMT = std::sqrt(rmsPMT);
  else
    rmsPMT = 0.;

  run->fSVCounter /= TotNbofEvents;
  run->fSV2 /= TotNbofEvents;
  G4double rmsSV = run->fSV2 - run->fSVCounter * run->fSVCounter;
  if(rmsSV > 0.)
    rmsSV = std::sqrt(rmsSV);
  else
    rmsSV = 0.;

  G4int prec = G4cout.precision(3);
  G4cout << "\n ======================== run summary ======================\n";

  G4cout << "Primary particle was: " << run->fParticle->GetParticleName()
         << " with energy " << G4BestUnit(run->fEnergy, "Energy") << "." << G4endl;
  G4cout << "Number of events: " << numberOfEvent << G4endl;

  G4cout << "Average number of Cerenkov photons created per event: "
         << run->fCerenkovCounter << " +- " << rmsCerenkov << G4endl;
  G4cout << "Average number of scintillation photons created per event: "
         << run->fScintillationCounter << " +- " << rmsScint << G4endl;
  G4cout << "Average number of PMT hits per event: " << run->fPMTCounter << " +- "
          << rmsPMT << G4endl;
  G4cout << "Average number of sensitive volume hits per event: " << run->fSVCounter
          << " +- " << rmsSV << G4endl;
  
  // 增加一个输出光收集效率的，即sensitive volume hits per event / scintillation+Cerenkov photons created per event
  G4double efficiency = run->fSVCounter / (run->fScintillationCounter + run->fCerenkovCounter);
  G4cout << "Average light collection efficiency: " << efficiency << G4endl;
          
  G4cout << G4endl;
  G4cout.precision(prec);


  static std::mutex fileMutex;
  std::lock_guard<std::mutex> lock(fileMutex); // 使用互斥锁确保线程安全

  std::ofstream outFile("run_data.csv", std::ios::app); // 以追加模式打开文件

  // 在文件为空时写入标题行
  if (outFile.tellp() == 0) {
      outFile << "Particle, Energy, Nums, Cerenkov, Scintillation, PMT Hits, SensitiveVolume, collection efficiency\n";
  }

  // 获取数据
  G4String particleName = run->fParticle->GetParticleName();
  G4double energy = run->fEnergy;
  G4double avgCerenkov = run->fCerenkovCounter;
  G4double avgScint = run->fScintillationCounter;
  G4double avgPMT = run->fPMTCounter;
  G4double avgSV = run->fSVCounter;


  // 写入数据
  outFile << particleName << "," << energy << " keV," << numberOfEvent << ","
          << avgCerenkov << "," << avgScint << "," << avgPMT << avgSV << efficiency << "\n";

  outFile.close(); // 关闭文件

  }
    
}
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
