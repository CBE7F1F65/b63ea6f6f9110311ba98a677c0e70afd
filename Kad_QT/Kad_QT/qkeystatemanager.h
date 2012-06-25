#ifndef QKEYSTATEMANAGER_H
#define QKEYSTATEMANAGER_H

#define KS_KEYMAX   256

class QKeyStateManager
{
public:
    static QKeyStateManager& getInstance() { static QKeyStateManager instance; return instance; }

private:
    QKeyStateManager();
    ~QKeyStateManager();
    QKeyStateManager(QKeyStateManager const&);
    void operator=(QKeyStateManager const&);

private:
    char keyState[KS_KEYMAX];
    int mapKey[KS_KEYMAX];
    void FillMapKey();

public:
    void Clear();
    void SetKey(int key, bool toSet=true);
    void Flush();

};

#endif // QKEYSTATEMANAGER_H
