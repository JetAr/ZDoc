rem
rem     Make "Upgrade" distribution in directory relupg
rem
rem     The directory tree created by this script will,
rem     when extracted on top of the Lite version, upgrade
rem     it to the Full release.
rem
rem     Note that it's up to you to make sure "mapbits"
rem     and "starmaps" were used to build the DLLs in the
rem     release directory; if the Lite versions were the last
rem     built this script will blindly use them.
rem
rm -rf relupg
mkdir relupg
copy release\mapbits.dll relupg
copy release\starmaps.dll relupg
copy *.csv relupg
rem Delete Lite version of objects.csv
del relupg\objlite.csv
copy sitename.txt relupg
rem Include full satellites database
copy satelite.sat relupg
rem Transfer image and sound directories 
mkdir relupg\images
copy images\*.* relupg\images
mkdir relupg\sounds
copy sounds\*.* relupg\sounds
del zips\hp3upg.zip
cd relupg
pkzip25 -add -director ..\zips\hp3upg.zip *.*
cd ..
rm -rf relupg
