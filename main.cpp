// Include important libraries here
#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include <SFML/System.hpp>
#include <sstream>

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
        // Handle error - texture failed to load
        return -1;
    }

    // Create a sprite and attach the texture to the sprite
    Sprite spriteBackground(textureBackground);

    // Set the spriteBackground to cover the screen
    spriteBackground.setPosition(Vector2f(0.0f, 0.0f));

    // Make a tree sprite
    Texture textureTree;
    if (!textureTree.loadFromFile("../graphics/tree.png")) {
        return -1;
    }
    Sprite spriteTree(textureTree);
    spriteTree.setPosition(Vector2f(810.0f, 0.0f));

    // Prepare the bee
    Texture textureBee;
    if (!textureBee.loadFromFile("../graphics/bee.png")) {
        return -1;
    }
    Sprite spriteBee(textureBee);
    spriteBee.setPosition(Vector2f(0.0f, 800.0f));

    // Is the bee currently moving ?
    bool beeActive = false;

    // How fast can the bee fly
    float beeSpeed = 0.0f;

    // Make 3 cloud sprites from 1 texture
    Texture textureCloud;
    // Load texture
    if (!textureCloud.loadFromFile("../graphics/cloud.png")) {
        return -1;
    }
    // 3 New sprites with the same texture
    Sprite spriteCloud1(textureCloud);
    Sprite spriteCloud2(textureCloud);
    Sprite spriteCloud3(textureCloud);
    // Position the clouds on the left of the screen at different heights
    spriteCloud1.setPosition(Vector2f(0.0f, 0.0f));
    spriteCloud2.setPosition(Vector2f(0.0f, 250.0f));
    spriteCloud3.setPosition(Vector2f(0.0f, 500.0f));
    // Are the clouds currently on screen?
    bool cloud1Active = false;
    bool cloud2Active = false;
    bool cloud3Active = false;
    // How fast is each cloud ?
    float cloud1Speed = 0.0f;
    float cloud2Speed = 0.0f;
    float cloud3Speed = 0.0f;

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
        return -1;
    }

    Text messageText(font);
    Text scoreText(font);

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
    scoreText.setPosition(Vector2f(20.0f, 20.0f));

    // Prepare 5 branches
    Texture textureBranch;
    if (!textureBranch.loadFromFile("../graphics/branch.png")) {
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

    /*
    *Ts
    updateBranches(1);
    updateBranches(2);
    updateBranches(3);
    updateBranches(4);
    updateBranches(5);
    */

    while (window.isOpen()) {
        /*
        **************************************** Handle the players input
        ****************************************
        */
        // Process events - SFML 3.0 approach
        while (const std::optional<Event> event = window.pollEvent()) {
            // Check for specific event types using getIf
            if (const auto *closeEvent = event->getIf<Event::Closed>()) {
                window.close();
            }
        }

        // Handle keyboard input
        if (Keyboard::isKeyPressed(Keyboard::Key::Escape)) {
            window.close();
        }

        // Start the game
        if (Keyboard::isKeyPressed(Keyboard::Key::Enter)) {
            paused = false;

            // Reset the time and the score
            score = 0;
            timeRemaining = 6;
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

                //Reposition the text based on its new size
                FloatRect textRect = messageText.getLocalBounds();
                messageText.setOrigin(textRect.getCenter());

                messageText.setPosition(Vector2f(1920 / 2.0f, 1080 / 2.0f));
            }

            // Setup the bee
            if (!beeActive) {
                // How fast is the bee
                srand((int) time(0));
                beeSpeed = (rand() % 200) + 200;

                // How high is the bee
                srand((int) time(0) * 10);
                float height = (rand() % 1350) + 500;
                spriteBee.setPosition(Vector2f(2000.0f, height));
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

            // Manage the clouds
            // Cloud 1
            if (!cloud1Active) {
                // How fast is the cloud
                srand((int) time(0) * 10);
                cloud1Speed = (rand() % 200);

                // How high is the cloud
                srand((int) time(0) * 10);
                float height = (rand() % 150);
                spriteCloud1.setPosition(Vector2f(-200.0f, height));
                cloud1Active = true;
            } else {
                spriteCloud1.setPosition(Vector2f(spriteCloud1.getPosition().x + (cloud1Speed * dt.asSeconds()),
                                                  spriteCloud1.getPosition().y));

                // Has the cloud reached the right hand edge of the screen ?
                if (spriteCloud1.getPosition().x > 1920) {
                    // Set it up ready to be a whole new cloud next frame
                    cloud1Active = false;
                }
            }

            // Cloud2
            if (!cloud2Active) {
                // How fast is the cloud
                srand((int) time(0) * 20);
                cloud2Speed = (rand() % 200);

                // How hight is the cloud
                srand((int) time(0) * 20);
                float height = (rand() % 300) - 150;
                spriteCloud2.setPosition(Vector2f(-200.0f, height));
                cloud2Active = true;
            } else {
                spriteCloud2.setPosition(Vector2f(spriteCloud2.getPosition().x + (cloud2Speed * dt.asSeconds()),
                                                  spriteCloud2.getPosition().y));

                // Has the cloud reached the right hand of the screen ?
                if (spriteCloud2.getPosition().x > 1920) {
                    // Set it up ready to be a whole new cloud next frame
                    cloud2Active = false;
                }
            }

            if (!cloud3Active) {
                // How fast is the cloud
                srand((int) time(0) * 30);
                cloud3Speed = (rand() % 200);

                // How high is the cloud
                srand((int) time(0) * 30);
                float height = (rand() % 450) - 150;
                spriteCloud3.setPosition(Vector2f(-200.0f, height));
                cloud3Active = true;
            } else {
                spriteCloud3.setPosition(Vector2f(spriteCloud3.getPosition().x + (cloud3Speed * dt.asSeconds()),
                                                  spriteCloud3.getPosition().y));

                // Has the cloud reached the right hand edge of the screen?
                if (spriteCloud3.getPosition().x > 1920) {
                    // Set it up ready to be a whole new cloud next frame
                    cloud3Active = false;
                }
            }

            // Update the score text
            std::stringstream ss;
            ss << "Score = " << score;
            scoreText.setString(ss.str());

            // update the branch sprites
            for (int i = 0; i < NUM_BRANCHES; i++) {
                float height = i * 150;

                if (branchPositions[i] == side::LEFT) {
                    // Move the sprite to the Left side
                    branches[i].setPosition(Vector2f(610, height));

                    // Flip the sprite round the other way
                    branches[i].setRotation(degrees(180));
                } else if (branchPositions[i] == side::RIGHT) {
                    // Move the sprite to the right side
                    branches[i].setPosition(Vector2f(1330, height));

                    // Set the sprite rotation to normal
                    branches[i].setRotation(degrees(0));
                } else {
                    // Hide the branch
                    branches[i].setPosition(Vector2f(3000, height));
                }
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
        window.draw(spriteCloud1);
        window.draw(spriteCloud2);
        window.draw(spriteCloud3);

        // Draw the branches
        for (int i = 0; i < NUM_BRANCHES; i++) {
            window.draw(branches[i]);
        }

        // Draw the tree
        window.draw(spriteTree);

        // Now draw the insect
        window.draw(spriteBee);

        // Draw the score
        window.draw(scoreText);

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
