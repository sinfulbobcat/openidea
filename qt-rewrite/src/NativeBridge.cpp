#include "NativeBridge.hpp"

#include "AppSettings.hpp"

#include <QCoreApplication>
#include <QDir>
#include <QFileDialog>
#include <QFileInfo>
#include <QSettings>
#include <QStandardPaths>
#include <QVariantList>

namespace {

QString currentPlatform()
{
#if defined(Q_OS_WIN)
    return "windows";
#elif defined(Q_OS_MACOS)
    return "macos";
#else
    return "linux";
#endif
}

QString canonicalDirectory(const QString& path)
{
    return QFileInfo(path).canonicalFilePath();
}

QStringList trustList(const QString& key)
{
    return QSettings("OpenIDEA", "QtRewrite").value(key).toStringList();
}

void setTrustList(const QString& key, const QStringList& paths)
{
    QSettings("OpenIDEA", "QtRewrite").setValue(key, paths);
}

bool directoryContains(const QString& directory, const QString& path)
{
    const QString relative = QDir(directory).relativeFilePath(path);
    return relative == "." || (relative != ".." && !relative.startsWith("../") && !QDir::isAbsolutePath(relative));
}

QVariantMap projectTrustStatus(const QString& root)
{
    const QStringList restrictedProjects = trustList("trust/restricted");
    const bool restricted = restrictedProjects.contains(root);
    bool trusted = !restricted && trustList("trust/projects").contains(root);
    if (!restricted && !trusted) {
        const QStringList locations = trustList("trust/locations");
        for (const QString& location : locations) {
            if (directoryContains(location, root)) {
                trusted = true;
                break;
            }
        }
    }
    return {{"root", root}, {"trusted", trusted}, {"decided", trusted || restricted}};
}

} // namespace

NativeBridge::NativeBridge(QObject* parent)
    : QObject(parent)
{
}

void NativeBridge::invoke(const QString& requestId, const QString& command, const QVariantMap& args)
{
    const auto fail = [this, &requestId](const QString& message) {
        const QVariantMap error = {{"message", message}};
        emit response(requestId, QVariantMap{{"ok", false}, {"error", error}});
    };

    if (command == "app_info") {
        const QVariantMap info = {
            {"version", QCoreApplication::applicationVersion()},
            {"platform", currentPlatform()},
            {"configDir", QStandardPaths::writableLocation(QStandardPaths::AppConfigLocation)},
        };
        emit response(requestId, {{"ok", true}, {"value", info}});
        return;
    }

    if (command == "home_dir") {
        emit response(requestId, {{"ok", true}, {"value", QDir::homePath()}});
        return;
    }

    if (command == "dialog_open_directory") {
        const QString path = QFileDialog::getExistingDirectory(
            qobject_cast<QWidget*>(parent()), args.value("title").toString(), args.value("defaultPath").toString());
        emit response(requestId, {{"ok", true}, {"value", path.isEmpty() ? QVariant{} : QVariant(path)}});
        return;
    }

    if (command == "dialog_open_files") {
        const QStringList paths = QFileDialog::getOpenFileNames(
            qobject_cast<QWidget*>(parent()), args.value("title").toString(), args.value("defaultPath").toString());
        emit response(requestId, {{"ok", true}, {"value", paths}});
        return;
    }

    if (command == "recent_projects") {
        QVariantList projects;
        for (const QString& path : AppSettings().recentProjects()) {
            const QFileInfo info(path);
            if (!info.isDir()) continue;
            projects.append(QVariantMap{
                {"path", info.absoluteFilePath()},
                {"name", info.fileName()},
                {"openedAtMs", QVariant{}},
                {"branch", QVariant{}},
            });
        }
        emit response(requestId, {{"ok", true}, {"value", projects}});
        return;
    }

    if (command == "remove_recent_project") {
        const QString path = args.value("path").toString();
        QStringList projects = AppSettings().recentProjects();
        projects.removeAll(path);
        AppSettings settings;
        settings.setRecentProjects(projects);
        emit response(requestId, {{"ok", true}, {"value", QVariant{}}});
        return;
    }

    if (command == "project_trust_status") {
        const QString root = canonicalDirectory(args.value("path").toString());
        if (root.isEmpty()) {
            fail("Project directory does not exist");
            return;
        }
        emit response(requestId, {{"ok", true}, {"value", projectTrustStatus(root)}});
        return;
    }

    if (command == "project_trust_set") {
        const QString root = canonicalDirectory(args.value("path").toString());
        if (root.isEmpty()) {
            fail("Project directory does not exist");
            return;
        }
        const bool trusted = args.value("trusted").toBool();
        const bool folder = args.value("folder").toBool();
        QStringList projects = trustList("trust/projects");
        QStringList locations = trustList("trust/locations");
        QStringList restricted = trustList("trust/restricted");
        if (trusted) {
            restricted.removeAll(root);
            if (folder) {
                locations.removeAll(root);
                locations.prepend(root);
            } else {
                projects.removeAll(root);
                projects.prepend(root);
            }
        } else {
            projects.removeAll(root);
            restricted.removeAll(root);
            restricted.prepend(root);
            if (folder) locations.removeAll(root);
        }
        setTrustList("trust/projects", projects);
        setTrustList("trust/locations", locations);
        setTrustList("trust/restricted", restricted);
        emit response(requestId, {{"ok", true}, {"value", QVariant{}}});
        return;
    }

    if (command == "project_trust_list") {
        const QVariantMap decisions = {
            {"projects", trustList("trust/projects")},
            {"locations", trustList("trust/locations")},
            {"restricted", trustList("trust/restricted")},
        };
        emit response(requestId, {{"ok", true}, {"value", decisions}});
        return;
    }

    if (command == "project_trust_remove_location") {
        QStringList locations = trustList("trust/locations");
        locations.removeAll(canonicalDirectory(args.value("path").toString()));
        setTrustList("trust/locations", locations);
        emit response(requestId, {{"ok", true}, {"value", QVariant{}}});
        return;
    }

    if (command == "window_open_project") {
        if (args.value("newWindow").toBool()) {
            fail("New project windows are not implemented");
            return;
        }
        emit response(requestId, {{"ok", true}, {"value", QVariantMap{{"kind", "current"}}}});
        return;
    }

    if (command == "window_set_title") {
        if (auto* window = qobject_cast<QWidget*>(parent())) {
            window->setWindowTitle(args.value("title").toString());
        }
        emit response(requestId, {{"ok", true}, {"value", QVariant{}}});
        return;
    }

    if (command == "open_project") {
        const QString requestedPath = args.value("path").toString();
        const QString root = canonicalDirectory(requestedPath);
        if (root.isEmpty() || !QFileInfo(root).isDir()) {
            fail(QString("Project directory does not exist: %1").arg(requestedPath));
            return;
        }

        AppSettings settings;
        QStringList projects = settings.recentProjects();
        projects.removeAll(root);
        projects.prepend(root);
        while (projects.size() > 30) projects.removeLast();
        settings.setRecentProjects(projects);

        const QVariantMap project = {
            {"trusted", projectTrustStatus(root).value("trusted").toBool()},
            {"root", root},
            {"name", QFileInfo(root).fileName()},
            {"isGit", QFileInfo(QDir(root).filePath(".git")).exists()},
            {"settings", QVariantMap{}},
        };
        emit response(requestId, {{"ok", true}, {"value", project}});
        return;
    }

    if (command == "fs_read_dir") {
        const QString path = args.value("path").toString();
        const QDir directory(path);
        if (!directory.exists()) {
            const QVariantMap error = {
                {"message", QString("Directory does not exist: %1").arg(path)},
            };
            emit response(requestId, QVariantMap{
                {"ok", false},
                {"error", error},
            });
            return;
        }

        QVariantList entries;
        const QFileInfoList children = directory.entryInfoList(
            QDir::AllEntries | QDir::Hidden | QDir::System | QDir::NoDotAndDotDot,
            QDir::DirsFirst | QDir::Name | QDir::IgnoreCase);
        for (const QFileInfo& info : children) {
            const bool symlink = info.isSymLink();
            const QString kind = symlink ? "symlink" : (info.isDir() ? "dir" : "file");
            const QDateTime modified = info.lastModified();
            QVariantMap entry = {
                {"name", info.fileName()},
                {"path", info.absoluteFilePath()},
                {"kind", kind},
                {"size", info.isDir() ? 0 : info.size()},
                {"mtimeMs", modified.isValid() ? QVariant::fromValue<qint64>(modified.toMSecsSinceEpoch()) : QVariant{}},
                {"ignored", false},
                {"excluded", false},
                {"hidden", info.isHidden()},
            };
            if (symlink) {
                entry.insert("targetKind", info.exists() ? QVariant(info.isDir() ? "dir" : "file") : QVariant{});
            }
            entries.append(entry);
        }
        emit response(requestId, {{"ok", true}, {"value", entries}});
        return;
    }

    fail(QString("C++ backend command not implemented: %1").arg(command));
}