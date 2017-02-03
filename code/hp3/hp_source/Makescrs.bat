rem
rem     Make release archive for screen saver.  Assumes
rem     you've already built hpssave.exe in the release
rem     directory.
rem 
del zips\hp3ssave.zip
cd release
ren hpssave.exe HomePlanet.scr
pkzip25 -add ..\Zips\hp3ssave.zip HomePlanet.scr
ren HomePlanet.scr hpssave.exe
cd ..
pkzip25 -test Zips\hp3ssave.zip
pkzip25 -view Zips\hp3ssave.zip
