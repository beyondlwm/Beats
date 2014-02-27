#ifndef BEATS_INITIALIZER_H__INCLUDE
#define BEATS_INITIALIZER_H__INCLUDE

class CInitializer
{
    BEATS_DECLARE_SINGLETON(CInitializer);

public:
    void Init(const HWND& hwnd, const HINSTANCE& hInstance);
    void Uninit();
};

#endif