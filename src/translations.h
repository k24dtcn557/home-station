#ifndef TRANSLATIONS_H
#define TRANSLATIONS_H

// Language support
enum Language { LANG_EN = 0, LANG_FR = 1, LANG_VI = 2 };

struct LocalizedStrings {
  const char* temp_placeholder;
  const char* feels_like_temp;
  const char* seven_day_forecast;
  const char* hourly_forecast;
  const char* today;
  const char* now;
  const char* am;
  const char* pm;
  const char* noon;
  const char* invalid_hour;
  const char* brightness;
  const char* location;
  const char* use_fahrenheit;
  const char* use_24hr;
  const char* save;
  const char* cancel;
  const char* close;
  const char* location_btn;
  const char* reset_wifi;
  const char* reset;
  const char* change_location;
  const char* aura_settings;
  const char* city;
  const char* search_results;
  const char* city_placeholder;
  const char* wifi_config;
  const char* reset_confirmation;
  const char* language_label;
  const char* weekdays[7];
  const char* use_night_mode;
  const char* nav_home;
  const char* nav_weather;
  const char* nav_settings;
  const char* climate_title;
  const char* climate_indoor_air;
  const char* climate_humidity;
  const char* climate_read_failed;
};

#define DEFAULT_CAPTIVE_SSID "Aura"

static const LocalizedStrings strings_en = {
  "--°C", "Feels Like", "SEVEN DAY FORECAST", "HOURLY FORECAST",
  "Today", "Now", "am", "pm", "Noon", "Invalid hour",
  "Brightness:", "Location:", "Use °F:", "24hr:",
  "Save", "Cancel", "Close", "Location", "Reset Wi-Fi",
  "Reset", "Change Location", "Aura Settings",
  "City:", "Search Results", "e.g. London",
  "Wi-Fi Configuration:\n\n"
  "Please connect your\n"
  "phone or laptop to the\n"
  "temporary Wi-Fi access\n point "
  DEFAULT_CAPTIVE_SSID
  "\n"
  "to configure.\n\n"
  "If you don't see a \n"
  "configuration screen \n"
  "after connecting,\n"
  "visit http://192.168.4.1\n"
  "in your web browser.",
  "Are you sure you want to reset "
  "Wi-Fi credentials?\n\n"
  "You'll need to reconnect to the Wifi SSID " DEFAULT_CAPTIVE_SSID
  " with your phone or browser to "
  "reconfigure Wi-Fi credentials.",
  "Language:",
  {"Sun", "Mon", "Tues", "Wed", "Thurs", "Fri", "Sat"},
  "Dim screen at night",
  "Home", "Weather", "Settings",
  "TEMPERATURE", "Indoor Air", "Humidity", "DHT12 read failed"
};

static const LocalizedStrings strings_fr = {
  "--°C", "Ressenti", "PRÉVISIONS 7 JOURS", "PRÉVISIONS HORAIRES",
  "Aujourd'hui", "Maintenant", "h", "h", "Midi", "Heure invalide",
  "Luminosité:", "Lieu:", "Utiliser °F:", "24h:",
  "Sauvegarder", "Annuler", "Fermer", "Lieu", "Wi-Fi",
  "Réinitialiser", "Changer de lieu", "Paramètres Aura",
  "Ville:", "Résultats de recherche", "ex. Paris",
  "Configuration Wi-Fi:\n\n"
  "Connectez votre téléphone\n"
  "ou ordinateur portable au\n"
  "point d'accès Wi-Fi\n"
  "temporaire "
  DEFAULT_CAPTIVE_SSID
  "\n"
  "pour configurer.\n\n"
  "Si vous ne voyez pas\n"
  "d'écran de configuration\n"
  "après connexion, visitez\n"
  "http://192.168.4.1\n"
  "dans votre navigateur.",
  "Êtes-vous sûr de vouloir\n"
  "réinitialiser les\n"
  "identifiants Wi-Fi?\n\n"
  "Vous devrez vous reconnecter\n"
  "au SSID " DEFAULT_CAPTIVE_SSID
  " avec votre téléphone ou\n"
  "navigateur pour reconfigurer\n"
  "les identifiants Wi-Fi.",
  "Langue:",
  {"Dim", "Lun", "Mar", "Mer", "Jeu", "Ven", "Sam"},
  "Nuit écran",
  "Accueil", "Météo", "Paramètres",
  "TEMPÉRATURE", "Air intérieur", "Humidité", "Échec lecture DHT12"
};

static const LocalizedStrings strings_vi = {
  "--°C", "Cam giac", "DU BAO 7 NGAY", "DU BAO THEO GIO",
  "Hom nay", "Bay gio", "SA", "CH", "Trua", "Gio khong hop le",
  "Do sang:", "Vi tri:", "Dung °F:", "24h:",
  "Luu", "Huy", "Dong", "Vi tri", "Dat lai Wi-Fi",
  "Dat lai", "Doi vi tri", "Cai dat",
  "Thanh pho:", "Ket qua tim kiem", "vd: Ha Noi",
  "Cau hinh Wi-Fi:\n\n"
  "Vui long ket noi dien\n"
  "thoai hoac laptop toi\n"
  "diem phat Wi-Fi tam thoi\n "
  DEFAULT_CAPTIVE_SSID
  "\n"
  "de cau hinh.\n\n"
  "Neu khong thay man hinh\n"
  "cau hinh xuat hien,\n"
  "truy cap dia chi\n"
  "http://192.168.4.1\n"
  "tren trinh duyet web.",
  "Ban co chac chan muon\n"
  "dat lai thong tin Wi-Fi?\n\n"
  "Ban se can ket noi lai\n"
  "SSID " DEFAULT_CAPTIVE_SSID
  " bang dien thoai hoac\n"
  "trinh duyet de cau hinh lai.",
  "Ngon ngu:",
  {"CN", "T2", "T3", "T4", "T5", "T6", "T7"},
  "Giam sang ban dem",
  "Trang chu", "Thoi tiet", "Cai dat",
  "NHIET DO", "Khong Khi Trong Nha", "Do Am", "Loi doc DHT12"
};

static const LocalizedStrings* get_strings(Language current_language) {
  switch (current_language) {
    case LANG_EN: return &strings_en;
    case LANG_FR: return &strings_fr;
    case LANG_VI: return &strings_vi;
    default: return &strings_vi;
  }
}

#endif // TRANSLATIONS_H