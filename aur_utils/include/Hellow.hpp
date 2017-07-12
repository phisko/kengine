#ifndef KENGINE_EXAMPLE_HELLOW_HPP
# define KENGINE_EXAMPLE_HELLOW_HPP

# include <GameObject.hpp>
# include <common/components/MetaComponent.hpp>
# include <common/components/TransformComponent.hpp>

class Hellow : public kengine::GameObject
{

    public:
        Hellow(std::string_view const& name) : GameObject(name)
    {
        attachComponent<kengine::TransformComponent3d>(putils::Point3d{0, 0, 0},
                putils::Point{311, 311});
        attachComponent<kengine::MetaComponent>("resources/hellow.jpg");
    }
};

#endif //KENGINE_EXAMPLE_HELLOW_HPP
