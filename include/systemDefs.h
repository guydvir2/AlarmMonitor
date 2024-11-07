// ~~~~ HW Pins and States ~~~~
#define DELAY_TO_REACT 3000 // millis
#define STATE_ON HIGH
#define SYSTEM_STATE_ON LOW
#define SYSTEM_STATE_ARM_PIN D3      //  Indication system is Armed
#define SYSTEM_STATE_ALARM_PIN D4    //  Indication system is Alarmed
#define SET_SYSTEM_ARMED_HOME_PIN D5 //   (Set system)  armed_Home
#define SET_SYSTEM_ARMED_AWAY_PIN D6 //   (Set system)  Armed_Away

const char *VER = "alarmMon_4.1";
const char *sys_states[5] = {
    "armed_home",
    "armed_away",
    "disarmed",
    "pending",
    "triggered"};
enum sys_state : const uint8_t
{
    ARMED_HOME_CODE,
    ARMED_AWAY_CODE,
    DISARMED,
    ARMED_KEYPAD,
    ALARMING,
    ARM_ERR,
};