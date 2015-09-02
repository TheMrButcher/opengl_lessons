#include <stdafx.h>
#include <gamebase/engine/TextBox.h>
#include <gamebase/text/Conversion.h>
#include <gamebase/engine/AutoLengthTextFilter.h>
#include <gamebase/serial/ISerializer.h>
#include <gamebase/serial/IDeserializer.h>
#include <locale>

namespace gamebase {

namespace {
bool isCharStartLess(const CharPosition& charPos, float x)
{
    return charPos.position.bottomLeft.x < x;
}
}

TextBox::TextBox(
    const std::shared_ptr<IRelativeOffset>& position,
    const std::shared_ptr<TextBoxSkin>& skin,
    const std::shared_ptr<ITextFilter>& textFilter)
    : OffsettedPosition(position)
    , FindableGeometry(this, skin->geometry())
    , Drawable(this)
    , m_skin(skin)
    , m_textFilter(textFilter
        ? textFilter : std::make_shared<AutoLengthTextFilter>(skin.get()))
    , m_selectionStart(0)
    , m_selectionEnd(0)
    , m_inited(false)
{}

void TextBox::setText(const std::string& text)
{
    m_selectionStart = 0;
    m_selectionEnd = 0;
    m_skin->setSelection(m_selectionStart, m_selectionEnd);
    try {
        m_skin->setText(text);
    } catch (std::exception& ex) {
        std::cout << "Error while trying to set text \"" << text << "\" to TextEdit"
            ". Reason: " << ex.what() << std::endl;
        return;
    }
    m_text.set(text);
    if (m_inited)
        m_skin->loadResources();
}

void TextBox::setSelectionState(SelectionState::Enum state)
{
    if (state == SelectionState::Pressed)
        state = SelectionState::Selected;
    if (m_selectionState == SelectionState::Selected && state == SelectionState::MouseOn)
        return;
    if (m_selectionState == SelectionState::Selected && state != m_selectionState) {
        m_skin->setSelection(0, 0);
        if (state == SelectionState::None && m_callback)
            m_callback(text());
    }
    m_selectionState = state;
    m_skin->setSelectionState(state);
}

void TextBox::processInput(const InputRegister& input)
{
    const auto& keys = input.keys.signals();
    for (auto it = keys.begin(); it != keys.end(); ++it)
        processKey(*it);

    const auto& specialKeys = input.specialKeys.signals();
    for (auto it = specialKeys.begin(); it != specialKeys.end(); ++it)
        processSpecialKey(*it);

    processMouse(input);
}

void TextBox::registerObject(PropertiesRegisterBuilder* builder)
{
    registerSelectionState(builder);
    builder->registerObject("skin", m_skin.get());
    builder->registerProperty("text", &m_text.toString(),
        std::bind(&TextBox::setText, this, std::placeholders::_1));
}

void TextBox::serialize(Serializer& s) const
{
    s << "position" << m_offset << "skin" << m_skin << "text" << m_text.toString();
    if (dynamic_cast<const ISerializable*>(m_textFilter.get()))
        s << "filter" << m_textFilter;
    else
        s << "filter" << std::shared_ptr<ITextFilter>();
}

std::unique_ptr<IObject> deserializeTextBox(Deserializer& deserializer)
{
    DESERIALIZE(std::shared_ptr<IRelativeOffset>, position);
    DESERIALIZE(std::shared_ptr<TextBoxSkin>, skin);
    DESERIALIZE(std::shared_ptr<ITextFilter>, filter);
    DESERIALIZE(std::string, text);
    std::unique_ptr<TextBox> result(new TextBox(position, skin, filter));
    result->setText(text);
    return std::move(result);
}

REGISTER_CLASS(TextBox);

void TextBox::processKey(char key)
{
    static std::locale loc("");
    bool anyChange = false;
    auto selectionLeft = std::min(m_selectionStart, m_selectionEnd);
    auto selectionRight = std::max(m_selectionStart, m_selectionEnd);

    if (std::isprint(key, loc)) {
        auto newText = m_text;
        newText.erase(selectionLeft, selectionRight);
        auto utf8Code = convertToUtf8(std::string(1, key));
        newText.insert(selectionLeft, utf8Code);
        m_text = m_textFilter->filter(m_text, newText);

        setCursor(selectionLeft + 1);
        anyChange = true;
    }

    if (key == 8 || key == 127) {
        if (selectionLeft == selectionRight) {
            if (key == 8) {
                if (selectionRight == 0)
                    return;
                else
                    selectionLeft = selectionRight - 1;
            }
            if (key == 127) {
                if (selectionLeft == m_text.size())
                    return;
                else
                    selectionRight = selectionLeft + 1;
            }
        }
        
        auto newText = m_text;
        newText.erase(selectionLeft, selectionRight);
        m_text = m_textFilter->filter(m_text, newText);

        setCursor(selectionLeft);
        anyChange = true;
    }

    if (key == 10 || key == 13) {
        setSelectionState(SelectionState::None);
        return;
    }

    if (anyChange) {
        m_skin->setText(m_text.toString());
        m_skin->setSelection(m_selectionStart, m_selectionEnd);
        m_skin->loadResources();
    }
}
    
void TextBox::processSpecialKey(SpecialKey::Enum key)
{
    switch (key) {
        case SpecialKey::Left: moveCursor(-1); break;
        case SpecialKey::Right: moveCursor(+1); break;
        case SpecialKey::Home: setCursor(0); break;
        case SpecialKey::End: setCursor(m_text.size()); break;
        default: return;
    }

    m_skin->setSelection(m_selectionStart, m_selectionEnd);
    m_skin->loadResources();
}

void TextBox::processMouse(const InputRegister& input)
{
    if (input.mouseButtons.isJustPressed(MouseButton::Left)
        || (input.mouseButtons.isPressed(MouseButton::Left) && input.changedPosition())) {
        float x = (fullTransform().inversed() * input.mousePosition()).x;
        if (input.mouseButtons.isJustPressed(MouseButton::Left)) {
            setCursor(calcCharIndex(x));
        } else {
            size_t newEnd = calcCharIndex(x);
            if (newEnd == m_selectionEnd)
                return;
            m_selectionEnd = newEnd;
        }

        m_skin->setSelection(m_selectionStart, m_selectionEnd);
        m_skin->loadResources();
    }
}

void TextBox::setCursor(size_t pos)
{
    pos = std::min(pos, m_text.size());
    m_selectionStart = pos;
    m_selectionEnd = pos;
}

void TextBox::moveCursor(int shift)
{
    size_t pos = shift < 0
        ? std::min(m_selectionStart, m_selectionEnd)
        : std::max(m_selectionStart, m_selectionEnd);
    if (shift < 0 && pos < static_cast<size_t>(-shift))
        pos = 0;
    else
        pos += shift;

    m_selectionStart = std::min(pos, m_text.size());
    m_selectionEnd = m_selectionStart;
}

size_t TextBox::calcCharIndex(float x)
{
    const auto& textGeom = m_skin->textGeometry();
    if (x <= textGeom.front().position.bottomLeft.x)
        return 0;
    if (x >= textGeom.back().position.bottomLeft.x)
        return m_text.size();
    auto it = std::lower_bound(textGeom.begin(), textGeom.end(),
        x, isCharStartLess);
    size_t index = static_cast<size_t>(it - textGeom.begin());
    if (index == 0)
        return 0;
    else
        return index - 1;
}

}