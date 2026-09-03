#include <stdio.h>
#include <stdint.h>
#include <string.h>

#define TICKS_GREEN    5U
#define TICKS_YELLOW   2U
#define TICKS_RED      4U
#define QUEUE_BUSY     6U
#define LOG_LEN       20U

typedef enum { LIGHT_GREEN = 0, LIGHT_YELLOW, LIGHT_RED } LightState_t;

/* Status bits definitions */
#define BIT_NIGHT      0U
#define BIT_BUSY       1U
#define BIT_BLINK_ON   2U

/* Bitwise manipulation macros */
#define SET_BIT(reg, n)    ((reg) |=  (uint8_t)(1U << (n)))
#define CLR_BIT(reg, n)    ((reg) &= (uint8_t)~(1U << (n)))
#define TOGGLE_BIT(reg, n) ((reg) ^=  (uint8_t)(1U << (n)))
#define READ_BIT(reg, n)   ((uint8_t)(((uint8_t)(reg) >> (n)) & (uint8_t)1U))

static LightState_t light;
static uint8_t      status;
static uint8_t      ticksLeft;
static uint8_t      carsWaiting;
static uint32_t     carsPassed;
static char         logLine[LOG_LEN];

static void clearBuffer(void) {
    int c;
    while ((c = getchar()) != '\n' && c != EOF) {}
}

static void pushLog(char c) {
    for (uint8_t i = 0U; i < (LOG_LEN - 1U); ++i) {
        logLine[i] = logLine[i + 1U];
    }
    logLine[LOG_LEN - 1U] = c;
}

static uint8_t ticksFor(LightState_t s) {
    if (s == LIGHT_GREEN) {
        return (READ_BIT(status, BIT_BUSY) != 0U) ? (TICKS_GREEN + 2U) : TICKS_GREEN;
    }
    if (s == LIGHT_YELLOW) {
        return TICKS_YELLOW;
    }
    return TICKS_RED;
}

static LightState_t nextState(LightState_t s) {
    if (s == LIGHT_GREEN) return LIGHT_YELLOW;
    if (s == LIGHT_YELLOW) return LIGHT_RED;
    return LIGHT_GREEN;
}

static void resetCrossing(void) {
    light = LIGHT_RED;
    status = 0U;
    ticksLeft = ticksFor(light);
    carsWaiting = 0U;
    carsPassed = 0U;
    for (uint8_t i = 0U; i < LOG_LEN; ++i) {
        logLine[i] = '-';
    }
    printf("Crossing reset complete.\n");
}

static void drawLight(void) {
    printf("\n+---+\n");
    if (READ_BIT(status, BIT_NIGHT) != 0U) {
        printf("| %c | (Yellow Blinking)\n", (READ_BIT(status, BIT_BLINK_ON) != 0U) ? 'O' : ' ');
    } else {
        printf("| %c | [Red]\n", (light == LIGHT_RED) ? 'O' : ' ');
        printf("| %c | [Yellow]\n", (light == LIGHT_YELLOW) ? 'O' : ' ');
        printf("| %c | [Green]\n", (light == LIGHT_GREEN) ? 'O' : ' ');
    }
    printf("+---+\n");
    printf("Ticks Left: %u | Cars Waiting: %u\n", ticksLeft, carsWaiting);
}

static void tick(void) {
    if (READ_BIT(status, BIT_NIGHT) != 0U) {
        TOGGLE_BIT(status, BIT_BLINK_ON);
        pushLog((READ_BIT(status, BIT_BLINK_ON) != 0U) ? 'y' : '.');
        return;
    }

    if (light == LIGHT_GREEN && carsWaiting > 0U) {
        uint8_t passing = (carsWaiting >= 2U) ? 2U : carsWaiting;
        carsWaiting = (uint8_t)(carsWaiting - passing);
        carsPassed += passing;
        if (carsWaiting <= QUEUE_BUSY) {
            CLR_BIT(status, BIT_BUSY);
        }
    }

    char symbol = (light == LIGHT_GREEN) ? 'G' : ((light == LIGHT_YELLOW) ? 'Y' : 'R');
    pushLog(symbol);

    ticksLeft--;
    if (ticksLeft == 0U) {
        light = nextState(light);
        ticksLeft = ticksFor(light);
    }
}

static void addCars(void) {
    int incoming = 0;
    printf("How many cars arrived: ");
    if (scanf("%d", &incoming) != 1 || incoming < 0 || incoming > 100) {
        printf("Invalid input or silly number!\n");
        clearBuffer();
        return;
    }
    carsWaiting = (uint8_t)(carsWaiting + (uint8_t)incoming);
    if (carsWaiting > QUEUE_BUSY) {
        SET_BIT(status, BIT_BUSY);
    }
    printf("Added %d cars. Current queue: %u\n", incoming, carsWaiting);
}

static void toggleNight(void) {
    TOGGLE_BIT(status, BIT_NIGHT);
    if (READ_BIT(status, BIT_NIGHT) != 0U) {
        SET_BIT(status, BIT_BLINK_ON);
        printf("Switched to Night Mode.\n");
    } else {
        CLR_BIT(status, BIT_BLINK_ON);
        light = LIGHT_RED;
        ticksLeft = ticksFor(light);
        printf("Switched to Daytime Mode.\n");
    }
}

static void showLog(void) {
    printf("Recent Log (Oldest -> Newest): ");
    for (uint8_t i = 0U; i < LOG_LEN; ++i) {
        putchar(logLine[i]);
    }
    putchar('\n');
}

static void printBinary8(uint8_t v) {
    for (int i = 7; i >= 0; --i) {
        putchar(((v >> i) & 1) ? '1' : '0');
    }
}

static void crossingReport(void) {
    printf("\n=========== CROSSING REPORT ===========\n");
    printf("Cars Passed     : %u\n", carsPassed);
    printf("Cars Waiting    : %u\n", carsWaiting);
    printf("Night Active    : %s\n", READ_BIT(status, BIT_NIGHT) ? "YES" : "NO");
    printf("Busy Active     : %s\n", READ_BIT(status, BIT_BUSY) ? "YES" : "NO");
    printf("Status Byte Hex : 0x%02X\n", status);
    printf("Status Byte Bin : 0b");
    printBinary8(status);
    putchar('\n');
    printf("=======================================\n");
}

int main(void) {
    int choice = 0;
    resetCrossing();
    do {
        printf("\n1. Draw Light\n2. Tick 1s\n3. Add Cars\n4. Toggle Night\n5. Show Log\n6. Report\n0. Exit\nChoice: ");
        if (scanf("%d", &choice) != 1) {
            printf("Invalid input!\n");
            clearBuffer();
            continue;
        }
        switch (choice) {
            case 1: drawLight(); break;
            case 2: tick(); break;
            case 3: addCars(); break;
            case 4: toggleNight(); break;
            case 5: showLog(); break;
            case 6: crossingReport(); break;
            case 0: printf("Terminating.\n"); break;
            default: printf("Unknown option!\n"); break;
        }
    } while (choice != 0);
    return 0;
}