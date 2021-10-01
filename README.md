# ZLinky_TIC
 Téléinformation Linky autoalimenté communiquant en ZigBee 3.0
 

## Description
![Description ZLinky_TIC](https://github.com/fairecasoimeme/Zlinky_TIC/blob/master/Doc/Images/ZLinky_description.JPG)
<img src="https://github.com/fairecasoimeme/Zlinky_TIC/blob/master/Doc/Images/ZLinky_prototype.jpg" width="400">

## PCB
<img src="https://github.com/fairecasoimeme/Zlinky_TIC/blob/master/Doc/Images/ZLinky_PCB_face.jpg" width="800">
<img src="https://github.com/fairecasoimeme/Zlinky_TIC/blob/master/Doc/Images/ZLinky_PCB_pile.jpg" width="800">

**ZLinky_TIC** est un appareil permettant de transmettre toutes les informations du Linky en ZigBee 3.0.  
**ZLinky_TIC** est alimenté par votre compteur Linky. Il suffit simplement d'enlever le cash (jaune fluo) et de "plugger" l'appareil en pressant sur le bouton orange.  
Au départ, **ZLinky_TIC** est en attente d'appairage mais si vous souhaitez réinitialiser l'appareil, il suffit de rester appuyer sur le bouton "link" pendant 10 secondes. L'appareil va clignoter lentement. Ensuite, le bouton relâché, l'appareil est reinitialisé.  
Une fois appairé à votre box domotique par l'intermédiaire d'une ZiGate ou d'un autre coordinateur ZigBee, vous pourrez gérer votre consommation d'électricité.

**!!! WARNING !!! Actuellement, l'appareil ne fonctionne qu'avec le mode Historique du compteur Linky. L'évolution du Firmware permettra de gérer le mode Standard et Gaspar**

## Voyant lumineux

L'appareil clignote en bleu toutes les 6-7 secondes correspondant aux phases de réveil.  
<img src="https://github.com/fairecasoimeme/Zlinky_TIC/blob/master/Doc/Images/manuel_LED_cycle_sleep.png" width="200">  

L'appareil clignote 2 fois toutes les 7 secondes : l'appareil n'est pas encore appairé.  
<img src="https://github.com/fairecasoimeme/Zlinky_TIC/blob/master/Doc/Images/manuel_LED_No_JOIN.png" width="400">  

L'appareil s'allume en bleu pendant 3 secondes correspondant à l'envoi des données au coordinateur.  
<img src="https://github.com/fairecasoimeme/Zlinky_TIC/blob/master/Doc/Images/manuel_LED_send_datas.png" width="200">  

L'appareil clignote 4 fois. Zlinky_TIC rencontre des erreurs lors de l'acquisition des données du Linky.  
<img src="https://github.com/fairecasoimeme/Zlinky_TIC/blob/master/Doc/Images/manuel_LED_cycle_error.png" width="200">  

L'appareil clignote lentement : le bouton "link" est actionné sans relachement pour une réinitilisation.  
<img src="https://github.com/fairecasoimeme/Zlinky_TIC/blob/master/Doc/Images/manuel_LED_Reset.png" width="200">  

## Clusters

### Clusters List

|Num (Hexa)|Name|I/O| Comment|
|----------|----|---|--------|  
|0x0000|Basic|I||
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

### Subscription (OPTARIF Values)
![#f03c15](https://via.placeholder.com/15/f03c15/000000?text=+) BASE  
![#c5f015](https://via.placeholder.com/15/c5f015/000000?text=+) HC..  
![#c56615](https://via.placeholder.com/15/c56615/000000?text=+) EJP.  
![#1589F0](https://via.placeholder.com/15/1589F0/000000?text=+) BBRx  

RO= READ only RP= Reportable / RW = Read/Write	

|Commande TIC|CLUSTER|Attribut|Droit|Comment|data type|size max|unit|designation|valeur par defaut|
|------------|-------|--------|-----|--------------|---------|-------|----|-----------|------------|				
|ADC0|0x0702|0x0308|RO||String|12 car|-|	Serial Number|NULL|
|![#f03c15](https://via.placeholder.com/15/f03c15/000000?text=+) BASE|0x0702|0x0000|RP||Uint32|9 car|Wh| Index Base|0|
|OPTARIF			|0xFF66|	0x0000|		RO||String|4 car|-| Option tarifaire|BASE|	
|ISOUSC			|0x0B01	|0x000D		|RO		||Uint16|2 car|A| Intensité souscrite|0|
|![#c5f015](https://via.placeholder.com/15/c5f015/000000?text=+) HCHC			|0x0702	|0x0102		|RO			||Uint32|9 car|Wh|Index HCHC|0|
|![#c5f015](https://via.placeholder.com/15/c5f015/000000?text=+) HCHP			|0x0702	|0x0100		|RO			||Uint32|9 car|Wh|Index HCHP|0|
|![#c56615](https://via.placeholder.com/15/c56615/000000?text=+) EJPHN			|0x0702	|0x0100		|RO			||Uint32|9 car|Wh|Index EJPHN|0|
|![#c56615](https://via.placeholder.com/15/c56615/000000?text=+) EJPHPM			|0x0702	|0x0102		|RO			||Uint32|9 car|Wh|Index EJPHPM|0|
|![#1589F0](https://via.placeholder.com/15/1589F0/000000?text=+) BBRHCJB			|0x0702	|0x0100		|RO			||Uint32|9 car|Wh|Index BBRHCJB|0|
|![#1589F0](https://via.placeholder.com/15/1589F0/000000?text=+) BBRHPJB			|0x0702	|0x0102		|RO			||Uint32|9 car|Wh|Index BBRHPJB|0|
|![#1589F0](https://via.placeholder.com/15/1589F0/000000?text=+) BBRHCJW			|0x0702	|0x0104		|RO			||Uint32|9 car|Wh|Index BBRHCJW|0|
|![#1589F0](https://via.placeholder.com/15/1589F0/000000?text=+) BBRHPJW			|0x0702	|0x0106		|RO			||Uint32|9 car|Wh|Index BBRHPJW|0|
|![#1589F0](https://via.placeholder.com/15/1589F0/000000?text=+) BBRHCJR			|0x0702	|0x0108		|RO			||Uint32|9 car|Wh|Index BBRHCJR|0|
|![#1589F0](https://via.placeholder.com/15/1589F0/000000?text=+) BBRHPJR			|0x0702	|0x010A		|RO			||Uint32|9 car|Wh|Index BBRHPJR|0|
|IINST			|0x0B04	|0x0508		|RP			||Uint16|3 car|A|Courant efficace|0xFFFF|
|IINST1			|0x0B04	|0x0508		|RP			||Uint16|3 car|A|Courant efficace phase 1|0xFFFF|
|IINST2			|0x0B04	|0x0908		|RP			||Uint16|3 car|A|Courant efficace phase 2|0xFFFF|
|IINST3			|0x0B04	|0x0A08		|RP			||Uint16|3 car|A|Courant efficace phase 3|0xFFFF|
|IMAX			|0x0B04	|0x050A		|RO			||Uint16|3 car|A|Intensité maximale|0xFFFF|
|IMAX1			|0x0B04	|0x050A		|RO			||Uint16|3 car|A|Intensité maximale phase 1|0xFFFF|
|IMAX2			|0x0B04	|0x090A		|RO			||Uint16|3 car|A|Intensité maximale phase 2|0xFFFF|
|IMAX3			|0x0B04	|0x0A0A		|RO			||Uint16|3 car|A|Intensité maximale phase 3|0xFFFF|
|PMAX			|0x0B04	|0x050D		|RO			||Uint16|5 car|W|Puissance maximale triphasée atteinte|0x8000|
|PAPP			|0x0B04	|0x050F		|RP			||Uint16|5 car|VA|Puissance apparente|0xFFFF|
|PTEC			|0x0702	|0x0020		|RO	||String|4 car|-|Periode tarifaire en cours|NULL|
|			|0xFF66	|0x0100		|RW		|Change the periodic Linky acquisition time based on 7 sec cycle. ex : value=1 → send every 7 sec. Value= 5 → send every 35 sec	etc|Uint16|-|-|-|0xA|								
|![#1589F0](https://via.placeholder.com/15/1589F0/000000?text=+) DEMAIN			|0xFF66	|0x0001		|RP||String|4 car|-|Couleur du lendemain|NULL|
|HHPHC			|0xFF66	|0x0002		|RO||Uint8|1 car|-|Horaire Heure Pleines Heures Creuses|0|
|PPOT 			|0xFF66	|0x0003		|RO||Uint8|2 car|-| Présence des potentiels|0|
|![#c56615](https://via.placeholder.com/15/c56615/000000?text=+) PEJP			|0xFF66	|0x0004		|RP||Uint8|2 car|Min|Préavis début EJP(30min)|0|
|ADPS			|0xFF66	|0x0005		|RP||Uint16|3 car|A|Avertissement de Dépassement De Puissance Souscrite|0|
|ADIR1			|0xFF66	|0x0006		|RP||Uint16|3 car|A|Avertissement de Dépassement D'intensité phase 1|0|
|ADIR2			|0xFF66	|0x0007		|RP||Uint16|3 car|A|Avertissement de Dépassement D'intensité phase 2|0|
|ADIR3			|0xFF66	|0x0008		|RP||Uint16|3 car|A|Avertissement de Dépassement D'intensité phase 3|0|

## Changelog

### Version 4000-0001
Version initiale
