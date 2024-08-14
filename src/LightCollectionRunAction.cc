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
/// \file LightCollection/src/LightCollectionRunAction.cc
/// \brief Implementation of the LightCollectionRunAction class
//
//
//
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
#include "LightCollectionRunAction.hh"
#include "LightCollectionPrimaryGeneratorAction.hh"
#include "LightCollectionRun.hh"
#include "G4ParticleDefinition.hh"
#include "G4Run.hh"
#include <fstream>
#include <filesystem>
#include <sstream>
#include <mutex>
#include "G4UnitsTable.hh"
#include "G4SystemOfUnits.hh"

#include "G4AnalysisManager.hh"
#include "G4AccumulableManager.hh"

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
LightCollectionRunAction::LightCollectionRunAction(LightCollectionPrimaryGeneratorAction *prim)
    : G4UserRunAction(), fRun(nullptr), fPrimary(prim),    fProtonDose(0.), fElectronDose(0.), fGammaDose(0.),
    fShieldedProtonDose(0.), fShieldedElectronDose(0.), fShieldedGammaDose(0.),
    fCrystalDose(0.)
{
    // Register accumulables to the manager
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
LightCollectionRunAction::~LightCollectionRunAction() {}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
G4Run *LightCollectionRunAction::GenerateRun()
{
  fRun = new LightCollectionRun();
  return fRun;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
void LightCollectionRunAction::BeginOfRunAction(const G4Run *)
{
  if (fPrimary)
  {
    G4double energy;
    G4ParticleDefinition *particle;

    LightCollectionPrimaryGeneratorAction::isInitialized = false;
    if (fPrimary->GetUseParticleGun())
    {
    particle = fPrimary->GetParticleGun()->GetParticleDefinition();
    energy = fPrimary->GetParticleGun()->GetParticleEnergy();
    }
    else{
    particle = fPrimary->GetGPS()->GetParticleDefinition();
    energy = fPrimary->GetGPS()->GetCurrentSource()->GetEneDist()->GetMonoEnergy();
    }

    fRun->SetPrimary(particle, energy);
  }


  // Get analysis manager
    auto analysisManager = G4AnalysisManager::Instance();
    analysisManager->SetNtupleMerging(true);

    // 创建Ntuple
    analysisManager->CreateNtuple("ShieldData", "Energy and particles in shield layers");
    analysisManager->CreateNtupleDColumn("layerID");
    analysisManager->CreateNtupleDColumn("energyDeposit");
    analysisManager->CreateNtupleDColumn("neutronEnergy");
    analysisManager->CreateNtupleDColumn("electronEnergy");
    analysisManager->FinishNtuple();
   
    // Creating histograms for the spectra
    analysisManager->CreateH1("ScintillationWavelength", "Scintillation Wavelength in Crystal", 600, 200.0, 800.0);
    analysisManager->CreateH1("SD2ScintillationWavelength", "Scintillation Wavelength in SD2", 600, 200.0, 800.0); 


   // Open an output file
  //
G4String fileName = getNewfileName("LYsimulations");
if (!analysisManager->OpenFile(fileName)) {
    G4cerr << "Error: could not open file " << fileName << G4endl;
} else {
    G4cout << "Successfully opened file " << fileName << G4endl;
}
G4cout << "Using " << analysisManager->GetType() << G4endl;

}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
void LightCollectionRunAction::EndOfRunAction(const G4Run *g4run)
{
  LightCollectionRun *run = dynamic_cast<LightCollectionRun *>(const_cast<G4Run *>(g4run));
  if (!run)
  {
    G4cerr << "Error: could not cast G4Run to LightCollectionRun." << G4endl;
    return;
  }

  // // save histograms & ntuple
  auto analysisManager = G4AnalysisManager::Instance();
  //   // Merge accumulables
  // G4AccumulableManager* accumulableManager = G4AccumulableManager::Instance();
  // accumulableManager->Merge();

  // // Print accumulated doses
  // G4cout << "Proton Dose: " << fProtonDose.GetValue() << G4endl;
  // G4cout << "Electron Dose: " << fElectronDose.GetValue() << G4endl;
  // G4cout << "Gamma Dose: " << fGammaDose.GetValue() << G4endl;
  // G4cout << "Shielded Proton Dose: " << fShieldedProtonDose.GetValue() << G4endl;
  // G4cout << "Shielded Electron Dose: " << fShieldedElectronDose.GetValue() << G4endl;
  // G4cout << "Shielded Gamma Dose: " << fShieldedGammaDose.GetValue() << G4endl;
  // G4cout << "Crystal Dose: " << fCrystalDose.GetValue() << G4endl;

  analysisManager->Write();
  analysisManager->CloseFile();
  G4cout << "Data written and file closed." << G4endl;


  // if (isMaster)
  // {
  //   fRun->EndOfRun();

  //   G4int numberOfEvent = run->GetNumberOfEvent();
  //   if (numberOfEvent == 0)
  //     return;
  //   G4double TotNbofEvents = G4double(numberOfEvent);

  //   run->fCerenkovCounter /= TotNbofEvents;
  //   run->fCerenkov2 /= TotNbofEvents;
  //   G4double rmsCerenkov = run->fCerenkov2 - run->fCerenkovCounter * run->fCerenkovCounter;
  //   if (rmsCerenkov > 0.)
  //     rmsCerenkov = std::sqrt(rmsCerenkov);
  //   else
  //     rmsCerenkov = 0.;

  //   run->fScintillationCounter /= TotNbofEvents;
  //   run->fScintillation2 /= TotNbofEvents;
  //   G4double rmsScint =
  //       run->fScintillation2 - run->fScintillationCounter * run->fScintillationCounter;
  //   if (rmsScint > 0.)
  //     rmsScint = std::sqrt(rmsScint);
  //   else
  //     rmsScint = 0.;

  //   run->fcrystalCounter /= TotNbofEvents;
  //   run->fcrystal2 /= TotNbofEvents;
  //   G4double rmscrystal = run->fcrystal2 - run->fcrystalCounter * run->fcrystalCounter;
  //   if (rmscrystal > 0.)
  //     rmscrystal = std::sqrt(rmscrystal);
  //   else
  //     rmscrystal = 0.;

  //   run->fSD2_Counter /= TotNbofEvents;
  //   run->fSD2_2 /= TotNbofEvents;
  //   G4double rmsSD2_ = run->fSD2_2 - run->fSD2_Counter * run->fSD2_Counter;
  //   if (rmsSD2_ > 0.)
  //     rmsSD2_ = std::sqrt(rmsSD2_);
  //   else
  //     rmsSD2_ = 0.;

  //   G4int prec = G4cout.precision(3);
  //   G4cout << "\n ======================== run summary ======================\n";

  //   G4cout << "Primary particle was: " << run->fParticle->GetParticleName()
  //          << " with energy " << G4BestUnit(run->fEnergy, "Energy") << "." << G4endl;
  //   G4cout << "Number of events: " << numberOfEvent << G4endl;

  //   G4cout << "Average number of Cerenkov photons created per event: "
  //          << run->fCerenkovCounter << " +- " << rmsCerenkov << G4endl;
  //   G4cout << "Average number of scintillation photons created per event: "
  //          << run->fScintillationCounter << " +- " << rmsScint << G4endl;
  //   G4cout << "Average number of crystal hits per event: " << run->fcrystalCounter << " +- "
  //          << rmscrystal << G4endl;
  //   G4cout << "Average number of sensitive volume hits per event: " << run->fSD2_Counter
  //          << " +- " << rmsSD2_ << G4endl;

  //   // 增加一个输出光收集效率的，即sensitive volume hits per event / scintillation+Cerenkov photons created per event
  //   G4double efficiency = run->fSD2_Counter / (run->fScintillationCounter + run->fCerenkovCounter);
  //   G4cout << "Average light collection efficiency: " << efficiency << G4endl;

  //   G4cout << G4endl;
  //   G4cout.precision(prec);

  //   static std::mutex fileMutex;
  //   std::lock_guard<std::mutex> lock(fileMutex); // 使用互斥锁确保线程安全

  //   std::ofstream outFile("run_data.csv", std::ios::app); // 以追加模式打开文件

  //   // 在文件为空时写入标题行
  //   if (outFile.tellp() == 0)
  //   {
  //     outFile << "Particle, Energy, Nums, Cerenkov, Scintillation, crystal Hits, SensitiveVolume, collection efficiency\n";
  //   }

  //   // 获取数据
  //   G4String particleName = run->fParticle->GetParticleName();
  //   G4double energy = run->fEnergy/keV;
  //   G4double avgCerenkov = run->fCerenkovCounter;
  //   G4double avgScint = run->fScintillationCounter;
  //   G4double avgcrystal = run->fcrystalCounter;
  //   G4double avgSD2_ = run->fSD2_Counter;

  //   // 写入数据
  //   outFile << particleName << "," << energy << " keV," << numberOfEvent << ","
  //           << avgCerenkov << "," << avgScint << "," << avgcrystal << ","<< avgSD2_ << "," << efficiency << "\n";

  //   outFile.close(); // 关闭文件
  // }
}

bool LightCollectionRunAction::fileExists(const std::string& fileName)
{
    std::ifstream file(fileName.c_str());
    return file.good();
}

G4String LightCollectionRunAction::getNewfileName(G4String baseFileName)
{
    G4String fileExtension = ".root";
    G4String fileName;
    int fileIndex = 0;

    do
    {
        std::stringstream ss;
        ss << baseFileName;
        if (fileIndex > 0)
        {
            ss << "(" << fileIndex << ")";
        }
        ss << fileExtension;
        fileName = ss.str();
        fileIndex++;
    } while (fileExists(fileName));

    return fileName;
}
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
