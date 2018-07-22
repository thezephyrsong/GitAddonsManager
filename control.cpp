/* Copyright 2018 WobLight
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

Control *Control::m_instance = nullptr;
QNetworkAccessManager *nam = nullptr;

Control::Control(QObject *parent) : QObject(parent),
    m_progress(0), m_total(0), m_status(Status::Ready), m_statusMessage(""), m_pool(new QThreadPool(this)),
    m_style(QQuickStyle::name()),m_availableStyles(QQuickStyle::availableStyles())
{
    connect(this, &Control::addonsPathChanged, this, &Control::scanForAddons);
    connect(this, &Control::addonsPathChanged, this, &Control::saveAddonsPath);
}

void Control::init() {
    QSettings settings;
    setAddonsPath(settings.value("addonsPath").toString());
    setFirstBoot(settings.value("firstBoot",true).toBool());
    setMinimizeToTray((MinimizeToTray)settings.value("minimizeToTray",(int)MinimizeToTrayAsk).toInt());
    git_libgit2_init();
    scanForAddons();
    nam = new QNetworkAccessManager(this);
    checkForUpdates();
}

void Control::delegate(QString taskname, auto work, auto callback)
{
    Q_ASSERT_X(QThread::currentThread() == thread(), "delegate", "Attempt to delegate from another thread.");
    if (status() != Status::Ready) {
        qInfo() << "Enqueueing" << taskname;
        m_tasks.enqueue({taskname, [this, taskname, work, callback](){delegate(taskname, work,callback);}});
        return;
    }
    qInfo() << "Executing " << taskname;
    setStatusMessage(taskname);
    setProgress(0);
    setTotal(0);
    setStatus(Status::Busy);
    using ret_t = typename std::invoke_result<decltype(work)>::type;
    QFuture<ret_t> fut = QtConcurrent::run(m_pool, work);
    QFutureWatcher<ret_t> *fw = new QFutureWatcher<ret_t>();
    connect(fw, &QFutureWatcher<ret_t>::finished, [this, callback, fw](){
        setStatus(Status::Ready);
        TaskQueue old;
        m_tasks.swap(old);

        if constexpr (std::is_same<ret_t, void>::value)
            callback();
        else
            callback(fw->result());

        m_tasks.append(old);

        if (!m_tasks.isEmpty() && status() != Status::Busy)
            m_tasks.dequeue().second();
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

QList<QObject *> Control::addons() const
{
    return m_addons;
}

QString Control::addonsPath() const
{
    return m_addonsPath;
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
        walkFoldersIf({root.absolutePath()},[&newRoot, &files, &root, &oldFiles](const QFileInfo &info){
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

void Control::setAddons(QList<QObject *> addons)
{
    if (m_addons == addons)
        return;

    m_addons = addons;
    emit addonsChanged(m_addons);
}

void Control::setAddonsPath(QString addonsPath)
{
    addonsPath = QUrl::fromLocalFile(addonsPath).adjusted(QUrl::NormalizePathSegments|QUrl::StripTrailingSlash).toLocalFile();
    if (m_addonsPath == addonsPath)
        return;

    m_addonsPath = addonsPath;
    emit addonsPathChanged(m_addonsPath);
}

void Control::saveAddonsPath()
{
    QSettings settings;
    settings.setValue("addonsPath", m_addonsPath);
}

void Control::scanForAddons()
{
    using init_t = QPair<QString, git_repository*>;
    delegate("Scanning addons folder...", [this](){
        QList<init_t> list;
        QDir addonsFolder(m_addonsPath);
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
    }, [this](auto list){
        QObjectList ol;
        foreach (init_t i, list)
            ol << new Addon(i.first, i.second);
        setAddons(ol);
    });
}

int clone_progress_cb(const git_transfer_progress *stats, void *payload) {
    Q_UNUSED(payload)
    Control::instance()->setTotal(stats->total_objects);
    Control::instance()->setProgress(stats->received_objects);
    return 0;
}

void Control::clone(QUrl url)
{
    delegate("Cloning addon...",[this, url]() {
        struct {
            QString name;
            git_repository *repo = nullptr;
            QString error;
        } data;
        data.name = url.fileName().section('.',0,0);
        git_clone_options opts = GIT_CLONE_OPTIONS_INIT;
        opts.fetch_opts.callbacks.transfer_progress = &clone_progress_cb;
        int error = git_clone(&data.repo, url.toString().toLocal8Bit(), (m_addonsPath + "/" + data.name).toLocal8Bit(), &opts);
        if (error < 0) {
            const git_error *e = giterr_last();
            data.error = QString::asprintf("Error %d/%d: %s", error,  e->klass, e->message);
            return data;
        }

        QDir dir(m_addonsPath);
        dir.cd(data.name);
        QStringList toc = dir.entryList({"*.toc"});
        if (toc.isEmpty()) {
        } else if (toc[0].toLower() != dir.dirName().toLower() + ".toc") {
            git_repository_free(data.repo);
            dir.cdUp();
            QString newName = toc[0].chopped(4);
            dir.rename(data.name, newName);
            data.name = newName;
            dir.cd(data.name);
            git_repository_open(&data.repo, dir.canonicalPath().toLocal8Bit());
        }
        return data;
    },[this](auto ret){
        if (ret.error.isEmpty()) {
            auto add = new Addon(ret.name, ret.repo);
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
#ifdef GAM_BUILD_NAME
    QNetworkRequest req(QUrl(QString("https://gitlab.com/woblight/GitAddonsManager/-/jobs/artifacts/master/download?job=%1").arg(GAM_BUILD_NAME)));
    req.setAttribute(QNetworkRequest::FollowRedirectsAttribute, true);
    connect(nam, &QNetworkAccessManager::finished, [](auto reply){
        QApplication::setApplicationDisplayName(reply->errorString());
    });
    auto reply = nam->head(req);
    setUpdateStatus(UpdateStatus::CheckingForUpdate);
    connect(reply, &QNetworkReply::finished,[this, reply](){
        QRegExp shaCapt(QString("GitAddonsManager_%1-(\\w{40})\\.zip").arg(GAM_BUILD_NAME));
        if (shaCapt.indexIn(reply->url().toString()) != -1) {
            if (!QString(GIT_DESCRIBE).endsWith(shaCapt.cap(1).chopped(33)))
                setUpdateStatus(UpdateStatus::UpdateAvailable);
            else setUpdateStatus(UpdateStatus::NoUpdate);
        };
    });
#endif
}

void Control::downloadUpdate()
{
#ifdef GAM_BUILD_NAME
    QFile *zip = new QFile(QApplication::applicationDirPath() + "/GitAddonsManager.zip");
    if (!zip->open(QFile::WriteOnly)) {
        setUpdateStatus(UpdateStatus::UpdateError);
        return;
    }

    QNetworkRequest req(QUrl(QString("https://gitlab.com/woblight/GitAddonsManager/-/jobs/artifacts/master/download?job=%1").arg(GAM_BUILD_NAME)));
    req.setAttribute(QNetworkRequest::FollowRedirectsAttribute, true);
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
        QuaZip zip("GitAddonsManager.zip");
        if (!zip.open(QuaZip::mdUnzip))
            return;
        QDir root(QApplication::applicationDirPath() + "/update");
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
        QFile::remove("GitAddonsManager.zip");
        QFile exe(root.absoluteFilePath("GitAddonsManager"));
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
