#include "stdafx.h"
#include "qkeystatemanager.h"
#include "MainInterface.h"

QKeyStateManager::QKeyStateManager()
{
    FillMapKey();
    Clear();
}

QKeyStateManager::~QKeyStateManager()
{
}

void QKeyStateManager::Clear()
{
    ZeroMemory(keyState, sizeof(keyState));
}

void QKeyStateManager::SetKey(int key, bool toSet)
{
    for (int i=0; i<KS_KEYMAX; i++)
    {
        if (mapKey[i] == key)
        {
            if (toSet)
            {
                keyState[i] = 1;
            }
            else
            {
                keyState[i] = 0;
            }
            break;
        }
    }
    Q_DASSERT(key);
}

void QKeyStateManager::Flush()
{
    HGE * hge = MainInterface::getInstance().hge;
    for (int i=0; i<KS_KEYMAX; i++)
    {
        if (keyState[i])
        {
            hge->Input_SetDIKey(i);
        }
        else
        {
            hge->Input_SetDIKey(i, false);
        }
    }
//    Clear();
}

void QKeyStateManager::FillMapKey()
{
    mapKey[	DIK_ESCAPE	]=Qt::Key_Escape	;
    mapKey[	DIK_1	]=Qt::Key_1	;
    mapKey[	DIK_2	]=Qt::Key_2	;
    mapKey[	DIK_3	]=Qt::Key_3	;
    mapKey[	DIK_4	]=Qt::Key_4	;
    mapKey[	DIK_5	]=Qt::Key_5	;
    mapKey[	DIK_6	]=Qt::Key_6	;
    mapKey[	DIK_7	]=Qt::Key_7	;
    mapKey[	DIK_8	]=Qt::Key_8	;
    mapKey[	DIK_9	]=Qt::Key_9	;
    mapKey[	DIK_0	]=Qt::Key_0	;
    mapKey[	DIK_MINUS	]=Qt::Key_Minus	;
    mapKey[	DIK_EQUALS	]=Qt::Key_Equal	;
    mapKey[	DIK_BACK	]=Qt::Key_Backspace	;
    mapKey[	DIK_TAB	]=Qt::Key_Tab	;
    mapKey[	DIK_Q	]=Qt::Key_Q	;
    mapKey[	DIK_W	]=Qt::Key_W	;
    mapKey[	DIK_E	]=Qt::Key_E	;
    mapKey[	DIK_R	]=Qt::Key_R	;
    mapKey[	DIK_T	]=Qt::Key_T	;
    mapKey[	DIK_Y	]=Qt::Key_Y	;
    mapKey[	DIK_U	]=Qt::Key_U	;
    mapKey[	DIK_I	]=Qt::Key_I	;
    mapKey[	DIK_O	]=Qt::Key_O	;
    mapKey[	DIK_P	]=Qt::Key_P	;
    mapKey[	DIK_LBRACKET	]=Qt::Key_BracketLeft	;
    mapKey[	DIK_RBRACKET	]=Qt::Key_BracketRight	;
    mapKey[	DIK_RETURN	]=Qt::Key_Return	;
    mapKey[	DIK_LCONTROL	]=Qt::CTRL	;
    mapKey[	DIK_A	]=Qt::Key_A	;
    mapKey[	DIK_S	]=Qt::Key_S	;
    mapKey[	DIK_D	]=Qt::Key_D	;
    mapKey[	DIK_F	]=Qt::Key_F	;
    mapKey[	DIK_G	]=Qt::Key_G	;
    mapKey[	DIK_H	]=Qt::Key_H	;
    mapKey[	DIK_J	]=Qt::Key_J	;
    mapKey[	DIK_K	]=Qt::Key_K	;
    mapKey[	DIK_L	]=Qt::Key_L	;
    mapKey[	DIK_SEMICOLON	]=Qt::Key_Semicolon	;
    mapKey[	DIK_APOSTROPHE	]=Qt::Key_Apostrophe	;
    mapKey[	DIK_GRAVE	]=Qt::Key_Dead_Grave	;
    mapKey[	DIK_LSHIFT	]=Qt::SHIFT	;
    mapKey[	DIK_BACKSLASH	]=Qt::Key_Backslash	;
    mapKey[	DIK_Z	]=Qt::Key_Z	;
    mapKey[	DIK_X	]=Qt::Key_X	;
    mapKey[	DIK_C	]=Qt::Key_C	;
    mapKey[	DIK_V	]=Qt::Key_V	;
    mapKey[	DIK_B	]=Qt::Key_B	;
    mapKey[	DIK_N	]=Qt::Key_N	;
    mapKey[	DIK_M	]=Qt::Key_M	;
    mapKey[	DIK_COMMA	]=Qt::Key_Comma	;
    mapKey[	DIK_PERIOD	]=Qt::Key_Period	;
    mapKey[	DIK_SLASH	]=Qt::Key_Slash	;
    mapKey[	DIK_RSHIFT	]=Qt::SHIFT	;
    mapKey[	DIK_MULTIPLY	]=Qt::Key_multiply	;
    mapKey[	DIK_LMENU	]=Qt::ALT	;
    mapKey[	DIK_SPACE	]=Qt::Key_Space	;
    mapKey[	DIK_CAPITAL	]=Qt::Key_CapsLock	;
    mapKey[	DIK_F1	]=Qt::Key_F1	;
    mapKey[	DIK_F2	]=Qt::Key_F2	;
    mapKey[	DIK_F3	]=Qt::Key_F3	;
    mapKey[	DIK_F4	]=Qt::Key_F4	;
    mapKey[	DIK_F5	]=Qt::Key_F5	;
    mapKey[	DIK_F6	]=Qt::Key_F6	;
    mapKey[	DIK_F7	]=Qt::Key_F7	;
    mapKey[	DIK_F8	]=Qt::Key_F8	;
    mapKey[	DIK_F9	]=Qt::Key_F9	;
    mapKey[	DIK_F10	]=Qt::Key_F10	;
    mapKey[	DIK_NUMLOCK	]=Qt::Key_NumLock	;
    mapKey[	DIK_SCROLL	]=Qt::Key_ScrollLock	;
    mapKey[	DIK_NUMPAD7	]=Qt::Key_7	;
    mapKey[	DIK_NUMPAD8	]=Qt::Key_8	;
    mapKey[	DIK_NUMPAD9	]=Qt::Key_9	;
    mapKey[	DIK_SUBTRACT	]=Qt::Key_Minus	;
    mapKey[	DIK_NUMPAD4	]=Qt::Key_4	;
    mapKey[	DIK_NUMPAD5	]=Qt::Key_5	;
    mapKey[	DIK_NUMPAD6	]=Qt::Key_6	;
    mapKey[	DIK_ADD	]=Qt::Key_Plus	;
    mapKey[	DIK_NUMPAD1	]=Qt::Key_1	;
    mapKey[	DIK_NUMPAD2	]=Qt::Key_2	;
    mapKey[	DIK_NUMPAD3	]=Qt::Key_3	;
    mapKey[	DIK_NUMPAD0	]=Qt::Key_0	;
    mapKey[	DIK_DECIMAL	]=Qt::Key_Period	;
    mapKey[	DIK_OEM_102	]=Qt::Key_Backslash	;
    mapKey[	DIK_F11	]=Qt::Key_F11	;
    mapKey[	DIK_F12	]=Qt::Key_F12	;
    mapKey[	DIK_F13	]=Qt::Key_F13	;
    mapKey[	DIK_F14	]=Qt::Key_F14	;
    mapKey[	DIK_F15	]=Qt::Key_F15	;
    mapKey[	DIK_KANA	]=Qt::Key_Hiragana_Katakana	;
    mapKey[	DIK_ABNT_C1	]=Qt::Key_Question	;
    mapKey[	DIK_CONVERT	]=Qt::Key_Henkan	;
    mapKey[	DIK_NOCONVERT	]=Qt::Key_Muhenkan	;
    mapKey[	DIK_YEN	]=Qt::Key_yen	;
    mapKey[	DIK_ABNT_C2	]=Qt::Key_Period	;
    mapKey[	DIK_NUMPADEQUALS	]=Qt::Key_Equal	;
    mapKey[	DIK_PREVTRACK	]=Qt::Key_MediaPrevious	;
    mapKey[	DIK_AT	]=Qt::Key_At	;
    mapKey[	DIK_COLON	]=Qt::Key_Colon	;
    mapKey[	DIK_UNDERLINE	]=Qt::Key_Underscore	;
    mapKey[	DIK_KANJI	]=Qt::Key_Kanji	;
    mapKey[	DIK_STOP	]=Qt::Key_MediaStop	;
    mapKey[	DIK_AX	]=Qt::Key_unknown	;
    mapKey[	DIK_UNLABELED	]=Qt::Key_unknown	;
    mapKey[	DIK_NEXTTRACK	]=Qt::Key_MediaNext	;
    mapKey[	DIK_NUMPADENTER	]=Qt::Key_Enter	;
    mapKey[	DIK_RCONTROL	]=Qt::CTRL	;
    mapKey[	DIK_MUTE	]=Qt::Key_VolumeMute	;
    mapKey[	DIK_CALCULATOR	]=Qt::Key_Calculator	;
    mapKey[	DIK_PLAYPAUSE	]=Qt::Key_MediaTogglePlayPause	;
    mapKey[	DIK_MEDIASTOP	]=Qt::Key_MediaStop	;
    mapKey[	DIK_VOLUMEDOWN	]=Qt::Key_VolumeDown	;
    mapKey[	DIK_VOLUMEUP	]=Qt::Key_VolumeUp	;
    mapKey[	DIK_WEBHOME	]=Qt::Key_HomePage	;
    mapKey[	DIK_NUMPADCOMMA	]=Qt::Key_Comma	;
    mapKey[	DIK_DIVIDE	]=Qt::Key_division	;
    mapKey[	DIK_SYSRQ	]=Qt::Key_SysReq	;
    mapKey[	DIK_RMENU	]=Qt::ALT	;
    mapKey[	DIK_PAUSE	]=Qt::Key_Pause	;
    mapKey[	DIK_HOME	]=Qt::Key_Home	;
    mapKey[	DIK_UP	]=Qt::Key_Up	;
    mapKey[	DIK_PRIOR	]=Qt::Key_PageUp	;
    mapKey[	DIK_LEFT	]=Qt::Key_Left	;
    mapKey[	DIK_RIGHT	]=Qt::Key_Right	;
    mapKey[	DIK_END	]=Qt::Key_End	;
    mapKey[	DIK_DOWN	]=Qt::Key_Down	;
    mapKey[	DIK_NEXT	]=Qt::Key_PageDown	;
    mapKey[	DIK_INSERT	]=Qt::Key_Insert	;
    mapKey[	DIK_DELETE	]=Qt::Key_Delete	;
    mapKey[	DIK_LWIN	]=Qt::Key_Meta	;
    mapKey[	DIK_RWIN	]=Qt::Key_Meta	;
    mapKey[	DIK_APPS	]=Qt::Key_ApplicationRight	;
    mapKey[	DIK_POWER	]=Qt::Key_PowerOff	;
    mapKey[	DIK_SLEEP	]=Qt::Key_Sleep	;
    mapKey[	DIK_WAKE	]=Qt::Key_WakeUp	;
    mapKey[	DIK_WEBSEARCH	]=Qt::Key_Search	;
    mapKey[	DIK_WEBFAVORITES	]=Qt::Key_Favorites	;
    mapKey[	DIK_WEBREFRESH	]=Qt::Key_Refresh	;
    mapKey[	DIK_WEBSTOP	]=Qt::Key_Stop	;
    mapKey[	DIK_WEBFORWARD	]=Qt::Key_Forward	;
    mapKey[	DIK_WEBBACK	]=Qt::Key_Back	;
    mapKey[	DIK_MYCOMPUTER	]=Qt::Key_unknown	;
    mapKey[	DIK_MAIL	]=Qt::Key_LaunchMail	;
    mapKey[	DIK_MEDIASELECT	]=Qt::Key_Select	;

}
