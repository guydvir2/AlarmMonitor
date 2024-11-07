myIOT2 iot;
extern void set_disarm();
extern uint8_t get_systemState();
extern void set_armState(uint8_t req_state);

void pub_systemState(uint8_t state = 5)
{
    if (state == 5) /* Not having any valid / input*/
    {
        state = get_systemState();
    }

    iot.pub_state(sys_states[state]);
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
void update_Parameters()
{
    iot.useSerial = false;
    iot.useFlashP = false;
    iot.noNetwork_reset = 15;
    iot.ignore_boot_msg = false;
}
void addiotnalMQTT(char *incoming_msg, char *_topic)
{
    char msg[100];

    if (strcmp(incoming_msg, "status") == 0)
    {
        sprintf(msg, "Status: [%s]", sys_states[get_systemState()]);
        iot.pub_msg(msg);
    }
    else if (strcmp(incoming_msg, sys_states[ARMED_HOME_CODE]) == 0) /* arm Home */
    {
        set_armState(ARMED_HOME_CODE);
    }
    else if (strcmp(incoming_msg, sys_states[ARMED_AWAY_CODE]) == 0) /* arm Away */
    {
        set_armState(ARMED_AWAY_CODE);
    }
    else if (strcmp(incoming_msg, sys_states[DISARMED]) == 0) /* disarmed */
    {
        set_armState(DISARMED);
    }
    else if (strcmp(incoming_msg, "ver2") == 0)
    {
        sprintf(msg, "ver:[%s]", VER);
        iot.pub_msg(msg);
    }
    else if (strcmp(incoming_msg, "clear") == 0)
    {
        digitalWrite(SET_SYSTEM_ARMED_HOME_PIN, STATE_ON);
        delay(DELAY_TO_REACT);
        digitalWrite(SET_SYSTEM_ARMED_HOME_PIN, !STATE_ON);
        delay(DELAY_TO_REACT);

        digitalWrite(SET_SYSTEM_ARMED_AWAY_PIN, STATE_ON);
        delay(DELAY_TO_REACT);
        digitalWrite(SET_SYSTEM_ARMED_AWAY_PIN, !STATE_ON);
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
        sprintf(msg, "armPin is [%s], AlarmPin is [%s], ArmHome is [%s], ArmAway is [%s]",
                digitalRead(SYSTEM_STATE_ARM_PIN) ? "OFF" : "ON",
                digitalRead(SYSTEM_STATE_ALARM_PIN) ? "OFF" : "ON",
                digitalRead(SET_SYSTEM_ARMED_HOME_PIN) ? "ON" : "OFF",
                digitalRead(SET_SYSTEM_ARMED_AWAY_PIN) ? "ON" : "OFF");
        iot.pub_msg(msg);
    }
}
void startIOTservices()
{
    setMQTTTopics();
    update_Parameters();
    iot.start_services(addiotnalMQTT);
}