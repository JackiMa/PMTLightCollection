// 2024年4月16日
// 关于几何位置的摆放还很有bug
// 如果设置为Reflector，闪烁体直接不发光，不太清楚为什么。


#include "OpNoviceDetectorConstruction.hh"
#include "OpNoviceDetectorMessenger.hh"

#include "G4Box.hh"
#include "G4Element.hh"
#include "G4GDMLParser.hh"
#include "G4LogicalBorderSurface.hh"
#include "G4LogicalSkinSurface.hh"
#include "G4LogicalVolume.hh"
#include "MyMaterials.hh"
#include "G4OpticalSurface.hh"
#include "G4VPhysicalVolume.hh"
#include "G4SystemOfUnits.hh"
#include "G4ThreeVector.hh"
#include "G4VisAttributes.hh"

#include "utilities.hh"
#include "MyPhysicalVolume.hh"

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
OpNoviceDetectorConstruction::OpNoviceDetectorConstruction()
  : G4VUserDetectorConstruction()
{
  fDumpGdmlFileName = "Teflon+NoAirGap+Grease.gdml";
  fVerbose          = false;
  fDumpGdml         = false;
  // create a messenger for this class
  fDetectorMessenger = new OpNoviceDetectorMessenger(this);

  isGrease = 0;
  isAirGap = 1;
  // scintillatorX = 1*mm;
  // scintillatorY = 1*mm;
  // scintillatorZ = 1*mm;
  G4double scintillatorXYZ = 0.5*mm;
  scintillatorX = scintillatorXYZ;
  scintillatorY = scintillatorXYZ;
  scintillatorZ = scintillatorXYZ;


  reflectorType = TEFLON;
  // reflectorType = REFLECTOR;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
OpNoviceDetectorConstruction::~OpNoviceDetectorConstruction()
{
  delete fDetectorMessenger;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
G4VPhysicalVolume* OpNoviceDetectorConstruction::Construct()
{
  G4bool checkOverlaps = true;

  // ------------- Materials -------------
  G4Material* air = MyMaterials::Air();
  G4Material* vaccum = MyMaterials::Vacuum();
  G4Material* water = MyMaterials::Water();
  G4Material* GAGG_Ce_Mg = MyMaterials::GAGG_Ce_Mg(-1,1,-1);
  // G4Material* GAGG_Ce_Mg = MyMaterials::BGO(-1,1,-1);
  // G4Material* GAGG_Ce_Mg = matter_construct_water();
  // G4Material* GAGG_Ce_Mg = water;
  G4Material* Glass = MyMaterials::Quartz();        // 石英窗（虽然PMTR6233是硼硅玻璃，这里用石英代替）
  G4Material* PVC = MyMaterials::PVC();             // 反光罩的基材 
  G4Material* OpticalGrease = MyMaterials::OpticalGrease(); // 光学硅脂

  // ------------- Surface -------------
  G4OpticalSurface* surf_Teflon = MyMaterials::surf_Teflon();
  // G4OpticalSurface* surf_BaSO4 = surf_Teflon;
  G4OpticalSurface* surf_BaSO4 = MyMaterials::surf_BaSO4();
  G4OpticalSurface* surf_GapToClearCrystal = MyMaterials::surf_GapToClearCrystal();
  G4OpticalSurface* surf_GlassToPhotocathode = MyMaterials::surf_GlassToPhotocathode();


  // ------------- Volumes --------------
  //
    // The world
    G4double worldSizeXY = 8*cm;
    G4double worldSize = 4 * cm;
    G4Box* solidWorld = new G4Box("World", 0.5*worldSizeXY, 0.5*worldSizeXY, 0.5*worldSize);
    G4LogicalVolume* logicWorld = new G4LogicalVolume(solidWorld, air, "World");
    MyPhysicalVolume* physWorld = new MyPhysicalVolume(0, G4ThreeVector(), logicWorld, "World", 0, false, 0);
    G4VPhysicalVolume* G4physWorld = physWorld->GetG4Placement();
    
    // PMT
    // PMT端窗
    G4double pmtRadius = 3.8 * cm;
    G4double pmtHeight = 0.5 * cm;
    G4double thickness_photocathode = 0.1*mm; // 光阴极厚度 100 um
    G4Tubs* solidPMT = new G4Tubs("PMTwindow", 0, pmtRadius, 0.5*pmtHeight, 0, 360*deg);
    G4LogicalVolume* logicPMT = new G4LogicalVolume(solidPMT, Glass, "PMTwindow");
    MyPhysicalVolume* physPMT = new MyPhysicalVolume(0, G4ThreeVector(0, 0, -0.5*worldSize + 0.5*pmtHeight), logicPMT, "PMTwindow", G4physWorld, false, 0);
    // 光阴极
    G4Tubs* solidPhotocathode = new G4Tubs("Photocathode", 0, pmtRadius, thickness_photocathode, 0, 360*deg);
    G4LogicalVolume* logicPhotocathode = new G4LogicalVolume(solidPhotocathode, air, "Photocathode");
    MyPhysicalVolume* physPhotocathode = new MyPhysicalVolume(0, G4ThreeVector(0, 0, -0.3*pmtHeight+0.5*thickness_photocathode), logicPhotocathode, "Photocathode", physPMT->GetG4Placement(), false, 0);
    // 入射窗与光阴极的界面(反射率30%)
    new G4LogicalBorderSurface("PhotocathodeInterface", physPMT->GetG4Placement(), physPhotocathode->GetG4Placement(), surf_GlassToPhotocathode);

    // PMT光阴极下面的灵敏体积，真空，占据端窗20%-光阴极的厚度
    G4Tubs* solidSV = new G4Tubs("SensitiveVolume", 0, pmtRadius, 0.2*0.5*pmtHeight, 0, 360*deg);
    G4LogicalVolume* logicSV = new G4LogicalVolume(solidSV, vaccum, "SensitiveVolume");
    new MyPhysicalVolume(0, G4ThreeVector(0, 0, -0.8*0.5*pmtHeight), logicSV, "SensitiveVolume", physPMT->GetG4Placement(), false, 0);

    

    // 设置 PMT 的颜色
    G4VisAttributes* pmtVisAtt = new G4VisAttributes(G4Colour(0.9, 0.7, 0.2, 0.2));  // 暗橙黄色，偏黄绿
    pmtVisAtt->SetForceSolid(true);  // 设置为实心，以便可以看到透明度
    pmtVisAtt->SetVisibility(true);
    logicPMT->SetVisAttributes(pmtVisAtt);
    G4VisAttributes* SVVisAtt = new G4VisAttributes(G4Colour(0.9, 0.8, 0.9, 0.1));  // 暗橙黄色，偏黄绿
    SVVisAtt->SetForceSolid(true);  // 设置为实心，以便可以看到透明度
    SVVisAtt->SetVisibility(true);
    logicSV->SetVisAttributes(SVVisAtt);

    // 闪烁体
    G4Box* solidScintillator = new G4Box("Scintillator", 0.5*scintillatorX, 0.5*scintillatorY, 0.5*scintillatorZ);
    

    G4LogicalVolume* logicScintillator = new G4LogicalVolume(solidScintillator, GAGG_Ce_Mg, "Scintillator");
    MyPhysicalVolume* topContainer = physWorld; // 创建一个空的物理体作为顶层容器，便于处理多种条件下闪烁体的位置
    G4ThreeVector posScintillator;
    // 这里暂时不放置闪烁体的位置，因为其取决于是否用Teflon，有没有空气层等等
    // 设置闪烁体的颜色
    G4VisAttributes* scintillatorVisAtt = new G4VisAttributes(G4Colour(1.0, 0.6, 0.0, 0.4));  // 橙黄色
    
    scintillatorVisAtt->SetVisibility(true);
    logicScintillator->SetVisAttributes(scintillatorVisAtt);



    // 是否涂抹光学硅脂
    G4double greaseThickness = 0;
    if (isGrease) {
          G4double skin = 1*mm; // 超出闪烁体的一部分
          greaseThickness = 0.1*mm;
          G4Box* solidGrease = new G4Box("OpticalGrease", 0.5*scintillatorX+skin, 0.5*scintillatorY+skin, greaseThickness);
          G4LogicalVolume* logicGrease = new G4LogicalVolume(solidGrease, OpticalGrease, "Grease");
          G4ThreeVector posGrease = G4ThreeVector(0, 0, -worldSize/2 + pmtHeight + greaseThickness/2);
          new MyPhysicalVolume(0, posGrease, logicGrease, "Grease", G4physWorld, false, 0);

          // 设置 OpticalGrease 的颜色和透明度
          G4VisAttributes* GreaseVisAtt = new G4VisAttributes(G4Colour(0.2, 0.3, 1.0, 0.8));  // 纯白色，透明度0.3
          GreaseVisAtt->SetForceSolid(true);  // 设置为实心，以便可以看到透明度
          GreaseVisAtt->SetVisibility(true);
          logicGrease->SetVisAttributes(GreaseVisAtt);
    }

    // Place Reflector or Teflon, based on choice
  switch (reflectorType) {
      case OpNoviceDetectorConstruction::TEFLON: {
          // Define and place Teflon coating here
          // Optionally place Grease
          G4double airGapThicknessX = 0;
          G4double airGapThicknessY = 0;
          G4double airGapThicknessZ = 0;
          G4double TeflonThickness = 0.5*mm;
          // Optionally place Air gap
          if (isAirGap) { 
            airGapThicknessX = 0.1*mm;
            airGapThicknessY = 0.1*mm;
            airGapThicknessZ = 0.1*mm;
            }
          // Place Teflon
          G4double TeflonY = scintillatorY + airGapThicknessX + TeflonThickness;
          G4double TeflonX = scintillatorX + airGapThicknessY + TeflonThickness;
          G4double TeflonZ = scintillatorZ + 0.5*airGapThicknessZ + 0.5*TeflonThickness;
          G4Box* solidTeflon = new G4Box("Teflon",0.5*TeflonX, 0.5*TeflonY, 0.5*TeflonZ);
          G4LogicalVolume* logicTeflon = new G4LogicalVolume(solidTeflon, PVC, "Teflon");
          G4ThreeVector posTeflon = G4ThreeVector(0, 0, -worldSize/2 + pmtHeight + greaseThickness + 0.5*TeflonZ);
          MyPhysicalVolume* physTeflon = new MyPhysicalVolume(0, posTeflon, logicTeflon, "Teflon", G4physWorld, false, 0);
          topContainer = physTeflon;   // 晶体放在Teflon中

          G4double AirX = scintillatorX + airGapThicknessX;
          G4double AirY = scintillatorY + airGapThicknessY;
          G4double AirZ = scintillatorZ + 0.5*airGapThicknessZ;
          if (isAirGap) {
            G4Box* solidAirGap = new G4Box("AirGap", 0.5*AirX, 0.5*AirY, 0.5*AirZ);
            G4LogicalVolume* logicAirGap = new G4LogicalVolume(solidAirGap, air, "AirGap");
            G4ThreeVector posAirGap = G4ThreeVector(0, 0, -0.5*TeflonZ+0.5*AirZ); 
            MyPhysicalVolume* physAirGap = new MyPhysicalVolume(0, posAirGap, logicAirGap, "AirGap", physTeflon->GetG4Placement(), false, 0);
            topContainer = physAirGap; // 如果定义了AirGap，晶体放在AirGap中
            posScintillator = G4ThreeVector(0, 0, -0.5*AirZ+0.5*scintillatorZ); // 使用Teflon时晶体相对的位置

            // G4LogicalBorderSurface* logic_surf_Teflon2AirGap = new G4LogicalBorderSurface(
            // "logic_surf_Teflon2AirGap", physReflector,physAirGap, surf_Teflon);
          }
          // locigal border surface
          // G4LogicalBorderSurface* logic_surf_Teflon2world = new G4LogicalBorderSurface(
          //   "logic_surf_Teflon2world", physReflector,physWorld, surf_Teflon);

          new G4LogicalSkinSurface("TeflonSurface", logicTeflon, surf_Teflon);

          if(topContainer == physTeflon){
            posScintillator = G4ThreeVector(0, 0, -0.5*TeflonZ+0.5*scintillatorZ); // 使用Teflon时晶体相对的位置
          }
          

          // 设置 Teflon 的颜色和透明度
          G4VisAttributes* teflonVisAtt = new G4VisAttributes(G4Colour(1.0, 1.0, 1.0, 0.3));  // 纯白色，透明度0.3
          teflonVisAtt->SetVisibility(true);
          teflonVisAtt->SetForceSolid(true);  // 设置为实心，以便可以看到透明度
          logicTeflon->SetVisAttributes(teflonVisAtt);

          break;
      }
      case OpNoviceDetectorConstruction::REFLECTOR: {
          // Define and place Reflector dome here
          // 定义 Reflector 的内外半径
          G4double innerRadius = 18*mm;
          G4double outerRadius = innerRadius + 5*mm;
          // 创建 Reflector
          G4Sphere* solidReflector = new G4Sphere("Reflector", innerRadius, outerRadius, 0, 2*CLHEP::pi, 0, 0.5*CLHEP::pi);
          G4LogicalVolume* logicReflector = new G4LogicalVolume(solidReflector, PVC, "Reflector");

          // 将 Reflector 放置在适当的位置
          G4ThreeVector posReflector = G4ThreeVector(0, 0, -0.5*worldSize+pmtHeight);  // 根据需要调整
          MyPhysicalVolume* physReflector = new MyPhysicalVolume(0, posReflector, logicReflector, "Reflector", G4physWorld, false, 0);
          // topContainer = logicReflector;
          topContainer = physWorld;

          new G4LogicalSkinSurface("ReflectorSurface", logicReflector, surf_BaSO4);

          // 设置 Reflector 的颜色和透明度
          G4VisAttributes* ReflectorVisAtt = new G4VisAttributes(G4Colour(1.0, 1.0, 1.0, 0.3));  // 纯白色，透明度0.3
          ReflectorVisAtt->SetVisibility(true);
          ReflectorVisAtt->SetForceSolid(true);  // 设置为实心，以便可以看到透明度
          logicReflector->SetVisAttributes(ReflectorVisAtt);


          // // 反射层内部的空气：
          // G4double Air_Radius = innerRadius;
          // // 创建 Reflector
          // G4Sphere* solidAirInReflector = new G4Sphere("AirInReflector", 0, Air_Radius, 0, 2*CLHEP::pi, 0, 0.5*CLHEP::pi);
          // G4LogicalVolume* logicAirInReflector = new G4LogicalVolume(solidAirInReflector, air, "AirInReflector");

          // // 将 AirInReflector 放置在适当的位置
          // G4ThreeVector posAirInReflector = G4ThreeVector(0, 0, 0);  // 根据需要调整
          // MyPhysicalVolume* physAirInReflector = new MyPhysicalVolume(0, posAirInReflector, logicAirInReflector, "AirInReflector", logicReflector, false, 0);
          // topContainer = logicAirInReflector;

          // posScintillator = G4ThreeVector(0, 0, greaseThickness + 0.5*scintillatorZ); 
          posScintillator = G4ThreeVector(0, 0, -0.5*worldSize+pmtHeight+greaseThickness + 0.5*scintillatorZ); 
          
          break;
      }
      default:
          // No reflector
          break;
  }

  // 闪烁体
  MyPhysicalVolume* physScintillator = new MyPhysicalVolume(0, posScintillator, logicScintillator, "Scintillator", topContainer->GetG4Placement(), false, 0);
  fVolumeMap["Scintillator"] = physScintillator;


  
  myPrint(lv, f("fVolumeMap length is {}\n",  fVolumeMap.size()));
  for (const auto& pair : fVolumeMap) {
      std::cout << "Volume name: " << pair.first << ", Volume address: " << pair.second << std::endl;
  }


  if(fDumpGdml)
  {
      std::ifstream ifile(fDumpGdmlFileName);
      if (ifile) {
          G4cout << fDumpGdmlFileName << " 已存在，不再写入。" << G4endl;
      } else {
          G4GDMLParser* parser = new G4GDMLParser();
          parser->Write(fDumpGdmlFileName, G4physWorld);
      }
  }

  
  return G4physWorld;
}

MyPhysicalVolume* OpNoviceDetectorConstruction::GetMyVolume(G4String volumeName) const
{
  auto it = fVolumeMap.find(volumeName);
  if (it != fVolumeMap.end())
  {
    myPrint(lv, "Volume is found!\n");
    return it->second;
  }
  else
  {
    myPrint(lv, "Volume not found!\n");
    return 0;
  }
}


//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
void OpNoviceDetectorConstruction::SetDumpGdml(G4bool val) { fDumpGdml = val; }

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
G4bool OpNoviceDetectorConstruction::IsDumpGdml() const { return fDumpGdml; }

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
void OpNoviceDetectorConstruction::SetVerbose(G4bool val) { fVerbose = val; }

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
G4bool OpNoviceDetectorConstruction::IsVerbose() const { return fVerbose; }

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
void OpNoviceDetectorConstruction::SetDumpGdmlFile(G4String filename)
{
  fDumpGdmlFileName = filename;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
G4String OpNoviceDetectorConstruction::GetDumpGdmlFile() const
{
  return fDumpGdmlFileName;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
void OpNoviceDetectorConstruction::PrintError(G4String ed)
{
  G4Exception("OpNoviceDetectorConstruction:MaterialProperty test", "op001",
              FatalException, ed);
}



G4Material* OpNoviceDetectorConstruction::matter_construct_water() {
    // Water elements and material definition
    G4double a, z, density;
    G4int nelements = 2;
    G4Element* H = new G4Element("Hydrogen", "H", z = 1, a = 1.01 * g / mole);
    G4Element* O = new G4Element("Oxygen", "O", z = 8, a = 16.00 * g / mole);
    G4Material* water = new G4Material("Water", density = 1.0 * g / cm3, nelements);
    water->AddElement(H, 2);
    water->AddElement(O, 1);


    std::vector<G4double> photonEnergy = {
    2.034 * eV, 2.068 * eV, 2.103 * eV, 2.139 * eV, 2.177 * eV, 2.216 * eV,
    2.256 * eV, 2.298 * eV, 2.341 * eV, 2.386 * eV, 2.433 * eV, 2.481 * eV,
    2.532 * eV, 2.585 * eV, 2.640 * eV, 2.697 * eV, 2.757 * eV, 2.820 * eV,
    2.885 * eV, 2.954 * eV, 3.026 * eV, 3.102 * eV, 3.181 * eV, 3.265 * eV,
    3.353 * eV, 3.446 * eV, 3.545 * eV, 3.649 * eV, 3.760 * eV, 3.877 * eV,
    4.002 * eV, 4.136 * eV
  };

    // Water
    std::vector<G4double> refractiveIndex1 = {
      1.3435, 1.344,  1.3445, 1.345,  1.3455, 1.346,  1.3465, 1.347,
      1.3475, 1.348,  1.3485, 1.3492, 1.35,   1.3505, 1.351,  1.3518,
      1.3522, 1.3530, 1.3535, 1.354,  1.3545, 1.355,  1.3555, 1.356,
      1.3568, 1.3572, 1.358,  1.3585, 1.359,  1.3595, 1.36,   1.3608
    };
    std::vector<G4double> absorption = {
      3.448 * m,  4.082 * m,  6.329 * m,  9.174 * m,  12.346 * m, 13.889 * m,
      15.152 * m, 17.241 * m, 18.868 * m, 20.000 * m, 26.316 * m, 35.714 * m,
      45.455 * m, 47.619 * m, 52.632 * m, 52.632 * m, 55.556 * m, 52.632 * m,
      52.632 * m, 47.619 * m, 45.455 * m, 41.667 * m, 37.037 * m, 33.333 * m,
      30.000 * m, 28.500 * m, 27.000 * m, 24.500 * m, 22.000 * m, 19.500 * m,
      17.500 * m, 14.500 * m
    };

    // Material properties can be added as arrays. However, in this case it is
    // up to the user to make sure both arrays have the same number of elements.
    G4double scintilFastArray[]{ 1.0, 1.0 };
    G4double energyArray[]{ 2.034 * eV, 4.136 * eV };
    G4int lenArray = 2;

    std::vector<G4double> scintilSlow = {
      0.01, 1.00, 2.00, 3.00, 4.00, 5.00, 6.00, 7.00, 8.00, 9.00, 8.00,
      7.00, 6.00, 4.00, 3.00, 2.00, 1.00, 0.01, 1.00, 2.00, 3.00, 4.00,
      5.00, 6.00, 7.00, 8.00, 9.00, 8.00, 7.00, 6.00, 5.00, 4.00
    };

    G4MaterialPropertiesTable* myMPT1 = new G4MaterialPropertiesTable();

    // Values can be added to the material property table individually.
    // With this method, spline interpolation cannot be set. Arguments
    // createNewKey and spline both take their default values of false.
    // Need to specify the number of entries (1) in the arrays, as an argument
    // to AddProperty.
    G4int numEntries = 1;

    myMPT1->AddProperty("RINDEX", photonEnergy, refractiveIndex1, numEntries);

    // Check that group velocity is calculated from RINDEX
    if(myMPT1->GetProperty("RINDEX")->GetVectorLength() !=
      myMPT1->GetProperty("GROUPVEL")->GetVectorLength())
    {
      G4ExceptionDescription ed;
      ed << "Error calculating group velocities. Incorrect number of entries "
            "in group velocity material property vector.";
      G4Exception("OpNovice::OpNoviceDetectorConstruction", "OpNovice001",
                  FatalException, ed);
    }

    myMPT1->AddProperty("ABSLENGTH", photonEnergy, absorption, false, true);
    // adding property with a C-style array
    myMPT1->AddProperty("SCINTILLATIONCOMPONENT1", energyArray, scintilFastArray,
                        lenArray, false, true);
    myMPT1->AddProperty("SCINTILLATIONCOMPONENT2", photonEnergy, scintilSlow,
                        false, true);
    myMPT1->AddConstProperty("SCINTILLATIONYIELD", 50. / MeV);
    myMPT1->AddConstProperty("RESOLUTIONSCALE", 1.0);
    myMPT1->AddConstProperty("SCINTILLATIONTIMECONSTANT1", 1. * ns);
    myMPT1->AddConstProperty("SCINTILLATIONTIMECONSTANT2", 10. * ns);
    myMPT1->AddConstProperty("SCINTILLATIONYIELD1", 0.8);
    myMPT1->AddConstProperty("SCINTILLATIONYIELD2", 0.2);
    std::vector<G4double> energy_water = {
      1.56962 * eV, 1.58974 * eV, 1.61039 * eV, 1.63157 * eV, 1.65333 * eV,
      1.67567 * eV, 1.69863 * eV, 1.72222 * eV, 1.74647 * eV, 1.77142 * eV,
      1.7971 * eV,  1.82352 * eV, 1.85074 * eV, 1.87878 * eV, 1.90769 * eV,
      1.93749 * eV, 1.96825 * eV, 1.99999 * eV, 2.03278 * eV, 2.06666 * eV,
      2.10169 * eV, 2.13793 * eV, 2.17543 * eV, 2.21428 * eV, 2.25454 * eV,
      2.29629 * eV, 2.33962 * eV, 2.38461 * eV, 2.43137 * eV, 2.47999 * eV,
      2.53061 * eV, 2.58333 * eV, 2.63829 * eV, 2.69565 * eV, 2.75555 * eV,
      2.81817 * eV, 2.88371 * eV, 2.95237 * eV, 3.02438 * eV, 3.09999 * eV,
      3.17948 * eV, 3.26315 * eV, 3.35134 * eV, 3.44444 * eV, 3.54285 * eV,
      3.64705 * eV, 3.75757 * eV, 3.87499 * eV, 3.99999 * eV, 4.13332 * eV,
      4.27585 * eV, 4.42856 * eV, 4.59258 * eV, 4.76922 * eV, 4.95999 * eV,
      5.16665 * eV, 5.39129 * eV, 5.63635 * eV, 5.90475 * eV, 6.19998 * eV
    };

    // Rayleigh scattering length calculated by G4OpRayleigh
    // assume 100 times larger than the rayleigh scattering for now.
    std::vector<G4double> mie_water = {
      167024.4 * m, 158726.7 * m, 150742 * m,   143062.5 * m, 135680.2 * m,
      128587.4 * m, 121776.3 * m, 115239.5 * m, 108969.5 * m, 102958.8 * m,
      97200.35 * m, 91686.86 * m, 86411.33 * m, 81366.79 * m, 76546.42 * m,
      71943.46 * m, 67551.29 * m, 63363.36 * m, 59373.25 * m, 55574.61 * m,
      51961.24 * m, 48527.00 * m, 45265.87 * m, 42171.94 * m, 39239.39 * m,
      36462.50 * m, 33835.68 * m, 31353.41 * m, 29010.30 * m, 26801.03 * m,
      24720.42 * m, 22763.36 * m, 20924.88 * m, 19200.07 * m, 17584.16 * m,
      16072.45 * m, 14660.38 * m, 13343.46 * m, 12117.33 * m, 10977.70 * m,
      9920.416 * m, 8941.407 * m, 8036.711 * m, 7202.470 * m, 6434.927 * m,
      5730.429 * m, 5085.425 * m, 4496.467 * m, 3960.210 * m, 3473.413 * m,
      3032.937 * m, 2635.746 * m, 2278.907 * m, 1959.588 * m, 1675.064 * m,
      1422.710 * m, 1200.004 * m, 1004.528 * m, 833.9666 * m, 686.1063 * m
    };

    // gforward, gbackward, forward backward ratio
    G4double mie_water_const[3] = { 0.99, 0.99, 0.8 };

    myMPT1->AddProperty("MIEHG", energy_water, mie_water, false, true);
    myMPT1->AddConstProperty("MIEHG_FORWARD", mie_water_const[0]);
    myMPT1->AddConstProperty("MIEHG_BACKWARD", mie_water_const[1]);
    myMPT1->AddConstProperty("MIEHG_FORWARD_RATIO", mie_water_const[2]);

    G4cout << "Water G4MaterialPropertiesTable:" << G4endl;
    myMPT1->DumpTable();

    water->SetMaterialPropertiesTable(myMPT1);

    // // Set the Birks Constant for the Water scintillator
    water->GetIonisation()->SetBirksConstant(0.126 * mm / MeV);



    // // test user-defined properties
    // G4String ed;
    // if(myMPT1->GetProperty("USERDEFINED") != nullptr)
    // {
    //   ed = "USERDEFINED != nullptr";
    //   PrintError(ed);
    // }
    // myMPT1->AddProperty("USERDEFINED", energy_water, mie_water, true, true);
    // if(myMPT1->GetProperty("USERDEFINED") == nullptr)
    // {
    //   ed = "USERDEFINED == nullptr";
    //   PrintError(ed);
    // }
    // [[maybe_unused]] G4int index_userdefined = -1;
    // if(myMPT1->GetProperty("USERDEFINED") != nullptr)
    // {
    //   index_userdefined = myMPT1->GetPropertyIndex("USERDEFINED");
    // }
    // if(!(index_userdefined >= 0 &&
    //       index_userdefined <
    //       (G4int) myMPT1->GetMaterialPropertyNames().size()))
    // {
    //   ed = "USERDEFINED index out of range";
    //   PrintError(ed);
    // }
    // myMPT1->RemoveProperty("USERDEFINED");
    // if(myMPT1->GetProperty("USERDEFINED") != nullptr)
    // {
    //   ed = "USERDEFINED != nullptr at end";
    //   PrintError(ed);
    // }

    // if(myMPT1->ConstPropertyExists("USERDEFINEDCONST") == true)
    // {
    //   ed = "ConstProperty USERDEFINEDCONST already exists.";
    //   PrintError(ed);
    // }
    // myMPT1->AddConstProperty("USERDEFINEDCONST", 3.14, true);
    // if(myMPT1->ConstPropertyExists("USERDEFINEDCONST") == false)
    // {
    //   ed = "ConstProperty USERDEFINEDCONST doesn't exist.";
    //   PrintError(ed);
    // }
    // [[maybe_unused]] G4int index_pi = -1;
    // if(myMPT1->ConstPropertyExists("USERDEFINEDCONST") == true)
    // {
    //   index_pi = myMPT1->GetConstPropertyIndex("USERDEFINEDCONST");
    // }
    // if (!(index_pi >= 0 &&
    //       index_pi < (G4int) myMPT1->GetMaterialConstPropertyNames().size()))
    // {
    //   ed = "ConstProperty USERDEFINEDCONST index out of range.";
    //   PrintError(ed);
    // }
    // myMPT1->RemoveConstProperty("USERDEFINEDCONST");
    // if (myMPT1->ConstPropertyExists("USERDEFINEDCONST") == true)
    // {
    //   ed = "ConstProperty USERDEFINEDCONST still exists.";
    //   PrintError(ed);
    // }
    // // done testing user-defined properties

    return water;
}