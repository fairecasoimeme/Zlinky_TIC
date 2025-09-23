
# ZLinky_TIC
 Téléinformation Linky autoalimenté communiquant en ZigBee 3.0  
 vous pouvez vous procurer le ZLinky_TIC à l'adresse suivante :  
 [Boutique LiXee](https://lixee.fr/)
 
# Sommaire  

* [Description](#description)  
* [PCB](#pcb)  
* [Installation](#installation)  
* [Voyant lumineux](#voyant-lumineux)  
* [Route or limited route (From V7)](#route-or-limited-route-from-v7)
* [Clusters](#clusters)
   * [Clusters List](#clusters-list)
   * [Basic Cluster (0x0000)](#basic-cluster-0x0000)
   * [PowerConfiguration Cluster (0x0001) --> (From v12)](#powerconfiguration-cluster-0x0001----from-v12)
* [Linky Mode ( From V4)](#linky-mode--from-v4)
* [Linky acquisition time (From V7)](#linky-acquisition-time-from-v7)
* [Subscription (OPTARIF Values)](#subscription-optarif-values)
   * [Mode historique](#mode-historique)
   * [Mode standard](#mode-standard)
* [Synthèse développeur](#synth%C3%A8se-d%C3%A9veloppeur)
* [Mise à jour](#mise-%C3%A0-jour) 
    * [OTA](#ota)
    * [Non OTA](#non-ota)
* [Errata (hardware)](#errata-hardware)
* [Intégrations](#intégrations)
   * [Jeedom](#jeedom)
   * [Home-Assistant](#home-assistant)
   * [Eedomus](#eedomus)
   * [Domoticz](#domoticz)
   * [Homey Pro](#homey-pro)
   * [SmartThing](#smartthing)
   * [Tuya](#tuya-from-v14)
* [FAQ](#faq)
* [ChangeLog](#changelog)
 

## Description

### Version 1.x
![Description ZLinky_TIC](https://github.com/fairecasoimeme/Zlinky_TIC/blob/master/Doc/Images/ZLinky_description.JPG)
<img src="https://github.com/fairecasoimeme/Zlinky_TIC/blob/master/Doc/Images/ZLinky_prototype.jpg" width="400">

### Version 2.x
![Description ZLinky_TIC_v2](https://github.com/fairecasoimeme/Zlinky_TIC/blob/master/Doc/Images/Description_ZLinky_v2.png)
<img src="https://github.com/fairecasoimeme/Zlinky_TIC/blob/master/Doc/Images/ZLinkyv2_onsite.jpg" width="400">

#### Améliorations hardware
* Cette version hardware + la version firmware v14 a été labellisée <img src="https://github.com/fairecasoimeme/Zlinky_TIC/blob/master/Doc/Images/1_Linky_Ready_Logo_Noir.jpg" width="50">  par le laboratoire Enedis.
* Déport T.I.C permettant de chaîner différents appareils compatibles avec la téléinformation (I1 et I2)
* Boitier ABS personnalisé plus adapté au Linky
  


## PCB

### Version 1.0
<img src="https://github.com/fairecasoimeme/Zlinky_TIC/blob/master/Doc/Images/ZLinky_PCB_face.jpg" width="800">
<img src="https://github.com/fairecasoimeme/Zlinky_TIC/blob/master/Doc/Images/ZLinky_PCB_pile.jpg" width="800">  

### Version 1.2  
<img src="https://github.com/fairecasoimeme/Zlinky_TIC/blob/master/Doc/Images/PCB_ZLinky_v1.2_face.jpg" width="800">  
<img src="https://github.com/fairecasoimeme/Zlinky_TIC/blob/master/Doc/Images/PCB_ZLinky_v1.2_pile.jpg" width="800">  

**ZLinky_TIC** est un appareil permettant de transmettre toutes les informations du Linky en ZigBee 3.0.  
**ZLinky_TIC** est alimenté par votre compteur Linky. Il suffit simplement d'enlever le cash (jaune fluo) et de "plugger" l'appareil en pressant sur le bouton orange.  
Au départ, **ZLinky_TIC** est en attente d'appairage mais si vous souhaitez réinitialiser l'appareil, il suffit de rester appuyer sur le bouton "link" pendant 10 secondes. Ensuite, le bouton relâché, la Led s'éteint puis se met à clignoter.  
Une fois appairé à votre box domotique par l'intermédiaire d'une ZiGate ou d'un autre coordinateur ZigBee, vous pourrez gérer votre consommation d'électricité.

**Actuellement, l'appareil fonctionne avec le mode Historique et standard du compteur Linky.**  
**Il permet de gérer tous les abonnements en mono ou triphasé et le mode production (uniquement en mode standard)** 

⚠️ **le mode historique ne fonctionne qu'avec les abonnements classique (BASE, heures pleines/creuses, bleu blanc rouge). Pour tous les abonnements particuliers (ex heures super creuses), il est obligatoire de passer le Linky en mode standard.**  

## Installation
Suivre les instructions suivantes :
* Retirer le capot vert du Linky
* Insérer le *ZLinky_TIC* en appuyant sur le bouton au-dessus

<img src="https://github.com/fairecasoimeme/Zlinky_TIC/blob/master/Doc/Images/Installation_ZLinky_v2.png" width="600">  

PS: L'image et l'emplacement des bornes I1 I2 et A peuvent varier en fonction de la marque du Linky

## Voyant lumineux

Le voyant lumineux permet de connaître l'état de l'appareil.

L'appareil clignote rapidement plusieurs fois. ZLinky_TIC est en mode attente d'appairage. (⚠️ Uniquement à partir de la V7)  
<img src="https://github.com/fairecasoimeme/Zlinky_TIC/blob/master/Doc/Images/ZLinky_Blink_Permit_join.gif" width="100"><img src="https://github.com/fairecasoimeme/Zlinky_TIC/blob/master/Doc/Images/ZLinky_v2_pair.gif" width="150">    

L'appareil clignote plusieurs fois lentement. Zlinky_TIC rencontre des erreurs lors de l'acquisition des données du Linky.  
<img src="https://github.com/fairecasoimeme/Zlinky_TIC/blob/master/Doc/Images/ZLinky_Blink_NOK.gif" width="100"><img src="https://github.com/fairecasoimeme/Zlinky_TIC/blob/master/Doc/Images/ZLinky_v2_blink_off.gif" width="150">    

La LED de l'appareil clignote une fois toutes les 3 secondes : Le ZLinky_TIC fonctionne correctement. L'appareil décode correctement le Linky  
<img src="https://github.com/fairecasoimeme/Zlinky_TIC/blob/master/Doc/Images/ZLinky_Blink_OK.gif" width="100"><img src="https://github.com/fairecasoimeme/Zlinky_TIC/blob/master/Doc/Images/ZLinky_v2_Blink_ok.gif" width="150">   

## Route or limited route (From V7)

À partir de la version 7 du firmware, il existe 2 versions de fichier de mise à jour (OTA)  
  
Par défaut, le ZLinky_TIC est en mode routeur (legacy). Il permet à des capteurs Zigbee tierces (max 5) de se connecter au Zlinky_TIC et de faire le lien vers un autre routeur ou coordinateur. Ce principe permet d'augmenter, fiabiliser le maillage ou d'élargir le réseau Zigbee.  
  
Cependant, comme le Zlinky_TIC est alimenté par le Linky avec très peu d'énergie (dépendant aussi des marques de Linky) et que le routage intensif consomme de l'énergie (émission / réception des paquets permanents), une version "Limited" a été développé afin de limiter le trafic des autres appareils sur le Zlinky_TIC.  
  
La version "Limited" est destiné aux utilisateurs qui ont des réseaux énormément maillés et qui entrainent un trafic trop important pour que le Zlinky_TIC soit stable énergétiquement.  

⚠️ **le firmware "Limited" ne concerne uniquement que le routage des enfants, pas son propre routage avec le coordinateur !**


## Clusters

### Clusters List

|Num (Hexa)|Name|I/O| Comment|
|----------|----|---|--------|  
|0x0000|Basic|I||
|0x0001|PowerConfiguration|I|(From v12)|
|0x0003|Identify|I||
|0x0702|Simple Metering|I||
|0x0B01|Meter Identification|I||
|0x0B04|Electric measurement|I||
|0xFF66|LiXee Private|I||
|0x0019|OTA|O|Not used yet|

### Basic Cluster (0x0000)

|attribut|Name|Value|
|------|----------|------|
|0x0000|ZCLVersion|0x0003|  
0x0001 | ApplicationVersion|0x0001|  
0x0002 | StackVersion|0x0002|
0x0003 | HWVersion|0x0001|
0x0004 | ManufacturerName|LiXee|
0x0005 | ModelIdentifier|ZLinky_TIC|
0x0006 | DateCode|20210401|
0x0007 | PowerSource|0x03|
0x4000 | SWBuildID| < V9: 4000-XXXX <br> > V9: 4000-XXXX (Legacy firmware) - 4001-XXXX (LIMITED firmware)|

### PowerConfiguration Cluster (0x0001) --> (From v12)
|attribut|Name|Right|Value|
|------|----------|------|------|
|0x0000| MainsVoltage|RO|| 
|0x0011 | MainsVoltageMinThreshold|RW|2600|  
|0x0012 | MainsVoltageMaxThreshold|RW|3600|

## Linky Mode ( From V4)

### values
|Cluster|Attribut|Value|
|-------|--------|------|
|0xFF66|0x0300| 0 - Mode historique monophasé<br> 1 - Mode standard monophasé <br> 2 - Mode historique triphasé<br> 3 - Mode standard triphasé<br> 5 - Mode standard monophasé producteur<br> 7 - Mode standard triphasé producteur|

### bit values
bit 0 - Mode historique / standard  
bit 1 - Triphasé / Monophasé   
bit 2 - Production / non production  

## Linky acquisition time (From V7)
Now you can modify the time to do a Linky acquisition. 
|Cluster|Attribut|Value|Type|Min|Max|
|-------|--------|------|-----|----|----|
|0xFF66|0x0100| 10 (by default)|Uint8 (0x20)|0x07|0x3C|


## Subscription (OPTARIF Values)

### Mode historique  
🟥 BASE  
🟩 HC..  
🟧 EJP.  
🟦 BBRx  


RO= READ only RP= Reportable / RW = Read/Write	

|Commande TIC|CLUSTER|Attribut|Droit|Comment|data type|size max|unit|designation|valeur par defaut|
|------------|-------|--------|-----|-------|---------|--------|----|-----------|-----------------|				
|ADC0|0x0702|0x0308|RO||String (0x41)|12 car|-|	Serial Number|NULL|
|🟥 BASE|0x0702|0x0000|RP||Uint48 (0x25)|9 car|Wh| Index Base|0|
|OPTARIF			|0xFF66|	0x0000|		RO||String (0x42)|4 car|-| Option tarifaire|BASE|	
|ISOUSC			|0x0B01	|0x000D		|RO		||Uint16 (0x21)|2 car|A| Intensité souscrite|0|
|🟩 HCHC			|0x0702	|0x0100		|RP			||Uint48 (0x25)|9 car|Wh|Index HCHC|0|
|🟩 HCHP			|0x0702	|0x0102		|RP			||Uint48 (0x25)|9 car|Wh|Index HCHP|0|
|🟧 EJPHN			|0x0702	|0x0100		|RP			||Uint48 (0x25)|9 car|Wh|Index EJPHN|0|
|🟧 EJPHPM			|0x0702	|0x0102		|RP			||Uint48 (0x25)|9 car|Wh|Index EJPHPM|0|
|🟦 BBRHCJB			|0x0702	|0x0100		|RP			||Uint48 (0x25)|9 car|Wh|Index BBRHCJB|0|
|🟦 BBRHPJB			|0x0702	|0x0102		|RP			||Uint48 (0x25)|9 car|Wh|Index BBRHPJB|0|
|🟦 BBRHCJW			|0x0702	|0x0104		|RP			||Uint48 (0x25)|9 car|Wh|Index BBRHCJW|0|
|🟦 BBRHPJW			|0x0702	|0x0106		|RP			||Uint48 (0x25)|9 car|Wh|Index BBRHPJW|0|
|🟦 BBRHCJR			|0x0702	|0x0108		|RP			||Uint48 (0x25)|9 car|Wh|Index BBRHCJR|0|
|🟦 BBRHPJR			|0x0702	|0x010A		|RP			||Uint48 (0x25)|9 car|Wh|Index BBRHPJR|0|
|IINST			|0x0B04	|0x0508		|RP			||Uint16 (0x21)|3 car|A|Courant efficace|0xFFFF|
|IINST1			|0x0B04	|0x0508		|RP			|Triphasé|Uint16 (0x21)|3 car|A|Courant efficace phase 1|0xFFFF|
|IINST2			|0x0B04	|0x0908		|RP			|Triphasé|Uint16 (0x21)|3 car|A|Courant efficace phase 2|0xFFFF|
|IINST3			|0x0B04	|0x0A08		|RP			|Triphasé|Uint16 (0x21)|3 car|A|Courant efficace phase 3|0xFFFF|
|IMAX			|0x0B04	|0x050A		|RO			||Uint16 (0x21)|3 car|A|Intensité maximale|0xFFFF|
|IMAX1			|0x0B04	|0x050A		|RO			|Triphasé|Uint16 (0x21)|3 car|A|Intensité maximale phase 1|0xFFFF|
|IMAX2			|0x0B04	|0x090A		|RO			|Triphasé|Uint16 (0x21)|3 car|A|Intensité maximale phase 2|0xFFFF|
|IMAX3			|0x0B04	|0x0A0A		|RO			|Triphasé|Uint16 (0x21)|3 car|A|Intensité maximale phase 3|0xFFFF|
|PMAX			|0x0B04	|0x050D		|RO			|Triphasé|Uint16 (0x21)|5 car|W|Puissance maximale triphasée atteinte|0x8000|
|PAPP			|0x0B04	|0x050F		|RP			|Triphasé|Uint16 (0x21)|5 car|VA|Puissance apparente|0xFFFF|
|PTEC			|0x0702	|0x0020		|RO	||String (0x42)|4 car|-|Periode tarifaire en cours|NULL|				
|🟦 DEMAIN			|0xFF66	|0x0001		|RP||String (0x42)|4 car|-|Couleur du lendemain|NULL|
|HHPHC			|0xFF66	|0x0002		|RO||Uint8 (0x20)|1 car|-|Horaire Heure Pleines Heures Creuses|0|
|PPOT 			|0xFF66	|0x0003		|RO|Triphasé|Uint8 (0x20)|2 car|-| Présence des potentiels|0|
|🟧 PEJP			|0xFF66	|0x0004		|RP||Uint8 (0x20)|2 car|Min|Préavis début EJP(30min)|0|
|ADPS			|0xFF66	|0x0005		|RP||Uint16 (0x21)|3 car|A|Avertissement de Dépassement De Puissance Souscrite|0|
|ADIR1			|0xFF66	|0x0006		|RP|Triphasé|Uint16 (0x21)|3 car|A|Avertissement de Dépassement D'intensité phase 1|0|
|ADIR2			|0xFF66	|0x0007		|RP|Triphasé|Uint16 (0x21)|3 car|A|Avertissement de Dépassement D'intensité phase 2|0|
|ADIR3			|0xFF66	|0x0008		|RP|Triphasé|Uint16 (0x21)|3 car|A|Avertissement de Dépassement D'intensité phase 3|0|
|MOTDETAT|0xFF66	|0x0009	|RO|-|String (0x42)|6 car||Etat du Linky (From V13)|"000000"|
|-			|0xFF66	|0x0300		|RO||Uint8 (0x20)|1 car|-|Protocole linky|0|

#### Mode standard
|Commande TIC|CLUSTER|Attribut|Droit|Comment|data type|size max|unit|designation|valeur par defaut|
|------------|-------|--------|-----|--------------|---------|-------|----|-----------|------------|				
|ADSC|0x0702|0x0308|RO||String (0x41)|12 car|-|	Adresse Secondaire du Compteur|NULL|
|NGTF|0xFF66|	0x0000|		RO||String (0x42)|16 car|-| Nom du calendrier tarifaire fournisseur|-|	
|LTARF|0xFF66|	0x0200|		RO||String (0x42)|16 car|-| Libellé tarif fournisseur en cours|-|	
|NTARF|0xFF66|	0x0201|		RO||Uint8 (0x20)|2 car|-| Numéro de l’index tarifaire en cours|-|	
|VTIC|0xB01|	0x000A|		RO||Uint16 (0x21)|2 car|-| Version de la TIC|-|	
|DATE|0xFF66|	0x0202|		RO||String (0x42)|10 car|-| Date et heure courant|EX: E211022162000 |	
|EAST|0x702|	0x0000|		RP||Uint48 (0x25)|9 car|Wh| Energie active soutirée totale|0 |	
|EASF01|0x702|	0x0100|		RP||Uint48 (0x25)|9 car|Wh| Energie active soutirée Fournisseur, index 01|0 |	
|EASF02|0x702|	0x0102|		RP||Uint48 (0x25)|9 car|Wh| Energie active soutirée Fournisseur, index 02|0 |	
|EASF03|0x702|	0x0104|		RP||Uint48 (0x25)|9 car|Wh| Energie active soutirée Fournisseur, index 03|0 |	
|EASF04|0x702|	0x0106|		RP||Uint48 (0x25)|9 car|Wh| Energie active soutirée Fournisseur, index 04|0 |	
|EASF05|0x702|	0x0108|		RP||Uint48 (0x25)|9 car|Wh| Energie active soutirée Fournisseur, index 05|0 |	
|EASF06|0x702|	0x010A|		RP||Uint48 (0x25)|9 car|Wh| Energie active soutirée Fournisseur, index 06|0 |	
|EASF07|0x702|	0x010C|		RP||Uint48 (0x25)|9 car|Wh| Energie active soutirée Fournisseur, index 07|0 |	
|EASF08|0x702|	0x010E|		RP||Uint48 (0x25)|9 car|Wh| Energie active soutirée Fournisseur, index 08|0 |	
|EASF09|0x702|	0x0110|		RP||Uint48 (0x25)|9 car|Wh| Energie active soutirée Fournisseur, index 09|0 |	
|EASF10|0x702|	0x0112|		RP||Uint48 (0x25)|9 car|Wh| Energie active soutirée Fournisseur, index10|0 |	
|EASD01|0xFF66|	0x0203|		RP||Uint32 (0x23)|9 car|Wh| Energie active soutirée Distributeur, index 01|0 |	
|EASD02|0xFF66|	0x0204|		RP||Uint32 (0x23)|9 car|Wh| Energie active soutirée Distributeur, index 02|0 |	
|EASD03|0xFF66|	0x0205|		RP||Uint32 (0x23)|9 car|Wh| Energie active soutirée Distributeur, index 03|0 |	
|EASD04|0xFF66|	0x0206|		RP||Uint32 (0x23)|9 car|Wh| Energie active soutirée Distributeur, index 04|0 |	
|EAIT|0x702|	0x0001|		RP|Production|Uint48 (0x25)|9 car|Wh| Energie active injectée totale|0 |	
|ERQ1|0xB04|	0x0305|		RP|Production|int32 (0x29)|9 car|VArh| Energie réactive Q1 total|0 |	
|ERQ2|0xB04|	0x050E|		RP|Production|int16 (0x29)|9 car|VArh| Energie réactive Q2 total|0 |	
|ERQ3|0xB04|	0x090E|		RP|Production|int16 (0x29)|9 car|VArh| Energie réactive Q3 total|0 |	
|ERQ4|0xB04|	0x0A0E|		RP|Production|int16 (0x29)|9 car|VArh| Energie réactive Q4 total|0 |	
|IRMS1|0xB04|	0x0508|		RP|mono / triphasé|Uint16 (0x21)|3 car|A| Courant efficace, phase 1|0 |	
|IRMS2|0xB04|	0x0908|		RP|Triphasé|Uint16 (0x21)|3 car|A| Courant efficace, phase 2|0 |	
|IRMS3|0xB04|	0x0A08|		RP|Triphasé|Uint16 (0x21)|3 car|A| Courant efficace, phase 3|0 |	
|URMS1|0xB04|	0x0505|		RP|mono / triphasé|Uint16 (0x21)|3 car|V|Tension efficace, phase 1|0 |	
|URMS2|0xB04|	0x0905|		RP|Triphasé|Uint16 (0x21)|3 car|V| Tension efficace, phase 2|0 |	
|URMS3|0xB04|	0x0A05|		RP|Triphasé|Uint16 (0x21)|3 car|V| Tension efficace, phase 3|0 |	
|PREF|0xB01|	0x000D|		RO||Uint16 (0x21)|2 car|kVA| Puissance app. de référence (PREF)|0 |	
|STGE|0xFF66|	0x0217|		RO||String (0x41)|8 car|-| Registre de Statuts|0 |	
|PCOUP|0xB01|	0x000E|		RO||Uint8 (0x20)|2 car|kVA| Puissance app. de coupure (PCOUP)|0 |	
|SINSTI|0xFF66|	0x0207|		RP|Production|Uint16 (0x21)|5 car|VA| Puissance app. Instantanée injectée|0 |	
|SMAXIN|0xFF66|	0x0208|		RP|Production|Uint16 (0x21)|5 car|VA| Puissance app max. injectée n|0 |	
|SMAXIN-1|0xFF66|	0x0209|		RO|Production|Uint16 (0x21)|5 car|VA| Puissance app max. injectée n-1|0 |	
|CCASN|0xB04|	0x050B|		RP||int16 (0x29)|5 car|W| Point n de la courbe de charge active soutirée|0 |	
|CCASN-1|0xB04|	0x090B|		RP||int16 (0x29)|5 car|W| Point n-1 de la courbe de charge active soutirée|0 |	
|CCAIN|0xFF66|	0x0210|		RP|Production|Uint16 (0x21)|5 car|W| Point n de la courbe de charge active injectée|0 |	
|CCAIN-1|0xFF66|	0x0211|		RO|Production|Uint16 (0x21)|5 car|W| Point n-1 de la courbe de charge active injectée|0 |	
|UMOY1|0xB04|	0x0511|		RP|Mono / triphasé|Uint16 (0x21)|3 car|V|Tension moy. ph. 1|0 |	
|UMOY2|0xB04|	0x0911|		RP|Triphasé|Uint16 (0x21)|3 car|V| Tension moy. ph. 2|0 |	
|UMOY3|0xB04|	0x0A11|		RP|Triphasé|Uint16 (0x21)|3 car|V| Tension moy. ph. 3|0 |	
|SINSTS|0xB04|	0x0306 (Triphasé) <br>0x050F (Monophasé)|		RP|(Mono /Triphasé depuis v10)|Uint16 (0x21)|5 car|VA|Puissance app. Instantanée soutirée|0 |	
|SINSTS1|0xB04|	0x050F|		RP|Triphasé|Uint16 (0x21)|5 car|VA| Puissance app. Instantanée soutirée ph.1|0 |	
|SINSTS2|0xB04|	0x090F|		RP|Triphasé|Uint16 (0x21)|5 car|VA| Puissance app. Instantanée soutirée ph. 2|0 |	
|SINSTS3|0xB04|	0x0A0F|		RP|Triphasé|Uint16 (0x21)|5 car|VA| Puissance app. Instantanée soutirée ph. 3|0 |	
|SMAXSN|0xB04|	0x0304 (Triphasé) <br>0x050D (Monophasé)|		RP|(Mono / Triphasé depuis v10)|int32 (0x2B)|5 car|VA|Puissance app. max. soutirée n|0 |	
|SMAXSN1|0xB04|	0x050D|		RP|Triphasé|int16 (0x29)|5 car|VA| Puissance app. max. soutirée n ph.1|0 |	
|SMAXSN2|0xB04|	0x090D|		RP|Triphasé|int16 (0x29)|5 car|VA| Puissance app. max. soutirée n ph. 2|0 |	
|SMAXSN3|0xB04|	0x0A0D|		RP|Triphasé|int16 (0x29)|5 car|VA| Puissance app. max. soutirée n ph. 3|0 |	
|SMAXSN-1|0xFF66|	0x0400 (Triphasé) <br>0x0212 (Monophasé)|		RO|(Mono / Triphasé depuis v10)|Uint16 (0x21)|5 car|VA|Puissance app. max. soutirée n-1|0 |	
|SMAXSN1-1|0xFF66|	0x0212|		RO|Triphasé|Uint16 (0x21)|5 car|VA| Puissance app. max. soutirée n-1 ph.1|0 |	
|SMAXSN2-1|0xFF66|	0x0213|		RO|Triphasé|Uint16 (0x21)|5 car|VA| Puissance app. max. soutirée n-1 ph.2|0 |	
|SMAXSN3-1|0xFF66|	0x0214|		RO|Triphasé|Uint16 (0x21)|5 car|VA| Puissance app. max. soutirée n-1 ph.3|0 |	
|MSG1|0xFF66|	0x0215|		RO||String (0x42)|32 car|-| Message court|0 |	
|MSG2|0xFF66|	0x0216|		RO||String (0x42)|16 car|-| Message ultra court|0 |	
|PRM|0x702|	0x0307|		RO||String (0x41)|14 car|-| PRM|0 |	
|DPM1|0xFF66|	0x0218|		RO||Uint8 (0x20)|2 car|-| Début Pointe Mobile 1|0 |	
|FPM1|0xFF66|	0x0219|		RO||Uint8 (0x20)|2 car|-| Fin Pointe Mobile 1|0 |	
|DPM2|0xFF66|	0x0220|		RO||Uint8 (0x20)|2 car|-| Début Pointe Mobile 2|0 |	
|FPM2|0xFF66|	0x0221|		RO||Uint8 (0x20)|2 car|-| Fin Pointe Mobile 2|0 |	
|DPM3|0xFF66|	0x0222|		RO||Uint8 (0x20)|2 car|-| Début Pointe Mobile 3|0 |	
|FPM3|0xFF66|	0x0223|		RO||Uint8 (0x20)|2 car|-| Fin Pointe Mobile 3|0 |	
|RELAIS|0xFF66|	0x0224|		RO||Uint16 (0x21)|3 car|-| RELAIS|0 |	
|NJOURF|0xFF66|	0x0225|		RO||Uint8 (0x20)|2 car|-| Numéro du jour en cours calendrier fournisseur|0 |	
|NJOURF+1|0xFF66|	0x0226|		RO||Uint8 (0x20)|2 car|-| Numéro du prochain jour calendrier fournisseur|0 |	
|PJOURF+1|0xFF66|	0x0227|		RO||String (0x42)|98 car|-|Profil du prochain jour calendrier fournisseur|0 |	
|PPOINTE1|0xFF66|	0x0228|		RO||String (0x42)|98 car|-|Profil du prochain jour de pointe|0 |	
|-			    |0xFF66	|0x0300|RO||Uint8 (0x20)|1 car|-|Protocole linky|0|

### Synthèse développeur
|Fonctions|CLUSTER|Attribut|
|---------|-------|--------|
|Standard : EAIT (Production)|0x0702|0x0001|
|Histo : PTEC|0x0702|0x0020|
|Histo : <br>- HCHC<br>- BASE<br>- EJPHN<br>- BBRHCJB<br>Standard :<br>- EASF01 |0x0702|0x0100|
|Histo : <br>- HCHP<br>- EJPHPM<br>- BBRHPJB<br>Standard :<br>- EASF02 |0x0702|0x0102|
|Histo : <br>- BBRHCJW<br>Standard :<br>- EASF03 |0x0702|0x0104|
|Histo : <br>- BBRHPJW<br>Standard :<br>- EASF04 |0x0702|0x0106|
|Histo : <br>- BBRHCJR<br>Standard :<br>- EASF05 |0x0702|0x0108|
|Histo : <br>- BBRHPJR<br>Standard :<br>- EASF06 |0x0702|0x010A|
|Standard : EASF07 |0x0702|0x010C|
|Standard : EASF08 |0x0702|0x010E|
|Standard : EASF09 |0x0702|0x0110|
|Standard : EASF10 |0x0702|0x0112|
|Standard : PRM|0x0702|0x0307|
|Histo : <br>- ADC0<br>Standard : <br>- ADSC|0x0702|0x0308|
|Standard: VTIC|0x0B01|0x000A|
|Histo : <br>- ISOUSC<br> Standard :<br>- PREF|0x0B01|0x000D|
|Standard: PCOUP|0x0B01|0x000E|
|Histo: <br>- IINST<br>- IINST1 (Triphasé)<br>Standard :<br>- IRMS1|0x0B04|0x0508|
|Histo: <br>- IINST2 (Triphasé)<br>Standard :<br>- IRMS2 (Triphasé)|0x0B04|0x0908|
|Histo: <br>- IINST3 (Triphasé)<br>Standard :<br>- IRMS3 (Triphasé)|0x0B04|0x0A08|
|Histo: <br>- IMAX<br>- IMAX1(Triphasé)|0x0B04|0x050A|
|Histo: <br>- IMAX2(Triphasé)|0x0B04|0x090A|
|Histo: <br>- IMAX3(Triphasé)|0x0B04|0x0A0A|
|Histo: <br>- PMAX(Triphasé)<br>Standard :<br>- SMAXN<br>- SMAXN1(Triphasé)|0x0B04|0x050D|
|Histo: <br>- PAPP<br>Standard :<br>- SINSTS<br>- SINSTS1(Triphasé)|0x0B04|0x050F|
|Standard : SINSTS2(Triphasé)|0x0B04|0x090F|
|Standard : SINSTS3(Triphasé)|0x0B04|0x0A0F|
|Standard : ERQ1 (Production)|0x0B04|0x0305|
|Standard : ERQ2 (Production)|0x0B04|0x050E|
|Standard : ERQ3 (Production)|0x0B04|0x090E|
|Standard : ERQ4 (Production)|0x0B04|0x0A0E|
|Standard : URMS1(Mono / Triphasé)|0x0B04|0x0505|
|Standard : URMS2(Triphasé)|0x0B04|0x0905|
|Standard : URMS3(Triphasé)|0x0B04|0x0A05|
|Standard : UMOY1(Mono / Triphasé)|0x0B04|0x0511|
|Standard : UMOY2(Triphasé)|0x0B04|0x0911|
|Standard : UMOY3(Triphasé)|0x0B04|0x0A11|
|Standard : CCASN |0x0B04|0x050B|
|Standard : CCASN-1 |0x0B04|0x090B|
|Histo : <br>- OPTARIF <br>Standard :<br>- NGTF|0xFF66|0x0000|
|Histo : DEMAIN|0xFF66|0x0001|
|Histo : HHPHC|0xFF66|0x0002|
|Histo : PPOT|0xFF66|0x0003|
|Histo : PEJP|0xFF66|0x0004|
|Histo : ADPS|0xFF66|0x0005|
|Histo : ADIR1 (Triphasé)|0xFF66|0x0006|
|Histo : ADIR2 (Triphasé)|0xFF66|0x0007|
|Histo : ADIR3 (Triphasé)|0xFF66|0x0008|
|Standard : LTARF|0xFF66|0x0200|
|Standard : NTARF|0xFF66|0x0201|
|Standard : DATE|0xFF66|0x0202|
|Standard : EASD01|0xFF66|0x0203|
|Standard : EASD02|0xFF66|0x0204|
|Standard : EASD03|0xFF66|0x0205|
|Standard : EASD04|0xFF66|0x0206|
|Standard : SINSTI (Production)|0xFF66|0x0207|
|Standard : SMAXIN (Production)|0xFF66|0x0208|
|Standard : SMAXIN-1 (Production)|0xFF66|0x0209|
|Standard : CCAIN (Production)|0xFF66|0x0210|
|Standard : CCAIN-1 (Production)|0xFF66|0x0211|
|Standard : <br>- SMAXN-1 (Monophasé)<br>- SMAXN1-1 (Triphasé)|0xFF66|0x0212|
|Standard : SMAXN1-1 (Triphasé)|0xFF66|0x0212|
|Standard : SMAXN2-1 (Triphasé)|0xFF66|0x0213|
|Standard : SMAXN3-1 (Triphasé)|0xFF66|0x0214|
|Standard : MSG1|0xFF66|0x0215|
|Standard : MSG2|0xFF66|0x0216|
|Standard : STGE|0xFF66|0x0217|
|Standard : DPM1|0xFF66|0x0218|
|Standard : FPM1|0xFF66|0x0219|
|Standard : DPM2|0xFF66|0x0220|
|Standard : FPM2|0xFF66|0x0221|
|Standard : DPM3|0xFF66|0x0222|
|Standard : FPM3|0xFF66|0x0223|
|Standard : RELAIS|0xFF66|0x0224|
|Standard : NJOURF|0xFF66|0x0225|
|Standard : NJOURF+1|0xFF66|0x0226|
|Standard : PJOURF+1|0xFF66|0x0227|
|Standard : PPOINTE1|0xFF66|0x0228|

## Mise à jour

### OTA

Il existe deux types de Firmware :  
* Legacy : Mode routeur (possibilité de gérer le routage de 5 enfants)  
* Limited : Mode routeur (1 seul routage autorisé)  
  
⚠️ **La mise à jour OTA ne peut se faire que si le mode du fichier OTA est le même que le précédent.**  
  
Pour changer de mode, veuillez suivre la mise à jour (non OTA)  

### Non OTA

⚠️ **Pour les nouveaux achats, le connecteur noir pour la mise à jour physique n'est plus disponible. Pour autant, il est tout de même possible de procéder à la mise à jour physique mais en maintenant le module USB sur les pastilles. Pour les dernières versions, le hardware et le software permettent d'effectuer des mises à jour OTA au travers de votre box domotique.**  

Tout d'abord, il faut dévisser le boitier afin de sortir la carte électronique.
Ensuite, il faut brancher le module USB TTL (vous pouvez vous en procurer un [ici](https://lixee.fr/produits/25-module-usb-ttl-cp2102.html) ou [ici](https://www.amazon.fr/s?k=CP2102)) sur le ZLinky_TIC comme sur la photo. 

<img src="https://github.com/fairecasoimeme/Zlinky_TIC/blob/master/Doc/Images/update_ZLinky_TIC_1.jpg" width="400">  

Pour ceux qui n'ont pas ce modèle, voici la correspondance des 5 PINs :  
V1:  
<img src="https://github.com/fairecasoimeme/Zlinky_TIC/blob/master/Doc/Images/update_ZLinky_TIC_4.jpg" width="400">  
V1.2:  
<img src="https://github.com/fairecasoimeme/Zlinky_TIC/blob/master/Doc/Images/PCB_ZLinky_v1.2_flash.jpg" width="400">  

Une fois que les branchements sont OK, il suffit d'insérer sur votre ordinateur la clef USB en maintenant le bouton **Flash** puis relacher.

<img src="https://github.com/fairecasoimeme/Zlinky_TIC/blob/master/Doc/Images/update_ZLinky_TIC_2.jpg" width="400"> 

V2.0:  

<img src="https://github.com/fairecasoimeme/Zlinky_TIC/blob/master/Doc/Images/ZLinky_v2_debug.jpg" width="800"> 
<img src="https://github.com/fairecasoimeme/Zlinky_TIC/blob/master/Doc/Images/ZLinky_v2_flash.jpg" width="800"> 
<img src="https://github.com/fairecasoimeme/Zlinky_TIC/blob/master/Doc/Images/Zlinky_v2_illustration.jpg" width="400"> 

Une fois que les branchements sont OK, il suffit d'insérer sur votre ordinateur la clef USB en maintenant le bouton **Flash** puis relacher.

**Sous Windows**  
* Télécharger et installer (si nécessaire) les pilotes du module USB TTL [ici](https://www.silabs.com/developers/usb-to-uart-bridge-vcp-drivers?tab=downloads)
* Télécharger et installer [l'outil de flashage](https://github.com/fairecasoimeme/ZiGatev2/raw/master/Tools/JN-SW-4407-DK6-Flash-Programmer.zip).
* Télécharger le firmware souhaité : (https://github.com/fairecasoimeme/Zlinky_TIC/releases)
* Ouvrir une console
* Aller dans le répertoire de l'outil de flash
* Lancer la commande suivante :
  
Sans remise à zéro (Conserve les appareils déjà appairés):  
`DK6Programmer -s COMXX -P 500000 -Y -p <NOM_DU_FIRMWARE.bin>`  

Avec remise à zéro (Efface toutes les données (appairages compris)):  
`DK6Programmer -s COMXX -P 500000 -e FLASH -Y -p <NOM_DU_FIRMWARE.bin>`  

## Errata (hardware v1.0)

Sur les premières versions hardware, il n'y a pas de condensateur (voir la photo) engendrant quelques fois, sur certains Linky et en fonction du nombre de requêtes, des coupures ou déconnexion. Si c'est le cas, il convient de contacter par mail le fabricant contact[at]lixee.fr pour effectuer la modification.

<img src="https://github.com/fairecasoimeme/Zlinky_TIC/blob/master/Doc/Images/ZLinky_PCB_face_without_capa.jpg" width="800">

Si vous souhaitez faire la modification vous même, vous pouvez le faire à vos risques. Cependant, pour ceux qui maîtrisent le fer à souder, vous pouvez rajouter le condensateur avec la référence suivante ou équivalente :  
**Ref : 293D107X06R3D2TE3  
Descriptif : 100uF 6.3Volts format: 293D**  

PS : Si vous n'avez pas exactement la bonne valeur, vous pouvez mettre plus que 100uF)  
Voici à quoi ressemble la modification:  
<img src="https://github.com/fairecasoimeme/Zlinky_TIC/blob/master/Doc/Images/ZLinky_condensateur.jpg" width="800">

## Intégrations 

Voici les différentes possibilités d'intégrations du ZLinky

### Jeedom
Afin d'intégrer le ZLinky à Jeedom, il suffit d'utiliser le [plugin officiel Zigbee](https://market.jeedom.com/index.php?v=d&p=market_display&id=4050).
le ZLinky est directement reconnu et les commandes sont automatiquements créés.

### Home assistant
il existe 2 méthodes pour intégrer le ZLinky. Soit en utilisant Zigbee2Mqtt, soit en utilisant ZHA  

#### Z2M
Vous pouvez installer Z2M en suivant le tutoriel suivant :  
* [Site officiel](https://www.zigbee2mqtt.io/guide/installation/03_ha_addon.html)  
* [Tutoriel domo-blog](https://www.domo-blog.fr/installer-mqtt-zigbee2mqtt-home-assistant-en-mode-supervision-guide-domotique-all-inclusive/)  
* [Tutoriel les alexiens](https://www.lesalexiens.fr/actualites/installer-serveur-zigbee2mqtt-home-assistant/)  

Une fois installé, Vous pourrez intégrer le ZLinky en lançant un appairage classique car il est nativement compatible

#### ZHA
Pour ZHA, l'installation est un peu plus complexe car le ZLinky n'est pas nativement compatible.  

* [Site officiel](https://www.home-assistant.io/integrations/zha/)  

La compatibilité du ZLinky_TIC est désormais native sur ZHA. Pour une configuration plus avancée et personnaliser les entités, vous pouvez suivre le lien suivant:  

* [Configuration avancée](https://github.com/fairecasoimeme/Zlinky_TIC/issues/18#issuecomment-1285215566)

### EEdomus

Pour la Box Eedomus, il suffit de suivre le [tutoriel officiel](https://doc.eedomus.com/view/Eedomus_et_ZLinky_TIC)  

### Domoticz

Intégration avec le [plugin Zigbee for Domoticz (gratuit)](https://github.com/zigbeefordomoticz/Domoticz-Zigbee#zigbeefordomoticz-plugin--connect-all-your-zigbee-devices-to-domoticz), il suffit d'appairer le ZLinky en version V12 avec le plugin.
Les modes Historique et Standard sont supportés. Nous avons effectués des tests avec des compteurs Linky Historique Base, Standard Base, Historique Tri

### Homey pro

Lien vers l'application : [App](https://homey.app/fr-fr/app/com.lixee/liXee/)  
Lien vers le forum concerné : [Forum ZLinky Homey](https://community.homey.app/t/app-pro-lixee/74924)  
⚠️ Avec certaines versions du logiciel LiXee sur HomeyPro 2023, il est parfois nécessaire d'avoir un routeur zigbee déjà jumelé avec la box pour que le ZLinky puisse s'appairer avec la box.


### SmartThing

Lien vers l'application : [Sources](https://github.com/sdalu/EdgeLinkyTIC)  
Lien vers l'installation : [Installation](https://community.smartthings.com/t/st-edge-zlinky-tic/254857)  

### Tuya (From v14)

Depuis la version v4000-0014, il est possible d'intégrer le ZLinky_TIC sur une passerelle zigbee Tuya. Pour cela, il suffit de passer la passerelle zigbee en mode "appairage".  
   * Cliquer sur la passerelle zigbee
   * Cliquer sur "Ajouter un sous-appareil"
   * Mettre le ZLinky en mode appairage - Appui long sur le bouton "link" (clignotement rapide de la LED bleue)
   * Cliquer sur "La LED clignote déjà"
   * Attendre que "Power Clamp" apparaisse dans la découverte des appareils zigbee à proximité

Voici les données exploitables sur la plateforme Tuya :
   * Mode historique:
     * Consommation totale (en kWh)
     * Consommation horaire (en kWh)
     * Intensité par phase (en A)
     * Puissance apparente totale (en kVA affiché kW)
     * Graphes consommation journalier / mensuel / annuel
     * 1 trigger sur la puissance apparente totale
     * 1 trigger sur la consommation totale     
    
   * Mode standard (mode conseillé):
     * Consommation totale (en kWh)
     * Consommation horaire (en kWh)
     * Intensité par phase (en A)
     * Tension par phase (en V)
     * Puissance apparente par phase (en kVA marqué kW)
     * Puissance apparente totale (en kVA affiché kW)
     * Graphes consommation journalier / mensuel / annuel
     * 1 trigger sur la puissance apparente totale
     * 1 trigger sur la consommation totale
    
Screenshots  

|Mode Historique|Mode Standard|Exemple graphique|
|---------------|-------------|-----------------|
|<img src="https://github.com/fairecasoimeme/Zlinky_TIC/blob/master/Doc/Images/Screenshot_dashboard_tuya_smart.jpg" width="200">|<img src="https://github.com/fairecasoimeme/Zlinky_TIC/blob/master/Doc/Images/Screenshot_dashboard_standard_tuya_smart.jpg" width="200">|<img src="https://github.com/fairecasoimeme/Zlinky_TIC/blob/master/Doc/Images/Screenshot_graphe_tuya_smart.jpg" width="200">|

## F.A.Q

### 1. J'ai changé d'abonnement et je ne retrouve plus mes index dans mon dashboard domotique
Le fait de changer d'abonnement historique (comme base ou heure creuse par exemple) vers un nouvel abonnement (super heures creuses, zenflex, heures creuses WE par exemple) implique un changement de gestion structurel du Linky.  
Par défaut, le Linky est installé en mode historique (ce mode gère uniquement les anciens contrats). Lors du changement d'abonnement, le Linky ne sait alors plus gérer les index car son mode n'est plus compatible.  
Pour retrouver les index correspondant aux différents tarifs, il suffit de demander le changement de mode à son fournisseur d'électricité. Il faut demander le mode standard.  
Pour passer son compteur Linky du mode historique au mode standard :  
   * si vous êtes chez EDF, vous pouvez effectuer la demande à travers ce [formulaire](https://particulier.edf.fr/fr/accueil/espace-client/contact/demande/formulaire/decret-conso.html#/selection-droit-exerce) de votre espace en ligne, en choisissant ensuite "Modifier le statut de la Télé Information Client (TIC)" puis en précisant que vous souhaitez passer en mode standard dans le champ commentaire.  
   * si vous n'êtes pas chez EDF, il faut appeler votre Fournisseur d'Electricité, et demander le passage de votre compteur Linky en mode standard, en précisant qu’il s’agit de la prestation F185 auprès d'Enedis.

Cette opération est gratuite et en général, le changement est effectué dans les 24 heures ouvrées.  

### 2. Qu'apporte le mode standard par rapport au mode historique ?
* La principale différence est qu'en mode standard, vous pourrez mieux gérer les nouveaux abonnements
* Vous aurez plus d'informations comme la tension par exemple [(voir le tableau des données)](#mode-historique)
* C'est une condition nécessaire pour gérer l'injection de vos panneaux photovoltaïques
* Vous aurez une meilleure précision sur la puissance apparente (10VA --> 1VA)

### 3. J'ai installé des panneaux photovoltaïques et j'aimerai récupérer les infos d'injection
Pour avoir les données d'injection, il faut remplir les 2 conditions suivantes:  
* Etre en mode standard  
* Avoir un contrat CRAE avec EDF OA. (Dans un futur proche, il suffira d'avoir un contrat CACSI mais à ma connaissance, ce n'est pas encore le cas)  

### 4. Je ne comprends pas, la puissance affichée par le Linky ne correspond pas à la puissance mesurée
Attention, le Linky ne mesure que la puissance apparente (en VA). Cette puissance est dans certains cas différente de la puissance active (en W). Tout dépend du déphasage (cos Phi) des appareils qui consomment dans votre maison.  
Pour mieux comprendre la différence, il existe énormément de tuto sur Internet mais j'aime bien celle-ci : [(video)](https://www.youtube.com/watch?v=buRaVQ5NKFs)  
⚠️ **Même si le Linky affiche la puissance apparente, votre fournisseur d'électricité ne facturera que la consommation réellement consommée (en Wh) basé sur la puissance active (en W)**   

### 5. La LED (bleue) de mon ZLinky reste éteinte, que faire ?
Si la LED bleu reste éteinte, ce type de phénomène arrive parfois lors d’une mise à jour du Linky, une coupure générale ou une mauvaise manipulation. L’alimentation de la téléinformation se met en protection. Pour régler le problème, il faut débrancher le ZLinky pendant au moins 30 min puis rebrancher (attention la LED peut mettre plusieurs secondes à s’allumer).  

### 6. Quelle est la distance maximale entre le ZLinky et le coordinateur Zigbee ?
Difficile de répondre préciséement à la question car cela dépend beaucoup de votre environnement. (obstacle et/ou perturbation radio)  
En général, la distance max pour un ZLinky avec l'antenne interne est de 15-20 mètres maximum et de 20-25 mètre avec l'antenne externe.  
Pour palier le problème de distance, il existe plusieurs solution :  
* Le zigbee permet de "mailler" le réseau. Grâce à des équipements zigbee qui ont la fonction de routeur, il est possible d'augmenter les distances en plaçant stratégiquement les appareils zigbee routeur qui joueront le rôle de relai et ainsi augmenter la portée ou fiabiliser les échanges.
* Utiliser la version du ZLinky avec antenne externe. En effet, grâce à cette version, vous pourrez utiliser une rallonge SMA [(exemple de rallonge compatible)](https://amzn.to/3PUPA4J) afin de déporter l'antenne à un endroit plus optimisé. Vous pourrez aussi utiliser une antenne avec un gain plus important que celle par défaut (2dBi). [(exemple d'antenne avec un meilleur gain)](https://amzn.to/4cPQDwA)
* Corriger les perturbation radios liés au WiFi. En effet, certains paramétrage WiFi peut polluer le réseau zigbee. Il convient parfois de limiter la bande passante de votre borne WiFi (passer en 20Mhz plutôt que 40Mhz) et/ou de changer de canal WiFi(il faut éloigner au maximum le canal WiFi du canal Zigbee. Je vous conseille de lire cet article qui rentrera plus dans les détails. [(Tutoriel)](https://haade.fr/fr/blog/interference-zigbee-wifi-2-4ghz-a-savoir)

### 7. Malgré le maillage de mon réseau zigbee, la portée n'est pas suffisante. Existe t-il une autre solution ?
Il est possible d'utiliser la solution suivante pour [augmenter la portée du ZLinky](https://faire-ca-soi-meme.fr/domotique/2025/03/29/augmenter-la-portee-du-zlinky_tic-v2/)

## Changelog

### Version 0016

* Fix dataType for SINSTS ERQ1 and SMAXN for the reporting
* Fix acquisition after update
* Delete some debug to free UART


### Version 0015

* Add Silvercrest / Livarno compatibility with Tuya (change the device id to Dimming ligth)
* Add tariffPeriod (0x10) attribute with Lixee cluster (0xff66).
   - Add alarm (reporting) on value update
* Decreasing MAX Report interval to 20 seconds (in aim to enhance reporting time)
* Fix Total apparent power for Tuya
* Enhance netwrok management
 

### Version 0014
* Add Tuya zigbee compatibility (see datas management [here](#tuya-from-v14)
* Add ENEDIS Labellisation software "Linky Ready"
  - Better data consistency
  - Better error management
  - Change LED blinking
  - Change UART decoding


### Version 0013
* Add MOTDETAT value (from historic mode) --> Cluster : 0xFF66 - Attribut : 0x0009
* Add null value for all of default metering properties
* Add watchdog reset process when hardfault error
* Fix duplicate voltage acquisition
* Fix operator error for comparison

### Version 0012
* Add voltage driver
* Add main voltage infos  
Clusters 0x0001 :
  - Attribut 0x0000 (mains voltage)
  - Attribut 0x0011 (mains voltage min threshold)
  - Attribut 0x0012 (mains voltage max threshold)
* Add more zigbee radio reactivity - Now you can send and receive zigbee requests during Linky acquisition


### Version 0011

* Add SINST SMAXN SMAXN-1 "monophasé" compatibility to all plugins

### Version 0010

* Update LED management to reduce consumption when all is ok (LED ON)
* Add new cluster/ attr SINSTS, SMAXN and SMAXN-1 because these values are unique in "mode triphasé" https://github.com/fairecasoimeme/Zlinky_TIC/issues/112
* Update MCUXpresso config files and Makefile to manage limited and legacy firmware
* Update OTA time wait. Verify update OTA between 2 hours and 4 hours instead of 2m-4m
* Rename the limited version configuration file

### Version 0009

* Fix Manufacturer specific issue. to fix the manufacturer Specific request error, The check of the flag has been deactivated. This method has been chosen to give compatibility with all plugins.
https://github.com/zigpy/zha-device-handlers/pull/1165


### Version 0008

* Fix inconsistent app.zpscfg for router mode. https://github.com/fairecasoimeme/Zlinky_TIC/issues/99 

### Version 0007

* Add Linky acquisition timer parameter.(Readable/Writable and in PDM memory) Cluster : 0xff66 - attribut : 0x0100 (par defaut : 0x0A)
* Add Blinking LED (fast) for permit join mode
* Add 2 config files. With or without childs routing function
* Fix blocking timer when Linky Acquisition.
* Fix Reportable Attribut. SMAXN2 Cluster : 0x0b04 - attribut : 0x090D / SMAXN3 Cluster : 0x0b04 - attribut : 0x0A0D

### Version 0006

* Add Route limitation to 1. (Save energy)
* Add fragmented packet capacity
* Increase size of config report table (max 50)
* Increase PDU size
* Fix data type (0x42) and length (max 72 for the moment) of Standard PJOUR+1 and PPONTE

### Version 0005

* Add bindtable in debug view
* Add compatibility with new hardware
* Fix ADPS RAZ bug


### Version 0004

* Add LinkyMode property (cluster 0xFF66 / property : 0x0300)
* Add Alarm reportable function
* Add Alarm when STGE was updated
* Fix reportable value (UMOY1, UMOY2, UMOY2, SMAXN, STGE)
* Fix ADPS value (RAZ when no appear)


### Version 0003

* Fix data type for 0x702 cluster attribut consumption. uint32 to uint48 (For ZiGate v3.21)
* Fix config report bug on cluster 0x702
* Fix PIN Led error
* Fix differents data type value (DATE, HHPHC, VTIC, PREF)
* Fix version size 0x0000 | 0x4000


### Version 0001
Version initiale
