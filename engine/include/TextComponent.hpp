//
// Created by antry on 1/21/17.
//

#ifndef KENGINE_TEXTCOMPONENT_HPP
# define KENGINE_TEXTCOMPONENT_HPP

# include <IComponent.hpp>
# include <iostream>

class TextComponent : public IComponent
{
public:
    static ComponentMask Mask;

public:
    TextComponent(const std::string& text);
    TextComponent(const TextComponent& other);
    TextComponent(TextComponent&& other);
    TextComponent& operator=(TextComponent other);
    TextComponent& operator=(TextComponent&& other);
    virtual ~TextComponent()
    { }

public:
    bool  isUnique() const;

public:
    friend void swap(TextComponent& left, TextComponent& right);

public:
    virtual ComponentMask getMask() const override;
    virtual std::string toString() const override;
    virtual std::string const& get_name() const override
    { return _name; }

private:
    std::string _name;
    std::string _text;
    static const bool _unique;
};

#endif //KENGINE_TEXTCOMPONENT_HPP
