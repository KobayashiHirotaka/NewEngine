[![DebugBuild](https://github.com/KobayashiHirotaka/NewEngine/actions/workflows/DebugBuild.yml/badge.svg)](https://github.com/KobayashiHirotaka/NewEngine/actions/workflows/DebugBuild.yml)
[![ReleaseBuild](https://github.com/KobayashiHirotaka/NewEngine/actions/workflows/ReleaseBuild.yml/badge.svg)](https://github.com/KobayashiHirotaka/NewEngine/actions/workflows/ReleaseBuild.yml)
# 現在制作しているゲーム
## 『モノクロファイター』
- ジャンル：格闘
- 制作開始：2024年 1月～ (現在約1年)
- 制作時間：約500時間
- 制作人数：1人
- 開発環境：DirectX12、Windows SDK(10.0.22621.3233)
- 外部ライブラリ：assimp、DirectXTex、imgui、nlohmann/json

![モノクロファイター](https://github.com/user-attachments/assets/bc0bb249-77c6-4b04-b3c0-14a262a67c2d)

## 特にこだわっているところ
### 気持ちのいいコンボ
**プレイしていても見ていても気持ちのいいコンボ**になるように技と技をスムーズにつなぐことを意識して制作しています。  
また、既存のゲームを参考にボタンを連打することでコンボがつながるような工夫もしています。

![Combo](https://github.com/user-attachments/assets/3379bbac-4e2e-44bc-9357-d0a55c0ef9df)

### 敵との対戦
**敵との駆け引き**を面白くできるように敵AIの実装にも力を入れています。
- HPに応じた行動パターンの変化
- プレイヤーの行動に対して反撃を取る
- 様子見など

![jump](https://github.com/user-attachments/assets/a7c2df31-87cb-4d40-9b42-cd2559b91fbb)


## アタックエディターについて
https://github.com/KobayashiHirotaka/NewEngine/tree/master/Project/Application/Game/AttackEditor
### 実装した経緯
格闘ゲームを制作していて、特に苦労した部分が**各攻撃のパラメータ調整**だったため、今後  
攻撃やコンボの実装・調整をしやすくするためにエディターを作成しました。

### 主な機能
- 各攻撃のパラメータをImGuiで調整
- 調整したパラメータをjsonとして出力
- jsonを読み込んでパラメータを適応

### 調整できるパラメータ
- 当たり判定をつける時間
- 硬直時間
- ダメージ
- ゲージ増加量
など

![スクリーンショット 2025-01-20 145123](https://github.com/user-attachments/assets/6d573113-f148-4d69-b5f4-13c55808abb2)

### 攻撃やコンボの調整をしやすくするための工夫
攻撃やコンボの調整をしやすくするために入力履歴の表示や当たり判定の可視化なども実装し、  
より調整をしやすいように工夫しています。

![スクリーンショット 2025-01-21 231432](https://github.com/user-attachments/assets/033cdadc-9ee8-4bed-9563-22fe6509ca67)

## 今後の展望
- 攻撃時やコンボ時に背景を変化させる(背景オブジェクトが壊れるなど)
- エフェクトの強化
