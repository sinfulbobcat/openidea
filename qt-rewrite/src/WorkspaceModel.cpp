#include "WorkspaceModel.hpp"

#include <QDir>
#include <QFileSystemModel>
#include <QModelIndex>

class WorkspaceModel::Impl {
public:
    QFileSystemModel model;
    QString rootPath;
};

WorkspaceModel::WorkspaceModel()
    : m_impl(new Impl())
{
    m_impl->model.setReadOnly(true);
    m_impl->model.setFilter(QDir::Dirs | QDir::Files | QDir::NoDotAndDotDot);
}

WorkspaceModel::~WorkspaceModel()
{
    delete m_impl;
}

void WorkspaceModel::setRootPath(const QString& path)
{
    m_impl->rootPath = path;
    m_impl->model.setRootPath(path);
}

QString WorkspaceModel::currentPath() const
{
    return m_impl->rootPath;
}
