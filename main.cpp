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

#include <QApplication>
#include <QQmlApplicationEngine>
#include "control.h"
#include "addon.h"
#include <QIcon>
#include <QQmlContext>
#include <QQuickStyle>
#include <QInputDialog>
#include <QSettings>
#include <QProcess>
#include <QFontDatabase>
#include <QCommandLineParser>

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    QCommandLineParser parser;
    parser.addOption({"update", "directory to update", "updateLocation"});
    parser.process(app);
    QString update = parser.value("update");
    app.setApplicationDisplayName(QObject::tr("Git Addons Manager"));
    QCoreApplication::setOrganizationName("GitAddonsManager");
    QCoreApplication::setApplicationName("Git Addons Manager");
    QFontDatabase::addApplicationFont(":/fonts/Hack-Bold.ttf");
    QFontDatabase::addApplicationFont(":/fonts/Hack-BoldItalic.ttf");
    QFontDatabase::addApplicationFont(":/fonts/Hack-Italic.ttf");
    QFontDatabase::addApplicationFont(":/fonts/Hack-Regular.ttf");
    QIcon::setThemeName("breeze");
    int res;
    do {
        qmlClearTypeRegistrations();

        QQmlApplicationEngine engine;
        bool ok;
        QSettings setts;
        QStringList styles = {"Fusion","Material","Imagine","Universal","Basic"};
        if (!styles.contains(QQuickStyle::name()))
            styles.prepend(QQuickStyle::name());
        Control::m_availableStyles = styles;
        QString style = setts.value("style").toString();
        if (style.isNull())
            style = QInputDialog::getItem(nullptr, QObject::tr("Choose a style"), QObject::tr("You will be able to pick another style later by visiting the Options tab."), styles, styles.indexOf(QQuickStyle::name()), false, &ok);
        QQuickStyle::setStyle(style);
        if (ok) {
            setts.setValue("style", style);
            setts.sync();
        }
        qmlRegisterSingletonType<Control>("GitAddonsManager.engine",1,0,"Engine",[](QQmlEngine *, QJSEngine *)->QObject*{
            return Control::instance();
        });
        if (!update.isEmpty()) {
            Control::instance()->completeUpdate(update);
            engine.load(QUrl(QStringLiteral("qrc:/Update.qml")));
        } else {
            Control::instance()->init();
            qmlRegisterUncreatableType<Addon>("GitAddonsManager.engine",1,0,"Addon","");
            engine.rootContext()->setContextProperty("gitVersion", QString("%1 (%2 %3)").arg(GIT_DESCRIBE)
#ifdef GAM_BUILD_NAME
                                                                        .arg(GAM_BUILD_NAME)
#else
                                                                        .arg(QSysInfo::productType())
#endif
#ifdef GAM_BANCH_NAME
                                                                       .arg(GAM_BANCH_NAME)
#else
                                                                       .arg(QSysInfo::buildCpuArchitecture())
#endif
                                                     );
            engine.load(QUrl(QStringLiteral("qrc:/main.qml")));
        }
        if (engine.rootObjects().isEmpty())
            return -1;
        QObject::connect(Control::instance(), &Control::styleChanged,[&setts](const QString &style){
            setts.setValue("style", style);
        });
        res = app.exec();
    } while (res == 1);
    if (res == 2) {
        QStringList args = app.arguments().mid(1);
        args << "--update" << QString("%1").arg(app.applicationDirPath());
        QProcess::startDetached(app.applicationDirPath()+"/update/" + GAM_EXEC, args, app.applicationDirPath()+"/update");
    }
    if (res == 3)
        QProcess::startDetached(update + "/" + GAM_EXEC, QApplication::arguments().mid(1) << "--update" << "", update);
    return res;
}
