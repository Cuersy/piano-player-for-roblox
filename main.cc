#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <windows.h>
#include <algorithm>

using namespace std;

BOOL CALLBACK screenhndlr(HWND hwnd, LPARAM lParam) {
    char windowTitle[256];
    GetWindowTextA(hwnd, windowTitle, sizeof(windowTitle));
    if (strstr(windowTitle, "Roblox") != nullptr) {
        SetActiveWindow(hwnd);
        SetForegroundWindow(hwnd);
        std::cout << "Focused to roblox window!" << std::endl;
        return FALSE;
    }
    return TRUE;
}

void setroblox() {
    EnumWindows(screenhndlr, 0);
}

void press(char keyParam) {
    SHORT key = VkKeyScan(keyParam);
    UINT mapped = MapVirtualKey(LOBYTE(key), MAPVK_VK_TO_VSC);
    INPUT input = { 0 };
    input.type = INPUT_KEYBOARD;
    input.ki.dwFlags = KEYEVENTF_SCANCODE;
    input.ki.wScan = mapped;
    SendInput(1, &input, sizeof(INPUT));
}

void release(char keyParam) {
    SHORT key = VkKeyScan(keyParam);
    UINT mapped = MapVirtualKey(LOBYTE(key), MAPVK_VK_TO_VSC);
    INPUT input = { 0 };
    input.type = INPUT_KEYBOARD;
    input.ki.dwFlags = KEYEVENTF_SCANCODE | KEYEVENTF_KEYUP;
    input.ki.wScan = mapped;
    SendInput(1, &input, sizeof(INPUT));
}

int main() {
    setroblox();

    ifstream file("keys.txt");
    if (!file.is_open()) {
        cerr << "i cant find keys.txt" << endl;
        return 1;
    }

    string allkeys((istreambuf_iterator<char>(file)), istreambuf_iterator<char>());
    file.close();
    allkeys.erase(remove(allkeys.begin(), allkeys.end(), '\n'), allkeys.end());
    allkeys.erase(remove(allkeys.begin(), allkeys.end(), '\r'), allkeys.end());

    int interval = 0;
    size_t i = 0;

    while (i < allkeys.size()) {
        if (allkeys.substr(i, 5) == "<bpm>") {
            size_t end_bpm = allkeys.find("</bpm>", i);
            if (end_bpm != string::npos) {
                string bpmstr = allkeys.substr(i + 5, end_bpm - (i + 5));
                int newbpm = stoi(bpmstr);
                interval = 60000 / newbpm;
                i = end_bpm + 6;
                continue;
            }
        }

        if (interval > 0) {
            if (allkeys[i] == '[') {
                vector<char> pressedvector;
                i++;
                while (i < allkeys.size() && allkeys[i] != ']') {
                    if (allkeys[i] != ' ') pressedvector.push_back(allkeys[i]);
                    i++;
                }
                for (char c : pressedvector) press(c);
                Sleep(interval / 2.5);
                for (char c : pressedvector) release(c);
            }
            else if (allkeys[i] == '|') {
                int pauseDuration = interval;
                while (i + 1 < allkeys.size() && allkeys[i + 1] == ' ') {
                    pauseDuration += interval / 2;
                    i++;
                }
                Sleep(pauseDuration);
            }
            else if (allkeys[i] != ' ') {
                press(allkeys[i]);
                Sleep(interval / 5);
                release(allkeys[i]);
            }
        }
        i++;
    }

    return 0;
}
