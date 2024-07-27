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

#ifndef GITCONTROL_H
#define GITCONTROL_H

#include <QException>
#include <QObject>
#include <QQueue>
#include <QUrl>
#include <functional>

class QThreadPool;
class GitException : public QException
{
    int m_code;
    QString m_errorString;
public:
    GitException(int code);
    GitException(const GitException&) = default;

    int code();
    QString errorString() const;

    // QException interface
public:
    void raise() const override;
    GitException *clone() const override;
};

int check_git_return(int code);

class Control : public QObject
{
    Q_OBJECT

    using TaskQueue = QQueue<QPair<QString,std::function<void()>>>;
    TaskQueue m_tasks;

    QList<QObject *> m_addons;

public:
    Q_PROPERTY(QList<QObject *> addons READ addons WRITE setAddons NOTIFY addonsChanged)
    Q_PROPERTY(int progress READ progress WRITE setProgress NOTIFY progressChanged)
    Q_PROPERTY(int total READ total WRITE setTotal NOTIFY totalChanged)
    Q_PROPERTY(QString statusMessage READ statusMessage WRITE setStatusMessage NOTIFY statusMessageChanged)
    Q_PROPERTY(bool firstBoot READ firstBoot WRITE setFirstBoot NOTIFY firstBootChanged)
    Q_PROPERTY(QString style READ style WRITE setStyle NOTIFY styleChanged)
    Q_PROPERTY(QStringList availableStyles READ availableStyles CONSTANT)
    Q_PROPERTY(QStringList addonsPaths READ addonsPaths WRITE setAddonsPaths NOTIFY addonsPathsChanged)
    Q_PROPERTY(bool useRepoDirectory READ useRepoDirectory WRITE setUseRepoDirectory NOTIFY useRepoDirectoryChanged)


    static QStringList m_availableStyles;

    ~Control();

    enum class Status {
        Error = -1,
        Ready,
        Busy
    };
    Q_ENUM(Status)
    Q_PROPERTY(Status status READ status WRITE setStatus NOTIFY statusChanged)

    enum MinimizeToTray {
        MinimizeToTrayAsk = -1,
        MinimizeToTrayNo = 0,
        MinimizeToTrayYes = 1
    };
    Q_ENUM(MinimizeToTray)
    Q_PROPERTY(MinimizeToTray minimizeToTray READ minimizeToTray WRITE setMinimizeToTray NOTIFY minimizeToTrayChanged)

    enum class UpdateStatus {
        UpdateError = -1,
        NoUpdate,
        UpdateAvailable,
        DownloadingUpdate,
        UpdateReady,
        CheckingForUpdate,
        UpdateDone
    };
    Q_ENUM(UpdateStatus)
    Q_PROPERTY(UpdateStatus updateStatus READ updateStatus WRITE setUpdateStatus NOTIFY updateStatusChanged)
    QList<QObject *> addons() const;

    static Control *instance();

    int progress() const;

    int total() const;

    Status status() const;

    QString statusMessage() const;

    bool firstBoot() const;

    MinimizeToTray minimizeToTray() const;

    QString style() const;

    QStringList availableStyles() const;

    UpdateStatus updateStatus() const;

    void completeUpdate(const QString &path);

    void init();

    QStringList addonsPaths() const;

    static void removeTocSuffixes(QString &string);

    bool useRepoDirectory() const;
    void setUseRepoDirectory(bool newUseRepoDirectory);

private:
    static Control *m_instance;
    explicit Control(QObject *parent = nullptr);

    int m_progress;

    int m_total;

    Status m_status;

    QString m_statusMessage;

    bool m_firstBoot;

    MinimizeToTray m_minimizeToTray;

    QThreadPool *m_pool;

    void delegate(QString message, auto work, auto callback);
    void delegate(QString message, auto work);

    QString m_style;

    UpdateStatus m_updateStatus = UpdateStatus::NoUpdate;

    QStringList m_addonsPaths;

    bool m_useRepoDirectory;

signals:
    void addonsChanged(QList<QObject *> addons);

    void progressChanged(int progress);

    void totalChanged(int total);

    void statusChanged(Status status);

    void statusMessageChanged(QString statusMessage);

    void firstBootChanged(bool firstBoot);

    void minimizeToTrayChanged(MinimizeToTray minimizeToTray);

    void styleChanged(QString style);

    void availableStylesChanged(QStringList availableStyles);

    void updateStatusChanged(UpdateStatus updateStatus);

    void addonsPathsChanged(QStringList addonsPaths);

    void addonsPathChanged(int i, QString path);

    void useRepoDirectoryChanged(bool use);

public slots:
    void setAddons(QList<QObject *> addons);
    void saveAddonsPaths();
    void scanForAddons(int i = -1);
    void clone(QUrl url, int i);
    void setProgress(int progress);
    void setTotal(int total);
    void setStatus(Status status);
    void setStatusMessage(QString statusMessage);
    void setFirstBoot(bool firstBoot);
    void setMinimizeToTray(MinimizeToTray minimizeToTray);
    void setStyle(QString style);
    void setAvailableStyles(QStringList availableStyles);
    void checkForUpdates();
    void downloadUpdate();
    void executeUpdate();
    void setUpdateStatus(UpdateStatus updateStatus);
    void setAddonsPaths(QStringList addonsPaths);
    void setAddonsPath(int i, QString path = QString());
    void saveUseRepoDirectory(bool use);
};
Q_DECLARE_METATYPE(Control::MinimizeToTray)

#endif // GITCONTROL_H
