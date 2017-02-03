rem
rem     Make "Full" distribution in directory relfull
rem
rem     Note that it's up to you to make sure "mapbits"
rem     and "starmaps" were used to build the DLLs in the
rem     release directory; if the Lite versions were the last
rem     built this script will blindly use them.
rem
rm -rf relfull
mkdir relfull
copy release\HomePlanet.exe relfull
copy release\*.dll relfull
copy release\astrelem.exe relfull
copy release\cometel.exe relfull
copy *.csv relfull
copy hpdde.xls relfull
rem Delete Lite version of objects.csv
del relfull\objlite.csv
copy *.hlp relfull
copy sitename.txt relfull
rem Include full satellites database
copy satelite.sat relfull
rem Transfer image and sound directories 
mkdir relfull\images
copy images\*.* relfull\images
mkdir relfull\sounds
copy sounds\*.* relfull\sounds
mkdir relfull\tools
copy tools\astrelem.c relfull\tools
copy tools\cometel.c relfull\tools
del zips\hp3full.zip
cd relfull
pkzip25 -add -director ..\zips\hp3full.zip *.*
cd ..
rm -rf relfull
