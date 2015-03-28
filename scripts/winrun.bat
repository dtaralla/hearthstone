@echo off
setlocal enableDelayedExpansion

:: Display the output of each process if the /O option is used
:: else ignore the output of each process
if /i "%~1" equ "/O" (
  set "lockHandle=1"
  set "showOutput=1"
) else (
  set "lockHandle=1^>nul 9"
  set "showOutput="
)

:: Define the maximum number of parallel processes to run.
:: Each process number can optionally be assigned to a particular server
:: and/or cpu via psexec specs (untested).
set "maxProc=128"

:: Optional - Define CPU targets in terms of PSEXEC specs
::           (everything but the command)
::
:: If a cpu is not defined for a proc, then it will be run on the local machine.
:: I haven't tested this feature, but it seems like it should work.
::
:: set cpu1=psexec \\server1 ...
:: set cpu2=psexec \\server1 ...
:: set cpu3=psexec \\server2 ...
:: etc.

:: For this demo force all cpu specs to undefined (local machine)
for /l %%N in (1 1 %maxProc%) do set "cpu%%N="

:: Get a unique base lock name for this particular instantiation.
:: Incorporate a timestamp from WMIC if possible, but don't fail if
:: WMIC not available. Also incorporate a random number.
  set "lock="
  for /f "skip=1 delims=-+ " %%T in ('2^>nul wmic os get localdatetime') do (
    set "lock=%%T"
    goto :break
  )
  :break
  set "lock=%temp%\lock%lock%_%random%_"

:: Initialize the counters
  set /a "startCount=0, endCount=0"

:: Clear any existing end flags
  for /l %%N in (1 1 %maxProc%) do set "endProc%%N="

:: Launch the commands in a loop
  set launch=1
  for /l %%i in (%2 1 %3) do (
  ::for /f "tokens=* delims=:" %%A in ('findstr /b ":::" "%~f0"') do (
    if !startCount! lss %maxProc% (
      set /a "startCount+=1, nextProc=startCount"
    ) else (
      call :wait
    )
    set cmd!nextProc!=step %%i
    if defined showOutput echo -------------------------------------------------------------------------------
    echo !time! - proc!nextProc!: starting step %%i
    2>nul del %lock%!nextProc!
    %= Redirect the lock handle to the lock file. The CMD process will     =%
    %= maintain an exclusive lock on the lock file until the process ends. =%
    start /b "" /D ..\ cmd /c %lockHandle%^>"%lock%!nextProc!" 2^>^&1 !cpu%%N! release\hsdatabasegenerator.exe classic_cardlist_light MAGE MAGE %%i
  )
  set "launch="

:wait
:: Wait for procs to finish in a loop
:: If still launching then return as soon as a proc ends
:: else wait for all procs to finish
  :: redirect stderr to null to suppress any error message if redirection
  :: within the loop fails.
  for /l %%N in (1 1 %startCount%) do 2>nul (
    %= Redirect an unused file handle to the lock file. If the process is    =%
    %= still running then redirection will fail and the IF body will not run =%
    if not defined endProc%%N if exist "%lock%%%N" 9>>"%lock%%%N" (
      %= Made it inside the IF body so the process must have finished =%
      if defined showOutput echo ===============================================================================
      echo !time! - proc%%N: finished !cmd%%N!
      if defined showOutput type "%lock%%%N"
      if defined launch (
        set nextProc=%%N
        exit /b
      )
      set /a "endCount+=1, endProc%%N=1"
    )
  )
  if %endCount% lss %startCount% (
    1>nul 2>nul ping /n 2 ::1
    goto :wait
  )

2>nul del %lock%*
if defined showOutput echo ===============================================================================
echo FINISHED