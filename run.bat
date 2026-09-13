@echo off
echo ==============================================
echo DANG KHOI DONG SERVER C++ TREN CONG 8080...
echo ==============================================

if exist "build\restaurant_api.exe" (
    echo [OK] Tim thay: build\restaurant_api.exe
    build\restaurant_api.exe
) else if exist "out\build\x64-Debug\restaurant_api.exe" (
    echo [OK] Tim thay: out\build\x64-Debug\restaurant_api.exe
    out\build\x64-Debug\restaurant_api.exe
) else if exist "out\build\x64-Release\restaurant_api.exe" (
    echo [OK] Tim thay: out\build\x64-Release\restaurant_api.exe
    out\build\x64-Release\restaurant_api.exe
) else if exist "build\Release\restaurant_api.exe" (
    echo [OK] Tim thay: build\Release\restaurant_api.exe
    build\Release\restaurant_api.exe
) else if exist "build\Debug\restaurant_api.exe" (
    echo [OK] Tim thay: build\Debug\restaurant_api.exe
    build\Debug\restaurant_api.exe
) else (
    echo [LOI] Khong tim thay restaurant_api.exe!
    echo Vui long chay .\build.bat truoc.
)
