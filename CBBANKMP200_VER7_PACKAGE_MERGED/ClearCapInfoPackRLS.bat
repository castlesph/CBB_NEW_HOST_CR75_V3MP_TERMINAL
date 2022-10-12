cd /d %~dp0
del .\Release\multiapp.mmci
del /s .\Release\capinfo.txt
7z a Release.zip .\Release\*
pause