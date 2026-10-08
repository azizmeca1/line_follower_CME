# Robot Suiveur de Ligne ESP32 (PID) - CME

Bienvenue dans la documentation complète du projet **line_follower_CME**. Ce guide permet à n'importe quel débutant, **sous Windows**, de câbler, tester, calibrer et régler un robot suiveur de ligne basé sur un microcontrôleur **ESP32**, un driver moteur **TB6612FNG** et une barre de 8 capteurs infrarouges **Pololu QTR-8A**.

---

## Table des Matières
1. [Aperçu du Système](#aperçu-du-système)
2. [Liste du Matériel (BOM)](#liste-du-matériel-bom)
3. [Schéma de Câblage Complet](#schéma-de-câblage-complet)
4. [Avertissements Matériels Critiques](#avertissements-matériels-critiques)
5. [Installation de l'Environnement de Développement (Windows)](#installation-de-lenvironnement-de-développement-windows)
6. [Structure du Dépôt](#structure-du-dépôt)
7. [Procédure de Test Étape par Étape](#procédure-de-test-étape-par-étape-validation-matérielle)
8. [Programme Principal (src/main.cpp)](#programme-principal-srcmaincpp)
9. [Guide de Démarrage et Calibration](#guide-de-démarrage-et-calibration)
10. [Réglage du PID](#réglage-du-pid)
11. [Guide de Dépannage (FAQ)](#guide-de-dépannage-faq)

---

## Aperçu du Système

Le robot lit en temps réel la réflectivité du sol grâce à 8 capteurs infrarouges analogiques. L'ESP32 calcule la position de la ligne noire (valeur normalisée de `0` à `7000`, centre idéal à `3500`). Un régulateur **PID** ajuste en permanence la vitesse PWM envoyée à chaque moteur via le TB6612FNG.

### Principales Fonctionnalités
- **Calibration automatique** : ajustement des seuils Blanc/Noir au démarrage.
- **Sécurité USB déconnecté** : coupure immédiate des moteurs si la liaison USB est rompue (`Ctrl+C` dans le terminal).
- **Arrêt d'urgence clavier** : interruption instantanée en appuyant sur `s` ou `Espace`.

---

## Liste du Matériel (BOM)

| Composant | Quantité | Description |
| :--- | :---: | :--- |
| **ESP32 DevKit V1** | 1 | Microcontrôleur principal 32 bits (3.3V) |
| **Pololu QTR-8A** | 1 | Barre de 8 capteurs IR analogiques |
| **TB6612FNG** | 1 | Driver moteur double pont en H |
| **Moteurs CC** | 2 | Micro-moteurs à réducteur (ex : N20 6V 500-1000 RPM) |
| **Source de puissance (VM)** | 1 | Batterie LiPo 2S (7.4V) ou support 4x piles AA / 18650 |
| **Câble USB (données)** | 1 | Câble USB compatible transfert de données (pas seulement charge) |
| **Châssis & câblage** | - | Câbles Dupont (F-F / M-F), châssis 2 roues motrices + roue folle |

---

## Schéma de Câblage Complet

### 1. Barrette de capteurs QTR-8A vers ESP32

| Broche QTR-8A | Broche ESP32 | Fonction |
| :--- | :--- | :--- |
| **VCC** | **3V3** | Alimentation logique 3.3V |
| **GND** | **GND** | Masse commune |
| **Capteur 1 (C1)** | **GPIO 32** | Entrée analogique |
| **Capteur 2 (C2)** | **GPIO 33** | Entrée analogique |
| **Capteur 3 (C3)** | **GPIO 25** | Entrée analogique |
| **Capteur 4 (C4)** | **GPIO 26** | Entrée analogique |
| **Capteur 5 (C5)** | **GPIO 27** | Entrée analogique |
| **Capteur 6 (C6)** | **GPIO 14** | Entrée analogique |
| **Capteur 7 (C7)** | **GPIO 34** | Entrée analogique (Input Only) |
| **Capteur 8 (C8)** | **GPIO 13** | Entrée analogique |

### 2. Driver TB6612FNG vers ESP32 et Batterie

| Broche TB6612FNG | Connexion | Description |
| :--- | :--- | :--- |
| **VCC** | **3V3** (ESP32) | Alimentation logique du driver |
| **GND** | **GND** (ESP32) + **GND Batterie** | **Masse commune (obligatoire)** |
| **VM** | **(+) Batterie externe** (5V-12V) | Puissance moteurs (indépendante du 3V3) |
| **STBY** | **GPIO 23** | Active le driver (HIGH = ON) |
| **PWMA** | **GPIO 18** | Vitesse PWM moteur gauche |
| **AIN1** | **GPIO 19** | Direction 1 moteur gauche |
| **AIN2** | **GPIO 21** | Direction 2 moteur gauche |
| **PWMB** | **GPIO 17** | Vitesse PWM moteur droit |
| **BIN1** | **GPIO 16** | Direction 1 moteur droit |
| **BIN2** | **GPIO 4** | Direction 2 moteur droit |
| **AO1 / AO2** | **Moteur gauche** | Sorties vers le moteur A |
| **BO1 / BO2** | **Moteur droit** | Sorties vers le moteur B |

---

## Avertissements Matériels Critiques

1. **GPIO 12 strictement interdit pour les capteurs** : c'est une broche de boot (*strapping pin*). Si un capteur y est relié, l'ESP32 échouera au démarrage ou au flashage (`A fatal error occurred: Packet content transfer stopped` / `Error 2`). C'est pourquoi le **GPIO 34** est utilisé.
2. **Masse commune (GND) obligatoire** : reliez ensemble le GND de l'ESP32, le GND du TB6612FNG et le pôle négatif (-) de la batterie. Sans référence commune, les signaux PWM ne fonctionnent pas.
3. **Séparation de la puissance (VM vs VCC)** : ne branchez jamais **VM** sur le 3.3V de l'ESP32. VM doit recevoir directement le courant de la batterie.

---

## Installation de l'Environnement de Développement (Windows)

### Méthode A : VS Code + PlatformIO (recommandé)

#### 1. Installer les outils
1. Installez [Visual Studio Code](https://code.visualstudio.com/).
2. Dans VS Code, ouvrez l'onglet **Extensions** (`Ctrl+Shift+X`), cherchez **PlatformIO IDE** et cliquez sur **Install**.
3. Attendez la fin de l'installation, puis redémarrez VS Code (la première initialisation peut prendre quelques minutes).

#### 2. Installer le driver USB de la carte
Selon la puce USB-UART de votre ESP32 DevKit, installez le driver correspondant :
- **CP210x** (Silicon Labs) ou **CH340** (WCH).
- Branchez la carte, puis ouvrez le **Gestionnaire de périphériques** (`Win+X`, puis Gestionnaire de périphériques) et repérez le port dans **Ports (COM et LPT)**, par exemple `COM3`.

> Si la carte n'apparaît pas, essayez un autre câble USB : beaucoup de câbles ne transmettent que l'alimentation.

#### 3. Configuration du projet
Ouvrez le dossier du projet dans VS Code (`Fichier > Ouvrir un dossier`). Le fichier `platformio.ini` à la racine doit contenir :

```ini
[env:esp32dev]
platform = espressif32
board = esp32dev
framework = arduino
monitor_speed = 115200
lib_deps =
    pololu/QTRSensors@^4.0.0
```

Si PlatformIO ne détecte pas le bon port automatiquement, ajoutez (en remplaçant `COM3` par votre port) :

```ini
upload_port = COM3
monitor_port = COM3
```

#### 4. Compiler, téléverser et ouvrir le moniteur série

**Avec l'interface (le plus simple)** : utilisez la barre bleue en bas de VS Code.

| Icône | Action |
| :--- | :--- |
| Coche | Compiler (Build) |
| Flèche vers la droite | Téléverser (Upload) |
| Prise / icône de terminal | Ouvrir le moniteur série (Serial Monitor) |

**En ligne de commande** : ouvrez le terminal PlatformIO (`PlatformIO: New Terminal` dans la palette de commandes `Ctrl+Shift+P`), puis :

```powershell
pio run
pio run --target upload
pio device monitor
```

Pour téléverser et ouvrir directement le moniteur :

```powershell
pio run --target upload --target monitor
```

> Si la commande `pio` n'est pas reconnue dans un terminal PowerShell classique, utilisez le chemin complet :
> ```powershell
> & "$env:USERPROFILE\.platformio\penv\Scripts\pio.exe" run
> ```

### Méthode B : Arduino IDE

1. Téléchargez et installez [Arduino IDE 2.x](https://www.arduino.cc/en/software).
2. Ajoutez l'ESP32 : `Fichier > Préférences`, puis dans **URL de gestionnaire de cartes supplémentaires** collez :
   `https://raw.githubusercontent.com/espressif/arduino-esp32/gh-pages/package_esp32_index.json`
3. Installez les cartes : `Outils > Type de carte > Gestionnaire de cartes`, cherchez **esp32** (Espressif Systems) et cliquez sur **Installer**.
4. Installez la bibliothèque : `Outils > Gérer les bibliothèques`, cherchez **QTRSensors** (Pololu) et cliquez sur **Installer**.
5. Téléversement :
   - Sélectionnez `ESP32 Dev Module` dans `Outils > Type de carte`.
   - Sélectionnez le port série (par exemple `COM3`) dans `Outils > Port`.
   - Cliquez sur **Téléverser** (flèche vers la droite).
6. Moniteur série : `Outils > Moniteur série`, vitesse **115200 bauds**.

---

## Structure du Dépôt

```
suiveur/esp32_qtr_projet/
|-- include/         Fichiers d'en-tête (.h)
|-- lib/             Bibliothèques locales
|-- src/             Code source (programme principal et sketch de test QTR)
|-- test/            Tests
`-- platformio.ini   Configuration PlatformIO
```

> Avec **PlatformIO**, un seul programme est compilé à la fois depuis `src/`. Pour lancer un sketch de test (ex : `test_QTR.ino`), placez-le temporairement dans `src/` à la place du programme principal, ou utilisez l'Arduino IDE. Pensez à restaurer `main.cpp` ensuite.

---

## Procédure de Test Étape par Étape (Validation Matérielle)

Avant de lancer le programme complet, **validez chaque composant séparément**. Cela évite de chercher une panne mécanique dans un code PID qui n'y est pour rien.

### Pré-requis communs
- Robot **posé sur un support** (roues en l'air) pour les tests moteurs.
- Câble USB branché, moniteur série ouvert à **115200 bauds**.
- Masse commune ESP32 / TB6612FNG / batterie vérifiée.

### Étape 1 : Test brut des capteurs QTR-8A

**Objectif** : vérifier que les 8 capteurs répondent et que le câblage GPIO est correct.

1. Téléversez le sketch de test des capteurs (`test_QTR.ino`).
2. Ouvrez le moniteur série.
3. Placez la barre à environ 3 à 8 mm du sol, d'abord au-dessus d'une zone **blanche**, puis au-dessus de la **ligne noire**.
4. Passez un doigt ou un morceau de papier sombre devant chaque capteur, un par un.

**Résultat attendu**

| Situation | Comportement attendu |
| :--- | :--- |
| Surface blanche | Valeurs **basses** (forte réflexion) |
| Ligne noire | Valeurs **hautes** (faible réflexion) |
| Capteur masqué un par un | Une seule colonne varie à la fois, dans l'ordre C1 à C8 |

**Si ça ne marche pas** : valeur figée sur un capteur, vérifiez son fil et son GPIO ; colonnes inversées, la barre est montée dans le mauvais sens ; valeurs aberrantes partout, vérifiez le 3V3 et le GND.

### Étape 2 : Test direct des moteurs TB6612FNG

**Objectif** : valider le sens de rotation et le câblage du driver, **sans** capteurs.

1. Robot **sur support**, batterie branchée sur **VM**.
2. Téléversez le sketch de test des moteurs.
3. Observez chaque moteur : marche avant, marche arrière, arrêt.

**Résultat attendu**
- Le moteur **gauche** puis le moteur **droit** tournent chacun dans les deux sens.
- En marche avant, **les deux roues avancent** dans le sens du robot.

**Si ça ne marche pas**

| Symptôme | Cause probable |
| :--- | :--- |
| Aucun moteur ne tourne | `STBY` non à HIGH, batterie non branchée sur VM, masse non commune |
| Un moteur tourne à l'envers | Inversez ses deux fils (AO1/AO2 ou BO1/BO2) |
| Gauche et droite inversés | Échangez les moteurs sur les sorties A et B |
| Un seul sens de rotation | Vérifiez AIN1/AIN2 (ou BIN1/BIN2) |

### Étape 3 : Test de calibration QTRSensors

**Objectif** : vérifier que la calibration et la position calculée (0 à 7000) fonctionnent.

1. Téléversez le sketch de test de calibration.
2. Pendant la phase de calibration, **balayez lentement la barre** de gauche à droite au-dessus de la ligne pour que chaque capteur voie à la fois le blanc et le noir.
3. Une fois terminée, déplacez la barre sur la ligne à la main et observez la position.

**Résultat attendu**

| Position de la ligne | Valeur affichée |
| :--- | :--- |
| Sous le capteur 1 (extrême gauche) | proche de `0` |
| Au centre de la barre | proche de `3500` |
| Sous le capteur 8 (extrême droite) | proche de `7000` |

Si les valeurs sont inversées (gauche = 7000), la barre est montée à l'envers ou l'ordre des GPIO est inversé.

---

## Programme Principal (src/main.cpp)

Le programme principal enchaîne : **initialisation, calibration, attente de départ, suivi de ligne PID**. Consultez le fichier `src/main.cpp` pour le code. Les paramètres à connaître sont :

| Paramètre | Rôle |
| :--- | :--- |
| `Kp`, `Ki`, `Kd` | Gains du régulateur PID |
| Vitesse de base | PWM moyenne des deux moteurs en ligne droite |
| Vitesse maximale | Limite haute du PWM envoyé à chaque moteur |
| Consigne (setpoint) | `3500` (ligne au centre de la barre) |

> Les noms exacts des variables peuvent différer dans votre version du code : adaptez ce tableau à `main.cpp`.

---

## Guide de Démarrage et Calibration

### 1. Préparer le circuit
- Piste : ligne **noire** (ruban isolant ou marqueur large) sur fond **blanc mat**.
- Largeur de ligne recommandée : environ **1,5 à 2 cm**.
- Évitez les surfaces brillantes et la lumière directe du soleil, qui perturbent les capteurs IR.
- Courbes arrondies, pas d'angle trop fermé pour les premiers essais.

### 2. Lancer le robot
1. Branchez le câble USB et téléversez le programme principal.
2. Ouvrez le moniteur série (115200 bauds).
3. Posez le robot **sur la ligne**, barre au-dessus du centre.
4. Branchez/activez la batterie (VM).
5. **Phase de calibration** : suivez les instructions affichées. Balayez le robot de gauche à droite au-dessus de la ligne pour que tous les capteurs voient blanc **et** noir.
6. Replacez le robot au centre de la ligne et lancez le départ comme indiqué dans le moniteur série.

### 3. Arrêter le robot

| Action | Effet |
| :--- | :--- |
| Touche `s` ou `Espace` dans le moniteur | Arrêt d'urgence immédiat |
| `Ctrl+C` / déconnexion USB | Les moteurs sont coupés automatiquement |
| Coupure de la batterie | Coupure totale de la puissance |

> **Refaites la calibration** à chaque changement de piste, d'éclairage ou de hauteur de barre.

---

## Réglage du PID

Réglez **un paramètre à la fois**, en commençant avec une vitesse de base **faible**.

### Méthode recommandée

1. **Partir de `Ki = 0` et `Kd = 0`.**
2. **Régler `Kp`** : augmentez-le progressivement jusqu'à ce que le robot suive la ligne en oscillant légèrement de chaque côté.
   - Trop faible : le robot sort des courbes.
   - Trop fort : oscillations violentes et rapides.
3. **Ajouter `Kd`** : augmentez-le pour amortir les oscillations jusqu'à obtenir une trajectoire fluide.
4. **Ajouter `Ki` en dernier**, avec une valeur très petite, uniquement si le robot a un biais constant d'un côté. Souvent, `Ki = 0` suffit.
5. **Augmenter la vitesse de base** par petits paliers, puis retoucher `Kp` et `Kd`.

### Tableau de diagnostic

| Comportement observé | Action |
| :--- | :--- |
| Sort de la ligne dans les virages | Augmenter `Kp` ou baisser la vitesse |
| Oscillations rapides et amples | Baisser `Kp` ou augmenter `Kd` |
| Oscillations lentes qui s'amplifient | Augmenter `Kd` |
| Dérive constante d'un côté | Vérifier la mécanique, puis ajouter un peu de `Ki` |
| Robot nerveux/vibrant en ligne droite | Baisser `Kd` |
| Perd la ligne après un croisement | Baisser la vitesse, vérifier la calibration |

---

## Guide de Dépannage (FAQ)

### Flashage et connexion

**Erreur `A fatal error occurred: Packet content transfer stopped` / `Error 2`**
- Vérifiez qu'aucun capteur n'est relié au **GPIO 12**.
- Débranchez les fils sur les broches de boot pendant le flashage.
- Essayez un autre câble USB (certains ne transmettent que l'alimentation).
- Maintenez le bouton **BOOT** de la carte au début du téléversement si nécessaire.

**Aucun port COM n'apparaît dans le Gestionnaire de périphériques**
- Changez de câble USB (câble données) et de port USB.
- Installez le driver **CP210x** ou **CH340** selon votre carte, puis rebranchez.
- Un périphérique inconnu avec un point d'exclamation jaune indique un driver manquant.

**Erreur `could not open port 'COM3'` / `Access is denied`**
- Le port est déjà utilisé : fermez le moniteur série (VS Code, Arduino IDE, PuTTY...) avant de téléverser.
- Vérifiez que le numéro de port dans `platformio.ini` (`upload_port`) correspond bien à celui du Gestionnaire de périphériques.

**Le moniteur série affiche des caractères illisibles**
- Réglez la vitesse à **115200 bauds**, identique à `monitor_speed`.

**La commande `pio` n'est pas reconnue**
- Ouvrez le terminal via `PlatformIO: New Terminal`, ou utilisez le chemin complet `%USERPROFILE%\.platformio\penv\Scripts\pio.exe`.

### Capteurs

**Un capteur reste figé sur une valeur**
- Vérifiez son fil, sa broche GPIO et la soudure sur la barre.

**Le GPIO 34 ne lit rien**
- C'est une broche *input only* : c'est normal pour un capteur, mais elle ne peut pas servir de sortie.

**Valeurs instables ou bruitées**
- Vérifiez l'alimentation 3V3 et la masse commune.
- Éloignez les fils de capteurs des fils moteurs.
- Réduisez la lumière ambiante intense.

**Le robot réagit à l'envers (tourne du mauvais côté)**
- La barre est inversée ou l'ordre des GPIO est inversé : refaites l'Étape 3.

### Moteurs

**Les moteurs ne tournent pas**
- Vérifiez : `STBY` à HIGH, batterie sur **VM**, **masse commune**, câblage PWMA/PWMB.

**Le robot tourne sur lui-même**
- Un moteur est inversé : inversez ses fils ou le sens dans le code.

**L'ESP32 redémarre quand les moteurs démarrent**
- Chute de tension : batterie trop faible ou fils trop fins. Ajoutez un condensateur sur VM et utilisez une batterie chargée.

**Le robot est trop lent ou manque de puissance**
- Vérifiez la tension de batterie et augmentez la vitesse de base progressivement.

### Suivi de ligne

**Le robot perd la ligne dans les virages**
- Baissez la vitesse, augmentez `Kp`/`Kd`, ou refaites la calibration.

**Le robot zigzague en ligne droite**
- `Kp` trop élevé ou `Kd` insuffisant : voir le tableau PID.

**Ça marche sur USB mais pas sur batterie**
- Vérifiez la tension de VM et l'état de charge de la batterie.

---

## Contributions

Les contributions (issues, pull requests) sont les bienvenues. Pensez à décrire votre configuration matérielle (carte, moteurs, batterie) et votre version de Windows lorsque vous signalez un problème.
