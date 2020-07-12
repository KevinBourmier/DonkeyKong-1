#include "pch.h"
#include "StringHelpers.h"
#include "Game.h"
#include "EntityManager.h"

using namespace std;

const float Game::PlayerSpeed = 100.f;
const sf::Time Game::TimePerFrame = sf::seconds(1.f / 60.f);

Game::Game()
	: mWindow(sf::VideoMode(840, 600), "Donkey Kong 1981", sf::Style::Close)
	, mTexture()
	, mPlayer()
	, mFont()
	, mStatisticsText()
	, mStatisticsUpdateTime()
	, mStatisticsNumFrames(0)
	, mIsMovingUp(false)
	, mIsMovingDown(false)
	, mIsMovingRight(false)
	, mIsMovingLeft(false)
{
	mWindow.setFramerateLimit(160);

	// Draw blocks

	_TextureBlock.loadFromFile("Media/Textures/Block.png");
	_sizeBlock = _TextureBlock.getSize();

	for (int i = 0; i < BLOCK_COUNT_X; i++)
	{
		for (int j = 0; j < BLOCK_COUNT_Y; j++)
		{
			_Block[i][j].setTexture(_TextureBlock);
			_Block[i][j].setPosition(100.f + 70.f * (i + 1), 0.f + BLOCK_SPACE * (j + 1));

			std::shared_ptr<Entity> se = std::make_shared<Entity>();
			se->m_sprite = _Block[i][j];
			se->m_type = EntityType::block;
			se->m_size = _TextureBlock.getSize();
			se->m_position = _Block[i][j].getPosition();
			EntityManager::m_Entities.push_back(se);
		}
	}

	// Draw Echelles

	_TextureEchelle.loadFromFile("Media/Textures/Echelle.png");

	for (int i = 0; i < ECHELLE_COUNT; i++)
	{
		_Echelle[i].setTexture(_TextureEchelle);
		_Echelle[i].setPosition(100.f + 70.f * (i + 1), 0.f + BLOCK_SPACE * (i + 1) + _sizeBlock.y );

		std::shared_ptr<Entity> se = std::make_shared<Entity>();
		se->m_sprite = _Echelle[i];
		se->m_type = EntityType::echelle;
		se->m_size = _TextureEchelle.getSize();
		se->m_position = _Echelle[i].getPosition();
		EntityManager::m_Entities.push_back(se);

	}

	// Draw Mario

	mTexture.loadFromFile("Media/Textures/Mario_small_transparent.png"); // Mario_small.png");
	_sizeMario = mTexture.getSize();
	mPlayer.setTexture(mTexture);
	sf::Vector2f posMario;
	posMario.x = 100.f + 70.f;
	posMario.y = BLOCK_SPACE * 5 - _sizeMario.y;

	mPlayer.setPosition(posMario);

	std::shared_ptr<Entity> player = std::make_shared<Entity>();
	player->m_sprite = mPlayer;
	player->m_type = EntityType::player;
	player->m_size = mTexture.getSize();
	player->m_position = mPlayer.getPosition();
	EntityManager::m_Entities.push_back(player);

	// Draw Statistic Font 

	mFont.loadFromFile("Media/Sansation.ttf");
	mStatisticsText.setString("Welcome to Donkey Kong 1981");
	mStatisticsText.setFont(mFont);
	mStatisticsText.setPosition(5.f, 5.f);
	mStatisticsText.setCharacterSize(10);

	//Draw Bruno

	mTextureBruno.loadFromFile("Media/Textures/bruno.png");
	_sizeBrunoFernandes = mTextureBruno.getSize();
	mBrunoFernandes.setTexture(mTextureBruno);
	sf::Vector2f posBruno;
	posBruno.x = 626.649;
	posBruno.y = 18.3537;

	mBrunoFernandes.setPosition(posBruno);

	std::shared_ptr<Entity> bruno = std::make_shared<Entity>();
	bruno->m_sprite = mBrunoFernandes;
	bruno->m_type = EntityType::bruno;
	bruno->m_size = mTextureBruno.getSize();
	bruno->m_position = mBrunoFernandes.getPosition();
	EntityManager::m_Entities.push_back(bruno);
}

void Game::run()
{
	sf::Clock clock;
	sf::Time timeSinceLastUpdate = sf::Time::Zero;
	while (mWindow.isOpen())
	{
		sf::Time elapsedTime = clock.restart();
		timeSinceLastUpdate += elapsedTime;
		while (timeSinceLastUpdate > TimePerFrame)
		{
			timeSinceLastUpdate -= TimePerFrame;

			processEvents();
			update(TimePerFrame);
		}

		updateStatistics(elapsedTime);
		render();
	}
}

void Game::processEvents()
{
	sf::Event event;
	while (mWindow.pollEvent(event))
	{
		switch (event.type)
		{
		case sf::Event::KeyPressed:
			handlePlayerInput(event.key.code, true);
			break;

		case sf::Event::KeyReleased:
			handlePlayerInput(event.key.code, false);
			break;

		case sf::Event::Closed:
			mWindow.close();
			break;
		}
	}
}

void Game::update(sf::Time elapsedTime){
	

	sf::Vector2f movement(0.f, 0.f);
	if (mIsMovingUp && moveUp)
		movement.y -= PlayerSpeed;
	if (mIsMovingDown && moveDown)
		movement.y += PlayerSpeed;
	if (mIsMovingLeft && moveLeft)
		movement.x -= PlayerSpeed;
	if (mIsMovingRight && moveRight)
		movement.x += PlayerSpeed;

	for (std::shared_ptr<Entity> entity : EntityManager::m_Entities)
	{
		if (entity->m_enabled == false)
		{
			continue;
		}

		if (entity->m_type != EntityType::player)
		{
			continue;
		}

		entity->m_sprite.move(movement * elapsedTime.asSeconds());
	}
}

void Game::render()
{
	mWindow.clear();

	for (std::shared_ptr<Entity> entity : EntityManager::m_Entities)
	{
		if (entity->m_enabled == false)
		{
			continue;
		}

		mWindow.draw(entity->m_sprite);
	}

	mWindow.draw(mStatisticsText);

	if (Victory()) {
		mVictory.setFillColor(sf::Color::White);
		mVictory.setFont(mFont);
		mVictory.setPosition(150.f, 200.f);
		mVictory.setCharacterSize(50);
		mVictory.setString("Vous avez gagné \nAppuyez sur A pour rejouer");
		mWindow.draw(mVictory);
	}
	mWindow.display();
}

void Game::updateStatistics(sf::Time elapsedTime)
{
	mStatisticsUpdateTime += elapsedTime;
	mStatisticsNumFrames += 1;

	if (mStatisticsUpdateTime >= sf::seconds(1.0f))
	{
		mStatisticsText.setString(
			"Frames / Second = " + toString(mStatisticsNumFrames) + "\n" +
			"Time / Update = " + toString(mStatisticsUpdateTime.asMicroseconds() / mStatisticsNumFrames) + "us");

		mStatisticsUpdateTime -= sf::seconds(1.0f);
		mStatisticsNumFrames = 0;
	}

	MovePlayer();

	if (mStatisticsUpdateTime >= sf::seconds(0.050f))
	{
		// Handle collision weapon enemies
	}

	Victory();
}

void Game::RestartGame(){
	std::shared_ptr<Entity> player = EntityManager::GetPlayer();
	player->m_sprite.setPosition(100.f + 70.f, BLOCK_SPACE * 5 - _sizeMario.y);
}

void Game::MovePlayer() {

	std::shared_ptr<Entity> player = EntityManager::GetPlayer();

	std::cout << player->m_sprite.getPosition().y << std::endl;

	std::cout << player->m_sprite.getPosition().x << std::endl;



	if (player->m_sprite.getPosition().x > 168.33 && player->m_sprite.getPosition().x < 688.314) {
		moveDown = false;
		moveUp = false;
		moveRight = true;
		moveLeft = true;
	
	}
	else if (player->m_sprite.getPosition().x <= 168.33) {
		moveLeft = false;
	}
	else if (player->m_sprite.getPosition().x >= 688.314) {
		moveRight = false;
	}

	// First Stage

	if (player->m_sprite.getPosition().x > 369.991 && player->m_sprite.getPosition().x < 378.334) {
		if (player->m_sprite.getPosition().y < 486.667 && player->m_sprite.getPosition().y > 388.330) {
			moveLeft = false;
			moveRight = false;

		}
		moveUp = true;
	}
	if (player->m_sprite.getPosition().x > 369.991 && player->m_sprite.getPosition().x < 378.334 && 
		player->m_sprite.getPosition().y >= 383.338 && player->m_sprite.getPosition().y <= 384.338) {
		std::cout << "Je ne peux plus monter 1" << std::endl;
		moveUp = false;
	}

	// Second stage 
	if (player->m_sprite.getPosition().x > 296.661 && player->m_sprite.getPosition().x < 306.661) {
		if (player->m_sprite.getPosition().y < 380.005  && player->m_sprite.getPosition().y > 278.443 ) {
			moveLeft = false;
			moveRight = false;

		}
		moveUp = true;
	}

	if (player->m_sprite.getPosition().x > 296.661 && player->m_sprite.getPosition().x < 306.661 &&
		player->m_sprite.getPosition().y >= 273.343  && player->m_sprite.getPosition().y <= 274.343) {
		std::cout << "Je ne peux plus monter 2" << std::endl;
		moveUp = false;
	}

	//Third Stage
	if (player->m_sprite.getPosition().x > 166.681 && player->m_sprite.getPosition().x < 236.664) {
		if (player->m_sprite.getPosition().y < 273.343  && player->m_sprite.getPosition().y > 168.347) {
			moveLeft = false;
			moveRight = false;

		}
		moveUp = true;
	}

	//Third Stage
	if (player->m_sprite.getPosition().x > 170 && player->m_sprite.getPosition().x < 172) {
		if (player->m_sprite.getPosition().y < 168.347  && player->m_sprite.getPosition().y > 56.6851) {
			moveLeft = false;
			moveRight = false;

		}
		moveUp = true;
	}

}

bool Game::Victory() {
	std::shared_ptr<Entity> player = EntityManager::GetPlayer();

	if (player->m_sprite.getPosition().x >= mBrunoFernandes.getPosition().x && player->m_sprite.getPosition().y >= mBrunoFernandes.getPosition().y){
		std::cout << "VICTOIIIIIIIRE" << std::endl;
		moveUp = false;
		moveDown = false;
		moveRight = false;
		moveLeft = false;
		return true;
	}

	return false;

}

void Game::handlePlayerInput(sf::Keyboard::Key key, bool isPressed)
{
	if (key == sf::Keyboard::Up)
		mIsMovingUp = isPressed;
	else if (key == sf::Keyboard::Down)
		mIsMovingDown = isPressed;
	else if (key == sf::Keyboard::Left)
		mIsMovingLeft = isPressed;
	else if (key == sf::Keyboard::Right)
		mIsMovingRight = isPressed;
	else if (Victory() && key == sf::Keyboard::A)
		RestartGame();


	if (key == sf::Keyboard::Space)
	{
	}
}
