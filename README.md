# Readme 
---
    
## MOBA网络游戏实验示例

&nbsp;&nbsp;&nbsp;&nbsp;示例实现基于Epic官方ShooterGame和ActionRPG两个学习项目融合扩展而成。旨在学习并掌握UE4引擎各模块基于C++的交互工作流程（玩法模式参考Dota2和英雄联盟）。
Engine版本：UE 4.20.3

##### 
![GamePreview](https://yw-dev.github.io/content/posts/2019-04-17/Shotcut.png)
<a href="https://www.bilibili.com/video/BV1cz4y1o79a">视频链接</a>  

###  概述:
![InGame流程图](https://yw-dev.github.io/content/posts/2019-04-17/InGame.png)

###  UI内容:

&nbsp;&nbsp;&nbsp;&nbsp;UI的两种实现方式：使用Slate的实现方式：Slate / Style / UE4智能指针库实现(参考ShooterGame中UI菜单实现方式)。另一种使用UMG / C++事件驱动UI(Delegate/Event)方式实现。

- **模拟大厅主菜单：** Slate/Style/HUD：角色模型级联菜单/session检索/退出。
- **玩家主界面：** UMG/HUD/C++驱动UI：TeamBar/PlayerDashboard/角色目标.
- **商店：** UMG/HUD/C++驱动UI：物品分类/物品一览/物品详情。
- **Alert提示：** 
- **计分板：**  Slate/Style/HUD：团队角色计分一览。
- **小地图：** 

###  Gameplay内容:

&nbsp;&nbsp;&nbsp;&nbsp;Gameplay作为UE4游戏核心应用层框架结合AssetManager/GameplayAbilitySystem(GAS)实现游戏流程控制、资源数据管理、玩家角色能力、能力数据、效果赋予与交互等内容。

####  道具:

&nbsp;&nbsp;&nbsp;&nbsp;游戏中各类道具Actor Items实现主要使用DataAsset/DataTable + AssetManager注册管理 + StreamableManager运行时加载。

- **防具/武器：** 
- **技能：** 
- **Pickup：** 
- **Pawn：** 

####  角色与Ability:

&nbsp;&nbsp;&nbsp;&nbsp;

- **角色：** 
- **Ability：** 

####  多人玩法:
- **模式：** 

###  Asset/Data内容:
- **DataAsset/DataTable(All Items)：** 
- **SaveGame：** 

###  插件化:




