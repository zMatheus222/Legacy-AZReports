@REM Define o nome do arquivo de origem (main.cpp)
@set source_file=main.cpp

@REM Define o nome do arquivo executável (AZReports.exe)
@set executable_file=AZReports.exe

@REM Comando para compilar o programa C++
@g++ %source_file% data/icon/icon-resource.o -o %executable_file%

@REM Verifica se a compilação foi bem-sucedida
@if %errorlevel% equ 0 (
    echo Compilação bem-sucedida. Rodando o programa...
    start %executable_file%
) else (
    echo Erro durante a compilação. Verifique o código fonte.
)

@REM Pausa para que você possa ver as mensagens antes de fechar a janela
@pause