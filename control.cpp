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
#include "addon.h"
#include <git2.h>
#include <QDebug>
#include <QtConcurrent>
#include <type_traits>

Control *Control::m_instance = nullptr;

Control::Control(QObject *parent) : QObject(parent),
    m_progress(0), m_total(0), m_status(Status::Ready), m_statusMessage(""), m_pool(new QThreadPool(this)),
    m_style(QQuickStyle::name()),m_availableStyles(QQuickStyle::availableStyles())
{
    QSettings settings;
    setAddonsPath(settings.value("addonsPath").toString());
    setFirstBoot(settings.value("firstBoot",true).toBool());
    setMinimizeToTray((MinimizeToTray)settings.value("minimizeToTray",(int)MinimizeToTrayAsk).toInt());
    connect(this, &Control::addonsPathChanged, this, &Control::scanForAddons);
    connect(this, &Control::addonsPathChanged, this, &Control::saveAddonsPath);
    delegate("Initializing libgit2...", [](){git_libgit2_init();},[this](){scanForAddons();});
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
