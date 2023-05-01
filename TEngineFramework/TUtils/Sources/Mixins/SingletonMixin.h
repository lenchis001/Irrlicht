#pragma once
template <class T>
class SingletonMixin {
public:
    static T& getInstance();
    SingletonMixin();
};

template <class T>
inline SingletonMixin<T>::SingletonMixin()
{
}

template <class T>
inline T& SingletonMixin<T>::getInstance()
{
    static T globalInstance;
    return globalInstance;
}
