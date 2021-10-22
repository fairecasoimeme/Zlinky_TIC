#!/bin/sh

# $1 : the OTA build directory
# $2 : the SDK directory name
# $3 : the manufacturer code
# $4 : 32 byte OTA header string
# $5 : JET VERSION 4 - JN5169 and 5 for JN5179 and 6 for JN5189/K32W
# $6 : Jennic chip family like JN516x and JN517x or JN518x K32Wx
# $7 : OTA Device Id
# $8 : Target
# $9 : Key
# ${10} : file version to generate
#
# Change the path to the OTA Build folder.

# ####################################################################################################:
# ####################################Build Encrpted Client Binary##################################################:
cd $1

    # Add serialisation Data with ImageType = 0x0XXX - Indicates it is for Encrpted devices
    $2/JET.exe -m combine -f $8.bin -x configOTA_LinkKey.txt -g 1 -v $5 -u $3 -t $7 -j $4 -k $9 -i 0x00000010111213141516171800000000  --sector_size=512
if [ -z "${10}" ]
  then
      # Create an Unencrpted Bootable Client with Veriosn 1
      $2/JET.exe -m otamerge --embed_hdr -c outputffffffffffffffff.bin -o $8Client.bin -v $5 -n 1 -u $3 -t $7 -j $4 --sector_size=512
	  
      # ###################Build OTA Encrypted Upgarde Image from the Unencrypted Bootable Client Image #########################:
      # Modify Embedded Header to reflect version 2 
      $2/JET.exe -m otamerge --embed_hdr -c $8Client.bin -o $8UpGradeImagewithOTAHeaderV2.bin -v $5 -n 2 -u $3 -t $7 -j $4 --sector_size=512

      # Now Encrypt the above Version 2  
      $2/JET.exe -m bin -c $8Client.bin -f $8UpGradeImagewithOTAHeaderV2.bin -e $8UpGradeImagewithOTAHeaderV2_Enc.bin -v $5 -k $9 -i 0x00000010111213141516171800000000 -j $4 --sector_size=512

      # Wrap the Image with OTA header with version 2
      $2/JET.exe -m otamerge --ota -c $8UpGradeImagewithOTAHeaderV2_Enc.bin -o $8Client_UpgradeImagewithOTAHeaderV2_Enc.ota -p 1 -v $5 -n 2 -u $3 -t $7 -j $4 --sector_size=512

      # Modify Embedded Header to reflect version 3 
      $2/JET.exe -m otamerge --embed_hdr -c $8Client.bin -o $8UpGradeImagewithOTAHeaderV3.bin -v $5 -n 3 -u $3 -t $7 -j $4 --sector_size=512

      # Now Encrypt the above Version 3  
      $2/JET.exe -m bin -c $8Client.bin -f $8UpGradeImagewithOTAHeaderV3.bin -e $8UpGradeImagewithOTAHeaderV3_Enc.bin -v $5 -k $9 -i 0x00000010111213141516171800000000 -j $4 --sector_size=512

      # Wrap the Image with OTA header with version 3 
      $2/JET.exe -m otamerge --ota -c $8UpGradeImagewithOTAHeaderV3_Enc.bin -o $8Client_UpgradeImagewithOTAHeaderV3_Enc.ota -p 1 -v $5 -n 3 -u $3 -t $7 -j $4 --sector_size=512
else
    # ####################################################################################################
    # Create an Unencrpted Bootable Client with Veriosn 1
    $2/JET.exe -m otamerge --embed_hdr -c outputffffffffffffffff.bin -o $8Client.bin -v $5 -n ${10} -u $3 -t $7 -j $4 --sector_size=512
	# ###################Build OTA Encrypted Upgarde Image from the Unencrypted Bootable Client Image #########################:
    # Modify Embedded Header to reflect version passed 
    $2/JET.exe -m otamerge --embed_hdr -c $8Client.bin -o $8UpGradeImagewithOTAHeaderV${10}.bin -v $5 -n ${10} -u $3 -t $7 -j $4 --sector_size=512
    # Now Encrypt the above Version   
    $2/JET.exe -m bin -c $8Client.bin -f $8UpGradeImagewithOTAHeaderV${10}.bin -e $8UpGradeImagewithOTAHeaderV${10}_Enc.bin -v $5 -k $9 -i 0x00000010111213141516171800000000 -j $4 --sector_size=512

    # Wrap the Image with OTA header with version passed to generate an OTA file of the encrypted upgrade
    $2/JET.exe -m otamerge --ota -c $8UpGradeImagewithOTAHeaderV${10}_Enc.bin -o $8Client_UpgradeImagewithOTAHeaderV${10}_Enc.ota -p 1 -v $5 -n ${10} -u $3 -t $7 -j $4 --sector_size=512

    #Wrap the Image with OTA header with version passed to generate an OTA file of the unencrypted upgrade
    $2/JET.exe -m otamerge --ota -c $8UpGradeImagewithOTAHeaderV${10}.bin -o $8Client_UpgradeImagewithOTAHeaderV${10}_Unenc.ota -p 1 -v $5 -n ${10} -u $3 -t $7 -j $4 --sector_size=512


# ####################################################################################################
# #################################### Clean Up Imtermediate files##################################################
fi

# ####################################################################################################:
# #################################### Clean Up Imtermediate files##################################################:
rm output*.bin
rm $8UpGradeImagewithOTAHeader*.bin

chmod 777 $8Client.bin
