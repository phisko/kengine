//
// Created by antry on 1/21/17.
//

#include "TextComponent.hpp"
#include <sstream>

ComponentMask TextComponent::Mask = ComponentMask::Ui;
const bool TextComponent::_unique = false;

TextComponent::TextComponent(const std::string &text) :
        _text(text)
{}

TextComponent::TextComponent(const TextComponent& other)
{
    if (this != &other)
        _text = other._text;
}

TextComponent::TextComponent(TextComponent &&other)
{
    swap(*this, other);
}

TextComponent& TextComponent::operator=(TextComponent other)
{
    swap(*this, other);
    return *this;
}

TextComponent& TextComponent::operator=(TextComponent&& other)
{
    swap(*this, other);
    return *this;
}

bool TextComponent::isUnique() const
{
    return _unique;
}

void swap(TextComponent& left, TextComponent& right)
{
    using std::swap;
    swap(left._name, right._name);
}

ComponentMask TextComponent::getMask() const
{
    return Mask;
}

std::string TextComponent::toString() const
{
    std::stringstream ss;
    ss << _text << std::endl;
    return ss.str();
}