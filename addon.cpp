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

#include "addon.h"
#include <QDir>
#include <git2.h>
#include <QDebug>
#include "control.h"
#ifdef Q_OS_WIN32
#include <windows.h>
#include <winbase.h>
#endif
#include <type_traits>
#include <QtConcurrent>
#include <QFutureWatcher>
#include <QReadWriteLock>

Addon::Addon(QString name, git_repository *repo, QObject *parent) : QObject(parent),
    m_name(name), m_repo(repo, &git_repository_free), m_progress(0), m_total(0), m_status(Status::Ready), m_gitStatus(GitStatusFlag::UpToDate), m_pool(new QThreadPool(this))
{
    connect(this, &Addon::remoteChanged, this, &Addon::fetchRemote);
    connect(this, &Addon::currentBranchChanged, this, &Addon::updateGitStatus);
    scanBranches();
}

QString Addon::name() const
{
    return m_name;
}

void Addon::setName(QString name)
{
    if (m_name == name)
        return;

    m_name = name;
    emit nameChanged(m_name);
}

void Addon::update()
{
    removeSubfolders();
    delegate("Update", [this](){
        git_reference *lbr = nullptr;
        git_reference *rbr = nullptr;
        git_reference *ubr = nullptr;
        int error = git_branch_lookup(&lbr, m_repo.get(), m_currentBranch.toLocal8Bit(), GIT_BRANCH_LOCAL);
        git_annotated_commit *ac;
        git_checkout_options opts = GIT_CHECKOUT_OPTIONS_INIT;
        opts.checkout_strategy = GIT_CHECKOUT_FORCE;
        if (error == GIT_ENOTFOUND) {
            git_branch_lookup(&lbr, m_repo.get(), m_currentBranch.toLocal8Bit(), GIT_BRANCH_REMOTE);
            git_annotated_commit_from_ref(&ac, m_repo.get(), lbr);
        } else {
            git_branch_upstream(&rbr, lbr);
            git_annotated_commit_from_ref(&ac, m_repo.get(), rbr);
        }
        if (!git_branch_is_head(lbr))
            git_repository_set_head(m_repo.get(), git_reference_name(lbr));
        git_reference_set_target(&ubr, lbr, git_annotated_commit_id(ac), nullptr);
        git_checkout_head(m_repo.get(), &opts);

        git_annotated_commit_free(ac);
        if (rbr)
            git_reference_free(rbr);
        git_reference_free(lbr);
        git_reference_free(ubr);
    }, [this](){updateGitStatus();});
    unpackSubfolders();
}

void Addon::scanBranches()
{
    delegate("Branch Scan", [this](){
        struct {
            QStringList rs;
            QStringList bs;
            QString cb;
            QString cr;
        } data;
        git_strarray remotes;
        int error = git_remote_list(&remotes, m_repo.get());
        for (unsigned int i = 0; i < remotes.count; i++)
            data.rs << remotes.strings[i];
        git_strarray_free(&remotes);

        git_branch_iterator *iter = nullptr;
        error = git_branch_iterator_new(&iter, m_repo.get(), GIT_BRANCH_ALL);

        git_reference *ref = nullptr;
        const char *bname = nullptr;
        git_branch_t btype;
        while (!(error = git_branch_next(&ref, &btype, iter))) {
            git_branch_name(&bname, ref);
            data.bs << bname;
            if (git_branch_is_head(ref)) {
                data.cb = bname;
                git_buf buf = GIT_BUF_INIT_CONST(0, nullptr);
                (git_reference_is_remote(ref) ?
                            git_branch_remote_name :
                            git_branch_upstream_remote
                            )(&buf, m_repo.get(), git_reference_name(ref));
                data.cr = buf.ptr;
            }
            git_reference_free(ref);
        }
        git_branch_iterator_free(iter);
        return data;
    },[this](auto data){
        setRemote(data.cr);
        setCurrentBranch(data.cb);
        setRemotes(data.rs);
        setBranches(data.bs);
        scanSubfolders();
    });
}

void Addon::scanSubfolders()
{
    delegate("Subfolders Scan", [this](){
        QStringList sf;
        git_object *obj = nullptr;
        git_revparse_single(&obj, m_repo.get(), "HEAD^{tree}");
        git_tree *tree = reinterpret_cast<git_tree *>(obj);
        git_tree_walk(tree, GIT_TREEWALK_PRE, [](const char *root, const git_tree_entry *entry, void *payload) -> int{
            QString path = root;
            if (path.isEmpty())
                return 0;
            QStringList *sf = static_cast<QStringList *>(payload);
            QString file = git_tree_entry_name(entry);
            if (file.toLower() == path.toLower().chopped(1) + ".toc") {
                *sf << path.chopped(1);
                return 1;
            }
            if (path.split("/").size() > 1)
                return 1;
            return 0;
        }, &sf);
        git_object_free(obj);
        return sf;
    },[this](auto subs){setSubfolders(subs);});
}

void Addon::setBranches(QStringList branches)
{
    if (m_branches == branches)
        return;

    m_branches = branches;
    emit branchesChanged(m_branches);
}

void Addon::setCurrentBranch(QString currentBranch)
{
    if (m_currentBranch == currentBranch)
        return;

    m_currentBranch = currentBranch;
    emit currentBranchChanged(m_currentBranch);
}

void Addon::setRemote(QString remote)
{
    if (m_remote == remote)
        return;

    m_remote = remote;
    emit remoteChanged(m_remote);
}

void Addon::setRemotes(QStringList remotes)
{
    if (m_remotes == remotes)
        return;

    m_remotes = remotes;
    emit remotesChanged(m_remotes);
}

int fetch_progress_cb(const git_transfer_progress *stats, void *payload) {
    Addon *a = static_cast<Addon *>(payload);
    QMetaObject::invokeMethod(a,"setTotal",Q_ARG(int, stats->total_objects));
    QMetaObject::invokeMethod(a,"setProgress",Q_ARG(int, stats->received_objects));
    return 0;
}
void Addon::fetchRemote(QString remote)
{
    delegate(remote + " Fetch", [this, remote](){
        git_fetch_options fetch_opts = GIT_FETCH_OPTIONS_INIT;
        fetch_opts.callbacks.transfer_progress = &fetch_progress_cb;
        fetch_opts.callbacks.payload = this;
        git_remote *r;
        git_remote_lookup(&r, m_repo.get(), remote.toLocal8Bit());
        int error = git_remote_fetch(r,nullptr,&fetch_opts,nullptr);
        if (error < 0) {
            const git_error *e = giterr_last();
            qDebug() <<QString::asprintf("Error %d/%d: %s\n", error, e->klass, e->message);
        }
        git_remote_free(r);
    }, [this](){updateGitStatus();});

}

void Addon::setProgress(int fetchProgress)
{
    if (m_progress == fetchProgress)
        return;

    m_progress = fetchProgress;
    emit progressChanged(m_progress);
}

void Addon::setTotal(int fetchSize)
{
    if (m_total == fetchSize)
        return;

    m_total = fetchSize;
    emit totalChanged(m_total);
}

void Addon::setStatus(Addon::Status status)
{
    if (m_status == status)
        return;

    m_status = status;
    emit statusChanged(m_status);
}

void Addon::setGitStatus(Addon::GitStatus gitStatus)
{
    if (m_gitStatus == gitStatus)
        return;

    m_gitStatus = gitStatus;
    emit gitStatusChanged(m_gitStatus);
}

void Addon::updateGitStatus()
{
    delegate("Git Status Update", [this](){
        git_reference *tr;
        git_branch_lookup(&tr, m_repo.get(), m_currentBranch.toLocal8Bit(), GIT_BRANCH_LOCAL);
        if (!tr) {
            git_branch_lookup(&tr, m_repo.get(), m_currentBranch.toLocal8Bit(), GIT_BRANCH_REMOTE);
        }
        else {
            git_reference *rbr;
            git_branch_upstream(&rbr, tr);
            git_reference_free(tr);
            tr = rbr;
        }
        if (!tr)
            return GitStatusFlag::UpToDate;
        git_reference *lr;
        git_repository_head(&lr, m_repo.get());
        git_annotated_commit *uc;
        git_annotated_commit *lc;
        git_annotated_commit_from_ref(&uc, m_repo.get(), tr);
        git_annotated_commit_from_ref(&lc, m_repo.get(), lr);
        size_t ahead;
        size_t behind;
        git_graph_ahead_behind(&ahead, &behind, m_repo.get(), git_annotated_commit_id(lc), git_annotated_commit_id(uc));
        if (!(ahead || behind))
            return GitStatusFlag::UpToDate;
        else if (ahead && !behind)
            return GitStatusFlag::Ahead;
        else if (!ahead)
            return GitStatusFlag::Behind;
        else
            return GitStatusFlag::Diverged;


        git_reference_free(tr);
        git_reference_free(lr);
        git_annotated_commit_free(uc);
        git_annotated_commit_free(lc);
        /// TODO: check for local changes/merge
        /*if (git_repository_head_detached(m_repo.get())) {
            setGitStatus(GitStatus::Conflicting);
            return;
        }
        git_reference *tr;
        git_branch_lookup(&tr, m_repo.get(), m_currentBranch.toLocal8Bit(), GIT_BRANCH_LOCAL);
        if (!tr) {
            git_branch_lookup(&tr, m_repo.get(), m_currentBranch.toLocal8Bit(), GIT_BRANCH_REMOTE);
        }
        else {
            git_reference *rbr;
            git_branch_upstream(&rbr, tr);
            git_reference_free(tr);
            tr = rbr;
        }
        git_buf buf = GIT_BUF_INIT_CONST(0, nullptr);
        git_branch_remote_name(&buf, m_repo.get(), git_reference_name(tr));
        if (buf.ptr != remote())
            setRemote(buf.ptr);
        else {
            git_annotated_commit *ac;
            git_annotated_commit_from_ref(&ac, m_repo.get(), tr);
            git_merge_analysis_t an;
            git_merge_preference_t pref;
            git_merge_analysis(&an, &pref, m_repo.get(),(const git_annotated_commit **)&ac, 1);
            if (an & GIT_MERGE_ANALYSIS_UP_TO_DATE) {
                if (git_annotated_commit_id(*ac) != git_head)
                    setGitStatus(GitStatus::Conflicting);
                else
                    setGitStatus(GitStatus::UpToDate);
            }
            else if (an & GIT_MERGE_ANALYSIS_FASTFORWARD)
                setGitStatus(GitStatus::FastForwardable);
            else
                setGitStatus(GitStatus::Conflicting);

            git_annotated_commit_free(ac);
        }
        git_reference_free(tr);*/

    }, [this](auto s){
        setGitStatus(s);
    });
}

void Addon::uninstall()
{
    removeSubfolders();
    closeRepo();
    delegate("Removing addon folder", [this](){
        removeFolder(Control::instance()->addonsPath() + "/" + m_name);
    }, [](){Control::instance()->scanForAddons();});
}

void Addon::setSubfolders(QStringList subfolders)
{
    if (m_subfolders == subfolders)
        return;
    m_subfolders = subfolders;
    emit subfoldersChanged(m_subfolders);
}

void walkFolders(QFileInfo &info, auto f){
    if (info.isDir() && !info.isSymLink()) {
        foreach (QFileInfo sub, QDir(info.absoluteFilePath()).entryInfoList(QDir::AllEntries|QDir::NoDotAndDotDot|QDir::Hidden))
            walkFolders(sub, f);
    }
    f(info);
}

void Addon::removeFolder(QString path, bool ask) {
    QFileInfo info(path);
    QStringList files;
    bool ok = ask;
    if (info.exists()) {
        walkFolders(info, [&files](QFileInfo info){
            files << info.absoluteFilePath();
        });
        if (ask) {
            m_mutex.lock();
            m_result = &ok;
            QMetaObject::invokeMethod(this, "setFilesToRemove", Q_ARG(QString, files.join('\n')));
            m_wait.wait(&m_mutex);
            m_mutex.unlock();
        }
        if (ok) {
            setTotal(files.size());
            for (int i = 0; i < files.size(); i++) {
                QFileInfo info(files[i]);
                if (!info.isSymLink() && info.isDir())
                    info.dir().rmdir(info.fileName());
                else
                    QFile(info.absoluteFilePath()).remove();
                setProgress(i);
            }
        }
    }
}

void Addon::removeSubfolders()
{
    delegate("Removing Subfolders", [this](){
        foreach (QString subf, m_subfolders)
            removeFolder(Control::instance()->addonsPath() + "/" + subf);
    });
}

void Addon::unpackSubfolders(){
    delegate("Unpacking Subfolders", [this](){
        QStringList errors;
        QDir addonsDir(Control::instance()->addonsPath());
        if (m_subfolders.contains(m_name, Qt::CaseInsensitive)) {
            closeRepo();
            QString oldName = m_name;
            setName(m_name + ".repo");
            addonsDir.rename(oldName, m_name);
            openRepo();
        }
        QDir::setCurrent(addonsDir.canonicalPath());
        QDir dir(m_name);
        foreach (QString subfn, m_subfolders) {
            QFileInfo addonFolder(subfn);
            if (addonFolder.exists()) {
                QFileInfo dest(addonFolder.fileName() + ".bak");
                int i = 0;
                while (dest.exists())
                    dest = QFileInfo(addonFolder.fileName() + ".bak." + QString::number(++i));
                addonsDir.rename(addonFolder.fileName(), dest.fileName());
                if (QFileInfo(subfn).exists()) {
                    errors << QString("The folder \"%1\" already existed and couldn't be renamed. The addon subfolder has been skipped").arg(subfn, dest.exists());
                    continue;
                }
                errors << QString("The folder \"%1\" already existed and has been renamed to \"%2\"").arg(subfn, dest.fileName());
            }
    #ifdef Q_OS_WIN32
            QString link = subfn;
            link.replace("/","\\");
            QString target = "./"+m_name+"/"+subfn;
            target.replace("/","\\");
            wchar_t link_w[link.size() +1];
            wchar_t target_w[target.size() +1];
            int pos = link.toWCharArray(link_w);
            link_w[pos] = '\0';
            pos = target.toWCharArray(target_w);
            target_w[pos] = '\0';
    #else
            QFile::link("./"+m_name+"/"+subfn, subfn);
    #endif
            if (!QFileInfo(subfn).exists()) {
                dir.rename(subfn, "../"+subfn);
            }
            QFileInfo info(subfn);
            if (!info.exists()) {
                errors << "Failed to link or move \""+ info.absoluteFilePath() + "\" to \"" + addonFolder.absoluteFilePath()+"\"";
            }
        }
        return errors;
    }, [this](auto errors){
         if (errors.size() > 0) {
             setStatus(Status::Error);
             //setStatusMessage(errors.join('\n'));
         }
    });
}

void Addon::setFilesToRemove(QString filesToRemove)
{
    if (m_filesToRemove == filesToRemove)
        return;

    m_filesToRemove = filesToRemove;
    emit filesToRemoveChanged(m_filesToRemove);
}

void Addon::confirmFileRemove(bool confirmed)
{
    m_mutex.lock();
    *static_cast<bool *>(m_result) = confirmed;
    m_wait.wakeOne();
    m_mutex.unlock();
}

QStringList Addon::branches() const
{
    return m_branches;
}

QString Addon::currentBranch() const
{
    return m_currentBranch;
}

QString Addon::remote() const
{
    return m_remote;
}

QStringList Addon::remotes() const
{
    return m_remotes;
}

int Addon::progress() const
{
    return m_progress;
}

int Addon::total() const
{
    return m_total;
}

Addon::Status Addon::status() const
{
    return m_status;
}

Addon::GitStatus Addon::gitStatus() const
{
    return m_gitStatus;
}

QStringList Addon::subfolders() const
{
    return m_subfolders;
}

QString Addon::filesToRemove() const
{
    return m_filesToRemove;
}

void Addon::closeRepo()
{
    m_repo.reset();
}

void Addon::openRepo()
{
    git_repository *repo = nullptr;
    git_repository_open(&repo, (Control::instance()->addonsPath() + "/" + m_name).toLocal8Bit());
    m_repo.reset(repo);
}

void Addon::delegate(QString taskname, auto work, auto callback)
{
    Q_ASSERT_X(QThread::currentThread() == thread(), "delegate", "Attempt to delegate from another thread.");
    if (status() != Status::Ready) {
        qInfo() << name() << "Enqueueing" << taskname;
        m_tasks.enqueue({taskname, [this, taskname, work, callback](){delegate(taskname, work,callback);}});
        return;
    }
    qInfo() << name() << "Executing " << taskname;
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

void Addon::delegate(QString taskname, auto work)
{
    using ret_t = typename std::invoke_result<decltype(work)>::type;
    if constexpr (std::is_same<ret_t, void>::value)
        delegate(taskname, work, [](){});
    else
        delegate(taskname, work, [](ret_t){});
}
