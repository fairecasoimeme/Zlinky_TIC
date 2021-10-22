set script_path=%0
set file=%1
set python_script=%script_path:dk6_iarpostbuild_ssbl.bat=dk6_image_tool.py%

arm-none-eabi-size %file%

py -3 %python_script% -g test_es2_sign -c -k -i=0 %file%

arm-none-eabi-objcopy -O binary %file% %file%.bin