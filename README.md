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

RO= READ only RP= Reportable / RW = Read/Write	

|Commande TIC|CLUSTER|Attribut|Droit|Reported|
|------------|-------|--------|-----|--------|					
|ADC0|0x0702|0x0308|RO||		
|BASE|0x0702|0x0000|RP|x|
|OPTARIF			|0xFF66|	0x0000|		RO||			
|ISOUSC			|0x0B01	|0x000D		|RO		||	
|HCHC			|0x0702	|0x0102		|RO			|x|
|HCHP			|0x0702	|0x0100		|RO			|x|
|EJPHN			|0x0702	|0x0100		|RO			|x|
|EJPHPM			|0x0702	|0x0102		|RO			|x|
|BBRHCJB			|0x0702	|0x0100		|RO			|x|
|BBRHPJB			|0x0702	|0x0102		|RO			|x|
|BBRHCJW			|0x0702	|0x0104		|RO			|x|
|BBRHPJW			|0x0702	|0x0106		|RO			|x|
|BBRHCJR			|0x0702	|0x0108		|RO			|x|
|BBRHPJR			|0x0702	|0x010A		|RO			|x|
|IINST			|0x0B04	|0x0508		|RP			||
|IINST1			|0x0B04	|0x0508		|RP			||
|IINST2			|0x0B04	|0x0908		|RP			||
|IINST3			|0x0B04	|0x0A08		|RP			||
|IMAX			|0x0B04	|0x050A		|RO			||
|IMAX1			|0x0B04	|0x050A		|RO			||
|IMAX2			|0x0B04	|0x090A		|RO			||
|IMAX3			|0x0B04	|0x0A0A		|RO			||
|PMAX			|0x0B04	|0x050D		|RO			||
|PAPP			|0x0B04	|0x050F		|RP			||
|PTEC			|0x0702	|0x0020		|RO	||
|			|0xFF66	|0x0100		|RW		|Change the periodic sending time based on 7 sec cycle. ex : value=1 → send every 7 sec. Value= 5 → send every 35 sec	etc|											
|DEMAIN			|0xFF66	|0x0001		|RO||
|HHPHC			|0xFF66	|0x0002		|RO||
|PPOT 			|0xFF66	|0x0003		|RO||
|PEJP			|0xFF66	|0x0004		|RO||
|ADPS			|0xFF66	|0x0005		|RO||			


## Changelog

### Version 4000-0001
Version initiale
