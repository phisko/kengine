//
// File: [Resources.hpp]
//
// Author: flff.
// Contact: <Samy.kettani@gmail.com> (github.com/Flff)
//
// Created on 2016-07-18 14:04
//

#ifndef RESOURCES_HPP_
# define RESOURCES_HPP_

# include <map>
# include <string>
# include <SFML/Graphics.hpp>
# include <SFML/Audio.hpp>

class Resources
{
public:
  Resources();
  ~Resources();

  Resources(const Resources&) = delete;
  Resources& operator=(const Resources&) = delete;

public:
  template<typename T>
  const T&			operator[](const std::string&) const;

public:
  const sf::Texture&		getTexture(const std::string&) const;
  const sf::SoundBuffer&	getSoundBuffer(const std::string&) const;
  void				addTexture(const std::string&,
					   const std::string&,
					   bool repeated = false);
  void				addSoundBuffer(const std::string&,
					       const std::string&);

private:
  std::map<std::string, std::unique_ptr<sf::Texture> >		_textures;
  std::map<std::string, std::unique_ptr<sf::SoundBuffer> >	_soundBuffers;
};
#endif //!RESOURCES_HPP_
