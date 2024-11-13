// ~~~~ HW Pins and States ~~~~
#define DELAY_TO_REACT 1500 // millis
#define STATE_ON HIGH
#define SYSTEM_STATE_ON LOW
#define SYSTEM_STATE_ARM_PIN D1    //  Indication system is Armed
#define SYSTEM_STATE_ALARM_PIN D2  //  Indication system is Alarmed
#define CONTROLLER_ARM_HOME_PIN D5 //   (Set system)  armed_Home
#define CONTROLLER_ARM_AWAY_PIN D6 //   (Set system)  Armed_Away

const uint8_t deBounceInt = 50;

const char *VER = "alarmMon_5.0";
struct AlarmSystemState
{
    bool is_armed = false;
    bool is_alarming = false;
} alarmsysState;

struct ContollertState
{
    bool is_armed_home = false;
    bool is_armed_away = false;
} controllerState;

const char *sys_states[5] = {
    "disarmed",
    "armed_home",
    "armed_away",
    "pending",
    "triggered"};

enum sys_state : const uint8_t
{
    DISARMED,
    ARMED_HOME_CODE,
    ARMED_AWAY_CODE,
    ARMED_KEYPAD,
    ALARMING,
    ARM_ERR,
};