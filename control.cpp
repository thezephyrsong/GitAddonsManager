/* Copyright 2018-2019 WobLight
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "control.h"
#include <QSettings>
#include <QDir>
#include <QFutureWatcher>
#include <QQuickStyle>
#include <QDesktopServices>
#include "addon.h"
#include <git2.h>
#include <QDebug>
#include <QtConcurrent>
#include <type_traits>
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QApplication>
#include <quazip.h>
#include <quazipfile.h>
#include <QTextStream>
#include <ranges>
#include <algorithm>

Control *Control::m_instance = nullptr;
QStringList Control::m_availableStyles;
QNetworkAccessManager *nam = nullptr;

Control::Control(QObject *parent) : QObject(parent),
    m_progress(0), m_total(0), m_status(Status::Ready), m_statusMessage(""), m_pool(new QThreadPool(this)),
    m_style(QQuickStyle::name())
{
    connect(this, &Control::addonsPathChanged, this, &Control::scanForAddons);
}

void Control::init() {
    git_libgit2_init();
    QSettings settings;
    if (settings.value("addonsPaths").isNull()) {
        setAddonsPath(-1, settings.value("addonsPath").toString());
        settings.remove("addonsPath");
    } else {
        auto paths = settings.value("addonsPaths").toStringList();
        for (int i = 0; i < paths.length(); i++)
            setAddonsPath(-1, paths[i]);
    }
    setUseRepoDirectory(settings.value("useRepoDirectory", false).toBool());
    setFirstBoot(settings.value("firstBoot",true).toBool());
    setMinimizeToTray((MinimizeToTray)settings.value("minimizeToTray",(int)MinimizeToTrayAsk).toInt());
    nam = new QNetworkAccessManager(this);
    checkForUpdates();

    connect(this, &Control::addonsPathsChanged, this, &Control::saveAddonsPaths);
    connect(this, &Control::useRepoDirectoryChanged, this, &Control::saveUseRepoDirectory);
}

QStringList Control::addonsPaths() const
{
    return m_addonsPaths;
}

void Control::removeTocSuffixes(QString &string)
{
    string.replace("-Classic.toc",".toc", Qt::CaseInsensitive);
    string.replace("-BCC.toc",".toc", Qt::CaseInsensitive);
    string.replace("_Vanilla.toc",".toc", Qt::CaseInsensitive);
    string.replace("_TBC.toc",".toc", Qt::CaseInsensitive);
    string.replace("_Mainline.toc",".toc", Qt::CaseInsensitive);
    string.replace("_Wrath.toc",".toc", Qt::CaseInsensitive);
    string.replace("-WOTLKC.toc",".toc", Qt::CaseInsensitive);
}

void Control::delegate(QString taskname, auto work, auto callback)
{
    Q_ASSERT_X(QThread::currentThread() == thread(), "delegate", "Attempt to delegate from another thread.");
    if (m_lock) {
        qInfo() << "Enqueueing" << taskname;
        m_tasks.enqueue({taskname, [this, taskname, work, callback](){delegate(taskname, work,callback);}});
        return;
    }
    qInfo() << "Executing " << taskname;
    setStatusMessage(taskname);
    setProgress(0);
    setTotal(0);
    BusyLock lock(this);
    m_lock = true;
    using ret_t = typename std::invoke_result<decltype(work)>::type;
    QFuture<ret_t> fut = QtConcurrent::run(m_pool, work);
    QFutureWatcher<ret_t> *fw = new QFutureWatcher<ret_t>();
    connect(fw, &QFutureWatcher<ret_t>::finished, [this, callback, taskname, fw, lock=std::move(lock)](){
        m_lock = false;
        TaskQueue old;
        m_tasks.swap(old);

        qInfo() << "Executing callback for " << taskname;
        if constexpr (std::is_same<ret_t, void>::value)
            callback();
        else
            callback(fw->result());
        
        qInfo() << "Completed " << taskname;
        
        m_tasks.append(old);

        if (m_status != Status::Error) {
            setStatusMessage("");
            if (m_tasks.empty())
                setStatus(Status::Ready);
            else if (!m_lock)
                m_tasks.dequeue().second();
        }
    });
    connect(fw, &QFutureWatcher<ret_t>::finished, fw, &QFutureWatcher<ret_t>::deleteLater);
    fw->setFuture(fut);
}

void Control::delegate(QString taskname, auto work)
{
    using ret_t = typename std::invoke_result<decltype(work)>::type;
    if constexpr (std::is_same<ret_t, void>::value)
        delegate(taskname, work, [](){});
    else
        delegate(taskname, work, [](ret_t){});
}

Control::~Control()
{
    m_instance = nullptr;
}

QList<Addon *> Control::addons() const
{
    return m_addons;
}

Control *Control::instance()
{
    if (!m_instance)
        m_instance = new Control();
    return m_instance;
}

int Control::progress() const
{
    return m_progress;
}

int Control::total() const
{
    return m_total;
}

Control::Status Control::status() const
{
    return m_status;
}

QString Control::statusMessage() const
{
    return m_statusMessage;
}

bool Control::firstBoot() const
{
    return m_firstBoot;
}

Control::MinimizeToTray Control::minimizeToTray() const
{
    return m_minimizeToTray;
}

QString Control::style() const
{
    return m_style;
}

QStringList Control::availableStyles() const
{
    return m_availableStyles;
}

Control::UpdateStatus Control::updateStatus() const
{
    return m_updateStatus;
}


void walkFoldersIf(const QFileInfo &info, auto f, auto t){
    int ret = t(info);
    if (ret <= 0) {
        if (info.isDir() && !info.isSymLink()) {
            foreach (QFileInfo sub, QDir(info.absoluteFilePath()).entryInfoList(QDir::AllEntries|QDir::NoDotAndDotDot|QDir::Hidden))
                walkFoldersIf(sub, f, t);
        }
    }
    if (ret >= 0)
        f(info);
}

void Control::completeUpdate(const QString &path)
{
    delegate("Update Application",[this, path](){
        QStringList files;
        QStringList oldFiles;
        QDir root(QApplication::applicationDirPath());
        QDir newRoot(path);
        if (!newRoot.mkpath(newRoot.absolutePath())) return false;
        QFile oldFilesList(newRoot.absoluteFilePath(".installedFiles"));
        if (oldFilesList.exists()) {
            oldFilesList.open(QFile::ReadOnly);
            QString line;
            QStringList oldFiles;
            QStringList oldDirs;
            while (!(line = oldFilesList.readLine()).isEmpty()) {
                if (line.startsWith('#') || line.startsWith('\n')) continue;
                oldFiles << line.chopped(1);
            }
            oldFilesList.close();
            setTotal(oldFiles.size());
            int count = 0;
            foreach (const QString &file, oldFiles) {
                QFileInfo info(newRoot.absoluteFilePath(file));
                if (info.exists() && (info.isFile() || info.isSymLink())) {
                    newRoot.remove(file);
                    setProgress(++count);
                }
                else if (info.isDir())
                    oldDirs << file;
            }
            for (auto i = oldDirs.rbegin(); i != oldDirs.rend(); i++)
                newRoot.rmdir(*i);

            setProgress(0);
            setTotal(-1);
        }
        walkFoldersIf(QFileInfo{root.absolutePath()},[&newRoot, &files, &root, &oldFiles](const QFileInfo &info){
            QFileInfo dest(newRoot.absoluteFilePath(root.relativeFilePath(info.absoluteFilePath())));
            files << info.absoluteFilePath();
            if (dest.exists())
                walkFoldersIf(dest,[&oldFiles](const QFileInfo &info){
                    oldFiles << info.absoluteFilePath();
                }, [](const auto&){return 0;});

        }, [&path, &root](const QFileInfo &info){
            QFileInfo dest(path + "/" + root.relativeFilePath(info.absoluteFilePath()));
            return !dest.exists() ? 1 : dest.isDir() && info.isDir() && !info.isSymLink() && !dest.isSymLink() ? -1 : 1;
        });
        setTotal(files.size() + oldFiles.size());
        qDebug() << oldFiles.size();
        int count = 0;
        bool success = true;
        foreach (auto file, oldFiles) {
            QFileInfo info(file);
            if (info.isDir() && !info.isSymLink())
                success &= newRoot.rmdir(file);
            else
                success &= newRoot.remove(file);
            setProgress(++count);
        }
        foreach (auto file, files) {
            success &= root.rename(file, newRoot.absoluteFilePath(root.relativeFilePath(file))) || root.rmdir(file);
            setProgress(++count);
        }
        setTotal(-1);
        if (success)
            root.removeRecursively();
        return success;
    },[this, &path](bool success){
        if (success)
            QApplication::exit(3);
        else {
            setStatusMessage(tr("Failed to move files to %1.").arg(path));
            setStatus(Status::Error);
        }
    });
}

void Control::setAddons(QList<Addon *> addons)
{
    if (m_addons == addons)
        return;

    m_addons = addons;
    emit addonsChanged(m_addons);
}

void Control::setAddonsPath(int index, QString addonsPath)
{
    if (addonsPath.isNull()) {
        for (int i = 0; i < m_addons.size();) {
            int s = m_addons.size();
            if (static_cast<Addon*>(m_addons[i])->path() == m_addonsPaths[index])
                delete m_addons.takeAt(i);
            if (s != m_addons.size())
                emit addonsChanged(m_addons);
            else i++;
        }
        if (index < 0)
            return;
        m_addonsPaths.removeAt(index);
        emit addonsPathsChanged(m_addonsPaths);
        return;
    }
    addonsPath = QUrl::fromLocalFile(addonsPath).adjusted(QUrl::NormalizePathSegments|QUrl::StripTrailingSlash).toLocalFile();
    if (index == -1) {
        m_addonsPaths.append(addonsPath);
        index = m_addonsPaths.size() -1;
    } else {
        if (m_addonsPaths[index] == addonsPath)
            return;
        for (int i = 0; i < m_addons.size();) {
            if (static_cast<Addon*>(m_addons[i])->path() == m_addonsPaths[index])
                delete m_addons.takeAt(i);
            else i++;
        }
        m_addonsPaths[index] = addonsPath;
    }

    emit addonsPathChanged(index, m_addonsPaths[index]);
    emit addonsPathsChanged(m_addonsPaths);
}

void Control::saveUseRepoDirectory(bool use)
{
    QSettings settings;
    settings.setValue("useRepoDirectory", use);
}

void Control::saveAddonsPaths()
{
    QSettings settings;
    settings.setValue("addonsPaths", m_addonsPaths);
}

void Control::scanForAddons(int i)
{
    if (i == -1) {
        BusyLock lock(this);
        foreach (Addon *addon, m_addons)
            addon->deleteLater();
        QList<Addon *> ol;
        setAddons(ol);
        for (int k = 0; k < m_addonsPaths.size(); ++k)
            scanForAddons(k);
        return;
    }
    using init_t = QPair<QString, git_repository*>;
    delegate("Scanning addons folder...", [this, i](){
        QList<init_t> list;
        QDir addonsFolder(m_addonsPaths[i]);
        foreach (QFileInfo dirInfo, addonsFolder.entryInfoList(QDir::AllDirs)) {
            QDir gitdir(dirInfo.canonicalFilePath() + "/.git");
            if (!gitdir.exists()) continue;
            git_repository *repo = nullptr;
            int error = git_repository_open(&repo, dirInfo.canonicalFilePath().toLocal8Bit());
            if (error) continue;

            //QFileInfo toc(dirInfo.canonicalFilePath() + "/" + dirInfo.fileName() + ".toc");
            //if (toc.exists())
                list << init_t(dirInfo.fileName(), repo);
        }
        return list;
    }, [this, k=i](auto list){
        foreach (init_t i, list)
            m_addons << new Addon(i.first, i.second, m_addonsPaths[k]);
        emit addonsChanged(m_addons);
    });
}

int clone_progress_cb(const git_transfer_progress *stats, void *payload) {
    Q_UNUSED(payload)
    Control::instance()->setTotal(stats->total_objects);
    Control::instance()->setProgress(stats->received_objects);
    return 0;
}

void Control::clone(QUrl url, int i)
{
    delegate("Cloning addon...",[this, url, i]() {
        struct {
            QString name;
            git_repository *repo = nullptr;
            QString error;
        } data;
        data.name = url.fileName().section('.',0,0);
        if (data.name.isEmpty())
            data.name = url.path().split('/', Qt::SkipEmptyParts).last();

        if (data.name.isEmpty()) {
            data.error = QString("Invalid url.");
            return data;
        }
        git_clone_options opts = GIT_CLONE_OPTIONS_INIT;
        opts.fetch_opts.callbacks.transfer_progress = &clone_progress_cb;
        int error = git_clone(&data.repo, url.toString().toLocal8Bit(), (m_addonsPaths[i] + "/" + data.name).toLocal8Bit(), &opts);
        if (error < 0) {
            const git_error *e = giterr_last();
            data.error = QString::asprintf("Error %d/%d: %s", error,  e->klass, e->message);
            return data;
        }

        QDir dir(m_addonsPaths[i]);
        dir.cd(data.name);
        QStringList tocs = dir.entryList({"*.toc"});
        QString expectedTocName = dir.dirName() + ".toc";
        if (!tocs.isEmpty()) {
            std::transform(tocs.begin(), tocs.end(),
                           tocs.begin(),
                           [](auto toc) {
                               Control::removeTocSuffixes(toc);
                            return toc;
            });
            if (!std::all_of(tocs.begin() + 1, tocs.end(),
                       [tocs](const auto &toc){ return !QString::compare(toc, tocs.first()); }))
                qWarning() << "Multiple toc files found! Expect troubles...";
            else if (QString::compare(tocs.first(), dir.dirName() + ".toc", Qt::CaseInsensitive)) {
                git_repository_free(data.repo);
                dir.cdUp();
                QString newName = tocs.first().chopped(4);
                dir.rename(data.name, newName);
                data.name = newName;
                dir.cd(data.name);
                git_repository_open(&data.repo, dir.canonicalPath().toLocal8Bit());
            }
        }
        return data;
    },[this, i](auto ret){
        if (ret.error.isEmpty()) {
            auto add = new Addon(ret.name, ret.repo, m_addonsPaths[i]);
            m_addons.prepend(add);
            addonsChanged(m_addons);
            add->unpackSubfolders();
        } else {
            setStatus(Status::Error);
            setStatusMessage(ret.error);
        }
    });
}

void Control::setProgress(int progress)
{
    if (m_progress == progress)
        return;

    m_progress = progress;
    emit progressChanged(m_progress);
}

void Control::setTotal(int total)
{
    if (m_total == total)
        return;

    m_total = total;
    emit totalChanged(m_total);
}

void Control::setStatus(Control::Status status)
{
    if (m_status == status)
        return;

    m_status = status;
    emit statusChanged(m_status);
}

void Control::setStatusMessage(QString statusMessage)
{
    if (m_statusMessage == statusMessage)
        return;

    m_statusMessage = statusMessage;
    emit statusMessageChanged(m_statusMessage);
}

void Control::setFirstBoot(bool firstBoot)
{
    if (m_firstBoot == firstBoot)
        return;

    m_firstBoot = firstBoot;
    emit firstBootChanged(m_firstBoot);

    QSettings s;
    s.setValue("firstBoot", false);
}

void Control::setMinimizeToTray(MinimizeToTray minimizeToTray)
{
    if (m_minimizeToTray == minimizeToTray)
        return;

    m_minimizeToTray = minimizeToTray;
    emit minimizeToTrayChanged(m_minimizeToTray);
    QSettings s;
    s.setValue("minimizeToTray",(int)minimizeToTray);
}

void Control::setStyle(QString theme)
{
    if (m_style == theme)
        return;

    m_style = theme;
    emit styleChanged(m_style);
}

void Control::setAvailableStyles(QStringList availableStyles)
{
    if (m_availableStyles == availableStyles)
        return;

    m_availableStyles = availableStyles;
    emit availableStylesChanged(m_availableStyles);
}

void Control::checkForUpdates()
{
#ifdef GAM_SELF_UPDATE
    QNetworkRequest req(QUrl(QString("https://gitlab.com/woblight/GitAddonsManager/-/jobs/artifacts/%1/download?job=%2").arg(GAM_BRANCH_NAME).arg(GAM_BUILD_NAME)));
    req.setAttribute(QNetworkRequest::RedirectPolicyAttribute, QNetworkRequest::NoLessSafeRedirectPolicy);
    connect(nam, &QNetworkAccessManager::finished, [](auto reply){
        QApplication::setApplicationDisplayName(reply->errorString());
    });
    auto reply = nam->head(req);
    setUpdateStatus(UpdateStatus::CheckingForUpdate);
    connect(reply, &QNetworkReply::finished,[this, reply](){
        QRegularExpression shaCapt(QString("GitAddonsManager_%1-(\\w{40})\\.zip").arg(GAM_BUILD_NAME));
        auto match = shaCapt.match(reply->url().toString());
        if (match.hasMatch()) {
            if (!QString(GIT_DESCRIBE).endsWith(match.captured(1).chopped(33)))
                setUpdateStatus(UpdateStatus::UpdateAvailable);
            else setUpdateStatus(UpdateStatus::NoUpdate);
        };
    });
#endif
}

void Control::downloadUpdate()
{
#ifdef GAM_SELF_UPDATE
    QFile *zip = new QFile(QApplication::applicationDirPath() + "/GitAddonsManager.zip");
    if (!zip->open(QFile::WriteOnly)) {
        setUpdateStatus(UpdateStatus::UpdateError);
        return;
    }

    QNetworkRequest req(QUrl(QString("https://gitlab.com/woblight/GitAddonsManager/-/jobs/artifacts/%1/download?job=%2").arg(GAM_BRANCH_NAME).arg(GAM_BUILD_NAME)));
    req.setAttribute(QNetworkRequest::RedirectPolicyAttribute, QNetworkRequest::NoLessSafeRedirectPolicy);
    auto reply = nam->get(req);
    setUpdateStatus(UpdateStatus::DownloadingUpdate);
    connect(reply, &QNetworkReply::readyRead,[reply, zip]() {
        zip->write(reply->readAll());
    });
    connect(reply, &QNetworkReply::finished, [this, reply, zip]() {
        zip->close();
        delete zip;
        setUpdateStatus(reply->error() == QNetworkReply::NoError ? UpdateStatus::UpdateReady : UpdateStatus::UpdateError);
        reply->deleteLater();
    });
    connect(reply, &QNetworkReply::downloadProgress, [this](qint64 recieved, qint64 bytesTotal) {
        setTotal(bytesTotal);
        setProgress(recieved);
    });
#endif
}

void Control::executeUpdate()
{
    delegate("Apply Update", [this](){
        QDir appRoot(QApplication::applicationDirPath());
        QuaZip zip(appRoot.absoluteFilePath("GitAddonsManager.zip"));
        if (!zip.open(QuaZip::mdUnzip))
            return;
        QDir root(appRoot.absoluteFilePath("update"));
        appRoot.mkdir(root.absolutePath());
        unsigned int size = 0;
        foreach (const QuaZipFileInfo &info, zip.getFileInfoList())
            size += info.uncompressedSize;
        setTotal(size);
        size = 0;
        foreach (const QuaZipFileInfo &info, zip.getFileInfoList()) {
            QuaZipFile z(zip.getZipName(), info.name);
            z.open(QuaZipFile::ReadOnly);
            QFileInfo finfo(root.absoluteFilePath(info.name.section("/",1)));
            root.mkpath(finfo.absolutePath());
            QFile f(finfo.absoluteFilePath());
            f.open(QFile::WriteOnly);
            QByteArray chunk;
            while (!(chunk = z.read(32*1024)).isEmpty()) {
                f.write(chunk);
                size += chunk.size();
                setProgress(size);
            }
            f.close();
        }
        zip.close();
        QFile::remove(appRoot.absoluteFilePath("GitAddonsManager.zip"));
        QFile exe(root.absoluteFilePath(GAM_EXEC));
        exe.setPermissions(QFile::ExeOwner | exe.permissions());
    },[this](){
        setUpdateStatus(UpdateStatus::UpdateDone);
        QApplication::exit(2);
    });
}

void Control::setUpdateStatus(Control::UpdateStatus updateStatus)
{
    if (m_updateStatus == updateStatus)
        return;

    m_updateStatus = updateStatus;
    emit updateStatusChanged(m_updateStatus);
}

void Control::setAddonsPaths(QStringList addonsPaths)
{
    if (m_addonsPaths == addonsPaths)
        return;

    m_addonsPaths = addonsPaths;
    emit addonsPathsChanged(m_addonsPaths);
}

GitException::GitException(int code) : m_code(code)
{
    const git_error *e = giterr_last();
    m_errorString = QString::asprintf("Error %d/%d: %s", code,  e->klass, e->message);
    giterr_clear();
}

int GitException::code()
{
    return m_code;
}

QString GitException::errorString() const
{
    return m_errorString;
}

void GitException::raise() const
{
    throw *this;
}

GitException *GitException::clone() const
{
    return new GitException(*this);
}

int check_git_return(int code)
{
    if (code < 0 && giterr_last() && code != GIT_ITEROVER && code != GIT_ENOTFOUND) throw GitException(code);
    else return code;
}

bool Control::useRepoDirectory() const
{
    return m_useRepoDirectory;
}

void Control::setUseRepoDirectory(bool newUseRepoDirectory)
{
    if (m_useRepoDirectory == newUseRepoDirectory)
        return;
    m_useRepoDirectory = newUseRepoDirectory;
    emit useRepoDirectoryChanged(newUseRepoDirectory);
}

void Control::exportAddonList(const QUrl &path)
{
    QFile outFile(path.toLocalFile());
    outFile.open(QFile::WriteOnly);
    QTextStream out(&outFile);
    out << Qt::endl;
    foreach (auto addon, m_addons) {
        QStringList remotes;
        foreach (auto remote, addon->remotes()) {
            if (remote == addon->remote())
                remotes << addon->getUrl(remote).toString().prepend("*");
            else
                remotes << addon->getUrl(remote).toString();
        }
        out << addon->path() << " " << addon->name() << " " << remotes.join(" ") << " " << addon->currentBranch() << Qt::endl;
    }
}

QString Control::version() const
{
    return m_version;
}

bool Control::selfUpdates() const
{
#ifdef GAM_SELF_UPDATE
    return true;
#else
    return false;
#endif
}

int Control::BusyLock::count = 0;

Control::BusyLock::BusyLock(Control *c)
{
    m_locked = c;
    m_locked->setStatus(Status::Busy);
    count++;
}

Control::BusyLock::BusyLock(BusyLock &&other)
{
    m_locked = other.m_locked;
    other.m_locked = nullptr;
}

Control::BusyLock::~BusyLock()
{
    if (m_locked)
        if (!--count && m_locked->m_tasks.empty() && m_locked->status() != Status::Error)
            m_locked->setStatus(Status::Ready);
}

void Control::log(QtMsgType type, const QMessageLogContext& context, const QString& msg)
{
    QString message = QString("<span style=\" color:%1;\">%2</span>");
    QString color;
    switch(type)
    {
        case QtDebugMsg:
            color = "silver";
            break;
        case QtInfoMsg:
            color = "black";
            break;
        case QtWarningMsg:
            color = "orange";
            break;
        case QtFatalMsg:
            color = "red";
            break;
        case QtCriticalMsg:
            color = "darkred";
            break;
    }

    emit logMessage(message.arg(color, msg));
}

