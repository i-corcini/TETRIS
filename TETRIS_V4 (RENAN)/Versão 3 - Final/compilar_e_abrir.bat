@echo off
setlocal
cd /d "%~dp0"
where gcc >nul 2>&1
if errorlevel 1 (
  echo ERRO: GCC nao encontrado no PATH. Instale GCC via MSYS2 UCRT64.
  pause
  exit /b 1
)
echo Compilando Tetris V3...
gcc -std=c11 -Wall -Wextra main.c tabuleiro.c tetraminos.c movimentacao.c pontuacao.c gravidade.c efeitos.c sons.c window.c -o tetris_v3_final.exe -lallegro_acodec -lallegro_audio -lallegro_ttf -lallegro_font -lallegro_primitives -lallegro -lm
if errorlevel 1 (
  echo Erro de compilacao. Confira a instalacao de Allegro e MSYS2.
  pause
  exit /b 1
)
echo Executando o jogo...
.\tetris_v3_final.exe
if errorlevel 1 pause
