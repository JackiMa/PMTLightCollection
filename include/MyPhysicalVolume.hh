#ifndef MY_PHYSICAL_VOLUME_HH
#define MY_PHYSICAL_VOLUME_HH

#include "G4PVPlacement.hh"
#include "G4ThreeVector.hh"

class MyPhysicalVolume : public G4PVPlacement {
private:
    G4ThreeVector absolutePosition;  // 新添加的属性，存储绝对坐标

public:
    // 构造函数
    MyPhysicalVolume(G4RotationMatrix* pRot, const G4ThreeVector& tlate, 
                     const G4String& pName, G4LogicalVolume* pLogical, 
                     MyPhysicalVolume* pMotherVolume, G4bool pMany, 
                     G4int pCopyNo, G4bool pSurfChk = false)
        : G4PVPlacement(pRot, tlate, pName, pLogical, 
                        pMotherVolume ? static_cast<G4VPhysicalVolume*>(pMotherVolume) : nullptr, 
                        pMany, pCopyNo, pSurfChk),
          absolutePosition(pMotherVolume ? pMotherVolume->GetAbsolutePosition() + tlate : tlate)
    {
    }

    // 获取绝对位置
    G4ThreeVector GetAbsolutePosition() const {
        return absolutePosition;
    }
};

#endif // MY_PHYSICAL_VOLUME_HH