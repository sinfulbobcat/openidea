#pragma once

#include <QString>

class QAbstractItemModel;

class WorkspaceModel {
public:
    WorkspaceModel();
    ~WorkspaceModel();

    void setRootPath(const QString& path);
    QString currentPath() const;

private:
    class Impl;
    Impl* m_impl = nullptr;
};
