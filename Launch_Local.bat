@echo off
setlocal

:: Chemin vers Unreal Engine (à adapter si besoin)
set UE="C:\Program Files\Epic Games\UE_5.6\Engine\Binaries\Win64\UnrealEditor.exe"

:: Récupère le dossier du .bat
set SCRIPT_DIR=%~dp0

:: Trouve automatiquement le .uproject dans ce dossier
for %%i in ("%SCRIPT_DIR%*.uproject") do set PROJECT=%%i

echo Projet detecte : %PROJECT%

:: Client 1
start "" %UE% "%PROJECT%" -game -log -windowed -ResX=960 -ResY=540 -WinX=0 -WinY=0 -port=7777

timeout /t 5 /nobreak

:: Client 2
start "" %UE% "%PROJECT%" -game -log -windowed -ResX=960 -ResY=540 -WinX=960 -WinY=0

timeout /t 5 /nobreak

:: Client 3
start "" %UE% "%PROJECT%" -game -log -windowed -ResX=960 -ResY=540 -WinX=0 -WinY=540

timeout /t 5 /nobreak

:: Client 4
start "" %UE% "%PROJECT%" -game -log -windowed -ResX=960 -ResY=540 -WinX=960 -WinY=540

endlocal