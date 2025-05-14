#include "types.h"
#include "stat.h"
#include "user.h"
int main(int argc, char *argv[])
{
    if(argc<2){
        printf(1,"please enter the correct input\n");
        exit();
    }
    int a=0;
    a=atoi(argv[1]);
    printf(1, "pid  %d time %d\n", a, ticks_running(a));
    exit();
}