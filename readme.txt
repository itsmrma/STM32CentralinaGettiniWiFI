/**
  @page Smart_Garden STM32 Smart Garden Project
  
  @verbatim
  ******************************************************************************
  * @file    readme.txt 
  * @author  Matteo Benassi, Lorenzo Cavallini, Nicolò Lodi, Pietro Savioli
  * @brief   Descrizione del progetto STM32 Smart Garden.
  ******************************************************************************
  *
  * Progetto sviluppato per il corso di Fondamenti di Elettronica.
  *
  ******************************************************************************
  @endverbatim

@par Descrizione del Progetto

Questo progetto implementa un sistema di irrigazione intelligente basato sulla 
scheda B-L475E-IOT01A (Discovery kit for IoT Node).

Il sistema agisce come un controller IoT che decide autonomamente se e quando 
innaffiare, basandosi sia su una pianificazione oraria configurabile dall'utente, 
sia sulle condizioni ambientali reali e previste.

Funzionalità principali:
1. **Controllo Valvole Elettroattuate**: Il firmware pilota dei relè (collegati ai GPIO) 
   che aprono o chiudono le valvole elettroattuate per la gestione dei gettini 
   di irrigazione.
2. **Web Server Integrato**: Interfaccia utente accessibile via browser (Smartphone/PC) 
   per monitorare lo stato e configurare i parametri (ora avvio, durata, giorni attivi).
3. **Logica "Smart" Meteo**:
   - Scarica le previsioni meteo (probabilità di pioggia) tramite API Open-Meteo.
   - Sospende automaticamente l'irrigazione pianificata se la probabilità di 
     pioggia supera una soglia critica (es. 60%), risparmiando acqua.
4. **Sincronizzazione Oraria**: Utilizza il protocollo Time (RFC 868) via TCP 
   per ottenere l'ora esatta da server NIST e mantenere il clock interno sincronizzato.
5. **Monitoraggio Ambientale**: Legge e mostra i dati dai sensori on-board:
   - Temperatura e Umidità (HTS221)
   - Pressione Atmosferica (LPS22HB)

@par Struttura del Software e Crediti

Il software è composto dalla logica applicativa sviluppata dagli studenti e dai 
driver di basso livello forniti dal produttore.

File principali:
 - main.c: Logica principale, Web Server, gestione sensori e logica irrigazione (Sviluppato dagli studenti).
 - webpage.h: Contiene l'HTML/CSS/JS dell'interfaccia web.
 - secrets.h: File di configurazione per le credenziali di rete.
 
Driver WiFi:
 - es_wifi.c / es_wifi.h: Driver per il modulo WiFi Inventek.
   *Autore*: STMicroelectronics (MCD Application Team).
   Nota: Il driver è stato adattato per gestire correttamente la ricezione asincrona TCP.

@par Hardware e Configurazione

  - **Piattaforma**: STM32L475VG (B-L475E-IOT01A).
  - **Attuatori**: I relè per le valvole elettroattuate devono essere collegati 
    ai pin configurati (es. GPIO Output su connettori Arduino/Pmod).
  
  @note Configurazione WiFi:
    Per collegare la scheda alla rete, aprire il file **secrets.h** presente nella 
    cartella dell'applicazione e inserire le proprie credenziali:
    - `#define SSID "IlTuoSSID"`
    - `#define PASSWORD "LaTuaPassword"`

  @note Monitoraggio Seriale:
    Collegare la scheda via USB. Aprire un terminale (es. TeraTerm, Putty):
    - BaudRate: 115200
    - Data: 8 bit, Parity: None, Stop: 1 bit
    Il terminale mostrerà l'indirizzo IP assegnato alla scheda e i log di sistema.

@par Istruzioni per l'uso

 1. Modificare `secrets.h` con le credenziali della rete WiFi locale.
 2. Compilare e caricare il firmware sulla scheda.
 3. Aprire il terminale seriale per visualizzare l'IP ottenuto (es. 192.168.1.x).
 4. Collegare lo Smartphone o PC alla stessa rete WiFi.
 5. Aprire il browser e navigare all'indirizzo IP della scheda.
 6. Dall'interfaccia web è possibile:
    - Vedere i dati dei sensori.
    - Impostare orario e giorni di irrigazione.
    - Attivare la "Modalità Simulazione" per testare la logica di blocco 
      irrigazione simulando una probabilità di pioggia manuale.

 */