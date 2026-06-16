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

Addon::Addon(QString name, git_repository *repo, QString path, QObject *parent) : QObject(parent),
    m_name(name), m_repo(repo, &git_repository_free), m_progress(0), m_total(0), m_status(Status::Ready), m_gitStatus(GitStatusFlag::UpToDate), m_pool(new QThreadPool(this)), m_path(path)
{
    connect(this, &Addon::remoteChanged, this, &Addon::fetchRemote);
    connect(this, &Addon::currentBranchChanged, this, &Addon::updateGitStatus);
    scanBranches();
    loadReadme();
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
        auto lbr = branchRef(m_currentBranch.toLocal8Bit());
        auto tr = remoteRefForBranch(m_currentBranch.toLocal8Bit());
        AutoPtr ubr(&git_reference_free);

        if (git_reference_is_remote(lbr))
        {
            git_buf buf = GIT_BUF_INIT_CONST(0, nullptr);
            AutoPtr<git_buf> remote(&buf, git_buf_free);
            check_git_return(git_branch_remote_name(remote, m_repo.get(), git_reference_name(lbr)));
            git_reference* nlbr = nullptr;
            git_commit* commit = nullptr;
            AutoPtr<git_commit> commit_g(commit, git_commit_free);
            check_git_return(git_commit_lookup(&commit, m_repo.get(), git_reference_target(tr)));
            QString newName;
            if (strcmp(buf.ptr, "origin") == 0)
                newName = m_currentBranch.sliced(7);
            else
                newName = QString(m_currentBranch).replace(strlen(buf.ptr), 1, '_');
            if (git_branch_create(&nlbr, m_repo.get(), newName.toLocal8Bit(), commit, 1) != GIT_OK)
                git_branch_lookup(&nlbr, m_repo.get(), newName.toLocal8Bit(), GIT_BRANCH_LOCAL);
            check_git_return(git_branch_set_upstream(nlbr, git_reference_name(lbr)));
            lbr.reset(nlbr);
        }

        if (!git_branch_is_head(lbr))
            check_git_return(git_repository_set_head(m_repo.get(), git_reference_name(lbr)));

        check_git_return(git_reference_set_target(&ubr, lbr, git_reference_target(!tr?lbr:tr), nullptr));

        git_checkout_options opts = GIT_CHECKOUT_OPTIONS_INIT;
        opts.checkout_strategy = GIT_CHECKOUT_FORCE;
        check_git_return(git_checkout_head(m_repo.get(), &opts));
    }, [this](){scanBranches(); updateGitStatus();});
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
        check_git_return(git_remote_list(&remotes, m_repo.get()));
        for (unsigned int i = 0; i < remotes.count; i++)
            data.rs << remotes.strings[i];
        git_strarray_free(&remotes);

        AutoPtr iter(&git_branch_iterator_free);
        check_git_return(git_branch_iterator_new(&iter, m_repo.get(), GIT_BRANCH_ALL));

        AutoPtr ref(&git_reference_free);
        const char *bname = nullptr;
        git_branch_t btype;
        while (!check_git_return(git_branch_next(&ref, &btype, iter))) {
            git_branch_name(&bname, ref);
            data.bs << bname;
            if (git_branch_is_head(ref)) {
                data.cb = bname;
                git_buf buf = GIT_BUF_INIT_CONST(0, nullptr);
                AutoPtr<git_buf> buf_g(&buf, git_buf_free);
                (git_reference_is_remote(ref) ?
                    git_branch_remote_name :
                    git_branch_upstream_remote
                )(&buf, m_repo.get(), git_reference_name(ref));
                data.cr = buf.ptr;
            }
            ref.reset();
        }
        if (data.cb.isEmpty()) {
            AutoPtr reflog(&git_reflog_free);
            if (!git_reflog_read(&reflog, m_repo.get(), "HEAD")) {
                for (size_t i = 0; i < git_reflog_entrycount(reflog); i++) {
                    auto entry = git_reflog_entry_byindex(reflog, i);
                    QRegularExpression rex("checkout: moving from \\S+ to (.*)");
                    auto match = rex.match(git_reflog_entry_message(entry));
                    if (match.hasMatch()) {
                        auto f = &git_branch_upstream_remote;
                        git_branch_lookup(&ref, m_repo.get(), match.captured(1).toLocal8Bit(), GIT_BRANCH_LOCAL);
                        if (!ref) {
                            f = &git_branch_remote_name;
                            git_branch_lookup(&ref, m_repo.get(), match.captured(1).toLocal8Bit(), GIT_BRANCH_REMOTE);
                        }
                        if (!ref) break;
                        git_buf buf = GIT_BUF_INIT_CONST(0, nullptr);
                        AutoPtr<git_buf> buf_g(&buf, git_buf_free);
                        if (!f(&buf, m_repo.get(), git_reference_name(ref))) {
                            data.cr = buf.ptr;
                            data.cb = match.captured(1);
                        }
                        ref.reset();
                        break;
                    }
                }
            }
        }
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
        AutoPtr obj(git_object_free);
        git_revparse_single(&obj, m_repo.get(), "HEAD^{tree}");
        git_tree *tree = reinterpret_cast<git_tree *>(obj.pointer);
        if (!tree)
            return sf;
        git_tree_walk(tree, GIT_TREEWALK_PRE, [](const char *root, const git_tree_entry *entry, void *payload) -> int{
            QString path = root;
            if (path.isEmpty())
                return 0;
            path = path.chopped(1);
            QStringList *sf = static_cast<QStringList *>(payload);
            QString file = git_tree_entry_name(entry);

            if (!sf->contains(path) && !file.compare(path + ".toc", Qt::CaseInsensitive)) {
                *sf << path;
                return 1;
            }

            Control::removeTocSuffixes(file);

            if (!sf->contains(path) && !file.compare(path + ".toc", Qt::CaseInsensitive)) {
                *sf << path;
                return 1;
            }

            if (path.split("/").size() > 1)
                return 1;
            return 0;
        }, &sf);
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
        AutoPtr r(git_remote_free);
        check_git_return(git_remote_lookup(&r, m_repo.get(), remote.toLocal8Bit()));
        check_git_return(git_remote_fetch(r,nullptr,&fetch_opts,nullptr));
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
        auto tr = remoteRefForBranch(m_currentBranch);
        if (!tr)
            return GitStatusFlag::UpToDate;
        AutoPtr lr(&git_reference_free);
        check_git_return(git_repository_head(&lr, m_repo.get()));

        const char* name = nullptr;

        if (git_reference_is_remote(lr) || git_branch_name(&name, lr) == GIT_OK && m_currentBranch != name)
            return GitStatusFlag::Diverged;

        AutoPtr uc(&git_annotated_commit_free);
        AutoPtr lc(&git_annotated_commit_free);
        check_git_return(git_annotated_commit_from_ref(&uc, m_repo.get(), tr));
        check_git_return(git_annotated_commit_from_ref(&lc, m_repo.get(), lr));
        if (!lc)
            return GitStatusFlag::Error;
        size_t ahead = 0;
        size_t behind = 0;
        git_graph_ahead_behind(&ahead, &behind, m_repo.get(), git_annotated_commit_id(lc), git_annotated_commit_id(uc));

        if (!(ahead || behind))
            return GitStatusFlag::UpToDate;
        else if (ahead && !behind)
            return GitStatusFlag::Ahead;
        else if (!ahead)
            return GitStatusFlag::Behind;
        else
            return GitStatusFlag::Diverged;

        /// TODO: check for local changes/merge
        /*if (git_repository_head_detached(m_repo.get())) {
            setGitStatus(GitStatus::Conflicting);
            return;
        }
        AutoPtr tr(&git_reference_free);
        git_branch_lookup(&tr, m_repo.get(), m_currentBranch.toLocal8Bit(), GIT_BRANCH_LOCAL);
        if (!tr) {
            git_branch_lookup(&tr, m_repo.get(), m_currentBranch.toLocal8Bit(), GIT_BRANCH_REMOTE);
        }
        else {
            AutoPtr rbr(&git_reference_free);
            git_branch_upstream(&rbr, tr);
            git_reference_free(tr);
            tr = rbr;
        }
        git_buf buf = GIT_BUF_INIT_CONST(0, nullptr);
        git_branch_remote_name(&buf, m_repo.get(), git_reference_name(tr));
        if (buf.ptr != remote())
            setRemote(buf.ptr);
        else {
            AutoPtr ac(&git_annotated_commit_free);
            git_annotated_commit_from_ref(&ac, m_repo.get(), tr);
            git_merge_analysis_t an;
            git_merge_preference_t pref;
            git_merge_analysis(&an, &pref, m_repo.get(),(const AutoPtr *)&ac, 1)(&git_annotated_commit_free);
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
            removeFolder(m_path + "/" + m_name);
        }, [](){Control::instance()->scanForAddons();});
}

void Addon::setSubfolders(QStringList subfolders)
{
    if (m_subfolders == subfolders)
        return;
    m_subfolders = subfolders;
    emit subfoldersChanged(m_subfolders);
}

void walkFolders(const QFileInfo &info, auto f){
    if (info.isDir() && !info.isSymLink()) {
        foreach (QFileInfo sub, QDir(info.absoluteFilePath()).entryInfoList(QDir::AllEntries|QDir::NoDotAndDotDot|QDir::Hidden))
            walkFolders(sub, f);
    }
    f(info);
}


bool Addon::removeFolders(QStringList paths, bool ask) {
    QStringList files;
    bool ok = ask;
    foreach (QString path, paths){
        QFileInfo info(path);
        if (info.exists()) {
            walkFolders(info, [&files](QFileInfo info){
                files << info.absoluteFilePath();
            });
        }
    }
    if (files.isEmpty()) return true;
    if (ask) {
        m_mutex.lock();
        m_result = &ok;
        QMetaObject::invokeMethod(this, "setFilesToRemove", Q_ARG(QString, files.join('\n')));
        m_wait.wait(&m_mutex);
        m_mutex.unlock();
        m_filesToRemove.clear();
    }
    if (ok) {
        setTotal(files.size());
        for (int i = 0; i < files.size(); i++) {
            QFileInfo info(files[i]);
            if (!info.isSymLink() && info.isDir())
                info.dir().rmdir(info.fileName());
            else {
                QFile f(info.absoluteFilePath());
                f.setPermissions(f.permissions()|QFile::WriteOther);
                f.remove();
            }
            setProgress(i);
        }
        return true;
    }
    return false;
}

bool Addon::removeFolder(QString path, bool ask) {
    return removeFolders({path}, ask);
}

void Addon::removeSubfolders()
{
    delegate("Removing Subfolders", [this](){
        QStringList paths(m_subfolders);
        std::transform(paths.begin(), paths.end(), paths.begin(), [this](auto p){return m_path + "/" + p;});
        removeFolders(paths);
    });
}

void Addon::unpackSubfolders(){
    delegate("Unpacking Subfolders", [this](){
        QStringList errors;
        QDir addonsDir(m_path);
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
            std::vector<wchar_t> link_buf(link.size() + 1);
            std::vector<wchar_t> target_buf(target.size() + 1);
            wchar_t* link_w = link_buf.data();
            wchar_t* target_w = target_buf.data();
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

void Addon::reset()
{
    removeSubfolders();
    delegate("Reset", [this](){
        AutoPtr ref(&git_reference_free);
        git_repository_head(&ref, m_repo.get());
        git_object *obj = nullptr;
        git_object_lookup(&obj, m_repo.get(), git_reference_target(ref), GIT_OBJ_ANY);
        git_reset(m_repo.get(), obj, GIT_RESET_HARD, nullptr);
    });
    unpackSubfolders();
}

void Addon::reclone()
{
    delegate("Reclone", [this](){
        git_strarray remotes;
        AutoPtr rr(&remotes, git_strarray_free);
        check_git_return(git_remote_list(rr, m_repo.get()));
        QStringList urls;
        for (size_t i = 0; i < remotes.count; i++) {
            AutoPtr remote(&git_remote_free);
            check_git_return(git_remote_lookup(&remote, m_repo.get(), remotes.strings[i]));
            urls << git_remote_url(remote);
        }
        const char *remote = nullptr;
        check_git_return(git_branch_name(&remote, remoteRefForBranch(m_currentBranch)));
        QString oldRemote(remote);
        closeRepo();
        if (removeFolder(m_path + "/" + name() + "/.git")){
            git_repository *repo = nullptr;
            check_git_return(git_repository_init(&repo, (m_path + "/" + m_name).toLocal8Bit(), false));
            m_repo.reset(repo);
            for (size_t i = 0; i < remotes.count; i++) {
                AutoPtr remote(&git_remote_free);
                check_git_return(git_remote_create(&remote, m_repo.get(), remotes.strings[i], urls[i].toLocal8Bit()));
                git_fetch_options fetch_opts = GIT_FETCH_OPTIONS_INIT;
                fetch_opts.callbacks.transfer_progress = &fetch_progress_cb;
                fetch_opts.callbacks.payload = this;
                AutoPtr r(git_remote_free);
                check_git_return(git_remote_lookup(&r, m_repo.get(),  git_remote_name(remote)));
                check_git_return(git_remote_fetch(r,nullptr,&fetch_opts,nullptr));
            }
            setCurrentBranch(oldRemote);
            return true;
        }
        return false;
    },[this](bool ok){
        if (ok) {
            update();
            scanBranches();
        }
    }, true);
}

void Addon::loadReadme()
{
    delegate("Readme Loading",[this](){
        QFileInfo md(m_path + "/" + m_name + "/README.md");
        if (!md.exists() && !md.isFile()) {
            QDir repodir(m_path + "/" + m_name);
            auto a = repodir.entryInfoList({"readme*"});
            if (a.size() > 0)
                md = a[0];
        }
        if (md.exists() && md.isFile()) {
            QFile file(md.absoluteFilePath());
            if (file.open(QFile::ReadOnly))
                return QString(file.readAll());
        }
        return QString("");
    }, [this](auto readme){
        setReadme(readme);
    });
}

void Addon::setReadme(QString readme)
{
    if (m_readme == readme)
        return;

    m_readme = readme;
    emit readmeChanged(m_readme);
}

QFuture<void> Addon::updateIfBehind()
{
    std::shared_ptr<QPromise<void>> p(new QPromise<void>());
    delegate("UpdateIfBehind", [this](){
    },[this, p](){
        if (m_gitStatus == Addon::GitStatusFlag::Behind)
            update();
        delegate("Finished", [p](){
            p->finish();
        });
    });
    return p->future();
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

QString Addon::readme() const
{
    return m_readme;
}

QString Addon::path() const
{
    return m_path;
}

QUrl Addon::getUrl(QString name) const
{
    git_remote *remote = nullptr;
    if (name.isNull())
        name = m_remote;
    if (git_remote_lookup(&remote, m_repo.get(), name.toLocal8Bit()))
        return QUrl();
    auto url = QUrl(git_remote_url(remote));
    git_remote_free(remote);
    return url;
}

void Addon::closeRepo()
{
    m_repo.reset();
}

void Addon::openRepo()
{
    git_repository *repo = nullptr;
    git_repository_open(&repo, (m_path + "/" + m_name).toLocal8Bit());
    m_repo.reset(repo);
}

AutoPtr<git_reference> Addon::remoteRefForBranch(QString branch)
{
    AutoPtr lbr(&git_reference_free);
    AutoPtr tr(&git_reference_free);
    int error = check_git_return(git_branch_lookup(&lbr, m_repo.get(), branch.toLocal8Bit(), GIT_BRANCH_LOCAL));
    if (error == GIT_ENOTFOUND)
        check_git_return(git_branch_lookup(&lbr, m_repo.get(), branch.toLocal8Bit(), GIT_BRANCH_REMOTE));
    else
        check_git_return(git_branch_upstream(&tr, lbr));
    if (!tr)
        return lbr;
    else
        return tr;
}

AutoPtr<git_reference> Addon::branchRef(QString name)
{
    AutoPtr lbr(&git_reference_free);
    int error = check_git_return(git_branch_lookup(&lbr, m_repo.get(), name.toLocal8Bit(), GIT_BRANCH_LOCAL));
    if (error == GIT_ENOTFOUND)
        check_git_return(git_branch_lookup(&lbr, m_repo.get(), name.toLocal8Bit(), GIT_BRANCH_REMOTE));
    return lbr;
}

void Addon::delegate(QString taskname, auto work, auto callback, bool force)
{
    Q_ASSERT_X(QThread::currentThread() == thread(), "delegate", "Attempt to delegate from another thread.");
    if (m_lock && !force) {
        qInfo() << name() << "Enqueueing" << taskname;
        m_tasks.enqueue({taskname, [this, taskname, work, callback](){delegate(taskname, work,callback);}});
        return;
    }
    qInfo() << name() << "Executing " << taskname;
    setProgress(0);
    setTotal(0);
    setStatus(Status::Busy);
    m_lock = true;
    using ret_t = typename std::invoke_result<decltype(work)>::type;
    QFuture<ret_t> fut = QtConcurrent::run(m_pool, work);
    QFutureWatcher<ret_t> *fw = new QFutureWatcher<ret_t>();
    connect(fw, &QFutureWatcher<ret_t>::finished, [this, callback, fw, taskname](){
        m_lock = false;
        if (fw->isCanceled()) {
            try {
                fw->waitForFinished();
            } catch (const GitException &error) {
                qCritical() << name() << "encountered an error while executing" << taskname << ": " << error.errorString();
                setStatus(Status::Error);
                setGitStatus(GitStatusFlag::Error);
            }
            return;
        }
        TaskQueue old;
        m_tasks.swap(old);

        if constexpr (std::is_same<ret_t, void>::value)
            callback();
        else
            callback(fw->result());

        m_tasks.append(old);

        if (m_tasks.empty())
            setStatus(Status::Ready);
        else if (!m_lock)
            m_tasks.dequeue().second();
    });
    connect(fw, &QFutureWatcher<ret_t>::finished, fw, &QFutureWatcher<ret_t>::deleteLater);
    fw->setFuture(fut);
}

void Addon::delegate(QString taskname, auto work, bool force)
{
    using ret_t = typename std::invoke_result<decltype(work)>::type;
    if constexpr (std::is_same<ret_t, void>::value)
        delegate(taskname, work, [](){}, force);
    else
        delegate(taskname, work, [](ret_t){}, force);
}

void Addon::delegate(QString taskname, auto work)
{
    delegate(taskname, work, false);
}

void Addon::delegate(QString taskname, auto work, auto callback)
{
    delegate(taskname, work, callback, false);
}
