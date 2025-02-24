// license:GPLv3+

#pragma once

#include <SDL3/SDL.h>
#include <SDL3/SDL_gamepad.h>
#include <SDL3/SDL_haptic.h>
#include <map>
#include <unordered_map>
#include <string>
#include <memory>

#define MAX_KEYQUEUE_SIZE 32

#if MAX_KEYQUEUE_SIZE & (MAX_KEYQUEUE_SIZE-1)
#error Note that MAX_KEYQUEUE_SIZE must be power of 2
#endif

// Types de contrôleurs spécifiques
#define USHOCKTYPE_PBWIZARD   1
#define USHOCKTYPE_ULTRACADE  2
#define USHOCKTYPE_SIDEWINDER 3
#define USHOCKTYPE_VIRTUAPIN  4
#define USHOCKTYPE_GENERIC    5

// Joystick axis normalized input range.
#define JOYRANGEMN (-65536)
#define JOYRANGEMX (+65536)
#define JOYRANGE ((JOYRANGEMX) - (JOYRANGEMN) + 1)

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
    EVENT_KEYBOARD = 0,
    EVENT_MOUSE_BUTTON = 1,
    EVENT_MOUSE_MOTION = 2,
    EVENT_TOUCH = 3,
    EVENT_TOUCH_REGION = 4,
    EVENT_GAMEPAD_BUTTON = 5,
    EVENT_GAMEPAD_AXIS = 6,
    EVENT_JOYSTICK_BUTTON = 7,
    EVENT_JOYSTICK_AXIS = 8
};

// Structure d'événement d'entrée unifiée
struct InputEvent {
    EventType type;        // Type d'événement
    Sint32 code;           // Code de la touche/bouton/axe
    Sint32 value;          // Valeur (pressé/relâché, position de l'axe...)
    SDL_JoystickID deviceId; // ID du périphérique
    int x, y;              // Coordonnées pour les événements souris/touche
};

// Open Pinball Device context (défini dans le module d'implémentation OPD)
class OpenPinDevContext;

class PinInput
{
public:
    PinInput();
    void LoadSettings(const Settings& settings);
    ~PinInput();

    void Init();
    void UnInit();

    enum InputAPI
    {
        PI_DIRECTINPUT, PI_XINPUT, PI_SDL, PI_GAMECONTROLLER
    };
    InputAPI GetInputAPI() const { return m_inputApi; }

    // Envoi synchronisé des événements de touche au moteur de simulation
    void FireKeyEvent(const int dispid, int keycode);

    void PushQueue(InputEvent* const event);
    const InputEvent* GetTail();

    void ProcessCameraKeys(const InputEvent* input);
    void ProcessKeys(int curr_time_msec);
    void ProcessInputEvent(const InputEvent* input, int curr_time_msec);
    void ProcessKeyboardInput(const InputEvent* input, int curr_time_msec);
    void ProcessMouseInput(const InputEvent* input);
    void ProcessTouchInput(const InputEvent* input);
    void ProcessButtonInput(const InputEvent* input, int curr_time_msec);
    void ProcessAxisInput(const InputEvent* input, int deviceId);

    // Speed: 0..1
    void PlayRumble(const float lowFrequencySpeed, const float highFrequencySpeed, const int ms_duration);

    int GetNextKey();

    void GetInputDeviceData();
    void HandleSDLEvent(SDL_Event& e);
    void RefreshSDLDevices();
    void CloseAllDevices();

    uint64_t m_leftkey_down_usec;
    unsigned int m_leftkey_down_frame;
    uint64_t m_leftkey_down_usec_rotate_to_end;
    unsigned int m_leftkey_down_frame_rotate_to_end;
    uint64_t m_leftkey_down_usec_EOS;
    unsigned int m_leftkey_down_frame_EOS;
    uint64_t m_lastclick_ballcontrol_usec;

    int m_num_joy;
    int uShockType;

    bool m_mixerKeyDown;
    bool m_mixerKeyUp;

    bool m_linearPlunger;
    bool m_plunger_retract; // enable 1s retract phase for button/key plunger

    int m_joycustom1key, m_joycustom2key, m_joycustom3key, m_joycustom4key;

private:
    int Started();

    void Autostart(const U32 msecs, const U32 retry_msecs, const U32 curr_time_msec);
    void ButtonExit(const U32 msecs, const U32 curr_time_msec);

    void Joy(const unsigned int n, const int updown, const bool start);

    void InitOpenPinballDevices();
    void ReadOpenPinballDevices(const U32 cur_time_msec);
    void TerminateOpenPinballDevices();

    // Open Pinball Device context.
    OpenPinDevContext *m_OpenPinDevContext = nullptr;

    // Open Pinball Device button status, for detecting button up/down events
    uint32_t m_openPinDev_generic_buttons = 0;
    uint32_t m_openPinDev_pinball_buttons = 0;
    bool m_openPinDev_flipper_l = false, m_openPinDev_flipper_r = false;

    BYTE m_oldMouseButtonState[3];

    U32 m_firedautostart;

    U32 m_first_stamp;
    U32 m_exit_stamp;

    bool m_pressed_start;

    bool m_as_down;
    bool m_as_didonce;

    bool m_tilt_updown;

    InputEvent m_eventQueue[MAX_KEYQUEUE_SIZE]; // file circulaire d'événements d'entrée

    int m_head; // head==tail signifie vide, (head+1)%MAX_KEYQUEUE_SIZE == tail signifie plein
    int m_tail; // Ce sont des indices entiers dans eventQueue et doivent être dans le domaine 0..MAX_KEYQUEUE_SIZE-1

    // Attributions d'axes - ces correspondances sont liées à l'index dans les listes déroulantes
    // de sélection d'axes dans la boîte de dialogue Keys:
    //
    //   0 = Désactivé
    //   1 = X
    //   2 = Y
    //   3 = Z
    //   4 = rX
    //   5 = rY
    //   6 = rZ
    //   7 = Slider 1
    //   8 = Slider 2
    //   9 = Open Pinball Device (sélectionne le mapping d'entrée correspondant à la même fonction que l'axe affecté)
    //
    int m_plunger_axis, m_plunger_speed_axis, m_lr_axis, m_ud_axis;
    int m_joylflipkey, m_joyrflipkey, m_joystagedlflipkey, m_joystagedrflipkey, m_joylmagnasave, m_joyrmagnasave, m_joyplungerkey, m_joystartgamekey, m_joyexitgamekey, m_joyaddcreditkey;
    int m_joyaddcreditkey2, m_joyframecount, m_joyvolumeup, m_joyvolumedown, m_joylefttilt, m_joycentertilt, m_joyrighttilt, m_joypmbuyin;
    int m_joypmcoin3, m_joypmcoin4, m_joypmcoindoor, m_joypmcancel, m_joypmdown, m_joypmup, m_joypmenter, m_joydebugballs, m_joydebugger, m_joylockbar, m_joymechtilt;
    int m_joycustom1, m_joycustom2, m_joycustom3, m_joycustom4;
    int m_joytablerecenter, m_joytableup, m_joytabledown, m_joypause, m_joytweak;
    int m_deadz;

    bool m_override_default_buttons, m_plunger_reverse, m_disable_esc, m_lr_axis_reverse, m_ud_axis_reverse;
    bool m_enableMouseInPlayer;

    bool m_cameraModeAltKey;
    bool m_enableCameraModeFlyAround;

    int m_cameraMode;
    bool m_keyPressedState[28]; // =EnumAssignKeys::eCKeys
    DWORD m_nextKeyPressedTime;

    InputAPI m_inputApi;
    int m_rumbleMode; // 0=Off, 1=Table only, 2=Generic only, 3=Table with generic as fallback

    static constexpr int m_LeftMouseButtonID = 25;
    static constexpr int m_RightMouseButtonID = 26;
    static constexpr int m_MiddleMouseButtonID = 27;

    // Périphériques d'entrée SDL
    SDL_Gamepad* m_pSDLGamePad = nullptr;
    SDL_Joystick* m_pSDLJoystick = nullptr; 
    SDL_Haptic* m_pSDLRumbleDevice = nullptr;
    
    // Collection des périphériques connectés
    std::map<SDL_JoystickID, InputDevice> m_connectedDevices;
    
#ifdef _WIN32
    HWND m_focusHWnd = nullptr;
#endif
};
