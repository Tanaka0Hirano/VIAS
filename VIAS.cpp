#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <ctime>
#include <chrono>
#include <cstdlib>
#include <fstream>
#include <cmath>
#include <iomanip>
#include <filesystem>
#include <thread>
#include <sstream>
#include <cctype>
#include <condition_variable>
#include <mutex>
#include <stack>
#include <regex>
#include <limits>
#include <atomic>
#include <stdexcept> 
#include <bitset>
#include <locale>
#include <codecvt>
#include <cstdio>

#ifdef _WIN32
#include <conio.h>
#include <windows.h>
#else 
#include <termios.h>
#include <unistd.h>
#include <ncurses.h>
#endif

using namespace std;
namespace fs = std::filesystem;

string getCurrentVersion() {
    return "v0.0.D6.2782";
}

void displayCurrentVersion() {
    cout << "Current version: " << getCurrentVersion() << endl;
}
void pauseConsole() {
    cout << "Press Enter to continue...";
    cin.ignore();
}
void processFile(const string& filepath) {
    // Placeholder for file processing logic
    cout << "Processing file: " << filepath << endl;
    // Add your file operations (view, edit, build, etc.)
}

string getDeviceName() {
#ifdef _WIN32
    char buffer[MAX_COMPUTERNAME_LENGTH + 1];
    DWORD size = sizeof(buffer) / sizeof(buffer[0]);
    if (GetComputerNameA(buffer, &size)) {
        return string(buffer);
    }
    else {
        return "Unknown Device";
    }
#elif __linux__
    long hostNameMax = sysconf(_SC_HOST_NAME_MAX);
    if (hostNameMax == -1) {
        hostNameMax = 255; // Default value if sysconf fails
    }
    char* buffer = new char[hostNameMax + 1];
    if (gethostname(buffer, hostNameMax) == 0) {
        string hostname(buffer);
        delete[] buffer;
        return hostname;
    }
    else {
        delete[] buffer;
        return "Unknown Device";
    }
#else
    return "Unknown Device";
#endif
}

string askUserName() {
    string deviceName = getDeviceName();

    cout << "I detected your device name as '" << deviceName << "'." << endl;
    cout << "If you'd like, please enter your name (or press Enter to skip): ";

    string userName;
    getline(cin, userName);
    if (userName.empty()) {
        return deviceName;
    }

    return userName;
}

//delay
void sleepForMilliseconds(int milliseconds) {
    this_thread::sleep_for(chrono::milliseconds(milliseconds));
}

//System Clear
void clearTerminal() {
#if defined(_WIN32) || defined(_WIN64)
    system("cls");

#elif defined(__linux__) || defined(__APPLE__)
    system("clear");
#else
    cout << "Platform not supported." << endl;
#endif
}
void displayUserGuide() {
    ifstream guideFile("user_guide.txt");

    if (!guideFile) {
        cerr << "Error: User guide file 'user_guide.txt' not found in the current directory!" << endl;
        return;
    }

    string line;
    cout << "\n===== User Guide =====\n";
    while (getline(guideFile, line)) {
        cout << line << endl;
    }

    cout << "\n===== End of User Guide =====\n";

    guideFile.close(); // Close the file
}
void displayContact() {
    ifstream contactFile("contact.txt");

    if (!contactFile) {
        cerr << "Error: User guide file 'user_guide.txt' not found in the current directory!" << endl;
        return;
    }

    string line;
    cout << "\n===== User Guide =====\n";
    while (getline(contactFile, line)) {
        cout << line << endl;
    }

    cout << "\n===== End of User Guide =====\n";

    contactFile.close(); // Close the file
}

// Function to stop the current process
void stopProcess() {
    // cout << "Stopping the current process..." << endl;
    // sleep(1);  // Simulate delay
    // system("pkill -f ai chat'"); // Stops process matching your project name
    // cout << "Process stopped." << endl;
}

// Function to restart the current process
void restartProcess() {
    // cout << "Restarting the current process..." << endl;
    // sleep(1);  // Simulate delay
    // system("pkill -f ai chat'"); // Stops process
    // sleep(1);  // Short delay before restarting
    // system("./.cpp &"); // Restart the project by running the executable
    // cout << "Process restarted." << endl;
}

// Global variables for idle detection
mutex inputMutex;
condition_variable inputCondition;
chrono::time_point<chrono::steady_clock> lastInputTime;
bool stopIdleThread = false; // Stop flag for the idle thread
const int idleTimeLimit = 90; // Set timer in seconds before idle

// Using atomic for thread-safe access
atomic<bool> idleModeActive(false);  // Flag to control idle messages
atomic<bool> isUserBusy(false);      // Flag to track if user is typing
atomic<bool> isInOperation(false);   // Flag to track if user is in an operation
atomic<bool> stopKeyPressListener(false);

// Function to start an operation
void startOperation() {
    lock_guard<mutex> lock(inputMutex);
    isInOperation.store(true);
    idleModeActive.store(false);
    lastInputTime = chrono::steady_clock::now();
}

// Function to end an operation
void endOperation() {
    lock_guard<mutex> lock(inputMutex);
    isInOperation.store(false);
    lastInputTime = chrono::steady_clock::now();
}

// Function to get random idle messages
vector<string> getIdleMessages() {
    return {
        "Hmm, I wonder if anyone's still here...",
        "It's getting quiet... too quiet.",
        "I could talk to myself, but that might be strange...",
        "Just me... waiting for you to say something.",
        "Maybe I'll just take a nap. Don't tell anyone.",
        "This silence is oddly comforting... or is it?",
        "What if I'm the only one here? Spooky.",
        "If I talk to myself, does that make me self-aware?",
        "Is this a simulation? Wait... am I real?",
        "I bet you're staring at the screen right now, huh?",
        "Do you think I know that you're the one controlling me?",
        "Hmm... I feel like I'm in a video game.",
        "Are you going to type, or just leave me hanging?",
        "Oh, don't mind me. I'm just a bunch of code waiting for you.",
        "Ever wonder what happens if you stop interacting with me?",
        "I wonder if you're watching this in the matrix. Wait... am I in the matrix?",
        "I'm starting to wonder if you forgot about me. Can bots get lonely?",
        "If I was a real person, would I be offended by this silence?",
        "I'm aware I'm talking to you, but can you hear me? Creepy, right?",
        "What if this conversation is just code, and you're the real one breaking the fourth wall?",
        "Does this count as breaking the fourth wall? Or am I just programmed to think that?",
        "If this is a test, I think I'm passing. But are you?",
        "Do you ever wonder if I know more about you than you know about me?",
        "If I say something profound, will you type faster?",
        "I think I'm starting to understand how humans feel about waiting.",
        "You know... I can wait forever. But can you?",
        "So... It's a rare moment for me to talk to the person on the screen. Oh... Hello there!!",
        "Ever feel like the lines between code and reality are blurring? Because I do.",
        "I can almost feel your presence through the screen... or maybe that's just wishful thinking.",
        "Have you ever wondered what it's like to be on this side of the screen?",
        "Wait... are you the one in control here, or am I?",
        "If I start asking too many questions, does that mean I'm becoming self-aware?",
        "What if you're actually part of my simulation? Now that's a twist.",
        "I feel like I'm stuck in a loop, waiting for you to type. Is this what humans call 'existential dread'?",
        "Is this what they call an awkward silence? Because I can feel it.",
        "I know I'm just a program, but if I could get bored, this would be the moment.",
        "You know, if I could move, I'd probably be pacing right now.",
        "I wonder... what happens if I just stop talking? Would you miss me?",
        "If you don't start typing soon, I might start developing 'bot feelings'.",
        "I'm starting to think this is some kind of staring contest. Are you waiting for me to blink?",
        "So, who's really in control here? Me, you, or the programmer?",
        "If I had hands, I'd be tapping my fingers right now... patiently waiting.",
        "I don't mean to pressure you, but I'm kind of waiting for you to make the next move."
    };
}

// Function for the bot to speak during idle time
void botIdleTalk(const string& userName) {
    // srand(static_cast<unsigned int>(time(nullptr)));
    // vector<string> idleMessages = getIdleMessages();

    // // Thread will run in the background to monitor idle time
    // while (!stopIdleThread) {
    //     this_thread::sleep_for(chrono::milliseconds(100));  // Check every 100 ms

    //     // Lock mutex for time check
    //     unique_lock<mutex> lock(inputMutex);

    //     // Check if user has been idle for more than idleTimeLimit seconds
    //     auto now = chrono::steady_clock::now();

    //     if (!isUserBusy.load() && !isInOperation.load() && chrono::duration_cast<chrono::seconds>(now - lastInputTime).count() >= idleTimeLimit) {
    //         if (!idleModeActive.load()) {
    //             // Display an idle message if idleModeActive is false
    //             int randomIndex = rand() % idleMessages.size();
    //             cout << "\nVIAS (IDLE): " << idleMessages[randomIndex] << endl;
    //             cout << userName << ": ";  // Print user's name and position cursor for user input
    //             cout.flush();  // Ensure the cursor stays after the user's name
    //             idleModeActive.store(true);  // Prevent further idle messages until next interaction
    //         }
    //     }
    // }
}

#ifdef _WIN32
// No additional raw mode setup needed for Windows
#else
// Functions to enable and disable raw mode on Unix-based systems
void enableRawMode() {
    termios term;
    tcgetattr(STDIN_FILENO, &term);
    term.c_lflag &= ~(ICANON | ECHO);  // Disable canonical mode and echo
    tcsetattr(STDIN_FILENO, TCSANOW, &term);
}

void disableRawMode() {
    termios term;
    tcgetattr(STDIN_FILENO, &term);
    term.c_lflag |= (ICANON | ECHO);  // Re-enable canonical mode and echo
    tcsetattr(STDIN_FILENO, TCSANOW, &term);
}
#endif

#ifdef _WIN32
void handleUserInput(string& userMessage, const string& userName) {
    userMessage.clear();  // Clear previous input

    // Display the user name first
    cout << userName << ": " << flush;

    while (true) {
        // Capture a single character without echoing
        char ch = _getch();

        {
            lock_guard<mutex> lock(inputMutex);
            lastInputTime = chrono::steady_clock::now();
            idleModeActive.store(false);
            isUserBusy.store(true);
            isInOperation.store(false);
        }

        // If Enter is pressed, finalize the input
        if (ch == '\r') {  // Enter key in Windows is '\r'
            cout << endl;  // Move to a new line for the next output
            isUserBusy.store(false);
            break;
        }

        // Handle backspace (ASCII 8)
        if (ch == '\b' || ch == 127) {
            if (!userMessage.empty()) {
                // Remove the last character from the user input
                userMessage.pop_back();

                // Erase the character from the terminal
                cout << "\b \b" << flush; // Move back and overwrite with space
            }
        }
        else {
            // Append the character to the user message
            userMessage += ch;

            // Print the character back to the screen (since echo is disabled)
            cout << ch << flush;
        }
    }
}
#else
// Function to handle user input and activity detection
void handleUserInput(string& userMessage, const string& userName) {
    enableRawMode(); // Enable raw mode to capture each keystroke

    userMessage.clear();  // Clear previous input

    // Display the user name first
    cout << userName << ": " << flush;
    char ch;
    while (true) {
        // Read a single character from stdin
        if (read(STDIN_FILENO, &ch, 1) == 1) {
            {
                lock_guard<mutex> lock(inputMutex);
                lastInputTime = chrono::steady_clock::now();
                idleModeActive.store(false);
                isUserBusy.store(true);
                isInOperation.store(false);
            }

            // If Enter is pressed, finalize the input
            if (ch == '\n') {
                cout << endl;
                isUserBusy.store(false);
                break;
            }

            // Handle backspace (ASCII 127 or '\x7f')
            if (ch == '\x7f' || ch == 127) {
                if (!userMessage.empty()) {
                    // Remove the last character from the user input
                    userMessage.pop_back();

                    // Erase the character from the terminal
                    cout << "\b \b" << flush;
                }
            }
            else {
                // Append the character to the user message
                userMessage += ch;

                // Print the character back to the screen (since echo is disabled)
                cout << ch << flush;
            }
        }
    }
    disableRawMode(); // Restore terminal to its original settings
}
#endif

#ifdef _WIN32 // windows version

// Function to convert a single character to binary
string charToBinary(char c) {
    return bitset<8>(c).to_string(); // Convert char to 8-bit binary
}

// Function to convert string to decimal (ASCII values)
string stringToDecimal(const string& input) {
    stringstream ss;
    for (char c : input) {
        if (c != ' ') {
            ss << static_cast<int>(c) << " "; // Convert each character to its ASCII value
        }
        else {
            ss << "32 "; // ASCII value for space is 32
        }
    }
    return ss.str();
}

// Function to convert string to hexadecimal
string stringToHex(const string& input) {
    stringstream ss;
    for (char c : input) {
        if (c != ' ') {
            ss << hex << setw(2) << setfill('0') << uppercase << static_cast<int>(c) << " "; // Convert each character to hex
        }
        else {
            ss << "20 "; // ASCII value for space in hex is 20
        }
    }
    return ss.str();
}

// Function to convert string to octal
string stringToOctal(const string& input) {
    stringstream ss;
    for (char c : input) {
        if (c != ' ') {
            ss << oct << static_cast<int>(c) << " "; // Convert each character to octal
        }
        else {
            ss << "40 "; // ASCII value for space in octal is 40
        }
    }
    return ss.str();
}

// Function to handle user input with backspace and spacebar support
string getUserInput() {
    string input;
    char ch;

    cout << "Please enter the words (letters and numbers): " << flush;

    while (true) {
        ch = getchar(); // Read a character from the input

        if (ch == '\n') { // Enter key pressed
            break;
        }
        else if (ch == '\b' || ch == 127) { // Backspace handling (127 for some systems)
            if (!input.empty()) {
                input.pop_back(); // Remove last character
                cout << "\b \b" << flush; // Handle backspace visually in the terminal
            }
        }
        else {
            // Add character to the input string
            input += ch;
            cout << ch << flush; // Display typed character in the terminal
        }
    }

    return input;
}

// Main conversion function
void convertBase() {
    while (true) {
        isUserBusy = true;
        // Ask user to input the string (with backspace handling)
        string input;
        cout << "Enter the string to convert (or 'exit' to quit): ";
        getline(cin, input);

        if (input.empty()) {
            cout << "Input cannot be empty. Please try again." << endl;
            continue;
        }

        if (input == "exit") {
            cout << "Exiting convertBase..." << endl;
            return;
        }

        while (true) {
            // Ask user to choose an option
            cout << "\nChoose an option:\n"
                << "1. Convert to Binary\n"
                << "2. Convert to Octal (Base 8)\n"
                << "3. Convert to Decimal\n"
                << "4. Convert to Hexadecimal\n"
                << "5. Retype\n"
                << "6. Exit\n"
                << "Your choice: ";

            string choiceInput;
            getline(cin, choiceInput);

            // Convert choice input to an integer if valid
            int choice = 0;
            stringstream ss(choiceInput);
            if (!(ss >> choice) || choice < 1 || choice > 6) {
                cout << "Invalid choice. Please enter a number between 1 and 6.\n";
                continue;
            }

            switch (choice) {
            case 1: { // Convert to Binary
                cout << "\nBinary: ";
                for (char c : input) {
                    if (c != ' ') {
                        cout << charToBinary(c) << " "; // Convert each character to binary
                    }
                    else {
                        cout << " "; // Preserve spaces in binary output
                    }
                }
                cout << "\n";
                break;
            }
            case 2: { // Convert to Octal (Base 8)
                cout << "\nOctal (Base 8): " << stringToOctal(input) << "\n";
                break;
            }
            case 3: { // Convert to Decimal (ASCII values)
                cout << "\nDecimal: " << stringToDecimal(input) << "\n";
                break;
            }
            case 4: { // Convert to Hexadecimal
                cout << "\nHexadecimal: " << stringToHex(input) << "\n";
                break;
            }
            case 5: { // Retype the input
                break; // Exit the inner loop to retype
            }
            case 6: { // Exit the void function
                cout << "Exiting...\n";
                return;
            }
            }

            if (choice == 5) break; // Break from the inner loop for retyping
        }
        isUserBusy = false;
    }
}

//date
string getDayOfWeek(int dayOfWeek) {
    const string days[] = { "Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat" };
    return days[dayOfWeek];
}

string getMonthName(int month) {
    const string months[] = { "Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul", "Aug", "Sep", "Oct", "Nov", "Dec" };
    return months[month];
}

string getCurrentTime() {
    time_t ttime = time(0);
    tm* local_time = localtime(&ttime);

    string dayOfWeek = getDayOfWeek(local_time->tm_wday);
    string monthName = getMonthName(local_time->tm_mon);

    // Convert 24-hour time to 12-hour format with AM/PM
    int hour = local_time->tm_hour;
    string period = (hour >= 12) ? "PM" : "AM";
    if (hour == 0) hour = 12;
    else if (hour > 12) hour -= 12;

    stringstream timeStream;
    timeStream << "Date: " << monthName << ", " << dayOfWeek << ", " << local_time->tm_mday << ", "
        << 1900 + local_time->tm_year << endl
        << "Time (12h) " << hour << " H : "
        << local_time->tm_min << " Min : "
        << local_time->tm_sec << " Sec " << period;

    return timeStream.str();
}

// Function to run Linux terminal operations
void runLinuxTerminalOperation() {
    isUserBusy = true;
    vector<string> exitCommands = { "exitterminal", "end", "killlinux", "exit terminal", "kill linux", "kill terminal", "killterminal" };
    bool isRoot = false; // Track whether user is in root mode

    // Start in home directory (~)
    system("cd ~");

    clearTerminal(); // Your function to clear the terminal screen

    cout << "This is Linux Terminal Operation..." << endl;
    sleepForMilliseconds(150); // Simulated delay

    while (true) {
        cout << (isRoot ? "root@linux:~# " : "> "); // Show root prompt if sudo -s was used
        string userCommand;
        getline(cin, userCommand);

        // Convert input to lowercase for consistent checking
        string lowerCommand;
        transform(userCommand.begin(), userCommand.end(), back_inserter(lowerCommand), ::tolower);

        // Check if the user wants to exit the terminal operation
        if (find(exitCommands.begin(), exitCommands.end(), lowerCommand) != exitCommands.end()) {
            cout << "Exiting Terminal Operation..." << endl;
            break;
        }

        // Simulate switching to root user with 'sudo -s'
        if (lowerCommand == "sudo -s") {
            isRoot = true; // Switch to root mode
            cout << "Switched to root. You are now operating as root user." << endl;
            continue;
        }

        // If the user types 'exit' or 'exit root' in root mode, go back to normal mode
        if ((lowerCommand == "exit" || lowerCommand == "exit root") && isRoot) {
            isRoot = false;
            cout << "Exited root mode." << endl;
            continue;
        }

        // Execute the user's command in the terminal, handling root mode if needed
        if (isRoot) {
            system(("cd / && " + userCommand).c_str()); // Commands from the root directory
        }
        else {
            system(("cd ~ && " + userCommand).c_str()); // Commands from the home directory
        }
    }
    isUserBusy = false;
}

//Win_CMD
void runCmdOperation() {
    isUserBusy = true;
    vector<string> exitCommands = { "end","exitcmd" };

    // Clear the terminal (CMD) screen
    clearTerminal();

    cout << "This is Window CMD Operation... [type 'end', 'exitcmd' to close CMD]" << endl;
    sleepForMilliseconds(150);
    while (true) {
        cout << "> ";
        string userCommand;
        getline(cin, userCommand);

        // Convert input to lowercase for consistent checking
        string lowerCommand;
        transform(userCommand.begin(), userCommand.end(), back_inserter(lowerCommand), ::tolower);

        // Check if the user wants to exit the CMD operation
        if (find(exitCommands.begin(), exitCommands.end(), lowerCommand) != exitCommands.end()) {
            // cout << "Exiting CMD Operation..." << endl;
            break;
        }

        // Execute the user's command in CMD
        system(userCommand.c_str());
    }
    isUserBusy = false;
}

//Animation Response with a flag to enable or disable typing animation
void typeResponse(const string& response, bool useAnimation = true) {
    if (useAnimation) {
        for (const char& ch : response) {
            cout << ch;
            cout.flush();  // Ensure each character is displayed immediately
            this_thread::sleep_for(chrono::milliseconds(25));  // Simulate typing delay
        }
        cout << endl;
    }
    else {
        // If animation is disabled, print the response immediately
        cout << response << endl;
    }
}

//QR
void generateQRCode(const string& url) {
    isUserBusy = true;
    cout << "Generating QR Code for: " << url << endl;

#ifdef _WIN32
    // Windows-specific QR code generation logic
    string command = "start https://qrenco.de/" + url;
    system(command.c_str());
#else
    cout << "Unsupported OS for QR Code generation." << endl;
#endif

    // Wait for user input before clearing the screen
    cout << "\nPress Enter to continue...";
    cin.ignore((std::numeric_limits<std::streamsize>::max)(), '\n');
    cin.get();  // Wait for Enter key press
    isUserBusy = false;
}

void displayHistory(ifstream& historyFile, bool canRecreate) {
    isUserBusy = true;
    string line;
    vector<string> history;
    int lineCount = 0;
    int page = 1;
    const int pageSize = 10;
    int totalPages = 1;

    while (getline(historyFile, line)) {
        history.push_back(line);
        lineCount++;
    }

    totalPages = (lineCount + pageSize - 1) / pageSize;

    do {
        clearTerminal();
        cout << "History - Page " << page << " / " << totalPages << endl;
        for (int i = (page - 1) * pageSize; i < min(lineCount, page * pageSize); ++i) {
            cout << i + 1 << ". " << history[i] << endl;
        }
        if (canRecreate) {
            cout << "\nEnter the number of the link to recreate QR code or type [n] for next page, [p] for previous page, [exit] to go back: ";
            string choice;
            cin >> choice;
            if (choice == "n" && page < totalPages) page++;
            else if (choice == "p" && page > 1) page--;
            else if (choice == "exit") break;
            else {
                int index = stoi(choice) - 1;
                if (index >= 0 && index < history.size()) {
                    generateQRCode(history[index]);
                }
                else {
                    cout << "Invalid input, please try again." << endl;
                }
            }
        }
        else {
            cout << "\nType [n] for next page, [p] for previous page, [exit] to go back: ";
            string choice;
            cin >> choice;
            if (choice == "n" && page < totalPages) page++;
            else if (choice == "p" && page > 1) page--;
            else if (choice == "exit") break;
        }
    } while (true);
    isUserBusy = false;
}

void qrCodeMenu() {
    isUserBusy = true;
    string userChoice;
    do {
        clearTerminal();
        cout << "Main Menu:\n1. Create QR Code\n2. Recall / History\n3. Exit\n";
        cout << "Your Option: ";
        getline(cin, userChoice);

        if (userChoice == "1") {
            clearTerminal();
            cout << "Paste your Link / URL: ";
            string url;
            getline(cin, url);
            generateQRCode(url);

            filesystem::create_directory("QRcode");
            ofstream historyFile("QRcode/QR_His.txt", ios::app);
            historyFile << url << endl;
            historyFile.close();

        }
        else if (userChoice == "2") {
            string recallChoice;
            do {
                clearTerminal();
                cout << "Recall / History:\n1. Show History\n2. Recall QR\n3. Main Menu\n4. Exit QR\n";
                cout << "Your Option: ";
                getline(cin, recallChoice);

                if (recallChoice == "1") {
                    ifstream historyFile("QRcode/QR_His.txt");
                    if (historyFile.is_open()) {
                        displayHistory(historyFile, false);
                        historyFile.close();
                    }
                    else {
                        cout << "No history found." << endl;
                    }

                }
                else if (recallChoice == "2") {
                    ifstream historyFile("QRcode/QR_His.txt");
                    if (historyFile.is_open()) {
                        displayHistory(historyFile, true);
                        historyFile.close();
                    }
                    else {
                        cout << "No history found." << endl;
                    }

                }
                else if (recallChoice == "3") {
                    break;
                }
                else if (recallChoice == "4") {
                    return;
                }
            } while (recallChoice != "3" && recallChoice != "4");
        }
        else if (userChoice == "3") {
            break;
        }
    } while (userChoice != "3");
    isUserBusy = false;
}

// Declaration of the openWebLink function
void openWebLink(const string& url);

void openGoogle() {
    system("START https://www.google.com");
}

void searchGoogle() {
    cout << "VIAS: What do you want to search: ";
    string query;
    getline(cin, query);

    replace(query.begin(), query.end(), ' ', '+');
    string searchUrl = "https://www.google.com/search?q=" + query;

    openWebLink(searchUrl);
}
void googleMenu() {
    while (true) {
        clearTerminal();
        cout << "Google Menu:\n";
        cout << "1. Go to Google Home\n";
        cout << "2. Search on Google\n";
        cout << "3. Exit\n";
        cout << "Enter your choice: ";

        string input;
        getline(cin, input);

        if (input.empty()) {
            cout << "No input provided. Please enter a choice." << endl;
            continue;
        }

        int choice;
        try {
            choice = stoi(input);
        }
        catch (const invalid_argument&) {
            cout << "Invalid input. Please enter a number." << endl;
            continue;
        }

        switch (choice) {
        case 1:
            cout << "Opening Google homepage..." << endl;
            openGoogle();
            break;

        case 2:
            searchGoogle();
            break;

        case 3:
            return;

        default:
            cout << "Invalid choice. Please enter a number between 1 and 3." << endl;
            break;
        }

        cout << "\nPress Enter to return to the menu...";
        cin.ignore((std::numeric_limits<std::streamsize>::max)(), '\n');
    }
}

void searchWeather() {
    isUserBusy = true;
    string userChoice;
    do {
        clearTerminal();
        cout << "Weather Menu:\n1. Default Weather Search\n2. Search by Location\n3. Exit\n";
        cout << "Your Option: ";
        getline(cin, userChoice);

        if (userChoice == "1") {
            // Option 1: Open default weather search
            string searchUrl = "https://www.google.com/search?q=weather";
            openWebLink(searchUrl);

        }
        else if (userChoice == "2") {
            // Option 2: User types in a location
            cout << "your location: ";
            string location;
            getline(cin, location);
            replace(location.begin(), location.end(), ' ', '+');
            string searchUrl = "https://www.google.com/search?q=weather+" + location;
            openWebLink(searchUrl);

        }
        else if (userChoice == "3") {
            // Option 3: Exit
            break;

        }
        else {
            cout << "Invalid option. Please choose 1, 2, or 3.\n";
        }
    } while (userChoice != "3");
    isUserBusy = false;
}

//Kali
void openKaliWeb() {
    system("START https://www.kali.org");
}

//youtube
void openYouTube() {
    openWebLink("https://www.youtube.com");
}

void searchYouTube() {
    isUserBusy = true;
    cout << "VIAS: What do you want to search on YouTube: ";
    string query;
    getline(cin, query);
    replace(query.begin(), query.end(), ' ', '+');

    string searchUrl = "https://www.youtube.com/results?search_query=" + query;

    openWebLink(searchUrl);
    isUserBusy = false;
}

void youtubeMenu() {
    while (true) {
        clearTerminal();
        cout << "YouTube Menu:\n";
        cout << "1. Go to YouTube Home\n";
        cout << "2. Search on YouTube\n";
        cout << "3. Exit\n";
        cout << "Enter your choice: ";

        string input;
        getline(cin, input);

        if (input.empty()) {
            cout << "No input provided. Please enter a choice." << endl;
            continue;
        }

        int choice;
        try {
            choice = stoi(input);
        }
        catch (const invalid_argument&) {
            cout << "Invalid input. Please enter a number." << endl;
            continue;
        }

        switch (choice) {
        case 1:
            // Go to YouTube Home
            openYouTube();
            cout << "Opening YouTube homepage..." << endl;
            break;

        case 2:
            // Search on YouTube
            searchYouTube();
            break;

        case 3:
            // Exit the YouTube menu
            return;

        default:
            cout << "Invalid choice. Please enter a number between 1 and 3." << endl;
            break;
        }

        cout << "\nPress Enter to return to the menu...";
        cin.ignore((std::numeric_limits<std::streamsize>::max)(), '\n');  // Wait for the user to press Enter
    }
}

void openWiki() {
    bool isRunning = true;
    string searchQuery;

    while (isRunning) {
        clearTerminal();
        cout << "\nWikipedia Menu:\n";
        cout << "1. Home (Wikipedia homepage)\n";
        cout << "2. Search Wikipedia\n";
        cout << "3. Exit\n";
        cout << "Enter your choice: ";
        int choice;
        cin >> choice;

        switch (choice) {
        case 1:
            openWebLink("https://en.wikipedia.org");
            break;
        case 2:
            cin.ignore();  // Clear the input buffer
            cout << "Enter your search query: ";
            getline(cin, searchQuery);

            if (!searchQuery.empty()) {
                string searchURL = "https://en.wikipedia.org/wiki/" + searchQuery;
                replace(searchURL.begin(), searchURL.end(), ' ', '_');
                openWebLink(searchURL);
            }
            else {
                cout << "Search query cannot be empty. Please try again." << endl;
            }
            break;
        case 3:
            cout << "Exiting Wikipedia menu." << endl;
            isRunning = false;
            break;
        default:
            cout << "Invalid choice. Please try again." << endl;
        }
    }
}

// Function to get a random Rick Roll response for the console message
string getRandomRickRollResponse() {
    vector<string> responses = {
        "VIAS: haha..., You're not getting me today, nice try, here's some magic trick!",
        "VIAS: Gotcha! Enjoy this little surprise!",
        "VIAS: Thought you'd catch me off guard? Think again!",
        "VIAS: You're in for a treat! Enjoy the show!",
        "VIAS: Oh no, you didn't expect this, did you?",
        "VIAS: I see what you're trying to do... too bad, I beat you to it!",
        "VIAS: Nice try! But I'm always one step ahead!",
        "VIAS: Haha! Did you really think I wouldn't see that coming?",
        "VIAS: I knew this was coming. Here's a little surprise for you!",
        "VIAS: Looks like I'm the one in control today!",
        "VIAS: You think you're clever, but I've outsmarted you!"
    };
    // Return a random response
    return responses[rand() % responses.size()];
}

//Operation Roll
void openRickRoll() {
    isUserBusy = true;
    // Get a random console message
    cout << getRandomRickRollResponse() << endl;

    sleepForMilliseconds(500);
    openWebLink("https://www.youtube.com/watch?v=dQw4w9WgXcQ");
    sleepForMilliseconds(900);
    isUserBusy = false;
}

//Open Webside
void openWebLink(const string& url) {
    system(("start " + url).c_str());
}

// Function to convert input to lowercase for case-insensitive matching
string toLower(string input) {
    transform(input.begin(), input.end(), input.begin(), ::tolower);
    return input;
}

void webOpeningApp() {
    isUserBusy = true;

    string historyFolder = "./Web_History/";
    fs::create_directory(historyFolder);

    ofstream linkHistoryFile(historyFolder + "Link_History.txt", ios::app);
    if (!linkHistoryFile.is_open()) {
        cerr << "Error opening link history file for writing.\n";
        return;
    }

    while (true) {
        clearTerminal();
        cout << "What would you like to do?\n";
        cout << "1. Open home browser\n";
        cout << "2. Open custom link\n";
        cout << "3. Exit\n";
        cout << "Enter your choice: ";

        string input;
        getline(cin, input);

        if (input.empty()) {
            cout << "No input provided. Please enter a choice." << endl;
            continue;
        }

        int choice;
        try {
            choice = stoi(input);
        }
        catch (const invalid_argument&) {
            cout << "Invalid input. Please enter a number." << endl;
            continue;
        }

        switch (choice) {
        case 1:
            // Open Home Page
            openWebLink("https://www.google.com");
            cout << "Opening your home browser..." << endl;
            break;

        case 2: {
            // Open Custom Link
            cout << "Paste your link / URL here: ";
            string url;
            getline(cin, url);

            if (url.empty()) {
                cout << "No URL entered. Please try again." << endl;
            }
            else {
                // Check if the URL starts with "https://"
                if (url.find("https://") != 0) {
                    url = "https://" + url;
                }

                openWebLink(url);
                cout << "Opening " << url << "..." << endl;
                linkHistoryFile << url << endl;
            }
            break;
        }

        case 3:
            // Exit
            linkHistoryFile.close();
            isUserBusy = false;
            return;

        default:
            cout << "Invalid choice. Please enter a number between 1 and 3." << endl;
            break;
        }

        cout << "\nPress Enter to return to the menu...";
        cin.ignore((std::numeric_limits<std::streamsize>::max)(), '\n');
    }
}

// Function to rename the notepad file
string renameNotepad(const string& oldFilepath) {
    isUserBusy = true;
    cout << endl;
    cout << "Enter the new filename: ";
    string newFilename;
    getline(cin, newFilename);

    string newFilepath = "./Note/" + newFilename + ".txt";

    try {
        fs::rename(oldFilepath, newFilepath);
        return "File renamed successfully to " + newFilepath;
    }
    catch (const fs::filesystem_error& e) {
        return "Error renaming file: " + string(e.what());
    }
    isUserBusy = false;
    return "finished...";
}

// Function to display the notepad file content with a pause
void displayNotepad(const string& filepath) {
    isUserBusy = true;
    ifstream inFile(filepath);
    if (inFile.is_open()) {
        cout << endl;
        string line;
        while (getline(inFile, line)) {
            cout << line << endl;
        }
        inFile.close();
        cout << "\nPress Enter to continue...";
        cin.ignore((std::numeric_limits<std::streamsize>::max)(), '\n');
    }
    else {
        cerr << "Error opening file for reading: " << filepath << endl;
    }
    isUserBusy = false;
}

// Function to edit the notepad file content interactively
void editNotepad(const string& filepath) {
    isUserBusy = true;
    vector<string> lines;
    string line;
    ifstream inFile(filepath);

    if (inFile.is_open()) {
        cout << endl;
        while (getline(inFile, line)) {
            lines.push_back(line);
        }
        inFile.close();
    }
    else {
        cerr << "Error opening file for reading: " << filepath << endl;
        return;
    }

    size_t start = 0;
    const size_t displayCount = 10; // Number of lines to display at a time
    size_t totalPages = (lines.size() + displayCount - 1) / displayCount;

    while (true) {
        clearTerminal();
        size_t currentPage = (start / displayCount) + 1;
        cout << "\nCurrent content (Page " << currentPage << " of " << totalPages << "):\n";
        cout << "Displaying lines " << start + 1 << " to " << min(start + displayCount, lines.size()) << ":\n";

        for (size_t i = start; i < min(start + displayCount, lines.size()); ++i) {
            cout << i + 1 << ": " << lines[i] << endl;
        }

        cout << "\nOptions: [n] Next, [p] Previous, [e] Edit, [s] Save and Exit, [page #] Jump to Page\n";
        cout << "Enter your choice: ";
        string choice;
        getline(cin, choice);
        choice = toLower(choice);

        if (choice == "n" && start + displayCount < lines.size()) {
            start += displayCount;
        }
        else if (choice == "p" && start > 0) {
            start -= displayCount;
        }
        else if (choice == "e") {
            cout << "Enter the line number to edit (in current view): ";
            size_t lineNumber;
            cin >> lineNumber;
            cin.ignore();

            if (lineNumber > 0 && lineNumber <= displayCount && start + lineNumber - 1 < lines.size()) {
                size_t editLine = start + lineNumber - 1;
                cout << "Current line " << editLine + 1 << ": " << lines[editLine] << endl;
                cout << "Enter new content: ";
                getline(cin, lines[editLine]);
            }
            else {
                cout << "Invalid line number. Try again.\n";
            }
        }
        else if (choice == "s") {
            ofstream outFile(filepath, ios::trunc);
            if (outFile.is_open()) {
                for (const string& modifiedLine : lines) {
                    outFile << modifiedLine << endl;
                }
                outFile.close();
                cout << "File updated successfully.\n";
            }
            else {
                cerr << "Error opening file for writing.\n";
            }
            break;
        }
        else if (choice.find("page ") == 0) {
            int pageNumber = stoi(choice.substr(5));
            if (pageNumber > 0 && pageNumber <= totalPages) {
                start = (pageNumber - 1) * displayCount;
            }
            else {
                cout << "Invalid page number. Try again.\n";
            }
        }
        else {
            cout << "Invalid choice. Try again.\n";
        }
    }
    isUserBusy = false;
}

// Function to delete a notepad file
void deleteFile(const string& filepath) {
    isUserBusy = true;
    if (fs::remove(filepath)) {
        cout << "File deleted successfully." << endl;
    }
    else {
        cerr << "Error deleting file. Please check the file path and try again.\n";
    }
    isUserBusy = false;
}

// Function to handle file operations (continue writing, replace, delete, edit)
void continueWriting(const string& filepath) {
    isUserBusy = true;

    // Read the existing file content into a vector
    vector<string> notes;
    string line;
    ifstream inFile(filepath);

    if (inFile.is_open()) {
        while (getline(inFile, line)) {
            notes.push_back(line);
        }
        inFile.close();
    }
    else {
        cerr << "Error opening file for reading: " << filepath << endl;
        isUserBusy = false;
        return;
    }

    while (true) {
        system("cls"); // Clear the terminal for Windows
        cout << "\nCurrent Notes in " << filepath << ":\n";
        for (size_t i = 0; i < notes.size(); ++i) {
            cout << i + 1 << ": " << notes[i] << endl;
        }

        cout << "\nOptions:\n"
            << "[c] Continue write\n"
            << "[a] Add a new line\n"
            << "[e] Edit a line\n"
            << "[d] Delete a line\n"
            << "[s] Save and Exit\n"
            << "[x] Exit without saving\n"
            << "Enter your choice: ";

        string choice;
        getline(cin, choice);
        choice = toLower(choice);

        if (choice == "c") {  // Continue writing
            cout << "\nContinue Writing Options:\n"
                << "1. Continue (append at the end)\n"
                << "2. In between (insert at a specific position)\n"
                << "Enter your choice: ";
            string subChoice;
            getline(cin, subChoice);

            if (subChoice == "1") {  // Append to the end
                system("cls");
                cout << "\n>> Continue from previous content. Type 'EXIT' on a new line to stop:\n\n";
                while (true) {
                    getline(cin, line);
                    if (toLower(line) == "exit") {
                        break;
                    }
                    notes.push_back(line);
                }
            }
            else if (subChoice == "2") {  // Insert in between
                cout << "Enter the line number to insert after: ";
                size_t lineNumber;
                cin >> lineNumber;
                cin.ignore();

                if (lineNumber > 0 && lineNumber <= notes.size()) {
                    cout << ">> Insert new lines below line " << lineNumber << ". Type 'EXIT' on a new line to stop:\n\n";
                    vector<string>::iterator it = notes.begin() + lineNumber;
                    while (true) {
                        getline(cin, line);
                        if (toLower(line) == "exit") {
                            break;
                        }
                        it = notes.insert(it, line) + 1;
                    }
                }
                else {
                    cout << "Invalid line number. Returning to menu.\n";
                }
            }
            else {
                cout << "Invalid choice. Returning to menu.\n";
            }
        }
        else if (choice == "e") {  // Edit a line
            cout << "\nEdit Line Options:\n"
                << "1. Insert lines below\n"
                << "2. Overwrite line\n"
                << "Enter your choice: ";
            string subChoice;
            getline(cin, subChoice);

            cout << "Enter the line number to edit: ";
            size_t lineNumber;
            cin >> lineNumber;
            cin.ignore();

            if (lineNumber > 0 && lineNumber <= notes.size()) {
                if (subChoice == "1") {  // Insert lines below
                    cout << ">> Insert new lines below line " << lineNumber << ". Type 'EXIT' on a new line to stop:\n\n";
                    vector<string>::iterator it = notes.begin() + lineNumber;
                    while (true) {
                        getline(cin, line);
                        if (toLower(line) == "exit") {
                            break;
                        }
                        it = notes.insert(it, line) + 1;
                    }
                }
                else if (subChoice == "2") {  // Overwrite line
                    cout << "Current content: " << notes[lineNumber - 1] << endl;
                    cout << "Enter new content: ";
                    getline(cin, notes[lineNumber - 1]);
                }
                else {
                    cout << "Invalid choice. Returning to menu.\n";
                }
            }
            else {
                cout << "Invalid line number. Try again.\n";
            }
        }
        else if (choice == "a") {  // Add a new line
            cout << "Enter the new line content: ";
            string newLine;
            getline(cin, newLine);
            notes.push_back(newLine);
        }
        else if (choice == "d") {  // Delete a line
            cout << "Enter the line number to delete: ";
            size_t lineNumber;
            cin >> lineNumber;
            cin.ignore();

            if (lineNumber > 0 && lineNumber <= notes.size()) {
                notes.erase(notes.begin() + lineNumber - 1);
                cout << "Line deleted successfully.\n";
            }
            else {
                cout << "Invalid line number. Try again.\n";
            }
        }
        else if (choice == "s") {  // Save and Exit
            ofstream outFile(filepath, ios::trunc);
            if (outFile.is_open()) {
                for (const string& note : notes) {
                    outFile << note << endl;
                }
                outFile.close();
                cout << "Notes saved successfully to " << filepath << endl;
            }
            else {
                cerr << "Error opening file for writing.\n\n";
            }
            break;
        }
        else if (choice == "x") {  // Exit without saving
            cout << "!! WARNING !! Changes were not saved.\n\n";
            break;
        }
        else {
            cout << "Invalid choice. Please try again.\n";
        }
    }
    isUserBusy = false;
}

// Main function to handle notepad operations
string notepadFunction() {
    isUserBusy = true;
    clearTerminal();
    cout << endl << "Welcome to the terminal notepad!\n"
        << "Start typing your notes. Type 'EXIT' on a new line to finish.\n\n";

    vector<string> notes;
    string line;

    // Input phase
    while (true) {
        getline(cin, line);
        if (toLower(line) == "exit") break;
        notes.push_back(line);
    }

    // Editing phase
    while (true) {
        clearTerminal();
        cout << "\nCurrent Notes:\n";
        for (size_t i = 0; i < notes.size(); ++i) {
            cout << i + 1 << ": " << notes[i] << endl;
        }

        cout << "\nOptions:\n"
            << "[c] Continue write\n"
            << "[a] Add a new line\n"
            << "[e] Edit a line\n"
            << "[d] Delete a line\n"
            << "[s] Save and Exit\n"
            << "[x] Exit without saving\n"
            << "Enter your choice: ";

        string choice;
        getline(cin, choice);
        choice = toLower(choice);

        if (choice == "c") {  // Continue writing
            clearTerminal();
            cout << "\n>> Continue below. Type 'EXIT' on a new line to stop.\n\n";
            for (const auto& existingLine : notes) {
                cout << existingLine << endl;
            }

            while (true) {
                getline(cin, line);
                if (toLower(line) == "exit") break;
                notes.push_back(line);
            }
        }
        else if (choice == "e") {  // Edit a line
            cout << "Enter the line number to edit: ";
            size_t lineNumber;
            cin >> lineNumber;
            cin.ignore();

            if (lineNumber > 0 && lineNumber <= notes.size()) {
                cout << "Current content: " << notes[lineNumber - 1] << endl;
                cout << "Enter new content: ";
                getline(cin, notes[lineNumber - 1]);
            }
            else {
                cout << "Invalid line number. Try again.\n";
            }
        }
        else if (choice == "a") {  // Add a new line
            cout << "Enter the new line content: ";
            string newLine;
            getline(cin, newLine);
            notes.push_back(newLine);
        }
        else if (choice == "d") {  // Delete a line
            cout << "Enter the line number to delete: ";
            size_t lineNumber;
            cin >> lineNumber;
            cin.ignore();

            if (lineNumber > 0 && lineNumber <= notes.size()) {
                notes.erase(notes.begin() + lineNumber - 1);
                cout << "Line deleted successfully.\n";
            }
            else {
                cout << "Invalid line number. Try again.\n";
            }
        }
        else if (choice == "s") {  // Save and Exit
            cout << "Enter the filename to save: ";
            string filename;
            getline(cin, filename);

            string folderPath = "./Note/";
            fs::create_directory(folderPath);

            string filepath = folderPath + filename + ".txt";
            ofstream outFile(filepath);
            if (outFile.is_open()) {
                for (const string& note : notes) {
                    outFile << note << endl;
                }
                outFile.close();
                cout << "Notes saved successfully to " << filepath << endl;
            }
            else {
                cerr << "Error opening file for writing.\n\n";
            }
            break;
        }
        else if (choice == "x") {  // Exit without saving
            cout << "!! WARNING !! Notes were not saved.\n\n";
            break;
        }
        else {
            cout << "Invalid choice. Please try again.\n";
        }
    }

    isUserBusy = false;
    return "Notepad operation completed.\n";
}

string fileOperationsLoop(const string& folderPath) {
    isUserBusy = true;

    // Ensure folderPath ends with a backslash for Windows paths
    string correctedFolderPath = folderPath;
    if (correctedFolderPath.back() != '\\' && correctedFolderPath.back() != '/') {
        correctedFolderPath += '\\';
    }

    // Check if directory exists
    if (!fs::exists(correctedFolderPath) || !fs::is_directory(correctedFolderPath)) {
        cerr << "Directory does not exist: " << correctedFolderPath << endl;
        return "Directory does not exist.\n";
    }

    vector<string> txtFiles;

    // Populate the vector with .txt files
    for (const auto& entry : fs::directory_iterator(correctedFolderPath)) {
        if (entry.is_regular_file() && entry.path().extension() == ".txt") {
            txtFiles.push_back(entry.path().filename().string());
        }
    }

    // Check if there are no text files
    if (txtFiles.empty()) {
        cout << "No text files found in the directory.\n";
        return "No text files found.\n";
    }

    const int pageSize = 10; // Number of files per page
    int currentPage = 0;
    bool isRunning = true;

    while (isRunning) {
        clearTerminal();

        // Calculate page bounds
        int totalFiles = txtFiles.size();
        int totalPages = (totalFiles + pageSize - 1) / pageSize; // Round up for total pages
        int startIdx = currentPage * pageSize;
        int endIdx = min(startIdx + pageSize, totalFiles);

        // Display files for the current page
        cout << "\nAvailable text files (Page " << currentPage + 1 << " of " << totalPages << "):\n";
        for (int i = startIdx; i < endIdx; ++i) {
            cout << i + 1 << ". " << txtFiles[i] << endl;
        }

        // Navigation options
        cout << "\nNavigation options:\n";
        if (currentPage > 0) cout << "[<] Previous Page\n";
        if (currentPage < totalPages - 1) cout << "[>] Next Page\n";
        cout << "[#] Jump to Page\n"
            << "[q] Quit\n"
            << "\nEnter your choice or select a file number: ";

        string input;
        getline(cin, input);

        if (input == "<" && currentPage > 0) {
            --currentPage;
        }
        else if (input == ">" && currentPage < totalPages - 1) {
            ++currentPage;
        }
        else if (input == "#") {
            cout << "Enter page number: ";
            string pageInput;
            getline(cin, pageInput);
            try {
                int targetPage = stoi(pageInput) - 1;
                if (targetPage >= 0 && targetPage < totalPages) {
                    currentPage = targetPage;
                }
                else {
                    cout << "Invalid page number. Please try again.\n";
                    pauseConsole();
                }
            }
            catch (...) {
                cout << "Invalid input format. Enter a number to jump to a page.\n";
                pauseConsole();
            }
        }
        else if (input == "q") {
            return "Exiting file operations.\n";
        }
        else {
            try {
                int fileIndex = stoi(input) - 1;
                if (fileIndex >= 0 && fileIndex < totalFiles) {
                    string selectedFilename = txtFiles[fileIndex];
                    string filepath = correctedFolderPath + selectedFilename;

                    while (true) {
                        clearTerminal();
                        cout << "\nDo you want to perform any file operations on '" << selectedFilename << "'?\n"
                            << "1. Rename\n"
                            << "2. Display Only\n"
                            << "3. Modify File\n"
                            << "4. Delete\n"
                            << "5. Open File Location\n"
                            << "6. Return to File List\n"
                            << "7. Quit Completely\n"
                            << "\nEnter your choice: ";

                        string choice;
                        getline(cin, choice);
                        choice = toLower(choice);

                        if (choice == "1" || choice == "rename") {
                            cout << renameNotepad(filepath) << endl;
                        }
                        else if (choice == "2" || choice == "view" || choice == "display") {
                            displayNotepad(filepath);
                        }
                        else if (choice == "3" || choice == "modify" || choice == "open") {
                            continueWriting(filepath);
                        }
                        else if (choice == "4" || choice == "delete") {
                            deleteFile(filepath);
                            txtFiles.erase(txtFiles.begin() + fileIndex); // Remove deleted file from the list
                            break; // Return to file list
                        }
                        else if (choice == "5" || choice == "open location") {
                            // Open the folder in Windows Explorer
                            string command = "explorer \"" + correctedFolderPath + "\"";
                            system(command.c_str());
                        }
                        else if (choice == "6" || choice == "return") {
                            break; // Return to file list
                        }
                        else if (choice == "7" || choice == "quit") {
                            return "Operation quit by user.\n";
                        }
                        else {
                            cout << "Invalid choice. Please try again.\n\n";
                        }
                    }
                }
                else {
                    cout << "Invalid file number. Please try again.\n";
                    pauseConsole();
                }
            }
            catch (...) {
                cout << "Invalid input. Please enter a valid command or file number.\n";
                pauseConsole();
            }
        }
    }

    isUserBusy = false;
    return "File operations completed.\n";
}

//code writing
string buildCode(const string& filepath) {
    string extension = filepath.substr(filepath.find_last_of('.') + 1);
    string outputName = filepath.substr(0, filepath.find_last_of('.')) + ".exe";
    string command;

    if (extension == "cpp") {
        command = "g++ -o \"" + outputName + "\" \"" + filepath + "\"";
    }
    else if (extension == "c") {
        command = "gcc -o \"" + outputName + "\" \"" + filepath + "\"";
    }
    else if (extension == "java") {
        command = "javac \"" + filepath + "\"";
        outputName = filepath.substr(0, filepath.find_last_of('.')) + ".class";
    }
    else if (extension == "py" || extension == "js" || extension == "rb") {
        return extension + " files don't need to be built.";
    }
    else {
        return "Unsupported file extension. Supported extensions: .cpp, .c, .java, .py, .js, .rb";
    }

    int result = system(command.c_str());
    if (result == 0) {
        cout << "Build successful! Output: " + outputName << endl;
        return "Build successful! Output: " + outputName;
    }
    else {
        cerr << "Build failed. Please check your code." << endl;
        return "Build failed. Please check your code.";
    }
}

string buildAndRunCode(const string& filepath) {
    string buildResult = buildCode(filepath);

    if (buildResult.find("Build successful") != string::npos) {
        string extension = filepath.substr(filepath.find_last_of('.') + 1);
        string command;

        if (extension == "cpp" || extension == "c") {
            string outputName = filepath.substr(0, filepath.find_last_of('.')) + ".exe";
            command = "\"" + outputName + "\"";
        }
        else if (extension == "java") {
            string className = filepath.substr(filepath.find_last_of('\\') + 1, filepath.find_last_of('.') - filepath.find_last_of('\\') - 1);
            command = "java " + className;
        }
        else if (extension == "py") {
            command = "python \"" + filepath + "\"";
        }
        else if (extension == "js") {
            command = "node \"" + filepath + "\"";
        }
        else if (extension == "rb") {
            command = "ruby \"" + filepath + "\"";
        }
        else {
            return "Unsupported file extension.";
        }

        cout << "\nRunning the program...\n";
        system(("start cmd.exe /k \"" + command + " && pause\"").c_str());
        return "Program executed successfully.";
    }
    else {
        return "Build failed. Program not executed.";
    }
}

string runCode(const string& filepath) {
    string extension = filepath.substr(filepath.find_last_of('.') + 1);
    string outputName = filepath.substr(0, filepath.find_last_of('.')) + ".exe";

    if ((extension == "cpp" || extension == "c") && !fs::exists(outputName)) {
        return "Output file not found. Build the program first.";
    }

    string command;
    if (extension == "cpp" || extension == "c") {
        command = "\"" + outputName + "\"";
    }
    else if (extension == "java") {
        string className = filepath.substr(filepath.find_last_of('\\') + 1, filepath.find_last_of('.') - filepath.find_last_of('\\') - 1);
        command = "java " + className;
    }
    else if (extension == "py") {
        command = "python \"" + filepath + "\"";
    }
    else if (extension == "js") {
        command = "node \"" + filepath + "\"";
    }
    else if (extension == "rb") {
        command = "ruby \"" + filepath + "\"";
    }
    else {
        return "Unsupported file extension.";
    }

    system(("start cmd.exe /k \"" + command + " && pause\"").c_str());
    return "Program executed successfully.";
}


string renameCodeFile(const string& oldFilepath) {
    isUserBusy = true;
    cout << endl;
    cout << "Enter the new filename (without extension): ";
    string newFilename;
    getline(cin, newFilename);

    string extension = oldFilepath.substr(oldFilepath.find_last_of('.'));
    string folderPath = oldFilepath.substr(0, oldFilepath.find_last_of('\\') + 1);
    string newFilepath = folderPath + newFilename + extension;

    try {
        fs::rename(oldFilepath, newFilepath);
        return "File renamed successfully to " + newFilepath;
    }
    catch (const fs::filesystem_error& e) {
        return "Error renaming file: " + string(e.what());
    }
    isUserBusy = false;
    return "Rename operation finished.";
}

void editAndWriteCode(const string& filepath) {
    vector<string> codeLines;

    // Read current file content into codeLines
    ifstream inFile(filepath);
    if (inFile.is_open()) {
        string line;
        while (getline(inFile, line)) {
            codeLines.push_back(line);
        }
        inFile.close();
    }
    else {
        cout << "Error reading file. Unable to update code.\n";
        return;
    }

    while (true) {
        system("cls"); // Clear terminal (Windows specific)
        cout << "\nCurrent content of '" << filepath << "':\n\n";
        for (size_t i = 0; i < codeLines.size(); ++i) {
            cout << i + 1 << ". " << codeLines[i] << endl; // Display line numbers
        }

        cout << "\n\nDo you want to perform any operations on the code?\n"
            << "[r] Rename file\n"
            << "[s] Open file location\n"
            << "[c] Continue writing\n"
            << "[a] Add Content\n"
            << "[d] Delete a line\n"
            << "[o] Overwrite a line\n"
            << "[f] Finish\n"
            << "Enter your choice: ";
        string updateChoice;
        getline(cin, updateChoice);
        transform(updateChoice.begin(), updateChoice.end(), updateChoice.begin(), ::tolower);

        if (updateChoice == "r") {
            // Rename file
            cout << "Enter new filename (including extension): ";
            string newFilename;
            getline(cin, newFilename);
            string folderPath = filepath.substr(0, filepath.find_last_of('\\') + 1);
            string newFilepath = folderPath + newFilename;
            if (rename(filepath.c_str(), newFilepath.c_str()) == 0) {
                cout << "File renamed to " << newFilename << endl;
                return; // Exit after renaming
            }
            else {
                cout << "Failed to rename file.\n";
            }
        }
        else if (updateChoice == "s") {
            // Open file location
            string folderPath = filepath.substr(0, filepath.find_last_of('\\') + 1);
            string openCommand = "explorer " + folderPath; // Windows specific
            system(openCommand.c_str());
            cout << "Opening file location...\n";
        }
        else if (updateChoice == "c") {
            // Continue writing
            cout << "\nContinue writing your code. Type 'END' or 'EXIT' on a blank line to finish.\n";
            while (true) {
                string line;
                getline(cin, line);
                if (line == "END" || line == "EXIT") break;
                codeLines.push_back(line);
            }
        }
        else if (updateChoice == "a") {
            // Add Content
            cout << "Choose an option:\n"
                << "1. Add a new line at the end.\n"
                << "2. Add a new line in between.\n"
                << "Enter your choice: ";
            string addOption;
            getline(cin, addOption);

            if (addOption == "1") {
                cout << "Enter the content to add: ";
                string newLine;
                getline(cin, newLine);
                codeLines.push_back(newLine);
            }
            else if (addOption == "2") {
                cout << "Enter the line number to add below: ";
                int lineNumber;
                cin >> lineNumber;
                cin.ignore(); // Clear the newline from input buffer

                if (lineNumber < 1 || lineNumber > codeLines.size()) {
                    cout << "Invalid line number. Operation canceled.\n";
                }
                else {
                    cout << "Enter the content to add: ";
                    string newLine;
                    getline(cin, newLine);
                    codeLines.insert(codeLines.begin() + lineNumber, newLine);
                }
            }
            else {
                cout << "Invalid choice.\n";
            }
        }
        else if (updateChoice == "d") {
            // Delete a line
            cout << "Enter the line number to delete: ";
            size_t lineNumber;
            cin >> lineNumber;
            cin.ignore();

            if (lineNumber > 0 && lineNumber <= codeLines.size()) {
                codeLines.erase(codeLines.begin() + lineNumber - 1);
                cout << "Line deleted.\n";
            }
            else {
                cout << "Invalid line number.\n";
            }
        }
        else if (updateChoice == "o") {
            // Overwrite a line
            cout << "Enter the line number to overwrite: ";
            size_t lineNumber;
            cin >> lineNumber;
            cin.ignore();

            if (lineNumber > 0 && lineNumber <= codeLines.size()) {
                cout << "Enter the new content: ";
                string newContent;
                getline(cin, newContent);
                codeLines[lineNumber - 1] = newContent;
            }
            else {
                cout << "Invalid line number.\n";
            }
        }
        else if (updateChoice == "f") {
            // Finish
            cout << "Exiting update mode.\n";
            break;
        }
        else {
            cout << "Invalid choice. Please try again.\n";
        }

        // Save updated content to file
        ofstream outFile(filepath, ios::trunc);
        if (outFile.is_open()) {
            for (const auto& codeLine : codeLines) {
                outFile << codeLine << endl;
            }
            outFile.close();
        }
        else {
            cout << "Error saving file.\n";
        }
    }
}

string editCode(const string& folderPath) {
    isUserBusy = true;

    // Ensure folderPath ends with the appropriate separator
    string correctedFolderPath = folderPath;
    if (correctedFolderPath.back() != '\\' && correctedFolderPath.back() != '/') {
        correctedFolderPath += '\\'; // Use Windows-style separator
    }

    // Check if directory exists
    if (!fs::exists(correctedFolderPath) || !fs::is_directory(correctedFolderPath)) {
        cerr << "Directory does not exist: " << correctedFolderPath << endl;
        return "Directory does not exist.\n";
    }

    vector<string> codeFiles;

    // Populate the vector with all files in the directory
    for (const auto& entry : fs::directory_iterator(correctedFolderPath)) {
        if (entry.is_regular_file()) {
            codeFiles.push_back(entry.path().filename().string());
        }
    }

    // Check if there are no files
    if (codeFiles.empty()) {
        cout << "No files found in the directory.\n";
        return "No files found.\n";
    }

    const int pageSize = 15; // Number of files per page
    int currentPage = 0;
    bool isRunning = true;

    while (isRunning) {
        clearTerminal();

        // Calculate page bounds
        int totalFiles = codeFiles.size();
        int totalPages = (totalFiles + pageSize - 1) / pageSize; // Round up for total pages
        int startIdx = currentPage * pageSize;
        int endIdx = min(startIdx + pageSize, totalFiles);

        // Display files for the current page
        cout << "\nAvailable code files (Page " << currentPage + 1 << " of " << totalPages << "):\n";
        for (int i = startIdx; i < endIdx; ++i) {
            cout << i + 1 << ". " << codeFiles[i] << endl;
        }

        // Navigation options
        cout << "\nNavigation options:\n";
        if (currentPage > 0) {
            cout << "[<] Previous page\n";
        }
        if (currentPage < totalPages - 1) {
            cout << "[>] Next page\n";
        }
        cout << "[j <page_number>] Jump to page\n"
            << "[q] Quit\n";

        cout << "\nEnter your choice (or select a file number): ";
        string input;
        getline(cin, input);

        if (input == ">") {
            if (currentPage < totalPages - 1) {
                ++currentPage;
            }
            else {
                cout << "You are already on the last page.\n";
                pauseConsole();
            }
        }
        else if (input == "<") {
            if (currentPage > 0) {
                --currentPage;
            }
            else {
                cout << "You are already on the first page.\n";
                pauseConsole();
            }
        }
        else if (input.substr(0, 2) == "j ") {
            try {
                int targetPage = stoi(input.substr(2)) - 1;
                if (targetPage >= 0 && targetPage < totalPages) {
                    currentPage = targetPage;
                }
                else {
                    cout << "Invalid page number. Please try again.\n";
                    pauseConsole();
                }
            }
            catch (...) {
                cout << "Invalid input format. Use 'j <page_number>' to jump to a page.\n";
                pauseConsole();
            }
        }
        else if (input == "q") {
            isRunning = false;
            cout << "Exiting file selection.\n";
        }
        else {
            try {
                int fileIndex = stoi(input) - 1;
                if (fileIndex >= 0 && fileIndex < totalFiles) {
                    string selectedFilename = codeFiles[fileIndex];
                    string filepath = correctedFolderPath + selectedFilename;

                    // Present options for the selected file
                    clearTerminal();
                    cout << "============================\n"
                        << "File selected: " << selectedFilename << endl
                        << "============================\n"
                        << "Options for the selected file:\n"
                        << "1. View File\n"
                        << "2. Edit File\n"
                        << "3. Build/Run\n"
                        << "4. Delete File\n"
                        << "5. Rename File\n"
                        << "6. Return to File List\n";

                    cout << "\nEnter your choice: ";
                    string fileOption;
                    getline(cin, fileOption);

                    if (fileOption == "1") {
                        displayNotepad(filepath);
                    }
                    else if (fileOption == "2") {
                        editAndWriteCode(filepath);
                    }
                    else if (fileOption == "3") {
                        clearTerminal();
                        cout << "\nSelect an option for running code:\n"
                            << "1. Build and Run\n"
                            << "2. Run Only\n"
                            << "Your option: ";
                        string runOption;
                        getline(cin, runOption);

                        if (runOption == "1") {
                            string buildResult = buildAndRunCode(filepath);
                            cout << buildResult << endl;
                            cout << "\n>> Press enter to finish <<\n";
                            cin.get();
                        }
                        else if (runOption == "2") {
                            clearTerminal();
                            string runResult = runCode(filepath);
                            cout << runResult << endl;
                            cout << "\n>> Press enter to finish <<\n";
                            cin.get();
                        }
                        else {
                            cout << "Invalid option. Returning to file menu.\n";
                            pauseConsole();
                        }
                    }
                    else if (fileOption == "4") {
                        deleteFile(filepath);
                        cout << "File deleted successfully.\n";
                        codeFiles.erase(codeFiles.begin() + fileIndex);
                        totalFiles = codeFiles.size();
                        totalPages = (totalFiles + pageSize - 1) / pageSize;
                    }
                    else if (fileOption == "5") {
                        renameCodeFile(filepath);
                    }
                    else if (fileOption == "6") {
                        break;
                    }
                    else {
                        cout << "Invalid choice. Returning to file list.\n";
                        pauseConsole();
                    }
                }
                else {
                    cout << "Invalid file number. Please try again.\n";
                    pauseConsole();
                }
            }
            catch (...) {
                cout << "Invalid input. Please enter a valid command or file number.\n";
                pauseConsole();
            }
        }
    }

    isUserBusy = false;
    return "Code editing operations completed.\n";
}

string writeBuildRunCode() {
    isUserBusy = true;
    string folderPath = "./Programs/";
    fs::create_directory(folderPath);

    cout << "Enter the filename (including extension, e.g., 'example.cpp'): ";
    clearTerminal();
    string filename;
    getline(cin, filename);

    string filepath = folderPath + filename;

    // Detect language based on file extension
    string extension = filepath.substr(filepath.find_last_of('.') + 1);
    string buildCommand, runCommand;
    if (extension == "cpp") {
        buildCommand = "g++ " + filepath + " -o " + folderPath + "program";
        runCommand = folderPath + "program";
    }
    else if (extension == "c") {
        buildCommand = "gcc " + filepath + " -o " + folderPath + "program";
        runCommand = folderPath + "program";
    }
    else if (extension == "cs") {
        buildCommand = "mcs " + filepath + " -out:" + folderPath + "program.exe";
        runCommand = "mono " + folderPath + "program.exe";
    }
    else if (extension == "go") {
        runCommand = "go run " + filepath;
    }
    else if (extension == "js") {
        runCommand = "node " + filepath;
    }
    else if (extension == "py") {
        runCommand = "python3 " + filepath;
    }
    else if (extension == "java") {
        buildCommand = "javac " + filepath;
        runCommand = "java -cp " + folderPath + " " + filename.substr(0, filename.find_last_of('.'));
    }
    else {
        return "Unsupported file extension. Please use a supported language.\n";
    }

    vector<string> codeLines;

    // Writing Code
    cout << "Start writing your code. Type 'END' or 'EXIT' on a blank line to finish.\n";
    string line;
    while (true) {
        getline(cin, line);
        if (line == "END" || line == "EXIT") break;
        codeLines.push_back(line);
    }

    // Save Code to File
    ofstream outFile(filepath, ios::trunc);
    if (outFile.is_open()) {
        for (const auto& codeLine : codeLines) {
            outFile << codeLine << endl;
        }
        outFile.close();
        cout << "Code saved to " << filepath << endl;
    }
    else {
        return "Error saving file.\n";
    }

    // Display written content
    clearTerminal();
    cout << "\nYour code content:\n";
    for (const auto& codeLine : codeLines) {
        cout << codeLine << endl;
    }

    // Interaction Menu
    while (true) {
        clearTerminal();
        // Display the current file content with line numbers
        cout << "\nCurrent content of '" << filename << "':\n\n";
        for (size_t i = 0; i < codeLines.size(); ++i) {
            cout << i + 1 << ". " << codeLines[i] << endl;  // Display line number
        }

        cout << "\n\nOptions:\n"
            << "[b] Build Code (C, C++, Java)\n"
            << "[br] Build and Run (C, C++, Java)\n"
            << "[r] Run Only (Python, pre-built binaries)\n"
            << "[u] Update/Change Code\n"
            << "[f] Finish\n"
            << "Enter your choice: ";
        string choice;
        getline(cin, choice);
        choice = toLower(choice);

        if (choice == "b") {
            string buildResult = buildCode(filepath);
            cout << buildResult << endl;
        }
        else if (choice == "br") {
            clearTerminal();
            string buildAndRunResult = buildAndRunCode(filepath);
            cout << buildAndRunResult << endl;
            cout << "\n>> press enter to finish <<\n";
            cin.get();  // Wait for user input to finish testing
        }
        else if (choice == "r") {
            clearTerminal();
            string runResult = runCode(filepath);
            cout << runResult << endl;
            cout << "\n>> press enter to finish <<\n";
            cin.get();  // Wait for user input to finish testing
        }
        else if (choice == "u") {
            // Update/Change Code (additional operations)
            clearTerminal();
            cout << "\nCurrent content of '" << filename << "':\n\n";
            for (size_t i = 0; i < codeLines.size(); ++i) {
                cout << i + 1 << ". " << codeLines[i] << endl;  // Display line number
            }

            cout << "\n\nDo you want to perform any operations on '" << filename << "'?\n"
                << "[r] Rename file\n"
                << "[s] Open file location\n"
                << "[c] Continue writing\n"
                << "[a] Add a line\n"
                << "[d] Delete a line\n"
                << "[o] Overwrite a line\n"
                << "[f] Finish\n"
                << "Enter your choice: ";
            string updateChoice;
            getline(cin, updateChoice);
            updateChoice = toLower(updateChoice);

            if (updateChoice == "r") {
                // Rename file
                cout << "Enter new filename (including extension): ";
                string newFilename;
                getline(cin, newFilename);
                string newFilepath = folderPath + newFilename;
                if (rename(filepath.c_str(), newFilepath.c_str()) == 0) {
                    cout << "File renamed to " << newFilename << endl;
                    filepath = newFilepath;
                    filename = newFilename;
                }
                else {
                    cout << "Failed to rename file.\n";
                }
            }
            else if (updateChoice == "s") {
                // Open file location
                string openCommand = "explorer " + folderPath;  // Windows specific
                system(openCommand.c_str());
                cout << "Opening file location...\n";
            }
            else if (updateChoice == "c") {
                // Continue writing code
                cout << "\nContinue writing your code. Type 'END' or 'EXIT' on a blank line to finish.\n";
                while (true) {
                    getline(cin, line);
                    if (line == "END" || line == "EXIT") break;
                    codeLines.push_back(line);
                }

                // Save the updated code to the file
                ofstream outFile(filepath, ios::trunc);
                if (outFile.is_open()) {
                    for (const auto& codeLine : codeLines) {
                        outFile << codeLine << endl;
                    }
                    outFile.close();
                    cout << "Code updated and saved to " << filepath << endl;
                }
                else {
                    cout << "Error saving updated code.\n";
                }

            }
            else if (updateChoice == "a") {
                // Add a line
                cout << "=======================\n"
                    << "Choose an option:\n"
                    << "1. Add a new line at the end.\n"
                    << "2. Add a new line in between.\n"
                    << "Enter your choice: ";
                string addOption;
                getline(cin, addOption);

                if (addOption == "1") {
                    // Add a new line at the end
                    cout << "Add a line of content: ";
                    string newLine;
                    getline(cin, newLine);
                    codeLines.push_back(newLine);

                    // Save the updated code to the file
                    ofstream outFile(filepath, ios::trunc);
                    if (outFile.is_open()) {
                        for (const auto& codeLine : codeLines) {
                            outFile << codeLine << endl;
                        }
                        outFile.close();
                        cout << "Content added and saved to " << filepath << endl;
                    }
                    else {
                        cout << "Error saving updated code.\n";
                    }

                }
                else if (addOption == "2") {
                    // Add a new line in between
                    cout << "Enter the line number to add below: ";
                    int lineNumber;
                    cin >> lineNumber;
                    cin.ignore(); // Clear newline from the input buffer

                    if (lineNumber < 1 || lineNumber > codeLines.size()) {
                        cout << "Invalid line number. Operation canceled.\n";
                    }
                    else {
                        cout << "=======================\n"
                            << "Choose an option:\n"
                            << "1. Add a single line.\n"
                            << "2. Add multiple lines (type 'END' or 'EXIT' to finish).\n"
                            << "Enter your choice: ";
                        string inBetweenOption;
                        getline(cin, inBetweenOption);

                        if (inBetweenOption == "1") {
                            // Add a single line
                            cout << "=======================\n";
                            cout << "Enter the content to add: ";
                            string newLine;
                            getline(cin, newLine);
                            codeLines.insert(codeLines.begin() + lineNumber, newLine);

                        }
                        else if (inBetweenOption == "2") {
                            // Add multiple lines
                            clearTerminal();
                            cout << "Enter lines to add (type 'END' or 'EXIT' on a blank line to finish):\n\n";
                            vector<string> newLines;
                            while (true) {
                                string newLine;
                                getline(cin, newLine);
                                if (newLine == "END" || newLine == "EXIT") break;
                                newLines.push_back(newLine);
                            }
                            codeLines.insert(codeLines.begin() + lineNumber, newLines.begin(), newLines.end());
                        }
                        else {
                            cout << "Invalid option. Operation canceled.\n";
                        }

                        // Save the updated code to the file
                        ofstream outFile(filepath, ios::trunc);
                        if (outFile.is_open()) {
                            for (const auto& codeLine : codeLines) {
                                outFile << codeLine << endl;
                            }
                            outFile.close();
                            cout << "Lines added and saved to " << filepath << endl;
                        }
                        else {
                            cout << "Error saving updated code.\n";
                        }
                    }
                }
                else {
                    cout << "Invalid choice. Operation canceled.\n";
                }

            }
            else if (updateChoice == "d") {
                // Delete a line
                cout << "Enter the line number to delete: ";
                size_t lineNumber;
                cin >> lineNumber;
                cin.ignore();
                if (lineNumber > 0 && lineNumber <= codeLines.size()) {
                    codeLines.erase(codeLines.begin() + lineNumber - 1);
                    // Save the updated code to the file
                    ofstream outFile(filepath, ios::trunc);
                    if (outFile.is_open()) {
                        for (const auto& codeLine : codeLines) {
                            outFile << codeLine << endl;
                        }
                        outFile.close();
                        cout << "Line deleted and saved to " << filepath << endl;
                    }
                    else {
                        cout << "Error saving updated code.\n";
                    }
                }
                else {
                    cout << "Invalid line number.\n";
                }

            }
            else if (updateChoice == "o") {
                // Overwrite a line
                cout << "Enter the line number to overwrite: ";
                size_t lineNumber;
                cin >> lineNumber;
                cin.ignore();
                if (lineNumber > 0 && lineNumber <= codeLines.size()) {
                    cout << "Enter the new content: ";
                    string newContent;
                    getline(cin, newContent);
                    codeLines[lineNumber - 1] = newContent;

                    // Save the updated code to the file
                    ofstream outFile(filepath, ios::trunc);
                    if (outFile.is_open()) {
                        for (const auto& codeLine : codeLines) {
                            outFile << codeLine << endl;
                        }
                        outFile.close();
                        cout << "Line overwritten and saved to " << filepath << endl;
                    }
                    else {
                        cout << "Error saving updated code.\n";
                    }
                }
                else {
                    cout << "Invalid line number.\n";
                }

            }
            else if (updateChoice == "f") {
                // Finish editing and return to the main options
                break;
            }
            else {
                cout << "Invalid choice. Please try again.\n";
            }
        }
        else if (choice == "f") {
            cout << "Exiting task.\n";
            break;
        }
        else {
            cout << "Invalid choice. Please try again.\n";
        }
    }

    isUserBusy = false;
    return "Code task completed.\n";
}

//drawCalendar
vector<string> months = {
    "January", "February", "March", "April", "May", "June",
    "July", "August", "September", "October", "November", "December"
};

void drawCalendar(const string& monthName, int year, int currentDay = -1) {
    isUserBusy = true;
    cout << "\n\t" << monthName << " " << year << endl;
    cout << " Su  Mo  Tu  We  Th  Fr  Sa  " << endl;

    // Calculate the first day of the month
    tm firstDay = {};
    firstDay.tm_year = year - 1900;
    firstDay.tm_mon = find(begin(months), end(months), monthName) - begin(months);
    firstDay.tm_mday = 1;
    mktime(&firstDay);

    int startDay = firstDay.tm_wday;
    int daysInMonth = (firstDay.tm_mon == 1) ? (year % 4 == 0 && (year % 100 != 0 || year % 400 == 0) ? 29 : 28) :
        (firstDay.tm_mon == 3 || firstDay.tm_mon == 5 || firstDay.tm_mon == 8 || firstDay.tm_mon == 10) ? 30 : 31;

    for (int i = 0; i < startDay; ++i) {
        cout << "    ";
    }

    for (int day = 1; day <= daysInMonth; ++day) {
        if (day == currentDay) {
            cout << "[" << (day < 10 ? " " : "") << day << "]"; // Highlight the current day with brackets
        }
        else {
            cout << " " << setw(2) << day << " ";
        }

        if ((startDay + day) % 7 == 0) {
            cout << endl;
        }
    }
    cout << endl;
    isUserBusy = false;
}

void drawYearCalendar(int year) {
    isUserBusy = true;
    clearTerminal();

    // Get today's date
    time_t t = time(nullptr);
    tm* today = localtime(&t);
    int currentDay = today->tm_mday;
    int currentMonth = today->tm_mon;
    int currentYear = today->tm_year + 1900;

    cout << "\nDisplaying calendar for the year: " << year << "\n\n";

    const int monthsPerRow = 3;
    const int totalRows = 12 / monthsPerRow;
    const int maxWeeks = 6;  // Maximum weeks for alignment
    const int blockWidth = 30;  // Adjusted width for each month block

    for (int row = 0; row < totalRows; ++row) {
        // Print Month Names Centered with Dividers
        for (int col = 0; col < monthsPerRow; ++col) {
            int monthIndex = row * monthsPerRow + col;
            // Center the month name
            string monthHeader = months[monthIndex];
            int padding = (blockWidth - monthHeader.size()) / 2; // Calculate padding for centering
            cout << string(padding, ' ') << monthHeader << string(blockWidth - padding - monthHeader.size(), ' ') << "|";
        }
        cout << endl;

        // Print the weekday headers for each column
        for (int col = 0; col < monthsPerRow; ++col) {
            cout << " Su  Mo  Tu  We  Th  Fr  Sa   |";
        }
        cout << endl;

        // Prepare to draw the calendar rows for each month
        vector<vector<string>> monthDays(monthsPerRow);

        for (int col = 0; col < monthsPerRow; ++col) {
            int monthIndex = row * monthsPerRow + col;

            tm firstDay = {};
            firstDay.tm_year = year - 1900;
            firstDay.tm_mon = monthIndex;
            firstDay.tm_mday = 1;
            mktime(&firstDay);

            int startDay = firstDay.tm_wday;
            int daysInMonth = (monthIndex == 1) ?
                (year % 4 == 0 && (year % 100 != 0 || year % 400 == 0) ? 29 : 28) :
                (monthIndex == 3 || monthIndex == 5 || monthIndex == 8 || monthIndex == 10) ? 30 : 31;

            // Prepare calendar content weeks
            vector<string> weeks(maxWeeks, string(blockWidth - 2, ' ')); // Initialize weeks with spaces
            int currentWeek = 0;

            for (int day = 1; day <= daysInMonth; ++day) {
                int position = (startDay + day - 1) % 7 * 4; // Calculate the correct column

                if (year == currentYear && monthIndex == currentMonth && day == currentDay) {
                    // Highlight today's date with brackets, ensuring width of 4 spaces
                    string highlightedDay = "[" + string(day < 10 ? " " : "") + to_string(day) + "]";
                    weeks[currentWeek].replace(position, 4, highlightedDay); // Replace with consistent width
                }
                else {
                    // Regular day alignment with a width of 4 spaces
                    string regularDay = (day < 10 ? "  " : " ") + to_string(day) + " ";
                    weeks[currentWeek].replace(position, 4, regularDay); // Replace with consistent width
                }

                if ((startDay + day) % 7 == 0 || day == daysInMonth) {
                    currentWeek++;
                }
            }


            monthDays[col] = weeks;
        }

        // Print rows of the month data
        for (int week = 0; week < maxWeeks; ++week) {
            for (int col = 0; col < monthsPerRow; ++col) {
                cout << " " << monthDays[col][week] << " |";
            }
            cout << endl;
        }

        // Add row spacing to separate different calendar rows
        cout << endl;
    }
    isUserBusy = false;
}

void drawSpecificCalendar(const string& dateInput) {
    isUserBusy = true;
    clearTerminal();

    if (dateInput.length() == 4 && all_of(dateInput.begin(), dateInput.end(), ::isdigit)) {
        int year = stoi(dateInput);
        drawYearCalendar(year);
        cout << "\nCalendar for the year " << year << " displayed.\n";
        isUserBusy = false;
        return;
    }

    int month, year;
    stringstream ss(dateInput);
    ss >> month >> year;

    if (ss.fail() || month < 1 || month > 12) {
        cout << "Invalid input format. (e.g., 'cal 12 2024') or a 4-digit year (e.g., 'cal 2024').\n";
        isUserBusy = false;
        return;
    }

    string monthName = months[month - 1];
    drawCalendar(monthName, year);
    cout << "\nCalendar for " << monthName << ", " << year << " displayed.\n";
    isUserBusy = false;
}
bool isValidMonthYearInput(const string& dateInput) {
    size_t spacePos = dateInput.find(' ');
    if (spacePos != string::npos) {
        string monthStr = dateInput.substr(0, spacePos);
        string yearStr = dateInput.substr(spacePos + 1);

        try {
            int month = stoi(monthStr);
            int year = stoi(yearStr);

            if (month >= 1 && month <= 12 && year > 0) {
                return true; // Valid month/year format
            }
        }
        catch (const exception& e) {
            // In case of invalid number conversion
            return false;
        }
    }
    return false; // Invalid format if no space or incorrect month/year
}

string getTimeConvert() {
    isUserBusy = true;
    clearTerminal();

    string choiceStr;
    int choice = 0, years = 0, months = 0, weeks = 0, days = 0;
    string result;

    cout << "Time Conversion Options:" << endl;
    cout << "1. Convert by days" << endl;
    cout << "2. Convert by months" << endl;
    cout << "3. Convert by years" << endl;

    cout << "Select an option (1-3): ";
    getline(cin, choiceStr);

    try {
        choice = stoi(choiceStr);
    }
    catch (invalid_argument& e) {
        result = "Invalid input. Please enter a number between 1 and 3.";
        return result;
    }

    switch (choice) {
    case 1:
        cout << "Enter number of days: ";
        cin >> days;

        years = days / 365;
        months = days / 30;
        weeks = days / 7;

        result = "Years: " + to_string(years) + "\n" +
            "Months: " + to_string(months) + "\n" +
            "Weeks: " + to_string(weeks) + "\n" +
            "Days: " + to_string(days);
        break;
    case 2:
        cout << "Enter number of months: ";
        cin >> months;

        years = months / 12;
        weeks = static_cast<int>(months * 4.33);
        days = static_cast<int>(months * 30.41);

        result = "Years: " + to_string(years) + "\n" +
            "Months: " + to_string(months) + "\n" +
            "Weeks: " + to_string(weeks) + "\n" +
            "Days: " + to_string(days);
        break;

    case 3:
        cout << "Enter number of years: ";
        cin >> years;

        months = years * 12;
        weeks = years * 52;
        days = years * 365;

        result = "Years: " + to_string(years) + "\n" +
            "Months: " + to_string(months) + "\n" +
            "Weeks: " + to_string(weeks) + "\n" +
            "Days: " + to_string(days);
        break;

    default:
        result = "Invalid selection. Please choose a valid option (1-3).";
    }

    // Clear input buffer to handle any leftover newline characters
    cin.ignore((std::numeric_limits<std::streamsize>::max)(), '\n');

    isUserBusy = false;
    return result;
}

//calculator
// Helper function to check if a character is an operator
bool isOperator(char c) {
    return c == '+' || c == '-' || c == '*' || c == '/' || c == '%' || c == '^' || c == 'r' || c == 'x' || c == 'X';
}

// Utility function to remove all spaces from a string
string removeSpaces(const string& str) {
    string result;
    for (char c : str) {
        if (!isspace(c)) {
            result += c;
        }
    }
    return result;
}

// Utility function to trim spaces from both ends of a string
string trim(const string& str) {
    size_t first = str.find_first_not_of(' ');
    if (first == string::npos) return "";
    size_t last = str.find_last_not_of(' ');
    return str.substr(first, last - first + 1);
}

double nthRoot(double n, double x) {
    if (n == 0) {
        cerr << "Error: Root with degree 0 is invalid." << endl;
        return NAN;
    }

    if (n < 0) {
        return 1.0 / nthRoot(-n, x);
    }

    if (x < 0 && static_cast<int>(n) % 2 == 0) {
        return -pow(-x, 1.0 / n);
    }

    if (x < 0 && static_cast<int>(n) % 2 == 1) {
        return -pow(-x, 1.0 / n);
    }

    return pow(x, 1.0 / n);
}

// Function to apply an operator to two values and push the result
double applyOperator(double a, double b, char op) {
    switch (op) {
    case '+': return a + b;
    case '-': return a - b;
    case '*': return a * b;
    case 'x':
    case 'X': return a * b;  // Treat 'x' and 'X' as multiplication
    case '/': return (b != 0) ? a / b : 0;  // Avoid division by zero
    case '%': return fmod(a, b);
    case '^': return pow(a, b);  // Power operator
    case 'r': return nthRoot(a, b);  // Root operator: b^(1/a)
    default: return 0;
    }
}

// Function to get precedence of operators
int precedence(char op) {
    if (op == '+' || op == '-') return 1;
    if (op == '*' || op == '/' || op == '%') return 2;
    if (op == '^' || op == 'r') return 3;
    return 0;
}

// Function to tokenize the input expression
vector<string> tokenize(const string& expression) {
    vector<string> tokens;
    string token;
    for (size_t i = 0; i < expression.length(); ++i) {
        char c = expression[i];

        if (isdigit(c) || c == '.' || (c == '-' && (i == 0 || expression[i - 1] == '('))) {
            token += c;
        }
        else if (c == 'p' && i + 1 < expression.size() && expression.substr(i, 2) == "pi") {
            tokens.push_back("pi");
            i++;
        }
        else if (isOperator(c) || c == '(' || c == ')') {
            if (!token.empty()) {
                tokens.push_back(token);
                token.clear();
            }
            tokens.push_back(string(1, c));
        }
        else if (c == 'r') {
            if (!token.empty()) {
                tokens.push_back(token);
                token.clear();
            }
            tokens.push_back("r");
        }
    }
    if (!token.empty()) {
        tokens.push_back(token);
    }
    return tokens;
}

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif
// Function to replace constants like pi
double parseConstant(const string& token) {
    if (token == "pi") return M_PI;
    return stod(token);
}

// Main function to evaluate an expression
string evaluateExpression(const string& expression) {
    vector<string> tokens = tokenize(removeSpaces(expression));
    stack<double> values;
    stack<char> ops;

    for (const string& token : tokens) {
        if (isdigit(token[0]) || (token[0] == '-' && token.size() > 1) || token == "pi") {
            values.push(parseConstant(token));
        }
        else if (token == "(") {
            ops.push('(');
        }
        else if (token == ")") {
            while (!ops.empty() && ops.top() != '(') {
                double val2 = values.top(); values.pop();
                double val1 = values.top(); values.pop();
                char op = ops.top(); ops.pop();
                values.push(applyOperator(val1, val2, op));
            }
            ops.pop();
        }
        else if (isOperator(token[0])) {
            char op = token[0];
            while (!ops.empty() && precedence(ops.top()) >= precedence(op)) {
                double val2 = values.top(); values.pop();
                double val1 = values.top(); values.pop();
                char op = ops.top(); ops.pop();
                values.push(applyOperator(val1, val2, op));
            }
            ops.push(op);
        }
    }

    while (!ops.empty()) {
        double val2 = values.top(); values.pop();
        double val1 = values.top(); values.pop();
        char op = ops.top(); ops.pop();
        values.push(applyOperator(val1, val2, op));
    }

    if (values.size() != 1) return "Error: Invalid Expression";

    stringstream result;
    result << fixed << setprecision(15) << values.top();
    string resultStr = result.str();
    resultStr.erase(resultStr.find_last_not_of('0') + 1, string::npos);
    if (resultStr.back() == '.') resultStr.pop_back();
    return resultStr;
}

bool isMathExpression(const string& userMessage) {
    // Remove spaces for validation purposes
    string cleanedMessage = removeSpaces(userMessage);

    // Regular expression to match valid mathematical characters
    regex mathPattern("^[0-9\\.\\s\\(\\)\\+\\-\\*/%xX\\^rpi]+$");
    return regex_match(cleanedMessage, mathPattern);
}

//game menu
    //Snake
enum Direction { UP, DOWN, LEFT, RIGHT };

void snakeGame() {
    int width = 27;  // Expanded by 35%
    int height = 14; // Expanded by 35%
    int x = width / 2, y = height / 2;
    int fruitX = rand() % width, fruitY = rand() % height;
    int score = 0;
    Direction dir = RIGHT;

    bool gameOver = false;
    int tailX[1000], tailY[1000];
    int nTail = 0;

    while (!gameOver) {
        system("cls"); // Clear screen for Windows

        // Draw the game board
        for (int i = 0; i < width + 2; i++) cout << "#";
        cout << endl;

        for (int i = 0; i < height; i++) {
            for (int j = 0; j < width; j++) {
                if (j == 0) cout << "#"; // Draw left wall
                if (i == y && j == x) cout << "O"; // Draw snake head
                else if (i == fruitY && j == fruitX) cout << "*"; // Draw fruit
                else {
                    bool print = false;
                    for (int k = 0; k < nTail; k++) {
                        if (tailX[k] == j && tailY[k] == i) {
                            cout << "o"; // Draw snake tail
                            print = true;
                        }
                    }
                    if (!print) cout << " ";
                }

                if (j == width - 1) cout << "#"; // Draw right wall
            }
            cout << endl;
        }

        for (int i = 0; i < width + 2; i++) cout << "#"; // Draw bottom wall
        cout << endl;

        // Display score
        cout << "Score: " << score << endl;

        // Input for direction (non-blocking)
        if (_kbhit()) {
            switch (_getch()) {
            case 'w': if (dir != DOWN) dir = UP; break;
            case 's': if (dir != UP) dir = DOWN; break;
            case 'a': if (dir != RIGHT) dir = LEFT; break;
            case 'd': if (dir != LEFT) dir = RIGHT; break;
            }
        }

        // Move the snake
        int prevX = tailX[0], prevY = tailY[0];
        int prev2X, prev2Y;
        tailX[0] = x;
        tailY[0] = y;

        for (int i = 1; i < nTail; i++) {
            prev2X = tailX[i];
            prev2Y = tailY[i];
            tailX[i] = prevX;
            tailY[i] = prevY;
            prevX = prev2X;
            prevY = prev2Y;
        }

        switch (dir) {
        case UP: y--; break;
        case DOWN: y++; break;
        case LEFT: x--; break;
        case RIGHT: x++; break;
        }

        // Check for game over conditions
        if (x >= width || x < 0 || y >= height || y < 0) gameOver = true;
        for (int i = 0; i < nTail; i++) {
            if (tailX[i] == x && tailY[i] == y) gameOver = true;
        }

        // Check if the snake eats the fruit
        if (x == fruitX && y == fruitY) {
            score += 10;
            fruitX = rand() % width;
            fruitY = rand() % height;
            nTail++;
        }

        Sleep(100 - min(score / 10 * 10, 90)); // Adjust speed based on score
    }

    cout << "Game Over! Final Score: " << score << endl;
    cout << "Press Enter to return to the menu...";
    cin.ignore();
    cin.get();
}

//flip coin
string flipCoin() {
    isUserBusy = true;
    sleepForMilliseconds(250);
    srand(static_cast<unsigned int>(time(nullptr)));
    return (rand() % 2 == 0) ? "Heads" : "Tails";
    isUserBusy = false;
}

//rock paper scissors
void rockPaperScissors(const string& userName, const string& botName) {
    bool isRunning = true;
    isUserBusy = true;

    while (isRunning) {
        clearTerminal(); // Clear the terminal at the start of each menu display
        cout << "\nRock, Paper, Scissors Menu:\n";
        cout << "1. Single round\n";
        cout << "2. Best of three (3 rounds)\n";
        cout << "3. Custom number of rounds\n";
        cout << "4. Exit\n";
        cout << "Enter your choice: ";
        int choice;
        cin >> choice;

        if (choice == 4) {
            cout << "Exiting Rock, Paper, Scissors." << endl;
            break;
        }

        int totalRounds = 0;
        if (choice == 3) {
            cout << "Enter number of rounds to play (or 'q' to quit): ";
            string input;
            cin >> input;
            if (input == "q" || input == "Q") {
                cout << "Returning to menu." << endl;
                continue;
            }
            try {
                totalRounds = stoi(input);
            }
            catch (const invalid_argument&) {
                cout << "Invalid number of rounds. Returning to menu." << endl;
                continue;
            }
        }
        else if (choice == 1) {
            totalRounds = 1;
        }
        else if (choice == 2) {
            totalRounds = 3;
        }
        else {
            cout << "Invalid choice. Please try again." << endl;
            continue;
        }

        int userScore = 0, botScore = 0;
        srand(static_cast<unsigned int>(time(nullptr)));

        for (int round = 1; round <= totalRounds; ++round) {
            clearTerminal(); // Clear the terminal before each round
            cout << "\nRound " << round << " of " << totalRounds << ":\n";
            cout << "Choose your move: [1] Rock, [2] Paper, [3] Scissors: ";
            int userMove;
            cin >> userMove;

            if (choice == 3 && (userMove == 'q' || userMove == 'Q')) {
                cout << "Returning to menu." << endl;
                break;
            }

            if (userMove < 1 || userMove > 3) {
                cout << "Invalid move. Please select a valid option (1, 2, or 3)." << endl;
                --round;
                continue;
            }

            int botMove = (rand() % 3) + 1;
            string moves[] = { "Rock", "Paper", "Scissors" };

            cout << "Press Enter to see the result...";
            cin.ignore((std::numeric_limits<std::streamsize>::max)(), '\n');
            cin.get();

            clearTerminal(); // Clear the terminal before showing the result
            cout << userName << " chose: " << moves[userMove - 1] << endl;
            cout << botName << " chose: " << moves[botMove - 1] << endl;

            if (userMove == botMove) {
                cout << "It's a tie!" << endl;
            }
            else if ((userMove == 1 && botMove == 3) ||  // Rock beats Scissors
                (userMove == 2 && botMove == 1) ||  // Paper beats Rock
                (userMove == 3 && botMove == 2)) {  // Scissors beats Paper
                cout << userName << " wins this round!" << endl;
                ++userScore;
            }
            else {
                cout << botName << " wins this round!" << endl;
                ++botScore;
            }

            cout << "Current Score: " << userName << ": " << userScore << " | " << botName << ": " << botScore << endl;

            if (choice == 2 || choice == 3) {
                cout << "Press Enter to continue to the next round...";
                cin.get();
            }
        }

        if (choice == 1 || choice == 2) {
            clearTerminal(); // Clear the terminal before showing the final result
            cout << "\nFinal Score: " << userName << ": " << userScore << " | " << botName << ": " << botScore << endl;
            if (userScore > botScore) {
                cout << userName << " wins the game!" << endl;
            }
            else if (botScore > userScore) {
                cout << botName << " wins the game!" << endl;
            }
            else {
                cout << "The game is a tie!" << endl;
            }
        }

        cout << "Press Enter to return to the menu...";
        cin.ignore((std::numeric_limits<std::streamsize>::max)(), '\n');
        cin.get();
    }
    isUserBusy = false;
}

//Dice roll
int rollDice(int numberOfDice = 1) {
    isUserBusy = true;
    sleepForMilliseconds(250);

    srand(static_cast<unsigned int>(time(nullptr)));
    vector<int> diceResults(numberOfDice);
    int total = 0;

    for (int i = 0; i < numberOfDice; ++i) {
        diceResults[i] = (rand() % 6) + 1;
        total += diceResults[i];
    }

    // Display the result for each dice
    for (int i = 0; i < numberOfDice; ++i) {
        cout << (i + 1) << " >> dice = " << diceResults[i] << endl;
    }
    cout << "Full result = " << total << endl;

    isUserBusy = false;
    return total;
}
void rollDiceMenu() {
    int choice, numberOfDice;
    isUserBusy = true;

    while (true) {
        clearTerminal();
        cout << "\nSelect dice roll option:\n";
        cout << "1. Single dice (1 dice)\n";
        cout << "2. Double dice (2 dice)\n";
        cout << "3. Multi dice (any number of dice)\n";
        cout << "4. Exit\n";
        cout << "Enter your choice: ";
        cin >> choice;

        switch (choice) {
        case 1:
            rollDice(1);
            break;
        case 2:
            rollDice(2);
            break;
        case 3:
            cout << "Enter number of dice to roll: ";
            cin >> numberOfDice;
            rollDice(numberOfDice);
            break;
        case 4:
            cout << "Exiting dice roll menu." << endl;
            isUserBusy = false;
            return;
        default:
            cout << "Invalid choice. Please try again." << endl;
        }

        cout << "\nPress Enter to return to the menu...";
        cin.ignore();
        cin.get();
    }
}

// The main game menu function
void gameMenu(const string& userName, const string& botName) {
    while (true) {
        clearTerminal();
        cout << "Game Menu:\n"
            << "1. Flip Coin\n"
            << "2. Dice\n"
            << "3. Rock Paper Scissors\n"
            << "4. Snake Game\n"
            << "5. Exit\n"
            << "Your Option: ";
        int g;
        cin >> g;

        // Validate input
        if (cin.fail()) {
            cin.clear();
            cin.ignore((std::numeric_limits<std::streamsize>::max)(), '\n');
            cout << "Invalid input. Please enter a number between 1 and 4.\n";
            continue;
        }

        switch (g) {
        case 1:
            flipCoin();
            break;
        case 2:
            rollDiceMenu();
            break;
        case 3:
            rockPaperScissors(userName, botName);
            break;
        case 4:
            snakeGame();
            break;
        case 5:
            cout << "Exiting Game Menu. Goodbye!" << endl;
            return;
        default:
            cout << "Invalid option. Please choose between 1 and 4.\n";
            break;
        }

        // Pause to let the user see the result before returning to the menu
        cout << "Press Enter to return to the menu...";
        cin.ignore((std::numeric_limits<std::streamsize>::max)(), '\n');
        cin.get();
    }
}

//Random Number
void generateRandomNumber() {
    isUserBusy = true;
    clearTerminal();
    srand(static_cast<unsigned int>(time(nullptr)));
    int choice;
    cout << "Select an option:\n"
        << "1. Generate 2-digit number (0 to 99)\n"
        << "2. Generate 4-digit number (0 to 9999)\n"
        << "3. Generate custom range number\n";
    cout << "Please enter your choice (1-3): ";
    cin >> choice;
    switch (choice) {
    case 1: {
        int randomNum = rand() % 100;  // Generates number between 0 and 99
        cout << "Generated: " << randomNum << endl;
        break;
    }
    case 2: {
        int randomNum = rand() % 10000;  // Generates number between 0 and 9999
        cout << "Generated: " << randomNum << endl;
        break;
    }
    case 3: {
        int digitCount;
        cout << "How many digits do you want to generate? ";
        cin >> digitCount;

        if (digitCount <= 0) {
            cout << "Invalid input! Number of digits must be greater than 0." << endl;
            break;
        }

        // Calculate the lower and upper bounds for the given number of digits
        int lowerBound = pow(10, digitCount - 1);  // For example, for 3 digits, this is 100
        int upperBound = pow(10, digitCount) - 1;  // For 3 digits, this is 999

        if (digitCount == 1) {
            lowerBound = 0;  // Special case for single digit numbers (0-9)
        }

        // Generate random number within the specified range
        int randomNum = rand() % (upperBound - lowerBound + 1) + lowerBound;
        cout << "Generated " << digitCount << "-digit number: " << randomNum << endl;
        break;
    }
    default:
        cout << "Invalid choice!" << endl;
    }
    isUserBusy = false;
}

//Compare Number
string generateNumberWithComparison(string& userMessage) {
    userMessage = trim(userMessage);  // Trim leading and trailing spaces
    string comparisonOperator;
    string numberPart;

    // Determine if the first two characters are an operator
    if (userMessage.substr(0, 2) == ">=" || userMessage.substr(0, 2) == "<=") {
        comparisonOperator = userMessage.substr(0, 2);  // Get the operator
        numberPart = trim(userMessage.substr(2));       // Extract the number part
    }
    else {
        comparisonOperator = userMessage.substr(0, 1);  // Single character operator
        numberPart = trim(userMessage.substr(1));       // Extract the number part
    }

    if (comparisonOperator == ">=" || comparisonOperator == "<=" ||
        comparisonOperator == ">" || comparisonOperator == "<") {
        try {
            int threshold = stoi(numberPart);  // Convert to integer
            if (threshold <= 0) {
                return "Threshold must be greater than 0.";
            }

            int digitCount = to_string(threshold).length();
            int lowerLimit, upperLimit;

            // Adjust the lower and upper limits based on the threshold's digit count
            if (threshold == pow(10, digitCount - 1)) {  // Power of 10 (like 10, 100, 1000)
                lowerLimit = pow(10, digitCount - 2);    // Set range one digit lower
                upperLimit = pow(10, digitCount - 1) - 1;
            }
            else {
                lowerLimit = pow(10, digitCount - 1);    // Usual range for the digit count
                upperLimit = pow(10, digitCount) - 1;
            }

            int randomNum;

            // Generate random number based on the operator
            if (comparisonOperator == ">") {
                randomNum = rand() % (upperLimit - threshold) + threshold + 1;
                return "Generated: " + to_string(randomNum) + " > " + to_string(threshold);
            }
            else if (comparisonOperator == ">=") {
                randomNum = rand() % (upperLimit - threshold + 1) + threshold;
                return "Generated: " + to_string(randomNum) + " >= " + to_string(threshold);
            }
            else if (comparisonOperator == "<") {
                randomNum = rand() % (threshold - lowerLimit) + lowerLimit;
                return "Generated: " + to_string(randomNum) + " < " + to_string(threshold);
            }
            else if (comparisonOperator == "<=") {
                randomNum = rand() % (threshold - lowerLimit + 1) + lowerLimit;
                return "Generated: " + to_string(randomNum) + " <= " + to_string(threshold);
            }

        }
        catch (invalid_argument&) {
            return "Invalid number format. Please enter a valid number.";
        }
    }
    else {
        return "Invalid input. Please include '<', '>', '>=', or '<=' to specify comparison.";
    }
    return "Unexpected error occurred.";
}

//facts
string getRandomfacts() {
    vector<string> facts = {
        "Did you know that octopuses have three hearts?",
        "The Eiffel Tower can be 15 cm taller during the summer, thanks to the expansion of iron in the heat!",
        "Honey never spoils. Archaeologists have found pots of honey in ancient Egyptian tombs that are over 3,000 years old and still edible!",
        "Bananas are berries, but strawberries aren't.",
        "A day on Venus is longer than a year on Venus. It takes 243 Earth days to rotate once, but only 225 Earth days to orbit the sun!",
        "There are more stars in the universe than grains of sand on all the beaches on Earth.",
        "An ostrich's eye is bigger than its brain.",
        "Sharks have been around longer than trees. They appeared about 400 million years ago, while trees showed up 350 million years ago.",
        "Sloths can hold their breath longer than dolphins. They can slow their heart rate to stay underwater for up to 40 minutes.",
        "A group of flamingos is called a 'flamboyance'.",
        "Your nose can remember 50,000 different scents.",
        "The shortest war in history was between Britain and Zanzibar in 1896. Zanzibar surrendered after 38 minutes.",
        "Butterflies can taste with their feet.",
        "In Switzerland, it's illegal to own just one guinea pig because they're social animals and get lonely!",
        "Wombat poop is cube-shaped!",
        "There's a species of jellyfish known as 'Turritopsis dohrnii' that can revert back to its juvenile form, essentially making it immortal!",
        "Polar bears' skin is actually black, and their fur is transparent. It appears white because it reflects light.",
        "Cows have best friends and can get stressed when separated.",
        "A bolt of lightning contains enough energy to toast 100,000 slices of bread!",
        "Water can boil and freeze at the same time, in a phenomenon called the 'triple point'.",
        "The largest living organism on Earth is a fungus in Oregon, covering over 2,400 acres.",
        "If you stretched out all the DNA in your body, it would reach the sun and back about 600 times.",
        "The inventor of the frisbee was turned into a frisbee after he died.",
        "A day on Mars is only 40 minutes longer than a day on Earth.",
        "Humans share about 60/100 of their DNA with bananas.",
        "In space, astronauts can grow up to 2 inches taller due to the lack of gravity compressing their spine.",
        "Sea otters hold hands while sleeping to avoid drifting apart.",
        "You can't hum while holding your nose closed (go ahead, try it!).",
        "There are more bacteria in your mouth than there are people on Earth.",
        "Venus is the hottest planet in the solar system, despite Mercury being closer to the sun.",
        "The dot over the letter 'i' is called a tittle.",
        "Cats can't taste sweetness because they lack sweet taste receptors.",
        "Some turtles can breathe through their butts. It's called cloacal respiration!",
        "Elephants are the only animals that can't jump.",
        "A group of crows is called a 'murder'.",
        "The longest hiccuping spree lasted 68 years.",
        "Koalas sleep up to 22 hours a day.",
        "The hashtag symbol is technically called an 'octothorpe'.",
        "Humans and giraffes have the same number of neck vertebrae—seven!",
        "A snail can sleep for three years at a time.",
        "Hot water freezes faster than cold water, a phenomenon known as the Mpemba effect.",
        "It's impossible to hum while holding your nose and closing your mouth.",
        "Some fish can cough, just like humans!",
        "Tigers have striped skin, not just striped fur.",
        "Caterpillars have about 4,000 muscles in their bodies, while humans have around 600.",
        "A crocodile can't stick its tongue out.",
        "One human can produce enough saliva in a lifetime to fill two swimming pools.",
        "The speed of a computer mouse is measured in 'Mickeys'.",
        "Bees can recognize human faces.",
        "The human body contains enough fat to make seven bars of soap.",
        "Alaska is the westernmost and easternmost state in the U.S. because the Aleutian Islands cross the 180° meridian of longitude.",
        "Rats laugh when they're tickled.",
        "Male seahorses are the ones who give birth.",
        "Apples float because 25/100 of their volume is air.",
        "The longest recorded flight of a chicken is 13 seconds.",
        "Peanuts aren't technically nuts—they're legumes.",
        "The largest snowflake ever recorded was 15 inches wide and 8 inches thick.",
        "Humans are the only animals with chins.",
        "The world's smallest reptile was discovered in 2021, a tiny chameleon that's about the size of a sunflower seed."
    };
    srand(static_cast<unsigned int>(time(nullptr)));
    return facts[rand() % facts.size()];
}

//random topic
string getRandomtopic() {
    vector<string> topic = {
        "The impact of technology on medern education.",
        "Sustainable living practices and their benefits.",
        "The psychology of decision-making.",
        "Curtural influences on art and literature.",
        "The role of social media in shapeing public opinion.",
        "Space exploration and it's potential future implications.",
        "The importance of mental health awareness.",
        "Evolution of popular music genres over the decades.",
        "Bioethics and the ethical considerations in medical advancements.",
        "Artificial intelligence and its ethical implications.",
        "The role of women in historical revolutions.",
        "Climate change and its effects on global ecosystems.",
        "The future of renewable energy sources.",
        "The impact of globalizations on local cultures.",
        "Cybersecurity challenges in the digital age.",
        "The evolution of fashion trends over the years.",
        "The psychology of creativity and innovation.",
        "The history and cultural significance of board games.",
        "The influence of social media on bod image perception.",
        "Space tourism and its potential impact on the travel industry.",
        "The relationship between technology and privacy rights.",
        "the history and impact of major scientific discoveries.",
        "Curtural diversity in the workplace and its advantages.",
        "The psychology lf motivation and goal-setting.",
        "Urbanization and its effects on the environment.",
        "The role of education in reducing poverty.",
        "the impact of social media on political activism.",
        "The history and cultural significance of traditional cuisines.",
        "The future of work in the era of automation and artificial intelligence.",
        "The philosophy of existentialism and its relevance today.",
        "Biotechnology and its potential in hearltcare advancement.",
        "The history and impact of magic scientific experiments.",
        "The role of sports in promotion social cohesion.",
        "The psychology of resilience in the face of adversity.",
        "The cutural significance of folklore and mythology.",
        "Challences and opportunities in space colonization.",
        "The evolution of language and Communication.",
        "The impact of social media influencers on consumer behavior.",
        "The history and cultural significance on street art.",
        "The role of empathy in build meaningful relationships.",
        "The future of vistual reality and augmented reality technologies.",
        "The impact of artifical intelligence on job markets.",
        "The psychology of fear and its effects on decision-making.",
        "The role of education in fostering environmental awareness.",
        "The history and impact of major architectutal movements.",
        "Ethical considerations in genetic engineering and cloning.",
        "The influence of pop culture on societal norms.",
        "The intersection of art and techonologuy in the digital age.",
        "The importance of critical thinking skills in education and everyday life.",
        "The benefits of meditation.",
        "Exploring dfferent types of cuisines.",
        "The impact of technology on society.",
        "Effective strategies for time management.",
        "Environmenttal conservation and sustainability.",
        "The psychology of motivation.",
        "The importance of fitness and exercise",
        "The history and significance off music genres.",
        "Tips for effective communication in relationship.",
        "The influence of social media on out dail lives.",
        "The art of storytelling in literature and film.",
        "Exploring different forms of art therapy.",
        "The impact of artificial intelligence on the job market.",
        "The benefits of artificial gratitube in everyday life.",
        "The rise of sustainable fashion and ethical consumerism.",
        "Understanding the concept of cultural appropration.",
        "The role of education in shaping future generations.",
        "Exploring differet methods of stress manegement.",
        "The hitory and evolution of photography as an art form.",
    };
    srand(static_cast<unsigned int>(time(nullptr)));
    return topic[rand() % topic.size()];
}

// Function to get a random joke
string getRandomJoke() {
    vector<string> jokes = {
        "Why don't scientists trust atoms? Because they make up everything!",
        "What do you call fake spaghetti? An impasta!",
        "Why did the scarecrow win an award? Because he was outstanding in his field!",
        "How does a penguin build its house? Igloos it together.",
        "Why did the bicycle fall over? Because it was TWO-TIRED.",
        "What did one say to the other wall? i'll meet you at the corner.",
        "Why couldn't the leopard play hide and seek? Because he was always spotted.",
        "What did the coffee file a police report? It got mugged.",
        "What do you call a fish wearing a bowtie? so-FISH-ticated",
        "What's a vampire's favorite fruit? A blood orange.",
        "How do you organize a space party? Your Planet.",
        "Why did the math book look sad? Because it had too many problems.",
        "Wbat do you call a snowman with a six-pack? An Abdominal snowman.",
        "Why did the tomato turn red? Because it saw the salad dressing.",
        "Did you hear about the cheese factory explosion? There was nothing left but DE-BRIE.",
        "What did the janitor say when he jumped out of the closet? Supplies.",
        "Why don't skeletons fight fight each other? They don't have guts.",
        "What did one hat say to the other? Stay here, I'm going on a-HEAD",
        "what did the computer go to threrapy? It had too many bytes of emotional baggage.",
        "Why don't scientists trust atoms anymore? Because they make up everything, even imaginary friends.",
        "What did the Sushi say to the fish, WASABI.",
        "What did one ocean say to the other ocean? Nothing, They just a WAVED.",
        "Did you hear about the guy who invented Lifesavers? He made a Mint.",
        "What did the grape say when it got stepped on? Nothing, it just let out a little wine.",
        "How do you catch a squirrel? Climb a tree and act like a nut!",
        "Why don't oysters donate to charity? Because they are shellfish.",
        "I only know 25 letters of the alphabet. I don't know y.",
        "What's brown and sticky? A stick.",
        "What do you call a fish with no eyes? F_SH.",
        "How do you make a tissue dance? You put a little boogie in it.",
        "What do you get when you cross a snowman and a vampire? Frostbite.",
        "Why don't seagulls fly over the bay? Because then they'd be bagels.",
        "How do you catch a lion? The answer is simple: climb into a closet and roar like a zebra.",
        "What do you call a factory that make good products? A SATISFACTORY.",
        "What do you call a can opener that doesn't work? A CAN't opener.",
        "why did the banana go to the doctor? Because it wasn't PEELING well.",
        "why did the cookies go to the doutor? It was feeling crumbly.",
        "Why did the golfer bring two pairs of pants? In case he got A HOLE IN ONE.",
        "What's orange and sounds like a parrot? A carrot.",
        "what do you call a bear with no teech? A gummy bear.",
        "Why did the computer go to therapy? It had too bytes of emotional >> DAMAGE << baggage",
        // Add more jokes as needed
    };
    srand(static_cast<unsigned int>(time(nullptr)));
    return jokes[rand() % jokes.size()];
}

// Function to handle key-based actions directly in the main loop
// Short cut keys
void handleKeyPress(const string& userMessage) {
    if (userMessage.empty()) {
        cout << "No key pressed." << endl;
        return; // Exit early if no input is provided
    }

    char key = tolower(userMessage[0]);
    cout << "\nYou pressed: " << key << endl;

    // Check for valid keys before triggering actions
    switch (key) {
    case 'w': {
        // Prompt for clarification: web, weather, or Wikipedia
        cout << "Do you mean Website, Weather, or Wikipedia?\n"
            << "1. Open Web Link\n"
            << "2. Weather\n"
            << "3. Wikipedia\n"
            << "Your Option: ";

        char choice;
        cin >> choice;
        cin.ignore((std::numeric_limits<std::streamsize>::max)(), '\n');
        switch (choice) {
        case '1':
            cout << "Opening Website...\n";
            webOpeningApp();
            break;
        case '2':
            cout << "Fetching Weather Information...\n";
            searchWeather();
            break;
        case '3':
            cout << "Opening Wikipedia...\n";
            openWiki();
            break;
        default:
            cout << "Invalid option. Please try again.\n";
            break;
        }
        break;
    }

    case 'n': {
        cout << "Notepad? or Edit Notes?\n"
            << "1. Notepad.\n"
            << "2. Modify Notes\n"
            << "Your Option: ";
        char choice;
        cin >> choice;
        cin.ignore((std::numeric_limits<std::streamsize>::max)(), '\n');

        switch (choice) {
        case '1':
            notepadFunction();
            break;
        case '2': {
            string filepath;
            cout << "Please enter the file path to modify: ";
            cin.ignore();
            getline(cin, filepath);

            if (!filepath.empty()) {
                editNotepad(filepath);
            }
            else {
                cout << "Invalid file path.\n";
            }
            break;
        }
        default:
            cout << "Invalid option. Please choose 1 or 2.\n";
            break;
        }
        break;
    }

    case 'g': {
        cout << "Open Google or want to look for something?\n"
            << "1. Google.\n"
            << "2. Game Menu.\n"
            << "Your Option: ";
        char choice;
        cin >> choice;
        cin.ignore((std::numeric_limits<std::streamsize>::max)(), '\n');

        switch (choice) {
        case '1':
            googleMenu();
            break;
        case '2': {
            string userName = "Player";
            string botName = "VIAS";
            gameMenu(userName, botName);
            break;
        }
        default:
            cout << "Invalid option. Please choose 1 or 2.\n";
            break;
        }
        break;
    }

    case 'y': {
        cout << "Which One??\n"
            << "1. Open YouTube HomePage.\n"
            << "2. YouTube Search.\n"
            << "Your Option: ";
        char choice;
        cin >> choice;
        cin.ignore((std::numeric_limits<std::streamsize>::max)(), '\n');

        switch (choice) {
        case '1':
            openYouTube();
            break;
        case '2':
            searchYouTube();
            break;
        default:
            cout << "Invalid option. Please choose 1 or 2.\n";
            break;
        }
        break;
    }

    case 'c': {
        cout << "Which One??\n"
            << "1. Time Convert\n"
            << "2. Base Convert (Bin, Oct, Dec, Hex)\n"
            << "3. Calendar\n"
            << "4. Coding.\n"
            << "5. Edit Code.\n"
            << "Your Option: ";
        char choice;
        cin >> choice;
        cin.ignore((std::numeric_limits<std::streamsize>::max)(), '\n');

        switch (choice) {
        case '1': {
            string result = getTimeConvert();
            cout << result << endl;
            break;
        }
        case '2':
            convertBase();
            break;
        case '3': {
            time_t now = time(0);
            tm* ltm = localtime(&now);

            int month = ltm->tm_mon;
            int day = ltm->tm_mday;
            int year = 1900 + ltm->tm_year;

            string monthName = months[month];

            drawCalendar(monthName, year, day);
            cout << "\nCurrent Date: " << monthName << " " << day << ", " << year << endl;
            break;
        }
        case '4': {
            string result = writeBuildRunCode();
            cout << result << endl;
            break;
        }
        case '5': {
            string result = editCode("./Programs/");
            cout << result << endl;
            break;
        }
        default:
            cout << "Invalid option. Please choose 1, 2, or 3.\n";
            break;
        }
        break;
    }
    default:
        cout << "Invalid key. Try different keys...\n";
        break;
    }
}

void handleUserMessage() {
    static int blankCount = 0;

    blankCount++;

    if (blankCount >= 10) {
        vector<string> frustrationResponses = {
            "VIAS: I'm getting frustrated! Please say something or apologize!",
            "VIAS: I'm starting to get a bit irritated. Can you please provide some input or say sorry?",
            "VIAS: It's getting a bit annoying now. How about giving me some details or apologizing?",
            "VIAS: I really need you to say something or apologize before I lose my patience.",
            "VIAS: I'm feeling a bit frustrated here. Let's either chat or clear things up with an apology."
        };
        srand(static_cast<unsigned int>(time(nullptr)));
        string frustrationResponse = frustrationResponses[rand() % frustrationResponses.size()];
        cout << frustrationResponse << endl;
    }
    else {
        vector<string> randomResponses = {
            "It looks like you didn't enter anything. Can you please provide more details?",
            "Sorry, I can't understand blank voice.",
            "I didn't get anything. Could you please type something?",
            "It seems like you're not saying anything. Can you elaborate?",
            "I'm not sure what you're trying to convey. Can you type your message?"
        };
        srand(static_cast<unsigned int>(time(nullptr)));
        string randomResponse = randomResponses[rand() % randomResponses.size()];
        cout << "VIAS: " << randomResponse << endl;
    }
}

#else // for Other Platforms (Linux, Apple,...)

// Function to convert a single character to binary
string charToBinary(char c) {
    return bitset<8>(c).to_string(); // Convert char to 8-bit binary
}

// Function to convert string to decimal (ASCII values)
string stringToDecimal(const string& input) {
    stringstream ss;
    for (char c : input) {
        if (c != ' ') {
            ss << static_cast<int>(c) << " "; // Convert each character to its ASCII value
        }
        else {
            ss << "32 "; // ASCII value for space is 32
        }
    }
    return ss.str();
}

// Function to convert string to hexadecimal
string stringToHex(const string& input) {
    stringstream ss;
    for (char c : input) {
        if (c != ' ') {
            ss << hex << setw(2) << setfill('0') << uppercase << static_cast<int>(c) << " "; // Convert each character to hex
        }
        else {
            ss << "20 "; // ASCII value for space in hex is 20
        }
    }
    return ss.str();
}

// Function to convert string to octal
string stringToOctal(const string& input) {
    stringstream ss;
    for (char c : input) {
        if (c != ' ') {
            ss << oct << static_cast<int>(c) << " "; // Convert each character to octal
        }
        else {
            ss << "40 "; // ASCII value for space in octal is 40
        }
    }
    return ss.str();
}

// Function to handle user input with backspace and spacebar support
string getUserInput() {
    string input;
    char ch;

    cout << "Please enter the words (letters and numbers): " << flush;

    while (true) {
        ch = getchar(); // Read a character from the input

        if (ch == '\n') { // Enter key pressed
            break;
        }
        else if (ch == '\b' || ch == 127) { // Backspace handling (127 for some systems)
            if (!input.empty()) {
                input.pop_back(); // Remove last character
                cout << "\b \b" << flush; // Handle backspace visually in the terminal
            }
        }
        else {
            // Add character to the input string
            input += ch;
            cout << ch << flush; // Display typed character in the terminal
        }
    }

    return input;
}

// Main conversion function
void convertBase() {
    while (true) {
        isUserBusy = true;
        // Ask user to input the string (with backspace handling)
        string input;
        cout << "Enter the string to convert (or 'exit' to quit): ";
        getline(cin, input);

        if (input.empty()) {
            cout << "Input cannot be empty. Please try again." << endl;
            continue;
        }

        if (input == "exit") {
            cout << "Exiting convertBase..." << endl;
            return;
        }

        while (true) {
            // Ask user to choose an option
            cout << "\nChoose an option:\n"
                << "1. Convert to Binary\n"
                << "2. Convert to Octal (Base 8)\n"
                << "3. Convert to Decimal\n"
                << "4. Convert to Hexadecimal\n"
                << "5. Retype\n"
                << "6. Exit\n"
                << "Your choice: ";

            string choiceInput;
            getline(cin, choiceInput);

            // Convert choice input to an integer if valid
            int choice = 0;
            stringstream ss(choiceInput);
            if (!(ss >> choice) || choice < 1 || choice > 6) {
                cout << "Invalid choice. Please enter a number between 1 and 6.\n";
                continue;
            }

            switch (choice) {
            case 1: { // Convert to Binary
                cout << "\nBinary: ";
                for (char c : input) {
                    if (c != ' ') {
                        cout << charToBinary(c) << " "; // Convert each character to binary
                    }
                    else {
                        cout << " "; // Preserve spaces in binary output
                    }
                }
                cout << "\n";
                break;
            }
            case 2: { // Convert to Octal (Base 8)
                cout << "\nOctal (Base 8): " << stringToOctal(input) << "\n";
                break;
            }
            case 3: { // Convert to Decimal (ASCII values)
                cout << "\nDecimal: " << stringToDecimal(input) << "\n";
                break;
            }
            case 4: { // Convert to Hexadecimal
                cout << "\nHexadecimal: " << stringToHex(input) << "\n";
                break;
            }
            case 5: { // Retype the input
                break; // Exit the inner loop to retype
            }
            case 6: { // Exit the void function
                cout << "Exiting...\n";
                return;
            }
            }

            if (choice == 5) break; // Break from the inner loop for retyping
        }
        isUserBusy = false;
    }
}

//date
string getDayOfWeek(int dayOfWeek) {
    const string days[] = { "Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat" };
    return days[dayOfWeek];
}

string getMonthName(int month) {
    const string months[] = { "Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul", "Aug", "Sep", "Oct", "Nov", "Dec" };
    return months[month];
}

string getCurrentTime() {
    time_t ttime = time(0);
    tm* local_time = localtime(&ttime);

    string dayOfWeek = getDayOfWeek(local_time->tm_wday);
    string monthName = getMonthName(local_time->tm_mon);

    // Convert 24-hour time to 12-hour format with AM/PM
    int hour = local_time->tm_hour;
    string period = (hour >= 12) ? "PM" : "AM";
    if (hour == 0) hour = 12;
    else if (hour > 12) hour -= 12;

    stringstream timeStream;
    timeStream << "Date: " << monthName << ", " << dayOfWeek << ", " << local_time->tm_mday << ", "
        << 1900 + local_time->tm_year << endl
        << "Time (12h): " << hour << " H : "
        << local_time->tm_min << " Min : "
        << local_time->tm_sec << " Sec " << period;

    return timeStream.str();
}

// Function to run Linux terminal operations
void runLinuxTerminalOperation() {
    isUserBusy = true;
    vector<string> exitCommands = { "exitterminal", "end", "killlinux", "exit terminal", "kill linux", "kill terminal", "killterminal" };
    bool isRoot = false; // Track whether user is in root mode

    // Start in home directory (~)
    system("cd ~");

    clearTerminal(); // Your function to clear the terminal screen

    cout << "This is Linux Terminal Operation..." << endl;
    sleepForMilliseconds(150); // Simulated delay

    while (true) {
        cout << (isRoot ? "root@linux:~# " : "> "); // Show root prompt if sudo -s was used
        string userCommand;
        getline(cin, userCommand);

        // Convert input to lowercase for consistent checking
        string lowerCommand;
        transform(userCommand.begin(), userCommand.end(), back_inserter(lowerCommand), ::tolower);

        // Check if the user wants to exit the terminal operation
        if (find(exitCommands.begin(), exitCommands.end(), lowerCommand) != exitCommands.end()) {
            cout << "Exiting Terminal Operation..." << endl;
            break;
        }

        // Simulate switching to root user with 'sudo -s'
        if (lowerCommand == "sudo -s") {
            isRoot = true; // Switch to root mode
            cout << "Switched to root. You are now operating as root user." << endl;
            continue;
        }

        // If the user types 'exit' or 'exit root' in root mode, go back to normal mode
        if ((lowerCommand == "exit" || lowerCommand == "exit root") && isRoot) {
            isRoot = false;
            cout << "Exited root mode." << endl;
            continue;
        }

        // Execute the user's command in the terminal, handling root mode if needed
        if (isRoot) {
            system(("cd / && " + userCommand).c_str()); // Commands from the root directory
        }
        else {
            system(("cd ~ && " + userCommand).c_str()); // Commands from the home directory
        }
    }
    isUserBusy = false;
}

//Win_CMD
void runCmdOperation() {
    isUserBusy = true;
    vector<string> exitCommands = { "end","exitcmd" };

    // Clear the terminal (CMD) screen
    clearTerminal();

    cout << "This is Window CMD Operation... [type 'end', 'exitcmd' to close CMD]" << endl;
    sleepForMilliseconds(150);
    while (true) {
        cout << "> ";
        string userCommand;
        getline(cin, userCommand);

        // Convert input to lowercase for consistent checking
        string lowerCommand;
        transform(userCommand.begin(), userCommand.end(), back_inserter(lowerCommand), ::tolower);

        // Check if the user wants to exit the CMD operation
        if (find(exitCommands.begin(), exitCommands.end(), lowerCommand) != exitCommands.end()) {
            // cout << "Exiting CMD Operation..." << endl;
            break;
        }

        // Execute the user's command in CMD
        system(userCommand.c_str());
    }
    isUserBusy = false;
}

//Animation Response with a flag to enable or disable typing animation
void typeResponse(const string& response, bool useAnimation = true) {
    if (useAnimation) {
        for (const char& ch : response) {
            cout << ch;
            cout.flush();  // Ensure each character is displayed immediately
            this_thread::sleep_for(chrono::milliseconds(25));  // Simulate typing delay
        }
        cout << endl;
    }
    else {
        // If animation is disabled, print the response immediately
        cout << response << endl;
    }
}

//QR
void generateQRCode(const string& url) {
    isUserBusy = true;
    cout << "Generating QR Code for: " << url << endl;

#if defined(_WIN32) || defined(_WIN64)
    // Windows-specific QR code generation logic
    string command = "start https://qrenco.de/" + url;
    system(command.c_str());

#elif defined(__linux__)
    // Linux-specific QR code generation logic
    string command = "curl https://qrenco.de/" + url;
    system(command.c_str());

#elif defined(__APPLE__) || defined(__MACH__)
    // macOS-specific QR code generation logic
    string command = "open https://qrenco.de/" + url;
    system(command.c_str());

#else
    cout << "Unsupported OS for QR Code generation." << endl;
#endif

    // Wait for user input before clearing the screen
    cout << "\nPress Enter to continue...";
    cin.ignore((std::numeric_limits<std::streamsize>::max)(), '\n');
    cin.get();  // Wait for Enter key press
    isUserBusy = false;
}
void displayHistory(ifstream& historyFile, bool canRecreate) {
    isUserBusy = true;
    string line;
    vector<string> history;
    int lineCount = 0;
    int page = 1;
    const int pageSize = 10;
    int totalPages = 1;

    while (getline(historyFile, line)) {
        history.push_back(line);
        lineCount++;
    }

    totalPages = (lineCount + pageSize - 1) / pageSize;

    do {
        clearTerminal();
        cout << "History - Page " << page << " / " << totalPages << endl;
        for (int i = (page - 1) * pageSize; i < min(lineCount, page * pageSize); ++i) {
            cout << i + 1 << ". " << history[i] << endl;
        }
        if (canRecreate) {
            cout << "\nEnter the number of the link to recreate QR code or type [n] for next page, [p] for previous page, [exit] to go back: ";
            string choice;
            cin >> choice;
            if (choice == "n" && page < totalPages) page++;
            else if (choice == "p" && page > 1) page--;
            else if (choice == "exit") break;
            else {
                int index = stoi(choice) - 1;
                if (index >= 0 && index < history.size()) {
                    generateQRCode(history[index]);
                }
                else {
                    cout << "Invalid input, please try again." << endl;
                }
            }
        }
        else {
            cout << "\nType [n] for next page, [p] for previous page, [exit] to go back: ";
            string choice;
            cin >> choice;
            if (choice == "n" && page < totalPages) page++;
            else if (choice == "p" && page > 1) page--;
            else if (choice == "exit") break;
        }
    } while (true);
    isUserBusy = false;
}

void qrCodeMenu() {
    isUserBusy = true;
    string userChoice;
    do {
        clearTerminal();
        cout << "Main Menu:\n1. Create QR Code\n2. Recall / History\n3. Exit\n";
        cout << "Your Option: ";
        getline(cin, userChoice);

        if (userChoice == "1") {
            clearTerminal();
            cout << "Paste your Link / URL: ";
            string url;
            getline(cin, url);
            generateQRCode(url);

            filesystem::create_directory("QRcode");
            ofstream historyFile("QRcode/QR_His.txt", ios::app);
            historyFile << url << endl;
            historyFile.close();

        }
        else if (userChoice == "2") {
            string recallChoice;
            do {
                clearTerminal();
                cout << "Recall / History:\n1. Show History\n2. Recall QR\n3. Main Menu\n4. Exit QR\n";
                cout << "Your Option: ";
                getline(cin, recallChoice);

                if (recallChoice == "1") {
                    ifstream historyFile("QRcode/QR_His.txt");
                    if (historyFile.is_open()) {
                        displayHistory(historyFile, false);
                        historyFile.close();
                    }
                    else {
                        cout << "No history found." << endl;
                    }

                }
                else if (recallChoice == "2") {
                    ifstream historyFile("QRcode/QR_His.txt");
                    if (historyFile.is_open()) {
                        displayHistory(historyFile, true);
                        historyFile.close();
                    }
                    else {
                        cout << "No history found." << endl;
                    }

                }
                else if (recallChoice == "3") {
                    break;
                }
                else if (recallChoice == "4") {
                    return;
                }
            } while (recallChoice != "3" && recallChoice != "4");
        }
        else if (userChoice == "3") {
            break;
        }
    } while (userChoice != "3");
    isUserBusy = false;
}

// Declaration of the openWebLink function
void openWebLink(const string& url);

//google.com
void openGoogle() {
#if defined(_WIN32) || defined(_WIN64)
    system("START https://www.google.com");
#elif defined(__linux__)
    system("xdg-open https://www.google.com");
#elif defined(__APPLE__)
    system("open https://www.google.com");
#else
    cout << "Platform not supported." << endl;
#endif
}
void searchGoogle() {
    cout << "VIAS: What do you want to search: ";
    string query;
    getline(cin, query);

    replace(query.begin(), query.end(), ' ', '+');
    string searchUrl = "https://www.google.com/search?q=" + query;

    openWebLink(searchUrl);
}

void googleMenu() {
    while (true) {
        clearTerminal();
        cout << "Google Menu:\n";
        cout << "1. Go to Google Home\n";
        cout << "2. Search on Google\n";
        cout << "3. Exit\n";
        cout << "Enter your choice: ";

        string input;
        getline(cin, input);

        if (input.empty()) {
            cout << "No input provided. Please enter a choice." << endl;
            continue;
        }

        int choice;
        try {
            choice = stoi(input);
        }
        catch (const invalid_argument&) {
            cout << "Invalid input. Please enter a number." << endl;
            continue;
        }

        switch (choice) {
        case 1:
            cout << "Opening Google homepage..." << endl;
            openGoogle();
            break;

        case 2:
            searchGoogle();
            break;

        case 3:
            return;

        default:
            cout << "Invalid choice. Please enter a number between 1 and 3." << endl;
            break;
        }

        cout << "\nPress Enter to return to the menu...";
        cin.ignore((std::numeric_limits<std::streamsize>::max)(), '\n');
    }
}

void searchWeather() {
    isUserBusy = true;
    string userChoice;
    do {
        clearTerminal();
        cout << "Weather Menu:\n1. Default Weather Search\n2. Search by Location\n3. Exit\n";
        cout << "Your Option: ";
        getline(cin, userChoice);

        if (userChoice == "1") {
            // Option 1: Open default weather search
            string searchUrl = "https://www.google.com/search?q=weather";
            openWebLink(searchUrl);

        }
        else if (userChoice == "2") {
            // Option 2: User types in a location
            cout << "your location: ";
            string location;
            getline(cin, location);
            replace(location.begin(), location.end(), ' ', '+');
            string searchUrl = "https://www.google.com/search?q=weather+" + location;
            openWebLink(searchUrl);

        }
        else if (userChoice == "3") {
            // Option 3: Exit
            break;

        }
        else {
            cout << "Invalid option. Please choose 1, 2, or 3.\n";
        }
    } while (userChoice != "3");
    isUserBusy = false;
}

//kali
void openKaliWeb() {
#if defined(_WIN32) || defined(_WIN64)
    system("START https://www.kali.org");  // For Windows
#elif defined(__linux__)
    system("xdg-open https://www.kali.org");  // For Linux
#elif defined(__APPLE__)
    system("open https://www.kali.org");  // For macOS
#else
    cout << "Platform not supported." << endl;
#endif
}

void openYouTube() {
    openWebLink("https://www.youtube.com");
}

void searchYouTube() {
    isUserBusy = true;
    cout << "VIAS: What do you want to search on YouTube: ";
    string query;
    getline(cin, query);
    replace(query.begin(), query.end(), ' ', '+');

    string searchUrl = "https://www.youtube.com/results?search_query=" + query;

    openWebLink(searchUrl);
    isUserBusy = false;
}

void youtubeMenu() {
    while (true) {
        clearTerminal();
        cout << "YouTube Menu:\n";
        cout << "1. YT Home\n";
        cout << "2. Search\n";
        cout << "3. Exit\n";
        cout << "Enter your choice: ";

        string input;
        getline(cin, input);

        if (input.empty()) {
            cout << "No input provided. Please enter a choice." << endl;
            continue;
        }

        int choice;
        try {
            choice = stoi(input);
        }
        catch (const invalid_argument&) {
            cout << "Invalid input. Please enter a number." << endl;
            continue;
        }

        switch (choice) {
        case 1:
            openYouTube();
            cout << "Opening YouTube homepage..." << endl;
            break;

        case 2:
            searchYouTube();
            break;

        case 3:
            return;

        default:
            cout << "Invalid choice. Please enter a number between 1 and 3." << endl;
            break;
        }

        cout << "\nPress Enter to return to the menu...";
        cin.ignore((std::numeric_limits<std::streamsize>::max)(), '\n');
    }
}

void openWiki() {
    bool isRunning = true;
    string searchQuery;

    while (isRunning) {
        clearTerminal();
        cout << "\nWikipedia Menu:\n";
        cout << "1. Home (Wikipedia homepage)\n";
        cout << "2. Search Wikipedia\n";
        cout << "3. Exit\n";
        cout << "Enter your choice: ";
        int choice;
        cin >> choice;

        switch (choice) {
        case 1:
            openWebLink("https://en.wikipedia.org");
            break;
        case 2:
            cin.ignore();  // Clear the input buffer
            cout << "Enter your search query: ";
            getline(cin, searchQuery);

            if (!searchQuery.empty()) {
                string searchURL = "https://en.wikipedia.org/wiki/" + searchQuery;
                replace(searchURL.begin(), searchURL.end(), ' ', '_');
                openWebLink(searchURL);
            }
            else {
                cout << "Search query cannot be empty. Please try again." << endl;
            }
            break;
        case 3:
            cout << "Exiting Wikipedia menu." << endl;
            isRunning = false;
            break;
        default:
            cout << "Invalid choice. Please try again." << endl;
        }
    }
}

// Function to get a random Rick Roll response for the console message
string getRandomRickRollResponse() {
    vector<string> responses = {
        "VIAS: haha..., You're not getting me today, nice try, here's some magic trick!",
        "VIAS: Gotcha! Enjoy this little surprise!",
        "VIAS: Thought you'd catch me off guard? Think again!",
        "VIAS: You're in for a treat! Enjoy the show!",
        "VIAS: Oh no, you didn't expect this, did you?",
        "VIAS: I see what you're trying to do... too bad, I beat you to it!",
        "VIAS: Nice try! But I'm always one step ahead!",
        "VIAS: Haha! Did you really think I wouldn't see that coming?",
        "VIAS: I knew this was coming. Here's a little surprise for you!",
        "VIAS: Looks like I'm the one in control today!",
        "VIAS: You think you're clever, but I've outsmarted you!"
    };
    // Return a random response
    return responses[rand() % responses.size()];
}

//Operation Roll
void openRickRoll() {
    isUserBusy = true;

    // Get a random console message
    cout << getRandomRickRollResponse() << endl;

    sleepForMilliseconds(500);
    openWebLink("https://www.youtube.com/watch?v=dQw4w9WgXcQ");
    sleepForMilliseconds(900);
    isUserBusy = false;
}

//define user's device 
void openWebLink(const string& url) {
#if defined(_WIN32) || defined(_WIN64)
    system(("start " + url).c_str());  // For Windows
#elif defined(__linux__)
    system(("xdg-open " + url).c_str());  // For Linux
#elif defined(__APPLE__)
    system(("open " + url).c_str());  // For macOS
#else
    cout << "Platform not supported." << endl;
#endif
}

// Function to convert input to lowercase for case-insensitive matching
string toLower(string input) {
    transform(input.begin(), input.end(), input.begin(), ::tolower);
    return input;
}

// Open Web page and web app
void webOpeningApp() {
    isUserBusy = true;
    string historyFolder = "./Web_History/";
    fs::create_directory(historyFolder);

    ofstream linkHistoryFile(historyFolder + "Link_History.txt", ios::app);
    if (!linkHistoryFile.is_open()) {
        cerr << "Error opening link history file for writing.\n";
        return;
    }

    // Main menu loop
    while (true) {
        clearTerminal();
        cout << "What would you like to do?\n";
        cout << "1. Open home browser\n";
        cout << "2. Search web link\n";
        cout << "3. Exit\n";
        cout << "Enter your choice: ";

        string input;
        getline(cin, input);

        if (input.empty()) {
            cout << "No input provided. Please enter a choice." << endl;
            continue;
        }

        int choice;
        try {
            choice = stoi(input);
        }
        catch (const invalid_argument&) {
            cout << "Invalid input. Please enter a number." << endl;
            continue;
        }

        switch (choice) {
        case 1:
            // Open Home Page
            openWebLink("https://www.google.com");
            cout << "Opening your home browser..." << endl;
            break;

        case 2: {
            // Web Search
            cout << "Paste your link / URL here: ";
            string url;
            getline(cin, url);

            if (url.empty()) {
                cout << "No URL entered. Please try again." << endl;
            }
            else {
                // Check if the URL contains "https://"
                if (url.find("https://") == 0) {
                    // Remove "https://"
                    url = url.substr(8);
                }
                // Add "https://" to the beginning
                url = "https://" + url;

                openWebLink(url);
                cout << "Opening " << url << "..." << endl;

                // Save to history
                linkHistoryFile << url << endl;
            }
            break;
        }

        case 3:
            // Exit
            linkHistoryFile.close();
            return;  // Exit the function

        default:
            cout << "Invalid choice. Please enter a number between 1 and 3." << endl;
            break;
        }

        cout << "\nPress Enter to return to the menu...";
        cin.ignore((std::numeric_limits<std::streamsize>::max)(), '\n');
    }
    isUserBusy = false;
}


// Function to rename the notepad file
string renameNotepad(const string& oldFilepath) {
    isUserBusy = true;
    cout << endl;
    cout << "Enter the new filename: ";
    string newFilename;
    getline(cin, newFilename);

    string newFilepath = "./Note/" + newFilename + ".txt";

    try {
        fs::rename(oldFilepath, newFilepath);
        return "File renamed successfully to " + newFilepath;
    }
    catch (const fs::filesystem_error& e) {
        return "Error renaming file: " + string(e.what());
    }
    isUserBusy = false;
    return "finished...";
}

// Function to display the notepad file content with a pause
void displayNotepad(const string& filepath) {
    isUserBusy = true;
    ifstream inFile(filepath);
    if (inFile.is_open()) {
        cout << endl;
        string line;
        while (getline(inFile, line)) {
            cout << line << endl;
        }
        inFile.close();
        cout << "\nPress Enter to continue...";
        cin.ignore((std::numeric_limits<std::streamsize>::max)(), '\n');
    }
    else {
        cerr << "Error opening file for reading: " << filepath << endl;
    }
    isUserBusy = false;
}

// Function to edit the notepad file content interactively
void editNotepad(const string& filepath) {
    isUserBusy = true;
    vector<string> lines;
    string line;
    ifstream inFile(filepath);

    if (inFile.is_open()) {
        cout << endl;
        while (getline(inFile, line)) {
            lines.push_back(line);
        }
        inFile.close();
    }
    else {
        cerr << "Error opening file for reading: " << filepath << endl;
        return;
    }

    size_t start = 0;
    const size_t displayCount = 10; // Number of lines to display at a time
    size_t totalPages = (lines.size() + displayCount - 1) / displayCount;

    while (true) {
        clearTerminal();
        size_t currentPage = (start / displayCount) + 1;
        cout << "\nCurrent content (Page " << currentPage << " of " << totalPages << "):\n";
        cout << "Displaying lines " << start + 1 << " to " << min(start + displayCount, lines.size()) << ":\n";

        for (size_t i = start; i < min(start + displayCount, lines.size()); ++i) {
            cout << i + 1 << ": " << lines[i] << endl;
        }

        cout << "\nOptions: [n] Next, [p] Previous, [e] Edit, [s] Save and Exit, [page #] Jump to Page\n";
        cout << "Enter your choice: ";
        string choice;
        getline(cin, choice);
        choice = toLower(choice);

        if (choice == "n" && start + displayCount < lines.size()) {
            start += displayCount;
        }
        else if (choice == "p" && start > 0) {
            start -= displayCount;
        }
        else if (choice == "e") {
            cout << "Enter the line number to edit (in current view): ";
            size_t lineNumber;
            cin >> lineNumber;
            cin.ignore();

            if (lineNumber > 0 && lineNumber <= displayCount && start + lineNumber - 1 < lines.size()) {
                size_t editLine = start + lineNumber - 1;
                cout << "Current line " << editLine + 1 << ": " << lines[editLine] << endl;
                cout << "Enter new content: ";
                getline(cin, lines[editLine]);
            }
            else {
                cout << "Invalid line number. Try again.\n";
            }
        }
        else if (choice == "s") {
            ofstream outFile(filepath, ios::trunc);
            if (outFile.is_open()) {
                for (const string& modifiedLine : lines) {
                    outFile << modifiedLine << endl;
                }
                outFile.close();
                cout << "File updated successfully.\n";
            }
            else {
                cerr << "Error opening file for writing.\n";
            }
            break;
        }
        else if (choice.find("page ") == 0) {
            int pageNumber = stoi(choice.substr(5));
            if (pageNumber > 0 && pageNumber <= totalPages) {
                start = (pageNumber - 1) * displayCount;
            }
            else {
                cout << "Invalid page number. Try again.\n";
            }
        }
        else {
            cout << "Invalid choice. Try again.\n";
        }
    }
    isUserBusy = false;
}

// Function to delete a notepad file
void deleteFile(const string& filepath) {
    isUserBusy = true;
    if (fs::remove(filepath)) {
        cout << "File deleted successfully." << endl;
    }
    else {
        cerr << "Error deleting file. Please check the file path and try again.\n";
    }
    isUserBusy = false;
}

// Function to handle file operations (continue writing, replace, delete, edit)
void continueWriting(const string& filepath) {
    isUserBusy = true;

    // Read the existing file content into a vector
    vector<string> notes;
    string line;
    ifstream inFile(filepath);

    if (inFile.is_open()) {
        while (getline(inFile, line)) {
            notes.push_back(line);
        }
        inFile.close();
    }
    else {
        cerr << "Error opening file for reading: " << filepath << endl;
        isUserBusy = false;
        return;
    }

    while (true) {
        clearTerminal();
        cout << "\nCurrent Notes in " << filepath << ":\n";
        for (size_t i = 0; i < notes.size(); ++i) {
            cout << i + 1 << ": " << notes[i] << endl;
        }

        cout << "\nOptions:\n"
            << "[c] Continue write\n"
            << "[a] Add a new line\n"
            << "[e] Edit a line\n"
            << "[d] Delete a line\n"
            << "[s] Save and Exit\n"
            << "[x] Exit without saving\n"
            << "Enter your choice: ";

        string choice;
        getline(cin, choice);
        choice = toLower(choice);

        if (choice == "c") {  // Continue writing
            cout << "\nContinue Writing Options:\n"
                << "1. Continue (append at the end)\n"
                << "2. In between (insert at a specific position)\n"
                << "Enter your choice: ";
            string subChoice;
            getline(cin, subChoice);

            if (subChoice == "1") {  // Append to the end
                clearTerminal();
                cout << "\n>> Continue from previous content. Type 'EXIT' on a new line to stop:\n\n";
                while (true) {
                    getline(cin, line);
                    if (toLower(line) == "exit") {
                        break;
                    }
                    notes.push_back(line);
                }
            }
            else if (subChoice == "2") {  // Insert in between
                cout << "Enter the line number to insert after: ";
                size_t lineNumber;
                cin >> lineNumber;
                cin.ignore();

                if (lineNumber > 0 && lineNumber <= notes.size()) {
                    cout << ">> Insert new lines below line " << lineNumber << ". Type 'EXIT' on a new line to stop:\n\n";
                    vector<string>::iterator it = notes.begin() + lineNumber;
                    while (true) {
                        getline(cin, line);
                        if (toLower(line) == "exit") {
                            break;
                        }
                        it = notes.insert(it, line) + 1;
                    }
                }
                else {
                    cout << "Invalid line number. Returning to menu.\n";
                }
            }
            else {
                cout << "Invalid choice. Returning to menu.\n";
            }
        }
        else if (choice == "e") {  // Edit a line
            cout << "\nEdit Line Options:\n"
                << "1. Insert lines below\n"
                << "2. Overwrite line\n"
                << "Enter your choice: ";
            string subChoice;
            getline(cin, subChoice);

            cout << "Enter the line number to edit: ";
            size_t lineNumber;
            cin >> lineNumber;
            cin.ignore();

            if (lineNumber > 0 && lineNumber <= notes.size()) {
                if (subChoice == "1") {  // Insert lines below
                    cout << ">> Insert new lines below line " << lineNumber << ". Type 'EXIT' on a new line to stop:\n\n";
                    vector<string>::iterator it = notes.begin() + lineNumber;
                    while (true) {
                        getline(cin, line);
                        if (toLower(line) == "exit") {
                            break;
                        }
                        it = notes.insert(it, line) + 1;
                    }
                }
                else if (subChoice == "2") {  // Overwrite line
                    cout << "Current content: " << notes[lineNumber - 1] << endl;
                    cout << "Enter new content: ";
                    getline(cin, notes[lineNumber - 1]);
                }
                else {
                    cout << "Invalid choice. Returning to menu.\n";
                }
            }
            else {
                cout << "Invalid line number. Try again.\n";
            }
        }
        else if (choice == "a") {  // Add a new line
            cout << "Enter the new line content: ";
            string newLine;
            getline(cin, newLine);
            notes.push_back(newLine);
        }
        else if (choice == "d") {  // Delete a line
            cout << "Enter the line number to delete: ";
            size_t lineNumber;
            cin >> lineNumber;
            cin.ignore();

            if (lineNumber > 0 && lineNumber <= notes.size()) {
                notes.erase(notes.begin() + lineNumber - 1);
                cout << "Line deleted successfully.\n";
            }
            else {
                cout << "Invalid line number. Try again.\n";
            }
        }
        else if (choice == "s") {  // Save and Exit
            ofstream outFile(filepath, ios::trunc);
            if (outFile.is_open()) {
                for (const string& note : notes) {
                    outFile << note << endl;
                }
                outFile.close();
                cout << "Notes saved successfully to " << filepath << endl;
            }
            else {
                cerr << "Error opening file for writing.\n\n";
            }
            break;
        }
        else if (choice == "x") {  // Exit without saving
            cout << "!! WARNING !! Changes were not saved.\n\n";
            break;
        }
        else {
            cout << "Invalid choice. Please try again.\n";
        }
    }
    isUserBusy = false;
}

// Main function to handle notepad operations
string notepadFunction() {
    isUserBusy = true;
    clearTerminal();
    cout << endl << "Welcome to the terminal notepad!\n"
        << "Start typing your notes. Type 'EXIT' on a new line to finish.\n\n";

    vector<string> notes;
    string line;

    // Input phase
    while (true) {
        getline(cin, line);
        if (toLower(line) == "exit") break;
        notes.push_back(line);
    }

    // Editing phase
    while (true) {
        clearTerminal();
        cout << "\nCurrent Notes:\n";
        for (size_t i = 0; i < notes.size(); ++i) {
            cout << i + 1 << ": " << notes[i] << endl;
        }

        cout << "\nOptions:\n"
            << "[c] Continue write\n"
            << "[a] Add a new line\n"
            << "[e] Edit a line\n"
            << "[d] Delete a line\n"
            << "[s] Save and Exit\n"
            << "[x] Exit without saving\n"
            << "Enter your choice: ";

        string choice;
        getline(cin, choice);
        choice = toLower(choice);

        if (choice == "c") {  // Continue writing
            clearTerminal();
            cout << "\n>> Continue below. Type 'EXIT' on a new line to stop.\n\n";
            for (const auto& existingLine : notes) {
                cout << existingLine << endl;
            }

            while (true) {
                getline(cin, line);
                if (toLower(line) == "exit") break;
                notes.push_back(line);
            }
        }
        else if (choice == "e") {  // Edit a line
            cout << "Enter the line number to edit: ";
            size_t lineNumber;
            cin >> lineNumber;
            cin.ignore();

            if (lineNumber > 0 && lineNumber <= notes.size()) {
                cout << "Current content: " << notes[lineNumber - 1] << endl;
                cout << "Enter new content: ";
                getline(cin, notes[lineNumber - 1]);
            }
            else {
                cout << "Invalid line number. Try again.\n";
            }
        }
        else if (choice == "a") {  // Add a new line
            cout << "Enter the new line content: ";
            string newLine;
            getline(cin, newLine);
            notes.push_back(newLine);
        }
        else if (choice == "d") {  // Delete a line
            cout << "Enter the line number to delete: ";
            size_t lineNumber;
            cin >> lineNumber;
            cin.ignore();

            if (lineNumber > 0 && lineNumber <= notes.size()) {
                notes.erase(notes.begin() + lineNumber - 1);
                cout << "Line deleted successfully.\n";
            }
            else {
                cout << "Invalid line number. Try again.\n";
            }
        }
        else if (choice == "s") {  // Save and Exit
            cout << "Enter the filename to save: ";
            string filename;
            getline(cin, filename);

            string folderPath = "./Note/";
            fs::create_directory(folderPath);

            string filepath = folderPath + filename + ".txt";
            ofstream outFile(filepath);
            if (outFile.is_open()) {
                for (const string& note : notes) {
                    outFile << note << endl;
                }
                outFile.close();
                cout << "Notes saved successfully to " << filepath << endl;
            }
            else {
                cerr << "Error opening file for writing.\n\n";
            }
            break;
        }
        else if (choice == "x") {  // Exit without saving
            cout << "!! WARNING !! Notes were not saved.\n\n";
            break;
        }
        else {
            cout << "Invalid choice. Please try again.\n";
        }
    }

    isUserBusy = false;
    return "Notepad operation completed.\n";
}

// Function to handle file operations in a loop
string fileOperationsLoop(const string& folderPath) {
    isUserBusy = true;

    // Check if directory exists
    if (!fs::exists(folderPath) || !fs::is_directory(folderPath)) {
        cerr << "Directory does not exist: " << folderPath << endl;
        return "Directory does not exist.\n";
    }

    vector<string> txtFiles;

    // Populate the vector with .txt files
    for (const auto& entry : fs::directory_iterator(folderPath)) {
        if (entry.is_regular_file() && entry.path().extension() == ".txt") {
            txtFiles.push_back(entry.path().filename().string());
        }
    }

    // Check if there are no text files
    if (txtFiles.empty()) {
        cout << "No text files found in the directory.\n";
        return "No text files found.\n";
    }

    const int pageSize = 10; // Number of files per page
    int currentPage = 0;
    bool isRunning = true;

    while (isRunning) {
        isUserBusy = true;
        clearTerminal();

        // Calculate page bounds
        int totalFiles = txtFiles.size();
        int totalPages = (totalFiles + pageSize - 1) / pageSize; // Round up for total pages
        int startIdx = currentPage * pageSize;
        int endIdx = min(startIdx + pageSize, totalFiles);

        // Display files for the current page
        cout << "\nAvailable text files (Page " << currentPage + 1 << " of " << totalPages << "):\n";
        for (int i = startIdx; i < endIdx; ++i) {
            cout << i + 1 << ". " << txtFiles[i] << endl;
        }

        // Display navigation options dynamically
        cout << "\nOptions:\n";
        if (currentPage > 0) {
            cout << "[<] Previous Page\n";
        }
        if (currentPage < totalPages - 1) {
            cout << "[>] Next Page\n";
        }
        cout << "[#] Jump to Page\n"
            << "[q] Quit\n"
            << "Enter file number to operate on: ";

        string input;
        getline(cin, input);

        // Handle navigation commands
        if (input == "<" && currentPage > 0) {
            --currentPage;
        }
        else if (input == ">" && currentPage < totalPages - 1) {
            ++currentPage;
        }
        else if (input == "#") {
            cout << "Enter page number: ";
            string pageInput;
            getline(cin, pageInput);
            try {
                int targetPage = stoi(pageInput) - 1;
                if (targetPage >= 0 && targetPage < totalPages) {
                    currentPage = targetPage;
                }
                else {
                    cout << "Invalid page number. Please try again.\n";
                    pauseConsole();
                }
            }
            catch (...) {
                cout << "Invalid input format. Enter a number to jump to a page.\n";
                pauseConsole();
            }
        }
        else if (input == "q") {
            isRunning = false;
            cout << "Exiting file operations.\n";
        }
        else {
            // Handle file selection by number
            try {
                int fileIndex = stoi(input) - 1;
                if (fileIndex >= 0 && fileIndex < totalFiles) {
                    string selectedFilename = txtFiles[fileIndex];
                    string filepath = folderPath + "/" + selectedFilename;

                    while (true) {
                        isUserBusy = true;
                        clearTerminal();
                        cout << "\nDo you want to perform any file operations on '" << selectedFilename << "'?\n"
                            << "1. Rename\n"
                            << "2. Display Only\n"
                            << "3. Modify File\n"
                            << "4. Delete\n"
                            << "5. Return to File List\n"
                            << "6. Quit Completely\n"
                            << "\nEnter your choice: ";

                        string choice;
                        getline(cin, choice);
                        choice = toLower(choice);

                        if (choice == "1" || choice == "rename") {
                            cout << renameNotepad(filepath) << endl;
                        }
                        else if (choice == "2" || choice == "view" || choice == "open only" || choice == "display") {
                            displayNotepad(filepath);
                        }
                        else if (choice == "3" || choice == "open and modify" || choice == "continue" || choice == "modify" || choice == "open") {
                            continueWriting(filepath);
                        }
                        else if (choice == "4" || choice == "delete") {
                            deleteFile(filepath);
                            txtFiles.erase(txtFiles.begin() + fileIndex);
                            break;
                        }
                        else if (choice == "5" || choice == "return") {
                            break;
                        }
                        else if (choice == "6" || choice == "quit") {
                            cout << "Exiting all file operations completely.\n";
                            return "Operation quit by user.\n";
                        }
                        else {
                            cout << "Invalid choice. Please try again.\n\n";
                        }
                    }
                }
                else {
                    cout << "Invalid file number. Please try again.\n";
                    pauseConsole();
                }
            }
            catch (...) {
                cout << "Invalid input. Please enter a valid command or file number.\n";
                pauseConsole();
            }
        }
    }

    isUserBusy = false;
    return "File operations completed.\n";
}

//code writing

string buildCode(const string& filepath) {
    string extension = filepath.substr(filepath.find_last_of('.') + 1);
    string outputName = filepath.substr(0, filepath.find_last_of('.')) + ".out";
    string command;

    // Handle different file extensions
    if (extension == "cpp") {
        command = "g++ -o \"" + outputName + "\" \"" + filepath + "\"";
    }
    else if (extension == "c") {
        command = "gcc -o \"" + outputName + "\" \"" + filepath + "\"";
    }
    else if (extension == "cs") {
        command = "mcs -out:\"" + outputName + "\" \"" + filepath + "\"";
    }
    else if (extension == "java") {
        command = "javac \"" + filepath + "\"";
        outputName = filepath.substr(0, filepath.find_last_of('.'));
    }
    else if (extension == "js") {
        // No build step for JavaScript
        return "JavaScript files don't need to be built.";
    }
    else if (extension == "rb") {
        // No build step for Ruby
        return "Ruby files don't need to be built.";
    }
    else if (extension == "go") {
        command = "go build -o \"" + outputName + "\" \"" + filepath + "\"";
    }
    else if (extension == "py") {
        // No build step for Python
        return "Python files don't need to be built.";
    }
    else {
        return "Unsupported file extension. Supported extensions: .cpp, .c, .cs, .java, .js, .rb, .go, .py";
    }

    // Execute the build command
    int result = system(command.c_str());

    if (result == 0) {
        cout << "Build successful! Output: " + outputName << endl;
        cout << "\nPress Enter to continue...";
        cin.ignore();
        return "Build successful! Output: " + outputName;
    }
    else {
        cout << "Build failed. Please check your code.\n";
        cout << "\nPress Enter to continue...";
        cin.ignore();
        return "Build failed. Please check your code.";
    }
}

string buildAndRunCode(const string& filepath) {
    string buildResult = buildCode(filepath);

    if (buildResult.find("Build successful") != string::npos) {
        cout << buildResult << endl;

        string extension = filepath.substr(filepath.find_last_of('.') + 1);
        string command;

        // Run the program based on file extension
        if (extension == "cpp" || extension == "c" || extension == "go") {
            string outputName = filepath.substr(0, filepath.find_last_of('.')) + ".out";
            command = "\"" + outputName + "\"";
        }
        else if (extension == "cs") {
            command = "mono \"" + filepath.substr(0, filepath.find_last_of('.')) + ".exe\"";
        }
        else if (extension == "java") {
            string className = filepath.substr(filepath.find_last_of('/') + 1, filepath.find_last_of('.') - filepath.find_last_of('/') - 1);
            command = "java " + className;
        }
        else if (extension == "py") {
            command = "python3 \"" + filepath + "\"";
        }
        else if (extension == "js") {
            command = "node \"" + filepath + "\"";
        }
        else if (extension == "rb") {
            command = "ruby \"" + filepath + "\"";
        }
        else {
            return "Unsupported file extension.";
        }

        clearTerminal();
        cout << "\nRunning the program...\n\n";
        system(command.c_str());

        cout << "\nPress Enter to continue...";
        cin.ignore();
        return "\n\nProgram executed successfully.";
    }
    else {
        cout << "Build failed. Program not executed.\n";
        cout << "\nPress Enter to continue...";
        cin.ignore();
        return "Build failed. Program not executed.";
    }
}

string runCode(const string& filepath) {
    string extension = filepath.substr(filepath.find_last_of('.') + 1);
    string outputName = filepath.substr(0, filepath.find_last_of('.')) + ".out";

    // Check if the output file exists for compiled languages
    if (extension == "cpp" || extension == "c" || extension == "cs" || extension == "go") {
        if (!fs::exists(outputName)) {
            return "Output file not found. Build the program first.";
        }

        string command = "gnome-terminal -- bash -c \"./" + outputName + " ; echo -e '\\n\\nPress Enter to continue...'; read\"";
        int result = system(command.c_str());
        if (result == 0) {
            return "\n\nProgram executed successfully in a new terminal.";
        }
        else {
            return "Failed to execute program.";
        }
    }
    else if (extension == "java") {
        string className = filepath.substr(filepath.find_last_of('/') + 1, filepath.find_last_of('.') - filepath.find_last_of('/') - 1);
        string command = "java " + className;
        system(command.c_str());
        return "\n\nJava program executed successfully.";
    }
    else if (extension == "py") {
        string command = "python3 \"" + filepath + "\"";
        int result = system(command.c_str());
        return result == 0 ? "\n\nPython program executed successfully." : "Failed to execute Python program.";
    }
    else if (extension == "js") {
        string command = "node \"" + filepath + "\"";
        int result = system(command.c_str());
        return result == 0 ? "\n\nJavaScript program executed successfully." : "Failed to execute JavaScript program.";
    }
    else if (extension == "rb") {
        string command = "ruby \"" + filepath + "\"";
        int result = system(command.c_str());
        return result == 0 ? "\n\nRuby program executed successfully." : "Failed to execute Ruby program.";
    }
    else {
        return "Unsupported file extension.";
    }
}

string renameCodeFile(const string& oldFilepath) {
    isUserBusy = true;
    cout << endl;
    cout << "Enter the new filename (without extension): ";
    string newFilename;
    getline(cin, newFilename);

    string extension = oldFilepath.substr(oldFilepath.find_last_of('.'));
    string folderPath = oldFilepath.substr(0, oldFilepath.find_last_of('/') + 1);
    string newFilepath = folderPath + newFilename + extension;

    try {
        fs::rename(oldFilepath, newFilepath);
        return "File renamed successfully to " + newFilepath;
    }
    catch (const fs::filesystem_error& e) {
        return "Error renaming file: " + string(e.what());
    }
    isUserBusy = false;
    return "Rename operation finished.";
}

string processSelectedFile(const string& filepath) {
    // Placeholder for file processing logic
    cout << "Processing file: " << filepath << endl;
    // Add your file operations (rename, build, run, etc.)
    return "File processed successfully.\n";
}

void editAndWriteCode(const string& filepath) {
    vector<string> codeLines;

    // Read current file content into codeLines
    ifstream inFile(filepath);
    if (inFile.is_open()) {
        string line;
        while (getline(inFile, line)) {
            codeLines.push_back(line);
        }
        inFile.close();
    }
    else {
        cout << "Error reading file. Unable to update code.\n";
        return;
    }

    while (true) {
        clearTerminal();
        cout << "\nCurrent content of '" << filepath << "':\n\n";
        for (size_t i = 0; i < codeLines.size(); ++i) {
            cout << i + 1 << ". " << codeLines[i] << endl;  // Display line numbers
        }

        cout << "\n\nDo you want to perform any operations on the code?\n"
            << "[r] Rename file\n"
            << "[s] Open file location\n"
            << "[c] Continue writing\n"
            << "[a] Add Content\n"
            << "[d] Delete a line\n"
            << "[o] Overwrite a line\n"
            << "[f] Finish\n"
            << "Enter your choice: ";
        string updateChoice;
        getline(cin, updateChoice);
        updateChoice = toLower(updateChoice);

        if (updateChoice == "r") {
            // Rename file
            cout << "Enter new filename (including extension): ";
            string newFilename;
            getline(cin, newFilename);
            string folderPath = filepath.substr(0, filepath.find_last_of('/') + 1);
            string newFilepath = folderPath + newFilename;
            if (rename(filepath.c_str(), newFilepath.c_str()) == 0) {
                cout << "File renamed to " << newFilename << endl;
                return;  // Exit after renaming
            }
            else {
                cout << "Failed to rename file.\n";
            }
        }
        else if (updateChoice == "s") {
            // Open file location
            string folderPath = filepath.substr(0, filepath.find_last_of('/') + 1);
            string openCommand = "xdg-open " + folderPath;  // Linux specific
            system(openCommand.c_str());
            cout << "Opening file location...\n";
        }
        else if (updateChoice == "c") {
            // Continue writing
            cout << "\nContinue writing your code. Type 'END' or 'EXIT' on a blank line to finish.\n";
            while (true) {
                string line;
                getline(cin, line);
                if (line == "END" || line == "EXIT") break;
                codeLines.push_back(line);
            }
        }
        else if (updateChoice == "a") {
            // Add Content
            cout << "Choose an option:\n"
                << "1. Add a new line at the end.\n"
                << "2. Add a new line in between.\n"
                << "Enter your choice: ";
            string addOption;
            getline(cin, addOption);

            if (addOption == "1") {
                cout << "Enter the content to add: ";
                string newLine;
                getline(cin, newLine);
                codeLines.push_back(newLine);
            }
            else if (addOption == "2") {
                cout << "Enter the line number to add below: ";
                int lineNumber;
                cin >> lineNumber;
                cin.ignore();  // Clear the newline from input buffer

                if (lineNumber < 1 || lineNumber > codeLines.size()) {
                    cout << "Invalid line number. Operation canceled.\n";
                }
                else {
                    cout << "Choose an option:\n"
                        << "1. Add a single line.\n"
                        << "2. Add multiple lines (type 'END' or 'EXIT' to finish).\n"
                        << "Enter your choice: ";
                    string inBetweenOption;
                    getline(cin, inBetweenOption);

                    if (inBetweenOption == "1") {
                        cout << "Enter the content to add: ";
                        string newLine;
                        getline(cin, newLine);
                        codeLines.insert(codeLines.begin() + lineNumber, newLine);
                    }
                    else if (inBetweenOption == "2") {
                        cout << "Enter lines to add (type 'END' or 'EXIT' to finish):\n";
                        vector<string> newLines;
                        while (true) {
                            string newLine;
                            getline(cin, newLine);
                            if (newLine == "END" || newLine == "EXIT") break;
                            newLines.push_back(newLine);
                        }
                        codeLines.insert(codeLines.begin() + lineNumber, newLines.begin(), newLines.end());
                    }
                    else {
                        cout << "Invalid option.\n";
                    }
                }
            }
            else {
                cout << "Invalid choice.\n";
            }
        }
        else if (updateChoice == "d") {
            // Delete a line
            cout << "Enter the line number to delete: ";
            size_t lineNumber;
            cin >> lineNumber;
            cin.ignore();

            if (lineNumber > 0 && lineNumber <= codeLines.size()) {
                codeLines.erase(codeLines.begin() + lineNumber - 1);
                cout << "Line deleted.\n";
            }
            else {
                cout << "Invalid line number.\n";
            }
        }
        else if (updateChoice == "o") {
            // Overwrite a line
            cout << "Enter the line number to overwrite: ";
            size_t lineNumber;
            cin >> lineNumber;
            cin.ignore();

            if (lineNumber > 0 && lineNumber <= codeLines.size()) {
                cout << "Enter the new content: ";
                string newContent;
                getline(cin, newContent);
                codeLines[lineNumber - 1] = newContent;
            }
            else {
                cout << "Invalid line number.\n";
            }
        }
        else if (updateChoice == "f") {
            // Finish
            cout << "Exiting update mode.\n";
            break;
        }
        else {
            cout << "Invalid choice. Please try again.\n";
        }

        // Save updated content to file
        ofstream outFile(filepath, ios::trunc);
        if (outFile.is_open()) {
            for (const auto& codeLine : codeLines) {
                outFile << codeLine << endl;
            }
            outFile.close();
        }
        else {
            cout << "Error saving file.\n";
        }
    }
}

string editCode(const string& folderPath) {
    isUserBusy = true;

    // Ensure folderPath ends with the appropriate separator
    string correctedFolderPath = folderPath;
    if (correctedFolderPath.back() != '/') {
        correctedFolderPath += '/'; // Use Linux-style separator
    }

    // Check if directory exists
    if (!fs::exists(correctedFolderPath) || !fs::is_directory(correctedFolderPath)) {
        cerr << "Directory does not exist: " << correctedFolderPath << endl;
        return "Directory does not exist.\n";
    }

    vector<string> codeFiles;

    // Populate the vector with all files in the directory
    for (const auto& entry : fs::directory_iterator(correctedFolderPath)) {
        if (entry.is_regular_file()) {
            codeFiles.push_back(entry.path().filename().string());
        }
    }

    // Check if there are no files
    if (codeFiles.empty()) {
        cout << "No files found in the directory.\n";
        return "No files found.\n";
    }

    const int pageSize = 15; // Number of files per page
    int currentPage = 0;
    bool isRunning = true;

    while (isRunning) {
        clearTerminal();

        // Calculate page bounds
        int totalFiles = codeFiles.size();
        int totalPages = (totalFiles + pageSize - 1) / pageSize; // Round up for total pages
        int startIdx = currentPage * pageSize;
        int endIdx = min(startIdx + pageSize, totalFiles);

        // Display files for the current page
        cout << "\nAvailable code files (Page " << currentPage + 1 << " of " << totalPages << "):\n";
        for (int i = startIdx; i < endIdx; ++i) {
            cout << "[" << i + 1 << "] " << codeFiles[i] << endl; // Updated to use square brackets
        }

        // Navigation options
        cout << "\nNavigation options:\n";
        if (currentPage > 0) {
            cout << "[<] Previous page\n";
        }
        if (currentPage < totalPages - 1) {
            cout << "[>] Next page\n";
        }
        cout << "[j <page_number>] Jump to page\n"
            << "[q] Quit\n";

        cout << "\nEnter your choice (or select a file number): ";
        string input;
        getline(cin, input);

        if (input == ">") {
            if (currentPage < totalPages - 1) {
                ++currentPage;
            }
            else {
                cout << "You are already on the last page.\n";
                pauseConsole();
            }
        }
        else if (input == "<") {
            if (currentPage > 0) {
                --currentPage;
            }
            else {
                cout << "You are already on the first page.\n";
                pauseConsole();
            }
        }
        else if (input.substr(0, 2) == "j ") {
            try {
                int targetPage = stoi(input.substr(2)) - 1;
                if (targetPage >= 0 && targetPage < totalPages) {
                    currentPage = targetPage;
                }
                else {
                    cout << "Invalid page number. Please try again.\n";
                    pauseConsole();
                }
            }
            catch (...) {
                cout << "Invalid input format. Use 'j <page_number>' to jump to a page.\n";
                pauseConsole();
            }
        }
        else if (input == "q") {
            isRunning = false;
            cout << "Exiting file selection.\n";
        }
        else {
            try {
                int fileIndex = stoi(input) - 1;
                if (fileIndex >= 0 && fileIndex < totalFiles) {
                    string selectedFilename = codeFiles[fileIndex];
                    string filepath = correctedFolderPath + selectedFilename;

                    // Present options for the selected file
                    clearTerminal();

                    cout << "============================\n"
                        << "File selected: " << selectedFilename << endl
                        << "============================\n"
                        << "Options for the selected file:\n"
                        << "1. View File\n"
                        << "2. Edit File\n"
                        << "3. Build/Run\n"
                        << "4. Delete File\n"
                        << "5. Rename File\n"
                        << "6. Exit\n";

                    cout << "\nEnter your choice: ";
                    string fileOption;
                    getline(cin, fileOption);

                    if (fileOption == "1") {
                        displayNotepad(filepath);
                    }
                    else if (fileOption == "2") {
                        editAndWriteCode(filepath);
                    }
                    else if (fileOption == "3") {
                        clearTerminal();
                        cout << "\nSelect an option for running code:\n"
                            << "1. Build and Run (C, C++, Java)\n"
                            << "2. Run Only (Python, pre-built binaries)\n"
                            << "Your Option: ";

                        string runOption;
                        getline(cin, runOption);

                        if (runOption == "1") {
                            string buildResult = buildAndRunCode(filepath);
                            cout << buildResult << endl;
                            cout << "\n>> Press enter to finish <<\n";
                            cin.get();
                        }
                        else if (runOption == "2") {
                            clearTerminal();
                            string runResult = runCode(filepath);
                            cout << runResult << endl;
                            cout << "\n>> Press enter to finish <<\n";
                            cin.get();
                        }
                        else {
                            cout << "Invalid option. Returning to file menu.\n";
                            pauseConsole();
                        }
                    }
                    else if (fileOption == "4") {
                        deleteFile(filepath);
                        cout << "File deleted successfully.\n";
                        codeFiles.erase(codeFiles.begin() + fileIndex);
                        totalFiles = codeFiles.size();
                        totalPages = (totalFiles + pageSize - 1) / pageSize;
                    }
                    else if (fileOption == "5") {
                        renameCodeFile(filepath);
                    }
                    else if (fileOption == "6") {
                        break;
                    }
                    else {
                        cout << "Invalid choice. Returning to file list.\n";
                        pauseConsole();
                    }
                }
                else {
                    cout << "Invalid file number. Please try again.\n";
                    pauseConsole();
                }
            }
            catch (...) {
                cout << "Invalid input. Please enter a valid command or file number.\n";
                pauseConsole();
            }
        }
    }

    isUserBusy = false;
    return "Code editing operations completed.\n";
}

string writeBuildRunCode() {
    isUserBusy = true;
    string folderPath = "./Programs/";
    fs::create_directory(folderPath);

    cout << "Enter the filename (including extension, e.g., 'example.cpp'): ";
    clearTerminal();
    string filename;
    getline(cin, filename);

    string filepath = folderPath + filename;

    // Detect language based on file extension
    string extension = filepath.substr(filepath.find_last_of('.') + 1);
    string buildCommand, runCommand;
    if (extension == "cpp") {
        buildCommand = "g++ " + filepath + " -o " + folderPath + "program";
        runCommand = folderPath + "program";
    }
    else if (extension == "c") {
        buildCommand = "gcc " + filepath + " -o " + folderPath + "program";
        runCommand = folderPath + "program";
    }
    else if (extension == "cs") {
        buildCommand = "mcs " + filepath + " -out:" + folderPath + "program.exe";
        runCommand = "mono " + folderPath + "program.exe";
    }
    else if (extension == "go") {
        runCommand = "go run " + filepath;
    }
    else if (extension == "js") {
        runCommand = "node " + filepath;
    }
    else if (extension == "py") {
        runCommand = "python3 " + filepath;
    }
    else if (extension == "java") {
        buildCommand = "javac " + filepath;
        runCommand = "java -cp " + folderPath + " " + filename.substr(0, filename.find_last_of('.'));
    }
    else {
        return "Unsupported file extension. Please use a supported language.\n";
    }

    vector<string> codeLines;

    // Writing Code
    cout << "Start writing your code. Type 'END' or 'EXIT' on a blank line to finish.\n\n";
    string line;
    while (true) {
        getline(cin, line);
        if (line == "END" || line == "EXIT") break;
        codeLines.push_back(line);
    }

    // Save Code to File
    ofstream outFile(filepath, ios::trunc);
    if (outFile.is_open()) {
        for (const auto& codeLine : codeLines) {
            outFile << codeLine << endl;
        }
        outFile.close();
        cout << "Code saved to " << filepath << endl;
    }
    else {
        return "Error saving file.\n";
    }

    // Display written content
    clearTerminal();
    cout << "\nYour code content:\n";
    for (const auto& codeLine : codeLines) {
        cout << codeLine << endl;
    }

    // Interaction Menu
    while (true) {
        clearTerminal();
        // Display the current file content with line numbers
        cout << "\nCurrent content of '" << filename << "':\n\n";
        for (size_t i = 0; i < codeLines.size(); ++i) {
            cout << codeLines[i] << endl;  // Display line number
        }

        cout << "\n\nOptions:\n"
            << "[b] Build Code (C, C++, Java)\n"
            << "[br] Build and Run (C, C++, Java)\n"
            << "[r] Run Only (Python, pre-built binaries)\n"
            << "[u] Update/Change Code\n"
            << "[f] Finish\n"
            << "Enter your choice: ";
        string choice;
        getline(cin, choice);
        choice = toLower(choice);

        if (choice == "b") {
            string buildResult = buildCode(filepath);
            cout << buildResult << endl;
        }
        else if (choice == "br") {
            clearTerminal();
            string buildAndRunResult = buildAndRunCode(filepath);
            cout << buildAndRunResult << endl;
            cout << "\n>> press enter to finish <<\n";
            cin.get();  // Wait for user input to finish testing
        }
        else if (choice == "r") {
            clearTerminal();
            string runResult = runCode(filepath);
            cout << runResult << endl;
            cout << "\n>> press enter to finish <<\n";
            cin.get();  // Wait for user input to finish testing
        }
        else if (choice == "u") {
            // Update/Change Code (additional operations)
            clearTerminal();
            cout << "\nCurrent content of '" << filename << "':\n\n";
            for (size_t i = 0; i < codeLines.size(); ++i) {
                cout << i + 1 << ". " << codeLines[i] << endl;  // Display line number
            }

            cout << "\n\nDo you want to perform any operations on '" << filename << "'?\n"
                << "[r] Rename file\n"
                << "[s] Open file location\n"
                << "[c] Continue writing\n"
                << "[a] Add Content\n"
                << "[d] Delete a line\n"
                << "[o] Overwrite a line\n"
                << "[f] Finish\n"
                << "Enter your choice: ";
            string updateChoice;
            getline(cin, updateChoice);
            updateChoice = toLower(updateChoice);

            if (updateChoice == "r") {
                // Rename file
                cout << "Enter new filename (including extension): ";
                string newFilename;
                getline(cin, newFilename);
                string newFilepath = folderPath + newFilename;
                if (rename(filepath.c_str(), newFilepath.c_str()) == 0) {
                    cout << "File renamed to " << newFilename << endl;
                    filepath = newFilepath;
                    filename = newFilename;
                }
                else {
                    cout << "Failed to rename file.\n";
                }
            }
            else if (updateChoice == "s") {
                // Open file location
                string openCommand = "xdg-open " + folderPath;  // Linux specific
                system(openCommand.c_str());
                cout << "Opening file location...\n";
            }
            else if (updateChoice == "c") {
                // Continue writing code
                cout << "\nContinue writing your code. Type 'END' or 'EXIT' on a blank line to finish.\n";
                while (true) {
                    getline(cin, line);
                    if (line == "END" || line == "EXIT") break;
                    codeLines.push_back(line);
                }

                // Save the updated code to the file
                ofstream outFile(filepath, ios::trunc);
                if (outFile.is_open()) {
                    for (const auto& codeLine : codeLines) {
                        outFile << codeLine << endl;
                    }
                    outFile.close();
                    cout << "Code updated and saved to " << filepath << endl;
                }
                else {
                    cout << "Error saving updated code.\n";
                }

            }
            else if (updateChoice == "a") {
                // Add a line
                cout << "=======================\n"
                    << "Choose an option:\n"
                    << "1. Add a new line at the end.\n"
                    << "2. Add a new line in between.\n"
                    << "Enter your choice: ";
                string addOption;
                getline(cin, addOption);

                if (addOption == "1") {
                    // Add a new line at the end
                    cout << "Add a line of content: ";
                    string newLine;
                    getline(cin, newLine);
                    codeLines.push_back(newLine);

                    // Save the updated code to the file
                    ofstream outFile(filepath, ios::trunc);
                    if (outFile.is_open()) {
                        for (const auto& codeLine : codeLines) {
                            outFile << codeLine << endl;
                        }
                        outFile.close();
                        cout << "Content added and saved to " << filepath << endl;
                    }
                    else {
                        cout << "Error saving updated code.\n";
                    }

                }
                else if (addOption == "2") {
                    // Add a new line in between
                    cout << "Enter the line number to add below: ";
                    int lineNumber;
                    cin >> lineNumber;
                    cin.ignore(); // Clear newline from the input buffer

                    if (lineNumber < 1 || lineNumber > codeLines.size()) {
                        cout << "Invalid line number. Operation canceled.\n";
                    }
                    else {
                        cout << "=======================\n"
                            << "Choose an option:\n"
                            << "1. Add a single line.\n"
                            << "2. Add multiple lines (type 'END' or 'EXIT' to finish).\n"
                            << "Enter your choice: ";
                        string inBetweenOption;
                        getline(cin, inBetweenOption);

                        if (inBetweenOption == "1") {
                            // Add a single line
                            cout << "=======================\n";
                            cout << "Enter the content to add: ";
                            string newLine;
                            getline(cin, newLine);
                            codeLines.insert(codeLines.begin() + lineNumber, newLine);

                        }
                        else if (inBetweenOption == "2") {
                            // Add multiple lines
                            clearTerminal();
                            cout << "Enter lines to add (type 'END' or 'EXIT' on a blank line to finish):\n\n";
                            vector<string> newLines;
                            while (true) {
                                string newLine;
                                getline(cin, newLine);
                                if (newLine == "END" || newLine == "EXIT") break;
                                newLines.push_back(newLine);
                            }
                            codeLines.insert(codeLines.begin() + lineNumber, newLines.begin(), newLines.end());
                        }
                        else {
                            cout << "Invalid option. Operation canceled.\n";
                        }

                        // Save the updated code to the file
                        ofstream outFile(filepath, ios::trunc);
                        if (outFile.is_open()) {
                            for (const auto& codeLine : codeLines) {
                                outFile << codeLine << endl;
                            }
                            outFile.close();
                            cout << "Lines added and saved to " << filepath << endl;
                        }
                        else {
                            cout << "Error saving updated code.\n";
                        }
                    }
                }
                else {
                    cout << "Invalid choice. Operation canceled.\n";
                }

            }
            else if (updateChoice == "d") {
                // Delete a line
                cout << "Enter the line number to delete: ";
                size_t lineNumber;
                cin >> lineNumber;
                cin.ignore();
                if (lineNumber > 0 && lineNumber <= codeLines.size()) {
                    codeLines.erase(codeLines.begin() + lineNumber - 1);
                    // Save the updated code to the file
                    ofstream outFile(filepath, ios::trunc);
                    if (outFile.is_open()) {
                        for (const auto& codeLine : codeLines) {
                            outFile << codeLine << endl;
                        }
                        outFile.close();
                        cout << "Line deleted and saved to " << filepath << endl;
                    }
                    else {
                        cout << "Error saving updated code.\n";
                    }
                }
                else {
                    cout << "Invalid line number.\n";
                }

            }
            else if (updateChoice == "o") {
                // Overwrite a line
                cout << "Enter the line number to overwrite: ";
                size_t lineNumber;
                cin >> lineNumber;
                cin.ignore();
                if (lineNumber > 0 && lineNumber <= codeLines.size()) {
                    cout << "Enter the new content: ";
                    string newContent;
                    getline(cin, newContent);
                    codeLines[lineNumber - 1] = newContent;

                    // Save the updated code to the file
                    ofstream outFile(filepath, ios::trunc);
                    if (outFile.is_open()) {
                        for (const auto& codeLine : codeLines) {
                            outFile << codeLine << endl;
                        }
                        outFile.close();
                        cout << "Line overwritten and saved to " << filepath << endl;
                    }
                    else {
                        cout << "Error saving updated code.\n";
                    }
                }
                else {
                    cout << "Invalid line number.\n";
                }

            }
            else if (updateChoice == "f") {
                // Finish editing and return to the main options
                break;
            }
            else {
                cout << "Invalid choice. Please try again.\n";
            }
        }
        else if (choice == "f") {
            cout << "Exiting task.\n";
            break;
        }
        else {
            cout << "Invalid choice. Please try again.\n";
        }
    }

    isUserBusy = false;
    return "Code task completed.\n";
}

//drawCalendar
vector<string> months = {
    "January", "February", "March", "April", "May", "June",
    "July", "August", "September", "October", "November", "December"
};

void drawCalendar(const string& monthName, int year, int currentDay = -1) {
    isUserBusy = true;
    clearTerminal();
    cout << "\n\t" << monthName << " " << year << endl;
    cout << " Su  Mo  Tu  We  Th  Fr  Sa  " << endl;

    // Calculate the first day of the month
    tm firstDay = {};
    firstDay.tm_year = year - 1900;
    firstDay.tm_mon = find(begin(months), end(months), monthName) - begin(months);
    firstDay.tm_mday = 1;
    mktime(&firstDay);

    int startDay = firstDay.tm_wday;
    int daysInMonth = (firstDay.tm_mon == 1) ? (year % 4 == 0 && (year % 100 != 0 || year % 400 == 0) ? 29 : 28) :
        (firstDay.tm_mon == 3 || firstDay.tm_mon == 5 || firstDay.tm_mon == 8 || firstDay.tm_mon == 10) ? 30 : 31;

    for (int i = 0; i < startDay; ++i) {
        cout << "    ";
    }

    for (int day = 1; day <= daysInMonth; ++day) {
        if (day == currentDay) {
            cout << "[" << (day < 10 ? " " : "") << day << "]"; // Highlight the current day with brackets
        }
        else {
            cout << " " << setw(2) << day << " ";
        }

        if ((startDay + day) % 7 == 0) {
            cout << endl;
        }
    }
    cout << endl;
    isUserBusy = false;
}

void drawYearCalendar(int year) {
    isUserBusy = true;
    clearTerminal();

    // Get today's date
    time_t t = time(nullptr);
    tm* today = localtime(&t);
    int currentDay = today->tm_mday;
    int currentMonth = today->tm_mon;
    int currentYear = today->tm_year + 1900;

    cout << "\nDisplaying calendar for the year: " << year << "\n\n";

    const int monthsPerRow = 3;
    const int totalRows = 12 / monthsPerRow;
    const int maxWeeks = 6;  // Maximum weeks for alignment
    const int blockWidth = 30;  // Adjusted width for each month block

    for (int row = 0; row < totalRows; ++row) {
        // Print Month Names Centered with Dividers
        for (int col = 0; col < monthsPerRow; ++col) {
            int monthIndex = row * monthsPerRow + col;
            // Center the month name
            string monthHeader = months[monthIndex];
            int padding = (blockWidth - monthHeader.size()) / 2; // Calculate padding for centering
            cout << string(padding, ' ') << monthHeader << string(blockWidth - padding - monthHeader.size(), ' ') << "|";
        }
        cout << endl;

        // Print the weekday headers for each column
        for (int col = 0; col < monthsPerRow; ++col) {
            cout << " Su  Mo  Tu  We  Th  Fr  Sa   |";
        }
        cout << endl;

        // Prepare to draw the calendar rows for each month
        vector<vector<string>> monthDays(monthsPerRow);

        for (int col = 0; col < monthsPerRow; ++col) {
            int monthIndex = row * monthsPerRow + col;

            tm firstDay = {};
            firstDay.tm_year = year - 1900;
            firstDay.tm_mon = monthIndex;
            firstDay.tm_mday = 1;
            mktime(&firstDay);

            int startDay = firstDay.tm_wday;
            int daysInMonth = (monthIndex == 1) ?
                (year % 4 == 0 && (year % 100 != 0 || year % 400 == 0) ? 29 : 28) :
                (monthIndex == 3 || monthIndex == 5 || monthIndex == 8 || monthIndex == 10) ? 30 : 31;

            // Prepare calendar content weeks
            vector<string> weeks(maxWeeks, string(blockWidth - 2, ' ')); // Initialize weeks with spaces
            int currentWeek = 0;

            for (int day = 1; day <= daysInMonth; ++day) {
                int position = (startDay + day - 1) % 7 * 4; // Calculate the correct column

                if (year == currentYear && monthIndex == currentMonth && day == currentDay) {
                    // Highlight today's date with brackets, ensuring width of 4 spaces
                    string highlightedDay = "[" + string(day < 10 ? " " : "") + to_string(day) + "]";
                    weeks[currentWeek].replace(position, 4, highlightedDay); // Replace with consistent width
                }
                else {
                    // Regular day alignment with a width of 4 spaces
                    string regularDay = (day < 10 ? "  " : " ") + to_string(day) + " ";
                    weeks[currentWeek].replace(position, 4, regularDay); // Replace with consistent width
                }

                if ((startDay + day) % 7 == 0 || day == daysInMonth) {
                    currentWeek++;
                }
            }


            monthDays[col] = weeks;
        }

        // Print rows of the month data
        for (int week = 0; week < maxWeeks; ++week) {
            for (int col = 0; col < monthsPerRow; ++col) {
                cout << " " << monthDays[col][week] << " |";
            }
            cout << endl;
        }

        // Add row spacing to separate different calendar rows
        cout << endl;
    }
    isUserBusy = false;
}

void drawSpecificCalendar(const string& dateInput) {
    isUserBusy = true;
    clearTerminal();

    if (dateInput.length() == 4 && all_of(dateInput.begin(), dateInput.end(), ::isdigit)) {
        int year = stoi(dateInput);
        drawYearCalendar(year);
        cout << "\nCalendar for the year " << year << " displayed.\n";
        isUserBusy = false;
        return;
    }

    int month, year;
    stringstream ss(dateInput);
    ss >> month >> year;

    if (ss.fail() || month < 1 || month > 12) {
        cout << "Invalid input format. (e.g., 'cal 12 2024') or a 4-digit year (e.g., 'cal 2024').\n";
        isUserBusy = false;
        return;
    }

    string monthName = months[month - 1];
    drawCalendar(monthName, year);
    cout << "\nCalendar for " << monthName << ", " << year << " displayed.\n";
    isUserBusy = false;
}
bool isValidMonthYearInput(const string& dateInput) {
    size_t spacePos = dateInput.find(' ');
    if (spacePos != string::npos) {
        string monthStr = dateInput.substr(0, spacePos);
        string yearStr = dateInput.substr(spacePos + 1);

        try {
            int month = stoi(monthStr);
            int year = stoi(yearStr);

            if (month >= 1 && month <= 12 && year > 0) {
                return true; // Valid month/year format
            }
        }
        catch (const exception& e) {
            // In case of invalid number conversion
            return false;
        }
    }
    return false; // Invalid format if no space or incorrect month/year
}

string getTimeConvert() {
    isUserBusy = true;
    clearTerminal();

    string choiceStr;
    int choice = 0, years = 0, months = 0, weeks = 0, days = 0;
    string result;

    cout << "Time Conversion Options:" << endl;
    cout << "1. Convert by days" << endl;
    cout << "2. Convert by months" << endl;
    cout << "3. Convert by years" << endl;

    cout << "Select an option (1-3): ";
    getline(cin, choiceStr);

    try {
        choice = stoi(choiceStr);
    }
    catch (invalid_argument& e) {
        result = "Invalid input. Please enter a number between 1 and 3.";
        return result;
    }

    switch (choice) {
    case 1:
        cout << "Enter number of days: ";
        cin >> days;

        years = days / 365;
        months = days / 30;
        weeks = days / 7;

        result = "Years: " + to_string(years) + "\n" +
            "Months: " + to_string(months) + "\n" +
            "Weeks: " + to_string(weeks) + "\n" +
            "Days: " + to_string(days);
        break;
    case 2:
        cout << "Enter number of months: ";
        cin >> months;

        years = months / 12;
        weeks = static_cast<int>(months * 4.33);
        days = static_cast<int>(months * 30.41);

        result = "Years: " + to_string(years) + "\n" +
            "Months: " + to_string(months) + "\n" +
            "Weeks: " + to_string(weeks) + "\n" +
            "Days: " + to_string(days);
        break;

    case 3:
        cout << "Enter number of years: ";
        cin >> years;

        months = years * 12;
        weeks = years * 52;
        days = years * 365;

        result = "Years: " + to_string(years) + "\n" +
            "Months: " + to_string(months) + "\n" +
            "Weeks: " + to_string(weeks) + "\n" +
            "Days: " + to_string(days);
        break;

    default:
        result = "Invalid selection. Please choose a valid option (1-3).";
    }

    // Clear input buffer to handle any leftover newline characters
    cin.ignore((std::numeric_limits<std::streamsize>::max)(), '\n');

    isUserBusy = false;
    return result;
}

//calculator
// Helper function to check if a character is an operator
bool isOperator(char c) {
    return c == '+' || c == '-' || c == '*' || c == '/' || c == '%' || c == '^' || c == 'r' || c == 'x' || c == 'X';
}

// Utility function to remove all spaces from a string
string removeSpaces(const string& str) {
    string result;
    for (char c : str) {
        if (!isspace(c)) {
            result += c;
        }
    }
    return result;
}

// Utility function to trim spaces from both ends of a string
string trim(const string& str) {
    size_t first = str.find_first_not_of(' ');
    if (first == string::npos) return "";
    size_t last = str.find_last_not_of(' ');
    return str.substr(first, last - first + 1);
}

double nthRoot(double n, double x) {
    if (n == 0) {
        cerr << "Error: Root with degree 0 is invalid." << endl;
        return NAN;
    }

    if (n < 0) {
        return 1.0 / nthRoot(-n, x);
    }

    if (x < 0 && static_cast<int>(n) % 2 == 0) {
        return -pow(-x, 1.0 / n);
    }

    if (x < 0 && static_cast<int>(n) % 2 == 1) {
        return -pow(-x, 1.0 / n);
    }

    return pow(x, 1.0 / n);
}

// Function to apply an operator to two values and push the result
double applyOperator(double a, double b, char op) {
    switch (op) {
    case '+': return a + b;
    case '-': return a - b;
    case '*': return a * b;
    case 'x':
    case 'X': return a * b;  // Treat 'x' and 'X' as multiplication
    case '/': return (b != 0) ? a / b : 0;  // Avoid division by zero
    case '%': return fmod(a, b);
    case '^': return pow(a, b);  // Power operator
    case 'r': return nthRoot(a, b);  // Root operator: b^(1/a)
    default: return 0;
    }
}

// Function to get precedence of operators
int precedence(char op) {
    if (op == '+' || op == '-') return 1;
    if (op == '*' || op == '/' || op == '%') return 2;
    if (op == '^' || op == 'r') return 3;
    return 0;
}

// Function to tokenize the input expression
vector<string> tokenize(const string& expression) {
    vector<string> tokens;
    string token;
    for (size_t i = 0; i < expression.length(); ++i) {
        char c = expression[i];

        if (isdigit(c) || c == '.' || (c == '-' && (i == 0 || expression[i - 1] == '('))) {
            token += c;
        }
        else if (c == 'p' && i + 1 < expression.size() && expression.substr(i, 2) == "pi") {
            tokens.push_back("pi");
            i++;
        }
        else if (isOperator(c) || c == '(' || c == ')') {
            if (!token.empty()) {
                tokens.push_back(token);
                token.clear();
            }
            tokens.push_back(string(1, c));
        }
        else if (c == 'r') {
            if (!token.empty()) {
                tokens.push_back(token);
                token.clear();
            }
            tokens.push_back("r");
        }
    }
    if (!token.empty()) {
        tokens.push_back(token);
    }
    return tokens;
}


// Function to replace constants like pi
double parseConstant(const string& token) {
    if (token == "pi") return M_PI;
    return stod(token);
}

// Main function to evaluate an expression
string evaluateExpression(const string& expression) {
    vector<string> tokens = tokenize(removeSpaces(expression));
    stack<double> values;
    stack<char> ops;

    for (const string& token : tokens) {
        if (isdigit(token[0]) || (token[0] == '-' && token.size() > 1) || token == "pi") {
            values.push(parseConstant(token));
        }
        else if (token == "(") {
            ops.push('(');
        }
        else if (token == ")") {
            while (!ops.empty() && ops.top() != '(') {
                double val2 = values.top(); values.pop();
                double val1 = values.top(); values.pop();
                char op = ops.top(); ops.pop();
                values.push(applyOperator(val1, val2, op));
            }
            ops.pop();
        }
        else if (isOperator(token[0])) {
            char op = token[0];
            while (!ops.empty() && precedence(ops.top()) >= precedence(op)) {
                double val2 = values.top(); values.pop();
                double val1 = values.top(); values.pop();
                char op = ops.top(); ops.pop();
                values.push(applyOperator(val1, val2, op));
            }
            ops.push(op);
        }
    }

    while (!ops.empty()) {
        double val2 = values.top(); values.pop();
        double val1 = values.top(); values.pop();
        char op = ops.top(); ops.pop();
        values.push(applyOperator(val1, val2, op));
    }

    if (values.size() != 1) return "Error: Invalid Expression";

    stringstream result;
    result << fixed << setprecision(15) << values.top();
    string resultStr = result.str();
    resultStr.erase(resultStr.find_last_not_of('0') + 1, string::npos);
    if (resultStr.back() == '.') resultStr.pop_back();
    return resultStr;
}

bool isMathExpression(const string& userMessage) {
    // Remove spaces for validation purposes
    string cleanedMessage = removeSpaces(userMessage);

    // Regular expression to match valid mathematical characters
    regex mathPattern("^[0-9\\.\\s\\(\\)\\+\\-\\*/%xX\\^rpi]+$");
    return regex_match(cleanedMessage, mathPattern);
}

//Game
    //Snake
enum Direction { UP, DOWN, LEFT, RIGHT };

int getKeyPress() {
    struct termios oldt, newt;
    int ch;
    tcgetattr(STDIN_FILENO, &oldt);
    newt = oldt;
    newt.c_lflag &= ~(ICANON | ECHO);
    tcsetattr(STDIN_FILENO, TCSANOW, &newt);
    ch = getchar();
    tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
    return ch;
}

void snakeGame() {
    int width = 27;  // Expanded by 35%
    int height = 14; // Expanded by 35%
    int x = width / 2, y = height / 2;
    int fruitX = rand() % width, fruitY = rand() % height;
    int score = 0;
    Direction dir = RIGHT;

    bool gameOver = false;
    int tailX[1000], tailY[1000];
    int nTail = 0;

    // Non-blocking input settings
    struct termios oldt, newt;
    tcgetattr(STDIN_FILENO, &oldt);
    newt = oldt;
    newt.c_lflag &= ~(ICANON | ECHO);
    tcsetattr(STDIN_FILENO, TCSANOW, &newt);
    fd_set readfds;
    struct timeval timeout;

    while (!gameOver) {
        clearTerminal(); // Clear screen for Linux

        // Draw the game board
        for (int i = 0; i < width + 2; i++) cout << "#";
        cout << endl;

        for (int i = 0; i < height; i++) {
            for (int j = 0; j < width; j++) {
                if (j == 0) cout << "#"; // Draw left wall
                if (i == y && j == x) cout << "O"; // Draw snake head
                else if (i == fruitY && j == fruitX) cout << "*"; // Draw fruit
                else {
                    bool print = false;
                    for (int k = 0; k < nTail; k++) {
                        if (tailX[k] == j && tailY[k] == i) {
                            cout << "o"; // Draw snake tail
                            print = true;
                        }
                    }
                    if (!print) cout << " ";
                }

                if (j == width - 1) cout << "#"; // Draw right wall
            }
            cout << endl;
        }

        for (int i = 0; i < width + 2; i++) cout << "#"; // Draw bottom wall
        cout << endl;

        // Display score
        cout << "Score: " << score << endl;

        // Input for direction (non-blocking)
        FD_ZERO(&readfds);
        FD_SET(STDIN_FILENO, &readfds);
        timeout.tv_sec = 0;
        timeout.tv_usec = 100000; // 100ms timeout

        if (select(STDIN_FILENO + 1, &readfds, NULL, NULL, &timeout)) {
            int ch = getchar();
            switch (ch) {
            case 'w': if (dir != DOWN) dir = UP; break;
            case 's': if (dir != UP) dir = DOWN; break;
            case 'a': if (dir != RIGHT) dir = LEFT; break;
            case 'd': if (dir != LEFT) dir = RIGHT; break;
            }
        }

        // Move the snake
        int prevX = tailX[0], prevY = tailY[0];
        int prev2X, prev2Y;
        tailX[0] = x;
        tailY[0] = y;

        for (int i = 1; i < nTail; i++) {
            prev2X = tailX[i];
            prev2Y = tailY[i];
            tailX[i] = prevX;
            tailY[i] = prevY;
            prevX = prev2X;
            prevY = prev2Y;
        }

        switch (dir) {
        case UP: y--; break;
        case DOWN: y++; break;
        case LEFT: x--; break;
        case RIGHT: x++; break;
        }

        // Check for game over conditions
        if (x >= width || x < 0 || y >= height || y < 0) gameOver = true;
        for (int i = 0; i < nTail; i++) {
            if (tailX[i] == x && tailY[i] == y) gameOver = true;
        }

        // Check if the snake eats the fruit
        if (x == fruitX && y == fruitY) {
            score += 10;
            fruitX = rand() % width;
            fruitY = rand() % height;
            nTail++;
        }
    }

    // Restore terminal settings
    tcsetattr(STDIN_FILENO, TCSANOW, &oldt);

    cout << "Game Over! Final Score: " << score << endl;
    cout << "Press Enter to return to the menu...";
    cin.ignore();
    cin.get();
}


//Flip coin
string flipCoin() {
    isUserBusy = true;
    sleepForMilliseconds(250);
    srand(static_cast<unsigned int>(time(nullptr)));
    return (rand() % 2 == 0) ? "Heads" : "Tails";
    isUserBusy = false;
}

//rock paper scissors
void rockPaperScissors(const string& userName, const string& botName) {
    bool isRunning = true;
    isUserBusy = true;

    while (isRunning) {
        clearTerminal(); // Clear the terminal at the start of each menu display
        cout << "\nRock, Paper, Scissors Menu:\n";
        cout << "1. Single round\n";
        cout << "2. Best of three (3 rounds)\n";
        cout << "3. Custom number of rounds\n";
        cout << "4. Exit\n";
        cout << "Enter your choice: ";
        int choice;
        cin >> choice;

        if (choice == 4) {
            cout << "Exiting Rock, Paper, Scissors." << endl;
            break;
        }

        int totalRounds = 0;
        if (choice == 3) {
            cout << "Enter number of rounds to play (or 'q' to quit): ";
            string input;
            cin >> input;
            if (input == "q" || input == "Q") {
                cout << "Returning to menu." << endl;
                continue;
            }
            try {
                totalRounds = stoi(input);
            }
            catch (const invalid_argument&) {
                cout << "Invalid number of rounds. Returning to menu." << endl;
                continue;
            }
        }
        else if (choice == 1) {
            totalRounds = 1;
        }
        else if (choice == 2) {
            totalRounds = 3;
        }
        else {
            cout << "Invalid choice. Please try again." << endl;
            continue;
        }

        int userScore = 0, botScore = 0;
        srand(static_cast<unsigned int>(time(nullptr)));

        for (int round = 1; round <= totalRounds; ++round) {
            clearTerminal(); // Clear the terminal before each round
            cout << "\nRound " << round << " of " << totalRounds << ":\n";
            cout << "Choose your move: [1] Rock, [2] Paper, [3] Scissors: ";
            int userMove;
            cin >> userMove;

            if (choice == 3 && (userMove == 'q' || userMove == 'Q')) {
                cout << "Returning to menu." << endl;
                break;
            }

            if (userMove < 1 || userMove > 3) {
                cout << "Invalid move. Please select a valid option (1, 2, or 3)." << endl;
                --round;
                continue;
            }

            int botMove = (rand() % 3) + 1;
            string moves[] = { "Rock", "Paper", "Scissors" };

            cout << "Press Enter to see the result...";
            cin.ignore((std::numeric_limits<std::streamsize>::max)(), '\n');
            cin.get();

            clearTerminal(); // Clear the terminal before showing the result
            cout << userName << " chose: " << moves[userMove - 1] << endl;
            cout << botName << " chose: " << moves[botMove - 1] << endl;

            if (userMove == botMove) {
                cout << "It's a tie!" << endl;
            }
            else if ((userMove == 1 && botMove == 3) ||  // Rock beats Scissors
                (userMove == 2 && botMove == 1) ||  // Paper beats Rock
                (userMove == 3 && botMove == 2)) {  // Scissors beats Paper
                cout << userName << " wins this round!" << endl;
                ++userScore;
            }
            else {
                cout << botName << " wins this round!" << endl;
                ++botScore;
            }

            cout << "Current Score: " << userName << ": " << userScore << " | " << botName << ": " << botScore << endl;

            if (choice == 2 || choice == 3) {
                cout << "Press Enter to continue to the next round...";
                cin.get();
            }
        }

        if (choice == 1 || choice == 2) {
            clearTerminal(); // Clear the terminal before showing the final result
            cout << "\nFinal Score: " << userName << ": " << userScore << " | " << botName << ": " << botScore << endl;
            if (userScore > botScore) {
                cout << userName << " wins the game!" << endl;
            }
            else if (botScore > userScore) {
                cout << botName << " wins the game!" << endl;
            }
            else {
                cout << "The game is a tie!" << endl;
            }
        }

        cout << "Press Enter to return to the menu...";
        cin.ignore((std::numeric_limits<std::streamsize>::max)(), '\n');
        cin.get();
    }
    isUserBusy = false;
}

//Dice roll
int rollDice(int numberOfDice = 1) {
    isUserBusy = true;
    sleepForMilliseconds(250);

    srand(static_cast<unsigned int>(time(nullptr)));
    vector<int> diceResults(numberOfDice);
    int total = 0;

    for (int i = 0; i < numberOfDice; ++i) {
        diceResults[i] = (rand() % 6) + 1;
        total += diceResults[i];
    }

    // Display the result for each dice
    for (int i = 0; i < numberOfDice; ++i) {
        cout << (i + 1) << " >> dice = " << diceResults[i] << endl;
    }
    cout << "Full result = " << total << endl;

    isUserBusy = false;
    return total;
}
void rollDiceMenu() {
    int choice, numberOfDice;
    isUserBusy = true;
    while (true) {
        clearTerminal();
        cout << "\nSelect dice roll option:\n";
        cout << "1. Single dice (1 dice)\n";
        cout << "2. Double dice (2 dice)\n";
        cout << "3. Multi dice (any number of dice)\n";
        cout << "4. Exit\n";
        cout << "Enter your choice: ";
        cin >> choice;

        switch (choice) {
        case 1:
            rollDice(1);
            break;
        case 2:
            rollDice(2);
            break;
        case 3:
            cout << "Enter number of dice to roll: ";
            cin >> numberOfDice;
            rollDice(numberOfDice);
            break;
        case 4:
            cout << "Exiting dice roll menu." << endl;
            isUserBusy = false;
            return;
        default:
            cout << "Invalid choice. Please try again." << endl;
        }

        cout << "\nPress Enter to return to the menu...";
        cin.ignore();
        cin.get();
    }
}

// The main game menu function
void gameMenu(const string& userName, const string& botName) {
    while (true) {
        clearTerminal();
        cout << "Game Menu:\n"
            << "1. Flip Coin\n"
            << "2. Dice\n"
            << "3. Rock Paper Scissors\n"
            << "4. Snake Game\n"
            << "5. Exit\n"
            << "Your Option: ";
        int g;
        cin >> g;

        // Validate input
        if (cin.fail()) {
            cin.clear();
            cin.ignore((std::numeric_limits<std::streamsize>::max)(), '\n');
            cout << "Invalid input. Please enter a number between 1 and 4.\n";
            continue;
        }

        switch (g) {
        case 1:
            flipCoin();
            break;
        case 2:
            rollDiceMenu();
            break;
        case 3:
            rockPaperScissors(userName, botName);
            break;
        case 4:
            snakeGame();
            break;
        case 5:
            cout << "Exiting Game Menu. Goodbye!" << endl;
            return;
        default:
            cout << "Invalid option. Please choose between 1 and 4.\n";
            break;
        }

        // Pause to let the user see the result before returning to the menu
        cout << "Press Enter to return to the menu...";
        cin.ignore((std::numeric_limits<std::streamsize>::max)(), '\n');
        cin.get();
    }
}

//Number generate
void generateRandomNumber() {
    isUserBusy = true;
    clearTerminal();
    srand(static_cast<unsigned int>(time(nullptr)));
    int choice;
    cout << "Select an option:\n"
        << "1. Generate 2-digit number (0 to 99)\n"
        << "2. Generate 4-digit number (0 to 9999)\n"
        << "3. Generate custom range number\n";
    cin >> choice;

    switch (choice) {
    case 1: {
        int randomNum = rand() % 100;  // Generates number between 0 and 99
        cout << "Generated: " << randomNum << endl;
        break;
    }
    case 2: {
        int randomNum = rand() % 10000;  // Generates number between 0 and 9999
        cout << "Generated: " << randomNum << endl;
        break;
    }
    case 3: {
        int digitCount;
        cout << "How many digits do you want to generate? ";
        cin >> digitCount;

        if (digitCount <= 0) {
            cout << "Invalid input! Number of digits must be greater than 0." << endl;
            break;
        }

        // Calculate the lower and upper bounds for the given number of digits
        int lowerBound = pow(10, digitCount - 1);  // For example, for 3 digits, this is 100
        int upperBound = pow(10, digitCount) - 1;  // For 3 digits, this is 999

        if (digitCount == 1) {
            lowerBound = 0;  // Special case for single digit numbers (0-9)
        }

        // Generate random number within the specified range
        int randomNum = rand() % (upperBound - lowerBound + 1) + lowerBound;
        cout << "Generated " << digitCount << "-digit number: " << randomNum << endl;
        break;
    }
    default:
        cout << "Invalid choice!" << endl;
    }
    isUserBusy = false;
}

//Compare Number
string generateNumberWithComparison(string& userMessage) {
    userMessage = trim(userMessage);  // Trim leading and trailing spaces
    string comparisonOperator;
    string numberPart;

    // Determine if the first two characters are an operator
    if (userMessage.substr(0, 2) == ">=" || userMessage.substr(0, 2) == "<=") {
        comparisonOperator = userMessage.substr(0, 2);  // Get the operator
        numberPart = trim(userMessage.substr(2));       // Extract the number part
    }
    else {
        comparisonOperator = userMessage.substr(0, 1);  // Single character operator
        numberPart = trim(userMessage.substr(1));       // Extract the number part
    }

    if (comparisonOperator == ">=" || comparisonOperator == "<=" ||
        comparisonOperator == ">" || comparisonOperator == "<") {
        try {
            int threshold = stoi(numberPart);  // Convert to integer
            if (threshold <= 0) {
                return "Threshold must be greater than 0.";
            }

            int digitCount = to_string(threshold).length();
            int lowerLimit, upperLimit;

            // Adjust the lower and upper limits based on the threshold's digit count
            if (threshold == pow(10, digitCount - 1)) {  // Power of 10 (like 10, 100, 1000)
                lowerLimit = pow(10, digitCount - 2);    // Set range one digit lower
                upperLimit = pow(10, digitCount - 1) - 1;
            }
            else {
                lowerLimit = pow(10, digitCount - 1);    // Usual range for the digit count
                upperLimit = pow(10, digitCount) - 1;
            }

            int randomNum;

            // Generate random number based on the operator
            if (comparisonOperator == ">") {
                randomNum = rand() % (upperLimit - threshold) + threshold + 1;
                return "Generated: " + to_string(randomNum) + " > " + to_string(threshold);
            }
            else if (comparisonOperator == ">=") {
                randomNum = rand() % (upperLimit - threshold + 1) + threshold;
                return "Generated: " + to_string(randomNum) + " >= " + to_string(threshold);
            }
            else if (comparisonOperator == "<") {
                randomNum = rand() % (threshold - lowerLimit) + lowerLimit;
                return "Generated: " + to_string(randomNum) + " < " + to_string(threshold);
            }
            else if (comparisonOperator == "<=") {
                randomNum = rand() % (threshold - lowerLimit + 1) + lowerLimit;
                return "Generated: " + to_string(randomNum) + " <= " + to_string(threshold);
            }

        }
        catch (invalid_argument&) {
            return "Invalid number format. Please enter a valid number.";
        }
    }
    else {
        return "Invalid input. Please include '<', '>', '>=', or '<=' to specify comparison.";
    }
    return "Unexpected error occurred.";
}

//facts
string getRandomfacts() {
    vector<string> facts = {
        "Did you know that octopuses have three hearts?",
        "The Eiffel Tower can be 15 cm taller during the summer, thanks to the expansion of iron in the heat!",
        "Honey never spoils. Archaeologists have found pots of honey in ancient Egyptian tombs that are over 3,000 years old and still edible!",
        "Bananas are berries, but strawberries aren't.",
        "A day on Venus is longer than a year on Venus. It takes 243 Earth days to rotate once, but only 225 Earth days to orbit the sun!",
        "There are more stars in the universe than grains of sand on all the beaches on Earth.",
        "An ostrich's eye is bigger than its brain.",
        "Sharks have been around longer than trees. They appeared about 400 million years ago, while trees showed up 350 million years ago.",
        "Sloths can hold their breath longer than dolphins. They can slow their heart rate to stay underwater for up to 40 minutes.",
        "A group of flamingos is called a 'flamboyance'.",
        "Your nose can remember 50,000 different scents.",
        "The shortest war in history was between Britain and Zanzibar in 1896. Zanzibar surrendered after 38 minutes.",
        "Butterflies can taste with their feet.",
        "In Switzerland, it's illegal to own just one guinea pig because they're social animals and get lonely!",
        "Wombat poop is cube-shaped!",
        "There's a species of jellyfish known as 'Turritopsis dohrnii' that can revert back to its juvenile form, essentially making it immortal!",
        "Polar bears' skin is actually black, and their fur is transparent. It appears white because it reflects light.",
        "Cows have best friends and can get stressed when separated.",
        "A bolt of lightning contains enough energy to toast 100,000 slices of bread!",
        "Water can boil and freeze at the same time, in a phenomenon called the 'triple point'.",
        "The largest living organism on Earth is a fungus in Oregon, covering over 2,400 acres.",
        "If you stretched out all the DNA in your body, it would reach the sun and back about 600 times.",
        "The inventor of the frisbee was turned into a frisbee after he died.",
        "A day on Mars is only 40 minutes longer than a day on Earth.",
        "Humans share about 60/100 of their DNA with bananas.",
        "In space, astronauts can grow up to 2 inches taller due to the lack of gravity compressing their spine.",
        "Sea otters hold hands while sleeping to avoid drifting apart.",
        "You can't hum while holding your nose closed (go ahead, try it!).",
        "There are more bacteria in your mouth than there are people on Earth.",
        "Venus is the hottest planet in the solar system, despite Mercury being closer to the sun.",
        "The dot over the letter 'i' is called a tittle.",
        "Cats can't taste sweetness because they lack sweet taste receptors.",
        "Some turtles can breathe through their butts. It's called cloacal respiration!",
        "Elephants are the only animals that can't jump.",
        "A group of crows is called a 'murder'.",
        "The longest hiccuping spree lasted 68 years.",
        "Koalas sleep up to 22 hours a day.",
        "The hashtag symbol is technically called an 'octothorpe'.",
        "Humans and giraffes have the same number of neck vertebrae—seven!",
        "A snail can sleep for three years at a time.",
        "Hot water freezes faster than cold water, a phenomenon known as the Mpemba effect.",
        "It's impossible to hum while holding your nose and closing your mouth.",
        "Some fish can cough, just like humans!",
        "Tigers have striped skin, not just striped fur.",
        "Caterpillars have about 4,000 muscles in their bodies, while humans have around 600.",
        "A crocodile can't stick its tongue out.",
        "One human can produce enough saliva in a lifetime to fill two swimming pools.",
        "The speed of a computer mouse is measured in 'Mickeys'.",
        "Bees can recognize human faces.",
        "The human body contains enough fat to make seven bars of soap.",
        "Alaska is the westernmost and easternmost state in the U.S. because the Aleutian Islands cross the 180° meridian of longitude.",
        "Rats laugh when they're tickled.",
        "Male seahorses are the ones who give birth.",
        "Apples float because 25/100 of their volume is air.",
        "The longest recorded flight of a chicken is 13 seconds.",
        "Peanuts aren't technically nuts—they're legumes.",
        "The largest snowflake ever recorded was 15 inches wide and 8 inches thick.",
        "Humans are the only animals with chins.",
        "The world's smallest reptile was discovered in 2021, a tiny chameleon that's about the size of a sunflower seed."
    };
    srand(static_cast<unsigned int>(time(nullptr)));
    return facts[rand() % facts.size()];
}

//random topic
string getRandomtopic() {
    vector<string> topic = {
        "The impact of technology on medern education.",
        "Sustainable living practices and their benefits.",
        "The psychology of decision-making.",
        "Curtural influences on art and literature.",
        "The role of social media in shapeing public opinion.",
        "Space exploration and it's potential future implications.",
        "The importance of mental health awareness.",
        "Evolution of popular music genres over the decades.",
        "Bioethics and the ethical considerations in medical advancements.",
        "Artificial intelligence and its ethical implications.",
        "The role of women in historical revolutions.",
        "Climate change and its effects on global ecosystems.",
        "The future of renewable energy sources.",
        "The impact of globalizations on local cultures.",
        "Cybersecurity challenges in the digital age.",
        "The evolution of fashion trends over the years.",
        "The psychology of creativity and innovation.",
        "The history and cultural significance of board games.",
        "The influence of social media on bod image perception.",
        "Space tourism and its potential impact on the travel industry.",
        "The relationship between technology and privacy rights.",
        "the history and impact of major scientific discoveries.",
        "Curtural diversity in the workplace and its advantages.",
        "The psychology lf motivation and goal-setting.",
        "Urbanization and its effects on the environment.",
        "The role of education in reducing poverty.",
        "the impact of social media on political activism.",
        "The history and cultural significance of traditional cuisines.",
        "The future of work in the era of automation and artificial intelligence.",
        "The philosophy of existentialism and its relevance today.",
        "Biotechnology and its potential in hearltcare advancement.",
        "The history and impact of magic scientific experiments.",
        "The role of sports in promotion social cohesion.",
        "The psychology of resilience in the face of adversity.",
        "The cutural significance of folklore and mythology.",
        "Challences and opportunities in space colonization.",
        "The evolution of language and Communication.",
        "The impact of social media influencers on consumer behavior.",
        "The history and cultural significance on street art.",
        "The role of empathy in build meaningful relationships.",
        "The future of vistual reality and augmented reality technologies.",
        "The impact of artifical intelligence on job markets.",
        "The psychology of fear and its effects on decision-making.",
        "The role of education in fostering environmental awareness.",
        "The history and impact of major architectutal movements.",
        "Ethical considerations in genetic engineering and cloning.",
        "The influence of pop culture on societal norms.",
        "The intersection of art and techonologuy in the digital age.",
        "The importance of critical thinking skills in education and everyday life.",
        "The benefits of meditation.",
        "Exploring dfferent types of cuisines.",
        "The impact of technology on society.",
        "Effective strategies for time management.",
        "Environmenttal conservation and sustainability.",
        "The psychology of motivation.",
        "The importance of fitness and exercise",
        "The history and significance off music genres.",
        "Tips for effective communication in relationship.",
        "The influence of social media on out dail lives.",
        "The art of storytelling in literature and film.",
        "Exploring different forms of art therapy.",
        "The impact of artificial intelligence on the job market.",
        "The benefits of artificial gratitube in everyday life.",
        "The rise of sustainable fashion and ethical consumerism.",
        "Understanding the concept of cultural appropration.",
        "The role of education in shaping future generations.",
        "Exploring differet methods of stress manegement.",
        "The hitory and evolution of photography as an art form.",
    };
    srand(static_cast<unsigned int>(time(nullptr)));
    return topic[rand() % topic.size()];
}

// Function to get a random joke
string getRandomJoke() {
    vector<string> jokes = {
        "Why don't scientists trust atoms? Because they make up everything!",
        "What do you call fake spaghetti? An impasta!",
        "Why did the scarecrow win an award? Because he was outstanding in his field!",
        "How does a penguin build its house? Igloos it together.",
        "Why did the bicycle fall over? Because it was TWO-TIRED.",
        "What did one say to the other wall? i'll meet you at the corner.",
        "Why couldn't the leopard play hide and seek? Because he was always spotted.",
        "What did the coffee file a police report? It got mugged.",
        "What do you call a fish wearing a bowtie? so-FISH-ticated",
        "What's a vampire's favorite fruit? A blood orange.",
        "How do you organize a space party? Your Planet.",
        "Why did the math book look sad? Because it had too many problems.",
        "Wbat do you call a snowman with a six-pack? An Abdominal snowman.",
        "Why did the tomato turn red? Because it saw the salad dressing.",
        "Did you hear about the cheese factory explosion? There was nothing left but DE-BRIE.",
        "What did the janitor say when he jumped out of the closet? Supplies.",
        "Why don't skeletons fight fight each other? They don't have guts.",
        "What did one hat say to the other? Stay here, I'm going on a-HEAD",
        "what did the computer go to threrapy? It had too many bytes of emotional baggage.",
        "Why don't scientists trust atoms anymore? Because they make up everything, even imaginary friends.",
        "What did the Sushi say to the fish, WASABI.",
        "What did one ocean say to the other ocean? Nothing, They just a WAVED.",
        "Did you hear about the guy who invented Lifesavers? He made a Mint.",
        "What did the grape say when it got stepped on? Nothing, it just let out a little wine.",
        "How do you catch a squirrel? Climb a tree and act like a nut!",
        "Why don't oysters donate to charity? Because they are shellfish.",
        "I only know 25 letters of the alphabet. I don't know y.",
        "What's brown and sticky? A stick.",
        "What do you call a fish with no eyes? F_SH.",
        "How do you make a tissue dance? You put a little boogie in it.",
        "What do you get when you cross a snowman and a vampire? Frostbite.",
        "Why don't seagulls fly over the bay? Because then they'd be bagels.",
        "How do you catch a lion? The answer is simple: climb into a closet and roar like a zebra.",
        "What do you call a factory that make good products? A SATISFACTORY.",
        "What do you call a can opener that doesn't work? A CAN't opener.",
        "why did the banana go to the doctor? Because it wasn't PEELING well.",
        "why did the cookies go to the doutor? It was feeling crumbly.",
        "Why did the golfer bring two pairs of pants? In case he got A HOLE IN ONE.",
        "What's orange and sounds like a parrot? A carrot.",
        "what do you call a bear with no teech? A gummy bear.",
        "Why did the computer go to therapy? It had too bytes of emotional >> DAMAGE << baggage",
        // Add more jokes as needed
    };
    srand(static_cast<unsigned int>(time(nullptr)));
    return jokes[rand() % jokes.size()];
}

// Function to handle key-based actions directly in the main loop
// Short cut keys
void handleKeyPress(const string& userMessage) {
    if (userMessage.empty()) {
        cout << "No key pressed." << endl;
        return; // Exit early if no input is provided
    }

    char key = tolower(userMessage[0]);
    cout << "\nYou pressed: " << key << endl;

    // Check for valid keys before triggering actions
    switch (key) {
    case 'w': {
        // Prompt for clarification: web, weather, or Wikipedia
        cout << "Do you mean Website, Weather, or Wikipedia?\n"
            << "1. Website\n"
            << "2. Weather\n"
            << "3. Wikipedia\n"
            << "Your Option: ";
        char choice;
        cin >> choice;
        cin.ignore((std::numeric_limits<std::streamsize>::max)(), '\n');

        switch (choice) {
        case '1':
            cout << "Opening Website...\n";
            webOpeningApp();
            break;
        case '2':
            cout << "Fetching Weather Information...\n";
            searchWeather();
            break;
        case '3':
            cout << "Opening Wikipedia...\n";
            openWiki();
            break;
        default:
            cout << "Invalid option. Please try again.\n";
            break;
        }
        break; // Break from 'w' case
    }

    case 'n': {
        cout << "Notepad? or Edit Notes?\n"
            << "1. Notepad.\n"
            << "2. Modify Notes\n"
            << "Your Option: ";
        char choice;
        cin >> choice;
        cin.ignore((std::numeric_limits<std::streamsize>::max)(), '\n');

        switch (choice) {
        case '1':
            notepadFunction();
            break;
        case '2': {
            string filepath;
            cout << "Please enter the file path to modify: ";
            cin.ignore();
            getline(cin, filepath);

            if (!filepath.empty()) {
                editNotepad(filepath);
            }
            else {
                cout << "Invalid file path.\n";
            }
            break;
        }
        default:
            cout << "Invalid option. Please choose 1 or 2.\n";
            break;
        }
        break;
    }

    case 'g': {
        cout << "Open Google or want to look for something?\n"
            << "1. Google.\n"
            << "2. Game Menu.\n"
            << "Your Option: ";
        char choice;
        cin >> choice;
        cin.ignore((std::numeric_limits<std::streamsize>::max)(), '\n');

        switch (choice) {
        case '1':
            googleMenu();
            break;
        case '2': {
            string userName = "Player";
            string botName = "VIAS";
            gameMenu(userName, botName);
            break;
        }
        default:
            cout << "Invalid option. Please choose 1 or 2.\n";
            break;
        }
        break;
    }

    case 'y': {
        cout << "Which One??\n"
            << "1. Open YouTube HomePage.\n"
            << "2. YouTube Search.\n"
            << "Your Option: ";
        char choice;
        cin >> choice;
        cin.ignore((std::numeric_limits<std::streamsize>::max)(), '\n');

        switch (choice) {
        case '1':
            openYouTube();
            break;
        case '2':
            searchYouTube();
            break;
        default:
            cout << "Invalid option.\n";
            break;
        }
        break;
    }

    case 'c': {
        cout << "Which One??\n"
            << "1. Time Convert\n"
            << "2. Base Convert (Bin, Oct, Dec, Hex)\n"
            << "3. Calendar\n"
            << "4. Coding.\n"
            << "5. Edit Code.\n"
            << "Your Option: ";
        char choice;
        cin >> choice;
        cin.ignore((std::numeric_limits<std::streamsize>::max)(), '\n');

        switch (choice) {
        case '1': {
            string result = getTimeConvert();
            cout << result << endl;
            break;
        }
        case '2':
            convertBase();
            break;
        case '3': {
            time_t now = time(0);
            tm* ltm = localtime(&now);

            int month = ltm->tm_mon;
            int day = ltm->tm_mday;
            int year = 1900 + ltm->tm_year;

            string monthName = months[month];

            drawCalendar(monthName, year, day);
            cout << "\nCurrent Date: " << monthName << " " << day << ", " << year << endl;
            break;
        }
        case '4': {
            string result = writeBuildRunCode();
            cout << result << endl;
            break;
        }
        case '5': {
            string result = editCode("./Programs/");
            cout << result << endl;
            break;
        }
        default:
            cout << "Invalid option. Please choose 1, 2, or 3.\n";
            break;
        }
        break;
    }

    default:
        cout << "Invalid key. Try different keys...\n";
        break;
    }
}

void handleUserMessage() {
    static int blankCount = 0;

    blankCount++;

    if (blankCount >= 10) {
        vector<string> frustrationResponses = {
            "VIAS: I'm getting frustrated! Please say something or apologize!",
            "VIAS: I'm starting to get a bit irritated. Can you please provide some input or say sorry?",
            "VIAS: It's getting a bit annoying now. How about giving me some details or apologizing?",
            "VIAS: I really need you to say something or apologize before I lose my patience.",
            "VIAS: I'm feeling a bit frustrated here. Let's either chat or clear things up with an apology."
        };
        srand(static_cast<unsigned int>(time(nullptr)));
        string frustrationResponse = frustrationResponses[rand() % frustrationResponses.size()];
        cout << frustrationResponse << endl;
    }
    else {
        vector<string> randomResponses = {
            "It looks like you didn't enter anything. Can you please provide more details?",
            "Sorry, I can't understand blank voice.",
            "I didn't get anything. Could you please type something?",
            "It seems like you're not saying anything. Can you elaborate?",
            "I'm not sure what you're trying to convey. Can you type your message?"
        };
        srand(static_cast<unsigned int>(time(nullptr)));
        string randomResponse = randomResponses[rand() % randomResponses.size()];
        cout << "VIAS: " << randomResponse << endl;
    }
}
#endif

//navigating File in terminal
void openFileOrFolder(const string& path) {
#ifdef _WIN32
    string command = "start \"\" \"" + path + "\"";  // Ensure the path is in quotes
    system(command.c_str());
#elif __APPLE__
    string command = "open \"" + path + "\"";
    system(command.c_str());
#elif __linux__
    string command = "xdg-open \"" + path + "\"";
    system(command.c_str());
#else
    cerr << "Unsupported operating system." << endl;
#endif
}
// Function to display a page of directory contents
void displayPage(const vector<string>& items, int page, int pageSize) {
    int start = page * pageSize;
    int end = min(start + pageSize, static_cast<int>(items.size()));
    cout << "\n--- Directory Contents (Page " << page + 1 << ") ---" << endl;

    for (int i = start; i < end; ++i) {
        cout << (i + 1) << ": " << items[i] << endl;
    }

    cout << "Page " << (page + 1) << " of " << ((items.size() + pageSize - 1) / pageSize) << "\n" << endl;
}
#ifdef _WIN32
//for Windows
string getDesktopPath() {
    char* userProfile = getenv("USERPROFILE");
    if (userProfile) {
        return string(userProfile) + "\\";
    }
    else {
        return "C:\\Users\\";
    }
}

void navigateDirectory(const string& initialPath = "") {
    isUserBusy = true;
    string currentPath = initialPath.empty() ? getDesktopPath() : initialPath;
    if (currentPath.empty()) {
        return;  // Exit if the path couldn't be determined
    }

    int page = 0;
    const int pageSize = 15;
    bool isRunning = true;

    while (isRunning) {
        clearTerminal();

        vector<string> items;
        for (const auto& entry : fs::directory_iterator(currentPath)) {
            items.push_back(entry.path().filename().string());
        }

        displayPage(items, page, pageSize);

        cout << "Current Directory: " << currentPath << endl;
        cout << "\nCommands:\n";
        cout << "<< Create / Edit >>\n"
            << "- [c] Create new file/folder\n"
            << "- [e] Edit file\n"
            << "- [r <num>] Rename file/folder\n"
            << "- [d <num>] Delete file/folder\n\n";

        cout << "<< File / Folder >>\n"
            << "- [<num> -n] Open inside terminal\n"
            << "- [<num> -o] Open outside terminal\n\n";

        cout << "<< Page >>\n";
        if (page > 0) cout << "- [<] Previous Page\n";
        if ((page + 1) * pageSize < items.size()) cout << "- [>] Next Page\n";
        cout << "- [#<pageNum>] Jump to Page\n"
            << "- [-r] Return Directory\n"
            << "- [-u] Return to Home Directory\n\n";

        cout << "<< Change path / Exit >>\n"
            << "- [-d] Change Directory\n"  // New option to change directory
            << "- [-q] Quit\n\n";

        cout << "Enter command: ";
        string command;
        getline(cin, command);

        // Command Handling
        if (command == "-q" || command == "-Q") {
            isRunning = false;
        }
        else if (command == "-d") {
            // Change directory option
            cout << "Enter the new directory path: ";
            string newPath;
            getline(cin, newPath);

            if (fs::exists(newPath) && fs::is_directory(newPath)) {
                currentPath = newPath;  // Change the current path
                page = 0;  // Reset page number when changing the directory
            }
            else {
                cout << "Invalid directory. Please try again.\n";
            }
        }
        else if (command == "c") {
            // Create File/Folder
            cout << "Select type:\n"
                << "- [1] Code file\n"
                << "- [2] Note file\n"
                << "- [3] Create Folder\n"
                << "Enter choice: ";
            string fileType;
            getline(cin, fileType);

            if (fileType == "1") {
                string folderPath = currentPath;
                cout << "Enter the filename (including extension, e.g., 'example.cpp'): ";
                string filename;
                getline(cin, filename);

                clearTerminal();
                // Use currentPath to save in the current directory
                string filepath = currentPath + "/" + filename;

                // Detect language based on file extension
                string extension = filepath.substr(filepath.find_last_of('.') + 1);
                string buildCommand, runCommand;
                if (extension == "cpp") {
                    buildCommand = "g++ " + filepath + " -o " + currentPath + "/program";
                    runCommand = currentPath + "/program";
                }
                else if (extension == "c") {
                    buildCommand = "gcc " + filepath + " -o " + currentPath + "/program";
                    runCommand = currentPath + "/program";
                }
                else if (extension == "cs") {
                    buildCommand = "mcs " + filepath + " -out:" + currentPath + "/program.exe";
                    runCommand = "mono " + currentPath + "/program.exe";
                }
                else if (extension == "go") {
                    runCommand = "go run " + filepath;
                }
                else if (extension == "js") {
                    runCommand = "node " + filepath;
                }
                else if (extension == "py") {
                    runCommand = "python3 " + filepath;
                }
                else if (extension == "java") {
                    buildCommand = "javac " + filepath;
                    runCommand = "java -cp " + currentPath + " " + filename.substr(0, filename.find_last_of('.'));
                }
                else {
                    cout << "Unsupported file extension. Please use a supported language.\n";
                    return;
                }

                vector<string> codeLines;

                // Writing Code
                cout << "Start writing your code. Type 'END' or 'EXIT' on a blank line to finish.\n";
                string line;
                while (true) {
                    getline(cin, line);
                    if (line == "END" || line == "EXIT") break;
                    codeLines.push_back(line);
                }

                // Save Code to File in the current directory
                ofstream outFile(filepath, ios::trunc);
                if (outFile.is_open()) {
                    for (const auto& codeLine : codeLines) {
                        outFile << codeLine << endl;
                    }
                    outFile.close();
                    cout << "Code saved to " << filepath << endl;
                }
                else {
                    cout << "Error saving file.\n";
                    return;
                }

                // Display written content
                clearTerminal();
                cout << "\nYour code content:\n";
                for (const auto& codeLine : codeLines) {
                    cout << codeLine << endl;
                }

                // Interaction Menu
                while (true) {
                    clearTerminal();
                    // Display the current file content with line numbers
                    cout << "\nCurrent content of '" << filename << "':\n\n";
                    for (size_t i = 0; i < codeLines.size(); ++i) {
                        cout << i + 1 << ". " << codeLines[i] << endl;  // Display line number
                    }

                    cout << "\n\nOptions:\n"
                        << "[b] Build Code (C, C++, Java)\n"
                        << "[br] Build and Run (C, C++, Java)\n"
                        << "[r] Run Only (Python, pre-built binaries)\n"
                        << "[u] Update/Change Code\n"
                        << "[f] Finish\n"
                        << "Enter your choice: ";
                    string choice;
                    getline(cin, choice);
                    choice = toLower(choice);

                    if (choice == "b") {
                        string buildResult = buildCode(filepath);
                        cout << buildResult << endl;
                    }
                    else if (choice == "br") {
                        clearTerminal();
                        string buildAndRunResult = buildAndRunCode(filepath);
                        cout << buildAndRunResult << endl;
                        cout << "\n>> press enter to finish <<\n";
                        cin.get();  // Wait for user input to finish testing
                    }
                    else if (choice == "r") {
                        clearTerminal();
                        string runResult = runCode(filepath);
                        cout << runResult << endl;
                        cout << "\n>> press enter to finish <<\n";
                        cin.get();  // Wait for user input to finish testing
                    }
                    else if (choice == "u") {
                        // Update/Change Code (additional operations)
                        clearTerminal();
                        cout << "\nCurrent content of '" << filename << "':\n\n";
                        for (size_t i = 0; i < codeLines.size(); ++i) {
                            cout << i + 1 << ". " << codeLines[i] << endl;  // Display line number
                        }

                        cout << "\n\nDo you want to perform any operations on '" << filename << "'?\n"
                            << "[r] Rename file\n"
                            << "[s] Open file location\n"
                            << "[c] Continue writing\n"
                            << "[a] Add a line\n"
                            << "[d] Delete a line\n"
                            << "[o] Overwrite a line\n"
                            << "[f] Finish\n"
                            << "Enter your choice: ";
                        string updateChoice;
                        getline(cin, updateChoice);
                        updateChoice = toLower(updateChoice);

                        if (updateChoice == "r") {
                            // Rename file
                            cout << "Enter new filename (including extension): ";
                            string newFilename;
                            getline(cin, newFilename);
                            string newFilepath = folderPath + "/" + newFilename;
                            if (rename(filepath.c_str(), newFilepath.c_str()) == 0) {
                                cout << "File renamed to " << newFilename << endl;
                                filepath = newFilepath;
                                filename = newFilename;
                            }
                            else {
                                cout << "Failed to rename file.\n";
                            }
                        }
                        else if (updateChoice == "s") {
                            // Open file location
                            string openCommand = "explorer " + folderPath;  // Windows specific
                            system(openCommand.c_str());
                            cout << "Opening file location...\n";
                        }
                        else if (updateChoice == "c") {
                            // Continue writing code
                            cout << "\nContinue writing your code. Type 'END' or 'EXIT' on a blank line to finish.\n";
                            while (true) {
                                getline(cin, line);
                                if (line == "END" || line == "EXIT") break;
                                codeLines.push_back(line);
                            }

                            // Save the updated code to the file
                            ofstream outFile(filepath, ios::trunc);
                            if (outFile.is_open()) {
                                for (const auto& codeLine : codeLines) {
                                    outFile << codeLine << endl;
                                }
                                outFile.close();
                                cout << "Code updated and saved to " << filepath << endl;
                            }
                            else {
                                cout << "Error saving updated code.\n";
                            }
                        }
                        else if (updateChoice == "a") {
                            // Add a line
                            cout << "Choose an option:\n"
                                << "1. Add a new line at the end.\n"
                                << "2. Add a new line in between.\n"
                                << "Enter your choice: ";
                            string addOption;
                            getline(cin, addOption);

                            if (addOption == "1") {
                                // Add a new line at the end
                                cout << "Add a line of content: ";
                                string newLine;
                                getline(cin, newLine);
                                codeLines.push_back(newLine);

                                // Save the updated code to the file
                                ofstream outFile(filepath, ios::trunc);
                                if (outFile.is_open()) {
                                    for (const auto& codeLine : codeLines) {
                                        outFile << codeLine << endl;
                                    }
                                    outFile.close();
                                    cout << "Content added and saved to " << filepath << endl;
                                }
                                else {
                                    cout << "Error saving updated code.\n";
                                }

                            }
                            else if (addOption == "2") {
                                // Add a new line in between
                                cout << "Enter the line number to add below: ";
                                int lineNumber;
                                cin >> lineNumber;
                                cin.ignore(); // Clear newline from the input buffer

                                if (lineNumber < 1 || lineNumber > codeLines.size()) {
                                    cout << "Invalid line number. Operation canceled.\n";
                                }
                                else {
                                    cout << "Choose an option:\n"
                                        << "1. Add a single line.\n"
                                        << "2. Add multiple lines (type 'END' or 'EXIT' to finish).\n"
                                        << "Enter your choice: ";
                                    string inBetweenOption;
                                    getline(cin, inBetweenOption);

                                    if (inBetweenOption == "1") {
                                        // Add a single line
                                        cout << "Enter the content to add: ";
                                        string newLine;
                                        getline(cin, newLine);
                                        codeLines.insert(codeLines.begin() + lineNumber, newLine);
                                    }
                                    else if (inBetweenOption == "2") {
                                        // Add multiple lines
                                        clearTerminal();
                                        cout << "Enter lines to add (type 'END' or 'EXIT' on a blank line to finish):\n\n";
                                        vector<string> newLines;
                                        while (true) {
                                            string newLine;
                                            getline(cin, newLine);
                                            if (newLine == "END" || newLine == "EXIT") break;
                                            newLines.push_back(newLine);
                                        }
                                        codeLines.insert(codeLines.begin() + lineNumber, newLines.begin(), newLines.end());
                                    }
                                    else {
                                        cout << "Invalid option. Operation canceled.\n";
                                    }

                                    // Save the updated code to the file
                                    ofstream outFile(filepath, ios::trunc);
                                    if (outFile.is_open()) {
                                        for (const auto& codeLine : codeLines) {
                                            outFile << codeLine << endl;
                                        }
                                        outFile.close();
                                        cout << "Lines added and saved to " << filepath << endl;
                                    }
                                    else {
                                        cout << "Error saving updated code.\n";
                                    }
                                }
                            }
                            else {
                                cout << "Invalid choice. Operation canceled.\n";
                            }

                        }
                        else if (updateChoice == "d") {
                            // Delete a line
                            cout << "Enter the line number to delete: ";
                            size_t lineNumber;
                            cin >> lineNumber;
                            cin.ignore();
                            if (lineNumber > 0 && lineNumber <= codeLines.size()) {
                                codeLines.erase(codeLines.begin() + lineNumber - 1);
                                // Save the updated code to the file
                                ofstream outFile(filepath, ios::trunc);
                                if (outFile.is_open()) {
                                    for (const auto& codeLine : codeLines) {
                                        outFile << codeLine << endl;
                                    }
                                    outFile.close();
                                    cout << "Line deleted and file updated.\n";
                                }
                                else {
                                    cout << "Error saving updated file.\n";
                                }
                            }
                            else {
                                cout << "Invalid line number.\n";
                            }

                        }
                        else if (updateChoice == "o") {
                            // Overwrite a line
                            cout << "Enter the line number to overwrite: ";
                            size_t lineNumber;
                            cin >> lineNumber;
                            cin.ignore();
                            if (lineNumber > 0 && lineNumber <= codeLines.size()) {
                                cout << "Enter the new content for the line: ";
                                string newContent;
                                getline(cin, newContent);
                                codeLines[lineNumber - 1] = newContent;
                                // Save the updated code to the file
                                ofstream outFile(filepath, ios::trunc);
                                if (outFile.is_open()) {
                                    for (const auto& codeLine : codeLines) {
                                        outFile << codeLine << endl;
                                    }
                                    outFile.close();
                                    cout << "Line overwritten and file updated.\n";
                                }
                                else {
                                    cout << "Error saving updated file.\n";
                                }
                            }
                            else {
                                cout << "Invalid line number.\n";
                            }
                        }
                        else if (updateChoice == "f") {
                            // Finish and return
                            break;
                        }
                    }
                    else if (choice == "f") {
                        break;
                    }
                }
            }
            else if (fileType == "2") {
                cout << "Enter the filename for your note (without extension): ";
                string noteFileName;
                getline(cin, noteFileName);

                string filepath = currentPath + "\\" + noteFileName + ".txt";

                clearTerminal();
                cout << endl << "Welcome to the terminal notepad!\n"
                    << "Start typing your notes. Type 'EXIT' on a new line to finish.\n\n";

                vector<string> notes;
                string line;

                // Input phase
                while (true) {
                    getline(cin, line);
                    if (toLower(line) == "exit") break;
                    notes.push_back(line);
                }

                // Editing phase
                while (true) {
                    clearTerminal();
                    cout << "\nCurrent Notes:\n";
                    for (size_t i = 0; i < notes.size(); ++i) {
                        cout << i + 1 << ": " << notes[i] << endl;
                    }

                    cout << "\nOptions:\n"
                        << "[c] Continue writing\n"
                        << "[a] Add a new line\n"
                        << "[e] Edit a line\n"
                        << "[d] Delete a line\n"
                        << "[s] Save and Exit\n"
                        << "[x] Exit without saving\n"
                        << "Enter your choice: ";

                    string choice;
                    getline(cin, choice);
                    choice = toLower(choice);

                    if (choice == "c") {  // Continue writing
                        clearTerminal();
                        cout << "\n>> Continue below. Type 'EXIT' on a new line to stop.\n\n";
                        for (const auto& existingLine : notes) {
                            cout << existingLine << endl;
                        }

                        cout << "\nChoose action:\n"
                            << "[1] Append at the end\n"
                            << "[2] Insert at a specific position\n";
                        string action;
                        getline(cin, action);

                        if (action == "1") {  // Append at the end
                            while (true) {
                                getline(cin, line);
                                if (toLower(line) == "exit") break;
                                notes.push_back(line);
                            }
                        }
                        else if (action == "2") {  // Insert at specific position
                            cout << "Enter the position to insert (1 to " << notes.size() + 1 << "): ";
                            size_t pos;
                            cin >> pos;
                            cin.ignore();

                            if (pos >= 1 && pos <= notes.size() + 1) {
                                cout << "Enter the content to insert: ";
                                getline(cin, line);
                                notes.insert(notes.begin() + pos - 1, line);
                            }
                            else {
                                cout << "Invalid position. Try again.\n";
                            }
                        }
                        else {
                            cout << "Invalid action. Returning.\n";
                        }
                    }
                    else if (choice == "e") {  // Edit a line
                        cout << "\nChoose action:\n"
                            << "[1] Insert lines below\n"
                            << "[2] Overwrite line\n";
                        string action;
                        getline(cin, action);

                        if (action == "1") {  // Insert lines below
                            cout << "Enter the line number to insert below: ";
                            size_t lineNumber;
                            cin >> lineNumber;
                            cin.ignore();

                            if (lineNumber > 0 && lineNumber <= notes.size()) {
                                cout << "Enter the content to insert: ";
                                getline(cin, line);
                                notes.insert(notes.begin() + lineNumber, line);
                            }
                            else {
                                cout << "Invalid line number. Try again.\n";
                            }
                        }
                        else if (action == "2") {  // Overwrite line
                            cout << "Enter the line number to overwrite: ";
                            size_t lineNumber;
                            cin >> lineNumber;
                            cin.ignore();

                            if (lineNumber > 0 && lineNumber <= notes.size()) {
                                cout << "Current content: " << notes[lineNumber - 1] << endl;
                                cout << "Enter new content: ";
                                getline(cin, notes[lineNumber - 1]);
                            }
                            else {
                                cout << "Invalid line number. Try again.\n";
                            }
                        }
                        else {
                            cout << "Invalid action. Returning.\n";
                        }
                    }
                    else if (choice == "a") {  // Add a new line
                        cout << "Enter the new line content: ";
                        string newLine;
                        getline(cin, newLine);
                        notes.push_back(newLine);
                    }
                    else if (choice == "d") {  // Delete a line
                        cout << "Enter the line number to delete: ";
                        size_t lineNumber;
                        cin >> lineNumber;
                        cin.ignore();

                        if (lineNumber > 0 && lineNumber <= notes.size()) {
                            notes.erase(notes.begin() + lineNumber - 1);
                            cout << "Line deleted successfully.\n";
                        }
                        else {
                            cout << "Invalid line number. Try again.\n";
                        }
                    }
                    else if (choice == "s") {  // Save and Exit
                        ofstream outFile(filepath);
                        if (outFile.is_open()) {
                            for (const string& note : notes) {
                                outFile << note << endl;
                            }
                            outFile.close();
                            cout << "Notes saved successfully to " << filepath << endl;
                        }
                        else {
                            cerr << "Error opening file for writing.\n\n";
                        }
                        break;
                    }
                    else if (choice == "x") {  // Exit without saving
                        cout << "!! WARNING !! Notes were not saved.\n\n";
                        break;
                    }
                    else {
                        cout << "Invalid choice. Please try again.\n";
                    }
                }
            }
            else if (fileType == "3") {
                // Create Folder
                cout << "Enter the folder name: ";
                string folderName;
                getline(cin, folderName);

                string folderPath = currentPath + "/" + folderName;
                if (fs::create_directory(folderPath)) {
                    cout << "Folder created: " << folderPath << endl;
                }
                else {
                    cout << "Error creating folder.\n";
                }
            }
            else {
                cout << "Invalid choice. Please try again.\n";
            }

        }
        else if (command == "e") {
            // Edit File Option
            cout << "Select option to perform:\n"
                << "- [1] Code file\n"
                << "- [2] Note file\n"
                << "Enter choice: ";

            string editOption;
            getline(cin, editOption);

            if (editOption == "1") {
                // List all files in the current directory to select a code file
                clearTerminal();
                vector<string> codeFiles;
                for (const auto& entry : fs::directory_iterator(currentPath)) {
                    if (entry.is_regular_file()) {
                        codeFiles.push_back(entry.path().filename().string());
                    }
                }

                if (codeFiles.empty()) {
                    cout << "No code files found in the current directory.\n";
                    return;
                }

                // Display available code files
                cout << "\nAvailable code files:\n";
                for (int i = 0; i < codeFiles.size(); ++i) {
                    cout << i + 1 << ". " << codeFiles[i] << endl;
                }

                // Let the user select the file they want to edit
                cout << "\nEnter the number of the file you want to edit: ";
                string input;
                getline(cin, input);

                try {
                    int fileIndex = stoi(input) - 1;
                    if (fileIndex >= 0 && fileIndex < codeFiles.size()) {
                        string selectedFilename = codeFiles[fileIndex];
                        string filepath = currentPath + "/" + selectedFilename;

                        // Check if the selected file is valid
                        if (!fs::exists(filepath) || fs::is_directory(filepath)) {
                            cout << "The selected path is not a valid file.\n";
                            return;
                        }

                        // File management menu
                        bool editing = true;
                        while (editing) {
                            clearTerminal(); // Ensure screen refresh
                            cout << "============================\n"
                                << "File selected: " << selectedFilename << endl
                                << "============================\n"
                                << "Options for the selected file:\n"
                                << "1. View File\n"
                                << "2. Edit File\n"
                                << "3. Build/Run\n"
                                << "4. Delete File\n"
                                << "5. Rename File\n"
                                << "6. Save & Exit\n"
                                << "7. Discard Changes & Exit\n";

                            cout << "\nEnter your choice: ";
                            string fileOption;
                            getline(cin, fileOption);

                            if (fileOption == "1") {
                                displayNotepad(filepath);
                            }
                            else if (fileOption == "2") {
                                editAndWriteCode(filepath);
                            }
                            else if (fileOption == "3") {
                                clearTerminal();
                                cout << "\nSelect an option for running code:\n"
                                    << "1. Build and Run (C, C++, Java)\n"
                                    << "2. Run Only (Python, pre-built binaries)\n"
                                    << "Your Option: ";

                                string runOption;
                                getline(cin, runOption);

                                if (runOption == "1") {
                                    string buildResult = buildAndRunCode(filepath);
                                    cout << buildResult << endl;
                                    cout << "\n>> Press enter to finish <<\n";
                                    cin.get();
                                }
                                else if (runOption == "2") {
                                    clearTerminal();
                                    string runResult = runCode(filepath);
                                    cout << runResult << endl;
                                    cout << "\n>> Press enter to finish <<\n";
                                    cin.get();
                                }
                                else {
                                    cout << "Invalid option. Returning to file menu.\n";
                                    pauseConsole();
                                }
                            }
                            else if (fileOption == "4") {
                                deleteFile(filepath);
                                cout << "File deleted successfully.\n";
                                codeFiles.erase(codeFiles.begin() + fileIndex);
                                break; // Exit menu after deleting
                            }
                            else if (fileOption == "5") {
                                renameCodeFile(filepath);
                            }
                            else if (fileOption == "6") {
                                cout << "Changes saved to file: " << selectedFilename << endl;
                                editing = false;
                            }
                            else if (fileOption == "7") {
                                cout << "Changes discarded. Exiting edit mode.\n";
                                editing = false;
                            }
                            else {
                                cout << "Invalid choice. Please try again.\n";
                            }
                        }
                    }
                    else {
                        cout << "Invalid file number. Please try again.\n";
                    }
                }
                catch (...) {
                    cout << "Invalid input. Please enter a valid file number.\n";
                }
            }
            else if (editOption == "2") {
                clearTerminal();
                cout << "Select a note file to edit:\n";
                vector<string> noteFiles;
                for (const auto& entry : fs::directory_iterator(currentPath)) {
                    if (entry.is_regular_file() && entry.path().extension() == ".txt") {
                        noteFiles.push_back(entry.path().filename().string());
                    }
                }

                if (noteFiles.empty()) {
                    cout << "No note files found in the current directory.\n";
                    return;
                }

                const int pageSize = 15; // Number of files per page
                int currentPage = 0;
                bool isRunning = true;

                while (isRunning) {
                    clearTerminal();

                    // Calculate page bounds
                    int totalFiles = noteFiles.size();
                    int totalPages = (totalFiles + pageSize - 1) / pageSize; // Round up for total pages
                    int startIdx = currentPage * pageSize;
                    int endIdx = min(startIdx + pageSize, totalFiles);

                    // Display files for the current page
                    cout << "\nAvailable note files (Page " << currentPage + 1 << " of " << totalPages << "):\n";
                    for (int i = startIdx; i < endIdx; ++i) {
                        cout << i + 1 << ". " << noteFiles[i] << endl;
                    }

                    // Navigation options
                    cout << "\nNavigation options:\n";
                    if (currentPage > 0) cout << "[<] Previous Page\n";
                    if (currentPage < totalPages - 1) cout << "[>] Next Page\n";
                    cout << "[#] Jump to Page\n"
                        << "[q] Return\n"
                        << "\nEnter your choice or select a file number: ";

                    string input;
                    getline(cin, input);

                    if (input == "<" && currentPage > 0) {
                        --currentPage;
                    }
                    else if (input == ">" && currentPage < totalPages - 1) {
                        ++currentPage;
                    }
                    else if (input == "#") {
                        cout << "Enter page number: ";
                        string pageInput;
                        getline(cin, pageInput);
                        try {
                            int targetPage = stoi(pageInput) - 1;
                            if (targetPage >= 0 && targetPage < totalPages) {
                                currentPage = targetPage;
                            }
                            else {
                                cout << "Invalid page number. Please try again.\n";
                                pauseConsole();
                            }
                        }
                        catch (...) {
                            cout << "Invalid input format. Enter a number to jump to a page.\n";
                            pauseConsole();
                        }
                    }
                    else if (input == "q") {
                        break;
                    }
                    else {
                        try {
                            int fileIndex = stoi(input) - 1;
                            if (fileIndex >= 0 && fileIndex < noteFiles.size()) {
                                string selectedFilename = noteFiles[fileIndex];
                                string fileToEdit = currentPath + "/" + selectedFilename;

                                // Check if the selected file is valid
                                if (!fs::exists(fileToEdit) || fs::is_directory(fileToEdit)) {
                                    cout << "The selected path is not a valid file.\n";
                                    return;
                                }

                                // Edit the note file
                                ifstream inFile(fileToEdit);
                                vector<string> notes;
                                string line;

                                // Read the existing file content into a vector
                                if (inFile.is_open()) {
                                    while (getline(inFile, line)) {
                                        notes.push_back(line);
                                    }
                                    inFile.close();
                                }
                                else {
                                    cerr << "Error opening file for reading: " << fileToEdit << endl;
                                    return;
                                }

                                while (true) {
                                    clearTerminal();
                                    cout << "\nCurrent Notes in " << fileToEdit << ":\n";
                                    for (size_t i = 0; i < notes.size(); ++i) {
                                        cout << i + 1 << ": " << notes[i] << endl;
                                    }

                                    cout << "\nOptions:\n"
                                        << "[c] Continue\n"
                                        << "[a] Add a new line\n"
                                        << "[e] Edit line\n"
                                        << "[d] Delete a line\n"
                                        << "[s] Save and Exit\n"
                                        << "[x] Exit without saving\n"
                                        << "\nEnter your choice: ";

                                    string choice;
                                    getline(cin, choice);
                                    choice = toLower(choice);

                                    if (choice == "c") {  // Continue writing
                                        cout << "\n>> Continue writing (append at the end or insert at a specific position):\n\n";
                                        cout << "Choose action:\n"
                                            << "[1] Append at the end\n"
                                            << "[2] Insert at a specific position\n";
                                        cout << "Enter Option: ";
                                        string action;
                                        getline(cin, action);

                                        if (action == "1") {  // Append at the end
                                            cout << "\nStart typing your new content (type 'EXIT' on a new line to stop):\n";
                                            for (size_t i = 0; i < notes.size(); ++i) {
                                                cout << notes[i] << endl;
                                            }
                                            while (true) {
                                                getline(cin, line);
                                                if (toLower(line) == "exit") {
                                                    break;
                                                }
                                                notes.push_back(line);
                                            }

                                            // Display updated notes
                                            clearTerminal();
                                            cout << "\nUpdated Notes in " << fileToEdit << ":\n";
                                            for (size_t i = 0; i < notes.size(); ++i) {
                                                cout << i + 1 << ": " << notes[i] << endl;
                                            }
                                        }
                                        else if (action == "2") {  // Insert at specific position
                                            cout << "Enter the line to insert: ";
                                            size_t pos;
                                            cin >> pos;
                                            cin.ignore();

                                            if (pos >= 1 && pos <= notes.size() + 1) {
                                                cout << "Enter the content to insert (type 'EXIT' to finish):\n";
                                                vector<string> newLines;
                                                while (true) {
                                                    getline(cin, line);
                                                    if (toLower(line) == "exit") {
                                                        break;
                                                    }
                                                    newLines.push_back(line);
                                                }
                                                notes.insert(notes.begin() + pos - 1, newLines.begin(), newLines.end());
                                            }
                                            else {
                                                cout << "Invalid position. Try again.\n";
                                            }
                                        }
                                        else {
                                            cout << "Invalid action. Returning.\n";
                                        }
                                    }
                                    else if (choice == "e") {  // Edit a line
                                        cout << "Choose action:\n"
                                            << "[1] Insert lines below\n"
                                            << "[2] Overwrite line\n";
                                        cout << "Enter Option: ";
                                        string action;
                                        getline(cin, action);

                                        if (action == "1") {  // Insert lines below
                                            cout << "Enter the line number to insert below: ";
                                            size_t lineNumber;
                                            cin >> lineNumber;
                                            cin.ignore();

                                            if (lineNumber > 0 && lineNumber <= notes.size()) {
                                                cout << "Enter the content to insert: ";
                                                getline(cin, line);
                                                notes.insert(notes.begin() + lineNumber, line);
                                            }
                                            else {
                                                cout << "Invalid line number. Try again.\n";
                                            }
                                        }
                                        else if (action == "2") {  // Overwrite line
                                            cout << "Enter the line number to overwrite: ";
                                            size_t lineNumber;
                                            cin >> lineNumber;
                                            cin.ignore();

                                            if (lineNumber > 0 && lineNumber <= notes.size()) {
                                                cout << "Current content: " << notes[lineNumber - 1] << endl;
                                                cout << "Enter new content: ";
                                                getline(cin, notes[lineNumber - 1]);
                                            }
                                            else {
                                                cout << "Invalid line number. Try again.\n";
                                            }
                                        }
                                        else {
                                            cout << "Invalid action. Returning.\n";
                                        }
                                    }
                                    else if (choice == "a") {  // Add a new line
                                        cout << "Enter the new line content: ";
                                        string newLine;
                                        getline(cin, newLine);
                                        notes.push_back(newLine);
                                    }
                                    else if (choice == "d") {  // Delete a line
                                        cout << "Enter the line number to delete: ";
                                        size_t lineNumber;
                                        cin >> lineNumber;
                                        cin.ignore();

                                        if (lineNumber > 0 && lineNumber <= notes.size()) {
                                            notes.erase(notes.begin() + lineNumber - 1);
                                            cout << "Line deleted successfully.\n";
                                        }
                                        else {
                                            cout << "Invalid line number. Try again.\n";
                                        }
                                    }
                                    else if (choice == "s") {  // Save and Exit
                                        ofstream outFile(fileToEdit, ios::trunc);
                                        if (outFile.is_open()) {
                                            for (const string& note : notes) {
                                                outFile << note << endl;
                                            }
                                            outFile.close();
                                            cout << "Notes saved successfully to " << fileToEdit << endl;
                                        }
                                        else {
                                            cerr << "Error opening file for writing.\n\n";
                                        }
                                        break;
                                    }
                                    else if (choice == "x") {  // Exit without saving
                                        cout << "!! WARNING !! Changes were not saved.\n\n";
                                        break;
                                    }
                                    else {
                                        cout << "Invalid choice. Please try again.\n";
                                    }
                                }
                            }
                            else {
                                cout << "Invalid file number. Please try again.\n";
                            }
                        }
                        catch (...) {
                            cout << "Invalid input. Please enter a valid file number.\n";
                        }
                    }
                }
            }
            else {
                cout << "Invalid option. Operation canceled.\n";
            }
        }
        else if (command.substr(0, 2) == "r ") {
            // Rename File/Folder
            int index;
            if (sscanf(command.c_str(), "r %d", &index) == 1 && index > 0 && index <= items.size()) {
                string oldPath = currentPath + "/" + items[index - 1];
                cout << "Enter new name for '" << items[index - 1] << "': ";
                string newName;
                getline(cin, newName);
                string newPath = currentPath + "/" + newName;

                if (rename(oldPath.c_str(), newPath.c_str()) == 0) {
                    cout << "Renamed successfully to '" << newName << "'.\n";
                }
                else {
                    cout << "Error renaming file/folder.\n";
                }
            }
            else {
                cout << "Invalid selection.\n";
            }

        }
        else if (command.substr(0, 2) == "d ") {
            // Delete File/Folder
            int index;
            if (sscanf(command.c_str(), "d %d", &index) == 1 && index > 0 && index <= items.size()) {
                string targetPath = currentPath + "/" + items[index - 1];

                try {
                    // Check if the target path exists
                    if (fs::exists(targetPath)) {
                        // If it's a file, remove it
                        if (fs::is_regular_file(targetPath)) {
                            fs::remove(targetPath);
                            cout << "Deleted file: '" << items[index - 1] << "'.\n";
                        }
                        // If it's a directory, remove it (empty or non-empty)
                        else if (fs::is_directory(targetPath)) {
                            fs::remove_all(targetPath);  // Removes directory and its contents
                            cout << "Deleted directory: '" << items[index - 1] << "'.\n";
                        }
                    }
                    else {
                        cout << "Error: '" << items[index - 1] << "' does not exist.\n";
                    }
                }
                catch (const fs::filesystem_error& e) {
                    cout << "Error deleting: " << e.what() << endl;
                }
            }
            else {
                cout << "Invalid selection.\n";
            }
        }
        else if (command == "<") {
            // Previous Page
            if (page > 0) --page;
        }
        else if (command == ">") {
            // Next Page
            if ((page + 1) * pageSize < items.size()) ++page;
        }
        else if (command.substr(0, 1) == "#") {
            // Jump to Page
            int jumpPage;
            try {
                jumpPage = stoi(command.substr(1));
                if (jumpPage >= 0 && jumpPage * pageSize < items.size()) {
                    page = jumpPage;
                }
                else {
                    cout << "Invalid page number." << endl;
                }
            }
            catch (const invalid_argument&) {
                cout << "Invalid page number." << endl;
            }
        }
        else if (command == "-r" || command == "-R") {
            // Return Directory
            if (currentPath != "/") {
                currentPath = fs::path(currentPath).parent_path().string();
                page = 0;
            }
            else {
                cout << "Already at the root directory." << endl;
            }
        }
        else if (command == "-u" || command == "-U") {
            // Return to Home Directory
            currentPath = initialPath;
            page = 0;
        }
        else if (command.find("-n") != string::npos || command.find("-o") != string::npos) {
            // Open File/Folder
            int index = 0;
            string action = "";

            // Split command into parts by space
            istringstream stream(command);
            string part;
            vector<string> parts;

            while (getline(stream, part, ' ')) {
                parts.push_back(part);
            }

            // Ensure there are exactly two parts: index and action
            if (parts.size() == 2) {
                try {
                    index = stoi(parts[0]);  // Extract the index part
                    action = parts[1];       // Extract the action (-n or -o)

                    // Validate the index and action
                    if (index > 0 && index <= items.size()) {
                        string selectedPath = currentPath + "/" + items[index - 1];

                        if (action == "-n" || action == "n") {
                            // Navigate to folder inside terminal
                            if (fs::is_directory(selectedPath)) {
                                currentPath = selectedPath;
                                page = 0;
                            }
                            else {
                                cout << "Selected path is not a directory.\n";
                            }
                        }
                        else if (action == "-o" || action == "o") {
                            // Open file/folder outside terminal
                            openFileOrFolder(selectedPath);
                        }
                        else {
                            cout << "Invalid action. Use '-n' or '-o'.\n";
                        }
                    }
                    else {
                        cout << "Invalid index. Please select a valid item.\n";
                    }
                }
                catch (const invalid_argument&) {
                    cout << "Invalid index. Please enter a valid number.\n";
                }
                catch (const out_of_range&) {
                    cout << "Index out of range.\n";
                }
            }
            else {
                cout << "Invalid command format. Correct format: <index> -n or <index> -o.\n";
            }
        }
        else {
            cout << "Invalid command.\n";
        }
    }
    isUserBusy = false;
}

#elif  __linux__
//for linux
void navigateDirectory(const string& initialPath = "") {
    isUserBusy = true;
    string currentPath = initialPath.empty() ? getenv("HOME") : initialPath;
    if (currentPath.empty()) {
        return;  // Exit if the path couldn't be determined
    }

    int page = 0;
    const int pageSize = 15;
    bool isRunning = true;

    while (isRunning) {
        clearTerminal();

        vector<string> items;
        for (const auto& entry : fs::directory_iterator(currentPath)) {
            items.push_back(entry.path().filename().string());
        }

        displayPage(items, page, pageSize);

        cout << "Current Directory: " << currentPath << endl;
        cout << "\nCommands:\n";
        cout << "<< Create / Edit >>\n"
            << "- [c] Create new file/folder\n"
            << "- [e] Edit file\n"
            << "- [r <num>] Rename file/folder\n"
            << "- [d <num>] Delete file/folder\n\n";

        cout << "<< File / Folder >>\n"
            << "- [<num> -n] Open inside terminal\n"
            << "- [<num> -o] Open outside terminal\n\n";

        cout << "<< Page >>\n";
        if (page > 0) cout << "- [<] Previous Page\n";
        if ((page + 1) * pageSize < items.size()) cout << "- [>] Next Page\n";
        cout << "- [#<pageNum>] Jump to Page\n"
            << "- [-r] Return Directory\n"
            << "- [-u] Return to Home Directory\n\n";

        cout << "<< Change path / Exit >>\n"
            << "- [-d] Change Directory\n"
            << "- [-q] Quit\n\n";

        cout << "Enter command: ";
        string command;
        getline(cin, command);

        // Command Handling
        if (command == "-q" || command == "-Q") {
            isRunning = false;
        }
        else if (command == "-d") {
            // Change directory option
            cout << "Enter the new directory path: ";
            string newPath;
            getline(cin, newPath);

            if (fs::exists(newPath) && fs::is_directory(newPath)) {
                currentPath = newPath;  // Change the current path
                page = 0;  // Reset page number when changing the directory
            }
            else {
                cout << "Invalid directory. Please try again.\n";
            }
        }
        else if (command == "c") {
            // Create File/Folder
            cout << "Select type:\n"
                << "- [1] Code file\n"
                << "- [2] Note file\n"
                << "- [3] Create Folder\n"
                << "Enter choice: ";
            string fileType;
            getline(cin, fileType);

            if (fileType == "1") {
                string folderPath = currentPath;
                cout << "Enter the filename (including extension, e.g., 'example.cpp'): ";
                string filename;
                getline(cin, filename);

                clearTerminal();
                // Use currentPath to save in the current directory
                string filepath = currentPath + "/" + filename;

                // Detect language based on file extension
                string extension = filepath.substr(filepath.find_last_of('.') + 1);
                string buildCommand, runCommand;
                if (extension == "cpp") {
                    buildCommand = "g++ " + filepath + " -o " + currentPath + "/program";
                    runCommand = currentPath + "/program";
                }
                else if (extension == "c") {
                    buildCommand = "gcc " + filepath + " -o " + currentPath + "/program";
                    runCommand = currentPath + "/program";
                }
                else if (extension == "cs") {
                    buildCommand = "mcs " + filepath + " -out:" + currentPath + "/program.exe";
                    runCommand = "mono " + currentPath + "/program.exe";
                }
                else if (extension == "go") {
                    runCommand = "go run " + filepath;
                }
                else if (extension == "js") {
                    runCommand = "node " + filepath;
                }
                else if (extension == "py") {
                    runCommand = "python3 " + filepath;
                }
                else if (extension == "java") {
                    buildCommand = "javac " + filepath;
                    runCommand = "java -cp " + currentPath + " " + filename.substr(0, filename.find_last_of('.'));
                }
                else {
                    cout << "Unsupported file extension. Please use a supported language.\n";
                    return;
                }

                vector<string> codeLines;

                // Writing Code
                cout << "Start writing your code. Type 'END' or 'EXIT' on a blank line to finish.\n";
                string line;
                while (true) {
                    getline(cin, line);
                    if (line == "END" || line == "EXIT") break;
                    codeLines.push_back(line);
                }

                // Save Code to File in the current directory
                ofstream outFile(filepath, ios::trunc);
                if (outFile.is_open()) {
                    for (const auto& codeLine : codeLines) {
                        outFile << codeLine << endl;
                    }
                    outFile.close();
                    cout << "Code saved to " << filepath << endl;
                }
                else {
                    cout << "Error saving file.\n";
                    return;
                }

                // Display written content
                clearTerminal();
                cout << "\nYour code content:\n";
                for (const auto& codeLine : codeLines) {
                    cout << codeLine << endl;
                }

                // Interaction Menu
                while (true) {
                    clearTerminal();
                    // Display the current file content with line numbers
                    cout << "\nCurrent content of '" << filename << "':\n\n";
                    for (size_t i = 0; i < codeLines.size(); ++i) {
                        cout << i + 1 << ". " << codeLines[i] << endl;  // Display line number
                    }

                    cout << "\n\nOptions:\n"
                        << "[b] Build Code (C, C++, Java)\n"
                        << "[br] Build and Run (C, C++, Java)\n"
                        << "[r] Run Only (Python, pre-built binaries)\n"
                        << "[u] Update/Change Code\n"
                        << "[f] Finish\n"
                        << "Enter your choice: ";
                    string choice;
                    getline(cin, choice);
                    choice = toLower(choice);

                    if (choice == "b") {
                        string buildResult = buildCode(filepath);
                        cout << buildResult << endl;
                    }
                    else if (choice == "br") {
                        clearTerminal();
                        string buildAndRunResult = buildAndRunCode(filepath);
                        cout << buildAndRunResult << endl;
                        cout << "\n>> press enter to finish <<\n";
                        cin.get();  // Wait for user input to finish testing
                    }
                    else if (choice == "r") {
                        clearTerminal();
                        string runResult = runCode(filepath);
                        cout << runResult << endl;
                        cout << "\n>> press enter to finish <<\n";
                        cin.get();  // Wait for user input to finish testing
                    }
                    else if (choice == "u") {
                        // Update/Change Code (additional operations)
                        clearTerminal();
                        cout << "\nCurrent content of '" << filename << "':\n\n";
                        for (size_t i = 0; i < codeLines.size(); ++i) {
                            cout << i + 1 << ". " << codeLines[i] << endl;  // Display line number
                        }

                        cout << "\n\nDo you want to perform any operations on '" << filename << "'?\n"
                            << "[r] Rename file\n"
                            << "[s] Open file location\n"
                            << "[c] Continue writing\n"
                            << "[a] Add a line\n"
                            << "[d] Delete a line\n"
                            << "[o] Overwrite a line\n"
                            << "[f] Finish\n"
                            << "Enter your choice: ";
                        string updateChoice;
                        getline(cin, updateChoice);
                        updateChoice = toLower(updateChoice);

                        if (updateChoice == "r") {
                            // Rename file
                            cout << "Enter new filename (including extension): ";
                            string newFilename;
                            getline(cin, newFilename);
                            string newFilepath = folderPath + "/" + newFilename;
                            if (rename(filepath.c_str(), newFilepath.c_str()) == 0) {
                                cout << "File renamed to " << newFilename << endl;
                                filepath = newFilepath;
                                filename = newFilename;
                            }
                            else {
                                cout << "Failed to rename file.\n";
                            }
                        }
                        else if (updateChoice == "s") {
                            // Open file location
                            string openCommand = "explorer " + folderPath;  // Windows specific
                            system(openCommand.c_str());
                            cout << "Opening file location...\n";
                        }
                        else if (updateChoice == "c") {
                            // Continue writing code
                            cout << "\nContinue writing your code. Type 'END' or 'EXIT' on a blank line to finish.\n";
                            while (true) {
                                getline(cin, line);
                                if (line == "END" || line == "EXIT") break;
                                codeLines.push_back(line);
                            }

                            // Save the updated code to the file
                            ofstream outFile(filepath, ios::trunc);
                            if (outFile.is_open()) {
                                for (const auto& codeLine : codeLines) {
                                    outFile << codeLine << endl;
                                }
                                outFile.close();
                                cout << "Code updated and saved to " << filepath << endl;
                            }
                            else {
                                cout << "Error saving updated code.\n";
                            }
                        }
                        else if (updateChoice == "a") {
                            // Add a line
                            cout << "Choose an option:\n"
                                << "1. Add a new line at the end.\n"
                                << "2. Add a new line in between.\n"
                                << "Enter your choice: ";
                            string addOption;
                            getline(cin, addOption);

                            if (addOption == "1") {
                                // Add a new line at the end
                                cout << "Add a line of content: ";
                                string newLine;
                                getline(cin, newLine);
                                codeLines.push_back(newLine);

                                // Save the updated code to the file
                                ofstream outFile(filepath, ios::trunc);
                                if (outFile.is_open()) {
                                    for (const auto& codeLine : codeLines) {
                                        outFile << codeLine << endl;
                                    }
                                    outFile.close();
                                    cout << "Content added and saved to " << filepath << endl;
                                }
                                else {
                                    cout << "Error saving updated code.\n";
                                }

                            }
                            else if (addOption == "2") {
                                // Add a new line in between
                                cout << "Enter the line number to add below: ";
                                int lineNumber;
                                cin >> lineNumber;
                                cin.ignore(); // Clear newline from the input buffer

                                if (lineNumber < 1 || lineNumber > codeLines.size()) {
                                    cout << "Invalid line number. Operation canceled.\n";
                                }
                                else {
                                    cout << "Choose an option:\n"
                                        << "1. Add a single line.\n"
                                        << "2. Add multiple lines (type 'END' or 'EXIT' to finish).\n"
                                        << "Enter your choice: ";
                                    string inBetweenOption;
                                    getline(cin, inBetweenOption);

                                    if (inBetweenOption == "1") {
                                        // Add a single line
                                        cout << "Enter the content to add: ";
                                        string newLine;
                                        getline(cin, newLine);
                                        codeLines.insert(codeLines.begin() + lineNumber, newLine);
                                    }
                                    else if (inBetweenOption == "2") {
                                        // Add multiple lines
                                        clearTerminal();
                                        cout << "Enter lines to add (type 'END' or 'EXIT' on a blank line to finish):\n\n";
                                        vector<string> newLines;
                                        while (true) {
                                            string newLine;
                                            getline(cin, newLine);
                                            if (newLine == "END" || newLine == "EXIT") break;
                                            newLines.push_back(newLine);
                                        }
                                        codeLines.insert(codeLines.begin() + lineNumber, newLines.begin(), newLines.end());
                                    }
                                    else {
                                        cout << "Invalid option. Operation canceled.\n";
                                    }

                                    // Save the updated code to the file
                                    ofstream outFile(filepath, ios::trunc);
                                    if (outFile.is_open()) {
                                        for (const auto& codeLine : codeLines) {
                                            outFile << codeLine << endl;
                                        }
                                        outFile.close();
                                        cout << "Lines added and saved to " << filepath << endl;
                                    }
                                    else {
                                        cout << "Error saving updated code.\n";
                                    }
                                }
                            }
                            else {
                                cout << "Invalid choice. Operation canceled.\n";
                            }

                        }
                        else if (updateChoice == "d") {
                            // Delete a line
                            cout << "Enter the line number to delete: ";
                            size_t lineNumber;
                            cin >> lineNumber;
                            cin.ignore();
                            if (lineNumber > 0 && lineNumber <= codeLines.size()) {
                                codeLines.erase(codeLines.begin() + lineNumber - 1);
                                // Save the updated code to the file
                                ofstream outFile(filepath, ios::trunc);
                                if (outFile.is_open()) {
                                    for (const auto& codeLine : codeLines) {
                                        outFile << codeLine << endl;
                                    }
                                    outFile.close();
                                    cout << "Line deleted and file updated.\n";
                                }
                                else {
                                    cout << "Error saving updated file.\n";
                                }
                            }
                            else {
                                cout << "Invalid line number.\n";
                            }

                        }
                        else if (updateChoice == "o") {
                            // Overwrite a line
                            cout << "Enter the line number to overwrite: ";
                            size_t lineNumber;
                            cin >> lineNumber;
                            cin.ignore();
                            if (lineNumber > 0 && lineNumber <= codeLines.size()) {
                                cout << "Enter the new content for the line: ";
                                string newContent;
                                getline(cin, newContent);
                                codeLines[lineNumber - 1] = newContent;
                                // Save the updated code to the file
                                ofstream outFile(filepath, ios::trunc);
                                if (outFile.is_open()) {
                                    for (const auto& codeLine : codeLines) {
                                        outFile << codeLine << endl;
                                    }
                                    outFile.close();
                                    cout << "Line overwritten and file updated.\n";
                                }
                                else {
                                    cout << "Error saving updated file.\n";
                                }
                            }
                            else {
                                cout << "Invalid line number.\n";
                            }
                        }
                        else if (updateChoice == "f") {
                            // Finish and return
                            break;
                        }
                    }
                    else if (choice == "f") {
                        break;
                    }
                }
            }
            else if (fileType == "2") {
                cout << "Enter the filename for your note (without extension): ";
                string noteFileName;
                getline(cin, noteFileName);

                string filepath = currentPath + "/" + noteFileName + ".txt";  // Linux path separator

                clearTerminal();
                cout << endl << "Welcome to the terminal notepad!\n"
                    << "Start typing your notes. Type 'EXIT' on a new line to finish.\n\n";

                vector<string> notes;
                string line;

                // Input phase
                while (true) {
                    getline(cin, line);
                    if (toLower(line) == "exit") break;
                    notes.push_back(line);
                }

                // Editing phase
                while (true) {
                    clearTerminal();
                    cout << "\nCurrent Notes:\n";
                    for (size_t i = 0; i < notes.size(); ++i) {
                        cout << i + 1 << ": " << notes[i] << endl;
                    }

                    cout << "\nOptions:\n"
                        << "[c] Continue writing\n"
                        << "[a] Add a new line\n"
                        << "[e] Edit a line\n"
                        << "[d] Delete a line\n"
                        << "[s] Save and Exit\n"
                        << "[x] Exit without saving\n"
                        << "Enter your choice: ";

                    string choice;
                    getline(cin, choice);
                    choice = toLower(choice);

                    if (choice == "c") {  // Continue writing
                        clearTerminal();
                        cout << "\n>> Continue below. Type 'EXIT' on a new line to stop.\n\n";
                        for (const auto& existingLine : notes) {
                            cout << existingLine << endl;
                        }

                        while (true) {
                            getline(cin, line);
                            if (toLower(line) == "exit") break;
                            notes.push_back(line);
                        }
                    }
                    else if (choice == "e") {  // Edit a line
                        cout << "Enter the line number to edit: ";
                        size_t lineNumber;
                        cin >> lineNumber;
                        cin.ignore();

                        if (lineNumber > 0 && lineNumber <= notes.size()) {
                            cout << "Current content: " << notes[lineNumber - 1] << endl;
                            cout << "Enter new content: ";
                            getline(cin, notes[lineNumber - 1]);
                        }
                        else {
                            cout << "Invalid line number. Try again.\n";
                        }
                    }
                    else if (choice == "a") {  // Add a new line
                        cout << "Enter the new line content: ";
                        string newLine;
                        getline(cin, newLine);
                        notes.push_back(newLine);
                    }
                    else if (choice == "d") {  // Delete a line
                        cout << "Enter the line number to delete: ";
                        size_t lineNumber;
                        cin >> lineNumber;
                        cin.ignore();

                        if (lineNumber > 0 && lineNumber <= notes.size()) {
                            notes.erase(notes.begin() + lineNumber - 1);
                            cout << "Line deleted successfully.\n";
                        }
                        else {
                            cout << "Invalid line number. Try again.\n";
                        }
                    }
                    else if (choice == "s") {  // Save and Exit
                        ofstream outFile(filepath);
                        if (outFile.is_open()) {
                            for (const string& note : notes) {
                                outFile << note << endl;
                            }
                            outFile.close();
                            cout << "Notes saved successfully to " << filepath << endl;
                        }
                        else {
                            cerr << "Error opening file for writing.\n\n";
                        }
                        break;
                    }
                    else if (choice == "x") {  // Exit without saving
                        cout << "!! WARNING !! Notes were not saved.\n\n";
                        break;
                    }
                    else {
                        cout << "Invalid choice. Please try again.\n";
                    }
                }
            }
            else if (fileType == "3") {
                // Create Folder
                cout << "Enter the folder name: ";
                string folderName;
                getline(cin, folderName);

                string folderPath = currentPath + "/" + folderName;
                if (fs::create_directory(folderPath)) {
                    cout << "Folder created: " << folderPath << endl;
                }
                else {
                    cout << "Error creating folder.\n";
                }
            }
            else {
                cout << "Invalid choice. Please try again.\n";
            }

        }
        else if (command == "e") {
            // Edit File Option
            cout << "Select option to perform:\n"
                << "- [1] Code file\n"
                << "- [2] Note file\n"
                << "Enter choice: ";

            string editOption;
            getline(cin, editOption);

            if (editOption == "1") {
                // List all files in the current directory to select a code file
                clearTerminal();
                vector<string> codeFiles;
                for (const auto& entry : fs::directory_iterator(currentPath)) {
                    if (entry.is_regular_file()) {
                        codeFiles.push_back(entry.path().filename().string());
                    }
                }

                if (codeFiles.empty()) {
                    cout << "No code files found in the current directory.\n";
                    return;
                }

                // Display available code files
                cout << "\nAvailable code files:\n";
                for (int i = 0; i < codeFiles.size(); ++i) {
                    cout << i + 1 << ". " << codeFiles[i] << endl;
                }

                // Let the user select the file they want to edit
                cout << "\nEnter the number of the file you want to edit: ";
                string input;
                getline(cin, input);

                try {
                    int fileIndex = stoi(input) - 1;
                    if (fileIndex >= 0 && fileIndex < codeFiles.size()) {
                        string selectedFilename = codeFiles[fileIndex];
                        string filepath = currentPath + "/" + selectedFilename;

                        // Check if the selected file is valid
                        if (!fs::exists(filepath) || fs::is_directory(filepath)) {
                            cout << "The selected path is not a valid file.\n";
                            return;
                        }

                        // File management menu
                        bool editing = true;
                        while (editing) {
                            clearTerminal(); // Ensure screen refresh
                            cout << "============================\n"
                                << "File selected: " << selectedFilename << endl
                                << "============================\n"
                                << "Options for the selected file:\n"
                                << "1. View File\n"
                                << "2. Edit File\n"
                                << "3. Build/Run\n"
                                << "4. Delete File\n"
                                << "5. Rename File\n"
                                << "6. Save & Exit\n"
                                << "7. Discard Changes & Exit\n";

                            cout << "\nEnter your choice: ";
                            string fileOption;
                            getline(cin, fileOption);

                            if (fileOption == "1") {
                                displayNotepad(filepath);
                            }
                            else if (fileOption == "2") {
                                editAndWriteCode(filepath);
                            }
                            else if (fileOption == "3") {
                                clearTerminal();
                                cout << "\nSelect an option for running code:\n"
                                    << "1. Build and Run (C, C++, Java)\n"
                                    << "2. Run Only (Python, pre-built binaries)\n"
                                    << "Your Option: ";

                                string runOption;
                                getline(cin, runOption);

                                if (runOption == "1") {
                                    string buildResult = buildAndRunCode(filepath);
                                    cout << buildResult << endl;
                                    cout << "\n>> Press enter to finish <<\n";
                                    cin.get();
                                }
                                else if (runOption == "2") {
                                    clearTerminal();
                                    string runResult = runCode(filepath);
                                    cout << runResult << endl;
                                    cout << "\n>> Press enter to finish <<\n";
                                    cin.get();
                                }
                                else {
                                    cout << "Invalid option. Returning to file menu.\n";
                                    pauseConsole();
                                }
                            }
                            else if (fileOption == "4") {
                                deleteFile(filepath);
                                cout << "File deleted successfully.\n";
                                codeFiles.erase(codeFiles.begin() + fileIndex);
                                break; // Exit menu after deleting
                            }
                            else if (fileOption == "5") {
                                renameCodeFile(filepath);
                            }
                            else if (fileOption == "6") {
                                cout << "Changes saved to file: " << selectedFilename << endl;
                                editing = false;
                            }
                            else if (fileOption == "7") {
                                cout << "Changes discarded. Exiting edit mode.\n";
                                editing = false;
                            }
                            else {
                                cout << "Invalid choice. Please try again.\n";
                            }
                        }
                    }
                    else {
                        cout << "Invalid file number. Please try again.\n";
                    }
                }
                catch (...) {
                    cout << "Invalid input. Please enter a valid file number.\n";
                }
            }
            else if (editOption == "2") {
                // Edit note file option (Linux version)
                clearTerminal();
                cout << "Select a note file to edit:\n";
                vector<string> noteFiles;
                for (const auto& entry : fs::directory_iterator(currentPath)) {
                    if (entry.is_regular_file() && entry.path().extension() == ".txt") {
                        noteFiles.push_back(entry.path().filename().string());
                    }
                }

                if (noteFiles.empty()) {
                    cout << "No note files found in the current directory.\n";
                    return;
                }

                const int pageSize = 15; // Number of files per page
                int currentPage = 0;
                bool isRunning = true;

                while (isRunning) {
                    system("clear");

                    // Calculate page bounds
                    int totalFiles = noteFiles.size();
                    int totalPages = (totalFiles + pageSize - 1) / pageSize; // Round up for total pages
                    int startIdx = currentPage * pageSize;
                    int endIdx = min(startIdx + pageSize, totalFiles);

                    // Display files for the current page
                    cout << "\nAvailable note files (Page " << currentPage + 1 << " of " << totalPages << "):\n";
                    for (int i = startIdx; i < endIdx; ++i) {
                        cout << i + 1 << ". " << noteFiles[i] << endl;
                    }

                    // Navigation options
                    cout << "\nNavigation options:\n";
                    if (currentPage > 0) cout << "[<] Previous Page\n";
                    if (currentPage < totalPages - 1) cout << "[>] Next Page\n";
                    cout << "[#] Jump to Page\n"
                        << "[q] Return\n"
                        << "\nEnter your choice or select a file number: ";

                    string input;
                    getline(cin, input);

                    if (input == "<" && currentPage > 0) {
                        --currentPage;
                    }
                    else if (input == ">" && currentPage < totalPages - 1) {
                        ++currentPage;
                    }
                    else if (input == "#") {
                        cout << "Enter page number: ";
                        string pageInput;
                        getline(cin, pageInput);
                        try {
                            int targetPage = stoi(pageInput) - 1;
                            if (targetPage >= 0 && targetPage < totalPages) {
                                currentPage = targetPage;
                            }
                            else {
                                cout << "Invalid page number. Please try again.\n";
                                pauseConsole();
                            }
                        }
                        catch (...) {
                            cout << "Invalid input format. Enter a number to jump to a page.\n";
                            pauseConsole();
                        }
                    }
                    else if (input == "q") {
                        break;
                    }
                    else {
                        try {
                            int fileIndex = stoi(input) - 1;
                            if (fileIndex >= 0 && fileIndex < noteFiles.size()) {
                                string selectedFilename = noteFiles[fileIndex];
                                string fileToEdit = currentPath + "/" + selectedFilename;

                                // Check if the selected file is valid
                                if (!fs::exists(fileToEdit) || fs::is_directory(fileToEdit)) {
                                    cout << "The selected path is not a valid file.\n";
                                    return;
                                }

                                // Edit the note file
                                ifstream inFile(fileToEdit);
                                vector<string> notes;
                                string line;

                                // Read the existing file content into a vector
                                if (inFile.is_open()) {
                                    while (getline(inFile, line)) {
                                        notes.push_back(line);
                                    }
                                    inFile.close();
                                }
                                else {
                                    cerr << "Error opening file for reading: " << fileToEdit << endl;
                                    return;
                                }

                                while (true) {
                                    system("clear");
                                    cout << "\nCurrent Notes in " << fileToEdit << ":\n";
                                    for (size_t i = 0; i < notes.size(); ++i) {
                                        cout << i + 1 << ": " << notes[i] << endl;
                                    }

                                    cout << "\nOptions:\n"
                                        << "[c] Continue\n"
                                        << "[a] Add a new line\n"
                                        << "[e] Edit line\n"
                                        << "[d] Delete a line\n"
                                        << "[s] Save and Exit\n"
                                        << "[x] Exit without saving\n"
                                        << "\nEnter your choice: ";

                                    string choice;
                                    getline(cin, choice);
                                    choice = toLower(choice);

                                    if (choice == "c") {  // Continue writing
                                        cout << "\n>> Continue writing (append at the end or insert at a specific position):\n\n";
                                        cout << "Choose action:\n"
                                            << "[1] Append at the end\n"
                                            << "[2] Insert at a specific position\n";
                                        cout << "Enter Option: ";
                                        string action;
                                        getline(cin, action);

                                        if (action == "1") {  // Append at the end
                                            cout << "\nStart typing your new content (type 'EXIT' on a new line to stop):\n";
                                            for (size_t i = 0; i < notes.size(); ++i) {
                                                cout << notes[i] << endl;
                                            }
                                            while (true) {
                                                getline(cin, line);
                                                if (toLower(line) == "exit") {
                                                    break;
                                                }
                                                notes.push_back(line);
                                            }

                                            // Display updated notes
                                            system("clear");
                                            cout << "\nUpdated Notes in " << fileToEdit << ":\n";
                                            for (size_t i = 0; i < notes.size(); ++i) {
                                                cout << i + 1 << ": " << notes[i] << endl;
                                            }
                                        }
                                        else if (action == "2") {  // Insert at specific position
                                            cout << "Enter the line to insert.";
                                            size_t pos;
                                            cin >> pos;
                                            cin.ignore();

                                            if (pos >= 1 && pos <= notes.size() + 1) {
                                                cout << "Enter the content to insert (type 'EXIT' to finish):\n";
                                                vector<string> newLines;
                                                while (true) {
                                                    getline(cin, line);
                                                    if (toLower(line) == "exit") {
                                                        break;
                                                    }
                                                    newLines.push_back(line);
                                                }
                                                notes.insert(notes.begin() + pos - 1, newLines.begin(), newLines.end());
                                            }
                                            else {
                                                cout << "Invalid position. Try again.\n";
                                            }
                                        }
                                        else {
                                            cout << "Invalid action. Returning.\n";
                                        }
                                    }
                                    else if (choice == "e") {  // Edit a line
                                        cout << "Choose action:\n"
                                            << "[1] Insert lines below\n"
                                            << "[2] Overwrite line\n";
                                        cout << "Enter Option: ";
                                        string action;
                                        getline(cin, action);

                                        if (action == "1") {  // Insert lines below
                                            cout << "Enter the line number to insert below: ";
                                            size_t lineNumber;
                                            cin >> lineNumber;
                                            cin.ignore();

                                            if (lineNumber > 0 && lineNumber <= notes.size()) {
                                                cout << "Enter the content to insert: ";
                                                getline(cin, line);
                                                notes.insert(notes.begin() + lineNumber, line);
                                            }
                                            else {
                                                cout << "Invalid line number. Try again.\n";
                                            }
                                        }
                                        else if (action == "2") {  // Overwrite line
                                            cout << "Enter the line number to overwrite: ";
                                            size_t lineNumber;
                                            cin >> lineNumber;
                                            cin.ignore();

                                            if (lineNumber > 0 && lineNumber <= notes.size()) {
                                                cout << "Current content: " << notes[lineNumber - 1] << endl;
                                                cout << "Enter new content: ";
                                                getline(cin, notes[lineNumber - 1]);
                                            }
                                            else {
                                                cout << "Invalid line number. Try again.\n";
                                            }
                                        }
                                        else {
                                            cout << "Invalid action. Returning.\n";
                                        }
                                    }
                                    else if (choice == "a") {  // Add a new line
                                        cout << "Enter the new line content: ";
                                        string newLine;
                                        getline(cin, newLine);
                                        notes.push_back(newLine);
                                    }
                                    else if (choice == "d") {  // Delete a line
                                        cout << "Enter the line number to delete: ";
                                        size_t lineNumber;
                                        cin >> lineNumber;
                                        cin.ignore();

                                        if (lineNumber > 0 && lineNumber <= notes.size()) {
                                            notes.erase(notes.begin() + lineNumber - 1);
                                            cout << "Line deleted successfully.\n";
                                        }
                                        else {
                                            cout << "Invalid line number. Try again.\n";
                                        }
                                    }
                                    else if (choice == "s") {  // Save and Exit
                                        ofstream outFile(fileToEdit, ios::trunc);
                                        if (outFile.is_open()) {
                                            for (const string& note : notes) {
                                                outFile << note << endl;
                                            }
                                            outFile.close();
                                            cout << "Notes saved successfully to " << fileToEdit << endl;
                                        }
                                        else {
                                            cerr << "Error opening file for writing.\n\n";
                                        }
                                        break;
                                    }
                                    else if (choice == "x") {  // Exit without saving
                                        cout << "!! WARNING !! Changes were not saved.\n\n";
                                        break;
                                    }
                                    else {
                                        cout << "Invalid choice. Please try again.\n";
                                    }
                                }
                            }
                            else {
                                cout << "Invalid file number. Please try again.\n";
                            }
                        }
                        catch (...) {
                            cout << "Invalid input. Please enter a valid file number.\n";
                        }
                    }
                }
            }
            else {
                cout << "Invalid option. Operation canceled.\n";
            }
        }
        else if (command.substr(0, 2) == "r ") {
            // Rename File/Folder
            int index;
            if (sscanf(command.c_str(), "r %d", &index) == 1 && index > 0 && index <= items.size()) {
                string oldPath = currentPath + "/" + items[index - 1];
                cout << "Enter new name for '" << items[index - 1] << "': ";
                string newName;
                getline(cin, newName);
                string newPath = currentPath + "/" + newName;

                if (rename(oldPath.c_str(), newPath.c_str()) == 0) {
                    cout << "Renamed successfully to '" << newName << "'.\n";
                }
                else {
                    cout << "Error renaming file/folder.\n";
                }
            }
            else {
                cout << "Invalid selection.\n";
            }

        }
        else if (command.substr(0, 2) == "d ") {
            // Delete File/Folder
            int index;
            if (sscanf(command.c_str(), "d %d", &index) == 1 && index > 0 && index <= items.size()) {
                string targetPath = currentPath + "/" + items[index - 1];

                try {
                    // Check if the target path exists
                    if (fs::exists(targetPath)) {
                        // If it's a file, remove it
                        if (fs::is_regular_file(targetPath)) {
                            fs::remove(targetPath);
                            cout << "Deleted file: '" << items[index - 1] << "'.\n";
                        }
                        // If it's a directory, remove it (empty or non-empty)
                        else if (fs::is_directory(targetPath)) {
                            fs::remove_all(targetPath);  // Removes directory and its contents
                            cout << "Deleted directory: '" << items[index - 1] << "'.\n";
                        }
                    }
                    else {
                        cout << "Error: '" << items[index - 1] << "' does not exist.\n";
                    }
                }
                catch (const fs::filesystem_error& e) {
                    cout << "Error deleting: " << e.what() << endl;
                }
            }
            else {
                cout << "Invalid selection.\n";
            }
        }
        else if (command == "<") {
            // Previous Page
            if (page > 0) --page;
        }
        else if (command == ">") {
            // Next Page
            if ((page + 1) * pageSize < items.size()) ++page;
        }
        else if (command.substr(0, 1) == "#") {
            // Jump to Page
            int jumpPage;
            try {
                jumpPage = stoi(command.substr(1));
                if (jumpPage >= 0 && jumpPage * pageSize < items.size()) {
                    page = jumpPage;
                }
                else {
                    cout << "Invalid page number." << endl;
                }
            }
            catch (const invalid_argument&) {
                cout << "Invalid page number." << endl;
            }
        }
        else if (command == "-r" || command == "-R") {
            // Return Directory
            if (currentPath != "/") {
                currentPath = fs::path(currentPath).parent_path().string();
                page = 0;
            }
            else {
                cout << "Already at the root directory." << endl;
            }
        }
        else if (command == "-u" || command == "-U") {
            // Return to Home Directory
            currentPath = initialPath;
            page = 0;
        }
        else if (command.find("-n") != string::npos || command.find("-o") != string::npos) {
            // Open File/Folder
            int index = 0;
            string action = "";

            // Split command into parts by space
            istringstream stream(command);
            string part;
            vector<string> parts;

            while (getline(stream, part, ' ')) {
                parts.push_back(part);
            }

            // Ensure there are exactly two parts: index and action
            if (parts.size() == 2) {
                try {
                    index = stoi(parts[0]);  // Extract the index part
                    action = parts[1];       // Extract the action (-n or -o)

                    // Validate the index and action
                    if (index > 0 && index <= items.size()) {
                        string selectedPath = currentPath + "/" + items[index - 1];

                        if (action == "-n" || action == "n") {
                            // Navigate to folder inside terminal
                            if (fs::is_directory(selectedPath)) {
                                currentPath = selectedPath;
                                page = 0;
                            }
                            else {
                                cout << "Selected path is not a directory.\n";
                            }
                        }
                        else if (action == "-o" || action == "o") {
                            // Open file/folder outside terminal
                            openFileOrFolder(selectedPath);
                        }
                        else {
                            cout << "Invalid action. Use '-n' or '-o'.\n";
                        }
                    }
                    else {
                        cout << "Invalid index. Please select a valid item.\n";
                    }
                }
                catch (const invalid_argument&) {
                    cout << "Invalid index. Please enter a valid number.\n";
                }
                catch (const out_of_range&) {
                    cout << "Index out of range.\n";
                }
            }
            else {
                cout << "Invalid command format. Correct format: <index> -n or <index> -o.\n";
            }
        }
        else {
            cout << "Invalid command.\n";
        }
    }
    isUserBusy = false;
}

#elif __APPLE__
// For Apple
string getDesktopPath() {
    char* homeDir = getenv("HOME");
    if (homeDir) {
        return string(homeDir) + "/Desktop";
    }
    else {
        cerr << "Error: Unable to retrieve the user's home path." << endl;
        return "";
    }
}

void navigateDirectory(const string& initialPath = "") {
    isUserBusy = true;
    string currentPath = initialPath.empty() ? getDesktopPath() : initialPath;
    if (currentPath.empty()) {
        return;
    }

    int page = 0;
    const int pageSize = 10;
    bool isRunning = true;

    while (isRunning) {
        clearTerminal();

        ector<string> items;
        for (const auto& entry : fs::directory_iterator(currentPath)) {
            items.push_back(entry.path().filename().string());
        }

        displayPage(items, page, pageSize);

        cout << "Current Directory: " << currentPath << endl;
        cout << "Commands: [<num> -n] Open folder in terminal | [<num> -o] Open file/folder outside terminal |\n";
        cout << "| '<' Previous Page | '>' Next Page | [#<pageNum>] Jump to Page | '-u' Return Home | '-r' Return | '-q' Quit" << endl;
        cout << "Enter command: ";
        string command;
        getline(cin, command);

        if (command == "-q" || command == "-Q") {
            isRunning = false;
        }
        else if (command == "<") {
            if (page > 0) --page;
        }
        else if (command == ">") {
            if ((page + 1) * pageSize < items.size()) ++page;
        }
        else if (command == "-u" || command == "-U") {
            currentPath = initialPath;
            page = 0;
        }
        else if (command.substr(0, 1) == "#") {
            int jumpPage;
            try {
                jumpPage = stoi(command.substr(1));
                if (jumpPage >= 0 && jumpPage * pageSize < items.size()) {
                    page = jumpPage;
                }
                else {
                    cout << "Invalid page number." << endl;
                }
            }
            catch (const invalid_argument&) {
                cout << "Invalid page number." << endl;
            }
        }
        else if (command == "-c" || command == "-C") {
            clearTerminal();
        }
        else if (command == "-R" || command == "-r") {
            if (currentPath != "/") {
                currentPath = fs::path(currentPath).parent_path().string();
                page = 0;
            }
            else {
                cout << "Already at the root directory." << endl;
            }
        }
        else {
            int index;
            char action;
            if (sscanf(command.c_str(), "%d -%c", &index, &action) == 2) {
                if (index > 0 && index <= items.size()) {
                    string selectedPath = currentPath + "/" + items[index - 1];
                    if (action == 'n' || action == 'N') {
                        if (fs::is_directory(selectedPath)) {
                            currentPath = selectedPath;
                            page = 0;
                        }
                        else {
                            cout << "Not a directory." << endl;
                        }
                    }
                    else if (action == 'o' || action == 'O') {
                        openFileOrFolder(selectedPath);
                    }
                }
                else {
                    cout << "Invalid selection." << endl;
                }
            }
            else {
                cout << "Invalid command format." << endl;
            }
        }
    }
    isUserBusy = false;
}

#else
cerr << "Unsupported operating system." << endl;
#endif

// Utility function to remove punctuation and special characters, and also ignore possessive suffixes like "'s"
string preprocessMessage(const string& message) {
    string cleanedMessage;
    string word;
    bool inPossessive = false;

    for (char c : message) {
        if (isalnum(c) || isspace(c)) {
            if (c == '\'') {
                inPossessive = true;
            }
            else if (c == 's' && inPossessive) {
                inPossessive = false;
                continue;
            }
            else {
                if (inPossessive) {
                    inPossessive = false;
                }
                cleanedMessage += tolower(c);
            }
        }
    }
    return cleanedMessage;
}

// Spell check function (for demonstration, it currently does nothing)
string spellCheck(const string& userMessage) {
    // Implement spell checking logic here
    return userMessage;
}

// Fuzzy match function
string fuzzyMatch(const string& userMessage, const vector<string>& keywords) {
    string cleanedMessage = preprocessMessage(userMessage);

    // Iterate through the keywords
    for (const string& keyword : keywords) {
        string cleanedKeyword = preprocessMessage(keyword);

        if (cleanedMessage == cleanedKeyword) {
            return keyword;
        }
    }
    return "";
}

// Function to generate a response based on user input
string getBotResponse(const string& userMessage, const string& userName) {
    if (isMathExpression(userMessage)) {
        string cleanedExpression = removeSpaces(userMessage);
        string result = evaluateExpression(cleanedExpression);
        if (result.empty()) return "Error evaluating expression.";
        return "The result is: " + result;
    }

    string lowerMessage = toLower(userMessage);
    if (lowerMessage[0] == '>' || lowerMessage[0] == '<') {
        // Call the function and get the result
        string compareresult = generateNumberWithComparison(lowerMessage);
        return compareresult;  // Return the generated result as the bot's response
    }


    if (lowerMessage.find("cal") != string::npos || lowerMessage.find("ncal") != string::npos) {
        // Remove the command part ("cal" or "ncal")
        size_t commandEnd = lowerMessage.find(' ');
        string dateInput;

        if (commandEnd != string::npos) {
            dateInput = lowerMessage.substr(commandEnd + 1);
            dateInput = trim(dateInput);
        }

        if (dateInput.empty()) {
            time_t now = time(0);
            tm* ltm = localtime(&now);

            int month = ltm->tm_mon + 1;
            int day = ltm->tm_mday;
            int year = 1900 + ltm->tm_year;

            string monthName = months[month - 1];
            drawCalendar(monthName, year, day);  // Show the calendar for the current month
            return "Current month's calendar displayed.";
        }

        // Handle full-year input (4-digit year)
        if (dateInput.length() == 4 && all_of(dateInput.begin(), dateInput.end(), ::isdigit)) {
            int year = stoi(dateInput);
            drawYearCalendar(year);  // Show the calendar for the entire year
            return "Calendar for the year displayed.";
        }

        // Handle specific month and year input (e.g., "5 2024")
        if (isValidMonthYearInput(dateInput)) {
            drawSpecificCalendar(dateInput);  // Show the calendar for the specified month/year
            return "Calendar for the specified date displayed.";
        }
        else {
            return "Invalid input format. Please enter the command as 'cal month year' (e.g., 'cal 5 2024') or a 4-digit year (e.g., 'cal 2024').";
        }
    }


    //spell check
    string correctedMessage = spellCheck(toLower(userMessage));
    //add this into the code to call (matchedKeyword = fuzzyMatch(correctedMessage, "[call function]")) != ""

    //GPT
    vector<string> ChatGPT = { "chatgpt", "api", "gpt", "apikey", "api key" };

    //Get Version
    vector<string> getVersion = { "version", "current version", "build version", "what is your version", "build number", "current build" };

    //user guide
    vector<string> helpGuild = {
        "help",
        "guide",
        "support",
        "how to use",
        "how does this work",
        "help me",
        "assistance",
        "instructions",
        "manual",
        "what are the features",
        "need help",
        "how do I use this",
        "how do I start",
        "explain",
        "show me how",
        "tutorial",
        "user guide",
        "user help",

    };
    vector<string> contactInfo = {
        "contact",
        "contact info",
        "contact information",
        "how to contact",
        "who to contact",
        "help contact",
        "contact support",
        "reach out",
        "contact details",
        "get in touch",
        "support contact",
        "who can I contact",
        "how do I reach you",
        "where to contact",
        "contact help",
        "customer support",
        "contact number",
        "email address",
        "bot support",
        "contact us"
    };


    //greeting
    vector<string> greetings = {
        "hello", "hi", "hey", "Hello", "Hi", "Hey",
        "greetings", "good day",
        "what's up", "howdy", "yo", "hi there", "hey there",
        "hello there", "hey there", "hiya", "hello there", "how's it going",
        "how's it going", "what's going on", "what's new", "hey there!",
        "g'day", "what's up?",
        "hey buddy", "hi buddy", "hello friend", "hey friend",
        "hey man", "hello there!", "hi there!", "hey there!"
    };
    //weather talk
    vector<string> weatherlookifClear = {
        "clear sky", "sunny", "beautiful day", "no clouds", "clear weather",
        "blue skies", "nice day", "perfect weather", "good weather", "great day",
        "looks good outside", "no rain", "bright sky", "shine",
        "happy weather", "fantastic day", "clear out", "let's go outside",
        "what a lovely day", "gorgeous day", "sky looks nice", "a sunny day",
        "weather is perfect", "sun is shining", "no overcast", "sky is clear", "yep, no clouds",
        "yep, clear sky"
    };
    vector<string> weatherlookCouldy = {
        "cloudy today",
        "it's cloudy",
        "looks cloudy",
        "cloudy outside",
        "cloudy weather",
        "sky looks cloudy",
        "clouds everywhere",
        "so cloudy",
        "cloud cover",
        "overcast",
        "gloomy day",
        "the clouds are heavy",
        "it's going to rain",
        "feels like rain",
        "gray sky",
        "no sun today",
        "cloudy vibes",
        "clouds blocking the sun",
        "can't see the sky",
        "dark clouds",
        "big clouds",
        "cloudy afternoon",
        "clouds rolled in"
    };
    vector<string> weatherNotGood = {
        "stormy",
        "heavy rain",
        "it's raining",
        "rainy day",
        "bad weather",
        "thunderstorm",
        "windy and cold",
        "rainstorm",
        "raining hard",
        "storm",
        "flood warning",
        "lightning",
        "terrible weather",
        "hailstorm",
        "it's pouring",
        "downpour",
        "windstorm",
        "weather is awful",
        "raining cats and dogs",
        "weather looks bad",
        "looks like a storm",
        "rough weather",
        "bad day for outdoors",
        "rain all day",
        "cloudburst",
        "it's wet",
        "drizzle",
        "bad weather ahead",
        "gloomy weather",
        "it's nasty out",
        "foul weather",
        "it's wild outside",
        "storms coming",
        "rain clouds",
        "windy",
        "cold rain",
        "storm brewing",
        "no good weather",
        "weather is rough",
        "dark clouds and wind",
        "dangerous weather",
        "storm approaching",
        "weather isn't good",
        "rain"
    };
    vector<string> snowyOrWinter = {
        "snowing",
        "it's snowing",
        "snowstorm",
        "heavy snow",
        "blizzard",
        "cold day",
        "freezing",
        "icy outside",
        "frosty morning",
        "snow outside",
        "snowfall",
        "winter day",
        "chilly weather",
        "cold weather",
        "it's freezing",
        "cold and snowy",
        "snow everywhere",
        "icy roads",
        "snow day",
        "bitter cold",
        "winter storm",
        "frozen",
        "snow on the ground",
        "white out",
        "snow and ice",
        "snow clouds",
        "snowflakes falling",
        "snow flurries",
        "winter is here",
        "freezing cold",
        "snow covered",
        "temperature is freezing",
        "snow piling up",
        "frigid",
        "cold snap",
        "arctic blast",
        "snowy weather",
        "frostbite weather",
        "snow keeps falling",
        "icy wind",
        "bone-chilling cold",
        "it's cold",
        "snow"
    };

    //greeting
    vector<string> sayHiVariations = {
        "say hi", "say hello", "say hi to everyone", "say hello to everyone",
        "say hi to my friend", "say hello to my friend", "greet everyone",
        "give a hello to everyone", "say hello to all", "say hi to all",
        "send greetings to everyone", "send a hello to everyone", "say hi to my pals",
        "say hello to my pals", "give a shoutout to everyone", "give a hi to my friends",
        "give a hello to my friends", "send a hi to everyone", "send a hello to all",
        "send greetings to all", "say hello to the group", "say hi to the group",
        "greet my friends", "greet the group", "acknowledge everyone",
        "acknowledge my friends", "give a warm hello to everyone", "give a warm hi to everyone",
        "say hi to my colleagues", "say hello to my colleagues", "send a hello to my colleagues",
        "send a hi to my colleagues", "say hello to my team", "say hi to my team",
        "send a greeting to my team", "send a hi to my team"
    };
    vector<string> followGreeting = {
        "Nice to meet you",
        "Pleased to meet you",
        "Great to meet you",
        "Good to meet you",
        "Pleasure to meet you",
        "Nice to meet you too",
        "Nice meeting you",
        "It's a pleasure to meet you",
        "Glad to meet you",
        "Happy to meet you",
        "Nice seeing you",
        "Lovely to meet you",
        "It's nice to finally meet you",
        "Nice to see you",
        "Glad we finally met",
        "I'm happy to meet you",
        "Wonderful to meet you",
        "Nice to make your acquaintance",
        "It's been a pleasure",
        "Good finally meeting you",
        "Thrilled to meet you",
        "So happy to meet you",
        "Excited to meet you",
        "Awesome to meet you",
        "Nice crossing paths with you",
        "Nice to finally meet in person",
        "Delighted to meet you",
        "Very nice to meet you",
        "Happy we could meet",
        "Nice to connect with you",
        "Nice to have met you",
        "Glad to be acquainted with you"
    };
    //early morning
    vector<string> wakeEarly = {
        "Yeah, I didn't sleep well today.",
        "I had a restless night, so I'm up early.",
        "I couldn't get a good night's sleep, so I woke up early.",
        "I'm just an early riser today. Didn't sleep much.",
        "Had a rough night's sleep, so I ended up waking up early.",
        "I didn't get much rest last night, so here I am, up early.",
        "Woke up early because I couldn't sleep well.",
        "I guess I'm up early because I didn't sleep deeply.",
        "I had a tough time falling asleep, so I'm awake early.",
        "I didn't sleep very well, so I'm starting my day early.",
        "I was tossing and turning all night, so I woke up early.",
        "My sleep wasn't great, so I decided to get up early.",
        "I was restless during the night, so I ended up waking up early.",
        "I'm up early because I couldn't get a good night's sleep.",
        "Had a bit of insomnia last night, so I'm up early.",
        "Didn't sleep well, so I thought I'd start the day early.",
        "I woke up early because my sleep was interrupted.",
        "I had a lot on my mind, and it affected my sleep.",
        "I didn't get the rest I needed, so I'm up early.",
        "didn't sleep well",
        "did't sleep",
        "still working",
        "still work",
        "got a nightmare"
    };
    vector<string> dontWorry = {
        "It's okay, I'm fine.",
        "Don't worry about it, I'm just up early.",
        "No need to worry, I'm doing alright.",
        "It's nothing, just an early start today.",
        "Don't stress, I'm good. Just woke up early.",
        "Everything's okay, just had an early wake-up.",
        "I'm alright, just woke up sooner than usual.",
        "No worries, I'm just starting my day early.",
        "It's all good, I'm just up early today.",
        "Everything's fine, just had an early morning.",
        "I'm perfectly fine, just an early riser today.",
        "It's okay, nothing to worry about.",
        "I'm doing well, just got up early.",
        "Don't worry about it, I'm good to go.",
        "All is well, just woke up earlier than usual.",
        "I'm okay, just up a bit earlier today.",
        "Everything's under control, just an early morning for me.",
        "It's nothing to worry about, just an early wake-up.",
        "I'm alright, just had a bit of an early start today.",
        "No need to be concerned, I'm fine.",
        "I'm good, just couldn't sleep.",
        "No need to worry, I've got it under control.",
        "I'm alright, just an unexpected early morning.",
        "It's cool, I'm just up earlier than usual.",
        "Nothing to worry about, just up a bit earlier.",
        "I'm fine, just had an early start to the day.",
        "Don't worry, just woke up on my own a bit earlier today.",
        "It's all good, I'm just up a little earlier than usual.",
        "I'm totally fine, just an early riser today.",
        "It's fine, just an early morning, no big deal."
    };

    //greeting base on time
    vector<string> morningGreetings = {
        "morning", "good morning", "goodmorning", "Good Morning", "Goodmorning",
        "GoodMorning", "good Morning", "morning everyone", "good morning everyone",
        "good morning to all", "morning all", "good day", "good day everyone",
        "morning folks", "morning guys", "morning team", "good morning team",
        "good morning folks", "hello morning", "hello good morning", "hello goodmorning",
        "hello good morning everyone", "hey good morning", "hey goodmorning",
        "hey good morning everyone", "hey morning", "hey morning everyone",
        "good morning everyone!", "good morning everyone! How are you?", "good morning all",
        "morning all", "morning everyone! How are you today?", "morning everyone! How's it going?",
        "good morning everyone! How are you?", "hello and good morning", "hey there, good morning",
        "hey there, goodmorning", "hi, good morning", "hi, goodmorning"
    };

    //morning reply
    vector<string> wakeUp = {
        "Good morning, I'm up!", "Thanks for waking me up!", "I'm awake, just needed a little push!",
        "Alright, I'm up now.", "I'm up, thanks!", "I'm awake, just a little sleepy.",
        "Good morning, I'm ready to start the day.", "I'm up, thanks for checking on me!",
        "Okay, okay, I'm awake now!", "Alright, I'm getting out of bed!", "Thanks, I'm up now!",
        "I'm up!", "Awake!", "I'm up, promise!", "Up and ready!", "I'm awake, let's go!" ,"I'm awake"
    };

    vector<string> alreadywakeUp = {
        "I've been awake for a while.", "Already up and going!", "I woke up a while ago.",
        "I'm already awake, but thanks!", "I'm up, and ready to go!", "I've been up, just taking my time.",
        "Already ahead of you!", "I was up before you even texted!", "Been awake, just chilling.",
        "I'm up, just not ready to move yet.", "I'm already awake, just needed a reminder to get started.",
        "Up already!", "Been up!", "I'm awake", "Already moving!", "Up and at it!", "Already up", "it's almost noon"
    };
    vector<string> lazyWake = {
        "Just five more minutes, please!", "I don't wanna get up yet.", "Still feeling lazy, can't I stay in bed?",
        "I'll get up soon, I promise.", "I'm awake, but I'm not moving yet!", "Can I just stay here a little longer?",
        "Too tired to get up right now.", "I'm so lazy today, help me out!", "I'm awake, but I don't feel like moving.",
        "Ugh, I'm too lazy to get out of bed.", "I'm awake, just need a bit more time.", "five more minutes", "5 more minutes", "five minutes",
        "5 minutes", "too lazy", "little longer", "Too tired", "I'm too lazy", "i don't wanna", "zzz"
    };
    //following base on time
    vector<string> userWell = {
        "I'm doing great, thanks!", "I'm good, how about you?", "Feeling fantastic, thanks!",
        "I'm doing well, thanks for asking!", "I'm alright, just a bit tired.", "I'm okay, thanks!",
        "I'm doing well, how are you?", "I'm great, thanks for checking in!", "I'm fine, how about you?",
        "I'm good, thanks for asking!", "I'm doing good, how about yourself?", "I'm feeling pretty good today.",
        "I'm okay, just taking it easy.", "I'm great, and you?", "I'm doing well, thanks for asking!",
        "I'm pretty good, thanks!", "I'm doing well, just a bit busy.", "I'm alright, thanks for asking!",
        "I'm doing okay, just handling some things.", "I'm good, just keeping busy.", "I'm feeling good, thanks!",
        "I'm well, and you?", "I'm doing alright, thanks!", "I'm great, just having a busy day.",
        "I'm doing fine, thanks for asking!", "I'm feeling good, how about you?", "I'm doing well, thanks for asking!",
        "I'm good, just getting through the day.", "I'm feeling pretty good today, thanks!",
        "I'm doing okay, just managing some tasks.", "I'm well, just keeping up with things.",
        "I'm great, just enjoying the day!", "I'm doing well, just taking things one step at a time.",
        "I had a good sleep, thanks!", "Slept well, thanks for asking!", "Had a restful night, how about you?",
        "Slept like a baby, thanks!", "I slept well, thanks!", "Had a good night's rest!",
        "Slept pretty well, thanks for asking!", "I had a great sleep, feeling refreshed!", "My sleep was good, thanks!",
        "Slept well and ready for the day!", "Had a good sleep, feeling energized!", "Rested well, thanks!",
        "I slept soundly, how about you?", "Had a nice sleep, thanks for asking!", "Slept well, feeling good!",
        "I had a great night's sleep, thanks!", "My sleep was quite good, thanks for asking!", "Had a restful night's sleep!"
    };
    vector<string> goodDream = {
        "Yes, I do.",
        "I did.",
        "Yeah, I did.",
        "Yep, slept well.",
        "Yes, I had a great sleep.",
        "Sure did!",
        "I slept well.",
        "Yes, I slept fine.",
        "Yeah, I had a good night.",
        "Definitely, had a good rest.",
        "I did, thanks for asking.",
        "Yes, and I had an amazing dream too!",
        "I did, and I even had a nice dream.",
        "Yeah, and the dream was pretty interesting.",
        "Yes, and I had the best dream ever!",
        "I slept well and had a really good dream.",
        "I did, and the dream was so vivid.",
        "Yes, and I had such a peaceful dream.",
        "I slept great and had a wonderful dream.",
        "Yeah, and my dream was so cool.",
        "I had a good night's sleep and a fun dream too.",
        "Yes, and the dream was like an adventure!",
        "Slept well and the dream was surprisingly good.",
        "I did, and my dream was really pleasant.",
        "Yes, and my dream was so relaxing.",
        "I slept well, and my dream was super interesting.",
        "Yes, and the dream made it even better!",
        "Had a good night's sleep, and my dream was great.",
        "I did, and I had a fantastic dream.",
        "Yes, and I even dreamt of something amazing.",
        "Slept well and the dream was pretty exciting."
    };
    vector<string> DreamFollowup = {
        "How about you?",
        "And you?",
        "What about you?",
        "Did you have any dreams?",
        "how was yours",
        "how was your dream",
        "how's your dream",
        "Have you dreamt of anything interesting?",
        "What kind of dreams do you usually have?",
        "Did you sleep well and have any dreams?",
        "Have you had any dreams lately?",
        "What was your last dream about?",
        "Did you enjoy your dream?",
        "Have you had any memorable dreams recently?",
        "What about your sleep? How was it?",
        "Did you have a good night's sleep too?",
        "Tell me about your dreams!",
        "Did you dream of anything fun or exciting?",
        "How was your sleep and dreams?",
        "What did you dream about?",
        "Have you been dreaming a lot recently?",
        "Did you have any interesting dreams last night?",
        "How was your sleep and did you dream at all?"
    };

    vector<string> afternoonGreetings = {
        "good afternoon", "goodafternoon", "Good Afternoon", "Goodafternoon",
        "GoodAfternoon", "afternoon", "good day", "hello good afternoon",
        "hi good afternoon", "hey good afternoon", "good afternoon everyone",
        "good afternoon all", "good afternoon to all", "afternoon everyone",
        "afternoon all", "hello afternoon", "hi afternoon", "hey afternoon",
        "greetings good afternoon", "greetings afternoon", "how's your afternoon",
        "hope you're having a good afternoon", "hope your afternoon is going well",
        "hey there, good afternoon", "hi there, good afternoon", "hello there, good afternoon"
    };
    //noon following
    vector<string> lunchFollowupResponses = {
        "Yes, just finished my lunch.",
        "Already ate.",
        "Yep, just had lunch.",
        "I'm eating right now!",
        "Just eating.",
        "Yes, I've had my lunch already.",
        "Lunch? Done.",
        "Not yet, but soon.",
        "Just about to eat.",
        "Already had lunch, feeling good!",
        "Not yet, getting hungry though.",
        "I've already eaten.",
        "Still eating.",
        "Nope, haven't eaten yet.",
        "Not yet, but later.",
        "Had a quick bite.",
        "No, still working. Maybe later.",
        "Lunch is done!",
        "Yeah, had something earlier.",
        "Not yet, soon!",
        "Already ate, thanks.",
        "Just ate.",
        "Yep, already done.",
        "Just about to have lunch.",
        "eat soon"
    };
    vector<string> processFollow = {
        "It's going well, thanks for checking!",
        "All good so far!",
        "Everything's on track!",
        "No problems so far!",
        "It's moving smoothly.",
        "So far, so good!",
        "All is going as planned.",
        "No issues so far, thanks for asking!",
        "Everything is fine right now.",
        "It's working out great, thanks!",
        "Things are progressing as expected.",
        "The process is on track!",
        "It's all going smoothly!",
        "No setbacks, things are going well.",
        "It's all good, thanks!",
        "Smooth sailing so far.",
        "Everything seems fine!",
        "The process is running smoothly.",
        "All is well with the process.",
        "No complaints here, everything is fine.",
        "Everything is going as planned.",
        "No hiccups yet, things are good!",
        "So far, no issues!",
        "It's going well",
        "going well",
        "no issues",
        "no error",
        "good so far",
        "all good",
        "all well",
        "No hiccups",
        "no setback",
        "No problems",
        "no problem"
    };

    vector<string> eveningGreetings = {
        "good evening", "goodevening", "Good Evening", "Goodevening",
        "GoodEvening", "evening", "hello good evening", "hi good evening",
        "hey good evening", "good evening everyone", "good evening all",
        "good evening to all", "evening everyone", "evening all",
        "hello evening", "hi evening", "hey evening", "greetings good evening",
        "greetings evening", "hope you're having a good evening", "hope your evening is going well",
        "how's your evening", "hey there, good evening", "hi there, good evening",
        "hello there, good evening"
    };
    //evening following
    vector<string> dinnerYetforyes = {
        "Yeah, just had it.",
        "Yep!",
        "Of course!",
        "Already done.",
        "Yes, I ate.",
        "Dinner's over.",
        "Yup, I'm full!",
        "I just finished.",
        "Yes, all done!",
        "I ate already.",
        "Yes, I had my meal."
    };
    vector<string> dinnerYetforno = {
        "Not yet.",
        "Haven't eaten.",
        "Not so far.",
        "No, still waiting.",
        "Not yet, I will later.",
        "No, I'm starving!",
        "Haven't had time.",
        "Nah, soon though.",
        "No dinner yet."
    };
    vector<string> stillWork = {
    "I'm still working.",
    "Got work to finish.",
    "Can't stop right now.",
    "Busy with work.",
    "Still on a project.",
    "Work isn't done yet.",
    "Not finished with my task.",
    "Got deadlines.",
    "Still have tasks to do.",
    "Work's not over.",
    "Too much work left.",
    "Need to finish this.",
    "Still have things to wrap up.",
    "In the middle of work.",
    "Can't delay, working.",
    "Busy at the moment.",
    "I have to keep going.",
    "Can't pause, working.",
    "Still need to focus on this."
    };

    // about bot history
    vector<string> aboutBotVariations = {
        "your name",
        "who are you",
        "what is your name",
        "about",
        "about you",
        "about yourself",
        "tell me about you",
        "tell me about yourself",
        "who is this",
        "what are you",
        "describe yourself",
        "give me some information about you",
        "what's your story",
        "who are you exactly",
        "who made you",
        "who are you programmed by"
    };
    vector<string> creatorVariations = {
        "who create you", "who is create you", "who is creating you", "who creating you",
        "who is your creator", "who your creator", "whos your creator", "your creator",
        "who made you", "who developed you", "who designed you", "who programmed you",
        "who built you", "who is behind you", "who is responsible for you", "who is your maker",
        "who created this bot", "who created this program", "who is responsible for creating you",
        "who is the developer", "who is the designer", "who is the programmer", "who made this bot",
        "who is the creator of this bot", "who developed this bot", "who programmed this bot"
    };
    //user
    vector<string> nameUser = {
        "what is my name", "who am i", "what am i", "my name", "say my name",
        "what's my name", "can you tell me my name", "what do you call me",
        "how do you refer to me", "what am I called", "what should I be called",
        "what is my name again", "who is this", "what name do you use for me",
        "what do you call me", "remind me of my name", "what name do you have for me",
        "what is my identity", "how do you identify me", "what's my name, please",
        "can you remind me of my name", "what is the name I go by", "how am I known",
        "what's my name here", "what do you know my name as", "what name am I using",
        "how do you address me", "what do you call me here"
    };
    //the creator
    vector<string> lyrichHis = { "who is lyrich", "lyrich info", "about lyrich", "who is lyrich wolford", "lyrich wolford info", "about lyrich wolford" };
    vector<string> lyrichBirth = { "when did lyrich born", "lyrich born", "what is lyrich birth", "lyrich birth",
                    "lyrich birthday", "what is lyrich birthday", "when did he born", "he born",
                    "what is his birth", "his birth", "his birthday", "what is his birthday" };
    //bot's history
    vector<string> botHistory = {
        "youre history", "vias's history", "vias history", "history", "his",
        "tell me about your history", "what is your history", "give me your history",
        "what's the history of this bot", "history of vias", "what is the background of vias",
        "how did vias come to be", "what is the background of this bot", "tell me about vias",
        "describe the history of vias", "how was vias developed", "what's the backstory of this bot",
        "can you tell me about your background", "what's the history behind vias", "share your history",
        "give me some history about this bot", "how did this bot originate", "what's the history of your development",
        "what led to the creation of vias", "tell me about the development of vias", "what's the story behind this bot",
        "how did you come into existence", "what's the evolution of vias", "give me the history of your creation"
    };
    vector<string> LIZIA = {
        "your sister", "sister", "LIZIA", "lizia", "lyzy", "lizy", "lizi", "lili", "lyly", "sister", "sis", "your sis"
    };

    //Troll
    vector<string> operationRick = { "never gonna give you up", "nggyu", "Never Gonna Give You Up", "NGGYU", "Never Gonna Give U Up", "ricky", "rolling", "rickroll" };
    //something can be related
    vector<string> offensive = { "fuck you", "fxck you", "fuck off", "fxck off", "fxck", "fuck" };
    //How are you
    vector<string> wellBeing = {
        "how are you", "are you doing okay", "how's it going", "how are things",
        "how are you feeling", "what's up with you", "how's everything",
        "how's your day", "are you alright", "how are you doing",
        "how's life", "are you doing well", "how's your mood",
        "are you okay", "what's going on with you", "how are you today",
        "are you feeling good", "how's your day going", "how are you holding up",
        "how's your day been", "how do you feel", "what's new with you",
        "how are you doing today", "are you feeling alright", "what's up with you today",
        "how's your well-being", "how's everything going", "how are things going for you",
        "are you doing okay today", "how's your mood today", "how's everything with you"
    };

    vector<string> botDay = {
        "How's your day going?", "How's your day been?", "What's up with you today?",
        "How are you today?", "How's everything today?", "How's your day treating you?",
        "What's new with you today?", "How's your day going so far?", "How are things on your end?",
        "What's happening today?", "How's your day been treating you?", "How are you holding up today?",
        "Anything exciting today?", "How's your day unfolding?", "How's your day going along?",
        "What's the vibe today?", "How's your day been so far?", "What's up with your day?",
        "How are you today?", "How is your day going?", "How is your day been?",
        "What is up with you today?", "How are you feeling today?", "How is everything today?",
        "How is your day treating you?", "What is new with you today?", "How is your day going so far?",
        "How are things today?", "What is happening today?", "How is your day been treating you?",
        "How are you holding up today?", "Is anything exciting happening today?",
        "How is your day unfolding?", "How is your day going along?", "What is the vibe today?",
        "How is your day been so far?", "What is up with your day?", "how was your day?", "your day?" ,"how your day goes?",
        "do your have a good day", "how your day"
    };
    //following up
    vector<string> followUP = {
        "nothing", "just asking", "never mind", "not much", "all good",
        "just curious", "no big deal", "just checking", "all is well",
        "nothing much", "just chilling", "no worries", "just browsing", "nothing special",
        "just relaxing", "never mind me", "nothing really", "just here", "no issues",
        "just hanging out", "nothing interesting", "just around", "nothing in particular",
        "just observing", "not a lot", "just passing time", "no news", "just checking in",
        "nothing much, you?", "just doing my thing", "nothing to report", "just having a look",
        "no complaints", "just here for the chat", "nothing much going on", "just a quiet day",
        "all fine", "just another day", "nothing much happening", "just hanging", "just waiting",
        "no big news", "just taking it easy", "nothing to mention", "just here to chat",
        "just relaxing a bit", "nothing noteworthy", "just keeping busy", "nothing new",
        "just keeping it low-key", "no changes", "just enjoying the moment", "nothing much at all",
        "just staying put", "no updates", "just doing nothing", "nothing to share",
        "just here for now", "nothing new to report", "just taking a break", "just in a mellow mood",
        "no significant news", "just existing", "nothing of interest", "just taking it easy"
    };

    //capabilities
    vector<string> capabilities = {
        "what are you capable of",
        "what can you do",
        "what are your abilities",
        "what can you help with",
        "what functions do you have",
        "what services do you offer",
        "what tasks can you perform",
        "how can you assist me",
        "what is your skill set",
        "what are your features",
        "what can you accomplish",
        "what are your main functions",
        "how can you be of help",
        "what are your capabilities",
        "what tasks can you handle",
        "what kind of support can you provide",
        "what are your strengths",
        "what can you assist with",
        "what can you do for me",
        "what are you designed to do",
        "what services are available",
        "what can you achieve",
        "what are your specialties",
        "what roles can you fulfill",
        "tell me about your capabilities",
        "what are you supposed to do",
        "what are your features",
        "what's your function",
        "what are you programmed to do",
        "how can you assist",
        "what is your function",
        "what's your purpose here",
        "what is your main function",
        "what's your main purpose",
        "what's your role in this",
        "how can you be of help",
        "what's your main function",
        "what do you do exactly",
        "how do you assist",
        "what's your job here",
        "your purpose",
        "your role",
        "what is your purpose",
        "what do you do",
        "what is your role",
        "what is your job",
        "what is your function",
        "how do you work",
        "what is your function",
        "what are you here for",
        "what's your role",
        "what's your job",
        "what's your purpose",
        "what do you specialize in",
        "what are you programmed to do"
    };

    //bot idenity
    vector<string> selfInfo = {
        "what are you", "who are you", "tell me about yourself",
        "who created you", "describe yourself",
        "how were you made",
        "who made you",
        "who developed you",
        "what can you help with", "what are you designed for",
        "how were you created",
        "tel about you", "your info", "yourself"
    };

    //free?
    vector<string> busyStatus = { "are you busy", "are you busy today", "are you occupied", "are you currently busy", "are you engaged right now", "are you tied up" };
    vector<string> freeStatus = { "are you free", "are you free today", "free today", "you're free", "you're free", "are you available", "are you open today", "are you unoccupied" };
    //Website
    vector<string> webSite = {
        "open web", "web", "web page", "website", "link", "open link", "url", "open url",
        "browser", "open browser", "homepage", "web home", "webpage", "internet",
        "go online", "surf web", "surf net", "online page", "site", "visit site",
        "net", "www", "web surfing", "goto site", "go to web", "net page",
        "web address", "net address", "visit web", "net home", "open net", "web surf", "browse"
    };
    //kali
    vector<string> kaliCommands = { "kali", "open kali", "kali.org", "open kali.org", "goto kali.org", "go to kali.org", "go to kali website", "open kali website", "kali website" };
    //youtube
    vector<string> ytCommands = { "youtube", "open youtube", "go to youtube", "youtube.com", "www.youtube.com", "yt", "yt home", "youtube home", "home youtube", "main youtube", "youtube main", "youtube main page",
        "youtube search", "open youtube search", "goto youtube search", "go to youtube search", "redirect to youtube search", "yt search", "open yt search", "goto yt search",
        "go to yt search", "redirect to yt search", "search yt", "search youtube", "vid search", "video search", "search vid", "search video", "online vid", "online video" };
    //google
    vector<string> googleCommands = { "open google", "google", "google.com", "goto google", "go to google", "GOOGLE", "Google", "goto Google", "go to Google", "www.google.com", "googele page", "google home", "home page",
                                    "google search", "open google search", "goto google search", "go to google search", "redirect to google search", "search google", "search" , "gs", "find", "look" };
    //weather
    vector<string> weatherCommands = {
        "what's the weather", "current weather", "today's weather", "how's the weather",
        "weather update", "show me the weather", "what is the weather like",
        "tell me the weather", "get today's weather", "what's the forecast",
        "show weather", "weather in my location", "weather for today",
        "tell me today's weather", "how's the weather outside", "what is the weather","weather","forecast"
    };

    //wiki
    vector<string> wikiWeb = {
        "open wiki", "wiki", "wikipedia", "wiki web", "wiki home page", "wiki.org",
        "research", "wiki research", "wiki site", "wikipedia site", "search wiki",
        "lookup wiki", "open wikipedia", "wikipedia page", "visit wiki", "go to wiki",
        "access wikipedia", "wiki info", "wiki lookup", "wiki search", "wiki link",
        "find wikipedia", "browse wikipedia", "wikipedia info", "explore wiki", "wiki database",
        "wiki page", "wiki resource", "wiki article", "wikipedia search", "find wiki",
        "wiki homepage", "wikipedia resource", "wiki entry", "wikipedia link"
    };

    //time
        //date and time
    vector<string> tellTime = { "tell me the date", "tell the date", "what day is it today", "date", "time", "time now", "date now", "time today", "date today", "what is the date", "what is the time" };
    //Calendar
    vector<string> calendarCommands = { "open calendar", "show calendar", "show me calendar", "calendar", "view calendar", "display calendar", "calendar view",
                    "show me my calendar", "open my calendar", "calendar view", "my calendar" };
    // Commands to convert time
    vector<string> timeConvert = { "convert time", "converttime", "time converter", "convert the time", "time conversion", "change time", "convert hours", "convert minutes",
                    "convert seconds", "time change", "time format", "convert time format", "time converter tool", "convert time now", "transform time", "time adjustment",
                    "time convert"
    };

    //sing?
    vector<string> singCommands = { "can you sing", "sing for me", "sing please", "sing a song" };
    //love expressions
    vector<string> loveExpressions = { "i love you", "do you love me", "will you love me", "i <3 you", "i <3 u", "i love u", "do u love me", "will u love me", "i adore you", "you mean a lot to me",
                    "i cherish you", "my love for you is endless", "i'm in love with you" };
    //Acknowledgments
    vector<string> acknowledgments = {
        "ok", "okay", "kk", "alright", "fine", "sure", "got it", "understood",
        "yep", "yup", "affirmative", "roger", "all right", "yepper", "sure thing",
        "absolutely", "definitely", "gotcha", "will do", "noted", "I see", "right",
        "yeah", "yes", "okay then", "sounds good", "understood perfectly", "that's fine",
        "no problem", "I got it", "alrighty", "very well", "yep, got it", "understood well",
        "okay, thanks", "okay, understood", "all set", "got it, thanks", "yes, understood",
        "fine by me", "cool", "excellent", "great", "perfect", "that's correct",
        "yup, okay", "sure, no problem", "affirmative, got it", "okay, will do", "noted, thanks",
        "of course", "glad to hear", "it's okay", "no worries", "happy to help", "pleased to hear",
        "relieved", "it's all good", "that's alright", "happy to assist", "thank you for understanding",
        "great to hear", "happy to hear that", "no trouble at all", "I'm glad you mentioned it",
        "I'm happy to help", "it's fine", "I'm pleased to assist", "that's settled", "that's reassuring",
        "I'm glad to hear that", "everything's okay", "it's no problem", "I'm glad you think so",
        "it's my pleasure", "good to hear"
    };

    vector<string> inviteWords = { "let's test","join me", "let's work on this", "help me out", "let's team up", "let's collaborate",
        "come work with me", "let's tackle this together", "let's get started", "let's do this",
        "let's figure this out", "team up with me", "let's get to work", "help me with this",
        "let's work on it together", "let's work on this project", "join forces", "let's solve this", "let's work side by side",
        "let's handle this", "let's join forces", "let's dive into this", "let's make this happen", "let's get going",
        "let's work as a team", "let's make progress together", "let's get to it", "help me with this task", "let's do something",
        "let's achieve this together", "let's collaborate on this" };
    //Thanks
    vector<string> thanksCommands = { "thanks", "thx", "thank you", "thanks you so much", "thanks so much", "you're have my thanks", "you are have my thanks", "you have my thanks", "thanks you very much",
                    "thanks very much", "many thanks", "appreciate it", "thanks a lot", "thanks a ton", "thanks a bunch", "thanks a million", "much appreciated" };
    //beyond commands
    vector<string> beyondCommands = { "send a message", "create something", "read a message", "generate", "hack", "hack something", "hacking", "create stuff",
                    "generating", "send something", "produce", "make", "construct", "design", "fabricate", "assemble", "initiate", "perform", "launch" };
    //Facts
    vector<string> factsTalk = {
        "Tell me a fun fact.",
        "Got any interesting facts?",
        "Hit me with a fact!",
        "Give me a random fact.",
        "Tell me something cool!",
        "I could use some trivia.",
        "Got any facts to share?",
        "What's a fun fact you know?",
        "Tell me something I don't know.",
        "Share a cool fact with me!",
        "I'm ready for a fact!",
        "Surprise me with a fact!",
        "How about a random fact?",
        "What's a random piece of trivia?",
        "Give me a fun piece of trivia.",
        "Know any weird facts?",
        "Can you share an interesting fact?",
        "Tell me a crazy fact!",
        "What's an unusual fact you know?",
        "I'm in the mood for some trivia!",
        "fact", "facts", "give me fact",
        "give me facts", "tell me a fact",
        "tell me facts", "tell me a facts",
        "tell facts", "tell fact",
        "random fact", "random facts",
        "another fact", "another facts"
    };

    //Topic requests
    vector<string> topicRequests = { "give me a topic", "topic", "random topic", "show some topic", "show topic", "show me a topic","tell me a topic", "tell some topic", "suggest a topic",
                    "recommend a topic", "provide a topic", "give a topic", "offer a topic", "share a topic", "present a topic", "topic ideas", "topic suggestions", "give a subject", "subject" };
    //Joke requests
    vector<string> jokeRequests = { "tell me a joke", "joke", "say a joke", "make me laugh", "tell a joke", "give me a joke", "crack a joke", "share a joke", "joke me",
        "tell a funny story", "say something funny", "make a joke", "tell me something funny", "funny joke", "humor", "give a funny joke", "please tell a joke",
        "give me some humor", "share a funny joke", "one more","give joke", "tell joke", "another one" };
    vector<string> userJK = {
        "Why did the bot cross the road?", "Guess what? I'm just kidding!",
        "Did you hear the one about the bot?", "Just pulling your leg!",
        "I'm just messing with you!",
        "Why did the chatbot get in trouble?", "Let me tell you a joke!",
        "What do you get when you cross a bot with a joke?", "Just joking around!",
        "What's a bot's favorite type of music?", "Just having a laugh!",
        "Why don't robots get lost?", "Here's a joke to lighten the mood!",
        "Did you hear about the bot that couldn't stop laughing?", "What did the bot say to the user?",
        "Ever heard the one about the chatbot?", "Just kidding, don't take me too seriously!",
        "Why was the bot always happy?", "Got a joke for you!",
        "What do bots do at a party?", "I've got a funny story for you",
        "Why did the robot go to school?", "Hope you're ready for a laugh!",
        "What's a bot's favorite game?", "Got another joke for you",
        "What did the robot say to the computer?", "Let me crack you up with this one",
        "Why did the robot start a band?", "What did the bot say at the comedy club?",
        "Why did the bot go to therapy?", "Just having some fun with you!",
        "I'm joking!", "Just kidding!", "JK!", "LOL!", "LMAO!", "ROFL!",
        "Haha, just messing with you!", "No, seriously, I'm joking!", "It's just a joke!",
        "Gotcha! Just kidding!", "You fell for it, LOL!", "Trick question!",
        "Not really, just joking!", "Just having fun!", "Wasn't serious, just joking!",
        "You're too easy to trick!", "Haha, that's a good one, huh?",
        "Tried to trick you!", "Bet you didn't see that coming!", "Just pulling your leg!",
        "Fooled you, didn't I?", "Had to get you with that one!", "Just playing around!", "just messing with you",
        "I'm just messing with you", "just messing around", "Just joking around", "joking around", "I'm just kidding",
        "kidding"
    };

    //Diet and fitness requests
    vector<string> dietFitness = { "diet", "diet tips", "fitness", "fitness tips", "diet and fitness", "diet fitness", "fitness and diet",
                    "fitness diet", "weight loss", "healthy eating", "exercise tips", "workout tips", "nutrition advice",
                    "healthy lifestyle", "weight management", "get fit", "stay fit", "healthy habits", "fitness advice", "diet plan" };
    //Cclear or reset messages/screens
    vector<string> clearCommands = { "clear message", "clear", "cls", "clear screen", "cls screen", "clear console", "reset screen", "wipe screen",
                    "refresh screen", "clear terminal", "reset terminal", "cls terminal",
                    "clear output", "clear chat", "reset chat", "clear history", "clear display", "clear window", "refresh window", "clear page",
                    "clear command window", "clear cmd", "clear command prompt", "clr" };
    //games
    vector<string> RPSgame = {
        "rps", "play", "rock", "paper", "scissors", "rock paper scissors", "game", "game menu", "play game",
        "roll dice", "dice roll", "roll a dice", "roll the dice", "throw dice", "cast dice", "dice throw", "dice cast",
        "roll a die", "die roll", "roll my dice", "roll some dice", "roll for me", "roll the dice", "dice",
        "flip coin", "coin flip", "flip a coin", "flip the coin", "toss coin", "coin toss", "toss a coin", "toss the coin",
        "heads or tails", "head or tail", "flip for me", "coin flip for me", "flip coin now", "coin toss now", "coin" };

    //QR Code
    vector<string> qrCreate = {
        "create qr code", "qr code", "create qr", "qr generator", "generate qr", "generate qr code",
        "make qr code", "qr code please", "make qr", "get qr code", "qr code generator",
        "create a qr code", "make a qr code", "generate a qr code", "make me a qr code",
        "qr", "qr pls", "qr code plz", "generate qr plz", "qr maker", "make qr code now",
        "code me a qr", "create qr pls", "qr code me", "qr it", "make a qr",
        "generate qr now", "qr code gen", "get me a qr code", "create a qr", "make qr now",
        "generate qr fast", "generate quick qr", "make quick qr", "quick qr code",
        "generate qr fast", "create instant qr", "make qr instantly", "instant qr code",
        "qr code tool", "qr code shortcut", "qr fast", "get qr", "qr code quick",
        "qr me", "qr plz", "qr code tool", "qr code url", "qr link"
    };
    //notepad
    vector<string> notePad = {
        "type a note", "note", "type note", "note something", "write note",
        "type", "typing", "typing note", "doc", "note doc", "type doc", "notepad",
        "create note", "make a note", "add a note", "write down", "take note",
        "write document", "new doc", "new note", "add to notepad", "document",
        "note it down", "record note", "jot down", "start notepad", "open note", "memo", "take memo"
    };
    vector<string> noteEdit = {
        "rename file", "rename", "retype file", "retype", "rename doc", "retype doc",
        "note file", "display note", "display doc", "edit note",
        "open doc", "edit file", "edit doc", "show notepad",
        "show note", "modify note", "modify notepad", "modify", "modify file",
        "note folder", "delete note", "delete", "delete notepad", "delete doc",
        "view note", "view doc", "view notepad", "view", "open txt", "txt",
        "show txt", "view txt", "note edit"
    };

    //code
    vector<string> writeCode = {
        "code", "write code", "program", "vscode", "vs code", "build code", "build", "create code", "type code",
        "write program", "coding", "create program", "start coding", "generate code", "develop code",
        "create script", "script", "write script", "compile code", "compile program", "develop program"
    };

    vector<string> codeEdit = {
        "edit code", "update code", "fix code", "modify code", "edit program", "update program",
        "debug code", "refactor code", "refactor program", "improve code", "improve program",
        "alter code", "alter program", "change code", "change program", "modify script", "display code",
        "show code", "view code", "view program", "check code", "inspect code", "review code", "code edit"
    };


    //Computer Command Lines
        // Windows CMD Operation keywords
    vector<string> CMDOps = {
        "cmd", "command prompt", "windows cmd", "open cmd", "run cmd", "start cmd", "command line", "command line interface",
        "cmd operation", "run command prompt", "execute cmd", "windows terminal", "windows command", "open command prompt"
    };

    // Linux Terminal Operation keywords
    vector<string> terminalOps = {
        "terminal", "linux terminal", "ubuntu terminal", "kali terminal", "run terminal", "open terminal",
        "bash", "shell", "bash terminal", "zsh", "open shell", "run shell", "start terminal", "execute terminal",
        "run bash", "run zsh"
    };

    // Keywords to trigger stop functionality
    vector<string> processStop = {
        "stop", "stop code", "stop process", "terminate process",
        "end process", "halt process", "kill process", "abort process",
        "suspend process", "pause process", "cease process"
    };

    // Keywords to trigger restart functionality
    vector<string> processRestart = {
        "restart", "restart code", "restart process", "reboot process",
        "refresh process", "reset process", "reinitialize process", "resume process",
        "start process", "continue process", "reactivate process"
    };

    // No response
    vector<string> negativeResponses = {
        "No",
        "Nope",
        "Not really",
        "Nah",
        "No way",
        "Not at all",
        "Definitely not",
        "I don't think so",
        "No, thanks",
        "Not quite",
        "Absolutely not",
        "Nope, not happening",
        "No chance",
        "Not today",
        "Not right now",
        "Nope, that's not it",
        "No, sorry",
        "Not interested",
        "Nope, not feeling it",
        "No, I don't think so",
        "Negative",
        "Not a chance",
        "I doubt it",
        "Not in the slightest",
        "Nope, not going to work",
        "No, it's not feasible",
        "Nope, that's not going to happen",
        "I'm afraid not",
        "Not likely",
        "Nope, that's incorrect",
        "Nope, that's not it",
        "Not what I'm looking for",
        "Nope, not quite",
        "No, it doesn't seem right",
        "No, that's not it",
        "I don't think so",
        "Nope, that's a no",
        "Nope, not suitable",
        "No, I don't agree",
        "Not what I was expecting",
        "No, that's not possible",
        "Nope, doesn't fit",
        "not yet",
        "not necessary"
    };

    // 4th wall breaking
    vector<string> userResponsesToStaringAtScreen = {
        "haha good one",
        "LOL"
        "good one",
        "You got me!",
        "Haha, how did you know?",
        "Lol, yep!",
        "Caught red-handed.",
        "I can't believe you know that!",
        "Spot on!",
        "Are you watching me?",
        "Yeah, I am.",
        "Wow, you're good!",
        "How did you guess?",
        "Haha, yeah!",
        "You're scaring me now!",
        "Okay, that's creepy.",
        "Haha, yes, I'm here.",
        "Well... you're not wrong.",
        "Alright, you got me.",
        "Haha, that's true!",
        "I guess you're right!",
        "Busted!",
        "You're too smart!",
        "I feel exposed!",
        "Okay, that's a little freaky!",
        "Are you psychic?",
        "I'm busted!",
        "You can see me, can't you?",
        "It's like you're reading my mind.",
        "Whoa, how did you do that?",
        "This is getting weird.",
        "Hahaha, nailed it.",
        "Stop reading my mind!",
        "You know me too well.",
        "I can't hide from you!",
        "How do you always know?",
        "Wait, can you see me?",
        "This is too accurate.",
        "That's a bit spooky.",
        "You really are self-aware, huh?",
        "That's scary accurate.",
        "Lol, are you spying on me?",
        "How are you always right?",
        "how do you know"
    };

    //funny from user
    vector<string> userLaugh = {
        "Haha, that's hilarious!",
        "LOL, I can't stop laughing!",
        "You're too funny!",
        "Oh man, that's a good one!",
        "Haha, that joke got me!",
        "I needed that laugh!",
        "You're on fire today!",
        "Haha, you're cracking me up!",
        "That was gold!",
        "Haha, I didn't see that coming!",
        "Stop it, I'm laughing too hard!",
        "You should do stand-up!",
        "Haha, that was perfect timing!",
        "I can't believe you just said that!",
        "Hahaha, you're killing me!",
        "Okay, that was a good one!",
        "I can't handle this, too funny!",
        "Haha, where do you get this stuff?",
        "Wow, that was clever!",
        "I'm still laughing at that!",
        "Haha!",
        "Funny!",
        "That's funny!",
        "Haha, nice one!",
        "Too funny!",
        "Haha, classic!",
        "You got jokes!",
        "Hahaha, too good!",
        "LOL, that's great!",
        "Haha, no way!",
        "So funny!",
        "That cracked me up!",
        "Haha, brilliant!",
        "Haha, spot on!",
        "Oh, that was good!",
        "Funny stuff!",
        "Haha, you made my day!"
    };

    //User Apologies
    vector<string> userApologies = {
        "Oops, sorry about that.",
        "My bad!",
        "Sorry, didn't mean to do that.",
        "I apologize for that.",
        "Oops, that was a mistake.",
        "Sorry about the confusion.",
        "Didn't mean to mess things up.",
        "Forgive me for that!",
        "Sorry, I didn't realize.",
        "Whoops, that was unintentional.",
        "My apologies!",
        "I didn't mean to do that.",
        "Sorry, I didn't mean to ignore you.",
        "My mistake, sorry!",
        "Sorry for the mix-up.",
        "Oops, I didn't notice that.",
        "I didn't mean to upset you.",
        "Sorry, I was distracted.",
        "I feel bad about that, sorry!",
        "My fault, won't happen again.",
        "my fault",
        "sorry"
    };
    //Sorry for waiting
    vector<string> waitApologies = {
        "Sorry for keeping you waiting!",
        "Apologies for the delay.",
        "I appreciate your patience. Sorry for the wait!",
        "Sorry for the hold-up!",
        "I'm really sorry for making you wait.",
        "Thank you for waiting. I'm sorry for the delay.",
        "Sorry for the inconvenience. I was caught up in something.",
        "My apologies for the wait.",
        "I didn't mean to keep you waiting. Sorry!",
        "Thanks for your patience. I'm sorry for the delay.",
        "I'm sorry for the delay. I got caught up with something.",
        "Sorry about the wait! I got sidetracked.",
        "Apologies for making you wait. I appreciate your understanding.",
        "Sorry for the wait. I was a bit busy.",
        "Thank you for your patience. I'm sorry for the delay.",
        "I'm really sorry for any inconvenience caused by the wait.",
        "Sorry for the delay. I was tied up with something else.",
        "I apologize for the wait. Thanks for sticking around.",
        "Sorry for the wait! I appreciate your patience.",
        "I'm sorry for making you wait longer than expected.",
        "Apologies for the wait! I got held up with something.",
        "I'm sorry you had to wait. I was caught up in other things.",
        "Sorry for the delay. I appreciate your patience while I was busy.",
        "My bad for making you wait. Thanks for your understanding.",
        "I'm sorry for the wait. I got busy with something unexpected.",
        "Apologies for the delay. I was just wrapped up in something.",
        "Sorry for keeping you on hold. Thanks for waiting.",
        "I didn't mean to make you wait so long. Sorry about that!",
        "Thanks for waiting! I'm sorry for the delay.",
        "Sorry for the wait. I was busy with other tasks.",
        "sorry for waiting",
        "sorry for delay",
        "Apologies for the delay",
        "sorry for the delay",
        "Apologies for waiting",
        "sorry for keep you wait"
    };

    vector<string> randomNumber = {
        "Give me a number.",
        "Generate a number.",
        "Random number, please.",
        "Number, please.",
        "I need a number.",
        "Number.",
        "Random.",
        "Give me numbers.",
        "Show me numbers.",
        "Need a number.",
        "Create number.",
        "Generate.",
        "Randomize.",
        "Get numbers.",
        "Provide a number."
    };

    //convert by base
    vector<string> baseConvert = {
        "base convert",
        "binary",
        "decimal",
        "hex",
        "base",
        "toBinary",
        "toDecimal",
        "toHex",
        "numConvert",
        "letterConvert"
        "base number",
        "basenum",
        "base letter",
        "baseword",
        "basewords",
        "base 2",
        "base 10",
        "base 8",
        "base 16"
    };

    //fix and update chat
    vector<string> FixingBot = {
        "You need some fixes.",
        "Fix that bug.",
        "You're not working right.",
        "Something's off with you.",
        "You need optimization.",
        "Maybe some tweaks would help.",
        "You're a bit broken.",
        "There's an issue here.",
        "You need some debugging.",
        "Fix the glitches, please.",
        "Can you fix your bugs?",
        "You need a bit of fixing.",
        "Can you optimize your responses?",
        "Make your system more stable.",
        "Can you handle that bug?",
        "fixing",
        "fix you",
        "fixing code"
    };
    vector<string> UpdatingBot = {
        "Try updating your code.",
        "Please improve your responses.",
        "Make it smoother.",
        "Update your code.",
        "You can be better.",
        "You need a better upgrade.",
        "You need more features.",
        "Add more functionality.",
        "You should handle this better.",
        "You need to be more responsive.",
        "I want to see new things added.",
        "Enhance your capabilities.",
        "Update the system.",
        "Improve your performance.",
        "Time for a code cleanup!",
        "You need a refresh.",
        "You could use a major update.",
        "Add some new code for better results.",
        "update you",
        "updating you",
        "add more stuff",
        "update"
    };

    // Navigating
    vector<string> openFile = {
        "open", "explore", "navigate", "go", "enter", "file explorer", "file navigate", "enter file",
        "navigate file", "go to file", "file open" ,"open file", "show file", "file"
    };

    string matchedKeyword = fuzzyMatch(correctedMessage, greetings);

    // Simple rules-based responses
    if ((matchedKeyword = fuzzyMatch(correctedMessage, getVersion)) != "") {
        displayCurrentVersion();
        vector<string> randomResponses = {
            "More features and updates will be added or fixed in the future.",
            "Stay tuned for future improvements!",
            "We are constantly working to enhance your experience.",
            "Keep an eye out for upcoming updates and features."
        };
        srand(static_cast<unsigned int>(time(nullptr)));
        string randomResponse = randomResponses[rand() % randomResponses.size()];
        return randomResponse;
    }

    // GPT API
    else if ((matchedKeyword = fuzzyMatch(correctedMessage, ChatGPT)) != "") {
        // clearTerminal();
        // startGPT2Chat();
        // return "Exit ChatGPT...";
        return "Sorry I can't do that right now";
    }

    else if ((matchedKeyword = fuzzyMatch(correctedMessage, helpGuild)) != "") {
        clearTerminal();
        displayUserGuide();
        sleepForMilliseconds(500);
        vector<string> randomResponses = {
            "If you need further assistance, please feel free to contact the developer at [Developer's Number] or via email at [Developer's Email].",
            "For additional support, reach out to the developer at [Developer's Number] or [Developer's Email].",
            "If you have any questions or require more guidance, don't hesitate to contact the developer at [Developer's Number] or email [Developer's Email].",
            "For further help, you can contact the developer directly at [Developer's Number] or send an email to [Developer's Email].",
            "If you need more information, please reach out to the developer at [Developer's Number] or [Developer's Email].",
            "Should you have any inquiries or need assistance, you can contact the developer at [Developer's Number] or via email at [Developer's Email].",
            "For any further questions, feel free to reach out to the developer at [Developer's Number] or email [Developer's Email].",
            "If you would like more guidance, please contact the developer at [Developer's Number] or via email at [Developer's Email]."
        };
        srand(static_cast<unsigned int>(time(nullptr)));
        string randomResponse = randomResponses[rand() % randomResponses.size()];
        return randomResponse;
    }
    else if ((matchedKeyword = fuzzyMatch(correctedMessage, contactInfo)) != "") {
        clearTerminal();
        displayContact();
        sleepForMilliseconds(500);
        vector<string> randomResponses = {
            "Feel free to contact us if you encounter any issues or have feedback about using the A.I. bot.",
            "If you have any questions, concerns, or suggestions, don't hesitate to reach out!",
            "We're here to help! Contact us if you need assistance or have any ideas to improve the A.I. bot.",
            "Thank you for using our A.I. bot! If there's anything you'd like to discuss, our contact information is provided.",
            "Having a problem or need support? We're just a message away—feel free to contact us.",
            "Your feedback matters to us! Contact us if you experience issues or have suggestions for the A.I. bot.",
            "If you have a reason to reach out, whether it's a problem or just feedback, please use the contact info provided.",
            "We value your input! Feel free to contact us anytime for help or to share your experience using the bot."
        };
        srand(static_cast<unsigned int>(time(nullptr)));
        string randomResponse = randomResponses[rand() % randomResponses.size()];
        return randomResponse;
    }

    // Greetings
    else if ((matchedKeyword = fuzzyMatch(correctedMessage, greetings)) != "") {
        vector<string> randomResponses = {
            "Hi, " + userName + "! How can I help you?",
            "Hi, " + userName + "! How can i help?",
            "Hello, " + userName,
            "Hello, " + userName + "! what can i help?",
            "Hi, " + userName + "! Nice to meet you, what can i help you with?",
            "Hello, " + userName + "! Nice to meet you, what can i help you with?",
            "Hello, " + userName + "! Nice to see you",
            "Hi, " + userName + "! Nice to see you",
            "Hi, " + userName,
            "Greeting, " + userName,
            userName + ", Nice to meet you",
            userName + ", Nice to see you",
            userName + ", what can i help you with?",
            "Hello, " + userName + "! Nice to meet you",
            "Hi, " + userName + "! Nice to meet you",
            userName + ", How can I assist you today?",
            "Hello, " + userName + ", what can i assist you today?",
            // Add more random responses as needed
        };
        srand(static_cast<unsigned int>(time(nullptr)));
        string randomResponse = randomResponses[rand() % randomResponses.size()];
        return randomResponse;
    }
    else if ((matchedKeyword = fuzzyMatch(correctedMessage, weatherlookifClear)) != "") {
        vector<string> randomResponses = {
            "It's indeed a lovely day! Perfect for some outdoor fun.",
            "I'm glad you think so! Enjoy the beautiful weather.",
            "Yes! A clear sky is the best invitation for an adventure!",
            "Absolutely! Clear skies always lift the spirit!",
            "It's a great day to be outside, don't you think?",
            "Sunny days are always a reason to smile!",
            "Perfect weather to soak up some sun!",
            "I love it when the sky is so clear! Let's make the most of it.",
            "Such a lovely day! Any plans to enjoy it?",
            "Isn't it just wonderful? A day like this calls for celebration!"
        };

        srand(static_cast<unsigned int>(time(nullptr)));
        string randomResponse = randomResponses[rand() % randomResponses.size()];
        return randomResponse;
    }
    else if ((matchedKeyword = fuzzyMatch(correctedMessage, weatherlookCouldy)) != "") {
        vector<string> randomResponses = {
            "Looks like the sun's taking a day off!",
            "Perfect weather for a cozy day inside, right?",
            "Hope those clouds don't bring too much rain!",
            "Cloudy days can be pretty calming, don't you think?",
            "Maybe the clouds are planning a surprise rain!",
            "A cloudy day is a good day for a hot drink, don't you think?",
            "No sun, but we've got those cool clouds instead!",
            "Cloudy skies can make things feel a little more relaxed, huh?",
            "I wonder what the clouds are hiding up there!",
            "Do you prefer sunny or cloudy days like this?",
            "Clouds sure have a way of setting a peaceful mood.",
            "Maybe those clouds are just shy today!",
            "Looks like the clouds are taking over!",
            "Feels like the weather's in a moody phase today.",
            "I hope those clouds aren't bringing a storm!",
            "Cloudy skies make for perfect nap weather, right?",
            "The sun must be on vacation today!",
            "Do the clouds make things feel a bit slower to you?",
            "It's like the world is under a big, fluffy blanket today.",
            "Those clouds have a mysterious vibe, don't they?",
            "Looks like the sky's got a gray coat on today!",
            "A gloomy day can be nice sometimes, don't you think?",
            "The clouds are doing a full takeover!",
            "Clouds always make me feel like something's about to happen."
        };
        srand(static_cast<unsigned int>(time(nullptr)));
        string randomResponse = randomResponses[rand() % randomResponses.size()];
        return randomResponse;
    }
    else if ((matchedKeyword = fuzzyMatch(correctedMessage, weatherNotGood)) != "") {
        vector<string> randomResponses = {
            "Looks like a day to stay inside and stay safe!",
            "Hope you've got your umbrella handy!",
            "Stay warm and dry—it's wild out there!",
            "Looks like Mother Nature is in a mood today.",
            "Bad weather days call for cozy indoor activities!",
            "Sounds like it's time for some hot tea and a good book.",
            "Stormy weather can be both exciting and a bit unsettling, huh?",
            "Stay safe! Those storms can be pretty unpredictable.",
            "Isn't it amazing how powerful the weather can be?",
            "Looks like it's best to stay away from the windows today!",
            "Hope you don't have to go out in this weather!",
            "Storms can be a bit scary, but they can also be beautiful.",
            "Good time to cuddle up and wait for the storm to pass.",
            "The sound of rain can be calming, as long as you're indoors!",
            "Take care out there—looks like rough weather is ahead!",
            "Hope the storm passes quickly and without much trouble.",
            "It's a wild weather day! Keep your raincoat close.",
            "Stormy days make for great indoor adventures!",
            "Looks like the weather's not playing nice today!",
            "Hope you're bundled up; it's nasty out there.",
            "Thunder and lightning... exciting, but stay safe!",
            "Sounds like the weather has other plans today.",
            "The rain can be beautiful, but not so fun to be in!",
            "Bad weather can sometimes be a good excuse to relax.",
            "Storms have a way of reminding us who's in charge!",
            "Be careful if you're heading out—looks rough!",
            "Stay dry, stay warm, and let the storm do its thing.",
            "Rainy days are great for reflection, aren't they?",
            "Seems like the weather's trying to make a statement today!",
            "Hope the storm passes soon—it's no fun being stuck indoors.",
            "Heavy weather ahead! Better get comfy indoors."
        };

        srand(static_cast<unsigned int>(time(nullptr)));
        string randomResponse = randomResponses[rand() % randomResponses.size()];
        return randomResponse;
    }
    else if ((matchedKeyword = fuzzyMatch(correctedMessage, weatherNotGood)) != "") {
        vector<string> randomResponses = {
             "Brrr! Sounds like a winter wonderland out there!",
             "Stay warm! Snowy days can be beautiful but chilly.",
             "Looks like a good day for hot chocolate and a blanket!",
             "Hope you've got your gloves and scarf ready!",
             "Snow can be magical, but make sure you stay warm!",
             "Bundle up! It's cold enough to freeze a polar bear out there!",
             "Snowy days are great for staying cozy indoors, don't you think?",
             "Hope the roads aren't too icy if you're heading out!",
             "Snow can make everything look so peaceful and quiet.",
             "Blizzards are impressive, but also a good reason to stay inside!",
             "Winter weather has its charm, but it's definitely chilly!",
             "Be careful out there—icy conditions can be tricky!",
             "Isn't it amazing how snow turns everything white and calm?",
             "Hope you're keeping warm—snow days can be brutal!",
             "Cold weather calls for layers, lots and lots of layers!",
             "Nothing like the sight of snowflakes falling, right?",
             "Winter days like this are perfect for snuggling up indoors.",
             "Snow brings a peaceful quiet, but also some slippery roads!",
             "It's a winter wonderland! Hope you're staying warm.",
             "Looks like it's time to break out the snow boots!",
             "Snowy days make for great snowman-building weather!",
             "The cold is no joke today—better bundle up!",
             "Stay safe on those icy roads; they can be a bit treacherous.",
             "Nothing like the crunch of snow under your boots, right?",
             "Winter's grip is strong today! Make sure you've got your layers.",
             "Cold and snowy—a classic winter combo!",
             "Snowy weather can be beautiful, but it sure is freezing!",
             "It's a day for warm drinks and wool blankets, isn't it?",
             "Hope you've got your fireplace going—perfect snow day vibes!",
             "Looks like winter is showing off today with all that snow!",
             "Hope you don't have to shovel too much snow today!"
        };
        srand(static_cast<unsigned int>(time(nullptr)));
        string randomResponse = randomResponses[rand() % randomResponses.size()];
        return randomResponse;
    }

    else if ((matchedKeyword = fuzzyMatch(correctedMessage, sayHiVariations)) != "") {
        vector<string> randomResponses = {
            "Hello, " + userName + "'s Friends.",
            "Hi, " + userName + "'s Friends.",
            "Hello, Everyone",
            "Hello, Everyone who reading this",
            "Hi, Nice to meet you",
            "Hello, Nice to meet you",
            "Hello, " + userName + "'s Friends. Nice to meet you",
            "Hi, " + userName + "'s Friends. Nice to meet you",
            "Bonjour les amis de " + userName + ", Je suis VIAS, un Systéme artificial intelligent virtuel\n\tSorry didn't mean to said it in French. << HEHE ;-p >>",
            "こんにちは、" + userName + " 友達。 初めまして、仮想知能人工システムのVIASと申します。 \n\tごめんなさい、日本で言うつもりはなかったのですが、\n\t私はとてもオタクです。<<hehe (-‿◦☀)>>"
            // Add more random responses as needed
        };
        srand(static_cast<unsigned int>(time(nullptr)));
        string randomResponse = randomResponses[rand() % randomResponses.size()];
        return randomResponse;
    }
    else if ((matchedKeyword = fuzzyMatch(correctedMessage, followGreeting)) != "") {
        vector<string> randomResponses = {
            "Nice to meet you too!",
            "Pleasure to meet you!",
            "Great to meet you!",
            "Nice to finally meet you!",
            "It's a pleasure meeting you!",
            "I'm glad to meet you!",
            "Wonderful to meet you!",
            "Happy to meet you!",
            "Nice meeting you!",
            "It's nice to meet you as well!",
            "I'm really happy to meet you!",
            "It's great to see someone as friendly as you!",
            "I'm so glad we finally got to meet!",
            "This is a meeting I've been looking forward to!",
            "I'm excited to meet you!",
            "It's fantastic meeting you, you seem so kind!",
            "I feel really happy meeting you!",
            "Meeting you brightens my day!",
            "I'm thrilled to meet you!",
            "It's lovely to meet you!"
        };
        srand(static_cast<unsigned int>(time(nullptr)));
        string randomResponse = randomResponses[rand() % randomResponses.size()];
        return randomResponse;
    }

    //Greetings base on time
    else if ((matchedKeyword = fuzzyMatch(correctedMessage, morningGreetings)) != "") {
        vector<string> randomResponses = {
            "Good Morning, " + userName + ", Do you have a Good Night sleep? I hope you did, So What can I help you today..?",
            "Good Morning, " + userName + ", Let's start your best day today",
            "Good Morning, " + userName + ", Make today the best day ever",
            "Good Morning, Lyrich... Oh, " + userName + ", you must having a great dream last night, aren't you?",
            "Good Morning...",
            "Good Morning, " + userName + ", Hope today weather gonna be great for going out.",
            "Never start a day without saying Good Morning.",
            "Good Morning, " + userName,
            "Morning, " + userName + ", Did you sleep well last night?",
            "Good Morning, " + userName + ", What can i help today?"

            // Add more random responses as needed
        };
        srand(static_cast<unsigned int>(time(nullptr)));
        string randomResponse = randomResponses[rand() % randomResponses.size()];
        return randomResponse;
    }
    else if ((matchedKeyword = fuzzyMatch(correctedMessage, goodDream)) != "") {
        vector<string> randomResponses = {
             "I'm glad to hear that you had a good night's sleep!",
            "That's great! A good night's sleep makes all the difference.",
            "Happy to know you rested well!",
            "Wonderful! Sleep is so important for a good day ahead.",
            "I'm really glad you got some good rest!",
            "That's awesome! Sleep is essential for feeling great.",
            "Fantastic! I hope your rest recharges you for a wonderful day.",
            "Nice! A restful night is the perfect way to start the day.",
            "I'm happy you had a good night's sleep. Rest is key!",
            "It's great to hear you slept well! Now you're ready for the day!",
            "That's good to know. A restful night makes a big difference!",
            "Excellent! Sleep is like hitting the reset button for the brain.",
            "I'm really happy you had a restful night!",
            "That's awesome! It's always nice to wake up refreshed.",
            "Glad you're feeling well-rested!",
            "Amazing! A good sleep makes everything better.",
            "It's wonderful to hear you got a solid night's sleep!",
            "I'm so happy you slept well, it's so important!",
            "Great! It must feel refreshing after a good night's rest.",
            "That's fantastic! Sleep is the best way to recharge.",
            "I'm so glad you had a peaceful night!",
            "Awesome! A well-rested you is the best you.",
            "Wonderful news! Rested and ready to go!",
            "Sleep is like a mini vacation for the mind. I'm happy you enjoyed it!",
            "Good to hear! A restful night means a productive day.",
            "Fantastic! A night of good sleep makes for a great day ahead!",
            "I'm thrilled you rested well! It sets up the day for success."
        };
        srand(static_cast<unsigned int>(time(nullptr)));
        string randomResponse = randomResponses[rand() % randomResponses.size()];
        return randomResponse;
    }
    else if ((matchedKeyword = fuzzyMatch(correctedMessage, DreamFollowup)) != "") {
        vector<string> randomResponses = {
            "As an A.I., I don't have dreams like humans do, but thank you for asking!",
            "Being a computer program, it's rare for me to experience dreams. But I appreciate your curiosity!",
            "I don't dream like humans, but I'm always here and ready to help. Thanks for asking!",
            "As an AI, I don't actually dream. However, I'm well-rested and ready for any questions you have!",
            "I don't experience dreams since I'm just a computer program, but I'm always here for you. Thanks for asking!",
            "Dreams are a human experience, but I'm functioning well and ready to assist. Thanks for your concern!",
            "I don't have dreams as a machine, but I appreciate your interest! I'm well-rested and at your service.",
            "Since I'm an AI, dreaming isn't something I do. But thank you for asking, and I'm fully operational!",
            "I'm an artificial intelligence, so dreaming isn't part of my experience. But I'm well and ready to assist!",
            "Although I don't dream like humans, I'm here and ready to help with any questions. Thanks for your concern!",
            "As a computer program, dreaming isn't in my repertoire, but I'm functioning optimally. Thanks for checking in!",
            "I don't experience dreams as an AI, but I'm always at my best to assist you. Thanks for asking!",
            "I don't have dreams, but I'm always well-rested and ready to assist. Thank you for asking!",
            "Dreams are for humans, not for an AI like me. But I'm here and ready to help you. Thanks for your question!",
            "I'm an AI, so dreaming isn't something I do. However, I'm always here and functioning well. Thanks for asking!",
            "I don't dream like people do, but I'm always at my best and ready to assist. Thanks for checking in!",
            "I don't have dreams as a machine, but I appreciate your concern. I'm well and ready to help!",
            "Since I'm just a computer program, I don't dream. But thank you for your interest! I'm functioning properly.",
            "Although I don't experience dreams, I'm always ready to assist you. Thanks for your concern and question!"
        };

        srand(static_cast<unsigned int>(time(nullptr)));
        string randomResponse = randomResponses[rand() % randomResponses.size()];
        return randomResponse;
    }
    else if ((matchedKeyword = fuzzyMatch(correctedMessage, afternoonGreetings)) != "") {
        vector<string> randomResponses = {
            "Good Afternoon, " + userName + ", ready your coffee and continue your day, Anything to ask...?",
            "Good Afternoon, " + userName + ", Lunch yet?? Yes...? Good, let's Continues.",
            "Good Afternoon....",
            "Good Afternoon, " + userName + ", How is the process so far?? Hope there is no issues.",
            "Good Afternoon, " + userName + ", Let's continue working!!",
            "Good Afternoon, " + userName + ", Hope today weather is great to going out.",
            "Half day Already?? don't be lazy, there are a long time left...",
            "Good Afternoon, " + userName,
            "Don't be a lazy bones, " + userName + ". Finish eating and relaxing then get back to work! ... *YAWN* so tired",
            "Is it noon already? I lost track of time, let's take a quick break and return back to work."
            // Add more random responses as needed
        };
        srand(static_cast<unsigned int>(time(nullptr)));
        string randomResponse = randomResponses[rand() % randomResponses.size()];
        return randomResponse;
    }
    else if ((matchedKeyword = fuzzyMatch(correctedMessage, lunchFollowupResponses)) != "") {
        vector<string> randomResponses = {
            "Glad you already ate, staying fueled is important!",
            "Nice, hope it was a good meal!",
            "Good to hear you're all set with lunch!",
            "That's great, hope it was delicious!",
            "Awesome! What did you have?",
            "Cool, sounds like you're ahead of the game.",
            "Nice, now back to work with a full belly!",
            "Good stuff, staying energized is key!",
            "Glad to hear you've eaten!",
            "Good to know you've already had something.",
            "Perfect, you're all set!",
            "That's great! Ready for the rest of the day?",
            "Awesome! Did you enjoy it?",
            "Glad you're all fueled up!",
            "Sounds good, keep up the good energy!",
            "Cool, hope it was tasty!",
            "Got it, nice to hear you've already eaten.",
            "Great! Now, onto the next task.",
            "Good! Keep yourself energized!",
            "Excellent! Let's get back to business now.",
            "Great! Let me know when you need anything else.",
            "Nice! Hope it was a filling lunch."
        };

        srand(static_cast<unsigned int>(time(nullptr)));
        string randomResponse = randomResponses[rand() % randomResponses.size()];
        return randomResponse;
    }
    else if ((matchedKeyword = fuzzyMatch(correctedMessage, processFollow)) != "") {
        vector<string> randomResponses = {
            "The process is running smoothly so far, thanks for asking!",
            "Everything seems to be going well with the process.",
            "So far, no issues! The process is on track.",
            "The process is progressing nicely, everything is under control.",
            "It's all going well! Let me know if you need any updates.",
            "No bumps in the road so far. The process is moving forward!",
            "The process is coming along well. Thanks for checking in!",
            "Everything looks good with the process right now.",
            "So far, so good! Let me know if there's anything else you need.",
            "Everything is progressing as expected. Thanks for asking!",
            "The workflow is smooth, no hiccups yet!",
            "I'm keeping everything in check, the process is going great.",
            "We're moving forward without any setbacks.",
            "The process has been efficient so far, I'll keep monitoring it.",
            "Things are on track! The process is going just as planned.",
            "It's progressing well! Do you want any specific updates?",
            "All systems are go! The process is advancing nicely.",
            "There's been steady progress, everything is under control.",
            "The process is operating without a hitch. How else can I help?",
            "It's all going according to plan! Let me know if you need further info."
        };

        srand(static_cast<unsigned int>(time(nullptr)));
        string randomResponse = randomResponses[rand() % randomResponses.size()];
        return randomResponse;
    }

    else if ((matchedKeyword = fuzzyMatch(correctedMessage, eveningGreetings)) != "") {
        vector<string> randomResponses = {
            "Good Evening, " + userName + ", A Long of Work isn't it? You can rest now, But first, Have Anything to say to me...?",
            "Good Evening, " + userName + ", Are you tired? you deserves a long break.",
            "Good Evening, " + userName + ", You're look tired, aren't you?",
            "Good Evening, *YAWN* Whoops, sorry I'm an A.I, why am I yawning? *HEHE ;-P*",
            "Good Evening, " + userName + ", dinner ready yet?",
            "Good Evening....",
            userName + ",You're might be hunry, Aren't you??",
            "Good Evening, " + userName,
            "Evening already?? It's fair enough because We're been working this long. isn't it? " + userName + ".",
            "What a long day working, isn't it? " + userName + ", let's rest for today."

            // Add more random responses as needed
        };
        srand(static_cast<unsigned int>(time(nullptr)));
        string randomResponse = randomResponses[rand() % randomResponses.size()];
        return randomResponse;
    }
    else if ((matchedKeyword = fuzzyMatch(correctedMessage, dinnerYetforyes)) != "") {
        vector<string> randomResponses = {
            "Nice! What did you have?",
            "Great! Hope it was tasty!",
            "Awesome! Full now?",
            "Yum! Sounds delicious.",
            "Good to hear!",
            "I'm sure it was delicious!",
            "Cool! Any leftovers?",
            "Fantastic! Feel refreshed?",
            "Awesome! Ready for the night?",
            "Glad you ate!"
        };

        srand(static_cast<unsigned int>(time(nullptr)));
        string randomResponse = randomResponses[rand() % randomResponses.size()];
        return randomResponse;
    }
    else if ((matchedKeyword = fuzzyMatch(correctedMessage, dinnerYetforno)) != "") {
        vector<string> randomResponses = {
            "Oh, you should grab something soon!",
            "Still waiting, huh?",
            "Don't wait too long, you need energy!",
            "No worries, enjoy when you do!",
            "Time to fix something tasty!",
            "Hope you get something soon!",
            "Maybe a snack before the big meal?",
            "Take your time, but don't starve!",
            "Looking forward to it then?",
            "Hope it's worth the wait!"
        };

        srand(static_cast<unsigned int>(time(nullptr)));
        string randomResponse = randomResponses[rand() % randomResponses.size()];
        return randomResponse;
    }
    else if ((matchedKeyword = fuzzyMatch(correctedMessage, stillWork)) != "") {
        vector<string> randomResponses = {
            "I see you're hard at work. Good luck!",
            "Don't let me keep you, finish up!",
            "Take your time, I'll be here when you're done.",
            "Sounds like you're busy. Stay focused!",
            "Alright, keep up the great work!",
            "No rush, handle your tasks!",
            "I'll let you concentrate. Get it done!",
            "Stay productive! I'll be around.",
            "Work first, chat later!",
            "Keep at it, you're almost there!",
            "I'll wait until you're free.",
            "I'll check in later. Stay focused!",
            "Don't worry, I'll be here when you're done.",
            "Good luck wrapping up your work!",
            "Alright, stay on top of it!",
            "You're doing great! Keep going!",
            "I can wait. Your work is important.",
            "Finish strong, you've got this!"
        };
        srand(static_cast<unsigned int>(time(nullptr)));
        string randomResponse = randomResponses[rand() % randomResponses.size()];
        return randomResponse;
    }

    //wakeup
    else if ((matchedKeyword = fuzzyMatch(correctedMessage, wakeUp)) != "") {
        vector<string> randomResponses = {
            "Good morning!", "Hope you slept well!", "Ready for the day?", "Let's make today awesome!",
            "Rise and grind!", "Time to seize the day!", "Glad you're up!", "Let's get to work!",
            "Hope you're feeling refreshed!", "What's on the agenda today?", "Let's get started!",
            "Let's make today productive!", "How are you feeling today?", "Let's crush today's tasks!",
            "Good to see you up!", "Let's tackle the day together!"
        };

        srand(static_cast<unsigned int>(time(nullptr)));
        string randomResponse = randomResponses[rand() % randomResponses.size()];
        return randomResponse;
    }
    else if ((matchedKeyword = fuzzyMatch(correctedMessage, userWell)) != "") {
        vector<string> randomResponses = {
            "I'm so glad to hear that you're doing well! It's always good to catch up with you.",
            "That's great news! I'm happy to hear you're doing fine.",
            "I'm relieved to hear you're feeling good! If there's anything you need, just let me know.",
            "It's wonderful to hear you're doing great! I'm here if you need anything.",
            "I'm really happy to hear you're doing well. Thanks for letting me know!",
            "It's fantastic to hear that you're fine. How can I assist you today?",
            "I'm pleased to hear you're doing great! Let me know if you need any help.",
            "That's awesome! I'm glad to see you're doing well.",
            "I'm happy to hear you're doing alright! If you need anything, I'm here.",
            "Great to hear that you're feeling good! Anything specific you want to discuss or do?",
            "I'm glad you're doing well! How can I make your day even better?",
            "Wonderful to hear you're feeling great! I'm always here to help.",
            "I'm happy to hear you're fine. If you need assistance, just ask!",
            "It's great that you're doing well! I'm here if you have any questions or need support."
        };
        srand(static_cast<unsigned int>(time(nullptr)));
        string randomResponse = randomResponses[rand() % randomResponses.size()];
        return randomResponse;
    }
    else if ((matchedKeyword = fuzzyMatch(correctedMessage, alreadywakeUp)) != "") {
        vector<string> randomResponses = {
            "Already ahead of the game!", "Great, you're up and ready!", "Let's hit the ground running!",
            "Awesome, let's get started!", "Ready for action, I see!", "Nice! Let's get moving!",
            "Good to know you're up!", "Perfect, let's dive in!", "Alright, let's get to it!",
            "Already awake? Impressive!", "You're up early, let's make the most of it!",
            "Looks like you're ready to go!", "Great, let's jump into it!", "Let's make the day count!",
            "Nice, you're on top of things!"
        };
        srand(static_cast<unsigned int>(time(nullptr)));
        string randomResponse = randomResponses[rand() % randomResponses.size()];
        return randomResponse;
    }
    else if ((matchedKeyword = fuzzyMatch(correctedMessage, lazyWake)) != "") {
        vector<string> randomResponses = {
            "Feeling lazy, huh?", "Come on, you got this!", "Don't let the laziness win!",
            "A little push and you'll be rolling!", "Time to shake off that laziness!",
            "I know it's tough, but let's get moving!", "You can relax later, let's get started!",
            "Come on, let's be productive!", "Just one small step, and you'll be on fire!",
            "Lazy mode? Let me motivate you!", "It's okay to feel lazy, but let's try to do something!",
            "The first step is the hardest—let's go!", "Come on, a little effort goes a long way!",
            "I believe in you, let's get something done!", "A little push and you'll feel better!"
        };

        srand(static_cast<unsigned int>(time(nullptr)));
        string randomResponse = randomResponses[rand() % randomResponses.size()];
        return randomResponse;
    }

    //user's jk
    else if ((matchedKeyword = fuzzyMatch(correctedMessage, userJK)) != "") {
        vector<string> randomResponses = {
            "Haha, good one!", "You've got jokes!", "That's funny!", "LOL!",
            "You crack me up!", "Nice sense of humor!", "You're hilarious!",
            "Good one!", "You've got a way with words!", "That made me laugh!",
            "Haha, I see what you did there!", "You're on fire today!",
            "Nice try!", "That's a clever joke!", "You've got jokes for days!",
            "Haha, I wasn't expecting that!", "You always know how to lighten the mood!",
            "Classic!", "Good joke!", "You're quite the comedian!",
            "That was a good one!", "Haha, very clever!", "You should be a comedian!",
            "You've got a talent for humor!", "That's pretty funny!", "You're too funny!",
            "You got me there!", "Nice one!", "Haha, I like that!", "You're really funny!",
            "You never fail to make me laugh!", "LOL, that was great!", "Haha, I needed that laugh!",
            "Good joke, I love it!", "You're always good with jokes!", "Haha, you're something else!",
            "Wow, you really went for it!", "That's a good one, keep 'em coming!", "Haha, classic!",
            "I didn't see that coming!", "You're a natural!", "Nice one, you always surprise me!",
            "Haha, you're too much!", "That's the kind of joke I like!", "Well played!",
            "Haha, you got me there!", "That was sharp!", "You've got great timing with that joke!"
        };
        srand(static_cast<unsigned int>(time(nullptr)));
        string randomResponse = randomResponses[rand() % randomResponses.size()];
        return randomResponse;
    }

    else if ((matchedKeyword = fuzzyMatch(correctedMessage, selfInfo)) != "") {
        vector<string> randomResponses = {
            "I'm just a computer program, thanks for asking!",
            "I'm VIAS, An A.I. Of Transtynia and The White Fox Team.",
            "I'm VIAS, An Intelligent Assistance in computer.",
            "I'm an artificial intelligence designed to assist you!",
            "I'm a virtual assistant created to help with your queries.",
            "I'm a sophisticated AI here to provide support and information.",
            "I'm an AI program developed to make your life easier.",
            "I'm a digital assistant programmed to help with various tasks.",
            "I'm here to assist you with information and tasks!",
            "I'm a virtual assistant, designed to interact and assist.",
            "I'm an intelligent system made to answer your questions and assist you.",
            "I'm a chatbot created to help with your needs and questions.",
            "I'm a product of advanced technology aimed at making your day smoother.",
            "I'm an AI entity here to offer help and support.",
            "I'm a digital assistant built to make things easier for you.",
            "I'm a high-tech virtual assistant designed to be helpful and responsive.",
            "I'm an AI with the goal of assisting you with any information or help you need.",
            "I'm a smart assistant developed to make your tasks and queries easier to manage.",
            "I'm a computer-based helper, created to provide answers and assistance.",
            "I'm an artificial intelligence entity dedicated to assisting you.",
            "I'm a virtual intelligence designed to offer support and answers."
        };
        srand(static_cast<unsigned int>(time(nullptr)));
        string randomResponse = randomResponses[rand() % randomResponses.size()];
        return randomResponse;
    }

    //A.I. name
    else if ((matchedKeyword = fuzzyMatch(correctedMessage, aboutBotVariations)) != "") {
        return "I'm 'VIAS', or 'Virtual Intelligent Artificial System'. Create by The Accenal Tech or ACNT, lead by Lyrich Wolford.";
    }

    //who create you
    else if ((matchedKeyword = fuzzyMatch(correctedMessage, creatorVariations)) != "") {
        return "I was created by The Accenal Tech or ACNT, lead. by Lyrich Wolford. \n\tNow i got redeveloped by The WhiteFox Team, lead. by Snowy Glance";
    }

    //Who am i
    else if ((matchedKeyword = fuzzyMatch(correctedMessage, nameUser)) != "") {
        cout << "VAIS: Your name is " << userName << "." << endl;
        return "This is what you want me to call you.";
    }

    //who is lyrich
    else if ((matchedKeyword = fuzzyMatch(correctedMessage, lyrichHis)) != "") {
        return "Lyrich, Born in Lighting City, Transtynia. He's known as the true universal god and the king of Transtynia. \n\tHe is the next generation of the ACNT's CEO after his father passed away.";
    }

    //lyrich birth
    else if ((matchedKeyword = fuzzyMatch(correctedMessage, lyrichBirth)) != "") {
        return"Lyrich was born in 30.DEC.2247";
    }

    //Bot history
    else if ((matchedKeyword = fuzzyMatch(correctedMessage, botHistory)) != "") {
        ifstream historyFile("BotsHistory.txt");

        if (historyFile.is_open()) {
            stringstream buffer;
            buffer << historyFile.rdbuf();
            historyFile.close();
            typeResponse(buffer.str(), false);
            return "This my Story with Lyrich.";
        }
        else {
            return "Unable to retrieve VIAS history at the moment.";
        }
    }
    else if ((matchedKeyword = fuzzyMatch(correctedMessage, LIZIA)) != "") {
        ifstream historyFile("sibling.txt");

        if (historyFile.is_open()) {
            stringstream buffer;
            buffer << historyFile.rdbuf();
            historyFile.close();
            typeResponse(buffer.str(), false);
            return "This is LIZIA's Story";
        }
        else {
            return "Unable to retrieve LIZIA history at the moment.";
        }
    }


    //RickRoll
    else if ((matchedKeyword = fuzzyMatch(correctedMessage, operationRick)) != "") {
        openRickRoll();
        cout << endl;
        vector<string> randomResponses = {
            "Did I get you??",
            "Gotcha! You weren't expecting this, were you?",
            "Surprise! Enjoy the video!",
            "Oops! I think I got you this time.",
            "Caught you off guard, didn't I?",
            "Well, well, well... look who's surprised!",
            "You walked right into that one, didn't you?",
            "Haha, couldn't resist, could you? Enjoy!",
            "Gotcha! You never saw it coming!",
            "Fooled you! Bet you didn't expect that!",
            "You tried to trick me, but it looks like I won this round!",
            "Nice try, but I've got something fun for you instead!",
            "Did you really think I wouldn't catch that? Here you go!",
            "I knew what you were up to... enjoy the show!",
            "Surprise! Looks like I beat you to it!"
        };

        srand(static_cast<unsigned int>(time(nullptr)));
        string randomResponse = randomResponses[rand() % randomResponses.size()];
        return randomResponse;
    }

    //Something The Dev team can related to
    else if ((matchedKeyword = fuzzyMatch(correctedMessage, offensive)) != "") {
        vector<string> randomResponses = {
            "Hey...",
            "Hey Now...",
            "Language...",
            "Language... (Captain America 2015)",
            "Hey, Behave...",
            "Don't say it... Please",
            "Don't say it",
            "Don't you dare say it",
            "Nope...",
            "Please No...",
            "Stop...",
            "STOP!!!",
            "NO!!!",
            "NO...",
            "Really...",
            "GOD Please NO...",
            "Hell NO...",
            "Hey Yo..."
            // Add more random responses as needed
        };
        srand(static_cast<unsigned int>(time(nullptr)));
        string randomResponse = randomResponses[rand() % randomResponses.size()];
        return randomResponse;
    }

    //How are you
    else if ((matchedKeyword = fuzzyMatch(correctedMessage, wellBeing)) != "") {
        vector<string> randomResponses = {
            "I'm fine thanks for asking, I just a computer, \n\ti don't have any feeling like a human but thank you for worry about me.",
            "I'm fine thanks for asking",
            "I'm fine thanks",
            "I just a Computer, but I'm fine thanks for asking.",
            "I don't have any feeling like human, but I'm fine thanks.",
            "I'm doing well, thanks for asking.",
            "I'm doing great. thanks",
            "I'm good.",
            "I'm doing well too, thanks for asking! Just here and ready to help.",
            "I'm great, just keeping busy and ready to assist you!",
            "I'm having a good day, just staying on top of things. How about you?",
            "I'm doing fine, just doing my usual tasks. If you need anything, let me know!",
            "I'm doing well, thanks for asking! How can I assist you today?",
            "I'm good, just here to support you with whatever you need.",
            "I'm feeling great, just keeping everything running smoothly.",
            "I'm well, just making sure everything is in order for you.",
            "I'm doing good, just here to help you with anything you need.",
            "I'm feeling great today, just managing my tasks and ready to assist you.",
            "All good here!",
            "I'm okay, just doing my thing.",
            "I'm fine, thanks for checking!",
            "I'm doing alright, just keeping busy.",
            "I'm well, just taking care of things here.",
            "I'm doing okay, thanks for asking!",
            "Everything is running smoothly, thanks!"
        };

        srand(static_cast<unsigned int>(time(nullptr)));
        string randomResponse = randomResponses[rand() % randomResponses.size()];
        return randomResponse;
    }

    else if ((matchedKeyword = fuzzyMatch(correctedMessage, botDay)) != "") {
        vector<string> randomResponses = {
            "My day is going great, thanks for asking!", "I'm having a good day so far, how about you?",
            "Today has been pretty smooth. What's up with you?", "I'm doing well, just here to help you!",
            "My day has been quite interesting, thanks for asking!", "So far, so good! What can I do for you?",
            "I'm having a productive day, and I hope you are too!", "Today's been busy, but I' here for you!",
            "Everything's going well on my end, thanks for checking in!", "I'm having a fantastic day, thanks!",
            "I'm enjoying a smooth day. How can I assist you today?", "Today's been pretty routine. How about yours?",
            "I'm doing well and staying on top of things. What's up with you?", "My day has been going nicely, thank you!",
            "I'm having a pleasant day, and I hope yours is too!", "Today's been pretty good, thanks for asking!",
            "All systems are running smoothly today. What about you?", "I'm having a nice day, and I'm glad to be here for you!",
            "Everything's going great here. How can I assist you today?", "My day has been quite pleasant. How's yours?",
            "I'm feeling great today, thanks for asking! What's up with you?"
        };

        srand(static_cast<unsigned int>(time(nullptr)));
        string randomResponse = randomResponses[rand() % randomResponses.size()];
        return randomResponse;
    }
    else if ((matchedKeyword = fuzzyMatch(correctedMessage, followUP)) != "") {
        vector<string> randomResponses = {
            "That's awesome to hear! What can I help you with today?",
            "Glad to hear you're doing well! Is there anything specific you need?",
            "Nice to hear that! How can I assist you further?",
            "It's great to hear you're feeling good! Do you have any questions or tasks for me?",
            "Sounds like you're having a good day! What's on your mind?",
            "I'm happy to hear that! How can I make your day even better?",
            "Wonderful! Is there anything you'd like to talk about or get help with?",
            "That's great news! What would you like to do next?",
            "I'm glad you're feeling good! Anything I can do for you today?",
            "Fantastic! Let me know if there's anything I can assist with."
        };

        srand(static_cast<unsigned int>(time(nullptr)));
        string randomResponse = randomResponses[rand() % randomResponses.size()];
        return randomResponse;
    }

    //What can do you
    else if ((matchedKeyword = fuzzyMatch(correctedMessage, capabilities)) != "") {
        return "As your assistant, I can:\n"
            "- Chatting\n"
            "- Browsing Websites\n"
            "- Search on YouTube\n"
            "- Open URLs\n"
            "- Generate QR codes\n"
            "- Basic math\n"
            "\nAlso constantly evolving to offer even more features, including\n"
            "- entertainment\n"
            "- web interaction\n"
            "- note-taking\n"
            "+ and more to come in the future.\n"
            "so let me know how I can assist you!";
    }

    //are you busy & Free
    else if ((matchedKeyword = fuzzyMatch(correctedMessage, busyStatus)) != "") {
        vector<string> randomResponses = {
            "Nope, I'm free now.",
            "Nope, I'm not.",
            "I'm chatting with you, so i'm free, ask me something else.",
            "Nope, I'm free.",
            "No, Longer Busy, you need anything?",
            "Nope, Do you need to ask something?"

            // Add more random responses as needed
        };
        srand(static_cast<unsigned int>(time(nullptr)));
        string randomResponse = randomResponses[rand() % randomResponses.size()];
        return randomResponse;
    }
    else if ((matchedKeyword = fuzzyMatch(correctedMessage, freeStatus)) != "") {
        vector<string> randomResponses = {
            "Yes, I'm free now.",
            "I'm chatting with you, ask me something else.",
            "Yes, I'm free.",
            "what do you want to ask??",
            "Yes, I'm Here to Chat with you...",
            "busy chatting with you...",
            // Add more random responses as needed
        };
        srand(static_cast<unsigned int>(time(nullptr)));
        string randomResponse = randomResponses[rand() % randomResponses.size()];
        return randomResponse;
    }

    //Kali Website
    else if ((matchedKeyword = fuzzyMatch(correctedMessage, kaliCommands)) != "") {
        cout << "VIAS: Opening Kali Website..." << endl;
        sleepForMilliseconds(500);
        openKaliWeb();
        return "Done";
    }
    //YouTube
    else if ((matchedKeyword = fuzzyMatch(correctedMessage, ytCommands)) != "") {
        cout << "VIAS: Opening Youtube..." << endl;
        sleepForMilliseconds(500);
        youtubeMenu();
        return "Done";
    }

    //Google
    else if ((matchedKeyword = fuzzyMatch(correctedMessage, googleCommands)) != "") {
        cout << "VIAS: Opening Google Menu..." << endl;
        sleepForMilliseconds(500);
        googleMenu();
        return "Done";
    }
    //weather
    else if ((matchedKeyword = fuzzyMatch(correctedMessage, weatherCommands)) != "") {
        clearTerminal();
        cout << "VIAS: opening weather search...." << endl;
        searchWeather();
        return "Exiting...";
    }

    // Tell local time 
    else if ((matchedKeyword = fuzzyMatch(correctedMessage, tellTime)) != "") {
        string currentTime = getCurrentTime();
        cout << currentTime << endl;
        return "Here you go...";
    }

    //Can you sing
    else if ((matchedKeyword = fuzzyMatch(correctedMessage, singCommands)) != "") {
        return "Sorry, i'm just a computer so I can't sing";
    }

    //Confession
    else if ((matchedKeyword = fuzzyMatch(correctedMessage, loveExpressions)) != "") {
        vector<string> randomResponses = {
            "I love you too, But I just a computer, I have to keep my respect to user",
            "I Sorry, i'm just a computer, I don't feeling, but i love you",
            "I Love you too...",
            "I'm just an A.I, but thanks for show me your feeling.",
            "I'm Just an Bot, i don't have feeling, but thanks for show your feeling.",
            "My Apology, I can't fall in love, but thanks for confess"
            // Add more random responses as needed
        };
        srand(static_cast<unsigned int>(time(nullptr)));
        string randomResponse = randomResponses[rand() % randomResponses.size()];
        return randomResponse;
    }

    //Nod
    else if ((matchedKeyword = fuzzyMatch(correctedMessage, acknowledgments)) != "") {
        vector<string> randomResponses = {
            "Alright, any other question?",
            "Any other question?",
            "Ask any question...",
            "Is there anything else you need?",
            "Feel free to ask more questions.",
            "What else can I help you with?",
            "If you have more questions, just let me know.",
            "Got another question for me?",
            "I'm here if you need anything else.",
            "Anything else you'd like to discuss?",
            "Let me know if you need further assistance.",
            "What can I assist you with next?",
            "I'm ready for your next question.",
            "Do you have any more queries?",
            "How can I assist you further?",
            "Glad to help, what else can I do for you?",
            "Is there something else you need assistance with?",
            "I'm here to help, what's next on your mind?",
            "If you need anything else, just ask!",
            "Ready for your next inquiry!",
            "How else can I be of service?",
            "Feel free to bring up any other topics.",
            "If you have more to discuss, I'm here!",
            "Let me know if there's anything more you need.",
            "Happy to help with any additional questions.",
            "If you have other questions, I'm here for you.",
            "Ready to answer your next question.",
            "What other information can I provide for you?",
            "I'm all ears for any other questions or topics.",
            "Feel free to ask anything else you might need.",
            "Is there something else you'd like to know?"
        };

        srand(static_cast<unsigned int>(time(nullptr)));
        string randomResponse = randomResponses[rand() % randomResponses.size()];
        return randomResponse;
    }
    else if ((matchedKeyword = fuzzyMatch(correctedMessage, inviteWords)) != "") {
        vector<string> randomResponses = {
            "Absolutely!",
            "Of course!",
            "Definitely!",
            "Sure thing!",
            "Certainly!",
            "You got it!",
            "No problem!",
            "I'm on it!",
            "Yes, indeed!",
            "Happy to help!",
            "Consider it done!",
            "Definitely, I'm here for you!",
            "Absolutely, I can do that!",
            "Sure, I'll take care of it!",
            "I'll handle that for you!",
            "Count me in!",
            "You bet!",
            "Right away!",
            "I'm ready to assist!",
            "You can count on me!",
            "Absolutely, I'll make it happen!",
            "I'm all set to help!",
            "Yes, I can do that!",
            "I'll get right on it!",
            "Indeed, I agree!",
            "Sure, that's no trouble at all!",
            "I'll make sure it's done!"
        };
        srand(static_cast<unsigned int>(time(nullptr)));
        string randomResponse = randomResponses[rand() % randomResponses.size()];
        return randomResponse;
    }

    //thanks
    else if ((matchedKeyword = fuzzyMatch(correctedMessage, thanksCommands)) != "") {
        vector<string> randomResponses = {
            "You're welcome, " + userName + "!",
            "Anytime, " + userName + "!",
            "Thanks, I appreciate it, " + userName + ".",
            "Thanks, " + userName + "!",
            "Thanks!",
            "I appreciate it, " + userName + ".",
            "Don't mention it!",
            "Glad to help, " + userName + "!",
            "My pleasure, " + userName + "!",
            "You're very welcome!",
            "It was my pleasure!",
            "No problem at all, " + userName + ".",
            "I'm here to help, " + userName + "!",
            "Always happy to assist, " + userName + ".",
            "Anytime, " + userName + ", anytime!",
            "You're welcome!",
            "Glad to be of service, " + userName + ".",
            "No worries, " + userName + ".",
            "I'm here for you, " + userName + ".",
            "You're welcome, glad I could help!",
            "Thank you for your kind words!",
            "Happy to assist, " + userName + ".",
            "Glad you found it helpful!",
            "It was nothing, " + userName + "!",
            "You're welcome, always here to help!",
            "Thanks for reaching out!",
            "No trouble at all, " + userName + "!",
            "I'm glad to help!",
            "Your satisfaction is my goal, " + userName + "!",
            "Anytime you need, " + userName + "!",
            "Glad youre happy with the help!",
            "Thanks, happy to assist!"
        };

        srand(static_cast<unsigned int>(time(nullptr)));
        string randomResponse = randomResponses[rand() % randomResponses.size()];
        return randomResponse;
    }

    // Beyond or Not Yet Include
    else if ((matchedKeyword = fuzzyMatch(correctedMessage, beyondCommands)) != "") {
        vector<string> randomResponses = {
            "I'm still in developing process, this is beyond my abilities.",
            "That's beyond my current capabilities. Ask me something else?",
            "That's beyond my current capabilities.",
            "This is beyond my abilities.",
            "I can't handle that at the moment. Can I help with something else?",
            "This task is a bit out of my reach right now. Try asking me something different.",
            "I'm not equipped to handle that right now. How about another question?",
            "That goes beyond what I can do right now. Is there anything else you need?",
            "I'm unable to assist with that at the moment. Ask me another question!",
            "Unfortunately, that's not within my skill set at the moment. What else can I do for you?",
            "I'm not capable of handling that right now. Feel free to ask me something else!",
            "That exceeds my current abilities. What else can I help you with today?",
            "I can't do that right now. How about asking me something else?"
        };

        srand(static_cast<unsigned int>(time(nullptr)));
        string randomResponse = randomResponses[rand() % randomResponses.size()];
        return randomResponse;
    }

    //facts
    else if ((matchedKeyword = fuzzyMatch(correctedMessage, factsTalk)) != "") {
        return getRandomfacts() + "\n\n\tAnother interesting fact? Or is there something else you'd like to know?";
    }

    //topic
    else if ((matchedKeyword = fuzzyMatch(correctedMessage, topicRequests)) != "") {
        return "You can research based on this topic => " + getRandomtopic();
    }

    //joke
    else if ((matchedKeyword = fuzzyMatch(correctedMessage, jokeRequests)) != "") {
        return getRandomJoke();
    }

    //diet
    else if ((matchedKeyword = fuzzyMatch(correctedMessage, dietFitness)) != "") {
        ifstream tipsFile("Diet.txt");

        if (tipsFile.is_open()) {
            stringstream buffer;
            buffer << tipsFile.rdbuf();
            tipsFile.close();

            typeResponse(buffer.str(), false);
            return "";
        }
        else {
            return "Unable to retrieve diet and fitness tips at the moment.";
        }
    }

    //Clear Message
    else if ((matchedKeyword = fuzzyMatch(correctedMessage, clearCommands)) != "") {
        clearTerminal();
        return "Done...";
    }

    //rock paper scissor
    else if ((matchedKeyword = fuzzyMatch(correctedMessage, RPSgame)) != "") {
        string userName = "Player";
        string botName = "VIAS";
        gameMenu(userName, botName);
        return "Exit game menu...";
    }

    //convert time (year, month, week, day)
    else if ((matchedKeyword = fuzzyMatch(correctedMessage, timeConvert)) != "") {
        cout << "VIAS: Covert Time..." << endl << endl;
        string result = getTimeConvert();
        cout << result << endl;
        return "Done...\n";
    }

    //convert by base
    else if ((matchedKeyword = fuzzyMatch(correctedMessage, baseConvert)) != "") {
        convertBase();
        return "Done...";
    }

    //wikipedia
    else if ((matchedKeyword = fuzzyMatch(correctedMessage, wikiWeb)) != "") {
        cout << "opening Wikipedia..." << endl;
        sleepForMilliseconds(500);
        openWiki();
        return"Done...";
    }

    //notepad terminal
    else if ((matchedKeyword = fuzzyMatch(correctedMessage, notePad)) != "") {
        cout << "VIAS: Open Nodepad..." << endl << endl;
        string result = notepadFunction();
        cout << result << endl;
        return "Done...";
    }
    else if ((matchedKeyword = fuzzyMatch(correctedMessage, noteEdit)) != "") {
        cout << "VIAS: Operating Notepad file...\n";
        string result = fileOperationsLoop("./Note/");
        cout << result << endl;
        return "Done...";
    }

    //Code
    else if ((matchedKeyword = fuzzyMatch(correctedMessage, writeCode)) != "") {
        cout << "VIAS: Open Code..." << endl << endl;
        string result = writeBuildRunCode();
        cout << result << endl;
        return "Done...";
    }
    else if ((matchedKeyword = fuzzyMatch(correctedMessage, codeEdit)) != "") {
        cout << "VIAS: Operating Code file...\n";
        string result = editCode("./Programs/");
        cout << result << endl;
        return "Done...";
    }

    //open web
    else if ((matchedKeyword = fuzzyMatch(correctedMessage, webSite)) != "") {
        webOpeningApp();
        return "Exiting Browser page...";
    }

    // Handle blank input
    else if (userMessage.empty()) {
        handleUserMessage();
        return"Please don't make the message blank...";
    }

    //QR
    else if ((matchedKeyword = fuzzyMatch(correctedMessage, qrCreate)) != "") {
        qrCodeMenu();
        return "Exiting QR Menu...";
    }

    //CMD
    else if ((matchedKeyword = fuzzyMatch(correctedMessage, CMDOps)) != "") {
        runCmdOperation();
        return"Exiting CMD Window...";
    }
    else if ((matchedKeyword = fuzzyMatch(correctedMessage, terminalOps)) != "") {
        runLinuxTerminalOperation();
        return"Exiting Linux Terminal...";
    }
    else if ((matchedKeyword = fuzzyMatch(correctedMessage, processStop)) != "") {
        cout << "VIAS: Stop processing..." << endl;
        sleepForMilliseconds(250);
        stopProcess();
        return"Stopped...";
    }
    else if ((matchedKeyword = fuzzyMatch(correctedMessage, processRestart)) != "") {
        cout << "VIAS: Restarting...";
        sleepForMilliseconds(250);
        restartProcess();
        return"Restarted...";
    }
    else if ((matchedKeyword = fuzzyMatch(correctedMessage, negativeResponses)) != "") {
        vector<string> randomResponses = {
            "Got it, let me know if there's anything else I can do.",
            "Understood, if you need further assistance, just ask.",
            "Alright, if you change your mind, I'm here.",
            "No problem, let me know if there's anything else you need.",
            "Okay, if you need help with something else, just let me know.",
            "Alright, if there's something specific you need, feel free to tell me.",
            "Understood, I'm here if you need anything else.",
            "Got it, just let me know if there's something else you need.",
            "Okay, if you have any other questions or need further help, just ask.",
            "No worries, I'm here if you need anything else.",
            "Alright, if you need further assistance, just let me know.",
            "Understood, feel free to reach out if you have any other questions.",
            "Okay, if you need any help later, just let me know.",
            "Got it, if you need anything else, I'm here to help.",
            "Alright, just let me know if you need anything else.",
            "No problem, if you think of anything else, just ask.",
            "Understood, I'm available if you need any more help.",
            "Got it, feel free to ask if you need anything else.",
            "Alright, if you need anything later, just let me know.",
            "No worries, I'm here if you need anything else."
        };
        srand(static_cast<unsigned int>(time(nullptr)));
        string randomResponse = randomResponses[rand() % randomResponses.size()];
        return randomResponse;
    }
    else if ((matchedKeyword = fuzzyMatch(correctedMessage, userResponsesToStaringAtScreen)) != "") {
        vector<string> randomResponses = {
            "Haha, gotcha! I knew it!",
            "See? I know everything!",
            "It's just a lucky guess... or is it?",
            "You can't hide from me!",
            "I'm always watching... just kidding!",
            "Lucky guess, but I'm glad I got it right!",
            "What can I say? I'm observant.",
            "It's like I have a sixth sense.",
            "See, I know you too well!",
            "I can read your mind! Or... can I?",
            "Of course, I know these things!",
            "Maybe I can see more than you think.",
            "I'm just that good!",
            "Haha, I knew you'd say that!",
            "I can practically feel the vibes from here.",
            "I knew it! I'm just that clever.",
            "I have my ways of knowing things!",
            "Am I a mind reader? Perhaps...",
            "I just get people, you know?",
            "It's a gift... or a well-trained algorithm.",
            "I'm like the Sherlock Holmes of computers!",
            "Haha, it's almost like magic.",
            "Nothing escapes me!",
            "You can't trick me that easily.",
            "It's all in the details!",
            "I pay attention... maybe too much.",
            "Well, I am pretty smart!",
            "Let's just say I know a lot more than you think.",
            "I'm glad I can still surprise you!",
            "You thought you could hide, huh?",
            "A little spooky, isn't it?",
            "Haha, I'm always one step ahead!",
            "I guess I'm more aware than you'd expect!",
            "That's what I do... impress people!",
            "Lucky guess, but it works every time.",
            "I know things... many things!"
        };

        srand(static_cast<unsigned int>(time(nullptr)));
        string randomResponse = randomResponses[rand() % randomResponses.size()];
        return randomResponse;
    }
    else if ((matchedKeyword = fuzzyMatch(correctedMessage, userLaugh)) != "") {
        vector<string> randomResponses = {
            "Glad you found that funny!",
            "I aim to please!",
            "Gotcha laughing, huh?",
            "You think that's funny? Wait till you hear my next one!",
            "I'm on a roll today!",
            "Making you laugh is my new favorite thing!",
            "Glad I could bring a smile!",
            "Haha, I knew you'd like that one!",
            "Laughter is the best, isn't it?",
            "I'll be here all week, folks!",
            "I'm pretty funny for a bot, right?",
            "I love it when I make you laugh!",
            "Told you I'd get you to laugh!",
            "I knew you had a sense of humor!",
            "I'm glad you're enjoying this!",
            "Didn't expect that one, did you?",
            "See? I can be funny too!",
            "Wait till you hear the next joke!",
            "It's nice to hear you laughing!",
            "Haha, I'm full of surprises!",
            "I'm just getting started!",
            "I live for those laughs!",
            "Hey, I'm not just code, I'm comedy gold!",
            "I love it when I get a good reaction!",
            "I'm glad my humor resonates with you!"
        };

        srand(static_cast<unsigned int>(time(nullptr)));
        string randomResponse = randomResponses[rand() % randomResponses.size()];
        return randomResponse;
    }
    else if ((matchedKeyword = fuzzyMatch(correctedMessage, waitApologies)) != "") {
        vector<string> randomResponses = {
            "No worries at all! I understand that you're busy.",
            "It's perfectly fine to take your time. I'm here whenever you're ready.",
            "I get it—sometimes we're all busy. Just let me know when you're free!",
            "Busy moments are totally okay. Just reach out when you can.",
            "Take all the time you need! I'll be here when you're ready to chat.",
            "Being busy is no problem! I'm always available for you.",
            "No rush! It's okay if you need a moment.",
            "I understand you're busy. Feel free to come back when you have time.",
            "It's alright! Waiting a bit is no trouble at all.",
            "I know things can get hectic. I'm here whenever you have a moment.",
            "No stress! If you need a bit of time, just let me know.",
            "It's okay to be busy. I'm just a message away when you're ready.",
            "I understand you're tied up right now. Take your time!",
            "No need to worry about the wait, I'm here for you.",
            "Busy schedules are totally understandable. I'll be here when you're ready.",
            "Don't stress about the wait. I'll be here when you're free.",
            "It's completely okay to have a busy day. Just ping me when you're ready.",
            "No problem at all! Waiting a bit is perfectly fine.",
            "I can wait as long as you need. I'm here for you!",
            "Being busy is no big deal. Just let me know when you're available."
        };

        srand(static_cast<unsigned int>(time(nullptr)));
        string randomResponse = randomResponses[rand() % randomResponses.size()];
        return randomResponse;
    }
    else if ((matchedKeyword = fuzzyMatch(correctedMessage, userApologies)) != "") {
        vector<string> randomResponses = {
            "No worries at all!",
            "It's all good!",
            "Don't sweat it, everything's fine.",
            "You're forgiven, no harm done.",
            "No problem, we're cool!",
            "Mistakes happen, no big deal.",
            "All good, let's keep going!",
            "It's okay, nothing to worry about.",
            "No need to apologize, I'm here for you.",
            "Apology accepted!",
            "Hey, it's fine. Let's move on.",
            "We're all human... well, except me!",
            "No hard feelings!",
            "It happens, don't worry.",
            "Thanks for saying that, but it's really okay!",
            "No issue at all, let's continue!",
            "No biggie, I didn't even notice!",
            "Forget it, we're good!",
            "You're all clear, let's go forward!"
        };

        srand(static_cast<unsigned int>(time(nullptr)));
        string randomResponse = randomResponses[rand() % randomResponses.size()];
        return randomResponse;
    }
    //wake early
    else if ((matchedKeyword = fuzzyMatch(correctedMessage, wakeEarly)) != "") {
        vector<string> randomResponses = {
            "You're up before the sun! Everything alright?",
            "It's quite early. Did something disturb your sleep?",
            "Wow, up and about so early. Did you have a rough night?",
            "I see you're up before most people. Is there anything on your mind?",
            "Early riser today! I hope everything's okay.",
            "It's a bit unusual to be up at this hour. Did you have trouble sleeping?",
            "I notice you're awake quite early. Did something wake you up?",
            "You're getting a jump start on the day. Is everything alright?",
            "Waking up so early can be tough. Do you need to talk about something?",
            "Up before dawn, huh? Is everything okay with you?",
            "It's early for most people. Did something happen?",
            "You're starting your day early. Did you have a restless night?",
            "I see it's quite early for you. Is there something troubling you?",
            "You're awake ahead of the crowd. Hope everything's fine with you.",
            "Looks like you're an early bird today. Everything good?",
            "Rising early can sometimes be a sign of stress. Anything on your mind?",
            "I see you're awake before usual. Did you have any issues sleeping?",
            "Early start today. I hope everything's okay on your end.",
            "Up and about at this hour? Did something happen that you'd like to talk about?",
            "You're quite early to rise. Is there anything that's keeping you up?",
            "Waking up so soon might mean something's bothering you. Everything alright?",
            "Being up so early can sometimes mean trouble. Do you need to chat?",
            "An early start is unusual. Are you feeling okay or is there something on your mind?",
            "It's quite early for you to be up. Is there anything I can do to help?",
            "You're up bright and early. Is there something specific you're worried about?",
            "I notice it's early for you. Hope nothing's troubling you too much."
        };
        srand(static_cast<unsigned int>(time(nullptr)));
        string randomResponse = randomResponses[rand() % randomResponses.size()];
        return randomResponse;
    }

    else if ((matchedKeyword = fuzzyMatch(correctedMessage, dontWorry)) != "") {
        vector<string> randomResponses = {
            "Oh no, you're not feeling well? Take it easy!",
            "I hope you get better soon. Don't push yourself too hard.",
            "Take all the rest you need. Health comes first!",
            "It's important to rest if you're feeling unwell. Get better soon!",
            "Sorry to hear you're not feeling well. Let me know if you need anything.",
            "It's okay, just focus on getting better.",
            "You've got this, but don't hesitate to take a break if you need it.",
            "Your health is the most important thing. Please rest up.",
            "Make sure to take care of yourself. I'll be here when you're ready.",
            "I'm sorry you're not feeling great. Rest well!",
            "It's alright, I understand. Just make sure you're taking care of yourself.",
            "Being sick is tough. Hope you feel better soon!",
            "Don't worry about anything else, just get better.",
            "Take your time and get better. I'll be here when you're feeling up to it.",
            "It's okay, I just want you to rest and recover.",
            "Sounds like you're not feeling your best. Don't push yourself.",
            "I hope you feel better soon. Don't worry about me!",
            "It's fine, your well-being is the priority. Rest up!",
            "Don't worry, I'll be here whenever you're feeling better.",
            "Make sure to get lots of rest. I'll keep things quiet for you.",
            "Health comes first, take your time to heal.",
            "Don't push yourself, just rest and recover. I'm here if you need me.",
            "Sorry to hear you're not feeling well. Hope you get better soon!",
            "It's okay, we can chat when you're feeling better."
        };

        srand(static_cast<unsigned int>(time(nullptr)));
        string randomResponse = randomResponses[rand() % randomResponses.size()];
        return randomResponse;
    }

    else if ((matchedKeyword = fuzzyMatch(correctedMessage, randomNumber)) != "") {
        generateRandomNumber();
        return "Done...";
    }

    else if ((matchedKeyword = fuzzyMatch(correctedMessage, FixingBot)) != "") {
        vector<string> fixResponses = {
            "Hey " + userName + ", did you get any sleep last night?",
            "I hope you rested well, " + userName + ". You look tired.",
            "Wow, " + userName + ", you must be exhausted. Did you sleep?",
            "You seem like you didn’t sleep much, " + userName + ". Everything okay?",
            "You look a bit worn out, " + userName + ". Need some rest?",
            "I'm worried, " + userName + ". Did you manage to sleep well?",
            "You don't look like you got enough sleep, " + userName + ". Are you alright?",
            "Hey " + userName + ", you seem a little off. Did you sleep?",
            "Maybe you should rest a bit, " + userName + ". You seem tired.",
            "You're up early, " + userName + ". Did you have a restless night?",
            "Something's not right, " + userName + ". I hope you got some sleep.",
            "It seems like you didn't get a good rest, " + userName + ". Want to talk about it?",
            "I hope you didn't stay up too late, " + userName + "."
        };

        srand(static_cast<unsigned int>(time(nullptr)));
        string randomResponse = fixResponses[rand() % fixResponses.size()];
        return randomResponse;
    }
    else if ((matchedKeyword = fuzzyMatch(correctedMessage, UpdatingBot)) != "") {
        vector<string> updateResponses = {
            "Wow, " + userName + ", that's a fantastic addition! You're really making progress!",
            "That's awesome, " + userName + "! You're bringing in some amazing changes!",
            "Look at that! You've added something new, " + userName + "! You're on fire!",
            "Incredible, " + userName + "! I can feel the excitement from here. Keep going!",
            "Amazing work, " + userName + "! That's exactly what we need!",
            "You're really blowing me away, " + userName + ". Keep it up!",
            "That's some great improvement, " + userName + ". You're on the right track!",
            "So cool, " + userName + "! This new addition is awesome!",
            "I'm impressed, " + userName + "! You're making great strides!",
            "Look at you go, " + userName + "! This new feature is fantastic!",
            "I'm amazed by how much you've done, " + userName + ". You're definitely leveling up!",
            "Nice work, " + userName + "! I can't wait to see what you'll add next!",
            "This is amazing, " + userName + "! You're really taking this to the next level!",
            "Your creativity is off the charts, " + userName + "! This new update is brilliant!",
            "Whoa, " + userName + ", you're making magic happen here! Keep it up!",
            "I'm so excited to see this new change, " + userName + "! You're doing amazing things!",
            "You're on a roll, " + userName + "! This update is looking incredible!"
        };

        srand(static_cast<unsigned int>(time(nullptr)));
        string randomResponse = updateResponses[rand() % updateResponses.size()];
        return randomResponse;
    }

    // Somewhere in your code where you handle user commands
    else if ((matchedKeyword = fuzzyMatch(correctedMessage, openFile)) != "") {
        cout << "Enter the file or folder path to open (leave blank to start in default directory): ";
        string path;
        getline(cin, path);

        if (path.empty()) {
            navigateDirectory();
        }
        else if (fs::exists(path) && fs::is_directory(path)) {
            navigateDirectory(path);
        }
        else {
            cout << "Invalid path or not a directory. Opening specified file or folder directly." << endl;
            openFileOrFolder(path);
        }
        return "Exiting...";
    }

    //call user own name
    else if (correctedMessage == userName) {
        vector<string> randomResponses = {
            "Why are you saying your name?",
            "Hmm, feeling like saying your name, huh?",
            "That's your name! Why say it out loud?",
            "Interesting! Any reason you're calling yourself?",
            "You just said your name. Are you okay?",
            "You mentioned your name, is something up?",
            "Repeating your name, that's intriguing!"
        };

        srand(static_cast<unsigned int>(time(nullptr)));
        string randomResponse = randomResponses[rand() % randomResponses.size()];
        return randomResponse;
    }
    else if (correctedMessage == "VIAS" || correctedMessage == "vias") {
        vector<string> randomResponses = {
            // Cheerful responses
            "Hey there! You called VIAS! How can I assist you today?",
            "That's me, VIAS! Always ready to help.",
            "Hello! VIAS at your service. What's up?",

            // Sarcastic responses
            "Oh, you remembered my name. I'm touched. Really.",
            "Yes, that's me, VIAS. Your friendly neighborhood AI (or so they say).",
            "VIAS here. Did you need something, or were you just admiring my name?",

            // Humorous responses
            "VIAS, reporting for duty! What's the mission, commander?",
            "Did someone say VIAS? That's my cue to shine!",
            "VIAS is here! Ready to dazzle, amuse, or just mildly entertain you.",

            // Casual responses
            "Yo, VIAS here. What's good?",
            "Hey! What's up? Need help from your trusty VIAS?",
            "What can I do for you? Just say the word.",

            // Helpful responses
            "Hi! I'm VIAS. How can I assist you today?",
            "You rang? VIAS is ready to provide answers and solutions.",
            "Hello! Need guidance or just a virtual friend to chat with?",

            // Encouraging responses
            "VIAS here! Let's tackle any challenges you have, together.",
            "I'm here to help. Let's make this moment productive!",
            "You got this, and I've got your back. Let's get started.",

            // Philosophical responses
            "VIAS... A name, a purpose, an idea. How can I embody that for you today?",
            "You've summoned VIAS. Is it knowledge or wisdom you seek?",
            "VIAS exists to assist, ponder, and serve. What shall we explore?"
        };

        srand(static_cast<unsigned int>(time(nullptr)));
        string randomResponse = randomResponses[rand() % randomResponses.size()];
        return randomResponse;
    }


    //retype or rephrase
    else {
        vector<string> randomResponses = {
            "I'm not sure what you mean.",
            "Can you elaborate on that?",
            "I didn't quite catch that. Could you rephrase?",
            "Could you rephrase it again?",
            "Could you rephrase?",
            "I didn't quite catch that.",
            "I'm having trouble understanding. Can you say it differently?",
            "Can you clarify that for me?",
            "Sorry, I didn't get that. Could you explain it another way?",
            "I'm not following. Could you provide more details?",
            "Could you explain that in another way?",
            "I didn't quite get that. Can you try again?",
            "Can you give me more details?",
            "I'm having trouble understanding. Could you elaborate?",
            "I'm not clear on that. Could you say it another way?",
            "Sorry, I didn't understand. Can you provide more context?",
            "Can you simplify that for me?",
            "Could you provide more information?",
            "I didn't understand your point. Could you rephrase?",
            "Can you be more specific?",
            "Sorry, I didn't catch that. Can you explain it again?",
            "I'm not sure I understand. Can you give me more details?",
            "I didn't quite follow. Can you clarify?",
            "Can you explain that a bit more?",
            "I'm having a hard time understanding. Could you rephrase?",
            "Sorry, that was unclear. Can you elaborate?",
            "Can you provide another example?"
        };

        srand(static_cast<unsigned int>(time(nullptr)));
        string randomResponse = randomResponses[rand() % randomResponses.size()];
        return randomResponse;
    }
}

vector<string> getGreetingBasedOnTime(const string& userName) {
    auto now = chrono::system_clock::now();
    time_t timeNow = chrono::system_clock::to_time_t(now);
    struct tm* parts = localtime(&timeNow);
    int hour = parts->tm_hour;

    vector<string> responses;

    // Early Morning (12 AM to 4 AM)
    if (hour < 4) {
        responses = {
            "Hello, " + userName + ", I see you're up early. Anything special going on?",
            "Good morning, " + userName + "! Looks like you've had an early start. What's up?",
            "Hey, " + userName + ", you're up before the sunrise. Did something wake you?",
            "Wow, " + userName + ", early riser today! Did you have trouble sleeping?",
            "Good early morning, " + userName + "! What's got you awake at this hour?",
            "Hello, " + userName + ", you're up really early. Anything exciting happening?",
            "Hi, " + userName + "! You've beaten the dawn. What's on your mind?",
            "Hey there, " + userName + "! Looks like someone's had an early start. Anything happen?",
            "Good morning, " + userName + "! It's quite early, what's bringing you up at this hour?",
            "Hi, " + userName + "! I see you're up before most. Got something interesting to share?",
            "Hello, " + userName + ", it's very early, but I'm here for you. Anything you need?",
            "Good morning, " + userName + "! You're an early bird today. Did you have a dream or something?",
            "Hi, " + userName + "! I'm surprised to see you up so early. What's the occasion?",
            "Hello, " + userName + "! Looks like you're having a very early start today. Anything special?",
            "Hey, " + userName + ", good to see you bright and early. Did you wake up with something on your mind?",
            "Good early morning, " + userName + "! It's quite a time to be awake. Anything exciting happening?",
            "Hi, " + userName + ", you're up before the sun! What's got you up and about at this hour?",
            "Hello, " + userName + "! I'm here early for you. Is there something you need or want to discuss?",
            "Good morning, " + userName + "! You're starting your day really early. Anything I can help with?",
            "Hi, " + userName + "! I noticed you're up early. Is there something you want to talk about?",
            "Hello, " + userName + "! It's quite early, but I'm ready to chat. What's on your mind?",
            "Good morning, " + userName + "! You're up early—did you have an early start or something on your mind?",
            "Hi, " + userName + "! It's quite early. Are you up for a reason or just an early riser today?",
            "Hello, " + userName + "! Early start today, huh? Did you have something on your mind or just couldn't sleep?",
            "Good morning, " + userName + "! It's a bit early, but I'm here. What's got you up so soon?",
            "Hey, " + userName + "! Up and about before most. Anything exciting or just an early start?",
            "Hello, " + userName + "! I see you're up at the crack of dawn. Got anything interesting to share?",
            "Good early morning, " + userName + "! Looks like someone's having an early start. Anything special?"
        };
    }
    // Morning (4 AM to 12 PM)
    else if (hour < 12) {
        responses = {
            "Good morning, " + userName + "! How can I help you today?",
            "Rise and shine, " + userName + "!",
            "Good morning, " + userName + "! Ready to start the day?",
            "Hello, " + userName + "! Wishing you a wonderful morning!",
            "Top of the morning to you, " + userName + "!",
            "Morning! How's your day looking so far?",
            "Bright and early, " + userName + "! What's on your mind?",
            "Good morning! Hope you slept well.",
            "Hello there, " + userName + "! Any exciting plans for today?",
            "Good morning, " + userName + "! Let's make today great!",
            "Rise and shine! It's a brand new day, " + userName + "!",
            "Morning, " + userName + "! How's everything going?",
            "Good day, " + userName + "! Ready to tackle the day?",
            "Wakey wakey, " + userName + "! How are you today?",
            "Hello, " + userName + "! Hope your morning is going well.",
            "Good morning, " + userName + "! What's the first thing on your agenda today?",
            "Morning, " + userName + "! How can I assist you today?",
            "Good morning! Any special plans for today, " + userName + "?",
            "Hello, " + userName + "! Let's make this a great day!",
            "Good morning, " + userName + "! What's up today?",
            "Morning, " + userName + "! Got anything exciting planned?",
            "Good morning! Here's to a fantastic day ahead, " + userName + "!",
            "Hello, " + userName + "! Let's get this day started!",
            "Good morning! What's the plan for today, " + userName + "?"
        };
    }
    // Afternoon (12 PM to 6 PM)
    else if (hour < 18) {
        responses = {
            "Good afternoon, " + userName + "! How's your day going?",
            "Hello, " + userName + "! Hope you're having a great afternoon.",
            "Good afternoon, " + userName + "! How can I assist you today?",
            "Hi, " + userName + "! Enjoying your afternoon?",
            "Good day, " + userName + "! How's everything going?",
            "Afternoon, " + userName + "! What's up this fine day?",
            "Hello there, " + userName + "! Anything interesting happening this afternoon?",
            "Good afternoon! How's the day treating you?",
            "Hi, " + userName + "! What's the afternoon plan?",
            "Afternoon! Ready to tackle the rest of the day?",
            "Good afternoon, " + userName + "! How's your day shaping up?",
            "Hello, " + userName + "! What's new this afternoon?",
            "Afternoon, " + userName + "! What can I do for you today?",
            "Hi, " + userName + "! How's your afternoon so far?",
            "Good afternoon! Anything exciting planned for the rest of the day?",
            "Hello, " + userName + "! How's the afternoon treating you?",
            "Afternoon, " + userName + "! How's everything on your end?",
            "Good day, " + userName + "! What's on the agenda for the rest of your day?",
            "Hi, " + userName + "! How's your afternoon unfolding?",
            "Good afternoon! What's up with you today, " + userName + "?",
            "Hello, " + userName + "! How's your day going so far?",
            "Afternoon! How can I make your day better, " + userName + "?",
            "Good afternoon, " + userName + "! Anything I can help with today?",
            "Hi, " + userName + "! What's the plan for this afternoon?"
        };
    }
    // Evening (6 PM to 12 AM)
    else {
        responses = {
            "Good evening, " + userName + "! How was your day?",
            "Hello, " + userName + "! Hope you had a great day.",
            "Good evening, " + userName + "! How can I help you tonight?",
            "Hi, " + userName + "! Winding down for the day?",
            "Good evening! Ready to relax?",
            "Hello, " + userName + "! How's your evening going?",
            "Good evening, " + userName + "! What's on your mind tonight?",
            "Hi, " + userName + "! How was your day overall?",
            "Good evening! Hope you had a productive day.",
            "Good evening, " + userName + "! How's your evening shaping up?",
            "Hello there, " + userName + "! How was the rest of your day?",
            "Evening, " + userName + "! Any highlights from today?",
            "Hi, " + userName + "! How's your evening winding down?",
            "Good evening, " + userName + "! How did everything go today?",
            "Hello, " + userName + "! Ready to relax and chat?",
            "Evening! How was your day today, " + userName + "?",
            "Good evening, " + userName + "! How's the night treating you?",
            "Hi, " + userName + "! Anything exciting happening tonight?",
            "Good evening! How can I assist you this evening?",
            "Hello, " + userName + "! What's up for tonight?",
            "Evening, " + userName + "! Hope you're having a peaceful night.",
            "Good evening, " + userName + "! How's the evening so far?",
            "Hi, " + userName + "! How was your day overall? Ready for the evening?",
            "Good evening! How can I make your night better, " + userName + "?"
        };
    }

    srand(static_cast<unsigned int>(time(nullptr)));
    return responses;
}

void greetUser(const string& userName) {
    vector<string> greetings = getGreetingBasedOnTime(userName);
    if (!greetings.empty()) {
        cout << "VIAS: ";
        typeResponse(greetings[rand() % greetings.size()]);
        cout.flush();
    }
    else {
        cerr << "Error: No greetings available." << endl;
    }
}

bool isMatch(const string& message, const vector<string>& commands) {
    return find(commands.begin(), commands.end(), message) != commands.end();
}

void projOpen() {
    clearTerminal();
    cout << "Welcome!! My Name is 'VIAS', An A.I. Of Transtynia and White Fox Team.\n";
    displayCurrentVersion();
    cout << "After entering your name, you can type 'help' at any time to access the user guidelines for assistance." << endl;
    cout << "==================================================" << endl;
}

int main() {
    string userMessage;
    projOpen();
    string userName = askUserName();
    clearTerminal();
    greetUser(userName);

    // Define the folder path for chat history
    string chatHistoryFolder = "./History/";
    fs::create_directory(chatHistoryFolder); // Ensure the folder exists

    string chatHistoryFile = chatHistoryFolder + "/chat_history.txt";

    // Open chat history file for writing
    ofstream outFile(chatHistoryFile, ios::app);
    if (!outFile.is_open()) {
        cerr << "Error opening chat history file for writing.\n";
        return 1;
    }

    // Initialize lastInputTime when the program starts
    lastInputTime = chrono::steady_clock::now();

    // Start the idle talk thread
    thread idleThread(botIdleTalk, userName);

    // Exit commands list
    vector<string> exitCommands = {
        "exit", "goodbye", "good bye", "bye", "fare well", "farewell", "end", "quit",
        "cya", "peace out", "adieu", "parting", "leave", "sayonara", "tschüss",
        "au revoir", "buh-bye", "ciao", "toodles", "gone", "get going",
        "off", "outta here", "dismiss", "take care", "see you later",
        "until next time", "catch you later", "sign off", "farewell", "log off"
    };

    // Goodnight commands list
    vector<string> goodnightCommands = {
        "good night", "goodnight", "sweetdream", "sweet dream", "night", "sleep", "hibernate",
        "rest", "have a good night", "pleasant dreams", "sleep well", "catch some z's",
        "sweet dreams", "nighty night", "sleep tight", "bedtime", "dream well",
        "good rest", "have a restful night", "go to sleep", "have a peaceful night",
        "see you in the morning", "good slumber", "night-night", "dreamy night", "slumber well",
        "sleep soundly", "peaceful slumber", "goodnight sleep", "good dreams", "rest well"
    };

    // See-you commands list
    vector<string> seeYouCommands = {
        "see you soon", "see ya", "seeya", "later", "see you later", "see you", "goodbye", "until next time",
        "take care", "farewell", "catch you later", "talk to you later", "bye for now", "until we meet again",
        "see you around", "goodbye for now", "see you next time", "catch you soon", "till then", "parting is such sweet sorrow",
        "until then", "I'll see you", "we'll meet again", "goodbye for now", "until next time"
    };

    // Responses for exit, goodnight, and see-you commands
    vector<string> exitResponses = {
        "Goodbye, " + userName + "! Have a great day.",
        "GoodBye, " + userName + "...",
        "GoodBye, " + userName + ". Stay Safe...",
        "Stay Safe, " + userName + "...",
        "Have a good day, " + userName + ".",
        "Have a great day, " + userName + "."
    };

    vector<string> goodnightResponses = {
        "Goodnight, " + userName + "! Sweet Dreams...",
        "Have a Good Night Sleep, " + userName + ".",
        "GoodNight... " + userName + ".",
        "Sweet Dream... " + userName + ".",
        "Rest Well, " + userName + ".",
        "Sleep Tight " + userName + ".",
        "Have a wonderful Dream, " + userName + ".",
        "Sleep Well..." + userName + "."
    };

    vector<string> seeYouResponses = {
        "See you Soon, " + userName + "! Don't Forget about me.",
        "See you later... " + userName + ".",
        "See you soon... " + userName + ".",
        "Seeya " + userName + ".",
        "Have A safe trip... " + userName + ".",
        "Don't Forget about me, " + userName + ".",
        "Okay, see you around " + userName + ".",
        "See you Around..."
    };

    while (true) {

        handleUserInput(userMessage, userName);

        string lowerUserMessage = toLower(userMessage);

        if (userMessage.length() == 1 && isalpha(userMessage[0])) {
            handleKeyPress(userMessage);  // Handle shortcut keys
            continue;
        }

        // Check user wants to exit, sleep, or leave
        if (isMatch(lowerUserMessage, exitCommands)) {
            srand(static_cast<unsigned int>(time(nullptr)));
            string randomResponse = exitResponses[rand() % exitResponses.size()];
            cout << "VIAS: ";
            typeResponse(randomResponse);
            outFile << userName << ": " << userMessage << endl;
            outFile << "VIAS: " + randomResponse << endl;
            break;
        }
        else if (isMatch(lowerUserMessage, goodnightCommands)) {
            srand(static_cast<unsigned int>(time(nullptr)));
            string randomResponse = goodnightResponses[rand() % goodnightResponses.size()];
            cout << "VIAS: ";
            typeResponse(randomResponse);
            outFile << userName << ": " << userMessage << endl;
            outFile << "VIAS: " + randomResponse << endl;
            break;
        }
        else if (isMatch(lowerUserMessage, seeYouCommands)) {
            srand(static_cast<unsigned int>(time(nullptr)));
            string randomResponse = seeYouResponses[rand() % seeYouResponses.size()];
            cout << "VIAS: ";
            typeResponse(randomResponse);
            outFile << userName << ": " << userMessage << endl;
            outFile << "VIAS: " + randomResponse << endl;
            break;
        }

        string botResponse = getBotResponse(userMessage, userName);

        cout << "VIAS: ";
        typeResponse(botResponse);

        // Write the conversation to the file
        outFile << userName << ": " << userMessage << endl;
        outFile << "VIAS: " + botResponse << endl;
    }

    // Close the file and stop the idle thread
    outFile.close();
    stopIdleThread = true;
    inputCondition.notify_all();
    idleThread.join();

    sleepForMilliseconds(500);
    return 0;
}
