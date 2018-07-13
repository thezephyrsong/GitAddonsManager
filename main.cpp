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

int main(int argc, char *argv[])
{
    int res;

    qmlClearTypeRegistrations();
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
    QApplication app(argc, argv);
    app.setApplicationDisplayName(QObject::tr("Git Addons Manager"));
    QCoreApplication::setOrganizationName("GitAddonsManager");
    QCoreApplication::setApplicationName("Git Addons Manager");

    QFontDatabase::addApplicationFont(":/fonts/Hack-Bold.ttf");
    QFontDatabase::addApplicationFont(":/fonts/Hack-BoldItalic.ttf");
    QFontDatabase::addApplicationFont(":/fonts/Hack-Italic.ttf");
    QFontDatabase::addApplicationFont(":/fonts/Hack-Regular.ttf");
    QIcon::setThemeName("breeze");
    QQmlApplicationEngine engine;
    bool ok;
    QSettings setts;
    QStringList styles = QQuickStyle::availableStyles();
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
    qmlRegisterUncreatableType<Addon>("GitAddonsManager.engine",1,0,"Addon","");
    engine.rootContext()->setContextProperty("gitVersion", GIT_DESCRIBE);
    engine.load(QUrl(QStringLiteral("qrc:/main.qml")));
    if (engine.rootObjects().isEmpty())
        return -1;
    QObject::connect(Control::instance(), &Control::styleChanged,[&setts](const QString &style){
        setts.setValue("style", style);
    });

    res = app.exec();
    if (res == 1)
        QProcess().startDetached(app.arguments()[0],app.arguments().mid(1));
    return res;
}
