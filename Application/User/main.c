/**
  ******************************************************************************
  * @file    main.c
  * @brief   STM32 Smart Garden
  ******************************************************************************
  */
#include "main.h"
#include "webpage.h"

#include "stm32l475e_iot01.h"
#include "stm32l475e_iot01_tsensor.h"
#include "stm32l475e_iot01_hsensor.h"
#include "stm32l475e_iot01_psensor.h"
#include <time.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "secrets.h"
// --- CONFIGURAZIONE RETE ---
#define PORT           80
#define WIFI_WRITE_TIMEOUT 10000
#define WIFI_READ_TIMEOUT  10000
#define SOCKET             0

#define TIME_PROTOCOL_DELTA 2208988800U

#define LOG(a) printf a

// Buffer globali
static uint8_t http[1024];
static uint8_t IP_Addr[4];

// Handle UART Globale
// MODIFICA 1: Aggiunto extern per evitare "multiple definition"
extern UART_HandleTypeDef hDiscoUart;

#ifdef __GNUC__
int __io_putchar(int ch)
#else
int fputc(int ch, FILE *f)
#endif
{
  HAL_UART_Transmit(&hDiscoUart, (uint8_t *)&ch, 1, 0xFFFF);
  return ch;
}


// --- VARIABILI PROGETTO ---
int cfg_start_hour = 20;
int cfg_duration_min = 10;
uint8_t cfg_days_mask = 127;

int sim_mode_active = 0;
int sim_rain_prob = 30;
int real_rain_prob = -1; //   variabile "Attiva" usata da tutto il programma
int buf_rain_today = -1;         // dato grezzo API Oggi
int buf_rain_tomorrow = -1;      //  dato grezzo API Domani
int is_showing_tomorrow = 0;     // Flag per la UI (0 = Oggi, 1 = Domani)
uint8_t skip_irrigation = 0;

int smart_mode_active = 0;       // 0 = Manuale, 1 = Smart Auto
int smart_threshold = 60;        // Soglia punteggio per irrigare (Default 60)
int last_calculated_score = 0;   // Per visualizzazione web

#define LAT "45.03"
#define LON "10.74"

typedef struct {
    uint8_t hours;
    uint8_t minutes;
    uint8_t seconds;
    uint8_t weekday; // 0=Lun, 1=Mar, ..., 6=Dom
} Time_Only_t;

Time_Only_t orario = {0, 0, 0};

uint32_t last_second_tick = 0;
uint32_t last_sync_tick = 0;
uint32_t sync_interval = 3600000;

//Salvataggio impostazioni in MEMORIA FLASH
// Indirizzo ultima pagina (STM32L475 1MB = Bank 2, Page 255)
#define FLASH_STORAGE_ADDR 0x080FF800
#define CONFIG_MAGIC 0xCAFEBABE // Codice per capire se i dati sono validi

// Questa struct serve SOLO per salvare/caricare.
typedef struct {
    uint32_t magic_number;
    int saved_start_hour;
    int saved_duration_min;
    uint8_t saved_days_mask;
    int saved_smart_active;
    int saved_smart_threshold;
    int saved_sim_mode;
    int saved_sim_rain_prob;
} FlashData_t;


// Prototipi
static void SystemClock_Config(void);
static WIFI_Status_t SendWebPage(void);
static int wifi_server(void);
static int wifi_connect(void);
static bool WebServerProcess(void);
void UpdateTimeHTTP(Time_Only_t *currentTime);
void IncrementTime(void);
void CheckWeatherForecast(void);
void UpdateIrrigationLogic(void);
int CalculateSmartScore(float t, float h, float p, int rain_prob);
void RefreshSystemState(void);
void ApplyIrrigationControl(void);
void LoadSettings(void);
void SaveSettings(void);
// Fix printf
int _write(int file, char *ptr, int len) {
    HAL_UART_Transmit(&hDiscoUart, (uint8_t*)ptr, len, 1000);
    return len;
}

int main(void)
{
  HAL_Init();
  SystemClock_Config();


	// --- INIZIALIZZAZIONE PIN RELÈ (PA4 - Arduino D7) ---
	// Abilita il clock per la porta GPIOA
	__HAL_RCC_GPIOA_CLK_ENABLE();

	GPIO_InitTypeDef GPIO_InitStruct = {0};
	GPIO_InitStruct.Pin = GPIO_PIN_4;           // Pin PA4 (Corrisponde a D7)
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP; // Output Push-Pull (Corretto per relè)
	GPIO_InitStruct.Pull = GPIO_NOPULL;         // Nessuna resistenza interna
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;// Bassa velocità

	HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

	// valvola chiusa (Basso)
	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_4, GPIO_PIN_RESET);



  // 1. INIZIALIZZAZIONE SERIALE
  hDiscoUart.Instance = DISCOVERY_COM1;
  hDiscoUart.Init.BaudRate = 115200;
  hDiscoUart.Init.WordLength = UART_WORDLENGTH_8B;
  hDiscoUart.Init.StopBits = UART_STOPBITS_1;
  hDiscoUart.Init.Parity = UART_PARITY_NONE;
  hDiscoUart.Init.Mode = UART_MODE_TX_RX;
  hDiscoUart.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  hDiscoUart.Init.OverSampling = UART_OVERSAMPLING_16;
  hDiscoUart.Init.OneBitSampling = UART_ONE_BIT_SAMPLE_DISABLE;
  hDiscoUart.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;

  BSP_COM_Init(COM1, &hDiscoUart);

  printf("\r\n\r\n==================================\r\n");
  printf("   SMART GARDEN BOOTING... \r\n");
  printf("==================================\r\n");

  // 2. INIZIALIZZAZIONE SENSORI
  printf("Init Sensori... ");

  if(BSP_HSENSOR_Init() != HSENSOR_OK) printf("[ERR HUM] ");
  else printf("[HUM OK] ");

  if(BSP_TSENSOR_Init() != TSENSOR_OK) printf("[ERR TMP] ");
  else printf("[TMP OK] ");

  if(BSP_PSENSOR_Init() != PSENSOR_OK) printf("[ERR PRS] ");
  else printf("[PRS OK] ");

  printf("\r\n");

  LoadSettings();

  HAL_Delay(500);

  // Test lettura valori
  float t_test = BSP_TSENSOR_ReadTemp();
  float h_test = BSP_HSENSOR_ReadHumidity();
  printf("Test Lettura: T=%.2f H=%.2f\r\n", t_test, h_test);
  if(t_test == 0.0f) printf("ATTENZIONE: Se leggi 0.00 controlla il flag -u _printf_float nel linker!\r\n");

  BSP_LED_Init(LED2);

  // 4. AVVIO WIFI
  wifi_server();
}

static int wifi_start(void) {
  if(WIFI_Init() ==  WIFI_STATUS_OK) return 0;
  return -1;
}

int wifi_connect(void) {
  wifi_start();
  LOG(("\nConnessione WiFi a %s...\n", SSID));
  if( WIFI_Connect(SSID, PASSWORD, WIFI_ECN_WPA_WPA2_PSK) == WIFI_STATUS_OK) {
    WIFI_GetIP_Address(IP_Addr, sizeof(IP_Addr));
    LOG(("> CONNESSO! IP: %d.%d.%d.%d\n", IP_Addr[0], IP_Addr[1], IP_Addr[2], IP_Addr[3]));
    return 0;
  }
  LOG(("Errore Connessione WiFi!\n"));
  return -1;
}

int wifi_server(void)
{
  bool StopServer = false;
  if (wifi_connect()!=0) return -1;

  LOG(("Sincronizzazione NTP e Meteo...\n"));
  UpdateTimeHTTP(&orario);
  CheckWeatherForecast();
  UpdateIrrigationLogic();
  RefreshSystemState();
  ApplyIrrigationControl();

  last_sync_tick = HAL_GetTick();
  last_second_tick = HAL_GetTick();

  WIFI_StartServer(SOCKET, WIFI_TCP_PROTOCOL, 1, "", PORT);
  LOG(("Server HTTP attivo su porta 80.\n"));

  do {
    uint8_t RemoteIP[4];
    uint16_t RemotePort;

    if (HAL_GetTick() - last_second_tick >= 1000) {
            last_second_tick += 1000;
            IncrementTime();
            ApplyIrrigationControl();
    }

    if (HAL_GetTick() - last_sync_tick >= sync_interval) {
        LOG(("\n--- Auto Sync ---\n"));
        UpdateTimeHTTP(&orario);
        CheckWeatherForecast();
        UpdateIrrigationLogic();
        RefreshSystemState();
        last_sync_tick = HAL_GetTick();
    }

    if (WIFI_STATUS_OK == WIFI_WaitServerConnection(SOCKET, 1000, RemoteIP, sizeof(RemoteIP), &RemotePort)) {
        StopServer = WebServerProcess();
        WIFI_CloseServerConnection(SOCKET);
        HAL_Delay(500);
    }
  } while(StopServer == false);

  WIFI_StopServer(SOCKET);
  return 0;
}

static bool WebServerProcess(void)
{
  uint16_t respLen;
  static uint8_t resp[1024];
  bool stopserver=false;

  if (WIFI_STATUS_OK == WIFI_ReceiveData(SOCKET, resp, 1000, &respLen, WIFI_READ_TIMEOUT))
  {
    if( respLen > 0)
    {
      if(respLen < 1024) resp[respLen] = '\0'; else resp[1023] = '\0';

      if(strstr((char *)resp, "GET")) {
        SendWebPage();
      }
      else if(strstr((char *)resp, "POST")) {
         LOG(("POST Ricevuto. Aggiorno configurazione...\n"));
         char *p;

         p = strstr((char *)resp, "cfg_hour=");
         if (p) cfg_start_hour = atoi(p + 9);

         p = strstr((char *)resp, "cfg_dur=");
         if (p) cfg_duration_min = atoi(p + 8);


         // --- NUOVO PARSING SMART MODE ---
		 if (strstr((char *)resp, "smart_active=1")) {
			  smart_mode_active = 1;
		 } else {
			  smart_mode_active = 0;
		 }

		 char *p_smart = strstr((char *)resp, "smart_th=");
		 if (p_smart) smart_threshold = atoi(p_smart + 9);
		 // -------------------------------

         cfg_days_mask = 0;
         if (strstr((char *)resp, "day_0=1")) cfg_days_mask |= (1<<0);
         if (strstr((char *)resp, "day_1=1")) cfg_days_mask |= (1<<1);
         if (strstr((char *)resp, "day_2=1")) cfg_days_mask |= (1<<2);
         if (strstr((char *)resp, "day_3=1")) cfg_days_mask |= (1<<3);
         if (strstr((char *)resp, "day_4=1")) cfg_days_mask |= (1<<4);
         if (strstr((char *)resp, "day_5=1")) cfg_days_mask |= (1<<5);
         if (strstr((char *)resp, "day_6=1")) cfg_days_mask |= (1<<6);

         if (strstr((char *)resp, "sim_mode=1")) {
             sim_mode_active = 1;
             p = strstr((char *)resp, "sim_rain=");
             if (p) sim_rain_prob = atoi(p + 9);
         } else {
             sim_mode_active = 0;
         }

         RefreshSystemState();
         ApplyIrrigationControl();
         SaveSettings();
         SendWebPage();
      }
    }
  }
  return stopserver;
}

void UpdateIrrigationLogic(void) {
    int prob_to_check = (sim_mode_active) ? sim_rain_prob : real_rain_prob;
    if(prob_to_check == -1) prob_to_check = 0;

    if (prob_to_check > 60) {
        if(skip_irrigation == 0) LOG(("LOGICA: Pioggia alta (%d%%) -> STOP Irrigazione\n", prob_to_check));
        skip_irrigation = 1;
    } else {
        if(skip_irrigation == 1) LOG(("LOGICA: Pioggia bassa (%d%%) -> OK Irrigazione\n", prob_to_check));
        skip_irrigation = 0;
    }
}

// Funzione per aggiornare sensori e logica decisionale
// Va chiamata nella sincronizzazione oraria e subito dopo aver ricevuto un POST
void RefreshSystemState(void) {
    // 1. Lettura Sensori (per avere dati freschi su Score e Dashboard)
    float current_T = BSP_TSENSOR_ReadTemp();
    float current_H = BSP_HSENSOR_ReadHumidity();
    float current_P = BSP_PSENSOR_ReadPressure();

    // 2. Calcolo Logica Temporale (Oggi vs Domani)
    int irrigation_end_minute = cfg_start_hour * 60 + cfg_duration_min;
    int current_minute_abs = orario.hours * 60 + orario.minutes;

    if (current_minute_abs >= irrigation_end_minute) {
        is_showing_tomorrow = 1;
        real_rain_prob = buf_rain_tomorrow;
    } else {
        is_showing_tomorrow = 0;
        real_rain_prob = buf_rain_today;
    }

    // 3. Determina probabilità attiva (Simulata o Reale)
    int prob_to_use = (sim_mode_active) ? sim_rain_prob : real_rain_prob;
    if(prob_to_use == -1) prob_to_use = 0;

    // 4. Aggiorna Skip Irrigation (Logica Manuale base)
    // (UpdateIrrigationLogic usa le variabili globali appena aggiornate sopra)
    UpdateIrrigationLogic();

    // 5. Calcola Score Smart (Logica AI)
    last_calculated_score = CalculateSmartScore(current_T, current_H, current_P, prob_to_use);
}

void ApplyIrrigationControl(void) {
    int start_irrigation = 0;

    int is_time_window = (orario.hours == cfg_start_hour && orario.minutes < cfg_duration_min);

    if (is_time_window) {

        if (smart_mode_active) {
            // Logica Smart: Score > Soglia
            // (Nota: last_calculated_score è già stato aggiornato da RefreshSystemState)
            if (last_calculated_score >= smart_threshold) {
                start_irrigation = 1;
            }
        } else {
            int is_day_active = (cfg_days_mask & (1 << orario.weekday));
            if (is_day_active && skip_irrigation == 0) {
                start_irrigation = 1;
            }
        }
    }

    if (start_irrigation) {
        HAL_GPIO_WritePin(GPIOA, GPIO_PIN_4, GPIO_PIN_SET);
        BSP_LED_On(LED2);
    } else {
        HAL_GPIO_WritePin(GPIOA, GPIO_PIN_4, GPIO_PIN_RESET);
        BSP_LED_Off(LED2);
    }
}



/*
 * Calcolo indice di bisogno idrico
 * Formula: (100-Umidità) + (Temp*2) - (PioggiaProb) + (CorrezionePressione)
 */
int CalculateSmartScore(float t, float h, float p, int rain_prob) {
    int score = 0;

    if (h < 0) h = 0;
    if (h > 100) h = 100;
    score += (100 - (int)h);

    if (t > 0) {
        score += ((int)t* 2);
    }

    score -= rain_prob;

    if (p < 1000.0f) {
        score -= 30; // Forte depressione, pioggia imminente
    } else if (p < 1010.0f) {
        score -= 10;
    } else if (p > 1020.0f) {
        score += 10; // Alta pressione, sereno
    }

    return score;
}

static WIFI_Status_t SendWebPage(void)
{
  uint16_t SentDataLength;
  WIFI_Status_t ret = WIFI_STATUS_OK;
  float H, T, P;
  uint32_t total_len, sent_len = 0;
  uint16_t chunk_len;

  // Lettura Sensori
  H = BSP_HSENSOR_ReadHumidity();
  T = BSP_TSENSOR_ReadTemp();
  P = BSP_PSENSOR_ReadPressure();

  // Recupero stato sistema
  // real_rain_prob è già impostato su Oggi o Domani dal loop principale
  int display_prob = (sim_mode_active) ? sim_rain_prob : real_rain_prob;

  // Legge fisicamente il pin per sapere se sta irrigando ORA
  int is_running = (HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_4) == GPIO_PIN_SET);

  char *status_class;
  char *status_text;
  char *time_label;

  // 3. Determina l'etichetta temporale (Oggi/Domani/Sim)
  if (sim_mode_active) time_label = "Simulazione";
  else time_label = (is_showing_tomorrow) ? " Domani" : " Oggi";

  // 4. Logica Decisionale per il Banner (Testo e Colore)
  if (is_running) {
      // PRIORITÀ MASSIMA: Se l'acqua scorre, sfondo VERDE e testo chiaro
      status_class = "bg-ok";
      status_text = "IRRIGAZIONE IN CORSO...";
  }
  else if (display_prob == -1 && !sim_mode_active) {
      status_class = "bg-wait";
      status_text = "ATTESA DATI METEO...";
  }
  else {
      // Se non irriga, mostriamo lo stato della pianificazione (Smart o Manuale)
      if (smart_mode_active) {
          if (last_calculated_score >= smart_threshold) {
              status_class = "bg-ok";
              status_text = "PIANIFICATA (Smart)";
          } else {
              status_class = "bg-stop"; // O bg-wait, a scelta
              status_text = "NON NECESSARIA (Score Basso)";
          }
      } else {
          // Modalità Manuale
          // Se display_prob > 60 (che sia oggi o domani), c'è rischio pioggia
          if (display_prob > 60) {
              status_class = "bg-stop";
              status_text = "SOSPESA (Previsione Pioggia)";
          } else {
              // Verifica se il giorno (oggi o domani) è abilitato
              int day_check = orario.weekday;
              // Se stiamo guardando a domani, controlliamo il giorno successivo nella maschera
              if (is_showing_tomorrow && !sim_mode_active) day_check = (day_check + 1) % 7;

              if (cfg_days_mask & (1 << day_check)) {
                  status_class = "bg-ok";
                  status_text = "PIANIFICATA";
              } else {
                  status_class = "bg-stop";
                  status_text = "DISABILITATA (Giorno OFF)";
              }
          }
      }
  }

  // --- INIZIO INVIO DATI (CHUNKS) ---

  // CHUNK 1: Header HTML statico
  total_len = strlen(HTML_HEADER);
  while (sent_len < total_len) {
      chunk_len = (total_len - sent_len > 500) ? 500 : (total_len - sent_len);
      memcpy(http, HTML_HEADER + sent_len, chunk_len);
      ret = WIFI_SendData(0, (uint8_t *)http, chunk_len, &SentDataLength, WIFI_WRITE_TIMEOUT);
      if(ret != WIFI_STATUS_OK) return ret;
      sent_len += chunk_len;
  }

  // CHUNK 2: Script Orologio
  sprintf((char *)http, "<script>startClock(%d,%d,%d);</script>", orario.hours, orario.minutes, orario.seconds);
  WIFI_SendData(0, (uint8_t *)http, strlen((char *)http), &SentDataLength, WIFI_WRITE_TIMEOUT);

  // CHUNK 3: Status Bar e Dashboard Sensori
  char rainStr[20];
  if (display_prob == -1 && !sim_mode_active) sprintf(rainStr, "--");
  else sprintf(rainStr, "%d%%", display_prob);

  sprintf((char *)http,
    "<div class='status-bar %s'>"
      "<div style='display:flex; justify-content:space-between; align-items:center;'>"
         "<div><strong>Stato:</strong> %s</div>"
         "<div class='badge' style='background:rgba(0,0,0,0.2);'>%s</div>"
      "</div>"
      "<div class='v-center' style='font-size:0.9em; margin-top:5px;'>"
         "<span class='material-icons' style='font-size:1.1em; margin-right:5px'>cloud</span>"
         "Previsione Pioggia per <strong>%s</strong>: <strong>%s</strong>"
      "</div>"
    "</div>"
    "<div class='dashboard'>"
      "<div class='card'><span class='material-icons'>device_thermostat</span><span class='value'>%.1f</span><span class='unit'>&deg;C</span></div>"
      "<div class='card'><span class='material-icons'>water_drop</span><span class='value'>%.1f</span><span class='unit'>%%</span></div>"
      "<div class='card'><span class='material-icons'>compress</span><span class='value'>%.0f</span><span class='unit'>hPa</span></div>"
    "</div>",
    status_class,
    status_text,
    (sim_mode_active ? "SIMULAZIONE" : "ONLINE"),
    time_label, // Qui scrive Oggi o Domani
    rainStr,
    T, H, P
  );
  WIFI_SendData(0, (uint8_t *)http, strlen((char *)http), &SentDataLength, WIFI_WRITE_TIMEOUT);

  // CHUNK 4: Pannello Pianificazione Manuale (ORA È IL PRIMO)
    // Nota: È qui che apriamo il tag <form>
    strcpy((char *)http, "<form method='POST'>");
    sprintf((char *)http + strlen((char *)http),
      "<div class='panel'>"
        "<h3><span class='material-icons'>schedule</span> Manuale</h3>"
        "<div class='form-row'>"
          "<div><label>Avvio (Ora)</label><input type='number' name='cfg_hour' value='%d' min='0' max='23'></div>"
          "<div><label>Durata (Min)</label><input type='number' name='cfg_dur' value='%d' min='1' max='60'></div>"
        "</div>"
        "<label>Giorni Attivi:</label>"
        "<div class='day-selector'>",
      cfg_start_hour, cfg_duration_min
    );
    WIFI_SendData(0, (uint8_t *)http, strlen((char *)http), &SentDataLength, WIFI_WRITE_TIMEOUT);

    // Continuazione Manuale (Checkbox Giorni)
    const char* days[] = {"L", "M", "M", "G", "V", "S", "D"};
    strcpy((char *)http, "");
    for(int i=0; i<7; i++) {
        sprintf((char *)http + strlen((char *)http),
          "<label><input type='checkbox' name='day_%d' value='1' %s>%s</label>",
          i, (cfg_days_mask & (1<<i)) ? "checked" : "", days[i]
        );
    }
    strcat((char *)http, "</div></div>");
    WIFI_SendData(0, (uint8_t *)http, strlen((char *)http), &SentDataLength, WIFI_WRITE_TIMEOUT);


    // CHUNK 5: Pannello Auto Mode (ORA È IL SECONDO)
    char *score_color = (last_calculated_score >= smart_threshold) ? "#27ae60" : "#7f8c8d";

    sprintf((char *)http,
        "<div class='panel' style='border-left: 5px solid #3498db;'>"
          "<h3><span class='material-icons'>psychology</span>Auto Mode</h3>"

          "<div class='toggle-wrap'>"
            "<input type='checkbox' name='smart_active' value='1' %s>"
            "<label><strong>ABILITA AUTO MODE</strong> (Ignora Giorni)</label>"
          "</div>"

          "<div style='margin: 15px 0; padding: 10px; background: #ecf0f1; border-radius: 5px; text-align: center;'>"
            "<div style='font-size: 0.9em; color: #7f8c8d;'>Indice Bisogno Idrico (%s)</div>"
            "<div style='font-size: 2em; font-weight: bold; color: %s;'>%d <span style='font-size:0.5em'>/ 100</span></div>"
            "<div style='font-size: 0.7em;'>Basato su: Umidità, Temp, Pressione e Pioggia (%s)</div>"
          "</div>"

          "<label>Soglia Attivazione (Default 60):</label>"
          "<div style='display:flex; gap:10px; align-items:center;'>"
            "<input type='number' name='smart_th' value='%d' min='0' max='200' style='width:80px'>"
            "<span style='font-size:0.8em; color:#7f8c8d;'>Più alto = Irrigazione meno frequente</span>"
          "</div>"
        "</div>",
        smart_mode_active ? "checked" : "",
        time_label,
        score_color,
        last_calculated_score,
        time_label,
        smart_threshold
    );
    WIFI_SendData(0, (uint8_t *)http, strlen((char *)http), &SentDataLength, WIFI_WRITE_TIMEOUT);

  // CHUNK 6: Pannello Simulazione, Submit e Footer
  sprintf((char *)http,
    "<div class='panel sim-panel'>"
      "<h3><span class='material-icons'>tune</span> Simulatore</h3>"
      "<div class='toggle-wrap'>"
        "<input type='checkbox' name='sim_mode' value='1' %s>"
        "<label>Usa dati manuali</label>"
      "</div>"
      "<label>Pioggia Simulata (%%):</label>"
      "<input type='number' name='sim_rain' value='%d' min='0' max='100'>"
      "<p style='margin:5px 0 0; font-size:0.8em; color:#d35400;'>Smart Mode usa questo valore se attivo.</p>"
    "</div>",
    sim_mode_active ? "checked" : "",
    sim_rain_prob
  );

  strcat((char *)http, "<input type='submit' value='SALVA IMPOSTAZIONI' class='btn-save'>");
  strcat((char *)http, "</form>");

  // Footer finale
  strcat((char *)http, HTML_FOOTER);

  WIFI_SendData(0, (uint8_t *)http, strlen((char *)http), &SentDataLength, WIFI_WRITE_TIMEOUT);

  return ret;
}

static void SystemClock_Config(void)
{
  RCC_ClkInitTypeDef RCC_ClkInitStruct;
  RCC_OscInitTypeDef RCC_OscInitStruct;
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_MSI;
  RCC_OscInitStruct.MSIState = RCC_MSI_ON;
  RCC_OscInitStruct.MSIClockRange = RCC_MSIRANGE_6;
  RCC_OscInitStruct.MSICalibrationValue = RCC_MSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_MSI;
  RCC_OscInitStruct.PLL.PLLM = 1;
  RCC_OscInitStruct.PLL.PLLN = 40;
  RCC_OscInitStruct.PLL.PLLR = 2;
  RCC_OscInitStruct.PLL.PLLP = 7;
  RCC_OscInitStruct.PLL.PLLQ = 4;
  HAL_RCC_OscConfig(&RCC_OscInitStruct);
  RCC_ClkInitStruct.ClockType = (RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2);
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;
  HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_4);
}


// Funzione ausiliaria per convertire Mese (Text) -> Numero
uint8_t GetMonthNum(char *monthStr) {
    if (strstr(monthStr, "Jan")) return 1;
    if (strstr(monthStr, "Feb")) return 2;
    if (strstr(monthStr, "Mar")) return 3;
    if (strstr(monthStr, "Apr")) return 4;
    if (strstr(monthStr, "May")) return 5;
    if (strstr(monthStr, "Jun")) return 6;
    if (strstr(monthStr, "Jul")) return 7;
    if (strstr(monthStr, "Aug")) return 8;
    if (strstr(monthStr, "Sep")) return 9;
    if (strstr(monthStr, "Oct")) return 10;
    if (strstr(monthStr, "Nov")) return 11;
    if (strstr(monthStr, "Dec")) return 12;
    return 1; // Default
}

// Funzione ausiliaria per convertire Giorno (Text) -> Numero (0=Lun, ... 6=Dom)
uint8_t GetWeekdayNum(char *dayStr) {
    if (strstr(dayStr, "Mon")) return 0;
    if (strstr(dayStr, "Tue")) return 1;
    if (strstr(dayStr, "Wed")) return 2;
    if (strstr(dayStr, "Thu")) return 3;
    if (strstr(dayStr, "Fri")) return 4;
    if (strstr(dayStr, "Sat")) return 5;
    if (strstr(dayStr, "Sun")) return 6;
    return 0; // Default
}

void UpdateTimeHTTP(Time_Only_t *currentTime) {
    uint8_t ipAddr[4] = {0};

    char recvBuf[1024] = {0};
    uint16_t len = 0;

    printf("\n--- TIME SYNC (HTTP Port 80) ---\n");

    // DNS Resolve Google
    printf("1. DNS Resolve (www.google.com)... ");
    if (WIFI_GetHostAddress("www.google.com", ipAddr, 4) != WIFI_STATUS_OK) {
        printf("FALLITO (DNS)\n");
        return;
    }
    printf("OK (%d.%d.%d.%d)\n", ipAddr[0], ipAddr[1], ipAddr[2], ipAddr[3]);

    // Connessione TCP Porta 80
    WIFI_CloseClientConnection(1);
    HAL_Delay(200);

    if (WIFI_OpenClientConnection(1, WIFI_TCP_PROTOCOL, "www.google.com", ipAddr, 80, 0) == WIFI_STATUS_OK) {
        printf("Connesso. Invio richiesta HTTP HEAD...\n");

        // Costruzione richiesta HTTP 1.1 Minima
        // HEAD invece di GET per risparmiare dati (scarica solo gli header)
        char *request = "HEAD / HTTP/1.1\r\nHost: www.google.com\r\nConnection: close\r\n\r\n";
        uint16_t sentLen = 0;

        WIFI_SendData(1, (uint8_t*)request, strlen(request), &sentLen, 1000);

        // Ricezione Risposta
        // Diamo tempo al server di processare
        HAL_Delay(500);

        // Leggiamo tutto in una volta (buffer da 1024 è sufficiente per gli header)
        WIFI_Status_t status = WIFI_ReceiveData(1, (uint8_t*)recvBuf, 1023, &len, 2000);

        if (status == WIFI_STATUS_OK && len > 0) {
            recvBuf[len] = '\0'; // Terminiamo la stringa per sicurezza
            // printf("Dump Ricevuto:\n%s\n", recvBuf); // Decommenta per debug

            // Parsing della stringa "Date:"
            // Formato standard: "Date: Sat, 31 Jan 2026 14:24:42 GMT"
            char *datePtr = strstr(recvBuf, "Date: ");

            if (datePtr != NULL) {
                // Variabili temporanee per il parsing
                char wdayStr[5] = {0};
                char monthStr[5] = {0};
                int day, year, h, m, s;

                // Formato stringa: "Wdy, DD Mon YYYY HH:MM:SS GMT"
                // Esempio: "Sat, 31 Jan 2026 14:24:42"
                if (sscanf(datePtr + 6, "%3s, %d %3s %d %d:%d:%d",
                           wdayStr, &day, monthStr, &year, &h, &m, &s) == 7) {

                    // L'ora ricevuta è GMT (Greenwich).
                    // Aggiungiamo +1 per l'Italia (o gestisci qui l'ora legale +2)
                    int fuso_orario = 1;
                    h += fuso_orario;
                    if (h >= 24) {
                        h -= 24;
                    }

                    currentTime->hours = h;
                    currentTime->minutes = m;
                    currentTime->seconds = s;
                    currentTime->weekday = GetWeekdayNum(wdayStr);

                    printf("\n>>> SUCCESSO HTTP: %02d:%02d:%02d (Giorno: %s) <<<\n",
                           currentTime->hours, currentTime->minutes, currentTime->seconds, wdayStr);

                } else {
                    printf("Errore nel parsing della data.\n");
                }
            } else {
                printf("Header 'Date:' non trovato nella risposta.\n");
            }

        } else {
            printf("Nessun dato ricevuto (Timeout o connessione chiusa).\n");
        }

        WIFI_CloseClientConnection(1);

    } else {
        printf("Errore apertura socket TCP 80.\n");
    }
}


void IncrementTime(void) {
    orario.seconds++;
    if (orario.seconds >= 60) {
        orario.seconds = 0; orario.minutes++;
        if (orario.minutes >= 60) {
            orario.minutes = 0; orario.hours++;
            if (orario.hours >= 24){
            	orario.hours = 0;

            	//cambio giorno
            	orario.weekday++;
            	if (orario.weekday > 6) orario.weekday = 0; // Dopo Domenica(6) torna Lunedì(0)
            }
        }
    }
}

void CheckWeatherForecast(void) {
    uint8_t ipAddr[4] = {0};
    uint8_t recvBuf[512];
    uint16_t receivedLen = 0, sentLen = 0;
    LOG(("\n--- METEO CHECK ---\n"));

    if (WIFI_GetHostAddress("api.open-meteo.com", ipAddr, 4) != WIFI_STATUS_OK) {
        LOG(("DNS Error API\n"));
        return;
    }

    if (WIFI_OpenClientConnection(1, WIFI_TCP_PROTOCOL, "api.open-meteo.com", ipAddr, 80, 0) == WIFI_STATUS_OK) {
        char request[512];
        sprintf(request, "GET /v1/forecast?latitude=%s&longitude=%s&daily=precipitation_probability_max&timezone=auto&forecast_days=2 HTTP/1.1\r\nHost: api.open-meteo.com\r\nConnection: close\r\n\r\n", LAT, LON);

        WIFI_SendData(1, (uint8_t*)request, strlen(request), &sentLen, 1000);

        if (WIFI_ReceiveData(1, recvBuf, 511, &receivedLen, 2000) == WIFI_STATUS_OK) {
            if (receivedLen > 0) {
                recvBuf[receivedLen] = 0;
                char *prob_start = strstr((char*) recvBuf, "precipitation_probability_max\":[");
                if (prob_start) {
                    char *ptr = strchr(prob_start, '[');
                    if (ptr) {
                        ptr++;
                        buf_rain_today = atoi(ptr); // Salva nel buffer OGGI

                        char *comma = strchr(ptr, ',');
                        if (comma) {
                            buf_rain_tomorrow = atoi(comma + 1); // Salva nel buffer DOMANI
                            LOG(("API Success: Buffer Oggi=%d%%, Domani=%d%%\n", buf_rain_today, buf_rain_tomorrow));
                        }
                    }
                }
            }
        }
        WIFI_CloseClientConnection(1);
    } else {
        LOG(("Connect Error API\n"));
    }
}

void SaveSettings(void) {
    FlashData_t dataToSave;

    // Copia variabili globali nella struct temporanea
    dataToSave.magic_number = CONFIG_MAGIC;
    dataToSave.saved_start_hour = cfg_start_hour;
    dataToSave.saved_duration_min = cfg_duration_min;
    dataToSave.saved_days_mask = cfg_days_mask;
    dataToSave.saved_smart_active = smart_mode_active;
    dataToSave.saved_smart_threshold = smart_threshold;
    dataToSave.saved_sim_mode = sim_mode_active;
    dataToSave.saved_sim_rain_prob = sim_rain_prob;

    // 2. SCRITTURA IN FLASH (Standard STM32)
    HAL_FLASH_Unlock();

    // Cancellazione pagina
    FLASH_EraseInitTypeDef EraseInitStruct;
    uint32_t PageError;
    EraseInitStruct.TypeErase   = FLASH_TYPEERASE_PAGES;
    EraseInitStruct.Banks       = FLASH_BANK_2;
    EraseInitStruct.Page        = 255;
    EraseInitStruct.NbPages     = 1;

    if (HAL_FLASHEx_Erase(&EraseInitStruct, &PageError) != HAL_OK) {
        printf("Errore Erase Flash!\n");
        HAL_FLASH_Lock();
        return;
    }

    // Scrittura dati
    uint64_t *data_ptr = (uint64_t*)&dataToSave;
    int iterations = (sizeof(FlashData_t) + 7) / 8; // Arrotonda a blocchi da 8 byte

    uint32_t address = FLASH_STORAGE_ADDR;
    for (int i = 0; i < iterations; i++) {
        HAL_FLASH_Program(FLASH_TYPEPROGRAM_DOUBLEWORD, address, data_ptr[i]);
        address += 8;
    }

    HAL_FLASH_Lock();
    printf("Configurazione Salvata in Flash!\n");
}

void LoadSettings(void) {
    // Puntatore diretto alla memoria Flash
    FlashData_t *loadedData = (FlashData_t*)FLASH_STORAGE_ADDR;

    // Controllo se c'è il Magic Number
    if (loadedData->magic_number == CONFIG_MAGIC) {
        printf("Trovata configurazione salvata. Caricamento...\n");

        cfg_start_hour = loadedData->saved_start_hour;
        cfg_duration_min = loadedData->saved_duration_min;
        cfg_days_mask = loadedData->saved_days_mask;
        smart_mode_active = loadedData->saved_smart_active;
        smart_threshold = loadedData->saved_smart_threshold;
        sim_mode_active = loadedData->saved_sim_mode;
        sim_rain_prob = loadedData->saved_sim_rain_prob;

    } else {
        printf("Memoria vuota o corrotta. Uso valori di default.\n");
    }
}


void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin) {
  if (GPIO_Pin == GPIO_PIN_1) SPI_WIFI_ISR();
}

void SPI3_IRQHandler(void) {
  HAL_SPI_IRQHandler(&hspi);
}

#ifdef USE_FULL_ASSERT
void assert_failed(uint8_t* file, uint32_t line) {}
#endif
