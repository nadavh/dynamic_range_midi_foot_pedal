#include <MIDIUSB.h>
#include <EEPROM.h>

#define MIDI_CHANNEL 0
#define MIDI_CC 1

#define INPUT_PIN A0
#define CONF_BUTTON_PIN 2
#define CONF_LED_PIN 3

#define AVG_ARR_SIZE 100
#define REPORT_AVG_SIZE 25
#define ANALOG_VAL_MIN 0
#define ANALOG_VAL_MAX 1022
#define MAX_CONF_TIME_MS 5000
#define DEBOUNCE_DELAY_MS 10
#define EEPROM_HEADER 0xA1B2

enum mode
{
    MODE_RUNNING = 0,
    MODE_START_CONF,
    MODE_CONFIGURING,
    MODE_END_CONF,
};

struct range {
    unsigned int header;
    unsigned int min;
    unsigned int max;
} analog_range;

void setup()
{
    pinMode(CONF_BUTTON_PIN, INPUT_PULLUP);
    pinMode(CONF_LED_PIN, OUTPUT);
    load_range();
}

void loop()
{
    static enum mode mode = MODE_RUNNING;
    static unsigned long conf_start_ts;

    bool button_on = digitalRead(CONF_BUTTON_PIN) == LOW;

    int raw_val = analogRead(INPUT_PIN);
    if (raw_val < ANALOG_VAL_MIN)
        raw_val = ANALOG_VAL_MIN;
    else if (raw_val > ANALOG_VAL_MAX)
        raw_val = ANALOG_VAL_MAX;

    switch (mode) {
    case MODE_RUNNING:
        send_midi(raw_val);
        if (button_on)
            mode = MODE_START_CONF;
        break;
    case MODE_START_CONF:
        delay(DEBOUNCE_DELAY_MS);
        if (!button_on) {
            conf_start_ts = millis();
            digitalWrite(CONF_LED_PIN, HIGH);
            reset_range();
            mode = MODE_CONFIGURING;
        }
        break;
    case MODE_CONFIGURING:
        adjust_range(raw_val);
        if (button_on || millis() - conf_start_ts > MAX_CONF_TIME_MS)
            mode = MODE_END_CONF;
        break;
    case MODE_END_CONF:
        delay(DEBOUNCE_DELAY_MS);
        if (!button_on) {
            digitalWrite(CONF_LED_PIN, LOW);
            save_range();
            mode = MODE_RUNNING;
        }
        break;
    }
}

void reset_range()
{
    analog_range.min = ANALOG_VAL_MAX;
    analog_range.max = ANALOG_VAL_MIN;
}

void load_range()
{
    EEPROM.get(0, analog_range);

    // Invalid configuration is expected in initial run. Set defaults
    if (analog_range.header != EEPROM_HEADER
        || analog_range.min > analog_range.max
        || analog_range.max > ANALOG_VAL_MAX) {
        reset_range();
    }
}

void save_range()
{
    analog_range.header = EEPROM_HEADER;
    EEPROM.put(0, analog_range);
}

void adjust_range(int input_val)
{
    if (analog_range.min > input_val)
        analog_range.min = input_val;

    if (analog_range.max < input_val)
        analog_range.max = input_val;
}

void send_midi(int input_val)
{
    static int avg_arr[AVG_ARR_SIZE] = {0};
    static int avg_idx = 0;
    static byte prev_cc_val = 0;

    byte cc_val;
    int avg;
    long sum;

    // Keep in range
    if (input_val < analog_range.min)
        input_val = analog_range.min;
    else if (input_val > analog_range.max)
        input_val = analog_range.max;

    // Calc running average
    sum = 0;
    avg_arr[avg_idx] = input_val;
    avg_idx = (avg_idx + 1) % AVG_ARR_SIZE;
    for (int i = 0; i < AVG_ARR_SIZE; i++)
        sum += avg_arr[i];
    avg = sum / AVG_ARR_SIZE;

    // Send data
    cc_val = map(avg, analog_range.min, analog_range.max, 0, 127);
    if (cc_val != prev_cc_val && (avg_idx % REPORT_AVG_SIZE) == 0)
    {
        midiEventPacket_t event = {0x0B, 0xB0 | MIDI_CHANNEL, MIDI_CC, cc_val};
        MidiUSB.sendMIDI(event);
        MidiUSB.flush();
        prev_cc_val = cc_val;
    }
}
