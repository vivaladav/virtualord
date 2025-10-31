#include "StatValue.h"

#include <limits>

namespace game
{

StatValue::StatValue(unsigned int statId, int val)
    : mId(statId)
    , mValue(val)
    , mMin(std::numeric_limits<int>::min())
    , mMax(std::numeric_limits<int>::max())
{
}

void StatValue::SetMin(int min)
{
    mMin = min;

    if(mValue < mMin)
    {
        const int oldValue = mValue;
        mValue = mMin;

        if(oldValue != mValue)
            NotifyObserversValue(oldValue, mValue);
    }

    NotifyObserversRange();
}

void StatValue::SetMax(int max)
{
    mMax = max;

    if(mValue > mMax)
    {
        const int oldValue = mValue;
        mValue = mMax;

        if(oldValue != mValue)
            NotifyObserversValue(oldValue, mValue);
    }

    NotifyObserversRange();
}

void StatValue::SetValue(int val)
{
    // same value -> exit
    if(val == mValue)
        return ;

    const int oldValue = mValue;

    mValue = val;

    NotifyObserversValue(oldValue, mValue);
}

void StatValue::SumValue(int val)
{
    const int oldValue = mValue;

    mValue += val;

    // clamp data
    if(mValue < mMin)
        mValue = mMin;
    else if(mValue > mMax)
        mValue = mMax;

    if(oldValue != mValue)
        NotifyObserversValue(oldValue, mValue);
}

unsigned int StatValue::AddOnValueChanged(const std::function<void(const StatValue *,
                                                                   int oldVal, int newVal)> & f)
{
    static unsigned int num = 0;

    mCallbacksVal.emplace(++num, f);

    return num;
}

unsigned int StatValue::AddOnRangeChanged(const std::function<void (const StatValue *)> & f)
{
    static unsigned int num = 0;

    mCallbacksRange.emplace(++num, f);

    return num;
}

void StatValue::RemoveOnValueChanged(unsigned int funId)
{
    auto it = mCallbacksVal.find(funId);

    if(it != mCallbacksVal.end())
        mCallbacksVal.erase(it);
}

void StatValue::RemoveOnRangeChanged(unsigned int funId)
{
    auto it = mCallbacksRange.find(funId);

    if(it != mCallbacksRange.end())
        mCallbacksRange.erase(it);
}

void StatValue::NotifyObserversValue(int oldVal, int newVal)
{
    for(auto & it : mCallbacksVal)
        it.second(this, oldVal, newVal);
}

void StatValue::NotifyObserversRange()
{
    for(auto & it : mCallbacksRange)
        it.second(this);
}

} // namespace game
