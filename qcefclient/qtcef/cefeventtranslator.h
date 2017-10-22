#pragma once

inline unsigned int getKeyModifiers(unsigned int& ret, Qt::KeyboardModifiers flag)
{
    if (flag & Qt::ShiftModifier)
    {
        ret |= EVENTFLAG_SHIFT_DOWN;
    }
    if (flag & Qt::ControlModifier)
    {
        ret |= EVENTFLAG_CONTROL_DOWN;
    }
    if (flag & Qt::AltModifier)
    {
        ret |= EVENTFLAG_ALT_DOWN;
    }
    if (flag & Qt::AltModifier)
    {
        ret |= EVENTFLAG_ALT_DOWN;
    }

    if (::GetKeyState(VK_NUMLOCK) & 1)
        ret |= EVENTFLAG_NUM_LOCK_ON;
    if (::GetKeyState(VK_CAPITAL) & 1)
        ret |= EVENTFLAG_CAPS_LOCK_ON;

    return ret;
}

inline unsigned int getMouseModifiers(unsigned int& ret, Qt::MouseButtons flag)
{
    if (flag & Qt::LeftButton)
    {
        ret |= EVENTFLAG_LEFT_MOUSE_BUTTON;
    }
    if (flag & Qt::RightButton)
    {
        ret |= EVENTFLAG_RIGHT_MOUSE_BUTTON;
    }
    if (flag & Qt::MiddleButton)
    {
        ret |= EVENTFLAG_MIDDLE_MOUSE_BUTTON;
    }

    return ret;
}

inline CefBrowserHost::MouseButtonType getMouseButtonType(Qt::MouseButton type)
{
    if (type == Qt::LeftButton)
    {
        return MBT_LEFT;
    }
    else if (type == Qt::RightButton)
    {
        return MBT_RIGHT;
    }
    else
    {
        return MBT_MIDDLE;
    }

}

inline bool isKeyDown(WPARAM wparam)
{
    return (GetKeyState(wparam) & 0x8000) != 0;
}

inline int getCefKeyboardModifiers(WPARAM wparam, LPARAM lparam)
{
    int modifiers = 0;
    if (isKeyDown(VK_SHIFT))
        modifiers |= EVENTFLAG_SHIFT_DOWN;
    if (isKeyDown(VK_CONTROL))
        modifiers |= EVENTFLAG_CONTROL_DOWN;
    if (isKeyDown(VK_MENU))
        modifiers |= EVENTFLAG_ALT_DOWN;

    if (::GetKeyState(VK_NUMLOCK) & 1)
        modifiers |= EVENTFLAG_NUM_LOCK_ON;
    if (::GetKeyState(VK_CAPITAL) & 1)
        modifiers |= EVENTFLAG_CAPS_LOCK_ON;

    switch (wparam) {
    case VK_RETURN:
        if ((lparam >> 16) & KF_EXTENDED)
            modifiers |= EVENTFLAG_IS_KEY_PAD;
        break;
    case VK_INSERT:
    case VK_DELETE:
    case VK_HOME:
    case VK_END:
    case VK_PRIOR:
    case VK_NEXT:
    case VK_UP:
    case VK_DOWN:
    case VK_LEFT:
    case VK_RIGHT:
        if (!((lparam >> 16) & KF_EXTENDED))
            modifiers |= EVENTFLAG_IS_KEY_PAD;
        break;
    case VK_NUMLOCK:
    case VK_NUMPAD0:
    case VK_NUMPAD1:
    case VK_NUMPAD2:
    case VK_NUMPAD3:
    case VK_NUMPAD4:
    case VK_NUMPAD5:
    case VK_NUMPAD6:
    case VK_NUMPAD7:
    case VK_NUMPAD8:
    case VK_NUMPAD9:
    case VK_DIVIDE:
    case VK_MULTIPLY:
    case VK_SUBTRACT:
    case VK_ADD:
    case VK_DECIMAL:
    case VK_CLEAR:
        modifiers |= EVENTFLAG_IS_KEY_PAD;
        break;
    case VK_SHIFT:
        if (isKeyDown(VK_LSHIFT))
            modifiers |= EVENTFLAG_IS_LEFT;
        else if (isKeyDown(VK_RSHIFT))
            modifiers |= EVENTFLAG_IS_RIGHT;
        break;
    case VK_CONTROL:
        if (isKeyDown(VK_LCONTROL))
            modifiers |= EVENTFLAG_IS_LEFT;
        else if (isKeyDown(VK_RCONTROL))
            modifiers |= EVENTFLAG_IS_RIGHT;
        break;
    case VK_MENU:
        if (isKeyDown(VK_LMENU))
            modifiers |= EVENTFLAG_IS_LEFT;
        else if (isKeyDown(VK_RMENU))
            modifiers |= EVENTFLAG_IS_RIGHT;
        break;
    case VK_LWIN:
        modifiers |= EVENTFLAG_IS_LEFT;
        break;
    case VK_RWIN:
        modifiers |= EVENTFLAG_IS_RIGHT;
        break;
    }
    return modifiers;
}
