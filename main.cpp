// Include important C++ libraries here
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

// Function declaration
void updateBranches(int seed, float difficulty);

// Read the saved high score when the game starts.
void loadHighScore();

// Save the high score whenever it changes.
void saveHighScore();

// Start a screen-shake effect.
void triggerShake(float strength, float duration);

// Difficulty is based on score rather than time played.
// Every 10 points makes the game noticeably harder, with a sensible cap.
void updateDifficulty(int score);

// Update the text origin after changing a centered message.
void centerText(Text &text, Vector2f position);

const int NUM_BRANCHES = 6;
// Sprite branches[NUM_BRANCHES];
std::vector<Sprite> branches;

// Where is the player/branch?
// Left or Right
enum class side { LEFT, RIGHT, NONE };

side branchPositions[NUM_BRANCHES];

// Persistent game data
int highScore = 0;
const std::string HIGH_SCORE_FILE = "highscore.txt";

// Difficulty starts at 1.0 and rises as the score increases.
float difficultyMultiplier = 1.0f;

// Screen shake state
float shakeTime = 0.0f;
float shakeStrength = 0.0f;

// This is where our game starts from
int main() {
    loadHighScore();
    std::srand(static_cast<unsigned>(std::time(nullptr)));

    // Create a video mode object - SFML 3.0 uses Vector2u
    VideoMode vm(Vector2u(1920, 1080));

    // Create and open a window for the game
    RenderWindow window(vm, "Timber!!!", State::Windowed);
    View gameView = window.getDefaultView();

    // Create a texture to hold a graphic on the GPU
    Texture textureBackground;
    // Load a graphic into the texture with error checking
    if (!textureBackground.loadFromFile("../graphics/background.png")) {
        std::cout << "Error loading background texture" << std::endl;

        return -1;
    }
    // Create a sprite and attach the texture to the sprite
    Sprite spriteBackground(textureBackground);
    // Set the spriteBackground to cover the screen
    spriteBackground.setPosition(Vector2f(0.0f, 0.0f));

    // Make a tree sprite
    Texture textureTree;
    if (!textureTree.loadFromFile("../graphics/tree.png")) {
        std::cout << "Error loading tree texture" << std::endl;

        return -1;
    }
    Sprite spriteTree(textureTree);
    spriteTree.setPosition(Vector2f(810.0f, 0.0f));

    // More tree sprites
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

    // Prepare the bee
    Texture textureBee;
    if (!textureBee.loadFromFile("../graphics/bee.png")) {
        std::cout << "Error loading bee texture" << std::endl;

        return -1;
    }
    Sprite spriteBee(textureBee);
    spriteBee.setPosition(Vector2f(0, 800));

    // Is the bee currently moving ?
    bool beeActive = false;

    // How fast can the bee fly
    float beeSpeed = 0.0f;

    // Make 3 cloud sprites from 1 texture
    Texture textureCloud;
    // Load texture
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

    // 3 New sprites with the same texture
    // Sprite spriteCloud1(textureCloud);
    // Sprite spriteCloud2(textureCloud);
    // Sprite spriteCloud3(textureCloud);
    // Position the clouds on the left of the screen at different heights
    // spriteCloud1.setPosition(Vector2f(0.0f, 0.0f));
    // spriteCloud2.setPosition(Vector2f(0.0f, 250.0f));
    // spriteCloud3.setPosition(Vector2f(0.0f, 500.0f));
    // Are the clouds currently on screen?
    // bool cloud1Active = false;
    // bool cloud2Active = false;
    // bool cloud3Active = false;
    // How fast is each cloud ?
    // float cloud1Speed = 0.0f;
    // float cloud2Speed = 0.0f;
    // float cloud3Speed = 0.0f;

    // Variables to control time itself
    Clock clock;
    Clock frameClock;

    // Time bar
    RectangleShape timeBar;
    float timeBarStartWidth = 400;
    float timeBarHeight = 80;
    timeBar.setSize(Vector2f(timeBarStartWidth, timeBarHeight));
    timeBar.setFillColor(Color::Red);
    // timeBar.setPosition(Vector2f((1920 / 2) - timeBarStartWidth / 2, 980));
    timeBar.setPosition(Vector2f(2000, 980));

    Time gameTimeTotal;
    float timeRemaining = 6.0f;
    float timeBarWidthPerSecond = timeBarStartWidth / timeRemaining;

    // Track whether the game is running
    bool paused = true;
    // Draw some text
    int score = 0;

    // Combo increases every successful chop and is used as a time bonus.
    int combo = 0;
    int comboMultiplier = 1;

    // We need to choose a font
    Font font;
    if (!font.openFromFile("../fonts/KOMIKAP_.ttf")) {
        std::cout << "Error loading font" << std::endl;

        return -1;
    }

    Text messageText(font);
    Text scoreText(font);
    // Text fpsText(font);
    Text highScoreText(font);
    Text comboText(font);

    // Set up the fps text
    // fpsText.setFillColor(Color::White);
    // fpsText.setCharacterSize(100);
    // fpsText.setPosition(Vector2f(1200, 20));

    highScoreText.setFillColor(Color::White);
    highScoreText.setCharacterSize(55);
    highScoreText.setPosition(Vector2f(20, 125));

    comboText.setFillColor(Color::White);
    comboText.setCharacterSize(55);
    comboText.setPosition(Vector2f(20, 190));

    // Assign the actual message
    messageText.setString("Press Enter to start!");
    scoreText.setString("Score = 0");

    {
        std::stringstream ss;
        ss << "Best = " << highScore;
        highScoreText.setString(ss.str());
    }
    comboText.setString("");

    // Make it really big
    messageText.setCharacterSize(75);
    scoreText.setCharacterSize(100);

    // Choose a color
    messageText.setFillColor(Color::White);
    scoreText.setFillColor(Color::White);

    // Position the text
    FloatRect textRect = messageText.getLocalBounds();
    messageText.setOrigin(textRect.getCenter());

    messageText.setPosition(Vector2f(1920 / 2.0f, 1080 / 2.0f));
    scoreText.setPosition(Vector2f(20, 20));

    // Backgrounds for the text
    RectangleShape rect1;
    rect1.setFillColor(Color(0, 0, 0, 150));
    rect1.setSize(Vector2f(600, 105));
    rect1.setPosition(Vector2f(0, 30));

    // RectangleShape rect2;
    // rect2.setFillColor(Color(0, 0, 0, 150));
    // rect2.setSize(Vector2f(650, 105));
    // rect2.setPosition(Vector2f(1250, 30));

    // Prepare 5 branches
    Texture textureBranch;
    if (!textureBranch.loadFromFile("../graphics/branch.png")) {
        std::cout << "Error loading branch texture" << std::endl;

        return -1;
    }

    // Create the 6 sprites (they must be constructed with a texture)
    branches.clear();
    branches.resize(NUM_BRANCHES, Sprite(textureBranch));

    // Set the texture for each branch sprite
    for (int i = 0; i < NUM_BRANCHES; i++) {
        branches[i].setTexture(textureBranch);
        branches[i].setPosition(Vector2f(-2000, -2000));

        // Set the sprite's origin to dead centre
        // We can then spinit round without changing its position
        branches[i].setOrigin(Vector2f(220, 20));
    }

    // Prepare the player
    Texture texturePlayer;
    if (!texturePlayer.loadFromFile("../graphics/player.png")) {
        std::cout << "Error loading player texture" << std::endl;

        return -1;
    }
    Sprite spritePlayer(texturePlayer);
    // spritePlayer.setPosition(Vector2f(580, 720));

    // Hide player before game starts
    spritePlayer.setPosition(Vector2f(2000, 2000));

    // The player starts on the left
    side playerSide = side::LEFT;

    // Prepare the gravestone
    Texture textureRIP;
    if (!textureRIP.loadFromFile("../graphics/rip.png")) {
        std::cout << "Error loading gravestone texture" << std::endl;

        return -1;
    }
    Sprite spriteRIP(textureRIP);
    // spriteRIP.setPosition(Vector2f(600, 860));

    // Hide gravestone before game starts
    spriteRIP.setPosition(Vector2f(2000, 2000));

    // Prepare the axe
    Texture textureAxe;
    if (!textureAxe.loadFromFile("../graphics/axe.png")) {
        std::cout << "Error loading axe texture" << std::endl;

        return -1;
    }
    Sprite spriteAxe(textureAxe);
    // spriteAxe.setPosition(Vector2f(700, 830));

    // Line the axe up with the tree
    const float AXE_POSITION_Y = 830;
    const float AXE_POSITION_LEFT = 700;
    const float AXE_POSITION_RIGHT = 1075;

    // Normal axe position
    spriteAxe.setPosition(Vector2f(700, AXE_POSITION_Y));
    // Hide axe before game starts
    spriteAxe.setPosition(Vector2f(2000, AXE_POSITION_Y));

    // Prepare the flying Log
    Texture textureLog;
    if (!textureLog.loadFromFile("../graphics/log.png")) {
        std::cout << "Error loading log texture" << std::endl;

        return -1;
    }
    Sprite spriteLog(textureLog);
    spriteLog.setPosition(Vector2f(810, 720));

    // Some other useful log related variables
    bool logActive = false;
    float logSpeedX = 1000;
    float logSpeedY = -1500;

    // Control the player input
    bool acceptInput = false;

    // Prepare the sound
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

    // Out of time
    SoundBuffer ootBuffer;
    if (!ootBuffer.loadFromFile("../sound/out_of_time.wav")) {
        std::cout << "Error loading out of time sound" << std::endl;

        return -1;
    }
    Sound outOfTime(ootBuffer);

    /*
    *Ts
    updateBranches(1);
    updateBranches(2);
    updateBranches(3);
    updateBranches(4);
    updateBranches(5);
    */

    // control the drawing of the score
    // int lastDrawn = 0;

    while (window.isOpen()) {
        /*
        **************************************** Handle the players input
        ****************************************
        */
        // while (const std::optional<Event> event = window.pollEvent()) {
        //     // Check for specific event types using getIf
        //     if (const auto *closeEvent = event->getIf<Event::Closed>()) {
        //         window.close();
        //     }
        // }
        while (const std::optional<Event> event = window.pollEvent()) {
            // Close window
            if (event->is<Event::Closed>()) {
                window.close();
            }

            // Key released
            if (event->is<Event::KeyReleased>() && !paused) {
                acceptInput = true;

                // Hide the axe
                spriteAxe.setPosition(
                    Vector2f(2000, spriteAxe.getPosition().y));
            }
        }

        // Event event;
        // while (window.pollEvent(event)) {
        //     if (event.type == Event::KeyReleased && !paused) {
        //         // Listen for key presses again
        //         acceptInput = true;
        //
        //         // hide the axe
        //         spriteAxe.setPosition(Vector2f(2000, spriteAxe.getPosition().y));
        //     }
        // }
        // while (const std::optional<Event> event = window.pollEvent()) {
        //     if (event->is<Event::KeyReleased>() && !paused) {
        //         // Listen for key presses again
        //         acceptInput = true;
        //
        //         // Hide the axe
        //         spriteAxe.setPosition(
        //             Vector2f(2000, spriteAxe.getPosition().y)
        //         );
        //     }
        // }

        /*
        ****************************************
        Handle the players input
        ****************************************
        */

        if (Keyboard::isKeyPressed(Keyboard::Key::Escape)) {
            window.close();
        }

        // Start the game
        if (Keyboard::isKeyPressed(Keyboard::Key::Enter)) {
            paused = false;

            // Show the time bar
            timeBar.setPosition(
                Vector2f((1920 / 2) - timeBarStartWidth / 2, 980)
            );

            // Reset the game state.
            score = 0;
            combo = 0;
            comboMultiplier = 1;
            difficultyMultiplier = 1.0f;
            timeRemaining = 6.0f;

            // Make all the branches disappear
            for (int i = 1; i < NUM_BRANCHES; i++) {
                branchPositions[i] = side::NONE;
            }

            // Make sure the gravestone is hidden
            spriteRIP.setPosition(Vector2f(675, 2000));

            // Move the player into position.
            spritePlayer.setPosition(Vector2f(580, 720));
            playerSide = side::LEFT;

            // Reset the axe/log.
            spriteAxe.setPosition(Vector2f(2000, spriteAxe.getPosition().y));
            spriteLog.setPosition(Vector2f(810, 720));
            logActive = false;

            // Reset the timer bar.
            timeBar.setSize(Vector2f(timeBarStartWidth, timeBarHeight));

            scoreText.setString("Score = 0");
            comboText.setString("");

            messageText.setString("Press Enter to start!");
            centerText(messageText, Vector2f(1920 / 2.0f, 1080 / 2.0f));

            acceptInput = true;
            clock.restart();
        }

        // Wrap the player controls to
        // Make sure we are accepting input
        if (acceptInput) {
            // First handle pressing the right cursor key
            if (Keyboard::isKeyPressed(Keyboard::Key::Right)) {
                // Make sure the player is on the right
                playerSide = side::RIGHT;

                score++;
                combo++;
                comboMultiplier = 1 + (combo / 5);

                updateDifficulty(score);

                // Add to the amount of time remaining
                // timeRemaining += (2 / score) + .15;
                // Later chop give slightly less raw time, but a good combo
                // gives a bonus multiplier. This keep the game fast.
                float timeBonus = (0.45f / difficultyMultiplier)
                                  + (0.10f * comboMultiplier);
                timeRemaining += timeBonus;
                timeRemaining = std::min(timeRemaining, 6.0f);

                spriteAxe.setPosition(Vector2f(AXE_POSITION_RIGHT, spriteAxe.getPosition().y));

                spritePlayer.setPosition(Vector2f(1200, 720));

                // Update the branches
                updateBranches(score, difficultyMultiplier);

                // Set the log flying to the left
                spriteLog.setPosition(Vector2f(810, 720));
                logSpeedX = -5000;
                logActive = true;

                acceptInput = false;

                // Update score/combo immediately
                {
                    std::stringstream ss;
                    ss << "Score = " << score;
                    scoreText.setString(ss.str());

                    std::stringstream cs;
                    cs << "Combo x" << comboMultiplier;
                    comboText.setString(cs.str());
                }

                // Update the high score immediately.
                if (score > highScore) {
                    highScore = score;
                    saveHighScore();

                    std::stringstream hs;
                    hs << "Best = " << highScore;
                    highScoreText.setString(hs.str());
                }

                // A successful chop gives a small satisfying shake.
                triggerShake(4.0f, 0.08f);

                // Play a chop sound
                chop.play();
            }

            // Handle the left cursor key
            if (Keyboard::isKeyPressed(Keyboard::Key::Left)) {
                // Make sure the player is on the left
                playerSide = side::LEFT;

                score++;
                combo++;
                comboMultiplier = 1 + (combo / 5);

                updateDifficulty(score);

                // Add to the amount of time remaining
                // timeRemaining += (2 / score) + .15;
                float timeBonus = (0.45f / difficultyMultiplier)
                                  + (0.10f * comboMultiplier);
                timeRemaining += timeBonus;
                timeRemaining = std::min(timeRemaining, 6.0f);

                spriteAxe.setPosition(Vector2f(AXE_POSITION_LEFT, spriteAxe.getPosition().y));

                spritePlayer.setPosition(Vector2f(580, 720));

                // update the branches
                updateBranches(score, difficultyMultiplier);

                // set the log flying
                spriteLog.setPosition(Vector2f(810, 720));
                logSpeedX = 5000;
                logActive = true;

                acceptInput = false;

                // Update score/combo immediately
                {
                    std::stringstream ss;
                    ss << "Score = " << score;
                    scoreText.setString(ss.str());

                    std::stringstream cs;
                    cs << "Combo x" << comboMultiplier;
                    comboText.setString(cs.str());
                }

                // Update the high score immediately.
                if (score > highScore) {
                    highScore = score;
                    saveHighScore();

                    std::stringstream hs;
                    hs << "Best = " << highScore;
                    highScoreText.setString(hs.str());
                }

                // A successful chop gives a small satisfying shake.
                triggerShake(4.0f, 0.08f);

                // Play a chop sound
                chop.play();
            }
        }

        /*
        **************************************** Update the scene
        ****************************************
        */

        if (!paused) {
            // Measure time
            // delta time, is time between two updates
            Time dt = clock.restart();

            // Subtract from the amount of time remaining
            timeRemaining -= dt.asSeconds();
            // Keep the timer bar inside its legal range.
            // timeBar.setSize(Vector2f(timeBarWidthPerSecond * timeRemaining, timeBarHeight));
            timeRemaining = std::max(0.0f, timeRemaining);
            timeBar.setSize(Vector2f(
                timeBarWidthPerSecond * timeRemaining,
                timeBarHeight
            ));

            // Difficulty affects the world, not just branch generation.
            updateDifficulty(score);

            if (timeRemaining <= 0.0f) {
                // Pause the game
                paused = true;
                // Change the message shown to the player
                messageText.setString("Out of time!");

                // Reposition the text based on its new size
                FloatRect textRect = messageText.getLocalBounds();
                messageText.setOrigin(textRect.getCenter());

                messageText.setPosition(Vector2f(1920 / 2.0f, 1080 / 2.0f));

                // Missing the time limit breaks the combo.
                combo = 0;
                comboMultiplier = 1;
                comboText.setString("");

                // Play the out of time sound
                outOfTime.play();
            }

            // Setup the bee
            if (!beeActive) {
                // How fast is the bee
                srand((int) time(0));
                beeSpeed = ((rand() % 200) + 200) * difficultyMultiplier;

                // How high is the bee
                srand((int) time(0) * 10);
                float height = (rand() % 500) + 500;
                spriteBee.setPosition(Vector2f(2000, height));
                beeActive = true;
            } else {
                // Move the bee
                spriteBee.setPosition(Vector2f(spriteBee.getPosition().x - (beeSpeed * dt.asSeconds()),
                                               spriteBee.getPosition().y));
                // Has the bee reached the left-hand edge of the screen ?
                if (spriteBee.getPosition().x < -100) {
                    // Set it up ready to be a whole new bee next frame
                    beeActive = false;
                }
            }

            // Manage the clouds.
            // Clouds are purely visual, but their speed increases slightly with difficulty.
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

            // Score/combo are updated when a chop happens, so we do not need
            // to rebuild their strings every frame.
            {
                std::stringstream hs;
                hs << "Best = " << highScore;
                highScoreText.setString(hs.str());
            }

            // Manage the clouds
            // Cloud 1
            // if (!cloud1Active) {
            //     // How fast is the cloud
            //     srand((int) time(0) * 10);
            //     cloud1Speed = (rand() % 200);
            //
            //     // How high is the cloud
            //     srand((int) time(0) * 10);
            //     float height = (rand() % 150);
            //     spriteCloud1.setPosition(Vector2f(-200.0f, height));
            //     cloud1Active = true;
            // } else {
            //     spriteCloud1.setPosition(Vector2f(spriteCloud1.getPosition().x + (cloud1Speed * dt.asSeconds()),
            //                                       spriteCloud1.getPosition().y));
            //
            //     // Has the cloud reached the right hand edge of the screen ?
            //     if (spriteCloud1.getPosition().x > 1920) {
            //         // Set it up ready to be a whole new cloud next frame
            //         cloud1Active = false;
            //     }
            // }

            // Cloud2
            // if (!cloud2Active) {
            //     // How fast is the cloud
            //     srand((int) time(0) * 20);
            //     cloud2Speed = (rand() % 200);
            //
            //     // How hight is the cloud
            //     srand((int) time(0) * 20);
            //     float height = (rand() % 300) - 150;
            //     spriteCloud2.setPosition(Vector2f(-200.0f, height));
            //     cloud2Active = true;
            // } else {
            //     spriteCloud2.setPosition(Vector2f(spriteCloud2.getPosition().x + (cloud2Speed * dt.asSeconds()),
            //                                       spriteCloud2.getPosition().y));
            //
            //     // Has the cloud reached the right hand of the screen ?
            //     if (spriteCloud2.getPosition().x > 1920) {
            //         // Set it up ready to be a whole new cloud next frame
            //         cloud2Active = false;
            //     }
            // }

            // if (!cloud3Active) {
            //     // How fast is the cloud
            //     srand((int) time(0) * 30);
            //     cloud3Speed = (rand() % 200);
            //
            //     // How high is the cloud
            //     srand((int) time(0) * 30);
            //     float height = (rand() % 450) - 150;
            //     spriteCloud3.setPosition(Vector2f(-200.0f, height));
            //     cloud3Active = true;
            // } else {
            //     spriteCloud3.setPosition(Vector2f(spriteCloud3.getPosition().x + (cloud3Speed * dt.asSeconds()),
            //                                       spriteCloud3.getPosition().y));
            //
            //     // Has the cloud reached the right hand edge of the screen?
            //     if (spriteCloud3.getPosition().x > 1920) {
            //         // Set it up ready to be a whole new cloud next frame
            //         cloud3Active = false;
            //     }
            // }

            // Draw the score and the frame rate once every 100 frames
            // lastDrawn++;
            // if (lastDrawn == 100) {
            //     // Update the score text
            //     std::stringstream ss;
            //     ss << "Score = " << score;
            //     scoreText.setString(ss.str());
            //
            //     // Draw the fps
            //     std::stringstream ss2;
            //     ss2 << "FPS = " << 1 / dt.asSeconds();
            //     fpsText.setString(ss2.str());
            //     lastDrawn = 0;
            // }

            // update the branch sprites
            for (int i = 0; i < NUM_BRANCHES; i++) {
                float height = i * 150;

                if (branchPositions[i] == side::LEFT) {
                    // Move the sprite to the Left side
                    branches[i].setPosition(Vector2f(610, height));
                    branches[i].setOrigin(Vector2f(220, 40));
                    // Flip the sprite round the other way
                    branches[i].setRotation(degrees(180));
                } else if (branchPositions[i] == side::RIGHT) {
                    // Move the sprite to the right side
                    branches[i].setPosition(Vector2f(1330, height));
                    branches[i].setOrigin(Vector2f(220, 40));
                    // Set the sprite rotation to normal
                    branches[i].setRotation(degrees(0));
                } else {
                    // Hide the branch
                    branches[i].setPosition(Vector2f(3000, height));
                }
            }

            // Handle a flying log
            if (logActive) {
                spriteLog.setPosition(Vector2f(spriteLog.getPosition().x + (logSpeedX * dt.asSeconds()),
                                               spriteLog.getPosition().y + (logSpeedY * dt.asSeconds())));

                // Has the log reached the right hand edge ?
                if (spriteLog.getPosition().x < -100 || spriteLog.getPosition().x > 2000) {
                    // Set it up ready to be a whole new log next frame
                    logActive = false;
                    spriteLog.setPosition(Vector2f(810, 720));
                }
            }

            // Has the player been squished by a branch ?
            if (branchPositions[5] == playerSide) {
                // death
                paused = true;
                acceptInput = false;

                // Hide time bar
                timeBar.setPosition(Vector2f(2000, 980));

                // Draw the gravestone
                if (playerSide == side::LEFT) {
                    spriteRIP.setPosition(Vector2f(580, 760));
                } else {
                    spriteRIP.setPosition(Vector2f(1200, 760));
                }

                // Hide the player
                spritePlayer.setPosition(Vector2f(2000, 660));
                // Hide the axe
                spriteAxe.setPosition(Vector2f(2000, spriteAxe.getPosition().y));

                // Change the text of the message
                messageText.setString("SQUISHED!");

                // Center it on the screen
                FloatRect textRect = messageText.getLocalBounds();
                messageText.setOrigin(textRect.getCenter());

                messageText.setPosition(Vector2f(1920 / 2.0f, 1080 / 2.0f));

                // Death breaks the combo and gives strong feedback.
                combo = 0;
                comboMultiplier = 1;
                comboText.setString("");

                triggerShake(18.0f, 0.35f);

                // Play the death sound
                death.play();
            }
        }

        /*
        **************************************** Draw the scene
        ****************************************
        */

        // Clear everything from the last frame
        window.clear();

        // ---------------- WORLD ----------------
        // Only the world moves during screen shake. UI stays stable.
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

        // Draw our game scene here
        window.draw(spriteBackground);

        // Draw the clouds
        // window.draw(spriteCloud1);
        // window.draw(spriteCloud2);
        // window.draw(spriteCloud3);

        // Draw every cloud.
        for (int i = 0; i < NUM_CLOUDS; i++) {
            window.draw(clouds[i]);
        }

        for (int i = 0; i < NUM_BRANCHES; i++) {
            window.draw(branches[i]);
        }

        // Draw the tree
        window.draw(spriteTree);
        // Draw the player
        window.draw(spritePlayer);
        // Draw the axe
        window.draw(spriteAxe);
        // Draw the flying logs
        window.draw(spriteLog);
        // Draw the gravestone
        window.draw(spriteRIP);
        // Draw backgrounds for the text
        // window.draw(rect1);
        // window.draw(rect2);
        // Now draw the insect
        window.draw(spriteBee);

        // ---------------- UI ----------------
        // Reset to the normal camera so score/time/text do not shake.

        window.setView(window.getDefaultView());

        window.draw(rect1);
        // window.draw(rect2);
        // Draw the score
        window.draw(scoreText);
        window.draw(highScoreText);
        window.draw(comboText);
        // Draw the FPS
        // window.draw(fpsText);
        // Draw the timebar
        window.draw(timeBar);

        if (paused) {
            // Draw our message
            window.draw(messageText);
        }

        // Reduce shake over real frame time, even while the game is paused.
        Time frameDt = frameClock.restart();
        if (shakeTime > 0.0f) {
            shakeTime -= frameDt.asSeconds();

            if (shakeTime <= 0.0f) {
                shakeTime = 0.0f;
                shakeStrength = 0.0f;
            }
        }

        // Show everything we just drew.
        window.display();
    }

    return 0;
}

// Function definition
void updateBranches(int seed, float difficulty) {
    // Move all the branches down one place
    for (int j = NUM_BRANCHES - 1; j > 0; j--) {
        branchPositions[j] = branchPositions[j - 1];
    }

    // Spawn a new branch at position 0
    // LEFT, RIGHT or NONE
    // At low difficulty, a branch is dangerous 40% of the time.
    // As difficulty rises, the chance increases toward 70%.
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

// Read the saved high score when the game starts.
void loadHighScore() {
    std::ifstream file(HIGH_SCORE_FILE);

    if (file) {
        file >> highScore;
    }
}

// Save the high score whenever it changes.
void saveHighScore() {
    std::ofstream file(HIGH_SCORE_FILE);

    if (file) {
        file << highScore;
    }
}

// Start a screen-shake effect.
void triggerShake(float strength, float duration) {
    shakeStrength = std::max(shakeStrength, strength);
    shakeTime = std::max(shakeTime, duration);
}

// Difficulty is based on score rather than time played.
// Every 10 points makes the game noticeably harder, with a sensible cap.
void updateDifficulty(int score) {
    difficultyMultiplier = 1.0f + (score / 10) * 0.12f;
    difficultyMultiplier = std::min(difficultyMultiplier, 2.5f);
}

// Update the text origin after changing a centered message.
void centerText(Text &text, Vector2f position) {
    FloatRect textRect = text.getLocalBounds();
    text.setOrigin(textRect.getCenter());
    text.setPosition(position);
}
