
#define HX711_GLITCHLEVEL 200   // Threshold for delta
#define HX711_TIMEOUT 5     // Timout in seconds

enum hx711_mode { MODE_B32 = 26, MODE_A64 = 27, MODE_A128 = 25 }; 

struct hx711 {
    int pd_sck, dout;       // Port numbers for pd_sck and dout
    enum hx711_mode mode;   // Input and gain
    int avgtime;            // Period to average over
    int zero;               // Zero reading
    int average;            // Averaged reading
    int last;               // The last single valid reading
    int avgcount;
    time_t avgstart;
};

int hx711_read(struct hx711 *channel);
void hx711_setup(struct hx711 *channel);
int hx711_zero(struct hx711 *channel, int count);
void hx711_sleep(struct hx711 *channel);
