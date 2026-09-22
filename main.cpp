#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include <SFML/System.hpp>
#include <SFML/Audio.hpp>

#include <sstream>
#include <fstream>
#include <vector>
#include <algorithm>
#include <cstdlib>
#include <ctime>
#include <iostream>

using namespace sf;

void updateBranches(int seed, float difficulty);

void loadHighScore();

void saveHighScore();

void triggerShake(float strength, float duration);

void updateDifficulty(int score);

void centerText(Text &text, Vector2f position);

enum class side { LEFT, RIGHT, NONE };

const int NUM_BRANCHES = 6;
std::vector<Sprite> branches;
side branchPositions[NUM_BRANCHES];

int highScore = 0;
const std::string HIGH_SCORE_FILE = "highscore.txt";

float difficultyMultiplier = 1.0f;

float shakeTime = 0.0f;
float shakeStrength = 0.0f;

int main() {
    loadHighScore();
    srand(static_cast<unsigned>(std::time(nullptr)));

    VideoMode vm(Vector2u(1920, 1080));

    RenderWindow window(vm, "Timber!!!", State::Windowed);
    View gameView = window.getDefaultView();

    Texture textureBackground;
    if (!textureBackground.loadFromFile("../graphics/background.png")) {
        std::cout << "Error loading background texture" << std::endl;

        return -1;
    }
    Sprite spriteBackground(textureBackground);
    spriteBackground.setPosition(Vector2f(0.0f, 0.0f));

    Texture textureTree;
    if (!textureTree.loadFromFile("../graphics/tree.png")) {
        std::cout << "Error loading tree texture" << std::endl;

        return -1;
    }
    Sprite spriteTree(textureTree);
    spriteTree.setPosition(Vector2f(810.0f, 0.0f));

    Texture textureTree2;
    if (!textureTree2.loadFromFile("../graphics/tree2.png")) {
        std::cout << "Error loading tree2 texture" << std::endl;

        return -1;
    }

    Sprite spriteTree2(textureTree2);
    Sprite spriteTree3(textureTree2);
    Sprite spriteTree4(textureTree2);
    Sprite spriteTree5(textureTree2);
    Sprite spriteTree6(textureTree2);

    spriteTree2.setPosition(Vector2f(20, 0));
    spriteTree3.setPosition(Vector2f(300, -400));
    spriteTree4.setPosition(Vector2f(1300, -400));
    spriteTree5.setPosition(Vector2f(1500, -500));
    spriteTree6.setPosition(Vector2f(1900, 0));

    Texture textureBee;
    if (!textureBee.loadFromFile("../graphics/bee.png")) {
        std::cout << "Error loading bee texture" << std::endl;

        return -1;
    }
    Sprite spriteBee(textureBee);
    spriteBee.setPosition(Vector2f(0, 800));

    bool beeActive = false;
    float beeSpeed = 0.0f;

    Texture textureCloud;
    if (!textureCloud.loadFromFile("../graphics/cloud.png")) {
        std::cout << "Error loading cloud texture" << std::endl;

        return -1;
    }

    const int NUM_CLOUDS = 6;
    std::vector<Sprite> clouds;
    int cloudSpeeds[NUM_CLOUDS];
    bool cloudsActive[NUM_CLOUDS];

    clouds.clear();
    clouds.resize(NUM_CLOUDS, Sprite(textureCloud));

    for (int i = 0; i < NUM_CLOUDS; i++) {
        clouds[i].setTexture(textureCloud);

        float startX = -300.0f + (i * 380.0f);
        float startY = 60.0f + ((i % 3) * 130.0f);

        clouds[i].setPosition(Vector2f(startX, startY));
        cloudsActive[i] = true;
        cloudSpeeds[i] = 60 + (rand() % 100);
    }

    Clock clock;
    Clock frameClock;

    RectangleShape timeBar;
    float timeBarStartWidth = 400;
    float timeBarHeight = 80;
    timeBar.setSize(Vector2f(timeBarStartWidth, timeBarHeight));
    timeBar.setFillColor(Color::Red);
    timeBar.setPosition(Vector2f(2000, 980));

    float timeRemaining = 6.0f;
    float timeBarWidthPerSecond = timeBarStartWidth / timeRemaining;

    bool paused = true;
    int score = 0;

    int combo = 0;
    int comboMultiplier = 1;

    Font font;
    if (!font.openFromFile("../fonts/KOMIKAP_.ttf")) {
        std::cout << "Error loading font" << std::endl;

        return -1;
    }

    Text messageText(font);
    Text scoreText(font);
    Text highScoreText(font);
    Text comboText(font);

    highScoreText.setFillColor(Color::White);
    highScoreText.setCharacterSize(55);
    highScoreText.setPosition(Vector2f(20, 125));

    comboText.setFillColor(Color::White);
    comboText.setCharacterSize(55);
    comboText.setPosition(Vector2f(20, 190));

    messageText.setString("Press Enter to start!");
    scoreText.setString("Score = 0");

    {
        std::stringstream ss;
        ss << "Best = " << highScore;
        highScoreText.setString(ss.str());
    }
    comboText.setString("");

    messageText.setCharacterSize(75);
    scoreText.setCharacterSize(100);

    messageText.setFillColor(Color::White);
    scoreText.setFillColor(Color::White);

    FloatRect textRect = messageText.getLocalBounds();
    messageText.setOrigin(textRect.getCenter());

    messageText.setPosition(Vector2f(1920 / 2.0f, 1080 / 2.0f));
    scoreText.setPosition(Vector2f(20, 20));

    RectangleShape rect1;
    rect1.setFillColor(Color(0, 0, 0, 150));
    rect1.setSize(Vector2f(600, 105));
    rect1.setPosition(Vector2f(0, 30));

    Texture textureBranch;
    if (!textureBranch.loadFromFile("../graphics/branch.png")) {
        std::cout << "Error loading branch texture" << std::endl;

        return -1;
    }
    branches.clear();
    branches.resize(NUM_BRANCHES, Sprite(textureBranch));

    for (int i = 0; i < NUM_BRANCHES; i++) {
        branches[i].setTexture(textureBranch);
        branches[i].setPosition(Vector2f(-2000, -2000));
        branches[i].setOrigin(Vector2f(220, 20));
    }

    Texture texturePlayer;
    if (!texturePlayer.loadFromFile("../graphics/player.png")) {
        std::cout << "Error loading player texture" << std::endl;

        return -1;
    }
    Sprite spritePlayer(texturePlayer);
    spritePlayer.setPosition(Vector2f(2000, 2000));

    side playerSide = side::LEFT;

    Texture textureRIP;
    if (!textureRIP.loadFromFile("../graphics/rip.png")) {
        std::cout << "Error loading gravestone texture" << std::endl;

        return -1;
    }
    Sprite spriteRIP(textureRIP);
    spriteRIP.setPosition(Vector2f(2000, 2000));

    Texture textureAxe;
    if (!textureAxe.loadFromFile("../graphics/axe.png")) {
        std::cout << "Error loading axe texture" << std::endl;

        return -1;
    }
    Sprite spriteAxe(textureAxe);

    const float AXE_POSITION_Y = 830;
    const float AXE_POSITION_LEFT = 700;
    const float AXE_POSITION_RIGHT = 1075;

    spriteAxe.setPosition(Vector2f(700, AXE_POSITION_Y));
    spriteAxe.setPosition(Vector2f(2000, AXE_POSITION_Y));

    Texture textureLog;
    if (!textureLog.loadFromFile("../graphics/log.png")) {
        std::cout << "Error loading log texture" << std::endl;

        return -1;
    }
    Sprite spriteLog(textureLog);
    spriteLog.setPosition(Vector2f(810, 720));

    bool logActive = false;
    float logSpeedX = 1000;
    float logSpeedY = -1500;

    bool acceptInput = false;

    SoundBuffer chopBuffer;
    if (!chopBuffer.loadFromFile("../sound/chop.wav")) {
        std::cout << "Error loading chop sound" << std::endl;

        return -1;
    }
    Sound chop(chopBuffer);

    SoundBuffer deathBuffer;
    if (!deathBuffer.loadFromFile("../sound/death.wav")) {
        std::cout << "Error loading death sound" << std::endl;

        return -1;
    }
    Sound death(deathBuffer);

    SoundBuffer ootBuffer;
    if (!ootBuffer.loadFromFile("../sound/out_of_time.wav")) {
        std::cout << "Error loading out of time sound" << std::endl;

        return -1;
    }
    Sound outOfTime(ootBuffer);

    while (window.isOpen()) {
        while (const std::optional<Event> event = window.pollEvent()) {
            if (event->is<Event::Closed>()) {
                window.close();
            }

            if (event->is<Event::KeyReleased>() && !paused) {
                acceptInput = true;

                spriteAxe.setPosition(
                    Vector2f(2000, spriteAxe.getPosition().y));
            }
        }

        if (Keyboard::isKeyPressed(Keyboard::Key::Escape)) {
            window.close();
        }

        if (Keyboard::isKeyPressed(Keyboard::Key::Enter)) {
            paused = false;

            timeBar.setPosition(
                Vector2f((1920 / 2) - timeBarStartWidth / 2, 980)
            );

            score = 0;
            combo = 0;
            comboMultiplier = 1;
            difficultyMultiplier = 1.0f;
            timeRemaining = 6.0f;

            for (int i = 1; i < NUM_BRANCHES; i++) {
                branchPositions[i] = side::NONE;
            }

            spriteRIP.setPosition(Vector2f(675, 2000));
            spritePlayer.setPosition(Vector2f(580, 720));

            playerSide = side::LEFT;

            spriteAxe.setPosition(Vector2f(2000, spriteAxe.getPosition().y));
            spriteLog.setPosition(Vector2f(810, 720));
            logActive = false;

            timeBar.setSize(Vector2f(timeBarStartWidth, timeBarHeight));

            scoreText.setString("Score = 0");
            comboText.setString("");

            messageText.setString("Press Enter to start!");
            centerText(messageText, Vector2f(1920 / 2.0f, 1080 / 2.0f));

            acceptInput = true;
            clock.restart();
        }

        if (acceptInput) {
            if (Keyboard::isKeyPressed(Keyboard::Key::Right)) {
                playerSide = side::RIGHT;

                score++;
                combo++;
                comboMultiplier = 1 + (combo / 5);

                updateDifficulty(score);

                float timeBonus = (0.45f / difficultyMultiplier)
                                  + (0.10f * comboMultiplier);
                timeRemaining += timeBonus;
                timeRemaining = std::min(timeRemaining, 6.0f);

                spriteAxe.setPosition(Vector2f(AXE_POSITION_RIGHT, spriteAxe.getPosition().y));

                spritePlayer.setPosition(Vector2f(1200, 720));

                updateBranches(score, difficultyMultiplier);

                spriteLog.setPosition(Vector2f(810, 720));
                logSpeedX = -5000;
                logActive = true;

                acceptInput = false;

                {
                    std::stringstream ss;
                    ss << "Score = " << score;
                    scoreText.setString(ss.str());

                    std::stringstream cs;
                    cs << "Combo x" << comboMultiplier;
                    comboText.setString(cs.str());
                }

                if (score > highScore) {
                    highScore = score;
                    saveHighScore();

                    std::stringstream hs;
                    hs << "Best = " << highScore;
                    highScoreText.setString(hs.str());
                }

                triggerShake(4.0f, 0.08f);

                chop.play();
            }

            if (Keyboard::isKeyPressed(Keyboard::Key::Left)) {
                playerSide = side::LEFT;

                score++;
                combo++;
                comboMultiplier = 1 + (combo / 5);

                updateDifficulty(score);

                float timeBonus = (0.45f / difficultyMultiplier)
                                  + (0.10f * comboMultiplier);
                timeRemaining += timeBonus;
                timeRemaining = std::min(timeRemaining, 6.0f);

                spriteAxe.setPosition(Vector2f(AXE_POSITION_LEFT, spriteAxe.getPosition().y));
                spritePlayer.setPosition(Vector2f(580, 720));

                updateBranches(score, difficultyMultiplier);

                spriteLog.setPosition(Vector2f(810, 720));
                logSpeedX = 5000;
                logActive = true;

                acceptInput = false;

                {
                    std::stringstream ss;
                    ss << "Score = " << score;
                    scoreText.setString(ss.str());

                    std::stringstream cs;
                    cs << "Combo x" << comboMultiplier;
                    comboText.setString(cs.str());
                }

                if (score > highScore) {
                    highScore = score;
                    saveHighScore();

                    std::stringstream hs;
                    hs << "Best = " << highScore;
                    highScoreText.setString(hs.str());
                }

                triggerShake(4.0f, 0.08f);

                chop.play();
            }
        }

        if (!paused) {
            Time dt = clock.restart();

            timeRemaining -= dt.asSeconds();

            timeRemaining = std::max(0.0f, timeRemaining);
            timeBar.setSize(Vector2f(
                timeBarWidthPerSecond * timeRemaining,
                timeBarHeight
            ));

            updateDifficulty(score);

            if (timeRemaining <= 0.0f) {
                paused = true;

                messageText.setString("Out of time!");

                FloatRect textRect = messageText.getLocalBounds();
                messageText.setOrigin(textRect.getCenter());

                messageText.setPosition(Vector2f(1920 / 2.0f, 1080 / 2.0f));

                combo = 0;
                comboMultiplier = 1;
                comboText.setString("");

                outOfTime.play();
            }

            if (!beeActive) {
                srand((int) time(0));
                beeSpeed = ((rand() % 200) + 200) * difficultyMultiplier;

                srand((int) time(0) * 10);
                float height = (rand() % 500) + 500;
                spriteBee.setPosition(Vector2f(2000, height));
                beeActive = true;
            } else {
                spriteBee.setPosition(Vector2f(spriteBee.getPosition().x - (beeSpeed * dt.asSeconds()),
                                               spriteBee.getPosition().y));

                if (spriteBee.getPosition().x < -100) {
                    beeActive = false;
                }
            }

            for (int i = 0; i < NUM_CLOUDS; i++) {
                if (!cloudsActive[i]) {
                    cloudSpeeds[i] = 60 + (rand() % 100);

                    float height = 40.0f + static_cast<float>(rand() % 400);
                    clouds[i].setPosition(Vector2f(-300.0f, height));
                    cloudsActive[i] = true;
                } else {
                    float speed = cloudSpeeds[i] * (0.8f + difficultyMultiplier * 0.2f);

                    clouds[i].setPosition(Vector2f(
                        clouds[i].getPosition().x + speed * dt.asSeconds(),
                        clouds[i].getPosition().y
                    ));

                    if (clouds[i].getPosition().x > 2100.0f) {
                        cloudsActive[i] = false;
                    }
                }
            }

            {
                std::stringstream hs;
                hs << "Best = " << highScore;
                highScoreText.setString(hs.str());
            }

            for (int i = 0; i < NUM_BRANCHES; i++) {
                float height = i * 150;

                if (branchPositions[i] == side::LEFT) {
                    branches[i].setPosition(Vector2f(610, height));
                    branches[i].setOrigin(Vector2f(220, 40));
                    branches[i].setRotation(degrees(180));
                } else if (branchPositions[i] == side::RIGHT) {
                    branches[i].setPosition(Vector2f(1330, height));
                    branches[i].setOrigin(Vector2f(220, 40));
                    branches[i].setRotation(degrees(0));
                } else {
                    branches[i].setPosition(Vector2f(3000, height));
                }
            }

            if (logActive) {
                spriteLog.setPosition(Vector2f(spriteLog.getPosition().x + (logSpeedX * dt.asSeconds()),
                                               spriteLog.getPosition().y + (logSpeedY * dt.asSeconds())));

                if (spriteLog.getPosition().x < -100 || spriteLog.getPosition().x > 2000) {
                    logActive = false;
                    spriteLog.setPosition(Vector2f(810, 720));
                }
            }

            if (branchPositions[5] == playerSide) {
                paused = true;
                acceptInput = false;

                timeBar.setPosition(Vector2f(2000, 980));

                if (playerSide == side::LEFT) {
                    spriteRIP.setPosition(Vector2f(580, 760));
                } else {
                    spriteRIP.setPosition(Vector2f(1200, 760));
                }

                spritePlayer.setPosition(Vector2f(2000, 660));
                spriteAxe.setPosition(Vector2f(2000, spriteAxe.getPosition().y));

                messageText.setString("SQUISHED!");

                FloatRect textRect = messageText.getLocalBounds();
                messageText.setOrigin(textRect.getCenter());

                messageText.setPosition(Vector2f(1920 / 2.0f, 1080 / 2.0f));

                combo = 0;
                comboMultiplier = 1;
                comboText.setString("");

                triggerShake(18.0f, 0.35f);

                death.play();
            }
        }

        window.clear();

        if (shakeTime > 0.0f) {
            float intensity = shakeStrength * (shakeTime / 0.35f);
            float offsetX = (static_cast<float>(rand() % 200) / 100.0f - 1.0f) * intensity;
            float offsetY = (static_cast<float>(rand() % 200) / 100.0f - 1.0f) * intensity;

            gameView.setCenter(Vector2f(
                960.0f + offsetX,
                540.0f + offsetY
            ));
        } else {
            gameView.setCenter(Vector2f(960.0f, 540.0f));
        }

        window.setView(gameView);

        window.draw(spriteBackground);

        for (int i = 0; i < NUM_CLOUDS; i++) {
            window.draw(clouds[i]);
        }

        for (int i = 0; i < NUM_BRANCHES; i++) {
            window.draw(branches[i]);
        }

        window.draw(spriteTree);
        window.draw(spritePlayer);
        window.draw(spriteAxe);
        window.draw(spriteLog);
        window.draw(spriteRIP);
        window.draw(spriteBee);

        window.setView(window.getDefaultView());

        window.draw(rect1);
        window.draw(scoreText);
        window.draw(highScoreText);
        window.draw(comboText);
        window.draw(timeBar);

        if (paused) {
            window.draw(messageText);
        }

        Time frameDt = frameClock.restart();
        if (shakeTime > 0.0f) {
            shakeTime -= frameDt.asSeconds();

            if (shakeTime <= 0.0f) {
                shakeTime = 0.0f;
                shakeStrength = 0.0f;
            }
        }

        window.display();
    }

    return 0;
}

void updateBranches(int seed, float difficulty) {
    for (int j = NUM_BRANCHES - 1; j > 0; j--) {
        branchPositions[j] = branchPositions[j - 1];
    }

    int dangerousChance = static_cast<int>(40.0f + (difficulty - 1.0f) * 20.0f);
    dangerousChance = std::min(dangerousChance, 70);

    int roll = rand() % 100;

    if (roll < dangerousChance / 2) {
        branchPositions[0] = side::LEFT;
    } else if (roll < dangerousChance) {
        branchPositions[0] = side::RIGHT;
    } else {
        branchPositions[0] = side::NONE;
    }
}

void loadHighScore() {
    std::ifstream file(HIGH_SCORE_FILE);

    if (file) {
        file >> highScore;
    }
}

void saveHighScore() {
    std::ofstream file(HIGH_SCORE_FILE);

    if (file) {
        file << highScore;
    }
}

void triggerShake(float strength, float duration) {
    shakeStrength = std::max(shakeStrength, strength);
    shakeTime = std::max(shakeTime, duration);
}

void updateDifficulty(int score) {
    difficultyMultiplier = 1.0f + (score / 10) * 0.12f;
    difficultyMultiplier = std::min(difficultyMultiplier, 2.5f);
}

void centerText(Text &text, Vector2f position) {
    FloatRect textRect = text.getLocalBounds();
    text.setOrigin(textRect.getCenter());
    text.setPosition(position);
}
