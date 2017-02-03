rem
rem     Make "Lite" distribution in directory rellite
rem
rem     Note that it's up to you to make sure "map_lite"
rem     and "StarLight" were used to build the DLLs in the
rem     release directory; if the full versions were the last
rem     built this script will blindly used them.
rem
rm -rf rellite
mkdir rellite
copy release\HomePlanet.exe rellite
copy release\*.dll rellite
copy release\astrelem.exe rellite
copy release\cometel.exe rellite
copy *.csv rellite
copy hpdde.xls rellite
rem Replace objects.csv with Lite version
copy objlite.csv rellite\objects.csv
rem Delete large .CSV databases not included in Lite
del rellite\astnum?.csv
del rellite\astuname.csv
del rellite\poss.csv
del rellite\spacecrf.csv
copy *.hlp rellite
copy sitename.txt rellite
rem Include only TVRO satellites in Lite
copy tvro.sat rellite\satelite.sat
mkdir rellite\tools
copy tools\astrelem.c rellite\tools
copy tools\cometel.c rellite\tools
del zips\hp3lite.zip
cd rellite
pkzip25 -add -director ..\zips\hp3lite.zip *.*
cd ..
rm -rf rellite
