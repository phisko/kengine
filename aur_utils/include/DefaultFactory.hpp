//
// Created by naliwe on 7/6/17.
//

#ifndef KENGINE_EXAMPLE_DEFAULTFACTORY_HPP
# define KENGINE_EXAMPLE_DEFAULTFACTORY_HPP

# include "EntityFactory.hpp"
# include "Hellow.hpp"

class DefaultFactory : public kengine::EntityFactory
{
    public:
        DefaultFactory()
            : _creators{
                {"Default",    [](auto name)
                    {
                        return std::make_unique<kengine::GameObject>(name);
                    }},
                    {"Hellow",     [](auto name)
                        {
                            return std::make_unique<Hellow>(name);
                        }}
            }
        {}

        virtual std::unique_ptr<kengine::GameObject>
            make(std::string_view type, std::string_view name) override
            {
                try
                {
                    return _creators.at(type.data())(name);
                }
                catch (std::exception const& exception)
                {
                    std::cerr << exception.what() << std::endl;
                }
            }

    private:
        std::unordered_map<std::string, std::function<
            std::unique_ptr<kengine::GameObject>(std::string_view)>>
            _creators;
};
#endif //KENGINE_EXAMPLE_DEFAULTFACTORY_HPP
