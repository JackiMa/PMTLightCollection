// 2024年5月22日
// 修复一些问题，增加了圆柱体反光罩的方案

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
  G4double scintillatorXYZ = 10*mm;
  scintillatorX = scintillatorXYZ;
  scintillatorY = scintillatorXYZ;
  scintillatorZ = scintillatorXYZ;


  // reflectorType = TEFLON;
  // reflectorType = REFLECTOR;
  reflectorType = CYLINDER;
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
  G4Material* GAGG_Ce_Mg = MyMaterials::GAGG_Ce_Mg(-1,0.1,-1);
  // G4Material* GAGG_Ce_Mg = MyMaterials::BGO(-1,1,-1);
  // G4Material* GAGG_Ce_Mg = matter_construct_water();
  // G4Material* GAGG_Ce_Mg = water;
  G4Material* Glass = MyMaterials::Quartz();        // 石英窗（虽然PMTR6233是硼硅玻璃，这里用石英代替）
  G4Material* PVC = MyMaterials::PVC();             // 反光罩的基材 
  G4Material* OpticalGrease = MyMaterials::OpticalGrease(); // 光学硅脂

  // ------------- Surface -------------
  G4OpticalSurface* surf_Teflon = MyMaterials::surf_Teflon();
  // G4OpticalSurface* surf_TiO2 = surf_Teflon;
  G4OpticalSurface* surf_TiO2 = MyMaterials::surf_TiO2();
  G4OpticalSurface* surf_GapToClearCrystal = MyMaterials::surf_GapToClearCrystal();
  G4OpticalSurface* surf_GlassToPhotocathode = MyMaterials::surf_GlassToPhotocathode();


  // ------------- Volumes --------------
  //
    // The world
    G4double worldSizeXY = 9*cm;
    G4double worldSize = 12 * cm;
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
    MyPhysicalVolume* physPMT = new MyPhysicalVolume(0, G4ThreeVector(0, 0, -0.5*worldSize + 0.5*pmtHeight), logicPMT, "PMTwindow", physWorld, false, 0);
    // 光阴极
    G4Tubs* solidPhotocathode = new G4Tubs("Photocathode", 0, pmtRadius, thickness_photocathode, 0, 360*deg);
    G4LogicalVolume* logicPhotocathode = new G4LogicalVolume(solidPhotocathode, air, "Photocathode");
    MyPhysicalVolume* physPhotocathode = new MyPhysicalVolume(0, G4ThreeVector(0, 0, -0.3*pmtHeight+0.5*thickness_photocathode), logicPhotocathode, "Photocathode", physPMT, false, 0);
    // 入射窗与光阴极的界面(反射率30%)
    new G4LogicalBorderSurface("PhotocathodeInterface", physPMT->GetG4Placement(), physPhotocathode->GetG4Placement(), surf_GlassToPhotocathode);

    // PMT光阴极下面的灵敏体积，真空，占据端窗20%-光阴极的厚度
    G4Tubs* solidSV = new G4Tubs("SensitiveVolume", 0, pmtRadius, 0.2*0.5*pmtHeight, 0, 360*deg);
    G4LogicalVolume* logicSV = new G4LogicalVolume(solidSV, vaccum, "SensitiveVolume");
    new MyPhysicalVolume(0, G4ThreeVector(0, 0, -0.8*0.5*pmtHeight), logicSV, "SensitiveVolume", physPMT, false, 0);

    

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
          new MyPhysicalVolume(0, posGrease, logicGrease, "Grease", physWorld, false, 0);

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
          MyPhysicalVolume* physTeflon = new MyPhysicalVolume(0, posTeflon, logicTeflon, "Teflon", physWorld, false, 0);
          topContainer = physTeflon;   // 晶体放在Teflon中

          G4double AirX = scintillatorX + airGapThicknessX;
          G4double AirY = scintillatorY + airGapThicknessY;
          G4double AirZ = scintillatorZ + 0.5*airGapThicknessZ;
          if (isAirGap) {
            G4Box* solidAirGap = new G4Box("AirGap", 0.5*AirX, 0.5*AirY, 0.5*AirZ);
            G4LogicalVolume* logicAirGap = new G4LogicalVolume(solidAirGap, air, "AirGap");
            G4ThreeVector posAirGap = G4ThreeVector(0, 0, -0.5*TeflonZ+0.5*AirZ); 
            MyPhysicalVolume* physAirGap = new MyPhysicalVolume(0, posAirGap, logicAirGap, "AirGap", physTeflon, false, 0);
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
          MyPhysicalVolume* physReflector = new MyPhysicalVolume(0, posReflector, logicReflector, "Reflector", physWorld, false, 0);
          // topContainer = logicReflector;
          topContainer = physWorld;

          new G4LogicalSkinSurface("ReflectorSurface", logicReflector, surf_TiO2);

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
      case OpNoviceDetectorConstruction::CYLINDER: // CYLINDER
{
    // 定义 Reflector 的内半径、外半径、高度和顶的厚度
    G4double innerRadius = 35*mm;
    G4double thickness = 3*mm;
    G4double outerRadius = innerRadius + thickness;
    G4double height = 5*cm;
    G4double topThickness = 3*mm;

    // 创建 Reflector 圆柱体部分
    G4Tubs* solidReflectorBody = new G4Tubs("ReflectorBody", innerRadius, outerRadius, 0.5*height, 0, 2*CLHEP::pi);
    G4LogicalVolume* logicReflectorBody = new G4LogicalVolume(solidReflectorBody, PVC, "ReflectorBody");

    // 创建 Reflector 顶部
    G4Tubs* solidReflectorTop = new G4Tubs("ReflectorTop", 0, outerRadius, 0.5*topThickness, 0, 2*CLHEP::pi);
    G4LogicalVolume* logicReflectorTop = new G4LogicalVolume(solidReflectorTop, PVC, "ReflectorTop");

    // 放置 Reflector 圆柱体部分
    G4ThreeVector posReflectorBody = G4ThreeVector(0, 0, -0.5*worldSize + pmtHeight + 0.5*height);
    MyPhysicalVolume* physReflectorBody = new MyPhysicalVolume(0, posReflectorBody, logicReflectorBody, "ReflectorBody", physWorld, false, 0, checkOverlaps);

    // 放置 Reflector 顶部
    G4ThreeVector posReflectorTop = G4ThreeVector(0, 0, posReflectorBody.z() + 0.5*height + 0.5*topThickness);
    MyPhysicalVolume* physReflectorTop = new MyPhysicalVolume(0, posReflectorTop, logicReflectorTop, "ReflectorTop", physWorld, false, 0, checkOverlaps);

    // 创建 Reflector 表面属性
    new G4LogicalSkinSurface("ReflectorSurfaceBody", logicReflectorBody, surf_TiO2);
    new G4LogicalSkinSurface("ReflectorSurfaceTop", logicReflectorTop, surf_TiO2);

    // 设置 Reflector 的颜色和透明度
    G4VisAttributes* ReflectorVisAtt = new G4VisAttributes(G4Colour(1.0, 1.0, 1.0, 0.3));  // 纯白色，透明度0.3
    ReflectorVisAtt->SetVisibility(true);
    ReflectorVisAtt->SetForceSolid(true);  // 设置为实心，以便可以看到透明度
    logicReflectorBody->SetVisAttributes(ReflectorVisAtt);
    logicReflectorTop->SetVisAttributes(ReflectorVisAtt);

    // 设置闪烁体的位置
    posScintillator = G4ThreeVector(0, 0, posReflectorBody.z() - 0.5*height + 0.5*scintillatorZ);

    break;
}

      
      default:
          // No reflector
          break;
  }

  // 闪烁体
  MyPhysicalVolume* physScintillator = new MyPhysicalVolume(0, posScintillator, logicScintillator, "Scintillator", topContainer, false, 0);
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


