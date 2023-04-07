#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <freertos/queue.h>
extern int lastMode;
extern int currentMode;
extern TaskHandle_t colecHandle;
extern TaskHandle_t penetHandle;
extern TaskHandle_t breakHandle;
extern TaskHandle_t alarmHandle;
extern TaskHandle_t scaleHandle;
extern TaskHandle_t resetHandle;
extern TaskHandle_t mpuHandle;
// extern TaskHandle_t scrtyHandle;
// Create a handle for the queue
extern QueueHandle_t xQueuePenet;
extern QueueHandle_t xQueueScale;
int updateMode(int newMode);
int getCurrentMode();
int getLastMode();
void enableModeTasks(int mode);