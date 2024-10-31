#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include <SFML/System.hpp>
#include <windows.h>
#include <iostream>
#include <string>
#include <thread>
#include <chrono>
#include <iomanip>
#include <sstream>
#include <fstream>
#include <vector>
#include <deque>

const unsigned int WINDOW_WIDTH = 800;
const unsigned int WINDOW_HEIGHT = 600;
const float TEXT_SIZE = 18.0f;
const float LINE_SPACING = 20.0f;
const std::string LINE_PREFIX = ">> ";
const float CHAR_ANIMATION_SPEED = 0.005f;
const float SCROLL_SPEED = 50.0f; 

struct AnimatedLine {
    std::string fullText;
    std::string currentDisplayText;
    float animationTimer;
    bool isComplete;
    bool isAsciiArt;
    bool hasStarted;

    AnimatedLine(const std::string& text, bool isArt = false) 
        : fullText(isArt ? text : LINE_PREFIX + text)
        , currentDisplayText(isArt ? "" : LINE_PREFIX)
        , animationTimer(0)
        , isComplete(false)
        , isAsciiArt(isArt)
        , hasStarted(false) {}  
};

class Terminal {
    private:
        sf::RenderWindow window;
        sf::Font font;
        sf::Texture backgroundTexture;
        sf::Sprite backgroundSprite;
        std::string currentInput;
        std::deque<AnimatedLine> displayBuffer;
        bool isLoggedIn;
        bool isWaitingForPassword;
        std::string username;
        float cursorBlinkTime;
        bool showCursor;
        float scrollOffset;  
        float maxScrollOffset;

    void updateMaxScrollOffset() {
        float contentHeight = displayBuffer.size() * LINE_SPACING + LINE_SPACING; 
        maxScrollOffset = std::max(0.0f, contentHeight - window.getSize().y + LINE_SPACING);
        scrollOffset = std::min(scrollOffset, maxScrollOffset);
    }

    void handleScroll(float delta) {
        scrollOffset = std::clamp(scrollOffset - (delta * SCROLL_SPEED), 0.0f, maxScrollOffset);
    }

    void updateBackgroundScale() {
        float scaleX = static_cast<float>(window.getSize().x) / backgroundTexture.getSize().x;
        float scaleY = static_cast<float>(window.getSize().y) / backgroundTexture.getSize().y;
        float scale = std::max(scaleX, scaleY);
        backgroundSprite.setScale(scale, scale);
        float newWidth = backgroundTexture.getSize().x * scale;
        float newHeight = backgroundTexture.getSize().y * scale;
        backgroundSprite.setPosition(
            (window.getSize().x - newWidth) / 2.0f,
            (window.getSize().y - newHeight) / 2.0f
        );
    }

    void addToBuffer(const std::string& text) {
        std::istringstream stream(text);
        std::string line;
        while (std::getline(stream, line)) {
            displayBuffer.push_back(AnimatedLine(line, false));
        }
        updateMaxScrollOffset();
        scrollOffset = maxScrollOffset;
    }


    void updateAnimations(float deltaTime) {
        bool previousComplete = true; 
        
        for (auto& line : displayBuffer) {
            if (!line.isComplete) {
                if (!line.hasStarted && !previousComplete) {
                    continue;
                }
                
                line.hasStarted = true;
                line.animationTimer += deltaTime;
                
                if (line.animationTimer >= CHAR_ANIMATION_SPEED) {
                    line.animationTimer = 0;
                    if (line.currentDisplayText.length() < line.fullText.length()) {
                        line.currentDisplayText += line.fullText[line.currentDisplayText.length()];
                    } else {
                        line.isComplete = true;
                    }
                }
            }
            previousComplete = line.isComplete;
        }
    }

    void draw() {
        window.clear();
        window.draw(backgroundSprite);
        
        updateMaxScrollOffset();
        
        float viewStart = scrollOffset;
        float viewEnd = scrollOffset + window.getSize().y;
        
        float yPos = 10.f - scrollOffset;
        for (const auto& line : displayBuffer) {
            if (yPos + LINE_SPACING >= 0 && yPos <= window.getSize().y) {
                sf::Text text(line.currentDisplayText, font, TEXT_SIZE);
                text.setFillColor(sf::Color::Green);
                text.setPosition(line.isAsciiArt ? 30.f : 10.f, yPos);
                window.draw(text);
            }
            yPos += LINE_SPACING;
        }

        std::string inputDisplay = LINE_PREFIX;
        if (isWaitingForPassword) {
            inputDisplay += std::string(currentInput.length(), '*');
        } else {
            inputDisplay += currentInput;
        }
        if (showCursor) {
            inputDisplay += "_";
        }
        
        sf::Text inputText(inputDisplay, font, TEXT_SIZE);
        inputText.setFillColor(sf::Color::White);
        inputText.setPosition(10.f, window.getSize().y - LINE_SPACING - 10.f);
        window.draw(inputText);

        window.display();
    }

    void processLogin() {
        if (!isWaitingForPassword) {
            username = currentInput;
            isWaitingForPassword = true;
            currentInput.clear();
            addToBuffer("Password: ");
        } else {
            if (username == "Hartmann" && currentInput == "demission") {
                isLoggedIn = true;
                isWaitingForPassword = false;  // Reset password flag
                displayBuffer.clear();
                addToBuffer("[INFO] Welcome to SCiPnet Terminal v5.213.0.9");
                addToBuffer("[INFO] Loading system protocols...");
                std::this_thread::sleep_for(std::chrono::milliseconds(500));
                addToBuffer("[INFO] Verifying credentials...");
                std::this_thread::sleep_for(std::chrono::milliseconds(500));
                addToBuffer("[INFO] Access Granted.");
                addToBuffer(R"(   _____ _______ ____             __)");
                addToBuffer(R"(  / ___// ____(_) __ \____  ___  / /_)");
                addToBuffer(R"(  \__ \/ /   / / /_/ / __ \/ _ \/ __/)");
                addToBuffer(R"( ___/ / /___/ / ____/ / / /  __/ /_)");
                addToBuffer(R"(/____/\____/_/_/   /_/ /_/\___/\__/)");
                addToBuffer(" "); 
                addToBuffer("[INFO] Welcome, Supervisor Hartmann.");
                addToBuffer("[INFO] You currently have [2] new messages.");
                addToBuffer("[INFO] Type 'help' for available commands.");
            } else {
                addToBuffer("[ERROR] Invalid credentials.");
                isWaitingForPassword = false;
                addToBuffer("Login: ");
            }
            currentInput.clear();
        }
    }

    void processCommand(const std::string& command) {
        addToBuffer(" ");
        addToBuffer("SCiPnet@Hartmann:~$ " + command);
        if (command == "help") {
            addToBuffer("Available commands:");
            addToBuffer("help                   - Show this help message");
            addToBuffer("clear                  - Clear the screen");
            addToBuffer("rs <num>               - Show SCP link for the specified SCP number");
            addToBuffer("rs <num> open          - Same as rs <num> + open the URL in the browser");
            addToBuffer("pixeleur               - Open Pixeleur website");
            addToBuffer("log <message>          - Log a message to the system log");
            addToBuffer("status                 - Check system status");
            addToBuffer("exit                   - Exit the terminal");
        }
        else if (command == "clear") {
            displayBuffer.clear();
        }
        else if (command.substr(0, 3) == "rs ") {
            try {
                std::string numberPart = command.substr(3);
                std::string commandAfterNumber;

                size_t spacePos = numberPart.find(' ');
                if (spacePos != std::string::npos) {
                    commandAfterNumber = numberPart.substr(spacePos + 1);
                    numberPart = numberPart.substr(0, spacePos); 
                }

                int num = std::stoi(numberPart);
                std::stringstream ss;
                ss << std::setw(3) << std::setfill('0') << num;

                std::string link = "http://fondationscp.wikidot.com/scp-" + ss.str();
                addToBuffer("Link: " + link);

                if (commandAfterNumber == "open") {
                    std::string command = "start \"\" \"" + link + "\""; 
                    system(command.c_str());
                }
            }
            catch (...) {
                addToBuffer("[ERROR] Invalid SCP number format");
            }
        }
        else if (command.substr(0, 4) == "log ") {
            std::string message = command.substr(4);
            std::ofstream logFile("system.log", std::ios::app);
            if (logFile.is_open()) {
                logFile << message << std::endl;
                addToBuffer("[INFO] Message logged successfully");
            }
        }
        else if (command == "status") {
            addToBuffer("[INFO] Checking system status...");
            std::this_thread::sleep_for(std::chrono::milliseconds(1000));
            addToBuffer("[STATUS] All systems operational.");
        }
        else if (command == "pixeleur") {
            addToBuffer("Opening Pixeleur website...");
            system("start https://pixeleur.fr");
        }
        else if (command != "exit") {
            addToBuffer("[ERROR] Unknown command: " + command);
        }
    }

public:
    Terminal() : 
        window(sf::VideoMode(WINDOW_WIDTH, WINDOW_HEIGHT), "SCiPnet Terminal", 
               sf::Style::Titlebar | sf::Style::Close | sf::Style::Resize),
        isLoggedIn(false),
        isWaitingForPassword(false),
        cursorBlinkTime(0),
        showCursor(true),
        scrollOffset(0),
        maxScrollOffset(0) {
        
        window.setFramerateLimit(60);
        
        if (!font.loadFromFile("resource/GFX/police/Consolas.ttf")) {
            throw std::runtime_error("Failed to load font");
        }

        if (!backgroundTexture.loadFromFile("resource/GFX/Background.bmp")) {
            throw std::runtime_error("Failed to load background");
        }

        backgroundSprite.setTexture(backgroundTexture);
        updateBackgroundScale();

        addToBuffer("Login: ");
    }

   void run() {
        sf::Clock clock;
        
        while (window.isOpen()) {
            sf::Event event;
            while (window.pollEvent(event)) {
                if (event.type == sf::Event::Closed) {
                    window.close();
                }
                else if (event.type == sf::Event::Resized) {
                    sf::FloatRect visibleArea(0, 0, event.size.width, event.size.height);
                    window.setView(sf::View(visibleArea));
                    updateBackgroundScale();
                }
                else if (event.type == sf::Event::MouseWheelScrolled) {
                    if (event.mouseWheelScroll.wheel == sf::Mouse::VerticalWheel) {
                        handleScroll(event.mouseWheelScroll.delta);
                    }
                }
                else if (event.type == sf::Event::Resized) {
                    sf::FloatRect visibleArea(0, 0, event.size.width, event.size.height);
                    window.setView(sf::View(visibleArea));
                    updateBackgroundScale();
                }
                else if (event.type == sf::Event::TextEntered) {
                    if (event.text.unicode == 8) {
                        if (!currentInput.empty()) {
                            currentInput.pop_back();
                        }
                    }
                    else if (event.text.unicode == 13) {
                        if (!isLoggedIn) {
                            processLogin();
                        }
                        else {
                            if (currentInput == "exit") {
                                window.close();
                            }
                            else {
                                processCommand(currentInput);
                                currentInput.clear();
                            }
                        }
                    }
                    else if (event.text.unicode < 128) {
                        currentInput += static_cast<char>(event.text.unicode);
                    }
                }
            }

            float deltaTime = clock.restart().asSeconds();
            
            cursorBlinkTime += deltaTime;
            if (cursorBlinkTime >= 0.5f) {
                cursorBlinkTime = 0;
                showCursor = !showCursor;
            }

            updateAnimations(deltaTime);
            draw();
        }
    }
};

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd) {
        try {
        Terminal terminal;
        terminal.run();
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
