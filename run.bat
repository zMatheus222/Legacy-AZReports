REM Comando para compilar o programa C++
g++ main.cpp data/icon/icon-resource.o -o AZReports.exe

REM Verifica se a compilação foi bem-sucedida
if %errorlevel% equ 0 (
    echo Compilação bem-sucedida. Rodando o programa...
    start AZReports.exe
) else (
    echo Erro durante a compilação. Verifique o código fonte.
)

@REM Pausa para que você possa ver as mensagens antes de fechar a janela
@pause