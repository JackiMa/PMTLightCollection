#include <vector>

#include "LightCollectionDetectorConstruction.hh"
#include "LightCollectionDetectorMessenger.hh"
#include "LightCollectionLayerSensitiveDetector.hh"

#include "G4SDManager.hh"
#include "G4Box.hh"
#include "G4Element.hh"
#include "G4GDMLParser.hh"
#include "G4LogicalBorderSurface.hh"
#include "G4LogicalSkinSurface.hh"
#include "G4LogicalVolume.hh"
#include "G4OpticalSurface.hh"
#include "G4VPhysicalVolume.hh"
#include "G4SystemOfUnits.hh"
#include "G4ThreeVector.hh"
#include "G4VisAttributes.hh"
#include "G4SubtractionSolid.hh"
#include "G4UnionSolid.hh"

#include "MyMaterials.hh"
#include "utilities.hh"
#include "MyPhysicalVolume.hh"
#include "config.hh"

std::vector<G4ThreeVector> generateCoordinates(int nums, double gaps);

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
// 在这里对类相关参数进行初始化
LightCollectionDetectorConstruction::LightCollectionDetectorConstruction()
  : G4VUserDetectorConstruction()
{
  fDumpGdmlFileName = "LightCollecion.gdml";
  fVerbose          = false;  // 是否输出详细信息
  fDumpGdml         = false;  // 是否保存GDML的几何文件
  // create a messenger for this class
  fDetectorMessenger = new LightCollectionDetectorMessenger(this);
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
LightCollectionDetectorConstruction::~LightCollectionDetectorConstruction()
{
  delete fDetectorMessenger;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
G4VPhysicalVolume* LightCollectionDetectorConstruction::Construct()
{
  G4bool checkOverlaps = false;




  // ------------- Volumes --------------
  // s_ for soild_
  // l_ for logical_
  // p_ for physical_
  //
    // The world
    G4Box* s_world = new G4Box("World", 0.5*g_worldX, 0.5*g_worldY, 0.5*g_worldZ);
    G4LogicalVolume* l_world = new G4LogicalVolume(s_world, g_world_material, "World");
    MyPhysicalVolume* p_world = new MyPhysicalVolume(0, G4ThreeVector(), "World", l_world, nullptr, false, 0, checkOverlaps);
    
    //
    // shield
    G4Box* s_shield = new G4Box("shield",0.5*g_shieldX, 0.5*g_shieldY, 0.5*g_shield_thickness);
    G4LogicalVolume* l_shield = new G4LogicalVolume(s_shield, g_shield_material, "shield");
    MyPhysicalVolume* p_shield = new MyPhysicalVolume(0, g_shield_pos, "shield", l_shield, p_world, false, checkOverlaps);
    fVolumeMap["shield"] = p_shield;
    for (int i = 0; i < g_shield_layers; ++i) {
      std::string layer_name = "shield" + std::to_string(i);
      G4Box* s_layer = new G4Box(layer_name, 0.5 * g_shieldX, 0.5 * g_shieldY, 0.5 * g_layer_thickness);
      G4LogicalVolume* l_layer = new G4LogicalVolume(s_layer, g_shield_material, layer_name);
      G4ThreeVector layer_pos = G4ThreeVector(0, 0, (i - g_shield_layers / 2.0 + 0.5) * g_layer_thickness);
      MyPhysicalVolume* p_layer = new MyPhysicalVolume(0, layer_pos, layer_name, l_layer, p_shield, false, checkOverlaps);
      fVolumeMap[layer_name] = p_layer;
      // set shield layers colour
      double ratio = static_cast<double>(i) / (g_shield_layers - 1);
      double red = ratio; // 从0到1
      double green = 0.0; // 保持为0
      double blue = 1.0 - ratio; // 从1到0
      G4VisAttributes* layer_VisAtt = new G4VisAttributes(G4Colour(red, green, blue, 0.8));
      layer_VisAtt->SetForceSolid(true);  // 设置为实心，以便可以看到透明度
      layer_VisAtt->SetVisibility(true);
      l_layer->SetVisAttributes(layer_VisAtt);

          // 注册敏感探测器
      G4SDManager* sdManager = G4SDManager::GetSDMpointer();
      LayerSensitiveDetector* layerSD = new LayerSensitiveDetector(layer_name, layer_name + "HitsCollection");
      sdManager->AddNewDetector(layerSD);
      l_layer->SetSensitiveDetector(layerSD);
    }
    // 设置 shield 的颜色，似乎会覆盖掉layers的颜色
    // G4VisAttributes* shield_VisAtt = new G4VisAttributes(G4Colour(1, 1, 1, 0.8));
    // shield_VisAtt->SetForceSolid(true);  // 设置为实心，以便可以看到透明度
    // shield_VisAtt->SetVisibility(true);
    // l_shield->SetVisAttributes(shield_VisAtt);

    // SD1
    G4double SD1_thickness = 1*mm;
    G4ThreeVector SD1_pos = g_shield_pos - G4ThreeVector(0, 0, 0.5*g_shield_thickness + 0.5*SD1_thickness);
    G4Box* s_SD1 = new G4Box("SD1",0.5*g_shieldX, 0.5*g_shieldY, SD1_thickness);
    G4LogicalVolume* l_SD1 = new G4LogicalVolume(s_SD1, g_world_material, "SD1");
    MyPhysicalVolume* p_SD1 = new MyPhysicalVolume(0, SD1_pos, "SD1", l_SD1, p_world, false, checkOverlaps);
    fVolumeMap["SD1"] = p_SD1;
    G4VisAttributes* SD1_VisAtt = new G4VisAttributes(G4Colour(1, 1, 1, 0.3));
    SD1_VisAtt->SetForceSolid(true);  // 设置为实心，以便可以看到透明度
    SD1_VisAtt->SetVisibility(true);
    l_SD1->SetVisAttributes(SD1_VisAtt);

  // ▣
  // scintillator 
  // wrapper_hole for lightguide
  std::vector<G4ThreeVector> coordinates = generateCoordinates(g_lg_nums, g_lg_gap); // 根据lightguide的数量生成坐标
  G4RotationMatrix* rotationMatrix = new G4RotationMatrix(); // 创建旋转矩阵，将其从Z轴旋转到X轴
  rotationMatrix->rotateY(90 * deg); // 绕Y轴旋转90度
  // 创建初始几何体
  G4Box* s_sc_wrapper = new G4Box("sc_wrapper", 0.5 * g_scintillatorX, 0.5 * g_scintillatorY, 0.5 * g_scintillatorZ);
  G4Tubs* s_wrapper_hole = new G4Tubs("wrapper_hole", 0, 1.1 * 0.5 * g_lightguide_r, 0.5*g_sc_wrapper_thickness, 0, 360 * deg);
  G4ThreeVector holes_pos(0.5 * g_scintillatorX - 0.5 * g_sc_wrapper_thickness, 0, 0); // hole组相对sc的位置
  G4VSolid* s_sc_wrapper_wrapper_with_hole = s_sc_wrapper;
  // 遍历 coordinates，依次减去所有的 hole
  for (const auto& coord : coordinates) {
      G4ThreeVector hole_position = coord + holes_pos;
      s_sc_wrapper_wrapper_with_hole = new G4SubtractionSolid("sc_wrapper_wrapper_with_hole", s_sc_wrapper_wrapper_with_hole, s_wrapper_hole, rotationMatrix, hole_position);
  }
  G4LogicalVolume* l_sc_wrapper = new G4LogicalVolume(s_sc_wrapper_wrapper_with_hole, g_sc_wrapper_material, "sc_wrapper");
  MyPhysicalVolume* p_sc_wrapper = new MyPhysicalVolume(0, g_scintillator_pos, "sc_wrapper", l_sc_wrapper, p_world, false, 0, checkOverlaps);
  fVolumeMap["sc_wrapper"] = p_sc_wrapper;
    G4VisAttributes* sc_wrapper_VisAtt = new G4VisAttributes(G4Colour(1.0, 1, 1, 0.3));
    sc_wrapper_VisAtt->SetVisibility(true);
    sc_wrapper_VisAtt->SetForceSolid(true);  // 设置为实心，以便可以看到透明度
    l_sc_wrapper->SetVisAttributes(sc_wrapper_VisAtt);

    // crystal
    G4Box* s_sc_crystal = new G4Box("sc_crystal",0.5*g_scintillatorX-g_sc_wrapper_thickness, 0.5*g_scintillatorY-g_sc_wrapper_thickness, 0.5*g_scintillatorZ-g_sc_wrapper_thickness);
    G4LogicalVolume* l_sc_crystal = new G4LogicalVolume(s_sc_crystal, g_sc_crystal_material, "sc_crystal");
    MyPhysicalVolume* p_sc_crystal = new MyPhysicalVolume(0, G4ThreeVector(0,0,0), "sc_crystal", l_sc_crystal, p_sc_wrapper, false, checkOverlaps);
    fVolumeMap["sc_crystal"] = p_sc_crystal;
    G4VisAttributes* sc_crystal_VisAtt = new G4VisAttributes(G4Colour(1.0, 0.65, 0, 0.3));
    sc_crystal_VisAtt->SetForceSolid(true);  // 设置为实心，以便可以看到透明度
    sc_crystal_VisAtt->SetVisibility(true);
    l_sc_crystal->SetVisAttributes(sc_crystal_VisAtt);

    // 


    //
    // lightguide
    G4Tubs* s_lightguide = new G4Tubs("lightguide", 0, 0.5 * g_lightguide_r, 0.5 * g_lightguide_length, 0, 360 * deg);
    G4LogicalVolume* l_lightguide = new G4LogicalVolume(s_lightguide, g_lg_fiber_material, "lightguide");

    // lg_fiber
    G4double fiber_r = g_lightguide_r - 2 * g_lg_wrapper_thickness;
    G4Tubs* s_lg_fiber = new G4Tubs("lg_fiber", 0, 0.5 * fiber_r, 0.5 * g_lightguide_length, 0, 360 * deg);
    G4LogicalVolume* l_lg_fiber = new G4LogicalVolume(s_lg_fiber, g_lg_fiber_material, "lg_fiber");
    // 将 lg_fiber 放置在 lightguide 中
    new G4PVPlacement(0, G4ThreeVector(0, 0, 0), l_lg_fiber, "lg_fiber", l_lightguide, false, 0, checkOverlaps);

    // SD2
    G4double SD2_length = 1 * mm;
    G4double SD2_pos_z = 0.5 * g_lightguide_length - g_lg_depth; // 希望设置在紧靠crystal的一端（X负半轴），由于放置圆柱体时要旋转，所以这里指定为Z负半轴。
    G4Tubs* s_SD2 = new G4Tubs("SD2", 0, 0.5 * fiber_r, 0.5 * SD2_length, 0, 360 * deg);
    G4LogicalVolume* l_SD2 = new G4LogicalVolume(s_SD2, g_lg_fiber_material, "SD2");
    // 将 SD2 放置在 lg_fiber 中
    new G4PVPlacement(0, G4ThreeVector(0, 0, SD2_pos_z), l_SD2, "SD2", l_lg_fiber, false, 0, checkOverlaps);

    // 根据坐标生成nums个lightguide
    int copyNumber = 0;
    for (const auto& coord : coordinates) {
        G4ThreeVector lg_pos = coord + g_lightguide_pos;
        MyPhysicalVolume* p_lightguide = new MyPhysicalVolume(rotationMatrix, lg_pos, "lightguide", l_lightguide, p_world, false, copyNumber, checkOverlaps);
        fVolumeMap["lightguide" + std::to_string(copyNumber)] = p_lightguide;
        copyNumber++; 
    }
    
    // 设置 lightguide 的颜色
    G4VisAttributes* lightguide_VisAtt = new G4VisAttributes(G4Colour(0.2, 0.2, 0.2, 0.8));
    lightguide_VisAtt->SetForceSolid(true);
    lightguide_VisAtt->SetVisibility(true);
    l_lightguide->SetVisAttributes(lightguide_VisAtt);
    G4VisAttributes* SD2_VisAtt = new G4VisAttributes(G4Colour(1, 1, 1, 0.3));
    SD2_VisAtt->SetForceSolid(true);
    SD2_VisAtt->SetVisibility(true);
    l_SD2->SetVisAttributes(SD2_VisAtt);
    G4VisAttributes* lg_fiber_VisAtt = new G4VisAttributes(G4Colour(0.8, 0.65, 0.2, 0.7)); 
    lg_fiber_VisAtt->SetForceSolid(true); 
    lg_fiber_VisAtt->SetVisibility(true);
    l_lg_fiber->SetVisAttributes(lg_fiber_VisAtt);

    // 设置两个界面的光学特性
    // new G4LogicalBorderSurface("PhotocathodeInterface", physcrystal, physPhotocathode, surf_GlassToPhotocathode);
    // 设置材料表面的光学特性
    new G4LogicalSkinSurface("TeflonSurface",l_sc_wrapper, surf_Teflon);
    new G4LogicalSkinSurface("TeflonSurface",l_lightguide, surf_Teflon);

  
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
          parser->Write(fDumpGdmlFileName, p_world);
      }
  }

  
  return p_world;
}

#include "G4Exception.hh"

MyPhysicalVolume* LightCollectionDetectorConstruction::GetMyVolume(G4String volumeName) const
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
    G4Exception("LightCollectionDetectorConstruction::GetMyVolume",
                "VolumeNotFound", FatalException,
                ("Volume " + volumeName + " not found in the volume map.").c_str());
    return nullptr; // 这行代码实际上不会被执行，因为G4Exception会终止程序
  }
}


//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
void LightCollectionDetectorConstruction::SetDumpGdml(G4bool val) { fDumpGdml = val; }

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
G4bool LightCollectionDetectorConstruction::IsDumpGdml() const { return fDumpGdml; }

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
void LightCollectionDetectorConstruction::SetVerbose(G4bool val) { fVerbose = val; }

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
G4bool LightCollectionDetectorConstruction::IsVerbose() const { return fVerbose; }

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
void LightCollectionDetectorConstruction::SetDumpGdmlFile(G4String filename)
{
  fDumpGdmlFileName = filename;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
G4String LightCollectionDetectorConstruction::GetDumpGdmlFile() const
{
  return fDumpGdmlFileName;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
void LightCollectionDetectorConstruction::PrintError(G4String ed)
{
  G4Exception("LightCollectionDetectorConstruction:MaterialProperty test", "op001",
              FatalException, ed);
}

// 根据输入数量和间隔生成坐标
std::vector<G4ThreeVector> generateCoordinates(int nums, double gaps) {
    std::vector<G4ThreeVector> coordinates;

    if (nums <= 0) {
        return coordinates;
    }

    if (nums == 1) {
        coordinates.push_back(G4ThreeVector(0, 0, 0));
    } else if (nums == 2) {
        coordinates.push_back(G4ThreeVector(0, 0, -gaps / 2));
        coordinates.push_back(G4ThreeVector(0, 0, gaps / 2));
    } else if (nums == 3) {
        coordinates.push_back(G4ThreeVector(0, -gaps / 2, -sqrt(3)*gaps / 6));
        coordinates.push_back(G4ThreeVector(0, gaps / 2, -sqrt(3)*gaps / 6));
        coordinates.push_back(G4ThreeVector(0, sqrt(3)*gaps/3, 0));
    } else if (nums == 4) {
        coordinates.push_back(G4ThreeVector(0, -gaps / 2, -gaps / 2));
        coordinates.push_back(G4ThreeVector(0, -gaps / 2, gaps / 2));
        coordinates.push_back(G4ThreeVector(0, gaps / 2, -gaps / 2));
        coordinates.push_back(G4ThreeVector(0, gaps / 2, gaps / 2));
    } else if (nums == 5 or nums == 6 or nums == 7) {
        coordinates.push_back(G4ThreeVector(0, 0, 0));
        double radius = gaps;
        double angle = 2 * M_PI / (nums-1);
        for (int i = 0; i < (nums-1); ++i) {
            double y = radius * std::cos(i * angle);
            double z = radius * std::sin(i * angle);
            coordinates.push_back(G4ThreeVector(0, y, z));
        }
    }
    else {
    G4Exception("LightCollectionDetectorConstruction", "op001", FatalException, 
                "The number of lightguides is not supported. Supported numbers are integers from 1 to 7.");
    }

    return coordinates;
}