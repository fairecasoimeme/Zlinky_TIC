set script_path=%0
set file=%1
set stated_size=%2
set app_id=%3
set python_script=%script_path:dk6_iarpostbuild.bat=dk6_image_tool.py%

if "%stated_size%"=="" (
  py -3 %python_script% %file%
) else (
  if "%app_id%" == "" (
    py -3 %python_script% -s %stated_size% %file%
  ) else (
    py -3 %python_script% -s %stated_size% -i %app_id% %file%
  )
)

arm-none-eabi-objcopy -O binary %file% %file%.bin