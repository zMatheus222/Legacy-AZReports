@echo off
echo Compilando e rodando o programa C++...

rem Define o nome do arquivo de origem (substitua "seu_programa.cpp" pelo nome do seu arquivo)
set source_file=main.cpp

rem Define o nome do arquivo executável (geralmente o mesmo nome do arquivo de origem sem a extensão .cpp)
set executable_file=AZReports.exe

rem Comando para compilar o programa C++
g++ %source_file% -o %executable_file%

rem Verifica se a compilação foi bem-sucedida
if %errorlevel% equ 0 (
    echo Compilação bem-sucedida. Rodando o programa...
    %executable_file%
) else (
    echo Erro durante a compilação. Verifique o código fonte.
)

pause