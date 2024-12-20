[![DebugBuild](https://github.com/KobayashiHirotaka/NewEngine/actions/workflows/DebugBuild.yml/badge.svg)](https://github.com/KobayashiHirotaka/NewEngine/actions/workflows/DebugBuild.yml)
[![ReleaseBuild](https://github.com/KobayashiHirotaka/NewEngine/actions/workflows/ReleaseBuild.yml/badge.svg)](https://github.com/KobayashiHirotaka/NewEngine/actions/workflows/ReleaseBuild.yml)
# 現在制作しているゲーム
## 『モノクロファイター』
- ジャンル：格闘
- 制作開始：2024年 1月～ (現在9ヶ月)
- 制作時間：約370時間
- 制作人数：1人
- 開発環境：DirectX12、Windows SDK(10.0.22621.3233)
- 外部ライブラリ：assimp、DirectXTex、imgui、nlohmann/json
  
![スクリーンショット 2024-10-30 030752](https://github.com/user-attachments/assets/6834cdb5-de2f-4e68-ba17-78f5120e59b8)

## 特にこだわっているところ
### 気持ちのいいコンボ
**プレイしていても見ていても気持ちのいいコンボ**になるように技と技をスムーズにつなぐことを意識して制作しています。  
また、既存のゲームを参考にボタンを連打することでコンボがつながるような工夫もしています。

![combo](https://github.com/user-attachments/assets/f30c9cb4-7555-4315-a8d0-ce8c165002a9)

### 攻撃やコンボの調整をしやすくするための工夫
攻撃やコンボの調整をしやすくするためにアタックエディターや当たり判定の可視化なども実装し、  
より調整をしやすいように工夫しています。

## アタックエディターについて
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
  
![スクリーンショット 2024-10-30 211155](https://github.com/user-attachments/assets/a82934e8-2487-459a-a19a-b1f45971bedc)

## 今後の展望
- 攻撃時やコンボ時に背景を変化させる(背景オブジェクトが壊れるなど)
- 入力履歴の表示
- エディターの拡張(当たり判定なども設定できるように)
- ゲーム中のカメラ移動の追加
