Public ：

AccountData.h：定义账户数据相关类，用于存储账户信息（如拥有的角色、装备、队伍信息等），提供角色添加等数据操作的接口，其实现对应 Private 中的AccountData.cpp。

AccountManager.h：定义账户管理类，负责账户的整体管理逻辑，可能包括账户数据的加载、保存、维护等，实现对应AccountManager.cpp。

AccountRegisterAndReader.h：定义账户注册与读取相关类，提供账户登录、注册、数据加载等功能的接口，实现对应AccountRegisterAndReader.cpp。

Attack_GameplayAbility.h：定义攻击相关的游戏能力类，继承自游戏能力基类，封装攻击能力的激活、执行等逻辑，实现对应Attack_GameplayAbility.cpp。

AttackAttach.h：定义攻击附着相关类，可能用于处理攻击过程中与其他对象的附着逻辑，实现对应AttackAttach.cpp。

AttackDoneAnimNotify.h：定义攻击完成动画通知类，用于在动画播放到攻击完成时触发特定逻辑（如伤害结算、状态切换等），实现对应AttackDoneAnimNotify.cpp。

AttackTrace.h：定义攻击轨迹检测类，用于检测攻击过程中的碰撞、命中判定等，实现对应AttackTrace.cpp。

CharacterDamageDA.h：定义角色伤害数据访问类，提供伤害率获取等伤害相关数据操作的接口，实现对应CharacterDamageDA.cpp。

CharacterDetail.h：定义角色详情类，负责角色详情的展示逻辑（如加载后初始化展示、角色模型在锚点生成等），实现对应CharacterDetail.cpp。

CharacterInfo.h：定义角色信息类，存储角色的基础信息（如名称、属性等），实现对应CharacterInfo.cpp。

CPP_BaseCharacter.h：定义基础角色类，作为玩家角色和敌人角色的父类，封装角色的通用属性和方法（如碰撞体设置、基础移动等），包含ReSetSphereCollisionRadius等方法，实现对应CPP_BaseCharacter.cpp。

CPP_EnemyCharacter.h：定义敌人角色类，继承自基础角色类，封装敌人特有的属性和行为（如 AI 逻辑、攻击模式等），实现对应CPP_EnemyCharacter.cpp。

CPP_PlayerCharacter.h：定义玩家角色类，继承自基础角色类，封装玩家角色的特有逻辑（如能力系统组件初始化、被控制器拥有时的处理等），实现对应CPP_PlayerCharacter.cpp。

CPP_PlayerController.h：定义玩家控制器类，负责处理玩家输入、角色控制、UI 交互等逻辑（如跳跃处理、相机跟随设置等），实现对应CPP_PlayerController.cpp。

CPP_PlayerState.h：定义玩家状态类，存储玩家的状态信息（如队伍数据、运行 / 行走状态等），提供队伍重载等功能，实现对应CPP_PlayerState.cpp。

DamageCalculation.h：定义伤害计算类，封装伤害计算的逻辑（如属性捕获、伤害公式计算等），实现对应DamageCalculation.cpp。

EnemyAttributeSet.h：定义敌人属性集类，存储敌人的属性数据（如攻击、防御、生命值等），实现对应EnemyAttributeSet.cpp。

EnemyHPBarWidget.h：定义敌人血条 UI 组件类，负责敌人生命值的可视化展示，实现对应EnemyHPBarWidget.cpp。

EquipmentImprove.h：定义装备强化类，提供装备强化相关的逻辑（如移除装备加成效果等），实现对应EquipmentImprove.cpp。

EquipmentItemWidget.h：定义装备物品 UI 组件类，用于展示单个装备的信息和交互，实现对应EquipmentItemWidget.cpp。

EquipmentManager.h：定义装备管理类，负责装备的整体管理（如获取所有装备、设置装备状态等），实现对应EquipmentManager.cpp。

EquipmentSlotWidget.h：定义装备槽 UI 组件类，负责装备槽的展示和装备穿戴、更换等交互逻辑，实现对应EquipmentSlotWidget.cpp。

FuncButtomItem.h：定义功能按钮项类，可能是功能按钮的基础组件，实现对应FuncButtomItem.cpp。

FuncItemButtom.h：定义功能物品按钮类，处理功能物品按钮的点击等交互逻辑，实现对应FuncItemButtom.cpp。

GamingPlayerUI.h：定义游戏中玩家 UI 类，负责玩家 UI 的展示和更新（如技能冷却、生命值显示等），实现对应GamingPlayerUI.cpp。

GASGameInstance.h：定义游戏实例类，负责管理全局游戏数据（如账户注册与读取器实例、当前登录账户信息等），贯穿游戏整个生命周期，作为各模块间数据交互的枢纽，实现对应 Private 中的GASGameInstance.cpp。

GetHitGameplayAbility.h：定义受击相关的游戏能力类，继承自游戏能力基类，封装角色受击时的能力逻辑（如受击动画触发、受击状态切换、移除攻击标签等），实现对应 Private 中的GetHitGameplayAbility.cpp。

LoginWidget.h：定义登录界面 UI 组件类，负责登录界面的展示与交互（如用户名 / 密码输入框、登录按钮绑定等），提供触发登录逻辑的接口，实现对应 Private 中的LoginWidget.cpp。

PauseMenuWidget.h：定义暂停菜单 UI 组件类，负责暂停菜单的展示与交互（如关闭菜单、设置选项等），处理暂停状态下的游戏逻辑切换，实现对应 Private 中的PauseMenuWidget.cpp。

PlayerAttributeSet.h：定义玩家属性集类，存储玩家核心属性数据（如攻击、防御、生命值等），作为能力系统组件（ASC）的属性基础，支持属性修改与监听，实现对应 Private 中的PlayerAttributeSet.cpp。

PlayerCameraActor.h：定义玩家相机类，封装相机视角控制逻辑（如旋转灵敏度、俯仰角限制等），负责跟随玩家角色并处理视角更新，实现对应 Private 中的PlayerCameraActor.cpp。

SwordSlash.h：定义剑类武器攻击相关的类，封装剑砍击的特效、碰撞检测范围、伤害判定时机等逻辑，与攻击能力类配合实现剑攻击效果，实现对应 Private 中的SwordSlash.cpp。

TeamMakerCharacterIconUI.h：定义队伍制作界面的角色图标 UI 组件类，负责单个角色图标的展示（如角色头像、选中状态等），处理图标与角色数据的绑定，实现对应 Private 中的TeamMakerCharacterIconUI.cpp。

TeamMakerWidget.h：定义队伍编辑页面的类，负责管理玩家队伍信息，修改以及展示，实现对应 Private 的 TeamMakerWidget.cpp。

TeamManager.h：定义队伍管理器类，负责队伍成员的初始化、切换、销毁等核心管理逻辑（如根据角色类生成队伍成员、切换当前控制角色等），协调队伍与玩家控制器的交互，实现对应 Private 中的TeamManager.cpp。

TeamWidgetItem.h：定义队伍编辑界面单个角色展示的图标的类，负责显示对应队伍对应序号的角色图标，可接收点击或者拖动信息，实现对应 Private 中的TeamWidgetItem.cpp。

Private ：

AccountData.cpp：实现UAccountData类的功能，包含AddCharacter方法，用于向账户添加角色（包含重复检查和日志输出）。

AccountManager.cpp：实现UAccountManager类的功能，负责记录现有的所有账户。

AccountRegisterAndReader.cpp：实现UAccountRegisterAndReader类的LoginAccount方法，处理账户登录逻辑（验证用户名密码、关联游戏实例等）。

Attack_GameplayAbility.cpp：实现UAttack_GameplayAbility类的ActivateAbility方法，处理攻击能力激活时的初始化（如获取角色、控制器、能力系统组件，设置动画回调等）。

AttackAttach.cpp：实现UAttackAttach类的Enemy_FaceToActor方法，目前为空实现，推测用于敌人面向目标的逻辑。

AttackDoneAnimNotify.cpp：实现攻击完成动画通知的触发逻辑。

AttackTrace.cpp：实现UAttackTrace类的NotifyBegin方法，在动画开始时初始化攻击轨迹检测（如获取武器组件、设置轨迹起点等）。

CharacterDamageDA.cpp：实现UCharacterDamageDA类的GetDamageRate方法，根据索引从伤害数据映射中获取伤害率（默认返回 1.0）。

CharacterDetail.cpp：实现UCharacterDetail类的OnLevelLoaded方法，处理关卡加载完成后的角色详情展示逻辑（如查找展示 Actor、切换相机、生成展示角色等）。

CharacterInfo.cpp：角色DataTable表结构。

CPP_BaseCharacter.cpp：实现ACPP_BaseCharacter类的构造函数，初始化角色的碰撞体（球体碰撞组件）。

CPP_EnemyCharacter.cpp：敌人角色的特有逻辑（如 AI 行为、攻击处理等）。

CPP_PlayerCharacter.cpp：实现ACPP_PlayerCharacter类的PossessedBy方法，处理角色被控制器拥有时的逻辑（如应用装备数据、初始化能力系统、同步移动速度等）。

CPP_PlayerController.cpp：实现ACPP_PlayerController类的OnJump方法，处理玩家跳跃输入（触发角色跳跃、调用能力测试等）。

CPP_PlayerState.cpp：实现ACPP_PlayerState类的ReloadCurrentTeam方法，从账户数据重载当前队伍（加载角色类、初始化队伍管理器等）。

DamageCalculation.cpp：定义FDamageStatics结构体，用于伤害计算中的属性捕获（如玩家和敌人的攻击、防御、生命值等）。

EnemyAttributeSet.cpp：实现敌人属性集的初始化、属性修改等逻辑。

EnemyHPBarWidget.cpp：实现敌人血条 UI 的更新逻辑（如根据生命值变化刷新显示）。

EquipmentImprove.cpp：实现管理装备加成相关的游戏效果。

EquipmentItemWidget.cpp：实现装备物品 UI 的展示和交互逻辑。

EquipmentManager.cpp：实现UEquipmentManager类的GetAllEquipment方法，获取指定账户拥有的所有装备（无效账户返回空数组）。

EquipmentSlotWidget.cpp：实现UEquipmentSlotWidget类的SetSlotEquipment方法，处理装备槽设置装备的逻辑（验证类型、更新装备状态、刷新 UI 等）。

FuncButtomItem.cpp：实现功能按钮项的基础交互逻辑。

FuncItemButtom.cpp：实现UFuncItemButtom类的OnClick方法，处理功能物品按钮点击事件（向父级列表广播点击的项）。

GamingPlayerUI.cpp：实现UGamingPlayerUI类的NativeTick方法，在每帧更新技能 UI（如根据技能冷却时间更新技能图标显示）。

GamingPlayerUI.cpp：实现UGASGameInstance类的功能，管理账户注册与读取器实例、当前登录账户等全局数据，提供各模块访问全局数据的接口，确保游戏生命周期内数据的持续性。

GetHitGameplayAbility.cpp：实现UGetHitGameplayAbility类的EndAbility方法，处理受击能力结束时的逻辑（移除相关游戏标签）。

LoginWidget.cpp：实现ULoginWidget类的TryLogin方法，处理登录 UI 的登录逻辑（获取输入的用户名密码、调用登录接口、更新游戏实例等）。

PauseMenuWidget.cpp：实现UPauseMenuWidget类的ClosePauseMenu方法，处理暂停菜单关闭逻辑（移除 UI、恢复游戏运行、设置输入模式等）。

PlayerAttributeSet.cpp：实现UPlayerAttributeSet类的功能，包括玩家属性（攻击、防御、生命值等）的初始化、默认值设置、属性修改方法（如通过游戏效果调整属性）及属性变化的通知逻辑。

PlayerCameraActor.cpp：实现APlayerCameraActor类的AddViewRotation方法，处理相机视角旋转逻辑（限制俯仰角、重置翻滚角等）。

SwordSlash.cpp：实现USwordSlash类的功能，处理剑砍击的具体逻辑（如播放攻击特效、检测碰撞范围内的敌人、传递伤害信息给命中目标等），与攻击轨迹检测类配合完成攻击判定。

TeamMakerCharacterIconUI.cpp：实现UTeamMakerCharacterIconUI类的功能，处理队伍制作界面中角色图标的 UI 逻辑（如根据角色数据加载头像纹理、响应选中事件切换图标状态等），绑定角色数据与 UI 展示。### Public 文件夹补充文件功能（按首字母排序）

TeamMakerWidget.cpp：实现UTeamMakerWidget类的PressTeam6方法，切换到第 6 个队伍页面（调用SwitchTeamPage(5)）。

TeamWidgetItem.cpp：实现UTeamWidgetItem类的NativeOnListItemObjectSet和SelectCharacter方法，处理团队界面列表项的初始化（关联角色、绑定选择事件）和角色选择逻辑（通知 UI 更新选中角色、展示角色详情等）。

TeamManager.cpp：实现UTeamManager类的Initialize方法，初始化队伍管理器（销毁旧角色、生成新角色、切换到第一个角色等）。
