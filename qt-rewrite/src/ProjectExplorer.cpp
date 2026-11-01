#include "ProjectExplorer.hpp"

#include <QFileSystemModel>
#include <QHeaderView>
#include <QTreeView>
#include <QVBoxLayout>

ProjectExplorer::ProjectExplorer(QWidget* parent)
    : QWidget(parent)
{
    auto* layout = new QVBoxLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);

    m_tree = new QTreeView(this);
    m_tree->setHeaderHidden(false);
    m_tree->header()->setStretchLastSection(true);

    m_model = new QFileSystemModel(this);
    m_model->setReadOnly(true);
    m_model->setFilter(QDir::Dirs | QDir::Files | QDir::NoDotAndDotDot);

    m_tree->setModel(m_model);
    layout->addWidget(m_tree);
}

void ProjectExplorer::setRootPath(const QString& path)
{
    if (!m_model) {
        return;
    }

    setProperty("rootPath", path);
    m_model->setRootPath(path);
    m_tree->setRootIndex(m_model->index(path));
}
