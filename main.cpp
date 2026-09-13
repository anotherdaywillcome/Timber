// Include important C++ libraries here
#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include <SFML/System.hpp>
#include <SFML/Audio.hpp>
#include <sstream>
#include<iostream>

// Make code easier to type with "using namespace"
using namespace sf;

// Function declaration
void updateBranches(int seed);

const int NUM_BRANCHES = 6;
// Sprite branches[NUM_BRANCHES];
std::vector<Sprite> branches;

// Where is the player/branch?
// Left or Right
enum class side { LEFT, RIGHT, NONE };

side branchPositions[NUM_BRANCHES];

// This is where our game starts from
int main() {
    // Create a video mode object - SFML 3.0 uses Vector2u
    VideoMode vm(Vector2u(1920, 1080));

    // Create and open a window for the game
    RenderWindow window(vm, "Timber!!!", State::Windowed);

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
        // Do we need setTexture ?
        clouds[i].setTexture(textureCloud);
        clouds[i].setPosition(Vector2f(-300, i * 150));
        cloudsActive[i] = false;
        cloudSpeeds[i] = 0;
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

    // Time bar
    RectangleShape timeBar;
    float timeBarStartWidth = 400;
    float timeBarHeight = 80;
    timeBar.setSize(Vector2f(timeBarStartWidth, timeBarHeight));
    timeBar.setFillColor(Color::Red);
    timeBar.setPosition(Vector2f((1920 / 2) - timeBarStartWidth / 2, 980));

    Time gameTimeTotal;
    float timeRemaining = 6.0f;
    float timeBarWidthPerSecond = timeBarStartWidth / timeRemaining;

    // Track whether the game is running
    bool paused = true;
    // Draw some text
    int score = 0;

    // We need to choose a font
    Font font;
    if (!font.openFromFile("../fonts/KOMIKAP_.ttf")) {
        std::cout << "Error loading font" << std::endl;

        return -1;
    }

    Text messageText(font);
    Text scoreText(font);
    Text fpsText(font);

    // Set up the fps text
    fpsText.setFillColor(Color::White);
    fpsText.setCharacterSize(100);
    fpsText.setPosition(Vector2f(1200, 20));

    // Assign the actual message
    messageText.setString("Press Enter to start!");
    scoreText.setString("Score = 0");

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
    rect1.setFillColor(sf::Color(0, 0, 0, 150));
    rect1.setSize(Vector2f(600, 105));
    rect1.setPosition(Vector2f(0, 30));

    RectangleShape rect2;
    rect2.setFillColor(sf::Color(0, 0, 0, 150));
    rect2.setSize(Vector2f(1000, 105));
    rect2.setPosition(Vector2f(1150, 30));

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
    spritePlayer.setPosition(Vector2f(580, 720));

    // The player starts on the left
    side playerSide = side::LEFT;

    // Prepare the gravestone
    Texture textureRIP;
    if (!textureRIP.loadFromFile("../graphics/rip.png")) {
        std::cout << "Error loading gravestone texture" << std::endl;

        return -1;
    }
    Sprite spriteRIP(textureRIP);
    spriteRIP.setPosition(Vector2f(600, 860));

    // Prepare the axe
    Texture textureAxe;
    if (!textureAxe.loadFromFile("../graphics/axe.png")) {
        std::cout << "Error loading axe texture" << std::endl;

        return -1;
    }
    Sprite spriteAxe(textureAxe);
    spriteAxe.setPosition(Vector2f(700, 830));

    // Line the axe up with the tree
    const float AXE_POSITION_LEFT = 700;
    const float AXE_POSITION_RIGHT = 1075;

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
    int lastDrawn = 0;

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

            // Reset the time and the score
            score = 0;
            timeRemaining = 6;

            // Make all the branches disappear
            for (int i = 1; i < NUM_BRANCHES; i++) {
                branchPositions[i] = side::NONE;
            }

            // Make sure the gravestone is hidden
            spriteRIP.setPosition(Vector2f(675, 2000));

            // Move the player into position
            spritePlayer.setPosition(Vector2f(580, 720));

            acceptInput = true;
        }

        // Wrap the player controls to
        // Make sure we are accepting input
        if (acceptInput) {
            // First handle pressing the right cursor key
            if (Keyboard::isKeyPressed(Keyboard::Key::Right)) {
                // Make sure the player is on the right
                playerSide = side::RIGHT;

                score++;

                // Add to the amount of time remaining
                timeRemaining += (2 / score) + .15;

                spriteAxe.setPosition(Vector2f(AXE_POSITION_RIGHT, spriteAxe.getPosition().y));;

                spritePlayer.setPosition(Vector2f(1200, 720));

                // Update the branches
                updateBranches(score);

                // Set the log flying to the left
                spriteLog.setPosition(Vector2f(810, 720));
                logSpeedX = -5000;
                logActive = true;

                acceptInput = false;

                // Play a chop sound
                chop.play();
            }

            // Handle the left cursor key
            if (Keyboard::isKeyPressed(Keyboard::Key::Left)) {
                // Make sure the player is on the left
                playerSide = side::LEFT;

                score++;

                // Add to the amount of time remaining
                timeRemaining += (2 / score) + .15;

                spriteAxe.setPosition(Vector2f(AXE_POSITION_LEFT, spriteAxe.getPosition().y));

                spritePlayer.setPosition(Vector2f(580, 720));

                // update the branches
                updateBranches(score);

                // set the log flying
                spriteLog.setPosition(Vector2f(810, 720));
                logSpeedX = 5000;
                logActive = true;

                acceptInput = false;

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
            // size up the time bar
            timeBar.setSize(Vector2f(timeBarWidthPerSecond * timeRemaining, timeBarHeight));

            if (timeRemaining <= 0.0f) {
                // Pause the game
                paused = true;
                // Change the message shown to the player
                messageText.setString("Out of time!");

                // Reposition the text based on its new size
                FloatRect textRect = messageText.getLocalBounds();
                messageText.setOrigin(textRect.getCenter());

                messageText.setPosition(Vector2f(1920 / 2.0f, 1080 / 2.0f));

                // Play the out of time sound
                outOfTime.play();
            }

            // Setup the bee
            if (!beeActive) {
                // How fast is the bee
                srand((int) time(0));
                beeSpeed = (rand() % 200) + 200;

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

            // Manage the clouds with arrays
            for (int i = 0; i < NUM_CLOUDS; i++) {
                if (!cloudsActive[i]) {
                    // How fast is the cloud
                    srand((int) time(0) * i);
                    cloudSpeeds[i] = (rand() % 200);

                    // How high is the cloud
                    srand((int) time(0) * i);
                    float height = (rand() % 150);
                    clouds[i].setPosition(Vector2f(-200, height));
                    cloudsActive[i] = true;
                } else {
                    clouds[i].setPosition(Vector2f(clouds[i].getPosition().x + (cloudSpeeds[i] * dt.asSeconds()),
                                                   clouds[i].getPosition().y));

                    // Has the cloud reached right hand edge of the screen ?
                    if (clouds[i].getPosition().x > 1920) {
                        // Set the cloud to be a whole new cloud next frame
                        cloudsActive[i] = false;
                    }
                }
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
            lastDrawn++;
            if (lastDrawn == 100) {
                // Update the score text
                std::stringstream ss;
                ss << "Score = " << score;
                scoreText.setString(ss.str());

                // Draw the fps
                std::stringstream ss2;
                ss2 << "FPS = " << 1 / dt.asSeconds();
                fpsText.setString(ss2.str());
                lastDrawn = 0;
            }

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

                // Draw the gravestone
                spriteRIP.setPosition(Vector2f(525, 760));

                // Hide the player
                spritePlayer.setPosition(Vector2f(2000, 660));

                // Change the text of the message
                messageText.setString("SQUISHED!");

                // Center it on the screen
                FloatRect textRect = messageText.getLocalBounds();
                messageText.setOrigin(textRect.getCenter());

                messageText.setPosition(Vector2f(1920 / 2.0f, 1080 / 2.0f));

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

        // Draw our game scene here
        window.draw(spriteBackground);

        // Draw the clouds
        // window.draw(spriteCloud1);
        // window.draw(spriteCloud2);
        // window.draw(spriteCloud3);

        // Draw the branches
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
        window.draw(rect1);
        window.draw(rect2);

        // Now draw the insect
        window.draw(spriteBee);

        // Draw the score
        window.draw(scoreText);

        // Draw the FPS
        window.draw(fpsText);

        // Draw the timebar
        window.draw(timeBar);

        if (paused) {
            // Draw our message
            window.draw(messageText);
        }

        // Show everything we just drew
        window.display();
    }

    return 0;
}

// Function definition
void updateBranches(int seed) {
    // Move all the branches down one place
    for (int j = NUM_BRANCHES - 1; j > 0; j--) {
        branchPositions[j] = branchPositions[j - 1];
    }

    // Spawn a new branch at position 0
    // LEFT, RIGHT or NONE
    srand((int) time(0) + seed);
    int r = (rand() % 5);

    switch (r) {
        case 0:
            branchPositions[0] = side::LEFT;
            break;
        case 1:
            branchPositions[0] = side::RIGHT;
        default:
            branchPositions[0] = side::NONE;
            break;
    }
}
