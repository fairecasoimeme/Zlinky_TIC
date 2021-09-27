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
L'appareil s'allume en bleu pendant 1 seconde correspondant à l'envoi des données au coordinateur.  
L'appareil clignote lentement : le bouton "link" est actionné sans relachement pour une réinitilisation.  

## Clusters

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



RO= READ only RP= Reportable / RW = Read/Write	

|Commande TIC|CLUSTER|Attribut|Droit|Force Reported|data type|size max|unit|designation|
|------------|-------|--------|-----|--------------|---------|-------|----|-----------|					
|ADC0|0x0702|0x0308|RO||String|12 car|-|	Serial Number|
|BASE|0x0702|0x0000|RP|x|Uint32|9 car|Wh| Index Base|
|OPTARIF			|0xFF66|	0x0000|		RO||String|4 car|-| Option tarifaire|			
|ISOUSC			|0x0B01	|0x000D		|RO		||Uint16|2 car|A| Intensité souscrite|
|HCHC			|0x0702	|0x0102		|RO			|x|Uint32|9 car|Wh|Index HCHC|
|HCHP			|0x0702	|0x0100		|RO			|x|Uint32|9 car|Wh|Index HCHP|
|EJPHN			|0x0702	|0x0100		|RO			|x|Uint32|9 car|Wh|Index EJPHN|
|EJPHPM			|0x0702	|0x0102		|RO			|x|Uint32|9 car|Wh|Index EJPHPM|
|BBRHCJB			|0x0702	|0x0100		|RO			|x|Uint32|9 car|Wh|Index BBRHCJB|
|BBRHPJB			|0x0702	|0x0102		|RO			|x|Uint32|9 car|Wh|Index BBRHPJB|
|BBRHCJW			|0x0702	|0x0104		|RO			|x|Uint32|9 car|Wh|Index BBRHCJW|
|BBRHPJW			|0x0702	|0x0106		|RO			|x|Uint32|9 car|Wh|Index BBRHPJW|
|BBRHCJR			|0x0702	|0x0108		|RO			|x|Uint32|9 car|Wh|Index BBRHCJR|
|BBRHPJR			|0x0702	|0x010A		|RO			|x|Uint32|9 car|Wh|Index BBRHPJR|
|IINST			|0x0B04	|0x0508		|RP			||Uint16|3 car|A|Courant efficace|
|IINST1			|0x0B04	|0x0508		|RP			||Uint16|3 car|A|Courant efficace phase 1|
|IINST2			|0x0B04	|0x0908		|RP			||Uint16|3 car|A|Courant efficace phase 2|
|IINST3			|0x0B04	|0x0A08		|RP			||Uint16|3 car|A|Courant efficace phase 3|
|IMAX			|0x0B04	|0x050A		|RO			||Uint16|3 car|A|Intensité maximale|
|IMAX1			|0x0B04	|0x050A		|RO			||Uint16|3 car|A|Intensité maximale phase 1|
|IMAX2			|0x0B04	|0x090A		|RO			||Uint16|3 car|A|Intensité maximale phase 2|
|IMAX3			|0x0B04	|0x0A0A		|RO			||Uint16|3 car|A|Intensité maximale phase 3|
|PMAX			|0x0B04	|0x050D		|RO			||Uint16|5 car|W|Puissance maximale triphasée atteinte|
|PAPP			|0x0B04	|0x050F		|RP			||Uint16|5 car|VA|Puissance apparente|
|PTEC			|0x0702	|0x0020		|RO	||String|4 car|-|Periode tarifaire en cours|
|			|0xFF66	|0x0100		|RW		|Change the periodic sending time based on 7 sec cycle. ex : value=1 → send every 7 sec. Value= 5 → send every 35 sec	etc|Uint16|-|-|-|											
|DEMAIN			|0xFF66	|0x0001		|RO||String|4 car|-|Couleur du lendemain|
|HHPHC			|0xFF66	|0x0002		|RO||Uint8|1 car|-|Horaire Heure Pleines Heures Creuses|
|PPOT 			|0xFF66	|0x0003		|RO||Uint8|2 car|-| Présence des potentiels|
|PEJP			|0xFF66	|0x0004		|RO||Uint8|2 car|Min|Préavis début EJP(30min)|
|ADPS			|0xFF66	|0x0005		|RO||Uint16|3 car|A|Avertissement de Dépassement De Puissance Souscrite|

## Changelog

### Version 4000-0001
Version initiale
