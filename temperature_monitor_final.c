/*
 *  SYSTÈME DE MONITORING INTELLIGENT DE TEMPÉRATURE
 *  École Nationale des Sciences de l'Informatique (ENSI)
 *  Projet C - Contrôleur Climatique IoT
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <math.h>

//  CONFIGURATION
#define MAX_READINGS 1000
#define MAX_LINE 256
#define CONFIG_FILE "config.txt"
#define SENSOR_FILE "sensor_data.txt"
#define LOG_FILE "journal_evenements.txt"
#define REPORT_FILE "rapport_journalier.txt"

// Couleurs pour terminal
#define RESET "\033[0m"
#define RED "\033[1;31m"
#define YELLOW "\033[1;33m"
#define GREEN "\033[1;32m"
#define CYAN "\033[1;36m"
#define MAGENTA "\033[1;35m"
#define BOLD "\033[1m"

// Couleurs de fond pour heat map
#define BG_BLUE "\033[44m"
#define BG_CYAN "\033[46m"
#define BG_GREEN "\033[42m"
#define BG_YELLOW "\033[43m"
#define BG_ORANGE "\033[48;5;208m"
#define BG_RED "\033[41m"

//  STRUCTURES

typedef struct
{
    double seuil_min;
    double seuil_max;
    int intervalle_mesure;
} Config;

typedef struct
{
    long timestamp;
    double temperature;
    double humidity;
    char location[32];
} SensorReading;

typedef struct
{
    double temp_min;
    double temp_max;
    double temp_moyenne;
    int total_lectures;
    int alertes_niveau1;
    int alertes_niveau2;
    int alertes_niveau3;
} Statistics;

typedef enum
{
    NORMAL = 0,
    AVERTISSEMENT = 1,
    ALERTE_MODEREE = 2,
    ALERTE_CRITIQUE = 3
} NiveauAlerte;

// Heat map structure
typedef struct
{
    char location[32];
    double temperatures[24];
} HeatMapRow;

//  VARIABLES GLOBALES
Config config;
SensorReading readings[MAX_READINGS];
int num_readings = 0;
Statistics stats = {0};
FILE *log_file = NULL;

HeatMapRow heatmap_data[10];
int heatmap_row_count = 0;

//  PROTOTYPES
void afficher_banniere(void);
int charger_configuration(const char *filename);
int charger_donnees_capteur(const char *filename);
NiveauAlerte evaluer_alerte(double temperature);
void traiter_lecture(SensorReading *reading);
void journaliser(long timestamp, const char *type, const char *message, double valeur);
void calculer_statistiques(void);
void generer_rapport(void);
void construire_heatmap(void);
void afficher_heatmap(void);
const char *niveau_to_string(NiveauAlerte niveau);
const char *niveau_to_color(NiveauAlerte niveau);
const char *get_temp_color(double temp);
char *timestamp_to_string(long ts);

//  IMPLÉMENTATION

void afficher_banniere(void)
{
    printf("\n");
    printf(CYAN "╔══════════════════════════════════════════════════════════════╗\n" RESET);
    printf(CYAN "║" BOLD "         SYSTÈME DE MONITORING INTELLIGENT DE TEMPÉRATURE     " RESET CYAN "║\n" RESET);
    printf(CYAN "║" RESET "             ENSI - Projet IoT Contrôleur Climatique          " CYAN "║\n" RESET);
    printf(CYAN "╚══════════════════════════════════════════════════════════════╝\n" RESET);
    printf("\n");
}

/*
 * TÂCHE 1: Charger la configuration
 */
int charger_configuration(const char *filename)
{
    FILE *file = fopen(filename, "r");
    if (!file)
    {
        fprintf(stderr, RED "[ERREUR] Impossible d'ouvrir %s\n" RESET, filename);
        return -1;
    }

    char line[MAX_LINE];
    char key[64];
    double value;

    // Valeurs par défaut
    config.seuil_min = 18.0;
    config.seuil_max = 28.0;
    config.intervalle_mesure = 5;

    while (fgets(line, MAX_LINE, file))
    {
        if (line[0] == '#' || line[0] == '\n')
            continue;

        if (sscanf(line, "%63[^=]=%lf", key, &value) == 2)
        {
            if (strcmp(key, "seuil_min") == 0)
                config.seuil_min = value;
            else if (strcmp(key, "seuil_max") == 0)
                config.seuil_max = value;
            else if (strcmp(key, "intervalle_mesure") == 0)
                config.intervalle_mesure = (int)value;
        }
    }

    fclose(file);
    printf(GREEN "[OK]" RESET " Configuration: [%.1f°C - %.1f°C]\n",
           config.seuil_min, config.seuil_max);
    return 0;
}

/*
 * TÂCHE 2: Charger les données capteur
 */
int charger_donnees_capteur(const char *filename)
{
    FILE *file = fopen(filename, "r");
    if (!file)
    {
        fprintf(stderr, RED "[ERREUR] Impossible d'ouvrir %s\n" RESET, filename);
        return -1;
    }

    char line[MAX_LINE];
    num_readings = 0;

    while (fgets(line, MAX_LINE, file) && num_readings < MAX_READINGS)
    {
        if (line[0] == '#' || line[0] == '\n')
            continue;

        SensorReading *r = &readings[num_readings];
        if (sscanf(line, "%ld,%lf,%lf,%31s",
                   &r->timestamp, &r->temperature, &r->humidity, r->location) >= 2)
        {
            num_readings++;
        }
    }

    fclose(file);
    printf(GREEN "[OK]" RESET " %d lectures chargées\n", num_readings);
    return num_readings;
}

/*
 * TÂCHE 5: Système d'alerte à 3 niveaux
 */
NiveauAlerte evaluer_alerte(double temperature)
{
    double ecart_haut = temperature - config.seuil_max;
    double ecart_bas = config.seuil_min - temperature;
    double ecart = fmax(ecart_haut, ecart_bas);

    if (ecart >= 8.0)
        return ALERTE_CRITIQUE;
    else if (ecart >= 4.0)
        return ALERTE_MODEREE;
    else if (ecart > 0)
        return AVERTISSEMENT;

    return NORMAL;
}

const char *niveau_to_string(NiveauAlerte niveau)
{
    switch (niveau)
    {
    case AVERTISSEMENT:
        return "AVERTISSEMENT";
    case ALERTE_MODEREE:
        return "ALERTE MODÉRÉE";
    case ALERTE_CRITIQUE:
        return "ALERTE CRITIQUE";
    default:
        return "NORMAL";
    }
}

const char *niveau_to_color(NiveauAlerte niveau)
{
    switch (niveau)
    {
    case AVERTISSEMENT:
        return YELLOW;
    case ALERTE_MODEREE:
        return MAGENTA;
    case ALERTE_CRITIQUE:
        return RED;
    default:
        return GREEN;
    }
}

/*
 * Convertit timestamp en chaîne lisible
 */
char *timestamp_to_string(long ts)
{
    static char buffer[32];
    time_t t = (time_t)ts;
    struct tm *tm_info = localtime(&t);
    strftime(buffer, 32, "%Y-%m-%d %H:%M:%S", tm_info);
    return buffer;
}

/*
 * TÂCHE 4: Journalisation avec TIMESTAMP CORRECT de la lecture
 */
void journaliser(long timestamp, const char *type, const char *message, double valeur)
{
    if (!log_file)
    {
        log_file = fopen(LOG_FILE, "w");
        if (!log_file)
            return;
    }

    char *ts = timestamp_to_string(timestamp);

    fprintf(log_file, "[%s] [%s] %s | Valeur: %.2f\n", ts, type, message, valeur);
    fflush(log_file);
}

/*
 * TÂCHE 3: Traiter une lecture et gérer les alertes
 */
void traiter_lecture(SensorReading *reading)
{
    NiveauAlerte niveau = evaluer_alerte(reading->temperature);

    if (niveau > NORMAL)
    {
        const char *color = niveau_to_color(niveau);
        const char *niveau_str = niveau_to_string(niveau);

        printf("%s⚠ [%s] %s @ %s: %.2f°C (%.1f%% humidité)\n" RESET,
               color, niveau_str, reading->location,
               timestamp_to_string(reading->timestamp),
               reading->temperature, reading->humidity);

        // Mise à jour statistiques
        if (niveau == AVERTISSEMENT)
            stats.alertes_niveau1++;
        else if (niveau == ALERTE_MODEREE)
            stats.alertes_niveau2++;
        else if (niveau == ALERTE_CRITIQUE)
            stats.alertes_niveau3++;

        // Journalisation avec le TIMESTAMP de la lecture
        char msg[128];
        snprintf(msg, sizeof(msg), "%s à %s", niveau_str, reading->location);
        journaliser(reading->timestamp, "ALERTE", msg, reading->temperature);
    }
}

/*
 * TÂCHE 6: Calculer les statistiques
 */
void calculer_statistiques(void)
{
    if (num_readings == 0)
        return;

    stats.temp_min = readings[0].temperature;
    stats.temp_max = readings[0].temperature;
    double sum = 0;

    for (int i = 0; i < num_readings; i++)
    {
        double t = readings[i].temperature;
        if (t < stats.temp_min)
            stats.temp_min = t;
        if (t > stats.temp_max)
            stats.temp_max = t;
        sum += t;
    }

    stats.total_lectures = num_readings;
    stats.temp_moyenne = sum / num_readings;
}

/*
 * TÂCHE 6: Générer le rapport
 */
void generer_rapport(void)
{
    FILE *report = fopen(REPORT_FILE, "w");
    if (!report)
    {
        fprintf(stderr, RED "[ERREUR] Impossible de créer le rapport\n" RESET);
        return;
    }

    time_t now = time(NULL);

    fprintf(report, "═══════════════════════════════════════════════════════════════\n");
    fprintf(report, "          RAPPORT JOURNALIER - MONITORING TEMPÉRATURE\n");
    fprintf(report, "          Généré le: %s", ctime(&now));
    fprintf(report, "═══════════════════════════════════════════════════════════════\n\n");

    fprintf(report, " STATISTIQUES\n");
    fprintf(report, "─────────────────────────────────────────────────────────────\n");
    fprintf(report, "  Total de lectures     : %d\n", stats.total_lectures);
    fprintf(report, "  Température minimale  : %.2f°C\n", stats.temp_min);
    fprintf(report, "  Température maximale  : %.2f°C\n", stats.temp_max);
    fprintf(report, "  Température moyenne   : %.2f°C\n", stats.temp_moyenne);
    fprintf(report, "\n");

    fprintf(report, "  ALERTES\n");
    fprintf(report, "─────────────────────────────────────────────────────────────\n");
    fprintf(report, "  Avertissements         : %d\n", stats.alertes_niveau1);
    fprintf(report, "  Alertes modérées       : %d\n", stats.alertes_niveau2);
    fprintf(report, "  Alertes critiques      : %d\n", stats.alertes_niveau3);
    fprintf(report, "\n");

    fprintf(report, "═══════════════════════════════════════════════════════════════\n");

    fclose(report);
    printf(GREEN "[OK]" RESET " Rapport généré: %s\n", REPORT_FILE);
}

/*
 * BONUS: Obtenir couleur pour heat map
 */
const char *get_temp_color(double temp)
{
    if (temp < 15.0)
        return BG_BLUE;
    else if (temp < 18.0)
        return BG_CYAN;
    else if (temp < 22.0)
        return BG_GREEN;
    else if (temp < 26.0)
        return BG_YELLOW;
    else if (temp < 30.0)
        return BG_ORANGE;
    else
        return BG_RED;
}

/*
 * BONUS: Construire les données du heat map
 */
void construire_heatmap(void)
{
    if (num_readings == 0)
        return;

    heatmap_row_count = 0;
    memset(heatmap_data, 0, sizeof(heatmap_data));

    long first_time = readings[0].timestamp;
    long last_time = readings[num_readings - 1].timestamp;
    long duration = last_time - first_time;
    long slot_duration = duration / 24;
    if (slot_duration == 0)
        slot_duration = 1;

    for (int i = 0; i < num_readings; i++)
    {
        SensorReading *r = &readings[i];

        // Trouver ou créer la ligne pour cette location
        int row_idx = -1;
        for (int j = 0; j < heatmap_row_count; j++)
        {
            if (strcmp(heatmap_data[j].location, r->location) == 0)
            {
                row_idx = j;
                break;
            }
        }

        if (row_idx == -1 && heatmap_row_count < 10)
        {
            row_idx = heatmap_row_count;
            strcpy(heatmap_data[row_idx].location, r->location);
            heatmap_row_count++;
        }

        if (row_idx == -1)
            continue;

        // Calculer le slot horaire
        int slot = (int)((r->timestamp - first_time) / slot_duration);
        if (slot >= 24)
            slot = 23;
        if (slot < 0)
            slot = 0;

        if (heatmap_data[row_idx].temperatures[slot] == 0)
        {
            heatmap_data[row_idx].temperatures[slot] = r->temperature;
        }
        else
        {
            // Moyenne si plusieurs lectures
            heatmap_data[row_idx].temperatures[slot] =
                (heatmap_data[row_idx].temperatures[slot] + r->temperature) / 2.0;
        }
    }
}

/*
 * BONUS: Afficher le heat map
 */
void afficher_heatmap(void)
{
    if (heatmap_row_count == 0)
    {
        printf(YELLOW "⚠ Pas assez de données pour le heat map\n" RESET);
        return;
    }

    printf("\n");
    printf(BOLD "╔══════════════════════════════════════════════════════════════╗\n" RESET);
    printf(BOLD "║" CYAN "                  HEAT MAP - TEMPÉRATURE TEMPORELLE           " RESET BOLD "║\n" RESET);
    printf(BOLD "╚══════════════════════════════════════════════════════════════╝\n" RESET);
    printf("\n");

    // Légende
    printf("  " BOLD "Légende:" RESET "  ");
    printf(BG_BLUE "   " RESET " <15°C  ");
    printf(BG_CYAN "   " RESET " 15-18°C  ");
    printf(BG_GREEN "   " RESET " 18-22°C  ");
    printf(BG_YELLOW "   " RESET " 22-26°C  ");
    printf(BG_ORANGE "   " RESET " 26-30°C  ");
    printf(BG_RED "   " RESET " >30°C\n\n");

    // En-tête
    printf("  Location        ");
    printf("0h    2h    4h    6h    8h   10h   12h   14h   16h   18h   20h   22h\n");
    printf("  " BOLD "%-15s" RESET " ", "──────────────");
    for (int i = 0; i < 24; i++)
        printf("──");
    printf("\n");

    // Lignes du heat map
    for (int row = 0; row < heatmap_row_count; row++)
    {
        HeatMapRow *hm_row = &heatmap_data[row];
        printf("  " BOLD "%-15s" RESET " ", hm_row->location);

        double row_avg = 0;
        int count = 0;

        for (int slot = 0; slot < 24; slot++)
        {
            double temp = hm_row->temperatures[slot];
            if (temp > 0)
            {
                const char *color = get_temp_color(temp);
                printf("%s  " RESET, color);
                row_avg += temp;
                count++;
            }
            else
            {
                printf("  ");
            }
        }

        if (count > 0)
        {
            printf(" │ %.1f°C", row_avg / count);
        }
        printf("\n");
    }
    printf("\n");
}

/*
 * FONCTION PRINCIPALE
 */
int main()
{
    afficher_banniere();

    // TÂCHE 1: Configuration
    printf("\n" BOLD "═══ INITIALISATION ═══\n" RESET);
    if (charger_configuration(CONFIG_FILE) != 0)
    {
        return EXIT_FAILURE;
    }

    // TÂCHE 2: Charger données
    if (charger_donnees_capteur(SENSOR_FILE) <= 0)
    {
        return EXIT_FAILURE;
    }

    // Ouvrir journal avec message initial
    log_file = fopen(LOG_FILE, "w");
    if (log_file)
    {
        // Log avec le timestamp de la première lecture
        journaliser(readings[0].timestamp, "SYSTÈME", "Démarrage du monitoring", 0);
    }

    // TÂCHE 3, 4, 5: Analyse et alertes
    printf("\n" BOLD "═══ ANALYSE DES DONNÉES ═══\n" RESET);
    for (int i = 0; i < num_readings; i++)
    {
        traiter_lecture(&readings[i]);
    }

    // TÂCHE 6: Statistiques et rapport
    printf("\n" BOLD "═══ GÉNÉRATION DU RAPPORT ═══\n" RESET);
    calculer_statistiques();
    generer_rapport();

    // BONUS: Heat Map
    printf("\n" BOLD "═══ VISUALISATION HEAT MAP ═══\n" RESET);
    construire_heatmap();
    afficher_heatmap();

    // Résumé
    printf("\n" BOLD "═══ RÉSUMÉ ═══\n" RESET);
    printf("┌─────────────────────────────────────────┐\n");
    printf("│ Lectures analysées    : %-15d │\n", stats.total_lectures);
    printf("│ Température moyenne   : %-13.2f°C │\n", stats.temp_moyenne);
    printf("│ Amplitude             : %.1f - %.1f°C   │\n",
           stats.temp_min, stats.temp_max);
    printf("│ Alertes totales       : %-15d │\n",
           stats.alertes_niveau1 + stats.alertes_niveau2 + stats.alertes_niveau3);
    printf("└─────────────────────────────────────────┘\n");

    // Fermeture avec log de fin
    if (log_file)
    {
        // Log avec le timestamp de la dernière lecture
        journaliser(readings[num_readings - 1].timestamp, "SYSTÈME", "Arrêt du monitoring", 0);
        fclose(log_file);
    }

    printf("\n" GREEN "✓ Terminé! Consultez:" RESET "\n");
    printf("  • %s (événements avec dates correctes)\n", LOG_FILE);
    printf("  • %s (statistiques)\n\n", REPORT_FILE);

    return EXIT_SUCCESS;
}
