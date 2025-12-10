#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

// ------------------------------------------------------
// STRUCTS
// ------------------------------------------------------

typedef struct
{
    float seuil_min;
    float seuil_max;
    int intervalle;
    char mode[16];
    float fixed_value;
} Config;

// ------------------------------------------------------
// CONFIG MANAGEMENT
// ------------------------------------------------------

void load_config(Config *cfg)
{
    FILE *f = fopen("config.txt", "r");
    if (!f)
    {
        printf("config.txt not found, using defaults.\n");
        cfg->seuil_min = 18;
        cfg->seuil_max = 26;
        cfg->intervalle = 5;
        strcpy(cfg->mode, "simulate");
        cfg->fixed_value = 22;
        return;
    }

    char line[128];
    while (fgets(line, sizeof(line), f))
    {
        char key[64], value[64];
        if (sscanf(line, "%[^=]=%s", key, value) == 2)
        {
            if (strcmp(key, "seuil_min") == 0)
                cfg->seuil_min = atof(value);
            else if (strcmp(key, "seuil_max") == 0)
                cfg->seuil_max = atof(value);
            else if (strcmp(key, "intervalle_mesure") == 0)
                cfg->intervalle = atoi(value);
            else if (strcmp(key, "mode") == 0)
                strcpy(cfg->mode, value);
            else if (strcmp(key, "fixed_value") == 0)
                cfg->fixed_value = atof(value);
        }
    }
    fclose(f);
}

// ------------------------------------------------------
// SENSOR SIMULATION
// ------------------------------------------------------

float read_sensor(Config *cfg)
{
    if (strcmp(cfg->mode, "fixed") == 0)
    {
        return cfg->fixed_value;
    }
    // simulate random temperature
    float base = (cfg->seuil_min + cfg->seuil_max) / 2.0;
    float noise = ((rand() % 100) / 100.0f) * 4 - 2; // [-2, +2]
    return base + noise;
}

// ------------------------------------------------------
// DECISION ENGINE (LEVEL 1–2–3)
// ------------------------------------------------------

int evaluate(float temp, Config *cfg, char *msg)
{
    float min = cfg->seuil_min;
    float max = cfg->seuil_max;

    if (temp >= min && temp <= max)
    {
        strcpy(msg, "OK : Temperature normale.");
        return 0;
    }

    float diff;
    if (temp > max)
        diff = temp - max;
    else
        diff = min - temp;

    if (diff <= 1.5)
    {
        strcpy(msg, "Niveau 1 : Depassement leger.");
        return 1;
    }
    else if (diff <= 4)
    {
        strcpy(msg, "Niveau 2 : Depassement modere.");
        return 2;
    }
    else
    {
        strcpy(msg, "Niveau 3 : Danger critique!");
        return 3;
    }
}

// ------------------------------------------------------
// LOGGING
// ------------------------------------------------------

void log_measure(float temp, const char *status)
{
    FILE *f = fopen("measurements.log", "a");
    if (!f)
        return;

    time_t t = time(NULL);
    fprintf(f, "%ld, %.2f, %s\n", t, temp, status);
    fclose(f);
}

void log_alert(float temp, int level, const char *msg)
{
    if (level == 0)
        return;

    FILE *f = fopen("alerts.log", "a");
    if (!f)
        return;

    time_t t = time(NULL);
    fprintf(f, "%ld | Level %d | %.2f°C | %s\n", t, level, temp, msg);
    fclose(f);
}

// ------------------------------------------------------
// MAIN LOOP
// ------------------------------------------------------

int main()
{
    srand(time(NULL));

    Config cfg;
    load_config(&cfg);

    printf("=== Temperature Controller ===\n");
    printf("Mode: %s | Interval: %ds | Range: [%.1f, %.1f]\n\n",
           cfg.mode, cfg.intervalle, cfg.seuil_min, cfg.seuil_max);

    while (1)
    {
        float temp = read_sensor(&cfg);

        char message[128];
        int level = evaluate(temp, &cfg, message);

        // timestamp
        time_t t = time(NULL);
        printf("[%ld] Temp = %.2f°C -> %s (Level %d)\n", t, temp, message, level);

        log_measure(temp, message);
        log_alert(temp, level, message);

        sleep(cfg.intervalle);
    }

    return 0;
}
