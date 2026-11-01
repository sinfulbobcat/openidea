#pragma once

#include <QWidget>

class QFileSystemModel;
class QTreeView;

class ProjectExplorer : public QWidget {
    Q_OBJECT

public:
    explicit ProjectExplorer(QWidget* parent = nullptr);
    void setRootPath(const QString& path);

private:
    QTreeView* m_tree = nullptr;
    QFileSystemModel* m_model = nullptr;
};
