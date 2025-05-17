
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <iostream>
#include <fstream>

using namespace sf;
using namespace std;

// Game States
enum ScreenState { MAIN_MENU, LEVEL_MENU, GAME_PLAY, GAME_OVER };
// Game configuration
struct GameConfig {
    int maxBullets, maxEnemies, maxHoneycombs;
    float playerSpeed, enemySpeed, spawnInterval;
};
// Player class
class Player{
public:
    Player(){
        sprite.setTexture(playerTexture);
        sprite.setPosition(400,550);
        speed=5.0f;
    }

    void movePlayer(){
        if (Keyboard::isKeyPressed(Keyboard::Right) && sprite.getPosition().x+sprite.getGlobalBounds().width<960){
            sprite.move(speed, 0);
        }
        if (Keyboard::isKeyPressed(Keyboard::Left)&&sprite.getPosition().x > 0){
            sprite.move(-speed, 0);
        }
    }
    bool shootBullet(Sprite bullets[], int& bulletcount, int maxBullets, int& totalBulletsFired) 
    {
        if (Keyboard::isKeyPressed(Keyboard::Space) && bulletcount<maxBullets&& canShoot) {
            bullets[bulletcount].setTexture(bulletTexture);
            bullets[bulletcount].setPosition(sprite.getPosition().x +sprite.getGlobalBounds().width/2, sprite.getPosition().y);
            bulletcount++;
            totalBulletsFired++;
            canShoot=false;
            return true;
        }
        if (!Keyboard::isKeyPressed(Keyboard::Space)){
            canShoot=true;
        }
        return false;
    }
    void draw(RenderWindow& window){
        window.draw(sprite);
    }
    FloatRect getBounds(){
        return sprite.getGlobalBounds();
    }
    void setSpeed(float speed){
        this->speed = speed;
    }
    static void loadTextures(){
        cout << "Loading player texture..." << endl;
        if (!playerTexture.loadFromFile("/Textures/spray.png")){
            cerr << "Failed to load player texture! Exiting..." << endl;
            exit(1);
        }
        cout << "Loading bullet texture..." << endl;
        if (!bulletTexture.loadFromFile("/Textures/bullet.png")){
            cerr << "Failed to load bullet texture! Exiting..." << endl;
            exit(1);
        }
    }

private:
    static Texture playerTexture;
    static Texture bulletTexture;
    Sprite sprite;
    float speed;
    bool canShoot =true;
};
Texture Player::playerTexture;
Texture Player::bulletTexture;
// Enemy class
class Enemy {
public:
    Enemy():speed(0.5f),direction(-1),active(false){
        sprite.setTexture(enemyTexture);
        sprite.setPosition(0,0);
        sprite.setScale(-1,1); // Inverted for left movement
    }
    Enemy(float x,float y){
        sprite.setTexture(enemyTexture);
        sprite.setPosition(x, y);
        speed = 0.5f;
        direction = -1; // Start moving left
        active = true;
        sprite.setScale(-1, 1); // Inverted for left movement
    }
    void move(){
        if (active){
            sprite.move(direction*speed,0);
            FloatRect bounds=sprite.getGlobalBounds();
            if (bounds.left<=0) {
                direction=1; // Hit left edge, move right
                sprite.setScale(1,1); // Normal image
            } else if (bounds.left + bounds.width>=960){
                direction=-1; // Hit right edge, move left
                sprite.setScale(-1,1); // Inverted image
            }
        }
    }
    void draw(RenderWindow& window) 
    {
        if (active) {
            window.draw(sprite);
        }
    }
    FloatRect getBounds(){
        return active ?sprite.getGlobalBounds():FloatRect(0,0,0,0);
    }
    void setSpeed(float speed){
        this->speed = speed;
    }

    bool isActive(){
        return active;
    }

    void setActive(bool state){
        active = state;
    }
    static void loadTexture(){
        cout <<"Loading enemy texture..." <<endl;
        if (!enemyTexture.loadFromFile("Textures/Regular_bee.png")){
            cerr<<"Failed to load enemy texture! Exiting..."<<endl;
            exit(1);
        }
        cout<<"Enemy texture loaded successfully."<<endl;
    }
private:
    static Texture enemyTexture;
    Sprite sprite;
    float speed;
    int direction; // 1 for right, -1 for left
    bool active;
};
Texture Enemy::enemyTexture;
// Honeycomb class
class Honeycomb{
public:
    Honeycomb() : points(0), active(false),speed(2.0f){
        sprite.setTexture(honeycombTexture);
        sprite.setPosition(0,0);
    }
    Honeycomb(float x, float y){
        sprite.setTexture(honeycombTexture);
        sprite.setPosition(x,y);
        points = (rand()%4 +1)*500;
        active =true;
        speed =2.0f;
    }
    void move(){
        // Stationary, no movement
    }
    void draw(RenderWindow& window){
        if (active) {
            window.draw(sprite);
        }
    }
    FloatRect getBounds(){
        return active?sprite.getGlobalBounds():FloatRect(0,0,0,0);
    }
    int getPoints(){
        return points;
    }
    bool isActive() 
    {
        return active;
    }
    void setActive(bool state) 
    {
        active = state;
    }
    static void loadTexture(){
        cout << "Loading honeycomb texture..." << endl;
        if (!honeycombTexture.loadFromFile("Textures/honeycomb.png")){
            cerr<<"Failed to load honeycomb texture! Exiting..."<<endl;
            exit(1);
        }
        cout<<"Honeycomb texture loaded successfully."<<endl;
    }
private:
    static Texture honeycombTexture;
    Sprite sprite;
    int points;
    bool active;
    float speed;
};
Texture Honeycomb::honeycombTexture;
// Save score to file
void saveScore(int score) {
    ofstream file("scores.txt",ios::app);
    if (file.is_open()) {
        file<<score <<endl;
        file.close();
    }
}
// Game Over screen
void GameOver(int score, Font& font) {
    RenderWindow window(VideoMode(960, 640),"Game Over");
    Text gameOverText,scoreText,retryText,exitText;
    gameOverText.setFont(font);
    scoreText.setFont(font);
    retryText.setFont(font);
    exitText.setFont(font);
    gameOverText.setString("Game Over!");
    scoreText.setString("Score: " + to_string(score));
    retryText.setString("Retry (R)");
    exitText.setString("Exit (E)");
    gameOverText.setCharacterSize(40);
    scoreText.setCharacterSize(30);
    retryText.setCharacterSize(20);
    exitText.setCharacterSize(20);
    gameOverText.setPosition(400, 200);
    scoreText.setPosition(400, 300);
    retryText.setPosition(400, 400);
    exitText.setPosition(400, 450);
    while (window.isOpen()) {
        Event event;
        while (window.pollEvent(event)){
            if (event.type == Event::Closed)
                window.close();
            if (event.type == Event::KeyPressed){
                if (event.key.code == Keyboard::R){
                    window.close();
                    return;
                }
                if (event.key.code == Keyboard::E){
                    window.close();
                    exit(0);
                }
            }
        }
        window.clear();
        window.draw(gameOverText);
        window.draw(scoreText);
        window.draw(retryText);
        window.draw(exitText);
        window.display();
    }
}
// Spawn bees function
void spawnBees(Enemy enemies[],int& enemycount,int maxEnemies,int beesToSpawn){
    for (int i=0; i< beesToSpawn && enemycount<maxEnemies;i++) {
        enemies[enemycount]=Enemy(rand()%900,50+rand()%151); // Random x: 0-899, y: 50-200
        enemies[enemycount].setSpeed(2.0f); // Speed will be set in GamePlay
        enemycount++;
    }
}
// GamePlay function
void GamePlay(int difficulty,Font& font) {
    const int WIDTH=960;
    const int HEIGHT=640;
    RenderWindow window(VideoMode(WIDTH, HEIGHT),"Buzz Bombers");
    Texture bgTexture;
    if (!bgTexture.loadFromFile("main_menu.png")) {
        cerr << "Failed to load background image\n";
    } else {
        cout << "Background image loaded successfully\n";
    }
    Sprite background(bgTexture);
    background.setScale(
        float(window.getSize().x)/bgTexture.getSize().x,
        float(window.getSize().y)/bgTexture.getSize().y
    );
    // window.draw(background);
    

    // Sprite & Texture declaration
    Sprite backgroundSprite;
    Texture backgroundTexture;
    if(!backgroundTexture.loadFromFile("background.jpeg")){
    cerr<<"Error loading background texture!"<<endl;}
    else {
        cout<<"Error loading background texture!"<<endl;
      
    }
    
    GameConfig config;
    if (difficulty ==1) { // Easy
        config = {1,10,5,4.0f,2.0f,5.0f}; // 10 bees, 5 honeycombs
    } else if (difficulty == 2) { // Medium
        config={1,15,7,4.0f,2.0f,4.0f}; // 15 bees, 7 honeycombs
    } else { // Hard
        config={1,20,10,4.0f,2.0f,3.0f}; // 20 bees, 10 honeycombs
    }

    cout<< "Loading textures..."<< endl;
    Player::loadTextures();
    Enemy::loadTexture();
    Honeycomb::loadTexture();
    cout<<"Textures loaded successfully."<< endl;

    cout <<"Initializing player..." << endl;
    Player player;
    player.setSpeed(config.playerSpeed);

    cout<<"Declaring arrays..."<<endl;
    Enemy enemies[20]; // Adjusted for Hard (20 bees)
    Sprite bullets[1]; // Adjusted for maxBullets = 1
    Honeycomb honeycombs[10]; // Adjusted for Hard (10 honeycombs)
    cout << "Arrays declared successfully." << endl;
    int enemycount = 0;
    int bulletcount = 0;
    int honeycombcount = 0;
    int score = 0;
    int enemiesKilled = 0;
    int totalBulletsFired = 0; // Track total bullets
    Clock clock;
    float beeSpawnTimer = 0.0f;
    bool levelComplete = false;

    cout << "Initializing honeycombs..."<<endl;
    for (int i=0; i<config.maxHoneycombs;i++) {
        honeycombs[i]=Honeycomb(rand() %900,50+ rand()% 251); // Random x: 0-899, y: 50-300
        honeycombcount++;
    }
    cout<<"Honeycombs initialized: "<< honeycombcount <<endl;

    // Initial bee spawn
    spawnBees(enemies, enemycount, config.maxEnemies,2); // Start with 2 bees
    for (int i=0;i<enemycount;i++) {
        enemies[i].setSpeed(config.enemySpeed);
    }
    cout<<"Initial bees spawned: "<<enemycount << endl;

    while (window.isOpen()&& !levelComplete) {
        Event event;
        while (window.pollEvent(event)) {
            if (event.type==Event::Closed)
                window.close();
        }

        player.movePlayer();
        player.shootBullet(bullets, bulletcount, config.maxBullets, totalBulletsFired);

        // Check bullet limit
        if (totalBulletsFired>=100) {
            saveScore(score);
            GameOver(score, font);
            return;
        }

        for (int i= 0;i<bulletcount; i++) {
            bullets[i].move(0, -10);
        }

        int newbulletcount=0;
        for (int i=0;i<bulletcount;i++) {
            if (bullets[i].getPosition().y>=0) {
                bullets[newbulletcount++]=bullets[i];
            }
        }
        bulletcount=newbulletcount;

        beeSpawnTimer+=clock.restart().asSeconds();
        if (beeSpawnTimer>=10.0f &&enemycount<config.maxEnemies) {
            spawnBees(enemies,enemycount,config.maxEnemies, 2); // Spawn 2 bees
            for (int i=enemycount-2; i<enemycount; i++) {
                if (i>=0) {
                    enemies[i].setSpeed(config.enemySpeed);
                }
            }
            beeSpawnTimer=0.0f;
            cout<<"Spawned 2 bees, total: "<<enemycount<< endl;
        }

        for (int i=0;i<enemycount;i++) 
        {
            if (enemies[i].isActive()){
                enemies[i].move();
            }
        }
        for (int i=0;i<honeycombcount;i++){
            if (honeycombs[i].isActive()){
                honeycombs[i].move(); // Stationary
            }
        }

        for (int i=0;i<bulletcount;i++){
            bool bulletHit = false;
            for (int j = 0; j < enemycount; j++) {
                if (enemies[j].isActive() && bullets[i].getGlobalBounds().intersects(enemies[j].getBounds())){
                    enemies[j].setActive(false);
                    score += 100;
                    enemiesKilled++;
                    bulletHit = true;
                    break;
                }
            }
            for (int j=0; !bulletHit&& j<honeycombcount;j++){
                if (honeycombs[j].isActive()&&bullets[i].getGlobalBounds().intersects(honeycombs[j].getBounds())){
                    score += honeycombs[j].getPoints();
                    honeycombs[j].setActive(false);
                    bulletHit = true;
                }
            }
            if (bulletHit) {
                for (int k = i; k < bulletcount - 1; k++){
                    bullets[k] = bullets[k + 1];
                }
                bulletcount--;
                i--;
            }
        }

        for (int i = 0; i < enemycount; i++) {
            if (enemies[i].isActive() && player.getBounds().intersects(enemies[i].getBounds())) {
                saveScore(score);
                GameOver(score, font);
                return;
            }
        }
        for (int i=0; i<honeycombcount;i++) {
            if (honeycombs[i].isActive()&& player.getBounds().intersects(honeycombs[i].getBounds())) {
                score+=honeycombs[i].getPoints();
                honeycombs[i].setActive(false);
            }
        }

        int newenemycount=0;
        for (int i=0;i<enemycount;i++) {
            if (enemies[i].isActive()){
                enemies[newenemycount++] = enemies[i];
            }
        }
        enemycount=newenemycount;

        int newhoneycombcount=0;
        for (int i=0;i<honeycombcount;i++) {
            if (honeycombs[i].isActive()){
                honeycombs[newhoneycombcount++]=honeycombs[i];
            }
        }
        honeycombcount=newhoneycombcount;

        if (enemycount==0 &&enemiesKilled>=config.maxEnemies){
            levelComplete =true;
            saveScore(score);
        }

        window.clear();
        player.draw(window);
        for (int i=0;i<enemycount; i++) {
            enemies[i].draw(window);
        }
        for (int i=0;i <bulletcount; i++) {
            window.draw(bullets[i]);
        }
        for (int i =0; i<honeycombcount; i++) {
            honeycombs[i].draw(window);
        }



        window.clear(Color::Blue);
        player.draw(window);
        for (int i=0;i<enemycount; i++) {
            enemies[i].draw(window);
        }
        for (int i=0;i <bulletcount; i++) {
            window.draw(bullets[i]);
        }
        for (int i =0; i<honeycombcount; i++) {
            honeycombs[i].draw(window);
        }

       
        
        Text scoreText,levelText;
        scoreText.setFont(font);
        levelText.setFont(font);
        scoreText.setString("Score: " +to_string(score));
        string levelStr = (difficulty == 1) ? "Easy" : (difficulty==2) ? "Medium" : "Hard";
        levelText.setString("Level: " + levelStr);
        scoreText.setCharacterSize(24);
        levelText.setCharacterSize(24);
        scoreText.setFillColor(Color::White);
        levelText.setFillColor(Color::White);
        scoreText.setPosition(10, 10);
        levelText.setPosition(10, 40);
        window.draw(scoreText);
        window.draw(levelText);
        window.display();
    }
}

// Level Menu function
void LevelMenu(int& difficulty, Font& font, RenderWindow& window) {
    const int level_items = 3;
    Text levelMenu[level_items];
    string levelOptions[level_items] = {"Easy", "Medium", "Hard"};

    for (int i = 0; i < level_items; ++i) {
        levelMenu[i].setFont(font);
        levelMenu[i].setString(levelOptions[i]);
        levelMenu[i].setCharacterSize(30);
        float textWidth = levelMenu[i].getLocalBounds().width;
        levelMenu[i].setPosition((960 - textWidth) / 2, 200 + i * 60);
        cout<< "Level menu option " <<i<<": " << levelOptions[i] <<" at (" << (960 - textWidth) / 2 << ", "<< 200 + i * 60 << ")\n";
    }
// Sprite & Texture declaration
Sprite backgroundSprite;
Texture backgroundTexture;
if(!backgroundTexture.loadFromFile("/main_menu.png")){
cerr<<"Error loading background texture!"<<endl;

}
else{
    cout<<"loading background texture "<<endl;
}

    int choice=0;
    Clock animationClock;

    while (window.isOpen()){
        Event event;
        while (window.pollEvent(event)){
            if (event.type==Event::Closed)
                window.close();

            if (event.type==Event::KeyPressed){
                if (event.key.code==Keyboard::Up){
                    choice = (choice + level_items - 1) % level_items;
                }
                if (event.key.code==Keyboard::Down){
                    choice = (choice+1) %level_items;
                }
                if (event.key.code==Keyboard::Enter){
                    if (choice==0){
                        difficulty= 1;
                    } else if (choice== 1) 
                    {
                        difficulty= 2;
                    } else if (choice== 2) 
                    {
                        difficulty =3;
                    }
                    cout<<"Level selected: "<< levelOptions[choice] << endl;
                    return;
                }
                if (event.key.code==Keyboard::Escape) {
                    return;
                }
            }
            if (event.type==Event::MouseButtonPressed) {
                if (event.mouseButton.button==Mouse::Right) {
                    choice=(choice + level_items - 1 )%level_items; // Navigate up
                }
                if (event.mouseButton.button == Mouse::Left) {
                    choice=(choice+1)%level_items; // Navigate down
                }
            }
        }

        window.clear();
        backgroundSprite.setTexture(backgroundTexture);
        window.draw(backgroundSprite);
        float time = animationClock.getElapsedTime().asSeconds();
        bool blink = static_cast<int>(time * 2) % 2 == 0;

        for (int i = 0; i < level_items; ++i) {
            if (i == choice) {
                levelMenu[i].setFillColor(blink ? Color::Cyan: Color::White);
            } else {
                levelMenu[i].setFillColor(Color::White);
            }
            
            window.draw(levelMenu[i]);
       
        }
        
        window.display();
    }
}

// Main Menu function
void MainMenu() {
    RenderWindow window(VideoMode(960, 640), "Buzz Bombers Game Menu");
    window.setFramerateLimit(60);
    SoundBuffer bufferSelect, bufferNavigate;
    if (!bufferSelect.loadFromFile("/Sound Effects/fire.wav")) {
        cerr << "Failed to load select sound\n";
    } else {
        cout << "Select sound loaded successfully\n";
    }
    if (!bufferNavigate.loadFromFile("/Sound Effects/hit.wav")) {
        cerr << "Failed to load navigate sound\n";
    } else {
        cout << "Navigate sound loaded successfully\n";
    }
    Sound soundSelect, soundNavigate;
    soundSelect.setBuffer(bufferSelect);
    soundNavigate.setBuffer(bufferNavigate);
    Texture bgTexture;
    if (!bgTexture.loadFromFile("/main_menu.png")) {
        cerr << "Failed to load background image\n";
    } else {
        cout << "Background image loaded successfully\n";
    }
    Sprite background(bgTexture);
    background.setScale(
        float(window.getSize().x)/bgTexture.getSize().x,
        float(window.getSize().y)/bgTexture.getSize().y
    );

    Font font;
    if (!font.loadFromFile("/Press_Start_2P/PressStart2P-Regular.ttf")) {
        cerr << "Failed to load font\n";
    } else {
        cout << "Font loaded successfully\n";
    }

    ScreenState currentScreen=MAIN_MENU;
    const int main_items=4;
    Text mainMenu[main_items];
    string mainOptions[main_items]={"Play Game","Choose Level", "Options", "Exit"};

    for (int i=0;i<main_items;++i) {
        mainMenu[i].setFont(font);
        mainMenu[i].setString(mainOptions[i]);
        mainMenu[i].setCharacterSize(30);
        float textWidth = mainMenu[i].getLocalBounds().width;
        mainMenu[i].setPosition((960 - textWidth)/2, 200+i*60);
        cout<<"Main menu option " <<i<<": "<< mainOptions[i] <<" at (" << (960 - textWidth) / 2 << ", " << 200 + i * 60 << ")\n";
    }
    int choice =0, difficulty=1;
    Clock animationClock;
    while (window.isOpen()) 
    {
        Event event;
        while (window.pollEvent(event)) 
        {
            if (event.type==Event::Closed)
                window.close();

            if (event.type== Event::KeyPressed) 
            {
                if (event.key.code ==Keyboard::Up){
                    if (currentScreen==MAIN_MENU) {
                        choice = (choice+main_items - 1) %main_items;
                        soundNavigate.play();
                    }
                }
                if (event.key.code== Keyboard::Down){
                    if (currentScreen ==MAIN_MENU) {
                        choice = (choice+1)%main_items;
                        soundNavigate.play();
                    }
                }
                if (event.key.code==Keyboard::Enter){
                    soundSelect.play();
                    if (currentScreen==MAIN_MENU) {
                        if (choice == 0) {
                            cout<<"Start Game\n";
                            window.clear();
                            window.display();
                            GamePlay(difficulty, font);
                        } else if (choice==1) {
                            LevelMenu(difficulty, font, window);
                            currentScreen=MAIN_MENU;
                            choice = 0;
                        } else if (choice==2) {
                            cout << "Options selected\n";
                           
                        } else if (choice==3) {
                            window.close();
                        }
                    }
                }
            }
        }

        window.clear();
        window.draw(background);

        float time=animationClock.getElapsedTime().asSeconds();
        bool blink=static_cast<int>(time*2) %2 ==0;

        if (currentScreen==MAIN_MENU) {
            for (int i=0;i<main_items;++i) {
                if (i==choice) {
                    mainMenu[i].setFillColor(blink ?Color::Cyan:Color::White);
                } else {
                    mainMenu[i].setFillColor(Color::White);
                }
                window.draw(mainMenu[i]);
            }
        }

        window.display();
    }
}

int main() {
    srand(time(0));
    MainMenu();
    return 0;
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
