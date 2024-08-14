# 代码说明
## StackingAction
用于处理发生某个物理过程时的数据。在本项目中，用于处理产生闪烁光的物理过程，统计产生的闪烁光光子数

## 统计信息
### 直接获取的统计信息
- source产生的能谱
    * proton
    * electron
    * gamma
- 经过shield后的能谱
    * proton
    * electron
    * gamma
- 在scintillator中沉积的能谱
    * proton
    * electrons 
    * gamma
    * 总能谱
- scintillator产生的光谱（波长-数量）
- 光纤中收集到的光谱（波长-数量）

### 间接获取的统计信息
- 由能谱获取剂量
- 由光谱获取光产额