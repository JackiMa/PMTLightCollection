#ifndef MY_PHYSICAL_VOLUME_HH
#define MY_PHYSICAL_VOLUME_HH

#include "G4PVPlacement.hh"
#include "G4ThreeVector.hh"

class MyPhysicalVolume {
private:
    G4PVPlacement* placement;    // 内部的G4PVPlacement对象
    G4ThreeVector absolutePosition;  // 新添加的属性，存储绝对坐标

public:
    // 构造函数
    MyPhysicalVolume(G4RotationMatrix* pRot, const G4ThreeVector& tlate, 
                     G4LogicalVolume* pLogical, const G4String& pName,
                     G4VPhysicalVolume* pMotherVolume, G4bool pMany, 
                     G4int pCopyNo, G4bool pSurfChk = false)
    {
        placement = new G4PVPlacement(pRot, tlate, pName, pLogical, 
                                      pMotherVolume, pMany, pCopyNo, pSurfChk);
        if (pMotherVolume) {
            // 计算绝对位置
            MyPhysicalVolume* motherAbs = dynamic_cast<MyPhysicalVolume*>(pMotherVolume);
            if (motherAbs) {
                absolutePosition = motherAbs->GetAbsolutePosition() + tlate;
            }
        } else {
            absolutePosition = tlate;  // 如果没有母体，则绝对位置为相对位置
        }
    }

    ~MyPhysicalVolume() {
        delete placement;  // 清理内部G4PVPlacement对象
    }

    // 获取绝对位置
    G4ThreeVector GetAbsolutePosition() const {
        return absolutePosition;
    }

    // 获取内部的G4PVPlacement对象，以便于Geant4内部使用
    G4PVPlacement* GetG4Placement() const {
        return placement;
    }

    // 类型转换运算符
    operator G4VPhysicalVolume*() const {
        return placement;
    }
};


#endif // MY_PHYSICAL_VOLUME_HH