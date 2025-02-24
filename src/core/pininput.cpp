// license:GPLv3+
// non

#include "core/stdafx.h"
#include "renderer/VRDevice.h"
#include "imgui/imgui_impl_sdl3.h"

#ifdef __LIBVPINBALL__
#include "standalone/VPinballLib.h"
#endif

#define INPUT_BUFFER_SIZE MAX_KEYQUEUE_SIZE

// Énumération des types de périphériques d'entrée
enum InputDeviceType {
    KEYBOARD = 0,
    MOUSE = 1,
    TOUCH = 2,
    GAMEPAD = 3,
    JOYSTICK = 4,
    OPD = 5 // Open Pinball Device
};

// Structure pour stocker les informations sur un périphérique d'entrée
struct InputDevice {
    SDL_JoystickID id;         // ID SDL du périphérique
    std::string uniqueId;      // ID unique stable pour la persistance
    InputDeviceType type;      // Type de périphérique
    std::string name;          // Nom du périphérique
    bool enabled;              // État activé/désactivé
    bool hasRumble;            // Support du retour de force
    
    // Informations spécifiques au périphérique
    int numAxes;               // Nombre d'axes
    int numButtons;            // Nombre de boutons
    std::unordered_map<int, int> buttonMapping; // Mapping personnalisé des boutons
    std::unordered_map<int, int> axisMapping;   // Mapping personnalisé des axes
};

// Énumération des types d'événements d'entrée
enum EventType {
    KEYBOARD = 0,
    MOUSE_BUTTON = 1,
    MOUSE_MOTION = 2,
    TOUCH = 3,
    TOUCH_REGION = 4,
    GAMEPAD_BUTTON = 5,
    GAMEPAD_AXIS = 6,
    JOYSTICK_BUTTON = 7,
    JOYSTICK_AXIS = 8
};

// Structure d'événement d'entrée unifiée
struct InputEvent {
    EventType type;        // Type d'événement
    Sint32 code;           // Code de la touche/bouton/axe
    Sint32 value;          // Valeur (pressé/relâché, position de l'axe...)
    SDL_JoystickID deviceId; // ID du périphérique
    int x, y;              // Coordonnées pour les événements souris/touche

PinInput::PinInput()
 : m_leftkey_down_usec(0)
 , m_leftkey_down_frame(0)
 , m_leftkey_down_usec_rotate_to_end(0)
 , m_leftkey_down_frame_rotate_to_end(0)
 , m_leftkey_down_usec_EOS(0)
 , m_leftkey_down_frame_EOS(0)
 , m_lastclick_ballcontrol_usec(0)
 , m_num_joy(0)
 , uShockType(0)
 , m_mixerKeyDown(false)
 , m_mixerKeyUp(false)
 , m_linearPlunger(false)
 , m_plunger_retract(false)
 , m_joycustom1key(0)
 , m_joycustom2key(0)
 , m_joycustom3key(0)
 , m_joycustom4key(0)
 , m_firedautostart(0)
 , m_exit_stamp(0)
 , m_pressed_start(false)
 , m_as_down(false)
 , m_as_didonce(false)
 , m_tilt_updown(false)
 , m_head(0)
 , m_tail(0)
 , m_joylflipkey(0)
 , m_joyrflipkey(0)
 , m_joystagedlflipkey(0)
 , m_joystagedrflipkey(0)
 , m_joylmagnasave(0)
 , m_joyrmagnasave(0)
 , m_joyplungerkey(0)
 , m_joystartgamekey(0)
 , m_joyexitgamekey(0)
 , m_joyaddcreditkey(0) 
 , m_joyaddcreditkey2(0)
 , m_joyframecount(0)
 , m_joyvolumeup(0)
 , m_joyvolumedown(0)
 , m_joylefttilt(0)
 , m_joycentertilt(0)
 , m_joyrighttilt(0)
 , m_joypmbuyin(0) 
 , m_joypmcoin3(0)
 , m_joypmcoin4(0)
 , m_joypmcoindoor(0)
 , m_joypmcancel(0)
 , m_joypmdown(0)
 , m_joypmup(0)
 , m_joypmenter(0)
 , m_joydebugballs(0)
 , m_joydebugger(0)
 , m_joylockbar(0)
 , m_joymechtilt(0) 
 , m_joycustom1(0)
 , m_joycustom2(0)
 , m_joycustom3(0)
 , m_joycustom4(0) 
 , m_joytablerecenter(0)
 , m_joytableup(0)
 , m_joytabledown(0)
 , m_joypause(0)
 , m_joytweak(0) 
 , m_deadz(0)
 , m_override_default_buttons(false)
 , m_plunger_reverse(false)
 , m_disable_esc(false)
 , m_lr_axis_reverse(false)
 , m_ud_axis_reverse(false)
 , m_enableMouseInPlayer(true)
 , m_cameraModeAltKey(false)
 , m_enableCameraModeFlyAround(false)
 , m_cameraMode(0)
 , m_nextKeyPressedTime(0)
 , m_inputApi(PI_SDL)
 , m_rumbleMode(0)
{
   ZeroMemory(m_oldMouseButtonState, sizeof(m_oldMouseButtonState));
   ZeroMemory(m_eventQueue, sizeof(m_eventQueue));
   ZeroMemory(m_keyPressedState, sizeof(m_keyPressedState));

   m_plunger_axis = 3;
   m_plunger_speed_axis = 0;
   m_lr_axis = 1;
   m_ud_axis = 2;

   m_joylflipkey = SDL_GAMEPAD_BUTTON_LEFT_SHOULDER + 1;
   m_joyrflipkey = SDL_GAMEPAD_BUTTON_RIGHT_SHOULDER + 1;
   m_joylmagnasave = SDL_GAMEPAD_BUTTON_LEFT_STICK + 1;
   m_joyrmagnasave = SDL_GAMEPAD_BUTTON_RIGHT_STICK + 1;
   m_joyplungerkey = SDL_GAMEPAD_BUTTON_DPAD_DOWN + 1;
   m_joyaddcreditkey = SDL_GAMEPAD_BUTTON_SOUTH + 1;
   m_joystartgamekey = SDL_GAMEPAD_BUTTON_EAST + 1;
   m_joypmcancel = SDL_GAMEPAD_BUTTON_NORTH + 1;
   m_joyframecount = SDL_GAMEPAD_BUTTON_WEST + 1;
   m_joycentertilt = SDL_GAMEPAD_BUTTON_DPAD_UP + 1;
   m_joylefttilt = SDL_GAMEPAD_BUTTON_DPAD_LEFT + 1;
   m_joyrighttilt = SDL_GAMEPAD_BUTTON_DPAD_RIGHT + 1;
   m_joylockbar = SDL_GAMEPAD_BUTTON_GUIDE + 1;

   m_first_stamp = msec();
   
   // Initialisation des structures et mappings
   for (int i = 0; i < eCKeys; i++)
      m_keyPressedState[i] = false;
   
   m_connectedDevices.clear();
}

PinInput::~PinInput()
{
   CloseAllDevices();
   TerminateOpenPinballDevices();
}

void PinInput::CloseAllDevices()
{
   if (m_pSDLGamePad)
      SDL_CloseGamepad(m_pSDLGamePad);
   m_pSDLGamePad = nullptr;

   if (m_pSDLRumbleDevice)
      SDL_CloseHaptic(m_pSDLRumbleDevice);
   m_pSDLRumbleDevice = nullptr;

   if (m_pSDLJoystick)
      SDL_CloseJoystick(m_pSDLJoystick); 
   m_pSDLJoystick = nullptr;
}

void PinInput::LoadSettings(const Settings& settings)
{
   m_lr_axis = settings.LoadValueWithDefault(Settings::Player, "LRAxis"s, m_lr_axis);
   m_ud_axis = settings.LoadValueWithDefault(Settings::Player, "UDAxis"s, m_ud_axis);
   m_lr_axis_reverse = settings.LoadValueWithDefault(Settings::Player, "LRAxisFlip"s, m_lr_axis_reverse);
   m_ud_axis_reverse = settings.LoadValueWithDefault(Settings::Player, "UDAxisFlip"s, m_ud_axis_reverse);
   m_plunger_axis = settings.LoadValueWithDefault(Settings::Player, "PlungerAxis"s, m_plunger_axis);
   m_plunger_speed_axis = settings.LoadValueWithDefault(Settings::Player, "PlungerSpeedAxis"s, m_plunger_speed_axis);
   m_plunger_reverse = settings.LoadValueWithDefault(Settings::Player, "ReversePlungerAxis"s, m_plunger_reverse);
   m_plunger_retract = settings.LoadValueWithDefault(Settings::Player, "PlungerRetract"s, m_plunger_retract);
   m_override_default_buttons = settings.LoadValueWithDefault(Settings::Player, "PBWDefaultLayout"s, m_override_default_buttons);
   m_disable_esc = settings.LoadValueWithDefault(Settings::Player, "DisableESC"s, m_disable_esc);
   m_joylflipkey = settings.LoadValueWithDefault(Settings::Player, "JoyLFlipKey"s, m_joylflipkey);
   m_joyrflipkey = settings.LoadValueWithDefault(Settings::Player, "JoyRFlipKey"s, m_joyrflipkey);
   m_joystagedlflipkey = settings.LoadValueWithDefault(Settings::Player, "JoyStagedLFlipKey"s, m_joystagedlflipkey);
   m_joystagedrflipkey = settings.LoadValueWithDefault(Settings::Player, "JoyStagedRFlipKey"s, m_joystagedrflipkey);
   m_joyplungerkey = settings.LoadValueWithDefault(Settings::Player, "JoyPlungerKey"s, m_joyplungerkey);
   m_joyaddcreditkey = settings.LoadValueWithDefault(Settings::Player, "JoyAddCreditKey"s, m_joyaddcreditkey);
   m_joyaddcreditkey2 = settings.LoadValueWithDefault(Settings::Player, "JoyAddCredit2Key"s, m_joyaddcreditkey2);
   m_joylmagnasave = settings.LoadValueWithDefault(Settings::Player, "JoyLMagnaSave"s, m_joylmagnasave);
   m_joyrmagnasave = settings.LoadValueWithDefault(Settings::Player, "JoyRMagnaSave"s, m_joyrmagnasave);
   m_joystartgamekey = settings.LoadValueWithDefault(Settings::Player, "JoyStartGameKey"s, m_joystartgamekey);
   m_joyframecount = settings.LoadValueWithDefault(Settings::Player, "JoyFrameCount"s, m_joyframecount);
   m_joyexitgamekey = settings.LoadValueWithDefault(Settings::Player, "JoyExitGameKey"s, m_joyexitgamekey);
   m_joyvolumeup = settings.LoadValueWithDefault(Settings::Player, "JoyVolumeUp"s, m_joyvolumeup);
   m_joyvolumedown = settings.LoadValueWithDefault(Settings::Player, "JoyVolumeDown"s, m_joyvolumedown);
   m_joylefttilt = settings.LoadValueWithDefault(Settings::Player, "JoyLTiltKey"s, m_joylefttilt);
   m_joycentertilt = settings.LoadValueWithDefault(Settings::Player, "JoyCTiltKey"s, m_joycentertilt);
   m_joyrighttilt = settings.LoadValueWithDefault(Settings::Player, "JoyRTiltKey"s, m_joyrighttilt);
   m_joypmbuyin = settings.LoadValueWithDefault(Settings::Player, "JoyPMBuyIn"s, m_joypmbuyin);
   m_joypmcoin3 = settings.LoadValueWithDefault(Settings::Player, "JoyPMCoin3"s, m_joypmcoin3);
   m_joypmcoin4 = settings.LoadValueWithDefault(Settings::Player, "JoyPMCoin4"s, m_joypmcoin4);
   m_joypmcoindoor = settings.LoadValueWithDefault(Settings::Player, "JoyPMCoinDoor"s, m_joypmcoindoor);
   m_joypmcancel = settings.LoadValueWithDefault(Settings::Player, "JoyPMCancel"s, m_joypmcancel);
   m_joypmdown = settings.LoadValueWithDefault(Settings::Player, "JoyPMDown"s, m_joypmdown);
   m_joypmup = settings.LoadValueWithDefault(Settings::Player, "JoyPMUp"s, m_joypmup);
   m_joypmenter = settings.LoadValueWithDefault(Settings::Player, "JoyPMEnter"s, m_joypmenter);
   m_joycustom1 = settings.LoadValueWithDefault(Settings::Player, "JoyCustom1"s, m_joycustom1);
   m_joycustom1key = settings.LoadValueWithDefault(Settings::Player, "JoyCustom1Key"s, m_joycustom1key);
   m_joycustom2 = settings.LoadValueWithDefault(Settings::Player, "JoyCustom2"s, m_joycustom2);
   m_joycustom2key = settings.LoadValueWithDefault(Settings::Player, "JoyCustom2Key"s, m_joycustom2key);
   m_joycustom3 = settings.LoadValueWithDefault(Settings::Player, "JoyCustom3"s, m_joycustom3);
   m_joycustom3key = settings.LoadValueWithDefault(Settings::Player, "JoyCustom3Key"s, m_joycustom3key);
   m_joycustom4 = settings.LoadValueWithDefault(Settings::Player, "JoyCustom4"s, m_joycustom4);
   m_joycustom4key = settings.LoadValueWithDefault(Settings::Player, "JoyCustom4Key"s, m_joycustom4key);
   m_joymechtilt = settings.LoadValueWithDefault(Settings::Player, "JoyMechTiltKey"s, m_joymechtilt);
   m_joydebugballs = settings.LoadValueWithDefault(Settings::Player, "JoyDebugKey"s, m_joydebugballs);
   m_joydebugger = settings.LoadValueWithDefault(Settings::Player, "JoyDebuggerKey"s, m_joydebugger);
   m_joylockbar = settings.LoadValueWithDefault(Settings::Player, "JoyLockbarKey"s, m_joylockbar);
   m_joypause = settings.LoadValueWithDefault(Settings::Player, "JoyPauseKey"s, m_joypause);
   m_joytweak = settings.LoadValueWithDefault(Settings::Player, "JoyTweakKey"s, m_joytweak);
   m_joytablerecenter = settings.LoadValueWithDefault(Settings::Player, "JoyTableRecenterKey"s, m_joytablerecenter);
   m_joytableup = settings.LoadValueWithDefault(Settings::Player, "JoyTableUpKey"s, m_joytableup);
   m_joytabledown = settings.LoadValueWithDefault(Settings::Player, "JoyTableDownKey"s, m_joytabledown);
   m_enableMouseInPlayer = settings.LoadValueWithDefault(Settings::Player, "EnableMouseInPlayer"s, m_enableMouseInPlayer);
   m_enableCameraModeFlyAround = settings.LoadValueWithDefault(Settings::Player, "EnableCameraModeFlyAround"s, m_enableCameraModeFlyAround);
   m_deadz = settings.LoadValueWithDefault(Settings::Player, "DeadZone"s, 0);
   m_deadz = m_deadz * JOYRANGEMX / 100;

   // Chargement des paramètres de périphériques
   for (auto& device : m_connectedDevices) {
      const string deviceState = "Device_" + device.second.uniqueId;
      device.second.enabled = settings.LoadValueWithDefault(Settings::ControllerDevices, deviceState, true);
   }
}

void PinInput::Init()
{
   if (!SDL_InitSubSystem(SDL_INIT_GAMEPAD | SDL_INIT_HAPTIC))
   {
      PLOGE << "SDL_InitSubSystem(SDL_INIT_GAMEPAD | SDL_INIT_HAPTIC) failed: " << SDL_GetError();
      exit(1);
   }
   
   // Chargement de la base de données des manettes
   string path = g_pvp->m_szMyPrefPath + "gamecontrollerdb.txt";
   if (!std::filesystem::exists(path))
      std::filesystem::copy(g_pvp->m_szMyPath + "assets" + PATH_SEPARATOR_CHAR + "Default_gamecontrollerdb.txt", path);
   int count = SDL_AddGamepadMappingsFromFile(path.c_str());
   if (count > 0) {
      PLOGI.printf("Game controller mappings added: count=%d, path=%s", count, path.c_str());
   }
   else {
      PLOGI.printf("No game controller mappings added: path=%s", path.c_str());
   }
   
   RefreshSDLDevices();
   
   for (int i = 0; i < eCKeys; i++)
      m_keyPressedState[i] = false;
   m_nextKeyPressedTime = 0;
   uShockType = USHOCKTYPE_GENERIC;

   m_rumbleMode = g_pvp->m_settings.LoadValueWithDefault(Settings::Player, "RumbleMode"s, 3);

   // initialiser les périphériques Open Pinball
   InitOpenPinballDevices();

   m_mixerKeyDown = false;
   m_mixerKeyUp = false;
}

void PinInput::UnInit()
{
   m_head = m_tail = 0;
   
   SDL_QuitSubSystem(SDL_INIT_GAMEPAD | SDL_INIT_HAPTIC);
   
   ZeroMemory(m_eventQueue, sizeof(m_eventQueue));
   
   // fermer les interfaces OpenPinballDevice
   TerminateOpenPinballDevices();
}

void PinInput::PlayRumble(const float lowFrequencySpeed, const float highFrequencySpeed, const int ms_duration)
{
   if (m_rumbleMode == 0) return;

   // Parcourir tous les périphériques avec capacité de rumble
   for (auto& device : m_connectedDevices)
   {
      if (device.second.hasRumble && device.second.enabled)
      {
         if (device.second.type == InputDeviceType::GAMEPAD && m_pSDLGamePad)
         {
            SDL_RumbleGamepad(m_pSDLGamePad, 
                             (Uint16)(saturate(lowFrequencySpeed) * 65535.f), 
                             (Uint16)(saturate(highFrequencySpeed) * 65535.f), 
                             ms_duration);
         }
         else if (device.second.type == InputDeviceType::JOYSTICK && m_pSDLRumbleDevice)
         {
            SDL_PlayHapticRumble(m_pSDLRumbleDevice, 
                                saturate(max(lowFrequencySpeed, highFrequencySpeed)), 
                                ms_duration);
         }
      }
   }
   
#ifdef __LIBVPINBALL__
   VPinballLib::RumbleData rumbleData = {
      (Uint16)(saturate(lowFrequencySpeed) * 65535.f),
      (Uint16)(saturate(highFrequencySpeed) * 65535.f),
      (Uint32)ms_duration
   };
   VPinballLib::VPinball::SendEvent(VPinballLib::Event::Rumble, &rumbleData);
#endif
}

void PinInput::RefreshSDLDevices()
{
   // Ferme tous les périphériques précédemment ouverts
   CloseAllDevices();
   m_num_joy = 0;

   // Obtient la liste de tous les périphériques connectés
   int joystick_count = 0;
   SDL_JoystickID* joystick_ids = SDL_GetJoysticks(&joystick_count);

   if (joystick_count > 0) {
      for (int idx = 0; idx < joystick_count; ++idx) {
         // Enregistre le périphérique trouvé
         PLOGI.printf("Input device found: id=%d name=%s",
            joystick_ids[idx],
            SDL_GetJoystickNameForID(joystick_ids[idx]));

         // Traite chaque périphérique comme un gamepad ou un joystick
         if (SDL_IsGamepad(joystick_ids[idx])) {
            // Saute la télécommande Apple sur les appareils iOS/tvOS
            #if defined(__APPLE__) && ((defined(TARGET_OS_IOS) && TARGET_OS_IOS) || (defined(TARGET_OS_TV) && TARGET_OS_TV))
            if (!lstrcmpi(SDL_GetGamepadNameForID(joystick_ids[idx]), "Remote")) continue;
            #endif

            // Essaie d'ouvrir comme gamepad
            m_pSDLGamePad = SDL_OpenGamepad(joystick_ids[idx]);
            if (m_pSDLGamePad) {
               m_num_joy++;

               // Ajoute le périphérique à notre collection
               InputDevice device;
               device.id = joystick_ids[idx];
               device.uniqueId = std::to_string(joystick_ids[idx]);
               device.type = InputDeviceType::GAMEPAD;
               device.name = SDL_GetGamepadNameForID(joystick_ids[idx]);
               device.enabled = true;

               SDL_PropertiesID props = SDL_GetGamepadProperties(m_pSDLGamePad);
               device.hasRumble = SDL_GetBooleanProperty(props, SDL_PROP_GAMEPAD_CAP_RUMBLE_BOOLEAN, false);

               m_connectedDevices[joystick_ids[idx]] = device;

               PLOGI.printf("Processing as Gamepad: %d axes, %d buttons, rumble=%s",
                  5,  // Standard gamepad has 6 axes
                  15, // Standard gamepad has 15 buttons
                  device.hasRumble ? "true" : "false");
            }
         } else {
            // Essaie d'ouvrir comme joystick standard
            m_pSDLJoystick = SDL_OpenJoystick(joystick_ids[idx]);
            if (m_pSDLJoystick) {
               m_num_joy++;
               
               // Ajoute le périphérique à notre collection
               InputDevice device;
               device.id = joystick_ids[idx];
               device.uniqueId = std::to_string(joystick_ids[idx]);
               device.type = InputDeviceType::JOYSTICK;
               device.name = SDL_GetJoystickNameForID(joystick_ids[idx]);
               device.enabled = true;
               device.hasRumble = false;

               // Vérifie si le joystick prend en charge le retour de force
               if (SDL_IsJoystickHaptic(m_pSDLJoystick)) {
                  m_pSDLRumbleDevice = SDL_OpenHapticFromJoystick(m_pSDLJoystick);
                  if (!SDL_InitHapticRumble(m_pSDLRumbleDevice)) {
                     ShowError(SDL_GetError());
                     SDL_CloseHaptic(m_pSDLRumbleDevice);
                     m_pSDLRumbleDevice = nullptr;
                  } else {
                     device.hasRumble = true;
                  }
               }

               m_connectedDevices[joystick_ids[idx]] = device;
               
               PLOGI.printf("Processing as Joystick: %d axes, %d buttons, rumble=%s",
                  SDL_GetNumJoystickAxes(m_pSDLJoystick),
                  SDL_GetNumJoystickButtons(m_pSDLJoystick),
                  device.hasRumble ? "true" : "false");
            }
         }
      }
   }

   // Aucun périphérique n'a été ouvert avec succès
   if (m_num_joy == 0) {
      PLOGI.printf("No game controllers or joysticks connected!");
   }

   SDL_free(joystick_ids);
}

void PinInput::PushQueue(InputEvent* const event)
{
   if ((!event) ||
       (((m_head + 1) % MAX_KEYQUEUE_SIZE) == m_tail)) // queue full?
       return;

   m_eventQueue[m_head] = *event;
   m_head = (m_head + 1) % MAX_KEYQUEUE_SIZE; // advance head of queue
}

const InputEvent* PinInput::GetTail()
{
   if (m_head == m_tail)
      return nullptr; // queue empty?

   const InputEvent* const ptr = &m_eventQueue[m_tail];
   m_tail = (m_tail + 1) % MAX_KEYQUEUE_SIZE; // advance tail of queue
   return ptr;
}

void PinInput::GetInputDeviceData()
{
   // Quand SDL Video est utilisé, les événements SDL sont traités pendant la boucle principale de messages 
   // de l'application, nous ne les traitons donc pas à nouveau ici
#ifndef ENABLE_SDL_VIDEO
   SDL_Event e;
   while (SDL_PollEvent(&e) != 0)
      HandleSDLEvent(e);
#endif

   ReadOpenPinballDevices(msec());
}

void PinInput::HandleSDLEvent(SDL_Event& e)
{
   InputEvent event;
   event.deviceId = 0;
   
   switch (e.type) 
   {
      case SDL_EVENT_KEY_DOWN:
      case SDL_EVENT_KEY_UP:
         if (e.key.repeat == 0) {
            event.type = EventType::KEYBOARD;
            event.code = e.key.key;
            event.value = e.type == SDL_EVENT_KEY_DOWN ? 1 : 0;
            PushQueue(&event);
         }
         break;

      case SDL_EVENT_MOUSE_BUTTON_DOWN:
      case SDL_EVENT_MOUSE_BUTTON_UP:
         if (m_enableMouseInPlayer) {
            event.type = EventType::MOUSE_BUTTON;
            event.code = e.button.button;
            event.value = e.type == SDL_EVENT_MOUSE_BUTTON_DOWN ? 1 : 0;
            PushQueue(&event);
            
            m_oldMouseButtonState[e.button.button < 3 ? e.button.button : 2] = 
               e.type == SDL_EVENT_MOUSE_BUTTON_DOWN ? 1 : 0;
         }
         break;

      case SDL_EVENT_MOUSE_MOTION:
         if (m_enableMouseInPlayer) {
            event.type = EventType::MOUSE_MOTION;
            event.code = 0;
            event.value = 0;
            event.x = e.motion.x;
            event.y = e.motion.y;
            PushQueue(&event);
         }
         break;

      #if (defined(__APPLE__) && (defined(TARGET_OS_IOS) && TARGET_OS_IOS)) || defined(__ANDROID__)
      case SDL_EVENT_FINGER_DOWN:
      case SDL_EVENT_FINGER_UP:
         {
            event.type = EventType::TOUCH;
            event.code = 0;
            event.value = e.type == SDL_EVENT_FINGER_DOWN ? 1 : 0;
            event.x = (int)((float)g_pplayer->m_playfieldWnd->GetWidth() * e.tfinger.x);
            event.y = (int)((float)g_pplayer->m_playfieldWnd->GetHeight() * e.tfinger.y);
            PushQueue(&event);
            
            // Traitement spécifique pour les régions tactiles
            POINT point = { event.x, event.y };
            for (unsigned int i = 0; i < MAX_TOUCHREGION; ++i)
            {
                if ((g_pplayer->m_touchregion_pressed[i] != (e.type == SDL_EVENT_FINGER_DOWN))
                   && Intersect(touchregion[i], g_pplayer->m_playfieldWnd->GetWidth(), g_pplayer->m_playfieldWnd->GetHeight(), point,
                      fmodf(g_pplayer->m_ptable->mViewSetups[g_pplayer->m_ptable->m_BG_current_set].mViewportRotation, 360.0f) != 0.f))
                {
                   g_pplayer->m_touchregion_pressed[i] = (e.type == SDL_EVENT_FINGER_DOWN);
                   InputEvent touchEvent;
                   touchEvent.type = EventType::TOUCH_REGION;
                   touchEvent.code = touchkeymap[i];
                   touchEvent.value = g_pplayer->m_touchregion_pressed[i] ? 1 : 0;
                   touchEvent.deviceId = 0;
                   PushQueue(&touchEvent);
                }
            }
         }
         break;
      #endif

      case SDL_EVENT_GAMEPAD_ADDED:
      case SDL_EVENT_GAMEPAD_REMOVED:
         RefreshSDLDevices();
         break;
         
      case SDL_EVENT_GAMEPAD_AXIS_MOTION:
         {
            // Vérifier si le périphérique est activé
            auto deviceIt = m_connectedDevices.find(e.gaxis.which);
            if (deviceIt == m_connectedDevices.end() || !deviceIt->second.enabled) {
               break;
            }
            
            // Appliquer la zone morte
            int value = e.gaxis.value;
            if (e.gaxis.axis < 6) {
               if (abs(value) <= m_deadz) {
                  value = 0;
               } else if (value < 0) {
                  value += m_deadz;
               } else {
                  value -= m_deadz;
               }
               
               event.type = EventType::GAMEPAD_AXIS;
               event.code = e.gaxis.axis;
               event.value = value;
               event.deviceId = e.gaxis.which;
               PushQueue(&event);
            }
         }
         break;
         
      case SDL_EVENT_GAMEPAD_BUTTON_DOWN:
      case SDL_EVENT_GAMEPAD_BUTTON_UP:
         {
            // Vérifier si le périphérique est activé
            auto deviceIt = m_connectedDevices.find(e.gbutton.which);
            if (deviceIt == m_connectedDevices.end() || !deviceIt->second.enabled) {
               break;
            }
            
            event.type = EventType::GAMEPAD_BUTTON;
            event.code = e.gbutton.button;
            event.value = e.type == SDL_EVENT_GAMEPAD_BUTTON_DOWN ? 1 : 0;
            event.deviceId = e.gbutton.which;
            PushQueue(&event);
         }
         break;
         
      case SDL_EVENT_JOYSTICK_ADDED:
      case SDL_EVENT_JOYSTICK_REMOVED:
         RefreshSDLDevices();
         break;
         
      case SDL_EVENT_JOYSTICK_AXIS_MOTION:
         {
            // Vérifier si le périphérique est activé
            auto deviceIt = m_connectedDevices.find(e.jaxis.which);
            if (deviceIt == m_connectedDevices.end() || !deviceIt->second.enabled) {
               break;
            }
            
            // Appliquer la zone morte
            int value = e.jaxis.value;
            if (e.jaxis.axis < 6) {
               if (abs(value) <= m_deadz) {
                  value = 0;
               } else if (value < 0) {
                  value += m_deadz;
               } else {
                  value -= m_deadz;
               }
               
               event.type = EventType::JOYSTICK_AXIS;
               event.code = e.jaxis.axis;
               event.value = value;
               event.deviceId = e.jaxis.which;
               PushQueue(&event);
            }
         }
         break;
         
      case SDL_EVENT_JOYSTICK_BUTTON_DOWN:
      case SDL_EVENT_JOYSTICK_BUTTON_UP:
         {
            // Vérifier si le périphérique est activé
            auto deviceIt = m_connectedDevices.find(e.jbutton.which);
            if (deviceIt == m_connectedDevices.end() || !deviceIt->second.enabled) {
               break;
            }
            
            event.type = EventType::JOYSTICK_BUTTON;
            event.code = e.jbutton.button;
            event.value = e.type == SDL_EVENT_JOYSTICK_BUTTON_DOWN ? 1 : 0;
            event.deviceId = e.jbutton.which;
            PushQueue(&event);
         }
         break;
   }
}

void PinInput::ProcessCameraKeys(const InputEvent* input)
{
    if (input->type != EventType::KEYBOARD) {
        return;
    }
    
    switch (input->code)
    {
    case SDLK_UP:
    case SDLK_DOWN:
        {
            const bool up = (input->code == SDLK_UP);
            if (input->value != 0)
            {
                if (!m_cameraModeAltKey)
                    g_pplayer->m_renderer->m_cam.y += up ? 10.0f : -10.0f;
                else
                    g_pplayer->m_renderer->m_cam.z += up ? 10.0f : -10.0f;

                m_cameraMode = up ? 1 : 2;
            }
            else
                m_cameraMode = 0;
        }
        break;
    case SDLK_RIGHT:
    case SDLK_LEFT:
        {
            const bool right = (input->code == SDLK_RIGHT);
            if (input->value != 0)
            {
                if (!m_cameraModeAltKey)
                    g_pplayer->m_renderer->m_cam.x += right ? -10.0f : 10.0f;
                else
                    g_pplayer->m_renderer->m_inc += right ? -0.01f : 0.01f;

                m_cameraMode = right ? 3 : 4;
            }
            else
                m_cameraMode = 0;
        }
        break;
    case SDLK_LALT:
        m_cameraModeAltKey = (input->value != 0);
        break;
    default:
        break;
    }
}

void PinInput::ProcessKeys(int curr_time_msec)
{
    if (!g_pplayer || !g_pplayer->m_ptable) return; // only if player is running
    g_pplayer->m_logicProfiler.OnProcessInput();

    if (curr_time_msec >= 0)
    {
        // Check if autostart is enabled.
        if (g_pplayer->m_ptable->m_tblAutoStartEnabled)
            // Update autostart.
            Autostart(g_pplayer->m_ptable->m_tblAutoStart, g_pplayer->m_ptable->m_tblAutoStartRetry, curr_time_msec);

        ButtonExit(g_pplayer->m_ptable->m_tblExitConfirm, curr_time_msec);
    }
    else
        curr_time_msec = -curr_time_msec; // due to special encoding to not do the stuff above

    // Check if we've been initialized.
    if (m_firedautostart == 0)
        m_firedautostart = curr_time_msec;

    GetInputDeviceData();

    ReadOpenPinballDevices(curr_time_msec);

    // Camera/Light tweaking mode (F6) incl. fly-around parameters
    if (g_pplayer->m_liveUI->IsTweakMode())
    {
        if (m_head == m_tail) // key queue empty, so just continue using the old pressed key
        {
            if ((curr_time_msec - m_nextKeyPressedTime) > 10) // reduce update rate
            {
                m_nextKeyPressedTime = curr_time_msec;

                // Flying
                if (m_cameraMode == 1)
                {
                    if (!m_cameraModeAltKey)
                        g_pplayer->m_renderer->m_cam.y += 10.0f;
                    else
                        g_pplayer->m_renderer->m_cam.z += 10.0f;
                }
                else if (m_cameraMode == 2)
                {
                    if (!m_cameraModeAltKey)
                        g_pplayer->m_renderer->m_cam.y -= 10.0f;
                    else
                        g_pplayer->m_renderer->m_cam.z -= 10.0f;
                }
                else if (m_cameraMode == 3)
                {
                    if (!m_cameraModeAltKey)
                        g_pplayer->m_renderer->m_cam.x -= 10.0f;
                    else
                        g_pplayer->m_renderer->m_inc -= 0.01f;
                }
                else if (m_cameraMode == 4)
                {
                    if (!m_cameraModeAltKey)
                        g_pplayer->m_renderer->m_cam.x += 10.0f;
                    else
                        g_pplayer->m_renderer->m_inc += 0.01f;
                }

                // Table tweaks, continuous actions
                if (g_pplayer->m_liveUI->IsTweakMode())
                    for (int i = 0; i < eCKeys; i++)
                        if (m_keyPressedState[i])
                            g_pplayer->m_liveUI->OnTweakModeEvent(0, g_pplayer->m_rgKeys[i]);
            }
            return;
        }
    }

    // Réinitialiser les états des touches si nous ne sommes pas la fenêtre active
    #ifdef _WIN32
    if (m_focusHWnd != GetForegroundWindow())
    {
        for (int i = 0; i < eCKeys; i++)
        {
            m_keyPressedState[i] = false;
        }
    }
    #endif

    const InputEvent* input;
    while ((input = GetTail()))
    {
        ProcessInputEvent(input, curr_time_msec);
    }
}

void PinInput::ProcessInputEvent(const InputEvent* input, int curr_time_msec)
{
    if (g_pplayer && g_pplayer->m_liveUI->IsTweakMode() && m_enableCameraModeFlyAround && input->type == EventType::KEYBOARD)
    {
        ProcessCameraKeys(input);
    }

    switch (input->type)
    {
    case EventType::KEYBOARD:
        if (g_pplayer == nullptr || !g_pplayer->m_liveUI->HasKeyboardCapture())
        {
            ProcessKeyboardInput(input, curr_time_msec);
        }
        break;

    case EventType::MOUSE_BUTTON:
        if (g_pplayer && !g_pplayer->m_liveUI->HasMouseCapture())
        {
            ProcessMouseInput(input);
        }
        break;

    case EventType::TOUCH:
    case EventType::TOUCH_REGION:
        ProcessTouchInput(input);
        break;

    case EventType::GAMEPAD_BUTTON:
    case EventType::JOYSTICK_BUTTON:
        ProcessButtonInput(input, curr_time_msec);
        break;

    case EventType::GAMEPAD_AXIS:
    case EventType::JOYSTICK_AXIS:
        ProcessAxisInput(input, input->deviceId);
        break;
    }
}

void PinInput::ProcessKeyboardInput(const InputEvent* input, int curr_time_msec)
{
    // Traitement des touches du clavier
    if (input->code == SDLK_F5) // FrameCount
    {
        if (input->value != 0)
            g_pplayer->m_liveUI->ToggleFPS();
    }
    else if (input->code == SDLK_PAUSE) // Pause
    {
        if (input->value != 0)
            g_pplayer->SetPlayState(!g_pplayer->IsPlaying());
    }
    else if (input->code == SDLK_F6) // Tweak
    {
        if (input->value != 0)
        {
            if (g_pplayer->m_liveUI->IsTweakMode())
                g_pplayer->m_liveUI->HideUI();
            else
                g_pplayer->m_liveUI->OpenTweakMode();
        }
    }
    else if (input->code == SDLK_F11) // Enable3D
    {
        if (input->value != 0)
        {
            // Gestion du mode 3D (voir code original)
            // ... (ajouter la logique 3D ici)
        }
    }
    else if (input->code == SDLK_F3) // DBGBalls
    {
        if (input->value != 0)
            g_pplayer->m_debugBalls = !g_pplayer->m_debugBalls;
    }
    else if (input->code == SDLK_F2) // Debugger
    {
        if (Started() || !g_pplayer->m_ptable->m_tblAutoStartEnabled)
        {
            if (input->value != 0)
            {
                m_first_stamp = curr_time_msec;
                m_exit_stamp = curr_time_msec;
            }
            else
            {
                m_exit_stamp = 0;
                g_pplayer->m_showDebugger = true;
            }
        }
    }
    else if ((input->code == SDLK_ESCAPE && !m_disable_esc) || input->code == SDLK_q) // Exit
    {
        // Check if we have started a game yet and UI is not open
        if (!g_pplayer->m_liveUI->IsOpened() && (Started() || !g_pplayer->m_ptable->m_tblAutoStartEnabled))
        {
            if (input->value != 0)
            {
                m_first_stamp = curr_time_msec;
                m_exit_stamp = curr_time_msec;
            }
            else
            {
                g_pplayer->SetCloseState(Player::CS_USER_INPUT);
                m_exit_stamp = 0;
#ifdef __STANDALONE__
                if (input->code == SDLK_q)
                    g_pplayer->SetCloseState(Player::CS_CLOSE_APP);
#endif
            }
        }
    }
    else
    {
        // Pour les autres touches, convertir en événement standard
        const int dispid = (input->value != 0) ? DISPID_GameEvents_KeyDown : DISPID_GameEvents_KeyUp;
        FireKeyEvent(dispid, input->code);
    }
}

void PinInput::ProcessMouseInput(const InputEvent* input)
{
    if (!g_pplayer->m_throwBalls && !g_pplayer->m_ballControl)
    {
        const int mouseButton = input->code;
        const bool buttonDown = (input->value != 0);
        
        // Convertir bouton de souris en action appropriée
        if (m_joylflipkey == mouseButton + 25) // Utilise l'ID de bouton de souris
            FireKeyEvent(buttonDown ? DISPID_GameEvents_KeyDown : DISPID_GameEvents_KeyUp, g_pplayer->m_rgKeys[eLeftFlipperKey]);
        else if (m_joyrflipkey == mouseButton + 25)
            FireKeyEvent(buttonDown ? DISPID_GameEvents_KeyDown : DISPID_GameEvents_KeyUp, g_pplayer->m_rgKeys[eRightFlipperKey]);
        else if (m_joyplungerkey == mouseButton + 25)
            FireKeyEvent(buttonDown ? DISPID_GameEvents_KeyDown : DISPID_GameEvents_KeyUp, g_pplayer->m_rgKeys[ePlungerKey]);
        else if (m_joylefttilt == mouseButton + 25)
            FireKeyEvent(buttonDown ? DISPID_GameEvents_KeyDown : DISPID_GameEvents_KeyUp, g_pplayer->m_rgKeys[eLeftTiltKey]);
        else if (m_joyrighttilt == mouseButton + 25)
            FireKeyEvent(buttonDown ? DISPID_GameEvents_KeyDown : DISPID_GameEvents_KeyUp, g_pplayer->m_rgKeys[eRightTiltKey]);
        else if (m_joycentertilt == mouseButton + 25)
            FireKeyEvent(buttonDown ? DISPID_GameEvents_KeyDown : DISPID_GameEvents_KeyUp, g_pplayer->m_rgKeys[eCenterTiltKey]);
    }
}

void PinInput::ProcessTouchInput(const InputEvent* input)
{
    if (input->type == EventType::TOUCH_REGION)
    {
        const int dispid = (input->value != 0) ? DISPID_GameEvents_KeyDown : DISPID_GameEvents_KeyUp;
        FireKeyEvent(dispid, input->code);
    }
}

void PinInput::ProcessButtonInput(const InputEvent* input, int curr_time_msec)
{
    const bool isGamepad = (input->type == EventType::GAMEPAD_BUTTON);
    const int button = input->code + 1; // +1 car les boutons commencent à 1 dans la configuration
    const bool buttonDown = (input->value != 0);
    const int dispid = buttonDown ? DISPID_GameEvents_KeyDown : DISPID_GameEvents_KeyUp;
    const bool start = ((curr_time_msec - m_firedautostart) > g_pplayer->m_ptable->m_tblAutoStart) || m_pressed_start || Started();
    
    // Map button to action
    if (m_joylflipkey == button)      FireKeyEvent(dispid, g_pplayer->m_rgKeys[eLeftFlipperKey]);
    if (m_joyrflipkey == button)      FireKeyEvent(dispid, g_pplayer->m_rgKeys[eRightFlipperKey]);
    if (m_joystagedlflipkey == button) FireKeyEvent(dispid, g_pplayer->m_rgKeys[eStagedLeftFlipperKey]);
    if (m_joystagedrflipkey == button) FireKeyEvent(dispid, g_pplayer->m_rgKeys[eStagedRightFlipperKey]);
    if (m_joyplungerkey == button)    FireKeyEvent(dispid, g_pplayer->m_rgKeys[ePlungerKey]);
    if (m_joyaddcreditkey == button)  FireKeyEvent(dispid, g_pplayer->m_rgKeys[eAddCreditKey]);
    if (m_joyaddcreditkey2 == button) FireKeyEvent(dispid, g_pplayer->m_rgKeys[eAddCreditKey2]);
    if (m_joylmagnasave == button)    FireKeyEvent(dispid, g_pplayer->m_rgKeys[eLeftMagnaSave]);
    if (m_joyrmagnasave == button)    FireKeyEvent(dispid, g_pplayer->m_rgKeys[eRightMagnaSave]);
    if (m_joytablerecenter == button) FireKeyEvent(dispid, g_pplayer->m_rgKeys[eTableRecenter]);
    if (m_joytableup == button)       FireKeyEvent(dispid, g_pplayer->m_rgKeys[eTableUp]);
    if (m_joytabledown == button)     FireKeyEvent(dispid, g_pplayer->m_rgKeys[eTableDown]);
    
    if (m_joystartgamekey == button && start)
    {
        m_pressed_start = true;
        FireKeyEvent(dispid, g_pplayer->m_rgKeys[eStartGameKey]);
    }
    
    if (m_joyexitgamekey == button && buttonDown)
    {
        g_pplayer->SetCloseState(Player::CS_USER_INPUT);
    }
    
    if (m_joyframecount == button && buttonDown)
    {
        g_pplayer->m_liveUI->ToggleFPS();
    }
    
    if (m_joyvolumeup == button)   FireKeyEvent(dispid, g_pplayer->m_rgKeys[eVolumeUp]);
    if (m_joyvolumedown == button) FireKeyEvent(dispid, g_pplayer->m_rgKeys[eVolumeDown]);
    if (m_joylefttilt == button)   FireKeyEvent(dispid, g_pplayer->m_rgKeys[eLeftTiltKey]);
    if (m_joycentertilt == button) FireKeyEvent(dispid, g_pplayer->m_rgKeys[eCenterTiltKey]);
    if (m_joyrighttilt == button)  FireKeyEvent(dispid, g_pplayer->m_rgKeys[eRightTiltKey]);
    if (m_joymechtilt == button)   FireKeyEvent(dispid, g_pplayer->m_rgKeys[eMechanicalTilt]);
    if (m_joydebugballs == button) FireKeyEvent(dispid, g_pplayer->m_rgKeys[eDBGBalls]);
    if (m_joydebugger == button)   FireKeyEvent(dispid, g_pplayer->m_rgKeys[eDebugger]);
    if (m_joylockbar == button)    FireKeyEvent(dispid, g_pplayer->m_rgKeys[eLockbarKey]);
    if (m_joypause == button)      FireKeyEvent(dispid, g_pplayer->m_rgKeys[ePause]);
    if (m_joytweak == button)      FireKeyEvent(dispid, g_pplayer->m_rgKeys[eTweak]);
    if (m_joycustom1 == button)    FireKeyEvent(dispid, m_joycustom1key);
    if (m_joycustom2 == button)    FireKeyEvent(dispid, m_joycustom2key);
    if (m_joycustom3 == button)    FireKeyEvent(dispid, m_joycustom3key);
    if (m_joycustom4 == button)    FireKeyEvent(dispid, m_joycustom4key);
}

void PinInput::ProcessAxisInput(const InputEvent* input, int deviceId)
{
    // Vérifie si le périphérique est activé
    auto deviceIt = m_connectedDevices.find(deviceId);
    if (deviceIt == m_connectedDevices.end() || !deviceIt->second.enabled) {
        return;
    }

    const int axis = input->code;
    const int value = input->value;
    
    // Récupérer le type de périphérique pour appliquer des configurations spécifiques
    const InputDeviceType deviceType = deviceIt->second.type;
    const bool isGamepad = (deviceType == InputDeviceType::GAMEPAD);
    
    // Traiter l'axe en fonction de sa configuration et du type de périphérique
    
    // Cas 1: Axes de mouvement (Nudge X/Y)
    if (axis == m_lr_axis - 1) // -1 car dans le code SDL les axes commencent à 0
    {
        // Axe gauche-droite pour le nudge
        if (isGamepad || deviceType == InputDeviceType::JOYSTICK) 
        {
            // Appliquer des transformations en fonction du type de périphérique
            if (deviceType == InputDeviceType::GAMEPAD)
                g_pplayer->SetNudgeX(!m_lr_axis_reverse ? -value : value, deviceId);
            else // JOYSTICK ou autre
                g_pplayer->SetNudgeX(!m_lr_axis_reverse ? -value : value, deviceId);
        }
    }
    else if (axis == m_ud_axis - 1)
    {
        // Axe haut-bas pour le nudge
        if (isGamepad || deviceType == InputDeviceType::JOYSTICK)
        {
            if (deviceType == InputDeviceType::GAMEPAD)
                g_pplayer->SetNudgeY(!m_ud_axis_reverse ? value : -value, deviceId);
            else // JOYSTICK ou autre
                g_pplayer->SetNudgeY(!m_ud_axis_reverse ? value : -value, deviceId);
        }
    }
    
    // Cas 2: Plunger et ses contrôles
    else if (axis == m_plunger_axis - 1)
    {
        // Axe pour le plunger
        if (deviceType == InputDeviceType::GAMEPAD)
        {
            // Les gamepads ont souvent besoin d'un traitement particulier pour le plunger
            // Par exemple, les gâchettes (triggers) ont une plage différente
            g_pplayer->MechPlungerIn(!m_plunger_reverse ? -value : value, deviceId);
        }
        else
        {
            // Joysticks ou autres périphériques
            g_pplayer->MechPlungerIn(!m_plunger_reverse ? -value : value, deviceId);
        }
    }
    else if (axis == m_plunger_speed_axis - 1)
    {
        // Axe pour la vitesse du plunger
        g_pplayer->MechPlungerSpeedIn(!m_plunger_reverse ? -value : value, deviceId);
    }
    
    // Traitement spécifique pour chaque type de manette
    
    // Pour PinballWizard ou VirtuaPin
    if (uShockType == USHOCKTYPE_PBWIZARD || uShockType == USHOCKTYPE_VIRTUAPIN)
    {
        // Ces contrôleurs nécessitent un traitement personnalisé
        if (axis == 0) // X-axis traditionnellement
        {
            if (m_lr_axis != 0)
                g_pplayer->SetNudgeX(-value, deviceId); // Rotation pour correspondre au Pinball Wizard
        }
        else if (axis == 1) // Y-axis traditionnellement
        {
            if (m_ud_axis != 0)
                g_pplayer->SetNudgeY(value, deviceId); // Rotation pour correspondre au Pinball Wizard
        }
        else if (axis == 2) // Z-axis souvent utilisé pour le plunger
        {
            if (m_plunger_axis != 0)
                g_pplayer->MechPlungerIn(-value, deviceId);
        }
    }
    
    // Pour UltraCade
    else if (uShockType == USHOCKTYPE_ULTRACADE)
    {
        // Traitement spécifique à UltraCade
        if (axis == 0)
        {
            if (m_lr_axis != 0)
                g_pplayer->SetNudgeY(-value, deviceId); // Rotation pour correspondre au joystick
        }
        else if (axis == 1)
        {
            if (m_ud_axis != 0)
                g_pplayer->SetNudgeX(-value, deviceId); // Rotation pour correspondre au joystick
        }
        else if (axis == 2)
        {
            g_pplayer->MechPlungerIn(value, deviceId);
        }
    }
    
    // Pour SideWinder
    else if (uShockType == USHOCKTYPE_SIDEWINDER)
    {
        if (axis == 0 && m_lr_axis != 0)
            g_pplayer->SetNudgeX(!m_lr_axis_reverse ? value : -value, deviceId);
        else if (axis == 1 && m_ud_axis != 0)
            g_pplayer->SetNudgeY(!m_ud_axis_reverse ? value : -value, deviceId);
        else if (axis == 2)
            g_pplayer->MechPlungerIn(!m_plunger_reverse ? -value : value, deviceId);
    }
    
    // Log pour le debug si nécessaire
    // PLOGI.printf("Axis: %d, Value: %d, Device: %d, Type: %d", axis, value, deviceId, (int)deviceType);
}
