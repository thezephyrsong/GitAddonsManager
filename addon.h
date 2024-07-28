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

#ifndef ADDON_H
#define ADDON_H

#include <QObject>
#include <QFileInfo>
#include <memory>
#include <functional>
#include <QQueue>
#include <QMutex>
#include <QWaitCondition>
#include "utils.h"
#include <QUrl>

class git_repository;
class git_reference;
using repo_p = std::unique_ptr<git_repository, void(*)(git_repository *)>;

class QThreadPool;

class Addon : public QObject
{
    Q_OBJECT
    using TaskQueue = QQueue<QPair<QString,std::function<void()>>>;
    TaskQueue m_tasks;

    QString m_name;
    repo_p m_repo;

    QStringList m_branches;

    QString m_currentBranch;

    QString m_remote;

    QStringList m_remotes;

    int m_progress;

    int m_total;

    QString m_path;

    QMutex m_mutex;
    QWaitCondition m_wait;
    void *m_result;
public:
    Q_PROPERTY(QString name READ name WRITE setName NOTIFY nameChanged)
    Q_PROPERTY(QStringList branches READ branches WRITE setBranches NOTIFY branchesChanged)
    Q_PROPERTY(QString currentBranch READ currentBranch WRITE setCurrentBranch NOTIFY currentBranchChanged)
    Q_PROPERTY(QString remote READ remote WRITE setRemote NOTIFY remoteChanged)
    Q_PROPERTY(QStringList remotes READ remotes WRITE setRemotes NOTIFY remotesChanged)
    Q_PROPERTY(int progress READ progress WRITE setProgress NOTIFY progressChanged)
    Q_PROPERTY(int total READ total WRITE setTotal NOTIFY totalChanged)
    Q_PROPERTY(QStringList subfolders READ subfolders WRITE setSubfolders NOTIFY subfoldersChanged)
    Q_PROPERTY(QString filesToRemove READ filesToRemove WRITE setFilesToRemove NOTIFY filesToRemoveChanged)
    Q_PROPERTY(QString readme READ readme WRITE setReadme NOTIFY readmeChanged)
    Q_PROPERTY(QString path READ path)

    enum class Status {
        Error = -1,
        Ready,
        Busy
    };
    Q_ENUM(Status)
    Q_PROPERTY(Status status READ status WRITE setStatus NOTIFY statusChanged)

    enum GitStatusFlag {
        Error = 0x1,
        UpToDate = 0x2,
        Ahead = 0x4,
        Behind = 0x8,
        Diverged = 0x10,
        FastForward = 0x20,
        Merge = 0x40,
        Conflict = 0x80,
        MergeStatusMask = 0x0C00
    };
    Q_ENUM(GitStatusFlag)
    Q_DECLARE_FLAGS(GitStatus, GitStatusFlag)
    Q_PROPERTY(GitStatus gitStatus READ gitStatus WRITE setGitStatus NOTIFY gitStatusChanged)
    Q_FLAGS(GitStatus)

    explicit Addon(QString name, git_repository *repo, QString path, QObject *parent = nullptr);

    QString name() const;

    QStringList branches() const;

    QString currentBranch() const;

    QString remote() const;

    QStringList remotes() const;

    int progress() const;

    int total() const;

    Status status() const;

    GitStatus gitStatus() const;

    QStringList subfolders() const;

    QString filesToRemove() const;

    QString readme() const;

    QString path() const;

    Q_INVOKABLE QUrl getUrl(QString name = QString()) const;

private:

    Status m_status;

    GitStatus m_gitStatus;

    QStringList m_subfolders;

    void closeRepo();

    void openRepo();

    AutoPtr<git_reference> remoteRefForBranch(QString name);
    AutoPtr<git_reference> branchRef(QString name);

    void delegate(QString taskname, auto work, auto callback, bool force);
    void delegate(QString taskname, auto work, bool force);

    void delegate(QString taskname, auto work, auto callback);
    void delegate(QString taskname, auto work);

    QThreadPool *m_pool;

    bool removeFolders(QStringList paths, bool ask = true);
    bool removeFolder(QString path, bool ask = true);
    QString m_filesToRemove;

    QString m_readme;
signals:

    void nameChanged(QString name);

    void branchesChanged(QStringList branches);

    void currentBranchChanged(QString currentBranch);

    void remoteChanged(QString remote);

    void remotesChanged(QStringList remotes);

    void progressChanged(int progress);

    void totalChanged(int total);

    void statusChanged(Status status);

    void gitStatusChanged(GitStatus gitStatus);

    void subfoldersChanged(QStringList subfolders);

    void filesToRemoveChanged(QString filesToRemove);

    void readmeChanged(QString readme);

    void pathChanged(QString path);

public slots:
    void setName(QString name);
    void update();
    void scanBranches();
    void scanSubfolders();
    void setBranches(QStringList branches);
    void setCurrentBranch(QString currentBranch);
    void setRemote(QString remote);
    void setRemotes(QStringList remotes);
    void fetchRemote(QString remote);
    void setProgress(int progress);
    void setTotal(int total);
    void setStatus(Status status);
    void setGitStatus(GitStatus gitStatus);
    void updateGitStatus();
    void uninstall();
    void setSubfolders(QStringList subfolders);
    void removeSubfolders();
    void unpackSubfolders();
    void setFilesToRemove(QString filesToRemove);
    void confirmFileRemove(bool confirmed);
    void reset();
    void reclone();
    void loadReadme();
    void setReadme(QString readme);
};

Q_DECLARE_OPERATORS_FOR_FLAGS(Addon::GitStatus)

#endif // ADDON_H
