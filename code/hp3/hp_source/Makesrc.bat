del Zips\hp_source.zip
pkzip25 -add -dir -rec -exc=*.bak -exc=*.obj -exc=*.exe -exc=*.lib -exc=Zips/*.* -exc=Release/*.* -exc=Debug/*.* -exc=*/Release/*.* -exc=*/Debug/*.* -exc=*.htm -exc=*.bsc -exc=*.ilk -exc=*.pdb -exc=*.pch  -exc=*.sbr -exc=*.aps -exc=*.ncb Zips\hp_source.zip *.*
