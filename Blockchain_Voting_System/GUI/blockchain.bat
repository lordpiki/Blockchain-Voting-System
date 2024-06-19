@echo off

if "%1" equ "run" (
    echo Running client...
    python app.py
) else if "%1" equ "servers" (
    echo Running servers...
    start cmd /k python ../Communicator/DNS_Server/DNS_Server.py
    start cmd /k python ../Communicator/Government/Government.py
) else if "%1" equ "del" (
    echo Deleting logs
    del /s /q *er.txt
) else (
    echo Enter 'run' to launch client, 'del' to delete logs, or 'servers' to launch servers
)