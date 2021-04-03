if (!(Test-Path ./bin)) { New-Item -Type Directory "bin" }
clang main.c -o bin/main.exe -g