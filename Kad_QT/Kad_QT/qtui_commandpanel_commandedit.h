#ifndef QTUI_COMMANDPANEL_COMMANDEDIT_H
#define QTUI_COMMANDPANEL_COMMANDEDIT_H

#include <QPlainTextEdit>

class QTUI_CommandPanel_CommandEdit : public QPlainTextEdit
{
    Q_OBJECT
public:
    explicit QTUI_CommandPanel_CommandEdit(QWidget *parent = 0);
    
protected:
    virtual void keyPressEvent(QKeyEvent * e);

signals:
    
public slots:
    
};

#endif // QTUI_COMMANDPANEL_COMMANDEDIT_H
