@echo off

::
:: solo
::
  :: family 15h
    perl "%CD%\AMDTools\ArchV9\ArchV9.pl" -o Br   -p Am4   -i "%CD%\AMDTools\ArchV9\Config" -e "%CD%"

  :: family 17h, models 00 - 0F
    perl "%CD%\AMDTools\ArchV9\ArchV9.pl" -o Sm   -p Am4   -i "%CD%\AMDTools\ArchV9\Config" -e "%CD%"
    perl "%CD%\AMDTools\ArchV9\ArchV9.pl" -o Sm   -p Sp4   -i "%CD%\AMDTools\ArchV9\Config" -e "%CD%"
    perl "%CD%\AMDTools\ArchV9\ArchV9.pl" -o Zp   -p Sp3   -i "%CD%\AMDTools\ArchV9\Config" -e "%CD%"
    perl "%CD%\AMDTools\ArchV9\ArchV9.pl" -o Zp   -p Sp4   -i "%CD%\AMDTools\ArchV9\Config" -e "%CD%"
    perl "%CD%\AMDTools\ArchV9\ArchV9.pl" -o Tr   -p Sp3   -i "%CD%\AMDTools\ArchV9\Config" -e "%CD%"

  :: family 17h, models 10 - 2F
    perl "%CD%\AMDTools\ArchV9\ArchV9.pl" -o Rv   -p Am4   -i "%CD%\AMDTools\ArchV9\Config" -e "%CD%"
    perl "%CD%\AMDTools\ArchV9\ArchV9.pl" -o Rv   -p Fp5   -i "%CD%\AMDTools\ArchV9\Config" -e "%CD%"

  :: family 17h, models 30 - 3F
    perl "%CD%\AMDTools\ArchV9\ArchV9.pl" -o Ssp  -p Sp3   -i "%CD%\AMDTools\ArchV9\Config" -e "%CD%"
    perl "%CD%\AMDTools\ArchV9\ArchV9.pl" -o Cp   -p Sp3   -i "%CD%\AMDTools\ArchV9\Config" -e "%CD%"

  :: family 17h, models 60 - 6F
    perl "%CD%\AMDTools\ArchV9\ArchV9.pl" -o Rn   -p Am4   -i "%CD%\AMDTools\ArchV9\Config" -e "%CD%"
    perl "%CD%\AMDTools\ArchV9\ArchV9.pl" -o Rn   -p Fp6   -i "%CD%\AMDTools\ArchV9\Config" -e "%CD%"

  :: family 17h, models 70 - 7F
    perl "%CD%\AMDTools\ArchV9\ArchV9.pl" -o Mts  -p Am4   -i "%CD%\AMDTools\ArchV9\Config" -e "%CD%"

  :: family 17h, models 20h-2Fh
    perl "%CD%\AMDTools\ArchV9\ArchV9.pl" -o Rv  -p Ft5   -i "%CD%\AMDTools\ArchV9\Config" -e "%CD%"

::
:: combo
::
  :: AM4 combo
    :: 2 combo
      perl "%CD%\AMDTools\ArchV9\ArchV9.pl" -o "Sm|Br"                      -p Am4   -i "%CD%\AMDTools\ArchV9\Config" -e "%CD%"
      perl "%CD%\AMDTools\ArchV9\ArchV9.pl" -o "Rv|Br"                      -p Am4   -i "%CD%\AMDTools\ArchV9\Config" -e "%CD%"
      perl "%CD%\AMDTools\ArchV9\ArchV9.pl" -o "Rv|Sm"                      -p Am4   -i "%CD%\AMDTools\ArchV9\Config" -e "%CD%"

    :: 3 combo
      perl "%CD%\AMDTools\ArchV9\ArchV9.pl" -o "Rv|Sm|Br"                   -p Am4   -i "%CD%\AMDTools\ArchV9\Config" -e "%CD%"
      perl "%CD%\AMDTools\ArchV9\ArchV9.pl" -o "Mts|Rv|Sm"                  -p Am4   -i "%CD%\AMDTools\ArchV9\Config" -e "%CD%"
    :: All combo
      perl "%CD%\AMDTools\ArchV9\ArchV9.pl" -o "Rn|Mts|Rv|Sm|Br"            -p Am4   -i "%CD%\AMDTools\ArchV9\Config" -e "%CD%"


  :: SP3 combo
    :: All combo
      perl "%CD%\AMDTools\ArchV9\ArchV9.pl" -o "Ssp|Zp"                     -p Sp3   -i "%CD%\AMDTools\ArchV9\Config" -e "%CD%"

  :: SP3r2 combo
    :: All combo
      perl "%CD%\AMDTools\ArchV9\ArchV9.pl" -o "Cp|Tr"                      -p Sp3   -i "%CD%\AMDTools\ArchV9\Config" -e "%CD%"

@echo on
