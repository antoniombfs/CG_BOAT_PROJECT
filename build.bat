@echo off
echo ========================================
echo    Boat Renderer - Build Script
echo ========================================
echo.

REM Criar pasta build
if not exist "build" mkdir build

cd build

echo [1/3] Configurando CMake...
cmake .. -G "MinGW Makefiles"
if %errorlevel% neq 0 (
    echo.
    echo ERRO: Falha ao configurar CMake!
    echo Verifica se o CMake esta instalado e no PATH
    pause
    exit /b %errorlevel%
)

echo.
echo [2/3] Compilando projeto...
mingw32-make
if %errorlevel% neq 0 (
    echo.
    echo ERRO: Falha ao compilar!
    pause
    exit /b %errorlevel%
)

echo.
echo [3/3] Compilacao concluida com sucesso!
echo.
echo ========================================
echo Executavel: build\BoatRenderer.exe
echo.
echo Para executar:
echo   cd build
echo   BoatRenderer.exe
echo ========================================
echo.
pause