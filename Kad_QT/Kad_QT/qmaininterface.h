#ifndef QMAININTERFACE_H
#define QMAININTERFACE_H

#include "kad_qt.h"
#include "qtui_glview.h"
#include "qtui_commandpanel_commandlog.h"
#include "qtui_commandpanel_commandedit.h"
#include "qtui_commandfloating_widget.h"
#include "qtui_commandfloating_command.h"
#include "qtui_history_table.h"

class QMainInterface
{
public:
    static QMainInterface& getInstance() { static QMainInterface instance; return instance; }

private:
    QMainInterface();
    ~QMainInterface();
    QMainInterface(QMainInterface const&);
    void operator=(QMainInterface const&);

public:

    void OnMainFrameSetupUIDone();

    /*
    Kad_QT * pMainWindow;
    void SetPMainWindow(Kad_QT * p)
    {
        Q_ASSERT(pMainWindow == NULL);
        pMainWindow=p;
    };
    Kad_QT * GetPMainWindow()
    {
        Q_ASSERT(pMainWindow != NULL);
        return pMainWindow;
    };
    */
#define _QMWDEF(T, N)   \
    T * p##N;   \
    void SetP##N(T * p) \
    {   \
        Q_ASSERT(p##N == NULL);  \
        p##N=p;  \
    };  \
    T * GetP##N()   \
    {   \
        Q_ASSERT(p##N != NULL);  \
        return p##N; \
    };

    _QMWDEF(Kad_QT, MainWindow);
    _QMWDEF(QTUI_GLView, GLView);
    _QMWDEF(QTUI_CommandPanel_CommandLog, CommandLog);
    _QMWDEF(QTUI_CommandPanel_CommandEdit, CommandEdit);
    _QMWDEF(QTUI_CommandDockable, CommandDockable);
    _QMWDEF(QTUI_CommandFloating_Widget, CommandFloatingWidget);
    _QMWDEF(QTUI_CommandFloating_Command, CommandFloatingEdit);
    _QMWDEF(QTUI_History_DockablePane, HistoryDockable);
    _QMWDEF(QTUI_History_Table, HistoryTable);

#undef _QMWDEF

};

#endif // QMAININTERFACE_H
