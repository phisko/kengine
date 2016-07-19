//
// File: [Resources.cpp]
//
// Author: flff.
// Contact: <Samy.kettani@gmail.com> (github.com/Flff)
//
// Created on 2016-07-18 14:12
//

#include <iostream>
#include <exception>
#include "Resources.hpp"

//TODO: Remove hard coded string (Default constructor parameters)
Resources::Resources()
{
  _textures.emplace("MissingTexture",
		    std::make_unique<sf::Texture>());
  if (not _textures["MissingTexture"]->loadFromFile("res/MissingTexture.png"))
    {
      throw std::runtime_error("Can't load default texture");
    }
  _soundBuffers.emplace("Missingsoundbuffer",
			std::make_unique<sf::SoundBuffer>());
  if (not _textures["MissingSoundbuffer"]->loadFromFile("res/MissingSound.wav"))
    {
      throw std::runtime_error("Can't load default sound");
    }
}

Resources::~Resources()
{
}

template<>
const sf::Texture&
Resources::operator[]<sf::Texture>(const std::string& name) const
{
  return getTexture(name);
}

template<>
const sf::SoundBuffer&
Resources::operator[]<sf::SoundBuffer>(const std::string& name) const
{
  return getSoundBuffer(name);
}

const sf::Texture&	Resources::getTexture(const std::string& name) const
{
  try
    {
      return *(_textures.at(name));
    }
  catch (std::out_of_range& e)
    {
      std::cerr << "Can't find texture " << name << std::endl;
      return *(_textures.at("MissingTexture"));
    }
}

const sf::SoundBuffer&
Resources::getSoundBuffer(const std::string& name) const
{
  try
    {
      return *(_soundBuffers.at(name));
    }
  catch (std::out_of_range& e)
    {
      std::cerr << "Can't find sound buffer " << name << std::endl;
      return *(_soundBuffers.at("MissingSoundBuffer"));
    }
}

void
Resources::addTexture(const std::string& name,
		      const std::string& path,
		      bool repeated)
{
  _textures.emplace(name, std::make_unique<sf::Texture>());
  if (not _textures[name]->loadFromFile(path))
    {
      _textures.erase(_textures.find(name));
      std::cerr << "Can't load " << name << '(' << path << ')' << std::endl;
      return ;
    }
  _textures[name]->setRepeated(repeated);
}

void
Resources::addSoundBuffer(const std::string& name,
			  const std::string& path)
{
  _soundBuffers.emplace(name, std::make_unique<sf::SoundBuffer>());
  if (not _soundBuffers[name]->loadFromFile(path))
    {
      _soundBuffers.erase(_soundBuffers.find(name));
      std::cerr << "Can't load " << name << '(' << path << ')' << std::endl;
    }
}
