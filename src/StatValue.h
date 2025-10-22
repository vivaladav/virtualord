#pragma once

#include <functional>
#include <unordered_map>

namespace game
{

class StatValue
{
public:
    StatValue(unsigned int statId, int val);

    unsigned int GetId() const;

    int GetMin() const;
    int GetMax() const;

    void SetMin(int min);
    void SetMax(int max);

    int GetValue() const;

    void SetValue(int val);

    void SumValue(int val);

    unsigned int AddOnValueChanged(const std::function<void(const StatValue *,
                                                            int oldVal, int newVal)> & f);
    unsigned int AddOnRangeChanged(const std::function<void(const StatValue *)> & f);
    void RemoveOnValueChanged(unsigned int funId);
    void RemoveOnRangeChanged(unsigned int funId);
    void ClearCallbacks();

private:
    void NotifyObserversValue(int oldVal, int newVal);
    void NotifyObserversRange();

private:
    std::unordered_map<unsigned int,
                       std::function<void(const StatValue *, int oldVal, int newVal)>> mCallbacksVal;
    std::unordered_map<unsigned int, std::function<void(const StatValue *)>> mCallbacksRange;

    unsigned int mId = 0;

    int mValue;
    int mMin;
    int mMax;
};

inline unsigned int StatValue::GetId() const { return mId; }

inline int StatValue::GetMin() const { return mMin; }
inline int StatValue::GetMax() const { return mMax; }

inline int StatValue::GetValue() const { return mValue; }

inline void StatValue::ClearCallbacks()
{
    mCallbacksVal.clear();
    mCallbacksRange.clear();
}

} // namespace game
