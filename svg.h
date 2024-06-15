#pragma once
#define _USE_MATH_DEFINES
#include <cstdint>
#include <iostream>
#include <memory>
#include <string>
#include <vector>
#include <cmath>
using namespace std::literals;

namespace svg {

struct Point {
    Point() = default;
    Point(double x, double y)
        : x(x)
        , y(y) {
    }
    double x = 0;
    double y = 0;
};
/*
 * Вспомогательная структура, хранящая контекст для вывода SVG-документа с отступами.
 * Хранит ссылку на поток вывода, текущее значение и шаг отступа при выводе элемента
 */
struct RenderContext {
    RenderContext(std::ostream& out)
        : out(out) {
    }

    RenderContext(std::ostream& out, int indent_step, int indent = 0)
        : out(out)
        , indent_step(indent_step)
        , indent(indent) {
    }

    RenderContext Indented() const {
        return {out, indent_step, indent + indent_step};
    }

    void RenderIndent() const {
        for (int i = 0; i < indent; ++i) {
            out.put(' ');
        }
    }

    std::ostream& out;
    int indent_step = 0;
    int indent = 0;
};

/*
 * Абстрактный базовый класс Object служит для унифицированного хранения
 * конкретных тегов SVG-документа
 * Реализует паттерн "Шаблонный метод" для вывода содержимого тега
 */
class Object {
public:
    void Render(const RenderContext& context) const;

    virtual ~Object() = default;

private:
    virtual void RenderObject(const RenderContext& context) const = 0;
};




/*
 * Класс Circle моделирует элемент <circle> для отображения круга
 * https://developer.mozilla.org/en-US/docs/Web/SVG/Element/circle
 */
class Circle final : public Object {

public:
    Circle& SetCenter(Point center); //возвращаем ссылку чтобы можно было реализовать Fluent interface
    Circle& SetRadius(double radius);

private:
    void RenderObject(const RenderContext& context) const override
    {
        auto& out = context.out;
        out << "<circle cx=\""sv << center_.x << "\" cy=\""sv << center_.y << "\" "sv;
        out << "r=\""sv << radius_ << "\" "sv;
        out << "/>"sv;
    }

    Point center_ = { 0.0, 0.0 };
    double radius_ = 1.0;
};




/*
 * Класс Polyline моделирует элемент <polyline> для отображения ломаных линий
 * https://developer.mozilla.org/en-US/docs/Web/SVG/Element/polyline
 */
class Polyline : public Object {
public:
    // Добавляет очередную вершину к ломаной линии
    Polyline& AddPoint(Point point)
    {
        all_point_.push_back(point);
        return *this;
    }


   
    /*
     * Прочие методы и данные, необходимые для реализации элемента <polyline>
     */

private:
    void RenderObject(const RenderContext& context) const override
    {
        auto& out = context.out;
        out << "polyline points=\""sv;

        for (const Point& point: all_point_)
        {
            out << point.x << "," << point.y << " ";
        }
        out << "/>"sv;
    }
    std::vector <Point> all_point_;
};

/*
 * Класс Text моделирует элемент <text> для отображения текста
 * https://developer.mozilla.org/en-US/docs/Web/SVG/Element/text
 */
class Text : public Object {
public:
    // Задаёт координаты опорной точки (атрибуты x и y)
    Text& SetPosition(Point pos);

    // Задаёт смещение относительно опорной точки (атрибуты dx, dy)
    Text& SetOffset(Point offset);

    // Задаёт размеры шрифта (атрибут font-size)
    Text& SetFontSize(uint32_t size);

    // Задаёт название шрифта (атрибут font-family)
    Text& SetFontFamily(std::string font_family);

    // Задаёт толщину шрифта (атрибут font-weight)
    Text& SetFontWeight(std::string font_weight);

    // Задаёт текстовое содержимое объекта (отображается внутри тега text)
    Text& SetData(std::string data);

    // Прочие данные и методы, необходимые для реализации элемента <text>
private:
    void RenderObject(const RenderContext& context) const override
    {
        auto& out = context.out;
        out << "<text x=\""sv << pos_.x << "\"y=\""sv << pos_.y << "\"dx=\""sv << offset_.x << "\"dy=\""sv << offset_.y << "\" "sv;
        out << "\"font-size=\""sv << size_ << "\"font-family=\""sv << font_family_ << "\" "sv;
        out << "\"font-weight=\""sv << font_weight_ << "\"font-weight=\""sv << font_weight_ << " >" << data_ << " </text>"sv ;
    }
        Point pos_ = { 0.0, 0.0 };
        Point offset_ = { 0.0, 0.0 };
        uint32_t size_ = 1;
        std::string font_family_;
        std::string font_weight_;
        std::string data_;
    
};

class Document {
public:
    /*
     Метод Add добавляет в svg-документ любой объект-наследник svg::Object.
     Пример использования:
     Document doc;
     doc.Add(Circle().SetCenter({20, 30}).SetRadius(15));
    */
    template <typename Obj>
    void Add(Obj obj) //обходим спользование абстрактного класса
    {
        
        objects_.emplace_back(std::make_unique<Obj>(std::move(obj)));
    }

    // Добавляет в svg-документ объект-наследник svg::Object
    void AddPtr(std::unique_ptr<Object>&& obj); //передача по ссылке имеющая значение для компилятора

    // Выводит в ostream svg-представление документа
    void Render(std::ostream& out) const
    {
        RenderContext ctx(std::cout, 2, 2);
        out << "<?xml version=\"1.0\" encoding=\"UTF-8\" ?>"sv << std::endl;
        out << "<svg xmlns=\"http://www.w3.org/2000/svg\" version=\"1.1\" >" << std::endl;;
        for (const auto& obj : objects_)
        {
            obj->Render(ctx);
        }
        out << "</svg>"sv;
    }
    
    // Прочие методы и данные, необходимые для реализации класса Document
private:
    std::vector <std::unique_ptr<Object>> objects_;
};

}  // namespace svg