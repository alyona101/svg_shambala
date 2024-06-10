#include "svg.h"

namespace svg {

using namespace std::literals;

void Object::Render(const RenderContext& context) const {
    
    context.RenderIndent();

    // Делегируем вывод тега своим подклассам
    RenderObject(context);

    context.out << std::endl;
}

// ---------- Circle ------------------

Circle& Circle::SetCenter(Point center)  {
    center_ = center;
    return *this;
}

Circle& Circle::SetRadius(double radius)  {
    radius_ = radius;
    return *this;
}

void Circle::RenderObject(const RenderContext& context) const {
    auto& out = context.out;
    out << "<circle cx=\""sv << center_.x << "\" cy=\""sv << center_.y << "\" "sv;
    out << "r=\""sv << radius_ << "\" "sv;
    out << "/>"sv;
}

// ---------- Text ------------------
Text& Text::SetPosition(Point pos)
{
    pos_ = pos;
    return *this;
}

// Задаёт смещение относительно опорной точки (атрибуты dx, dy)
Text& Text::SetOffset(Point offset)
{
    offset_ = offset;
    return *this;
}

// Задаёт размеры шрифта (атрибут font-size)
Text& Text::SetFontSize(uint32_t size)
{
    font_size_ = size;
    return *this;
}

// Задаёт название шрифта (атрибут font-family)
Text& Text::SetFontFamily(std::string font_family)
{
    font_family_ = font_family;
    return *this;
}

// Задаёт толщину шрифта (атрибут font-weight)
Text& Text::SetFontWeight(std::string font_weight)
{
    font_weight_ = font_weight;
    return *this;
}

Text& Text::SetFontFill(std::string font_Fill)
{
    font_fill_ = font_fill;
    return *this;
}

// Задаёт текстовое содержимое объекта (отображается внутри тега text)
Text& Text::SetData(std::string data)
{
    data_ = data;
    return *this;
}

void Text::RenderObject(const RenderContext& context) const
{
    auto& out = context.out;
    out << "<text x=\""sv << pos_.x << "\" "sv;
    out << "y=\""sv << pos_.y << "\" "sv;
    out << "font-size=\""sv << font_size_ << "\" "sv;
    out << "font-weight=\""sv << font_weight_ << "\" "sv;
    out << "font-family=\""sv << font_family_ << "\" "sv;
    out << "fill=\""sv << font_fill_ << "\" "sv;
    out << ">\n"sv;
    out << "  "sv << data_ << std::endl;
    out << "  </text>"sv;
}
}