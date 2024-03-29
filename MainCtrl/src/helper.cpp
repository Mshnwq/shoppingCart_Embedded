#include "helper.h"
#include "lock/lock.h"
// Task modes
int lastMode = 0;
int currentMode = 0;
int updateMode(int newMode){
    // if(lastMode == currentMode)
    //   return -1;
    lastMode = currentMode;
    currentMode = newMode;
    enableModeTasks(newMode);
    return currentMode;
}
void enableModeTasks(int mode){
    switch (mode) {
      case 0:
        // locked tasks
        // vTaskResume(sleepHandle);
        workTime = time(NULL);
        // vTaskSuspend(scaleHandle);
        // vTaskSuspend(alarmHandle);
        // vTaskSuspend(penetHandle);
        // vTaskSuspend(resetHandle);
        // vTaskSuspend(mpuHandle);
        activateLock();
        deactivateBuz();
        break;
      case 1:
        // active tasks
        // vTaskSuspend(resetHandle);
        // vTaskSuspend(scaleHandle);
        // vTaskSuspend(alarmHandle);
        // vTaskSuspend(mpuHandle);
        // vTaskResume(penetHandle);
        deactivateLockWithBuz();
        break;
      case 2:
        // weighing tasks
        activateLock();
        deactivateBuz();
        // vTaskResume(penetHandle);
        // vTaskResume(scaleHandle);
        // vTaskResume(resetHandle);
        break;
        case 3:
          // weighing tasks
          deactivateLockWithBuz();
          // vTaskResume(penetHandle);
          // vTaskResume(scaleHandle);
          // vTaskResume(resetHandle);
          break;
        case 4:
          // weighing tasks
          deactivateLockWithBuz();
          // vTaskResume(penetHandle);
          // vTaskResume(scaleHandle);
          // vTaskResume(resetHandle);
          break;
        case 5:
          // alarm tasks
          activateLockWithBuz();
          break;
        case 6:
          // paid tasks
          // vTaskSuspend(resetHandle);
          // vTaskSuspend(scaleHandle);
          // vTaskSuspend(breakHandle);
          // vTaskSuspend(alarmHandle);
          // vTaskSuspend(resetHandle);
          // vTaskSuspend(mpuHandle);
          // vTaskResume(breakHandle);
          break;
    }
}
int getCurrentMode(){
  return currentMode;
}
