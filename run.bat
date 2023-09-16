REM  Define o nome do arquivo de origem (substitua "seu_programa.cpp" pelo nome do seu arquivo)
set source_file=main.cpp

REM  Define o nome do arquivo executável (geralmente o mesmo nome do arquivo de origem sem a extensão .cpp)
set executable_file=AZReports.exe

REM  Comando para compilar o programa C++
g++ %source_file% data\icon\resource.o -o %executable_file%

REM  Verifica se a compilação foi bem-sucedida
if %errorlevel% equ 0 (
    echo Compilação bem-sucedida. Rodando o programa...
    %executable_file%
) else (
    echo Erro durante a compilação. Verifique o código fonte.
)

pause