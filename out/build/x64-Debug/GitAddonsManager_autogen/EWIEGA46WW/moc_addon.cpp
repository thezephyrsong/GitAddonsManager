/****************************************************************************
** Meta object code from reading C++ file 'addon.h'
**
** Created by: The Qt Meta Object Compiler version 69 (Qt 6.11.1)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../../../addon.h"
#include <QtCore/qmetatype.h>

#include <QtCore/qtmochelpers.h>

#include <memory>


#include <QtCore/qxptype_traits.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'addon.h' doesn't include <QObject>."
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
struct qt_meta_tag_ZN5AddonE_t {};
} // unnamed namespace

template <> constexpr inline auto Addon::qt_create_metaobjectdata<qt_meta_tag_ZN5AddonE_t>()
{
    namespace QMC = QtMocConstants;
    QtMocHelpers::StringRefStorage qt_stringData {
        "Addon",
        "nameChanged",
        "",
        "name",
        "branchesChanged",
        "branches",
        "currentBranchChanged",
        "currentBranch",
        "remoteChanged",
        "remote",
        "remotesChanged",
        "remotes",
        "progressChanged",
        "progress",
        "totalChanged",
        "total",
        "statusChanged",
        "Status",
        "status",
        "gitStatusChanged",
        "GitStatus",
        "gitStatus",
        "subfoldersChanged",
        "subfolders",
        "filesToRemoveChanged",
        "filesToRemove",
        "readmeChanged",
        "readme",
        "pathChanged",
        "path",
        "setName",
        "update",
        "scanBranches",
        "scanSubfolders",
        "setBranches",
        "setCurrentBranch",
        "setRemote",
        "setRemotes",
        "fetchRemote",
        "setProgress",
        "setTotal",
        "setStatus",
        "setGitStatus",
        "updateGitStatus",
        "uninstall",
        "setSubfolders",
        "removeSubfolders",
        "unpackSubfolders",
        "setFilesToRemove",
        "confirmFileRemove",
        "confirmed",
        "reset",
        "reclone",
        "loadReadme",
        "setReadme",
        "updateIfBehind",
        "QFuture<void>",
        "getUrl",
        "QUrl",
        "Error",
        "Ready",
        "Busy",
        "GitStatusFlag",
        "UpToDate",
        "Ahead",
        "Behind",
        "Diverged",
        "FastForward",
        "Merge",
        "Conflict",
        "MergeStatusMask"
    };

    QtMocHelpers::UintData qt_methods {
        // Signal 'nameChanged'
        QtMocHelpers::SignalData<void(QString)>(1, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::QString, 3 },
        }}),
        // Signal 'branchesChanged'
        QtMocHelpers::SignalData<void(QStringList)>(4, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::QStringList, 5 },
        }}),
        // Signal 'currentBranchChanged'
        QtMocHelpers::SignalData<void(QString)>(6, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::QString, 7 },
        }}),
        // Signal 'remoteChanged'
        QtMocHelpers::SignalData<void(QString)>(8, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::QString, 9 },
        }}),
        // Signal 'remotesChanged'
        QtMocHelpers::SignalData<void(QStringList)>(10, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::QStringList, 11 },
        }}),
        // Signal 'progressChanged'
        QtMocHelpers::SignalData<void(int)>(12, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::Int, 13 },
        }}),
        // Signal 'totalChanged'
        QtMocHelpers::SignalData<void(int)>(14, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::Int, 15 },
        }}),
        // Signal 'statusChanged'
        QtMocHelpers::SignalData<void(enum Status)>(16, 2, QMC::AccessPublic, QMetaType::Void, {{
            { 0x80000000 | 17, 18 },
        }}),
        // Signal 'gitStatusChanged'
        QtMocHelpers::SignalData<void(GitStatus)>(19, 2, QMC::AccessPublic, QMetaType::Void, {{
            { 0x80000000 | 20, 21 },
        }}),
        // Signal 'subfoldersChanged'
        QtMocHelpers::SignalData<void(QStringList)>(22, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::QStringList, 23 },
        }}),
        // Signal 'filesToRemoveChanged'
        QtMocHelpers::SignalData<void(QString)>(24, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::QString, 25 },
        }}),
        // Signal 'readmeChanged'
        QtMocHelpers::SignalData<void(QString)>(26, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::QString, 27 },
        }}),
        // Signal 'pathChanged'
        QtMocHelpers::SignalData<void(QString)>(28, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::QString, 29 },
        }}),
        // Slot 'setName'
        QtMocHelpers::SlotData<void(QString)>(30, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::QString, 3 },
        }}),
        // Slot 'update'
        QtMocHelpers::SlotData<void()>(31, 2, QMC::AccessPublic, QMetaType::Void),
        // Slot 'scanBranches'
        QtMocHelpers::SlotData<void()>(32, 2, QMC::AccessPublic, QMetaType::Void),
        // Slot 'scanSubfolders'
        QtMocHelpers::SlotData<void()>(33, 2, QMC::AccessPublic, QMetaType::Void),
        // Slot 'setBranches'
        QtMocHelpers::SlotData<void(QStringList)>(34, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::QStringList, 5 },
        }}),
        // Slot 'setCurrentBranch'
        QtMocHelpers::SlotData<void(QString)>(35, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::QString, 7 },
        }}),
        // Slot 'setRemote'
        QtMocHelpers::SlotData<void(QString)>(36, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::QString, 9 },
        }}),
        // Slot 'setRemotes'
        QtMocHelpers::SlotData<void(QStringList)>(37, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::QStringList, 11 },
        }}),
        // Slot 'fetchRemote'
        QtMocHelpers::SlotData<void(QString)>(38, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::QString, 9 },
        }}),
        // Slot 'setProgress'
        QtMocHelpers::SlotData<void(int)>(39, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::Int, 13 },
        }}),
        // Slot 'setTotal'
        QtMocHelpers::SlotData<void(int)>(40, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::Int, 15 },
        }}),
        // Slot 'setStatus'
        QtMocHelpers::SlotData<void(enum Status)>(41, 2, QMC::AccessPublic, QMetaType::Void, {{
            { 0x80000000 | 17, 18 },
        }}),
        // Slot 'setGitStatus'
        QtMocHelpers::SlotData<void(GitStatus)>(42, 2, QMC::AccessPublic, QMetaType::Void, {{
            { 0x80000000 | 20, 21 },
        }}),
        // Slot 'updateGitStatus'
        QtMocHelpers::SlotData<void()>(43, 2, QMC::AccessPublic, QMetaType::Void),
        // Slot 'uninstall'
        QtMocHelpers::SlotData<void()>(44, 2, QMC::AccessPublic, QMetaType::Void),
        // Slot 'setSubfolders'
        QtMocHelpers::SlotData<void(QStringList)>(45, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::QStringList, 23 },
        }}),
        // Slot 'removeSubfolders'
        QtMocHelpers::SlotData<void()>(46, 2, QMC::AccessPublic, QMetaType::Void),
        // Slot 'unpackSubfolders'
        QtMocHelpers::SlotData<void()>(47, 2, QMC::AccessPublic, QMetaType::Void),
        // Slot 'setFilesToRemove'
        QtMocHelpers::SlotData<void(QString)>(48, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::QString, 25 },
        }}),
        // Slot 'confirmFileRemove'
        QtMocHelpers::SlotData<void(bool)>(49, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::Bool, 50 },
        }}),
        // Slot 'reset'
        QtMocHelpers::SlotData<void()>(51, 2, QMC::AccessPublic, QMetaType::Void),
        // Slot 'reclone'
        QtMocHelpers::SlotData<void()>(52, 2, QMC::AccessPublic, QMetaType::Void),
        // Slot 'loadReadme'
        QtMocHelpers::SlotData<void()>(53, 2, QMC::AccessPublic, QMetaType::Void),
        // Slot 'setReadme'
        QtMocHelpers::SlotData<void(QString)>(54, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::QString, 27 },
        }}),
        // Slot 'updateIfBehind'
        QtMocHelpers::SlotData<QFuture<void>()>(55, 2, QMC::AccessPublic, 0x80000000 | 56),
        // Method 'getUrl'
        QtMocHelpers::MethodData<QUrl(QString) const>(57, 2, QMC::AccessPublic, 0x80000000 | 58, {{
            { QMetaType::QString, 3 },
        }}),
        // Method 'getUrl'
        QtMocHelpers::MethodData<QUrl() const>(57, 2, QMC::AccessPublic | QMC::MethodCloned, 0x80000000 | 58),
    };
    QtMocHelpers::UintData qt_properties {
        // property 'name'
        QtMocHelpers::PropertyData<QString>(3, QMetaType::QString, QMC::DefaultPropertyFlags | QMC::Writable | QMC::StdCppSet, 0),
        // property 'branches'
        QtMocHelpers::PropertyData<QStringList>(5, QMetaType::QStringList, QMC::DefaultPropertyFlags | QMC::Writable | QMC::StdCppSet, 1),
        // property 'currentBranch'
        QtMocHelpers::PropertyData<QString>(7, QMetaType::QString, QMC::DefaultPropertyFlags | QMC::Writable | QMC::StdCppSet, 2),
        // property 'remote'
        QtMocHelpers::PropertyData<QString>(9, QMetaType::QString, QMC::DefaultPropertyFlags | QMC::Writable | QMC::StdCppSet, 3),
        // property 'remotes'
        QtMocHelpers::PropertyData<QStringList>(11, QMetaType::QStringList, QMC::DefaultPropertyFlags | QMC::Writable | QMC::StdCppSet, 4),
        // property 'progress'
        QtMocHelpers::PropertyData<int>(13, QMetaType::Int, QMC::DefaultPropertyFlags | QMC::Writable | QMC::StdCppSet, 5),
        // property 'total'
        QtMocHelpers::PropertyData<int>(15, QMetaType::Int, QMC::DefaultPropertyFlags | QMC::Writable | QMC::StdCppSet, 6),
        // property 'subfolders'
        QtMocHelpers::PropertyData<QStringList>(23, QMetaType::QStringList, QMC::DefaultPropertyFlags | QMC::Writable | QMC::StdCppSet, 9),
        // property 'filesToRemove'
        QtMocHelpers::PropertyData<QString>(25, QMetaType::QString, QMC::DefaultPropertyFlags | QMC::Writable | QMC::StdCppSet, 10),
        // property 'readme'
        QtMocHelpers::PropertyData<QString>(27, QMetaType::QString, QMC::DefaultPropertyFlags | QMC::Writable | QMC::StdCppSet, 11),
        // property 'path'
        QtMocHelpers::PropertyData<QString>(29, QMetaType::QString, QMC::DefaultPropertyFlags),
        // property 'status'
        QtMocHelpers::PropertyData<enum Status>(18, 0x80000000 | 17, QMC::DefaultPropertyFlags | QMC::Writable | QMC::EnumOrFlag | QMC::StdCppSet, 7),
        // property 'gitStatus'
        QtMocHelpers::PropertyData<GitStatus>(21, 0x80000000 | 20, QMC::DefaultPropertyFlags | QMC::Writable | QMC::EnumOrFlag | QMC::StdCppSet, 8),
    };
    QtMocHelpers::UintData qt_enums {
        // enum 'Status'
        QtMocHelpers::EnumData<enum Status>(17, 17, QMC::EnumIsScoped).add({
            {   59, Status::Error },
            {   60, Status::Ready },
            {   61, Status::Busy },
        }),
        // enum 'GitStatusFlag'
        QtMocHelpers::EnumData<enum GitStatusFlag>(62, 62, QMC::EnumFlags{}).add({
            {   59, GitStatusFlag::Error },
            {   63, GitStatusFlag::UpToDate },
            {   64, GitStatusFlag::Ahead },
            {   65, GitStatusFlag::Behind },
            {   66, GitStatusFlag::Diverged },
            {   67, GitStatusFlag::FastForward },
            {   68, GitStatusFlag::Merge },
            {   69, GitStatusFlag::Conflict },
            {   70, GitStatusFlag::MergeStatusMask },
        }),
        // flag 'GitStatus'
        QtMocHelpers::EnumData<GitStatus>(20, 62, QMC::EnumIsFlag).add({
            {   59, GitStatusFlag::Error },
            {   63, GitStatusFlag::UpToDate },
            {   64, GitStatusFlag::Ahead },
            {   65, GitStatusFlag::Behind },
            {   66, GitStatusFlag::Diverged },
            {   67, GitStatusFlag::FastForward },
            {   68, GitStatusFlag::Merge },
            {   69, GitStatusFlag::Conflict },
            {   70, GitStatusFlag::MergeStatusMask },
        }),
    };
    return QtMocHelpers::metaObjectData<Addon, qt_meta_tag_ZN5AddonE_t>(QMC::MetaObjectFlag{}, qt_stringData,
            qt_methods, qt_properties, qt_enums);
}
Q_CONSTINIT const QMetaObject Addon::staticMetaObject = { {
    QMetaObject::SuperData::link<QObject::staticMetaObject>(),
    qt_staticMetaObjectStaticContent<qt_meta_tag_ZN5AddonE_t>.stringdata,
    qt_staticMetaObjectStaticContent<qt_meta_tag_ZN5AddonE_t>.data,
    qt_static_metacall,
    nullptr,
    qt_staticMetaObjectRelocatingContent<qt_meta_tag_ZN5AddonE_t>.metaTypes,
    nullptr
} };

void Addon::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    auto *_t = static_cast<Addon *>(_o);
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: _t->nameChanged((*reinterpret_cast<std::add_pointer_t<QString>>(_a[1]))); break;
        case 1: _t->branchesChanged((*reinterpret_cast<std::add_pointer_t<QStringList>>(_a[1]))); break;
        case 2: _t->currentBranchChanged((*reinterpret_cast<std::add_pointer_t<QString>>(_a[1]))); break;
        case 3: _t->remoteChanged((*reinterpret_cast<std::add_pointer_t<QString>>(_a[1]))); break;
        case 4: _t->remotesChanged((*reinterpret_cast<std::add_pointer_t<QStringList>>(_a[1]))); break;
        case 5: _t->progressChanged((*reinterpret_cast<std::add_pointer_t<int>>(_a[1]))); break;
        case 6: _t->totalChanged((*reinterpret_cast<std::add_pointer_t<int>>(_a[1]))); break;
        case 7: _t->statusChanged((*reinterpret_cast<std::add_pointer_t<enum Status>>(_a[1]))); break;
        case 8: _t->gitStatusChanged((*reinterpret_cast<std::add_pointer_t<GitStatus>>(_a[1]))); break;
        case 9: _t->subfoldersChanged((*reinterpret_cast<std::add_pointer_t<QStringList>>(_a[1]))); break;
        case 10: _t->filesToRemoveChanged((*reinterpret_cast<std::add_pointer_t<QString>>(_a[1]))); break;
        case 11: _t->readmeChanged((*reinterpret_cast<std::add_pointer_t<QString>>(_a[1]))); break;
        case 12: _t->pathChanged((*reinterpret_cast<std::add_pointer_t<QString>>(_a[1]))); break;
        case 13: _t->setName((*reinterpret_cast<std::add_pointer_t<QString>>(_a[1]))); break;
        case 14: _t->update(); break;
        case 15: _t->scanBranches(); break;
        case 16: _t->scanSubfolders(); break;
        case 17: _t->setBranches((*reinterpret_cast<std::add_pointer_t<QStringList>>(_a[1]))); break;
        case 18: _t->setCurrentBranch((*reinterpret_cast<std::add_pointer_t<QString>>(_a[1]))); break;
        case 19: _t->setRemote((*reinterpret_cast<std::add_pointer_t<QString>>(_a[1]))); break;
        case 20: _t->setRemotes((*reinterpret_cast<std::add_pointer_t<QStringList>>(_a[1]))); break;
        case 21: _t->fetchRemote((*reinterpret_cast<std::add_pointer_t<QString>>(_a[1]))); break;
        case 22: _t->setProgress((*reinterpret_cast<std::add_pointer_t<int>>(_a[1]))); break;
        case 23: _t->setTotal((*reinterpret_cast<std::add_pointer_t<int>>(_a[1]))); break;
        case 24: _t->setStatus((*reinterpret_cast<std::add_pointer_t<enum Status>>(_a[1]))); break;
        case 25: _t->setGitStatus((*reinterpret_cast<std::add_pointer_t<GitStatus>>(_a[1]))); break;
        case 26: _t->updateGitStatus(); break;
        case 27: _t->uninstall(); break;
        case 28: _t->setSubfolders((*reinterpret_cast<std::add_pointer_t<QStringList>>(_a[1]))); break;
        case 29: _t->removeSubfolders(); break;
        case 30: _t->unpackSubfolders(); break;
        case 31: _t->setFilesToRemove((*reinterpret_cast<std::add_pointer_t<QString>>(_a[1]))); break;
        case 32: _t->confirmFileRemove((*reinterpret_cast<std::add_pointer_t<bool>>(_a[1]))); break;
        case 33: _t->reset(); break;
        case 34: _t->reclone(); break;
        case 35: _t->loadReadme(); break;
        case 36: _t->setReadme((*reinterpret_cast<std::add_pointer_t<QString>>(_a[1]))); break;
        case 37: { QFuture<void> _r = _t->updateIfBehind();
            if (_a[0]) *reinterpret_cast<QFuture<void>*>(_a[0]) = std::move(_r); }  break;
        case 38: { QUrl _r = _t->getUrl((*reinterpret_cast<std::add_pointer_t<QString>>(_a[1])));
            if (_a[0]) *reinterpret_cast<QUrl*>(_a[0]) = std::move(_r); }  break;
        case 39: { QUrl _r = _t->getUrl();
            if (_a[0]) *reinterpret_cast<QUrl*>(_a[0]) = std::move(_r); }  break;
        default: ;
        }
    }
    if (_c == QMetaObject::IndexOfMethod) {
        if (QtMocHelpers::indexOfMethod<void (Addon::*)(QString )>(_a, &Addon::nameChanged, 0))
            return;
        if (QtMocHelpers::indexOfMethod<void (Addon::*)(QStringList )>(_a, &Addon::branchesChanged, 1))
            return;
        if (QtMocHelpers::indexOfMethod<void (Addon::*)(QString )>(_a, &Addon::currentBranchChanged, 2))
            return;
        if (QtMocHelpers::indexOfMethod<void (Addon::*)(QString )>(_a, &Addon::remoteChanged, 3))
            return;
        if (QtMocHelpers::indexOfMethod<void (Addon::*)(QStringList )>(_a, &Addon::remotesChanged, 4))
            return;
        if (QtMocHelpers::indexOfMethod<void (Addon::*)(int )>(_a, &Addon::progressChanged, 5))
            return;
        if (QtMocHelpers::indexOfMethod<void (Addon::*)(int )>(_a, &Addon::totalChanged, 6))
            return;
        if (QtMocHelpers::indexOfMethod<void (Addon::*)(Status )>(_a, &Addon::statusChanged, 7))
            return;
        if (QtMocHelpers::indexOfMethod<void (Addon::*)(GitStatus )>(_a, &Addon::gitStatusChanged, 8))
            return;
        if (QtMocHelpers::indexOfMethod<void (Addon::*)(QStringList )>(_a, &Addon::subfoldersChanged, 9))
            return;
        if (QtMocHelpers::indexOfMethod<void (Addon::*)(QString )>(_a, &Addon::filesToRemoveChanged, 10))
            return;
        if (QtMocHelpers::indexOfMethod<void (Addon::*)(QString )>(_a, &Addon::readmeChanged, 11))
            return;
        if (QtMocHelpers::indexOfMethod<void (Addon::*)(QString )>(_a, &Addon::pathChanged, 12))
            return;
    }
    if (_c == QMetaObject::ReadProperty) {
        void *_v = _a[0];
        switch (_id) {
        case 0: *reinterpret_cast<QString*>(_v) = _t->name(); break;
        case 1: *reinterpret_cast<QStringList*>(_v) = _t->branches(); break;
        case 2: *reinterpret_cast<QString*>(_v) = _t->currentBranch(); break;
        case 3: *reinterpret_cast<QString*>(_v) = _t->remote(); break;
        case 4: *reinterpret_cast<QStringList*>(_v) = _t->remotes(); break;
        case 5: *reinterpret_cast<int*>(_v) = _t->progress(); break;
        case 6: *reinterpret_cast<int*>(_v) = _t->total(); break;
        case 7: *reinterpret_cast<QStringList*>(_v) = _t->subfolders(); break;
        case 8: *reinterpret_cast<QString*>(_v) = _t->filesToRemove(); break;
        case 9: *reinterpret_cast<QString*>(_v) = _t->readme(); break;
        case 10: *reinterpret_cast<QString*>(_v) = _t->path(); break;
        case 11: *reinterpret_cast<enum Status*>(_v) = _t->status(); break;
        case 12: QtMocHelpers::assignFlags<GitStatus>(_v, _t->gitStatus()); break;
        default: break;
        }
    }
    if (_c == QMetaObject::WriteProperty) {
        void *_v = _a[0];
        switch (_id) {
        case 0: _t->setName(*reinterpret_cast<QString*>(_v)); break;
        case 1: _t->setBranches(*reinterpret_cast<QStringList*>(_v)); break;
        case 2: _t->setCurrentBranch(*reinterpret_cast<QString*>(_v)); break;
        case 3: _t->setRemote(*reinterpret_cast<QString*>(_v)); break;
        case 4: _t->setRemotes(*reinterpret_cast<QStringList*>(_v)); break;
        case 5: _t->setProgress(*reinterpret_cast<int*>(_v)); break;
        case 6: _t->setTotal(*reinterpret_cast<int*>(_v)); break;
        case 7: _t->setSubfolders(*reinterpret_cast<QStringList*>(_v)); break;
        case 8: _t->setFilesToRemove(*reinterpret_cast<QString*>(_v)); break;
        case 9: _t->setReadme(*reinterpret_cast<QString*>(_v)); break;
        case 11: _t->setStatus(*reinterpret_cast<enum Status*>(_v)); break;
        case 12: _t->setGitStatus(*reinterpret_cast<GitStatus*>(_v)); break;
        default: break;
        }
    }
}

const QMetaObject *Addon::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *Addon::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_staticMetaObjectStaticContent<qt_meta_tag_ZN5AddonE_t>.strings))
        return static_cast<void*>(this);
    return QObject::qt_metacast(_clname);
}

int Addon::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 40)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 40;
    }
    if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 40)
            *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType();
        _id -= 40;
    }
    if (_c == QMetaObject::ReadProperty || _c == QMetaObject::WriteProperty
            || _c == QMetaObject::ResetProperty || _c == QMetaObject::BindableProperty
            || _c == QMetaObject::RegisterPropertyMetaType) {
        qt_static_metacall(this, _c, _id, _a);
        _id -= 13;
    }
    return _id;
}

// SIGNAL 0
void Addon::nameChanged(QString _t1)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 0, nullptr, _t1);
}

// SIGNAL 1
void Addon::branchesChanged(QStringList _t1)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 1, nullptr, _t1);
}

// SIGNAL 2
void Addon::currentBranchChanged(QString _t1)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 2, nullptr, _t1);
}

// SIGNAL 3
void Addon::remoteChanged(QString _t1)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 3, nullptr, _t1);
}

// SIGNAL 4
void Addon::remotesChanged(QStringList _t1)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 4, nullptr, _t1);
}

// SIGNAL 5
void Addon::progressChanged(int _t1)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 5, nullptr, _t1);
}

// SIGNAL 6
void Addon::totalChanged(int _t1)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 6, nullptr, _t1);
}

// SIGNAL 7
void Addon::statusChanged(Status _t1)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 7, nullptr, _t1);
}

// SIGNAL 8
void Addon::gitStatusChanged(GitStatus _t1)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 8, nullptr, _t1);
}

// SIGNAL 9
void Addon::subfoldersChanged(QStringList _t1)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 9, nullptr, _t1);
}

// SIGNAL 10
void Addon::filesToRemoveChanged(QString _t1)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 10, nullptr, _t1);
}

// SIGNAL 11
void Addon::readmeChanged(QString _t1)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 11, nullptr, _t1);
}

// SIGNAL 12
void Addon::pathChanged(QString _t1)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 12, nullptr, _t1);
}
QT_WARNING_POP
