#pragma once

#include <gamebase/GameBaseAPI.h>
#include <gamebase/engine/IAnimation.h>
#include <gamebase/serial/ISerializable.h>

namespace gamebase {

class GAMEBASE_API RepeatingAnimation : public IAnimation, public ISerializable {
public:
    static const int Infinity = -1;

    RepeatingAnimation(int repeatTimes, const std::shared_ptr<IAnimation>& animation)
        : m_repeatTimes(repeatTimes)
        , m_animation(animation)
    {}

    virtual void load(const PropertiesRegister& props) override
    {
        m_animation->load(props);
    }

    virtual void start() override
    {
        m_cur = 0;
        m_needRestart = true;
    }

    virtual void step() override
    {
        while (!isFinished()) {
            if (m_needRestart) {
                m_animation->start();
                m_needRestart = false;
            }
            m_animation->step();
            if (!m_animation->isFinished())
                break;
            ++m_cur;
            m_needRestart = true;
        }
    }

    virtual bool isFinished() const override
    {
        return m_repeatTimes != Infinity && m_cur >= m_repeatTimes;
    }

    virtual void serialize(Serializer& serializer) const override;

private:
    int m_repeatTimes;
    std::shared_ptr<IAnimation> m_animation;
    int m_cur;
    bool m_needRestart;
};

}
