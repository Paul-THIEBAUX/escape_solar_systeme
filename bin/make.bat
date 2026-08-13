@echo off 

SolutionGenerator.exe -bp -make

copy /Y "..\lib\SDL3\lib\x64\*.dll" "..\ide\gcle\"

copy /Y "..\lib\SDL3_ttf\lib\x64\*.dll" "..\ide\gcle\"

copy /Y "..\lib\SDL3_Image\lib\x64\*.dll" "..\ide\gcle\"
copy /Y "..\lib\SDL3_Image\lib\x64\optional\*.dll" "..\ide\gcle\"

copy /Y "..\lib\SDL3_mixer\lib\x64\*.dll" "..\ide\gcle\"
copy /Y "..\lib\SDL3_mixer\lib\x64\optional\*.dll" "..\ide\gcle\"

pause