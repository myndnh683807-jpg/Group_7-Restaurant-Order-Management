@echo off
echo ==============================================
echo [0/3] DON DEP CACHE CU...
echo ==============================================
if exist build rmdir /s /q build

echo.
echo ==============================================
echo [1/3] THIET LAP MOI TRUONG MSVC...
echo ==============================================
call "C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Auxiliary\Build\vcvars64.bat"

echo.
echo ==============================================
echo [2/3] DANG CAU HINH DU AN (CMAKE + Ninja)...
echo ==============================================
"C:\Program Files\Microsoft Visual Studio\2022\Community\Common7\IDE\CommonExtensions\Microsoft\CMake\CMake\bin\cmake.exe" -B build -G "Ninja" -DCMAKE_BUILD_TYPE=Release -DCMAKE_CXX_COMPILER="cl.exe"

echo.
echo ==============================================
echo [3/3] DANG BIEN DICH RA FILE CHAY (.EXE)...
echo ==============================================
"C:\Program Files\Microsoft Visual Studio\2022\Community\Common7\IDE\CommonExtensions\Microsoft\CMake\CMake\bin\cmake.exe" --build build --config Release

echo.
if exist "build\restaurant_api.exe" (
    echo ==============================================
    echo HOAN TAT THANH CONG! File: build\restaurant_api.exe
    echo DE CHAY SERVER, HAY GO LENH: .\run.bat
    echo ==============================================
) else (
    echo ==============================================
    echo [LOI] Build that bai! Vui long doc lai thong bao loi o tren.
    echo ==============================================
)
