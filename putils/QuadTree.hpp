#pragma once

#include <stdexcept>
#include <iostream>
#include <algorithm>
#include <vector>
#include <cstddef>
#include "Point.hpp"
#include "fwd.hpp"

namespace putils
{
    template<typename Contained, typename Precision = int, std::size_t MaxChildren = 4>
    class QuadTree
    {
    public:
        struct Obj
        {
            Point<Precision> pos;
            Point<Precision> size;
            Contained obj;
        };

    public:
        QuadTree(const Point<Precision> &topLeft, const Point<Precision> &size, QuadTree *parent = nullptr)
                : _isLeaf(true), _parent(parent),
                  _items(), _children(),
                  _topLeft(topLeft), _size(size),
                  _middle({_topLeft.x + _size.x / 2,
                           _topLeft.y + _size.y / 2})
        {}

    public:
        const Point<Precision> &getSize() const noexcept { return _size; }
        const Point<Precision> &getPos() const noexcept { return _topLeft; }
        const std::vector<Obj> &getItems() const noexcept { return _items; }
        const std::vector<QuadTree> &getChildren() const noexcept { return _children; }

        std::vector<const Obj *> getAllItems(const std::function<bool(const Obj &)> condition = nullptr) const noexcept
        {
            std::vector<const Obj*> ret;
            std::vector<const QuadTree *> toProcess;
            toProcess.push_back(this);

            while (toProcess.size())
            {
                const auto quad = toProcess.back();
                toProcess.pop_back();

                const auto &items = quad->getItems();
                for (const auto &e : items)
                    if (condition == nullptr || condition(e))
                        ret.push_back(&e);

                for (const auto &c : quad->getChildren())
                    toProcess.push_back(&c);
            }

            return ret;
        }

    public:
        void add(auto &&obj, const Point<Precision> &pos, const Point<Precision> &size = {0, 0}) noexcept
        {
            if (_parent == nullptr &&
                (pos.x < _topLeft.x || pos.y < _topLeft.y ||
                 pos.x + size.x > _topLeft.x + _size.x || pos.y + size.y > _topLeft.y + _size.y))
            {
                enlarge(pos);
                add(FWD(obj), pos, size);
                return;
            }

            if (pos.x < _topLeft.x && pos.x < _topLeft.y &&
                pos.x + size.x > _topLeft.x + _size.x && pos.y + size.y > _topLeft.y + _size.y)
                return;

            if (_isLeaf)
            {
                _items.push_back({pos, size, FWD(obj)});
                if (_items.size() > MaxChildren && _size.x > 1 && _size.y > 1)
                    divideIntoChildren();
                return;
            }

            if (pos.y < _middle.y)
            {
                if (pos.x < _middle.x)
                    _children[TopLeft].add(FWD(obj), pos, size);
                if (pos.x + size.x > _middle.x) // >= ?
                    _children[TopRight].add(FWD(obj), pos, size);
            }
            if (pos.y + size.y >= _middle.y)
            {
                if (pos.x < _middle.x)
                    _children[BottomLeft].add(FWD(obj), pos, size);
                if (pos.x + size.x > _middle.x) // >= ?
                    _children[BottomRight].add(FWD(obj), pos, size);
            }
        }

    public:
        bool contains(const Contained &obj) const noexcept { return getContainer(obj) != nullptr; }

    public:
        const Point<Precision> &find(const Contained &obj) const
        {
            if (_isLeaf)
            {
                auto it = std::find_if(_items.begin(), _items.end(),
                        [&obj](const auto &item){ return item.obj == obj; }
                );
                if (it == _items.end())
                    throw std::out_of_range("Item not found");

                return it->pos;
            }

            for (const auto &c : _children)
                if (c.contains(obj))
                    return c.find(obj);

            throw std::out_of_range("Item not found");
        }

    public:
        void move(const Contained &obj, const Point<Precision> &pos) noexcept
        {
            try
            {
                auto &toMove = getObj(obj);
                toMove.pos = pos;
                if (fits(toMove.pos, toMove.size))
                    return;
                auto copy = toMove;
                remove(obj);
                add(std::move(copy));
                rebalance();
            }
            catch (const std::out_of_range &)
            {
                std::cerr << "Attempt to move unknown object" << std::endl;
            }
        }

    public:
        void remove(const Contained &obj) noexcept
        {
            if (_isLeaf)
            {
                const auto it = std::find_if(_items.begin(), _items.end(),
                        [&obj](const auto &item){ return item.obj == obj; }
                );
                if (it != _items.end())
                    _items.erase(it);

                return;
            }

            for (auto &c : _children)
                c.remove(obj);

            rebalance();
        }

    public:
        const std::vector<Obj> &getObjects(const Point<Precision> &pos) const noexcept
        {
            if (_isLeaf)
                return _items;

            if (pos.y < _middle.y)
            {
                if (pos.x < _middle.x)
                    return _children[TopLeft].getObjects(pos);
                else
                    return _children[TopRight].getObjects(pos);
            }
            else if (pos.x < _middle.x)
                return _children[BottomLeft].getObjects(pos);
            else
                return _children[BottomRight].getObjects(pos);
        }

    public:
        QuadTree *getContainer(const Contained &obj) noexcept
        {
            if (_isLeaf)
            {
                auto it = std::find_if(_items.begin(), _items.end(),
                        [&obj](const auto &item){ return item.obj == obj; }
                );
                if (it != _items.end())
                    return this;
                return nullptr;
            }

            for (auto &c : _children)
            {
                auto ret = c.getContainer(obj);
                if (ret != nullptr)
                    return ret;
            }

            return nullptr;
        }

    public:
        const QuadTree *getContainer(const Contained &obj) const noexcept
        {
            if (_isLeaf)
            {
                auto it = std::find_if(_items.begin(), _items.end(),
                        [&obj](const auto &item){ return item.obj == obj; }
                );
                if (it != _items.end())
                    return this;
                return nullptr;
            }

            for (auto &c : _children)
            {
                auto ret = c.getContainer(obj);
                if (ret != nullptr)
                    return ret;
            }

            return nullptr;
        }

    public:
        const QuadTree *getParent() const noexcept { return _parent; }
        QuadTree *getParent() noexcept { return _parent; }

    private:
        enum Position { TopLeft, TopRight, BottomLeft, BottomRight };

        Position getMyPosition(const Point<Precision> &pos) noexcept
        {
            if (pos.y < _topLeft.y)
            {
                if (pos.x < _topLeft.x)
                    return BottomRight;
                else
                    return BottomLeft;
            }
            else if (pos.x < _topLeft.x)
                return TopRight;
            else
                return TopLeft;
        }

        void enlarge(const Point<Precision> &mustContain) noexcept
        {
            const Point<Precision> topLeft
                    {
                            mustContain.x < _topLeft.x ? _topLeft.x - _size.x : _topLeft.x,
                            mustContain.y < _topLeft.y ? _topLeft.y - _size.y : _topLeft.y
                    };
            const Point<Precision> middle { topLeft.x + _size.x, topLeft.y + _size.y };

            std::vector<QuadTree> children;

            children.push_back(QuadTree(topLeft, _size, this)); // Top left
            children.push_back(QuadTree({ middle.x, topLeft.y }, _size, this)); // Top right
            children.push_back(QuadTree({ topLeft.x, middle.y }, _size, this)); // Bottom left
            children.push_back(QuadTree(middle, _size, this)); // Bottom right

            const Position position = getMyPosition(mustContain);

            if (_children.size())
                children[position]._isLeaf = false;
            children[position]._children = std::move(_children);
            children[position]._items = std::move(_items);

            _children = std::move(children);
            _topLeft = topLeft;
            _middle.x = _topLeft.x + _size.x;
            _middle.y = _topLeft.y + _size.y;
            _size.x *= 2; _size.y *= 2;
            _isLeaf = false;
        }

        void rebalance() noexcept
        {
            if (_isLeaf)
                return;

            for (auto &c : _children)
                c.rebalance();

            if (!_children[0]._isLeaf)
                return;

            size_t objects = 0;
            for (const auto &c : _children)
                objects += c._items.size();
            if (objects <= MaxChildren)
                mergeChildren();
        }

        void mergeChildren() noexcept
        {
            for (auto &c : _children)
            {
                for (auto &obj : c._items)
                    _items.push_back(std::move(obj));
                c._items.clear();
            }
            _children.clear();
            _isLeaf = true;
        }

    private:
        Obj &getObj(const Contained &obj)
        {
            auto container = getContainer(obj);
            if (container == nullptr)
                throw std::out_of_range("Attempt to get unknown object");
            auto &items = container->_items;
            for (auto &item : items)
                if (item.obj == obj)
                    return item;
            throw std::out_of_range("Attempt to get unknown object");
        }

    private:
        void divideIntoChildren() noexcept
        {
            const Point<Precision> childSize = {_size.x / 2, _size.y / 2};

            _children.push_back(QuadTree(_topLeft, childSize, this)); // Top left
            _children.push_back(QuadTree({ _middle.x, _topLeft.y }, childSize, this)); // Top right
            _children.push_back(QuadTree({ _topLeft.x, _middle.y }, childSize, this)); // Bottom left
            _children.push_back(QuadTree(_middle, childSize, this)); // Bottom right

            for (auto &obj : _items)
                addToChildren(std::move(obj));

            _items.clear();
            _isLeaf = false;
        }

    private:
        void addToChildren(Obj &&obj) noexcept
        {
            for (int i = 0; i < 4; ++i)
                _children[i].tryAdd(std::move(obj));
        }

    private:
        void tryAdd(Obj &&obj) noexcept
        {
            if (fits(obj.pos, obj.size))
                add(std::move(obj));
        }

    private:
        bool fits(const Point<Precision> &pos, const Point<Precision> &size) noexcept
        {
            return pos.x < _topLeft.x + _size.x &&
                   pos.x + size.x >= _topLeft.x &&
                   pos.y < _topLeft.y + _size.y &&
                   pos.y + size.y >= _topLeft.y;
        }

    private:
        void add(Obj &&obj) noexcept
        {
            add(std::move(obj.obj), obj.pos, obj.size);
        }

    private:
        bool _isLeaf;
        QuadTree *_parent;
        std::vector<Obj> _items;
        std::vector<QuadTree> _children;
        Point<Precision> _topLeft;
        Point<Precision> _size;
        Point<Precision> _middle;
    };

#include <assert.h>
    namespace test
    {
        static bool find(char c, const std::vector<putils::QuadTree<char>::Obj> &objects)
        {
            return std::find_if(objects.begin(), objects.end(),
                    [c](const auto &obj){ return obj.obj == c; }) != objects.end();
        }

        inline void quadTree()
        {
            putils::QuadTree<char> tree({0, 0}, {2, 2});
            tree.add('a', {-1, 0});
            tree.add('b', {1, 0});
            tree.add('c', {0, 1});
            tree.add('d', {1, 1});
            tree.add('e', {0, 0}, {3, 3});

            const auto &topLeft = tree.getObjects({-1, 0});
            assert(find('a', topLeft));

            const auto &topRight = tree.getObjects({1, 0});
            assert(find('b', topRight) && find('e', topRight));

            const auto &bottomLeft = tree.getObjects({0, 1});
            assert(find('c', bottomLeft) && find('e', bottomLeft));

            const auto &bottomRight = tree.getObjects({1, 1});
            assert(find('d', bottomRight) && find('e', bottomRight));

            assert(tree.contains('d'));
            assert(tree.contains('e'));

            const auto &cPos = tree.find('c');
            assert(cPos.x == 0 && cPos.y == 1);

            try
            {
                tree.find('d');
            }
            catch (std::out_of_range &)
            {
                assert(false);
            }

            std::cout << "All good!" << std::endl;
        }
    }
}