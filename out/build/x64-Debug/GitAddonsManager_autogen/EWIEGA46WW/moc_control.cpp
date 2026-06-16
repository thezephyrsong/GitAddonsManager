/****************************************************************************
** Meta object code from reading C++ file 'control.h'
**
** Created by: The Qt Meta Object Compiler version 69 (Qt 6.11.1)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../../../control.h"
#include <QtCore/qmetatype.h>
#include <QtCore/QList>

#include <QtCore/qtmochelpers.h>

#include <memory>


#include <QtCore/qxptype_traits.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'control.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 69
#error "This file was generated using the moc from 6.11.1. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

#ifndef Q_CONSTINIT
#define Q_CONSTINIT
#endif

QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
QT_WARNING_DISABLE_GCC("-Wuseless-cast")
namespace {
struct qt_meta_tag_ZN7ControlE_t {};
} // unnamed namespace

template <> constexpr inline auto Control::qt_create_metaobjectdata<qt_meta_tag_ZN7ControlE_t>()
{
    namespace QMC = QtMocConstants;
    QtMocHelpers::StringRefStorage qt_stringData {
        "Control",
        "addonsChanged",
        "",
        "QList<Addon*>",
        "addons",
        "progressChanged",
        "progress",
        "totalChanged",
        "total",
        "statusChanged",
        "Status",
        "status",
        "statusMessageChanged",
        "statusMessage",
        "firstBootChanged",
        "firstBoot",
        "minimizeToTrayChanged",
        "MinimizeToTray",
        "minimizeToTray",
        "styleChanged",
        "style",
        "availableStylesChanged",
        "availableStyles",
        "updateStatusChanged",
        "UpdateStatus",
        "updateStatus",
        "addonsPathsChanged",
        "addonsPaths",
        "addonsPathChanged",
        "i",
        "path",
        "useRepoDirectoryChanged",
        "use",
        "logMessage",
        "log",
        "setAddons",
        "saveAddonsPaths",
        "scanForAddons",
        "clone",
        "QUrl",
        "url",
        "setProgress",
        "setTotal",
        "setStatus",
        "setStatusMessage",
        "setFirstBoot",
        "setMinimizeToTray",
        "setStyle",
        "setAvailableStyles",
        "checkForUpdates",
        "downloadUpdate",
        "executeUpdate",
        "setUpdateStatus",
        "setAddonsPaths",
        "setAddonsPath",
        "saveUseRepoDirectory",
        "exportAddonList",
        "version",
        "selfUpdates",
        "useRepoDirectory",
        "Error",
        "Ready",
        "Busy",
        "MinimizeToTrayAsk",
        "MinimizeToTrayNo",
        "MinimizeToTrayYes",
        "UpdateError",
        "NoUpdate",
        "UpdateAvailable",
        "DownloadingUpdate",
        "UpdateReady",
        "CheckingForUpdate",
        "UpdateDone"
    };

    QtMocHelpers::UintData qt_methods {
        // Signal 'addonsChanged'
        QtMocHelpers::SignalData<void(QList<Addon*>)>(1, 2, QMC::AccessPublic, QMetaType::Void, {{
            { 0x80000000 | 3, 4 },
        }}),
        // Signal 'progressChanged'
        QtMocHelpers::SignalData<void(int)>(5, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::Int, 6 },
        }}),
        // Signal 'totalChanged'
        QtMocHelpers::SignalData<void(int)>(7, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::Int, 8 },
        }}),
        // Signal 'statusChanged'
        QtMocHelpers::SignalData<void(enum Status)>(9, 2, QMC::AccessPublic, QMetaType::Void, {{
            { 0x80000000 | 10, 11 },
        }}),
        // Signal 'statusMessageChanged'
        QtMocHelpers::SignalData<void(QString)>(12, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::QString, 13 },
        }}),
        // Signal 'firstBootChanged'
        QtMocHelpers::SignalData<void(bool)>(14, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::Bool, 15 },
        }}),
        // Signal 'minimizeToTrayChanged'
        QtMocHelpers::SignalData<void(enum MinimizeToTray)>(16, 2, QMC::AccessPublic, QMetaType::Void, {{
            { 0x80000000 | 17, 18 },
        }}),
        // Signal 'styleChanged'
        QtMocHelpers::SignalData<void(QString)>(19, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::QString, 20 },
        }}),
        // Signal 'availableStylesChanged'
        QtMocHelpers::SignalData<void(QStringList)>(21, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::QStringList, 22 },
        }}),
        // Signal 'updateStatusChanged'
        QtMocHelpers::SignalData<void(enum UpdateStatus)>(23, 2, QMC::AccessPublic, QMetaType::Void, {{
            { 0x80000000 | 24, 25 },
        }}),
        // Signal 'addonsPathsChanged'
        QtMocHelpers::SignalData<void(QStringList)>(26, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::QStringList, 27 },
        }}),
        // Signal 'addonsPathChanged'
        QtMocHelpers::SignalData<void(int, QString)>(28, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::Int, 29 }, { QMetaType::QString, 30 },
        }}),
        // Signal 'useRepoDirectoryChanged'
        QtMocHelpers::SignalData<void(bool)>(31, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::Bool, 32 },
        }}),
        // Signal 'logMessage'
        QtMocHelpers::SignalData<void(QString)>(33, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::QString, 34 },
        }}),
        // Slot 'setAddons'
        QtMocHelpers::SlotData<void(QList<Addon*>)>(35, 2, QMC::AccessPublic, QMetaType::Void, {{
            { 0x80000000 | 3, 4 },
        }}),
        // Slot 'saveAddonsPaths'
        QtMocHelpers::SlotData<void()>(36, 2, QMC::AccessPublic, QMetaType::Void),
        // Slot 'scanForAddons'
        QtMocHelpers::SlotData<void(int)>(37, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::Int, 29 },
        }}),
        // Slot 'scanForAddons'
        QtMocHelpers::SlotData<void()>(37, 2, QMC::AccessPublic | QMC::MethodCloned, QMetaType::Void),
        // Slot 'clone'
        QtMocHelpers::SlotData<void(QUrl, int)>(38, 2, QMC::AccessPublic, QMetaType::Void, {{
            { 0x80000000 | 39, 40 }, { QMetaType::Int, 29 },
        }}),
        // Slot 'setProgress'
        QtMocHelpers::SlotData<void(int)>(41, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::Int, 6 },
        }}),
        // Slot 'setTotal'
        QtMocHelpers::SlotData<void(int)>(42, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::Int, 8 },
        }}),
        // Slot 'setStatus'
        QtMocHelpers::SlotData<void(enum Status)>(43, 2, QMC::AccessPublic, QMetaType::Void, {{
            { 0x80000000 | 10, 11 },
        }}),
        // Slot 'setStatusMessage'
        QtMocHelpers::SlotData<void(QString)>(44, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::QString, 13 },
        }}),
        // Slot 'setFirstBoot'
        QtMocHelpers::SlotData<void(bool)>(45, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::Bool, 15 },
        }}),
        // Slot 'setMinimizeToTray'
        QtMocHelpers::SlotData<void(enum MinimizeToTray)>(46, 2, QMC::AccessPublic, QMetaType::Void, {{
            { 0x80000000 | 17, 18 },
        }}),
        // Slot 'setStyle'
        QtMocHelpers::SlotData<void(QString)>(47, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::QString, 20 },
        }}),
        // Slot 'setAvailableStyles'
        QtMocHelpers::SlotData<void(QStringList)>(48, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::QStringList, 22 },
        }}),
        // Slot 'checkForUpdates'
        QtMocHelpers::SlotData<void()>(49, 2, QMC::AccessPublic, QMetaType::Void),
        // Slot 'downloadUpdate'
        QtMocHelpers::SlotData<void()>(50, 2, QMC::AccessPublic, QMetaType::Void),
        // Slot 'executeUpdate'
        QtMocHelpers::SlotData<void()>(51, 2, QMC::AccessPublic, QMetaType::Void),
        // Slot 'setUpdateStatus'
        QtMocHelpers::SlotData<void(enum UpdateStatus)>(52, 2, QMC::AccessPublic, QMetaType::Void, {{
            { 0x80000000 | 24, 25 },
        }}),
        // Slot 'setAddonsPaths'
        QtMocHelpers::SlotData<void(QStringList)>(53, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::QStringList, 27 },
        }}),
        // Slot 'setAddonsPath'
        QtMocHelpers::SlotData<void(int, QString)>(54, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::Int, 29 }, { QMetaType::QString, 30 },
        }}),
        // Slot 'setAddonsPath'
        QtMocHelpers::SlotData<void(int)>(54, 2, QMC::AccessPublic | QMC::MethodCloned, QMetaType::Void, {{
            { QMetaType::Int, 29 },
        }}),
        // Slot 'saveUseRepoDirectory'
        QtMocHelpers::SlotData<void(bool)>(55, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::Bool, 32 },
        }}),
        // Method 'exportAddonList'
        QtMocHelpers::MethodData<void(const QUrl &)>(56, 2, QMC::AccessPublic, QMetaType::Void, {{
            { 0x80000000 | 39, 30 },
        }}),
    };
    QtMocHelpers::UintData qt_properties {
        // property 'version'
        QtMocHelpers::PropertyData<QString>(57, QMetaType::QString, QMC::DefaultPropertyFlags | QMC::Constant),
        // property 'selfUpdates'
        QtMocHelpers::PropertyData<bool>(58, QMetaType::Bool, QMC::DefaultPropertyFlags | QMC::Constant),
        // property 'addons'
        QtMocHelpers::PropertyData<QList<Addon*>>(4, 0x80000000 | 3, QMC::DefaultPropertyFlags | QMC::Writable | QMC::EnumOrFlag | QMC::StdCppSet, 0),
        // property 'progress'
        QtMocHelpers::PropertyData<int>(6, QMetaType::Int, QMC::DefaultPropertyFlags | QMC::Writable | QMC::StdCppSet, 1),
        // property 'total'
        QtMocHelpers::PropertyData<int>(8, QMetaType::Int, QMC::DefaultPropertyFlags | QMC::Writable | QMC::StdCppSet, 2),
        // property 'statusMessage'
        QtMocHelpers::PropertyData<QString>(13, QMetaType::QString, QMC::DefaultPropertyFlags | QMC::Writable | QMC::StdCppSet, 4),
        // property 'firstBoot'
        QtMocHelpers::PropertyData<bool>(15, QMetaType::Bool, QMC::DefaultPropertyFlags | QMC::Writable | QMC::StdCppSet, 5),
        // property 'style'
        QtMocHelpers::PropertyData<QString>(20, QMetaType::QString, QMC::DefaultPropertyFlags | QMC::Writable | QMC::StdCppSet, 7),
        // property 'availableStyles'
        QtMocHelpers::PropertyData<QStringList>(22, QMetaType::QStringList, QMC::DefaultPropertyFlags | QMC::Constant),
        // property 'addonsPaths'
        QtMocHelpers::PropertyData<QStringList>(27, QMetaType::QStringList, QMC::DefaultPropertyFlags | QMC::Writable | QMC::StdCppSet, 10),
        // property 'useRepoDirectory'
        QtMocHelpers::PropertyData<bool>(59, QMetaType::Bool, QMC::DefaultPropertyFlags | QMC::Writable | QMC::StdCppSet, 12),
        // property 'status'
        QtMocHelpers::PropertyData<enum Status>(11, 0x80000000 | 10, QMC::DefaultPropertyFlags | QMC::Writable | QMC::EnumOrFlag | QMC::StdCppSet, 3),
        // property 'minimizeToTray'
        QtMocHelpers::PropertyData<enum MinimizeToTray>(18, 0x80000000 | 17, QMC::DefaultPropertyFlags | QMC::Writable | QMC::EnumOrFlag | QMC::StdCppSet, 6),
        // property 'updateStatus'
        QtMocHelpers::PropertyData<enum UpdateStatus>(25, 0x80000000 | 24, QMC::DefaultPropertyFlags | QMC::Writable | QMC::EnumOrFlag | QMC::StdCppSet, 9),
    };
    QtMocHelpers::UintData qt_enums {
        // enum 'Status'
        QtMocHelpers::EnumData<enum Status>(10, 10, QMC::EnumIsScoped).add({
            {   60, Status::Error },
            {   61, Status::Ready },
            {   62, Status::Busy },
        }),
        // enum 'MinimizeToTray'
        QtMocHelpers::EnumData<enum MinimizeToTray>(17, 17, QMC::EnumFlags{}).add({
            {   63, MinimizeToTray::MinimizeToTrayAsk },
            {   64, MinimizeToTray::MinimizeToTrayNo },
            {   65, MinimizeToTray::MinimizeToTrayYes },
        }),
        // enum 'UpdateStatus'
        QtMocHelpers::EnumData<enum UpdateStatus>(24, 24, QMC::EnumIsScoped).add({
            {   66, UpdateStatus::UpdateError },
            {   67, UpdateStatus::NoUpdate },
            {   68, UpdateStatus::UpdateAvailable },
            {   69, UpdateStatus::DownloadingUpdate },
            {   70, UpdateStatus::UpdateReady },
            {   71, UpdateStatus::CheckingForUpdate },
            {   72, UpdateStatus::UpdateDone },
        }),
    };
    return QtMocHelpers::metaObjectData<Control, qt_meta_tag_ZN7ControlE_t>(QMC::MetaObjectFlag{}, qt_stringData,
            qt_methods, qt_properties, qt_enums);
}
Q_CONSTINIT const QMetaObject Control::staticMetaObject = { {
    QMetaObject::SuperData::link<QObject::staticMetaObject>(),
    qt_staticMetaObjectStaticContent<qt_meta_tag_ZN7ControlE_t>.stringdata,
    qt_staticMetaObjectStaticContent<qt_meta_tag_ZN7ControlE_t>.data,
    qt_static_metacall,
    nullptr,
    qt_staticMetaObjectRelocatingContent<qt_meta_tag_ZN7ControlE_t>.metaTypes,
    nullptr
} };

void Control::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    auto *_t = static_cast<Control *>(_o);
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: _t->addonsChanged((*reinterpret_cast<std::add_pointer_t<QList<Addon*>>>(_a[1]))); break;
        case 1: _t->progressChanged((*reinterpret_cast<std::add_pointer_t<int>>(_a[1]))); break;
        case 2: _t->totalChanged((*reinterpret_cast<std::add_pointer_t<int>>(_a[1]))); break;
        case 3: _t->statusChanged((*reinterpret_cast<std::add_pointer_t<enum Status>>(_a[1]))); break;
        case 4: _t->statusMessageChanged((*reinterpret_cast<std::add_pointer_t<QString>>(_a[1]))); break;
        case 5: _t->firstBootChanged((*reinterpret_cast<std::add_pointer_t<bool>>(_a[1]))); break;
        case 6: _t->minimizeToTrayChanged((*reinterpret_cast<std::add_pointer_t<enum MinimizeToTray>>(_a[1]))); break;
        case 7: _t->styleChanged((*reinterpret_cast<std::add_pointer_t<QString>>(_a[1]))); break;
        case 8: _t->availableStylesChanged((*reinterpret_cast<std::add_pointer_t<QStringList>>(_a[1]))); break;
        case 9: _t->updateStatusChanged((*reinterpret_cast<std::add_pointer_t<enum UpdateStatus>>(_a[1]))); break;
        case 10: _t->addonsPathsChanged((*reinterpret_cast<std::add_pointer_t<QStringList>>(_a[1]))); break;
        case 11: _t->addonsPathChanged((*reinterpret_cast<std::add_pointer_t<int>>(_a[1])),(*reinterpret_cast<std::add_pointer_t<QString>>(_a[2]))); break;
        case 12: _t->useRepoDirectoryChanged((*reinterpret_cast<std::add_pointer_t<bool>>(_a[1]))); break;
        case 13: _t->logMessage((*reinterpret_cast<std::add_pointer_t<QString>>(_a[1]))); break;
        case 14: _t->setAddons((*reinterpret_cast<std::add_pointer_t<QList<Addon*>>>(_a[1]))); break;
        case 15: _t->saveAddonsPaths(); break;
        case 16: _t->scanForAddons((*reinterpret_cast<std::add_pointer_t<int>>(_a[1]))); break;
        case 17: _t->scanForAddons(); break;
        case 18: _t->clone((*reinterpret_cast<std::add_pointer_t<QUrl>>(_a[1])),(*reinterpret_cast<std::add_pointer_t<int>>(_a[2]))); break;
        case 19: _t->setProgress((*reinterpret_cast<std::add_pointer_t<int>>(_a[1]))); break;
        case 20: _t->setTotal((*reinterpret_cast<std::add_pointer_t<int>>(_a[1]))); break;
        case 21: _t->setStatus((*reinterpret_cast<std::add_pointer_t<enum Status>>(_a[1]))); break;
        case 22: _t->setStatusMessage((*reinterpret_cast<std::add_pointer_t<QString>>(_a[1]))); break;
        case 23: _t->setFirstBoot((*reinterpret_cast<std::add_pointer_t<bool>>(_a[1]))); break;
        case 24: _t->setMinimizeToTray((*reinterpret_cast<std::add_pointer_t<enum MinimizeToTray>>(_a[1]))); break;
        case 25: _t->setStyle((*reinterpret_cast<std::add_pointer_t<QString>>(_a[1]))); break;
        case 26: _t->setAvailableStyles((*reinterpret_cast<std::add_pointer_t<QStringList>>(_a[1]))); break;
        case 27: _t->checkForUpdates(); break;
        case 28: _t->downloadUpdate(); break;
        case 29: _t->executeUpdate(); break;
        case 30: _t->setUpdateStatus((*reinterpret_cast<std::add_pointer_t<enum UpdateStatus>>(_a[1]))); break;
        case 31: _t->setAddonsPaths((*reinterpret_cast<std::add_pointer_t<QStringList>>(_a[1]))); break;
        case 32: _t->setAddonsPath((*reinterpret_cast<std::add_pointer_t<int>>(_a[1])),(*reinterpret_cast<std::add_pointer_t<QString>>(_a[2]))); break;
        case 33: _t->setAddonsPath((*reinterpret_cast<std::add_pointer_t<int>>(_a[1]))); break;
        case 34: _t->saveUseRepoDirectory((*reinterpret_cast<std::add_pointer_t<bool>>(_a[1]))); break;
        case 35: _t->exportAddonList((*reinterpret_cast<std::add_pointer_t<QUrl>>(_a[1]))); break;
        default: ;
        }
    }
    if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        switch (_id) {
        default: *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType(); break;
        case 0:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType(); break;
            case 0:
                *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType::fromType< QList<Addon*> >(); break;
            }
            break;
        case 14:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType(); break;
            case 0:
                *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType::fromType< QList<Addon*> >(); break;
            }
            break;
        }
    }
    if (_c == QMetaObject::IndexOfMethod) {
        if (QtMocHelpers::indexOfMethod<void (Control::*)(QList<Addon*> )>(_a, &Control::addonsChanged, 0))
            return;
        if (QtMocHelpers::indexOfMethod<void (Control::*)(int )>(_a, &Control::progressChanged, 1))
            return;
        if (QtMocHelpers::indexOfMethod<void (Control::*)(int )>(_a, &Control::totalChanged, 2))
            return;
        if (QtMocHelpers::indexOfMethod<void (Control::*)(Status )>(_a, &Control::statusChanged, 3))
            return;
        if (QtMocHelpers::indexOfMethod<void (Control::*)(QString )>(_a, &Control::statusMessageChanged, 4))
            return;
        if (QtMocHelpers::indexOfMethod<void (Control::*)(bool )>(_a, &Control::firstBootChanged, 5))
            return;
        if (QtMocHelpers::indexOfMethod<void (Control::*)(MinimizeToTray )>(_a, &Control::minimizeToTrayChanged, 6))
            return;
        if (QtMocHelpers::indexOfMethod<void (Control::*)(QString )>(_a, &Control::styleChanged, 7))
            return;
        if (QtMocHelpers::indexOfMethod<void (Control::*)(QStringList )>(_a, &Control::availableStylesChanged, 8))
            return;
        if (QtMocHelpers::indexOfMethod<void (Control::*)(UpdateStatus )>(_a, &Control::updateStatusChanged, 9))
            return;
        if (QtMocHelpers::indexOfMethod<void (Control::*)(QStringList )>(_a, &Control::addonsPathsChanged, 10))
            return;
        if (QtMocHelpers::indexOfMethod<void (Control::*)(int , QString )>(_a, &Control::addonsPathChanged, 11))
            return;
        if (QtMocHelpers::indexOfMethod<void (Control::*)(bool )>(_a, &Control::useRepoDirectoryChanged, 12))
            return;
        if (QtMocHelpers::indexOfMethod<void (Control::*)(QString )>(_a, &Control::logMessage, 13))
            return;
    }
    if (_c == QMetaObject::RegisterPropertyMetaType) {
        switch (_id) {
        default: *reinterpret_cast<int*>(_a[0]) = -1; break;
        case 2:
            *reinterpret_cast<int*>(_a[0]) = qRegisterMetaType< QList<Addon*> >(); break;
        }
    }
    if (_c == QMetaObject::ReadProperty) {
        void *_v = _a[0];
        switch (_id) {
        case 0: *reinterpret_cast<QString*>(_v) = _t->version(); break;
        case 1: *reinterpret_cast<bool*>(_v) = _t->selfUpdates(); break;
        case 2: *reinterpret_cast<QList<Addon*>*>(_v) = _t->addons(); break;
        case 3: *reinterpret_cast<int*>(_v) = _t->progress(); break;
        case 4: *reinterpret_cast<int*>(_v) = _t->total(); break;
        case 5: *reinterpret_cast<QString*>(_v) = _t->statusMessage(); break;
        case 6: *reinterpret_cast<bool*>(_v) = _t->firstBoot(); break;
        case 7: *reinterpret_cast<QString*>(_v) = _t->style(); break;
        case 8: *reinterpret_cast<QStringList*>(_v) = _t->availableStyles(); break;
        case 9: *reinterpret_cast<QStringList*>(_v) = _t->addonsPaths(); break;
        case 10: *reinterpret_cast<bool*>(_v) = _t->useRepoDirectory(); break;
        case 11: *reinterpret_cast<enum Status*>(_v) = _t->status(); break;
        case 12: *reinterpret_cast<enum MinimizeToTray*>(_v) = _t->minimizeToTray(); break;
        case 13: *reinterpret_cast<enum UpdateStatus*>(_v) = _t->updateStatus(); break;
        default: break;
        }
    }
    if (_c == QMetaObject::WriteProperty) {
        void *_v = _a[0];
        switch (_id) {
        case 2: _t->setAddons(*reinterpret_cast<QList<Addon*>*>(_v)); break;
        case 3: _t->setProgress(*reinterpret_cast<int*>(_v)); break;
        case 4: _t->setTotal(*reinterpret_cast<int*>(_v)); break;
        case 5: _t->setStatusMessage(*reinterpret_cast<QString*>(_v)); break;
        case 6: _t->setFirstBoot(*reinterpret_cast<bool*>(_v)); break;
        case 7: _t->setStyle(*reinterpret_cast<QString*>(_v)); break;
        case 9: _t->setAddonsPaths(*reinterpret_cast<QStringList*>(_v)); break;
        case 10: _t->setUseRepoDirectory(*reinterpret_cast<bool*>(_v)); break;
        case 11: _t->setStatus(*reinterpret_cast<enum Status*>(_v)); break;
        case 12: _t->setMinimizeToTray(*reinterpret_cast<enum MinimizeToTray*>(_v)); break;
        case 13: _t->setUpdateStatus(*reinterpret_cast<enum UpdateStatus*>(_v)); break;
        default: break;
        }
    }
}

const QMetaObject *Control::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *Control::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_staticMetaObjectStaticContent<qt_meta_tag_ZN7ControlE_t>.strings))
        return static_cast<void*>(this);
    return QObject::qt_metacast(_clname);
}

int Control::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 36)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 36;
    }
    if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 36)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 36;
    }
    if (_c == QMetaObject::ReadProperty || _c == QMetaObject::WriteProperty
            || _c == QMetaObject::ResetProperty || _c == QMetaObject::BindableProperty
            || _c == QMetaObject::RegisterPropertyMetaType) {
        qt_static_metacall(this, _c, _id, _a);
        _id -= 14;
    }
    return _id;
}

// SIGNAL 0
void Control::addonsChanged(QList<Addon*> _t1)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 0, nullptr, _t1);
}

// SIGNAL 1
void Control::progressChanged(int _t1)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 1, nullptr, _t1);
}

// SIGNAL 2
void Control::totalChanged(int _t1)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 2, nullptr, _t1);
}

// SIGNAL 3
void Control::statusChanged(Status _t1)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 3, nullptr, _t1);
}

// SIGNAL 4
void Control::statusMessageChanged(QString _t1)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 4, nullptr, _t1);
}

// SIGNAL 5
void Control::firstBootChanged(bool _t1)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 5, nullptr, _t1);
}

// SIGNAL 6
void Control::minimizeToTrayChanged(MinimizeToTray _t1)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 6, nullptr, _t1);
}

// SIGNAL 7
void Control::styleChanged(QString _t1)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 7, nullptr, _t1);
}

// SIGNAL 8
void Control::availableStylesChanged(QStringList _t1)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 8, nullptr, _t1);
}

// SIGNAL 9
void Control::updateStatusChanged(UpdateStatus _t1)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 9, nullptr, _t1);
}

// SIGNAL 10
void Control::addonsPathsChanged(QStringList _t1)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 10, nullptr, _t1);
}

// SIGNAL 11
void Control::addonsPathChanged(int _t1, QString _t2)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 11, nullptr, _t1, _t2);
}

// SIGNAL 12
void Control::useRepoDirectoryChanged(bool _t1)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 12, nullptr, _t1);
}

// SIGNAL 13
void Control::logMessage(QString _t1)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 13, nullptr, _t1);
}
QT_WARNING_POP
