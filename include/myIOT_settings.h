myIOT2 iot;

extern void set_disarm();
extern uint8_t get_system_stateCode();
extern bool set_arm_withCode(uint8_t _armType);
extern uint8_t atBoot_systemState;

void pub_stateTopic(uint8_t state)
{
    iot.pub_state(sys_states[state]);
}
void pub_AlarmSystemState(uint8_t state)
{
    char a[50];
    // char b[30];
    // const char *origins[2] = {"Controller", "KeyPad"};

    sprintf(a, "SystemState: [%s]", sys_states[state]);
    iot.pub_msg(a);

    // if (state == ARMED_AWAY_CODE || state == ARMED_HOME_CODE)
    // {
    //     sprintf(b, " using [%s]", origins[0]);
    // }
    // else if (state == DISARMED || state == ALARMING)
    // {
    //     sprintf(b, " ");
    // }
    // else if (state == ARMED_KEYPAD)
    // {
    //     sprintf(b, " using [%s]", origins[1]);
    // }
    // else
    // {
    //     iot.pub_msg("Err");
    // }
    // sprintf(a, "SystemState: from[%s] to [%s]%s",
    //         sys_states[atBoot_systemState], sys_states[state], b);
    // iot.pub_msg(a);
}
void pub_ControllerCMD(uint8_t state)
{
    char a[50];
    sprintf(a, "ControllerCMD: [%s]", sys_states[state]);
    iot.pub_msg(a);
}
void setMQTTTopics()
{
    const char *t[] = {"DvirHome/Messages", "DvirHome/log", "DvirHome/debug"};
    const char *t2[] = {"DvirHome/alarmMonitor", "DvirHome/All"};
    const char *t3[2] = {"DvirHome/alarmMonitor/Avail", "DvirHome/alarmMonitor/State"};

    iot.add_gen_pubTopic(t, sizeof(t) / sizeof(t[0]));
    iot.add_subTopic(t2, sizeof(t2) / sizeof(t2[0]));
    iot.add_pubTopic(t3, sizeof(t3) / sizeof(t3[0]));
}

void addiotnalMQTT(char *incoming_msg, char *_topic)
{
    char msg[100];

    if (strcmp(incoming_msg, "status") == 0)
    {
        sprintf(msg, "Status: [%s]", sys_states[get_system_stateCode()]);
        iot.pub_msg(msg);
    }
    else if (strcmp(incoming_msg, sys_states[ARMED_HOME_CODE]) == 0) /* arm Home */
    {
        set_arm_withCode(ARMED_HOME_CODE);
    }
    else if (strcmp(incoming_msg, sys_states[ARMED_AWAY_CODE]) == 0) /* arm Away */
    {
        set_arm_withCode(ARMED_AWAY_CODE);
    }
    else if (strcmp(incoming_msg, sys_states[DISARMED]) == 0) /* disarmed */
    {
        set_arm_withCode(DISARMED);
    }
    else if (strcmp(incoming_msg, "ver2") == 0)
    {
        sprintf(msg, "ver:[%s]", VER);
        iot.pub_msg(msg);
    }
    else if (strcmp(incoming_msg, "clear") == 0)
    {
        digitalWrite(CONTROLLER_ARM_HOME_PIN, !STATE_ON);
        digitalWrite(CONTROLLER_ARM_AWAY_PIN, !STATE_ON);
        delay(DELAY_TO_REACT);

        iot.sendReset("Reset via MQTT");
    }
    else if (strcmp(incoming_msg, "all_off") == 0)
    {
        set_disarm();
        sprintf(msg, "All OFF: Received from %s", "MQTT");
        iot.pub_msg(msg);
    }
    else if (strcmp(incoming_msg, "help2") == 0)
    {
        sprintf(msg, "Help2: Commands #1 - [status, ver2, help2]");
        iot.pub_msg(msg);
        sprintf(msg, "Help2: Commands #2 - [armed_home, armed_away, disarmed, clear, all_off, debug]");
        iot.pub_msg(msg);
    }
    else if (strcmp(incoming_msg, "debug") == 0)
    {
        sprintf(msg, "Armed_Home [%s], Armed_Away is [%s], armPin [%s], AlarmPin [%s]",
                controllerState.is_armed_home ? "ON" : "OFF",
                controllerState.is_armed_away ? "ON" : "OFF",
                alarmsysState.is_armed ? "ON" : "OFF",
                alarmsysState.is_alarming ? "ON" : "OFF");
        iot.pub_msg(msg);
    }
}
void def_iot_parameters()
{
    iot.useSerial = false;
    iot.useFlashP = false;
    iot.noNetwork_reset = 15;
    iot.ignore_boot_msg = false;
}
void startIOTservices()
{
    setMQTTTopics();
    def_iot_parameters();
    iot.start_services(addiotnalMQTT);
}