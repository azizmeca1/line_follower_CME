# 🤖 Robot Suiveur de Ligne ESP32 avec Algorithme PID

Bienvenue dans le projet **line_follower_CME** ! Ce guide est conçu pour vous accompagner pas à pas dans la fabrication, le câblage, la configuration et la programmation d'un robot suiveur de ligne haute performance basé sur un microcontrôleur **ESP32**, une barre de 8 capteurs **QTR-8A** et un driver moteur **TB6612FNG**.

---

## 📋 Table des Matières
1. [Présentation du Projet](#-présentation-du-projet)
2. [Matériel Requis](#-matériel-requis)
3. [Schéma de Câblage](#-schéma-de-câblage)
4. [Configuration de l'Environnement](#-configuration-de-lenvironnement)
   - [Méthode A : VS Code + PlatformIO (Recommandé)](#méthode-a--vs-code--platformio-recommandé)
   - [Méthode B : Arduino IDE](#méthode-b--arduino-ide)
5. [Installation des Bibliothèques](#-installation-des-bibliothèques)
6. [Structure du Code & Fonctionnalités](#-structure-du-code--fonctionnalités)
7. [Guide de Démarrage & Réglages PID](#-guide-de-démarrage--réglages-pid)
8. [Résolution des Problèmes Courants](#-résolution-des-problèmes-courants)

---

## 🛠️ Matériel Requis

| Composant | Quantité | Description |
| :--- | :---: | :--- |
| **ESP32 DevKit V1** | 1 | Microcontrôleur principal (32 bits, Dual Core) |
| **Pololu QTR-8A** | 1 | Barre de 8 capteurs d'affichage infrarouge analogiques |
| **TB6612FNG** | 1 | Driver moteur double pont en H (Rendement supérieur au L298N) |
| **Moteurs CC à réducteur** | 2 | Moteurs de type "N20" ou motoréducteurs jaunes (3V-9V) |
| **Batterie Externe** | 1 | LiPo 2S (7.4V) ou support 4x piles AA (pour l'alimentation moteur) |
| **Câbles Dupont / Châssis** | - | Câbles Mâle-Femelle, Mâle-Mâle et châssis 2 roues |

---

## 🔌 Schéma de Câblage

### 1. Capteur QTR-8A (Analogique) $\rightarrow$ ESP32
> **⚠️ ATTENTION :** Ne connectez **JAMAIS** de capteur sur la broche **GPIO 12**. Le GPIO 12 empêche l'ESP32 de démarrer (*boot fail*).

| Capteur QTR-8A | Broche ESP32 | Remarque |
| :--- | :--- | :--- |
| **VCC** | **3V3** | Alimentation logique 3.3V |
| **GND** | **GND** | Masse commune |
| **Capteur 1 à 8** | **32, 33, 25, 26, 27, 14, 34, 13** | Entrées analogiques ADC |

### 2. Driver Moteur TB6612FNG $\rightarrow$ ESP32 & Batterie

| Broche TB6612 | Connexion | Rôle |
| :--- | :--- | :--- |
| **VCC** | **3V3** (ESP32) | Logique de commande (3.3V) |
| **GND** | **GND** (ESP32) + **Masse Batterie** | **Masse commune obligatoirement reliée !** |
| **VM** | **Pôle (+) de la Batterie** | Alimentation de puissance (5V à 12V) |
| **STBY** | **GPIO 23** | Activer/Désactiver le driver |
| **PWMA** | **GPIO 18** | Vitesse Moteur Gauche (PWM) |
| **AIN1 / AIN2**| **GPIO 19 / GPIO 21** | Sens de rotation Moteur Gauche |
| **PWMB** | **GPIO 17** | Vitesse Moteur Droit (PWM) |
| **BIN1 / BIN2**| **GPIO 16 / GPIO 4** | Sens de rotation Moteur Droit |
| **AO1 / AO2** | **Moteur Gauche** | Bornes du moteur A |
| **BO1 / BO2** | **Moteur Droit** | Bornes du moteur B |

---

## 💻 Configuration de l'Environnement

### Méthode A : VS Code + PlatformIO (Recommandé)

1. Téléchargez et installez **[VS Code](https://code.visualstudio.com/)**.
2. Ouvrez VS Code, allez dans les extensions (`Ctrl+Shift+X`) et cherchez **PlatformIO IDE**, puis cliquez sur **Install**.
3. Ouvrez le dossier du projet dans VS Code (`Fichier > Ouvrir le
