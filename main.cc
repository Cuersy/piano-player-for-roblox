#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <windows.h>

BOOL CALLBACK enum_windows_proc(HWND hwnd, LPARAM lparam) {
    char win_title[256];
    GetWindowTextA(hwnd, win_title, sizeof(win_title));
    if (strstr(win_title, "Roblox") != nullptr) {
        SetActiveWindow(hwnd);
        SetForegroundWindow(hwnd);
        std::cout << "focused to roblox window!" << std::endl;
        return FALSE;
    }
    return TRUE;
}

void set_roblox_focus() {
    EnumWindows(enum_windows_proc, 0);
}

void key_action(char key_param, bool press) {
    SHORT key = VkKeyScan(key_param);
    UINT mapped_key = MapVirtualKey(LOBYTE(key), MAPVK_VK_TO_VSC);
    INPUT input = { 0 };
    input.type = INPUT_KEYBOARD;
    input.ki.dwFlags = press ? KEYEVENTF_SCANCODE : (KEYEVENTF_SCANCODE | KEYEVENTF_KEYUP);
    input.ki.wScan = mapped_key;
    SendInput(1, &input, sizeof(INPUT));
}

bool is_paused = false;
void toggle_pause() {
    is_paused = !is_paused;
    std::cout << (is_paused ? "basma lan." : "basom lan.") << std::endl;
}

void process_key_sequence(const std::string& sequence, int interval) {
    for (size_t i = 0; i < sequence.size(); i++) {
        if (sequence[i] == '[') {
            std::vector<char> pressed_keys;
            i++;
            while (i < sequence.size() && sequence[i] != ']') {
                if (sequence[i] != ' ') pressed_keys.push_back(sequence[i]);
                i++;
            }
            for (char c : pressed_keys) key_action(c, true);
            Sleep(interval / 2);
            for (char c : pressed_keys) key_action(c, false);
        }
        else if (sequence[i] == '|') {
            int pause_duration = interval;
            while (i + 1 < sequence.size() && sequence[i + 1] == ' ') {
                pause_duration += interval / 2;
                i++;
            }
            Sleep(pause_duration);
        }
        else if (sequence[i] != ' ') {
            key_action(sequence[i], true);
            Sleep(interval / 5);
            key_action(sequence[i], false);
        }
    }
}

int main() {
    int bpm;
    std::cout << "Enter BPM(if u don't know just search on google) : ";
    std::cin >> bpm;
    if (bpm <= 0) {
        std::cerr << "bpm value must be greater than 0" << std::endl;
        while (1) {}
    }
    
    set_roblox_focus();
    int interval = 60000 / bpm;

    std::ifstream file("keys.txt");
    if (!file.is_open()) {
        std::cerr << "i cant find keys.txt" << std::endl;
        while (1) {}
    }

    std::string line;
    while (std::getline(file, line)) {
        if (GetAsyncKeyState(0x50) & 0x8000) {
            toggle_pause();
            Sleep(100);
        }
        if (is_paused) {
            Sleep(50);
            continue;
        }
        process_key_sequence(line, interval);
        Sleep(interval);
    }
    file.close();
    return 0;
}
