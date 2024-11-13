#include <Arduino.h>
#include <myIOT2.h>
#include "systemDefs.h"
#include "myIOT_settings.h"

bool first_boot_notification = true;
uint8_t atBoot_systemState = 0;
void startGPIOs()
{
        pinMode(CONTROLLER_ARM_HOME_PIN, OUTPUT);      /* Pin to switch system state*/
        pinMode(CONTROLLER_ARM_AWAY_PIN, OUTPUT);      /* Pin to switch system state*/
        pinMode(SYSTEM_STATE_ARM_PIN, INPUT_PULLUP);   /* Pin to get system state*/
        pinMode(SYSTEM_STATE_ALARM_PIN, INPUT_PULLUP); /* Pin to get system state*/

        // digitalWrite(CONTROLLER_ARM_HOME_PIN, !STATE_ON);
        // digitalWrite(CONTROLLER_ARM_AWAY_PIN, !STATE_ON);
}
bool check_alarmstate_alarming()
{
        return digitalRead(SYSTEM_STATE_ALARM_PIN) == SYSTEM_STATE_ON;
}
bool check_alarmstate_armed()
{
        return digitalRead(SYSTEM_STATE_ARM_PIN) == SYSTEM_STATE_ON;
}
bool check_armed_home()
{
        return digitalRead(CONTROLLER_ARM_HOME_PIN) == STATE_ON;
}
bool check_armed_away()
{
        return digitalRead(CONTROLLER_ARM_AWAY_PIN) == STATE_ON;
}
void update_startup_systemState()
{
        alarmsysState.is_armed = check_alarmstate_armed();
        alarmsysState.is_alarming = check_alarmstate_alarming();
        atBoot_systemState = get_system_stateCode();
}

bool check_stateChange(bool (*func)(), bool &_controllerState)
{
        if (func() != _controllerState)
        {
                delay(deBounceInt);
                if (func() != _controllerState)
                {
                        _controllerState = !_controllerState;
                        return true;
                }
                else
                {
                        return false;
                }
        }
        else
        {
                return false;
        }
}
void set_disarm()
{ /*
  When System was armed using Keypad,
  in order to disarmed using controller,
  you need to arm using controller and then disarm.
 */
        if (get_system_stateCode() == ARMED_KEYPAD)
        {
                set_arm_withCode(ARMED_HOME_CODE);
                delay(DELAY_TO_REACT); // time to AlarmSystem to react.
        }
        digitalWrite(CONTROLLER_ARM_HOME_PIN, !STATE_ON);
        digitalWrite(CONTROLLER_ARM_AWAY_PIN, !STATE_ON);
}
bool set_arm_withCode(uint8_t _armType)
{
        if (get_system_stateCode() != _armType)
        {
                pub_ControllerCMD(_armType);
                if (_armType == ARMED_HOME_CODE || _armType == ARMED_AWAY_CODE)
                {

                        if (_armType == ARMED_HOME_CODE)
                        {
                                digitalWrite(CONTROLLER_ARM_HOME_PIN, STATE_ON);
                        }
                        else
                        {
                                digitalWrite(CONTROLLER_ARM_AWAY_PIN, STATE_ON);
                        }
                        return true;
                }
                else if (_armType == DISARMED)
                {
                        set_disarm();
                        return true;
                }
                else
                {
                        return false;
                }
        }
        else
        {
                return false;
        }
}

uint8_t get_system_stateCode()
{
        if (alarmsysState.is_alarming)
        {
                return ALARMING;
        }
        else if (alarmsysState.is_armed)
        {
                if (controllerState.is_armed_home)
                {
                        return ARMED_HOME_CODE;
                }
                else if (controllerState.is_armed_away)
                {
                        return ARMED_AWAY_CODE;
                }
                else if (!controllerState.is_armed_away && !controllerState.is_armed_home)
                {
                        return ARMED_KEYPAD;
                }
                else
                {
                        return ARM_ERR;
                }
        }
        else
        {
                return DISARMED;
        }
}
void readContoller_looper()
{
        check_stateChange(check_armed_away, controllerState.is_armed_away);
        check_stateChange(check_armed_home, controllerState.is_armed_home);
        // if (check_stateChange(check_armed_away, controllerState.is_armed_away))
        // {
        //         controllerState.is_armed_away = check_armed_away();
        // }
        // if (check_stateChange(check_armed_home, controllerState.is_armed_home))
        // {
        //         controllerState.is_armed_home = check_armed_home();
        // }
}
void readalarmSystem_looper()
{
        bool state_change = false;
        if (check_stateChange(check_alarmstate_alarming, alarmsysState.is_alarming))
        {
                state_change = true;
        }
        if (check_stateChange(check_alarmstate_armed, alarmsysState.is_armed))
        {
                state_change = true;
                readContoller_looper(); // in case state happens only in this loop.
                if (alarmsysState.is_armed == false)
                {
                        set_disarm();
                }
        }

        if (state_change)
        {

                uint8_t stateCode = get_system_stateCode();
                pub_stateTopic(stateCode);
                pub_AlarmSystemState(stateCode);
                atBoot_systemState = stateCode;
        }
}
void setup()
{
        startGPIOs();
        update_startup_systemState();
        startIOTservices();
}
void loop()
{
        iot.looper();
        readContoller_looper();
        readalarmSystem_looper();

        if (iot.isMqttConnected() && first_boot_notification)
        {
                pub_stateTopic(atBoot_systemState); /* in case of reset - update State */
                first_boot_notification = false;
        }
}

// bool set_armHome()
// {
//         if (!alarmsysState.is_armed)
//         {
//                 digitalWrite(CONTROLLER_ARM_HOME_PIN, STATE_ON);
//                 return true;
//         }
//         else
//         {
//                 return false;
//         }
// }
// void set_armAway()
// {
//         digitalWrite(CONTROLLER_ARM_AWAY_PIN, STATE_ON);
// }

// uint8_t get_systemState()
// {
//         /* Alarm can be in one of three state:
//         Alarming, Armed, disarmed
//          */
//         if (get_system_state_is_alarming()) /* Check if alarm pin is ON */
//         {
//                 return ALARMING;
//         }
//         else if (get_system_state_is_armed()) /* Check if Armed pin is ON */
//         {
//                 if (get_isArmedHome() && !get_isArmedAway()) /* set by code TO home_Armed ?*/
//                 {
//                         return ARMED_HOME_CODE;
//                 }
//                 else if (get_isArmedAway() && !get_isArmedHome()) /* set by code TO away_Armed ?*/
//                 {
//                         return ARMED_AWAY_CODE;
//                 }
//                 else if (get_isArmedAway() && get_isArmedHome())
//                 {
//                         return ARM_ERR;
//                 }
//                 else
//                 {
//                         return ARMED_KEYPAD; /* armed, but without ext inputs, so it is keypad */
//                 }
//         }
//         else
//         {
//                 return DISARMED;
//         }
// }
// void set_armState(uint8_t req_state)
// {
//         char a[50];
//         uint8_t curState = get_systemState();
//         if (req_state != curState)
//         {
//                 if (req_state == DISARMED) /* set_disarm when armed using keybaord */
//                 {
//                         if (curState == ARMED_KEYPAD)
//                         {
//                                 /* Armed by keypad. Can't set_disarm without any of setter set to arm.
//                                 Have to fake an input by system in order to turn off */
//                                 set_armHome();
//                                 delay(DELAY_TO_REACT); // Time for system to react to fake state change
//                         }
//                         set_disarm();
//                         sprintf(a, "System change: [disarmed] using [Code]");
//                         iot.pub_msg(a);
//                 }
//                 else if (req_state == ARMED_HOME_CODE)
//                 {
//                         if (curState != DISARMED)
//                         {
//                                 set_disarm();
//                         }
//                         set_armHome();
//                 }
//                 else if (req_state == ARMED_AWAY_CODE)
//                 {
//                         if (curState != DISARMED)
//                         {
//                                 set_disarm();
//                         }
//                         set_armAway();
//                 }
//         }
//         else
//         {
//                 sprintf(a, "System No-change: already in [%s] state", sys_states[curState]);
//                 iot.pub_msg(a);
//         }
// }

// void check_systemState_armed()
// {
//         if (check_systemState_change(SYSTEM_STATE_ARM_PIN, indication_ARMED_lastState))
//         {
//                 char a[30];
//                 uint8_t curState = get_systemState();
//                 pub_stateTopic(curState);
//                 sprintf(a, "System state: [%s]", sys_states[curState]);
//                 iot.pub_msg(a);

//                 if (curState == DISARMED) /* Verify all inputs are OFF */
//                 {
//                         set_disarm();
//                 }
//                 else if (curState == ARMED_KEYPAD)
//                 {
//                         sprintf(a, "Keypad: [Armed]");
//                         iot.pub_msg(a);
//                 }
//                 else if (curState == ARMED_HOME_CODE || ARMED_AWAY_CODE)
//                 {
//                         sprintf(a, "Code: [Armed]");
//                         iot.pub_msg(a);
//                 }
//         }
// }
// void check_systemState_alarming()
// {
//         if (check_systemState_change(SYSTEM_STATE_ALARM_PIN, indication_ALARMED_lastState))
//         {
//                 uint8_t curState = get_systemState();
//                 pub_stateTopic(curState);

//                 if (curState == ALARMING)
//                 {
//                         iot.pub_msg("[Alarm] is triggered");
//                 }
//                 else
//                 {
//                         iot.pub_msg("[Alarm] stopped");
//                 }
//         }
// }

// bool get_isArmedHome()
// {
//         return digitalRead(CONTROLLER_ARM_HOME_PIN) == STATE_ON;
// }
// bool get_isArmedAway()
// {
//         return digitalRead(CONTROLLER_ARM_AWAY_PIN) == STATE_ON;
// }
// bool get_system_state_is_alarming()
// {
//         return digitalRead(SYSTEM_STATE_ALARM_PIN) == SYSTEM_STATE_ON;
// }
// bool get_system_state_is_armed()
// {
//         return digitalRead(SYSTEM_STATE_ARM_PIN) == SYSTEM_STATE_ON;
// }
// void getEntireSystemState(char state[])
// {
//         sprintf(state, "%d;%d;%d;%d;", get_system_state_is_armed(), get_system_state_is_alarming(), get_isArmedHome(), get_isArmedAway());
// }