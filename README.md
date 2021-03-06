# 网络游戏实验示例
 
&nbsp;&nbsp;&nbsp;&nbsp;示例实现基于Epic官方ShooterGame和ActionRPG两个学习项目融合扩展。旨在学习并掌握UE4引擎各模块基于C++的交互工作流程。
Engine版本：UE 4.20.3
<a href="https://www.bilibili.com/video/BV11t4y1a7p5"> 示例视频链接</a>  

##### 
![GamePreview](https://yw-dev.github.io/content/posts/2019-04-17/Shotcut.png)


####  概述:
![InGame流程图](https://yw-dev.github.io/content/posts/2019-04-17/InGame.png)

##  GamePlay

####  数据:

- **DataAsset/DataTable(CSV)：** 角色默认状态表、各道具数据结构的基本Asset配置。

####  UI:

- **模拟大厅主菜单：** 角色模型级联菜单/session检索/退出。
- **玩家主界面：** TeamBar/PlayerDashboard/角色目标.
- **商店：** 物品分类/物品一览/物品详情/购买。
- **Alert提示：** 动态创建TextBlock显示RPC请求的返回结果(如：购买功能)。
- **计分板：**  团队角色计分一览。
- **小地图：**  
- **玩家目标：**  3D头像、玩家昵称、等级信息。

####  Props:

- **Armor：** 
- **Weapon：** 
- **Skill：** 
- **Pickup：** 
- **Pawn：** 

####  Character:

- **角色：** 
- **头像：** Camera/RenderTarget实现捕捉角色模型material渲染至UI控件。
- **状态机：** 根据武器的类型执行默认、法杖、剑、斧、枪械等对应的子状态机。
- **目标：**  
- **近战攻击/伤害：** AbilitySystemComponent激活攻击的能力，LineTrace/AnimNotify实现近战攻击径向多目标的命中碰撞检测和通知。
- **远程技能/伤害：**  
- **等级/晋级：** 
- **Inventory：** 有限数量的Inventory Slots； 固定主/复武器Slots位. 相对数量的护具、药品等。

####  GameMode:

&nbsp;&nbsp;&nbsp;&nbsp;.

####  NetWork:

- **WorkFlow：** 
- **RPC：** 购买/Alert消息、激活攻击/触发伤害
- **Replication：** PlayerState、AttributeSet、WeaponActors、DamageInfo

####  AsyncTask:

&nbsp;&nbsp;&nbsp;&nbsp;. 

##  OnlineBecons

&nbsp;&nbsp;&nbsp;&nbsp;. 

##  插件化 

&nbsp;&nbsp;&nbsp;&nbsp;. 






