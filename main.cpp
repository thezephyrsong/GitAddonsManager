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

int main(int argc, char *argv[])
{
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
    QApplication app(argc, argv);
    app.setApplicationDisplayName("Git Addons Manager");
    QCoreApplication::setOrganizationName("GitAddonsManager");
    QCoreApplication::setApplicationName("Git Addons Manager");

    QIcon::setThemeName("breeze");

    QQmlApplicationEngine engine;
    qmlRegisterSingletonType<Control>("GitAddonsManager.engine",1,0,"Engine",[](QQmlEngine *, QJSEngine *)->QObject*{
        return Control::instance();
    });
    qmlRegisterUncreatableType<Addon>("GitAddonsManager.engine",1,0,"Addon","");
    engine.load(QUrl(QStringLiteral("qrc:/main.qml")));
    if (engine.rootObjects().isEmpty())
        return -1;

    return app.exec();
}
