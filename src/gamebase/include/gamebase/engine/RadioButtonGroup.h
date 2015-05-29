#pragma once

#include <gamebase/GameBaseAPI.h>
#include <functional>
#include <map>

namespace gamebase {

class RadioButton;

class GAMEBASE_API RadioButtonGroup {
public:
    RadioButtonGroup()
        : m_selected(-1)
        , m_nextID(0)
    {}

    void setCallback(const std::function<void(int)>& callback) { m_callback = callback; }

    int selected() const;

    void setSelected(int index);

    int add(RadioButton* button);
    void insert(int id, RadioButton* button);
    void remove(int id);

private:
    int m_selected;
    int m_nextID;
    std::map<int, RadioButton*> m_elements;
    std::function<void(size_t)> m_callback;
};

}
